
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
	std::cout<<"WE GOT HERE 8"<<std::endl;
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
	std::cout<<"WE GET HERE 2"<<std::endl;
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

	trim(message);

	size_t pos = 0;
	std::string delimiter = "\e";

	pos = message.find(delimiter);
	std::string token = message.substr(0, pos);

	message.erase(0, pos + delimiter.length());

	std::string outm = "";

	//resquest a login by providing a password
	if(token == "PASSWORD")
	{
		if (message == PASSWORD)
		{
			outm = "FILELIST\e";

			std::set<spreadsheet_session_ptr> sessions = server_->get_spreadsheets();

			std::cout<<"List of spreadsheets: "<<std::endl;
			
			for (auto i = sessions.begin(); i != sessions.end(); ++i)
			{
				outm += (*i)->get_name() + "\e";
				std::cout<<(*i)->get_name()<<std::endl;
			}

			outm += "\r\n"
			
			std::cout<<"outgoing: " << outm << std::endl;
			deliver(outm);
		}
		else
		{
			outm = "INVALID\r\n";
			std::cout<<"outgoing: " << outm << std::endl;
			deliver(outm);
		}
	}
	//Open SS request, if one does not exist, error is sent back
	else if(token == "OPEN")
	{
		outm = "OPENNEW " + message + "\r\n";
		std::cout<<"outgoing: " << outm << std::endl;

		//assign the session to the editor so he can commuticate his changes to other users

		deliver(outm);
	}
	//client asks to create a new SS, if if the name doesn't exist
	//create a new SS, if it does user gets an error back
	else if(token == "CREATE")
	{
		outm = "OPENNEW something\r\n";
		std::cout<<"outgoing: " << outm << std::endl;
		deliver(outm);
	}
	//
	else if(token == "ENTER")
	{
		outm = "OKENTER " + message + "\r\n";
		std::cout<<"outgoing: " << outm << std::endl;
		deliver(outm);
	}





}
