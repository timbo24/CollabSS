
#include <boost/lexical_cast.hpp>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <functional> 
#include <cctype>
#include <locale>
#include <mutex>

#define PASSWORD "Malloc"

#include "session.h"
#include "editor.h"

// trim from start
 static inline std::string &ltrim(std::string &s) {
         s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	 return s;
 }

 // trim from end
 static inline std::string &rtrim(std::string &s) {
	 s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	 return s;
 }

 // trim from both ends
 static inline std::string &trim(std::string &s) {
	 return ltrim(rtrim(s));
 }
//----------------------------------------------------------------------

/* class represents a client who is connected to the server and 
 * editing a spreadsheet, this allows for reading from the client
 * and writing to the client asynchronously
 * */
spreadsheet_editor::spreadsheet_editor(boost::asio::io_service& io_service, server* server)
	: socket_(io_service),
	  server_(server)
{
}

/* returns member socket
 * */
tcp::socket& spreadsheet_editor::socket()
{
	return socket_;
}

/* adds this editor to the spreadsheet session
 * begins receiving data, tying it to the callback
 * */	
void spreadsheet_editor::start()
{
	//session_.join(shared_from_this());
	boost::asio::async_read_until(socket_, 
				      input_buffer_, 
				      '\n', 
				      boost::bind(&spreadsheet_editor::handle_read, 
						  shared_from_this(), 
						  boost::asio::placeholders::error));
}

/* called to write to a client
 * */
void spreadsheet_editor::deliver(const std::string& msg)
{
	bool write_in_progress = !write_msgs_.empty();
	write_msgs_.push_back(msg);
	if (!write_in_progress)
	{
		boost::asio::async_write(socket_,
					 boost::asio::buffer(write_msgs_.front(),
					 write_msgs_.front().length()),
					 boost::bind(&spreadsheet_editor::handle_write, 
						     shared_from_this(),
						     boost::asio::placeholders::error));
	}
}

/* callback for strings read in from clients
 * if there is an error, the client is removed from
 * the session
 * */
void spreadsheet_editor::handle_read(const boost::system::error_code& error)
{


	if (!error)
	{
		mtx.lock();
		std::istream is(&input_buffer_);
		std::getline(is, read_msg_);


		incoming_message(read_msg_);

		mtx.unlock();

		boost::asio::async_read_until(socket_, input_buffer_, '\n', boost::bind(&spreadsheet_editor::handle_read, shared_from_this(), boost::asio::placeholders::error));
	}
	else
	{

		std::cout << error.message() << std::endl;
		//session_.leave(shared_from_this());
	}
}


/* callback function after a write to a client, pops messages from the message_queue
 * and calls asyncwrite again if there are more messages
 * */
void spreadsheet_editor::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		write_msgs_.pop_front();
		if (!write_msgs_.empty())
		{
			boost::asio::async_write(socket_,
			    boost::asio::buffer(write_msgs_.front(),
			      write_msgs_.front().length()),
			    boost::bind(&spreadsheet_editor::handle_write, shared_from_this(),
			      boost::asio::placeholders::error));
		}
	}
	else
	{
		//session_.leave(shared_from_this());
	}
}


void spreadsheet_editor::incoming_message(std::string message)
{
	//trim the endline from the string
	std::cout<<"incoming: " << message<< std::endl;

	size_t pos = 0;
	char delimiter = static_cast<char>(27);

	pos = message.find(delimiter);
	std::string token = message.substr(0, pos);

	message.erase(0, pos + 1);

	std::string outm = "";

	//resquest a login by providing a password
	if(token == "PASSWORD")
	{
		std::cout<<"\n\n1\n\n"<<std::endl;
		if (message == PASSWORD)
		{
			outm = "FILELIST";

			//pull the full spreadsheey map from the server
			std::map<std::string, spreadsheet_session_ptr> sessions = server_->get_spreadsheets();

			//populate message with the names of all aailable spreadsheets
			for (auto i = sessions.begin(); i != sessions.end(); ++i)
			{
				outm += static_cast<char>(27) + i->first;
			}

			outm += "\n";
			
			deliver(outm);
		}
		else
		{
			outm = "INVALID\n";
			deliver(outm);
		}
	}
	//Open SS request, if one does not exist, error is sent back
	else if(token == "OPEN")
	{
		std::cout<<"\n\n2\n\n"<<std::endl;

		if (server_->spreadsheet_exists(message))
		{
			outm = "UPDATE" + static_cast<char>(27);

			//set the session for this editor
			session_ = server_->get_spreadsheet(message);
			session_->join(shared_from_this());

			//load the current spreadsheet
			outm += server_->load(message) + "\n";
		}
		else
		{
			outm = "ERROR\n";
		}


		deliver(outm);
	}
	//client asks to create a new SS, if if the name doesn't exist
	//create a new SS, if it does user gets an error back
	else if(token == "CREATE")
	{
		if (server_->spreadsheet_exists(message))
		{
			std::cout<<"\n\n3\n\n"<<std::endl;
			outm = "ERROR\n";
		}
		else
		{
			std::cout<<"\n\n8\n\n"<<std::endl;
			outm = "UPDATE";
			outm += static_cast<char>(27);

			//add the spreadsheet and set is the member spreadsheet
			session_ = server_->add_spreadsheet(message);

			session_->join(shared_from_this());

			outm += boost::lexical_cast<std::string>(session_->get_version()) + "\n";
		}


		deliver(outm);
	}
	else if(token == "ENTER")
	{
		std::cout<<"\n\n4\n\n"<<std::endl;

		pos = 0;
		pos = message.find(delimiter);
		message.erase(0, pos + 1);

		pos = message.find(delimiter);
		std::string cell = message.substr(0, pos);
		message.erase(0, pos + 1);

		//Circular dependency check
		if (session_->circular_check(cell, message))
		{
			outm = "UPDATE";
		        outm += static_cast<char>(27);
			outm += boost::lexical_cast<std::string>(session_->get_version());
			outm += static_cast<char>(27);
			outm += cell;
			outm += static_cast<char>(27);
			outm += message + "\n";

			server_->update(session_->get_name(), cell, message);

			session_->deliver(outm);
		}
		else 
		{
			outm = "ERROR";
			outm += static_cast<char>(27); 
			outm += "circular dependency\n";

			deliver(outm);
		}
	}
	else if(token == "RESYNC")
	{
		//TODO resync
	}

	else
	{
		std::cout<<"**\n**\nMESSAGE READ ERROR\n**\n**"<<std::endl;
	}





}
