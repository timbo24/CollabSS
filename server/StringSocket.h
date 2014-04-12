/*
 * StringSocket class, implemented in c++
 * used to send string over a tcp connection
 * asyncronously in order to handle multiple messages
 * */

#ifndef STRINGSOCKET_H
#define STRINGSOCKET_H

#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class StringSocket
	: public boost::enable_shared_from_this<StringSocket>
{
	public:
		StringSocket(boost::asio::io_service& io_service,
      		             tcp::resolver::iterator endpoint_iterator);
		void BeginSend(std::string s,  
			       void (*SendCallBack)(void *payload), 
			       void *payload); 

	private:
		void HandleConnect();
          	void ProcessSendQueue();
		void BytesSent(const boost::system::error_code& ec, std::size_t bytes_transferred);
		

		std::mutex mutex_;
		tcp::socket socket_;

//		void (*SendCallBack)(const boost::system::error_code &ec, void *payload);


		/*struct SendRequest
		{
		*/
};



#endif





