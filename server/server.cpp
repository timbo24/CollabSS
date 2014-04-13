//
// server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

//typedef std::deque<chat_message> chat_message_queue;
typedef std::deque<std::string> message_queue;

//----------------------------------------------------------------------

class participant
{
	public:
		virtual ~participant() {}
          	virtual void deliver(const std::string& msg) = 0;
};

typedef boost::shared_ptr<participant> participant_ptr;

//----------------------------------------------------------------------

class spreadsheet_session
{
	public:
		void join(participant_ptr prt)
		{
			participants_.insert(prt);
			std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
				boost::bind(&participant::deliver, prt, _1));
		}

		void leave(participant_ptr prt)
		{
			participants_.erase(prt);
		}

		void deliver(const std::string& msg)
		{
			recent_msgs_.push_back(msg);
			while (recent_msgs_.size() > max_recent_msgs)
				recent_msgs_.pop_front();

			std::for_each(participants_.begin(), participants_.end(),
				boost::bind(&participant::deliver, _1, boost::ref(msg)));
		}

	private:
		std::set<participant_ptr> participants_;
		enum { max_recent_msgs = 100 };
		message_queue recent_msgs_;
		//chat_message_queue recent_msgs_;
};
//----------------------------------------------------------------------

class spreadsheet_editor
	: public participant,
	  public boost::enable_shared_from_this<spreadsheet_editor>
{
	public:
		/* Constructor for an editor, assigns the member socket and spreadsheet_session
		 * */
		spreadsheet_editor(boost::asio::io_service& io_service, spreadsheet_session& session)
			: socket_(io_service),
			  session_(session),
			  message_buffer_size(1)
		{
			
		}

		/* returns member socket
		 * */
		tcp::socket& socket()
		{
			return socket_;
		}

	        /* adds this editor to the spreadsheet session
		 * begins receiving data, tying it to the callback
		 * */	
		void start()
		{
			std::cout<<"starting a session"<<std::endl;
			session_.join(shared_from_this());
			boost::asio::async_read(socket_,
						boost::asio::buffer(read_msg_, message_buffer_size),
						boost::bind(&spreadsheet_editor::handle_read, 
						        shared_from_this(),
						        boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		}

		/* called to write to a client
		 * */
		void deliver(const std::string& msg)
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
		void handle_read(const boost::system::error_code& error, 
				 std::size_t bytes_transferred)
		{


			if (!error)
			{
				//convert the message from char[] to a string
				std::string temp(read_msg_);

				//detect whether there is a newline character
				std::size_t found = temp.find('\n');

				//if there is a newline, we append the portion ending in a newline to 
				//the final message, and we append the remainder to the partial message
				//we then deliver the final message to all users in the session
				if (found != std::string::npos)
				{
					final_msg_ += partial_msg_ + temp.substr(0,found);
					partial_msg_ = temp.substr(found + 1);

					std::cout<<final_msg_<<std::endl;


					session_.deliver(final_msg_);
					final_msg_ = "";
				}
				else
				{
					partial_msg_ += temp;
				}


				boost::asio::async_read(socket_,
						        boost::asio::buffer(read_msg_, 
								            message_buffer_size),
							boost::bind(&spreadsheet_editor::handle_read, 
								    shared_from_this(), 
								    boost::asio::placeholders::error,
								    boost::asio::placeholders::bytes_transferred));




			}
			else
			{

				std::cout << error.message() << std::endl;
				session_.leave(shared_from_this());
			}
		}

		void handle_write(const boost::system::error_code& error)
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
				session_.leave(shared_from_this());
			}
		}

	private:
		tcp::socket socket_;
		spreadsheet_session& session_;
		char read_msg_[1];
		std::string final_msg_;
		std::string partial_msg_;
		message_queue write_msgs_;
		size_t message_buffer_size;

		/*
		chat_message read_msg_;
		chat_message_queue write_msgs_;
		*/
};

typedef boost::shared_ptr<spreadsheet_editor> spreadsheet_editor_ptr;

//----------------------------------------------------------------------

class server
{
public:

	/* constructor for the server, sets members and 
	 * begins to accept connections from clients
	 * */	
	server(boost::asio::io_service& io_service,
		    const tcp::endpoint& endpoint)
	: io_service_(io_service),
	  acceptor_(io_service, endpoint)
	{
		begin_accept();
	}


	/* creates a new editor object, 
	 * as well as begins accepting connections from clients
	 * */
	void begin_accept()
	{
		spreadsheet_editor_ptr new_editor(new spreadsheet_editor(io_service_, session_));
		acceptor_.async_accept(new_editor->socket(),
		                       boost::bind(&server::handle_accept,
			                           this,
						   new_editor,
						   boost::asio::placeholders::error)); 
	} 
		
	/* The callback method when a connection comes in if there
	 * is no error, the editor will start will call begin_accept again
	 * so that other editors can connect
	 * */
	void handle_accept(spreadsheet_editor_ptr editor,
			   const boost::system::error_code& error)
	{
		if (!error)
		{
			editor->start();
		}

		begin_accept();
	}

private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	spreadsheet_session session_;
};

typedef boost::shared_ptr<server> server_ptr;
typedef std::list<server_ptr> server_list;

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std;

    tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));

    server_ptr svr(new server(io_service, endpoint));

    /*
    server_list servers;
    for (int i = 1; i < argc; ++i)
    {
      using namespace std; // For atoi.
      tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
      server_ptr server(new server(io_service, endpoint));
      servers.push_back(server);
    }
    */
    std::cout<<"Server up and running..."<<std::endl;

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
