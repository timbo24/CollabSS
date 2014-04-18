


#ifndef EDITOR_H
#define EDITOR_H

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

#include "session.h"

using boost::asio::ip::tcp;

class spreadsheet_session;

typedef std::deque<std::string> message_queue;

class participant
{
	public:
		virtual ~participant() {}
          	virtual void deliver(const std::string& msg) = 0;
};

typedef boost::shared_ptr<participant> participant_ptr;

/* class represents a client who is connected to the server and 
 * editing a spreadsheet, this allows for reading from the client
 * and writing to the client asynchronously
 * */
class spreadsheet_editor
	: public participant,
	  public boost::enable_shared_from_this<spreadsheet_editor>
{
	public:
		spreadsheet_editor(boost::asio::io_service& io_service, spreadsheet_session& session);
		tcp::socket& socket();
		void start();
		void deliver(const std::string& msg);
		void handle_read(const boost::system::error_code& error);
		void handle_write(const boost::system::error_code& error);
		void incoming_message(std::string message);
	private:
		tcp::socket socket_;
		spreadsheet_session& session_;
		boost::asio::streambuf input_buffer_;
		std::string read_msg_;
		message_queue write_msgs_;
		std::mutex mtx;
};

typedef boost::shared_ptr<spreadsheet_editor> spreadsheet_editor_ptr;

#endif
