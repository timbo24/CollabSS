
#include <iostream>
#include "StringSocket.h"
#include <mutex>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp> 
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


StringSocket::StringSocket(boost::asio::io_service& io_service, 
		           tcp::resolver::iterator endpoint_iterator)
	:socket_(io_service)
{
	boost::asio::async_connect(socket_, 
			           endpoint_iterator,
			           boost::bind(&StringSocket::HandleConnect, this));
}


void StringSocket::HandleConnect()
{
}


void StringSocket::BeginSend(std::string s,  
	       void (*SendCallBack)(void *payload), 
	       void *payload)
{

	mutex_.lock();
	boost::asio::async_write(socket_,
                                 boost::asio::buffer(s, s.length()),
                                 boost::bind(&StringSocket::BytesSent, 
				           this,
				           boost::asio::placeholders::error,
				           boost::asio::placeholders::bytes_transferred));
	mutex_.unlock();
}

void StringSocket::BytesSent(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
}

void StringSocket::ProcessSendQueue()
{

}


