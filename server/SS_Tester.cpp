
#include <string>
#include "StringSocket.h"
#include <iostream>
#include <mutex>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp> 
#include <boost/asio.hpp>

using namespace std;


void MessageReceivedd(void *payload){}
 


int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: chat_client <host> <port>\n";
			return 1;
		}

		cout<< "Program compiles"<<endl;

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(argv[1], argv[2]);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		StringSocket SS(io_service, iterator);


		std::string message = "test";


		void (*test)(void *payload);
		test = &MessageReceivedd;

		SS.BeginSend(message, 
			      test,
			      NULL);


	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}






