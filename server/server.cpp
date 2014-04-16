
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

using boost::asio::ip::tcp;

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

typedef std::deque<std::string> message_queue;

//----------------------------------------------------------------------

/* virtual class represents a participant in a spreadsheet
 * */
class participant
{
	public:
		virtual ~participant() {}
          	virtual void deliver(const std::string& msg) = 0;
};

typedef boost::shared_ptr<participant> participant_ptr;

//----------------------------------------------------------------------

/* class represents an editing session in a spreadsheet
 * containts a list of participants who are able to 
 * make changes to a spreadsheet
 * */
class spreadsheet_session
{
	public:
		/* add a participlant to a session
		 * */
		void join(participant_ptr prt)
		{
			participants_.insert(prt);
			/*std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
				boost::bind(&participant::deliver, prt, _1));
				*/
		}

		/* remove a participant from the session
		 * */
		void leave(participant_ptr prt)
		{
			participants_.erase(prt);
		}

		/* delivers messages to clients in the session
		 * */
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
};



//----------------------------------------------------------------------

/* class represents a client who is connected to the server and 
 * editing a spreadsheet, this allows for reading from the client
 * and writing to the client asynchronously
 * */
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
			session_.join(shared_from_this());
			boost::asio::async_read(socket_,
						boost::asio::buffer(read_msg_, message_buffer_size),
						boost::bind(&spreadsheet_editor::handle_read, 
						        shared_from_this(),
						        boost::asio::placeholders::error));
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
		void handle_read(const boost::system::error_code& error)
		{


			if (!error)
			{
				//convert the message from char[] to a string
				std::string temp(read_msg_);

				cout<<"TEMP: " << temp << std::endl;

				//detect whether there is a newline character
				std::size_t found = temp.find('\n');

				//if there is a newline, we append the portion ending in a newline to 
				//the final message, and we append the remainder to the partial message
				//we then deliver the final message to all users in the session
				if (found != std::string::npos)
				{
					final_msg_ += partial_msg_ + temp.substr(0,found);
					partial_msg_ = temp.substr(found + 1);

					incoming_message(final_msg_);
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
								    boost::asio::placeholders::error));

			}
			else
			{

				std::cout << error.message() << std::endl;
				session_.leave(shared_from_this());
			}
		}

		/* callback function after a write to a client, pops messages from the message_queue
		 * and calls asyncwrite again if there are more messages
		 * */
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


		void incoming_message(std::string message)
		{
			//trim the endline from the string
			std::cout<<"incoming: " << message<< std::endl;

			trim(message);

			size_t pos = 0;
			std::string delimiter = " ";

			pos = message.find(delimiter);
			std::string token = message.substr(0, pos);

		        message.erase(0, pos + delimiter.length());

			std::string outm = "";

			if(token == "PASSWORD")
			{
				outm = "CONNECTED\r\n";
				std::cout<<"outgoing: " << outm << std::endl;
				deliver(outm);

			}
			else if(token == "CREATE")
			{
				outm = "OPENNEW something\r\n";
				std::cout<<"outgoing: " << outm << std::endl;
				deliver(outm);
			}
			else if(token == "ENTER")
			{
				outm = "OKENTER " + message + "\r\n";
				std::cout<<"outgoing: " << outm << std::endl;
				deliver(outm);;
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
};

typedef boost::shared_ptr<spreadsheet_editor> spreadsheet_editor_ptr;

//----------------------------------------------------------------------

/* server class establishes a connection to a port, and begins accepting
 * connections from clients
 * */
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


	/* callback creates a new editor object, 
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
	 * is no error,  will call begin_accept again
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

		//io_service helps manage different thread to handle async calls 
		boost::asio::io_service io_service;

		using namespace std;

		//endpoint for establishing a connection for the server, using the command arg as the port#
		tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));

		//start the server
		server_ptr svr(new server(io_service, endpoint));

		std::cout<<"Server up and running..."<<std::endl;

		//being the io_service
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}


