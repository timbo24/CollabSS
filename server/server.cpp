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

#include "editor.h"
#include "session.h"
#include "server.h"

#include <mysql/mysql.h>
#define SERVER "atr.eng.utah.edu"
#define USER "cs4540_tpayne"
#define PASSWORD "502365727"
#define DATABASE "cs4540_tpayne"




using boost::asio::ip::tcp;





/**
 * Example code on how to connect with and use the database
 */

int db_example()
{
    MYSQL *con;
    con = mysql_init(NULL);

    if (!con)
    {
        std::cout << "Mysql Initialization Failed";
        return 1;
    }
    
    // See definitions at beggining of file for values of SERVER, USER, PASSWORD, DATABASE
    con = mysql_real_connect(con, SERVER, USER, PASSWORD, DATABASE, 0,NULL,0);

    if (con)
    {
        std::cout << "DB Connection Succeeded\n";
    }
    else
    {
        std::cout << "DB Connection Failed\n";
    }

    MYSQL_RES *res_set;
    MYSQL_ROW row;

    // Creating an INSERT statement
    std::string ss_name = "ss6"; // ***If this spreadsheet name already exists, the insert will fail***
    std::string sql_insert  = "INSERT INTO Spreadsheet VALUES ('" + ss_name + "')";

    // Returns 0 on success
    if ( mysql_query (con, sql_insert.c_str()) )
      {
	std::cout << mysql_error(con) << std::endl;
	mysql_close (con);
	std::cout << "Database connection closed." << std::endl;
	return 1 ;
      }

    // The number of rows affected
    int rows_inserted = mysql_affected_rows(con);
    std::cout << "Inserted " << rows_inserted << " rows" << std::endl;


    // Now we will do a SELECT statement
    std::string sql_query  = "SELECT * FROM Spreadsheet";
    
    if ( mysql_query (con, sql_query.c_str()) )
      {
	std::cout << mysql_error(con) << std::endl;
	mysql_close (con);
	std::cout << "Database connection closed." << std::endl;
	return 1 ;
      }

    // Get the result of the query
    res_set = mysql_store_result(con);
    int numrows = mysql_num_rows(res_set);

    int i = 0;
    std::cout << "Spreadsheets in the database:" << std::endl;
     while (((row = mysql_fetch_row(res_set)) != NULL))
    {
        std::cout << row[i] << std::endl;
    }

    // Creating an UPDATE statement
    ss_name = "ss1";
    std::string cell_content = "Hello";
    std::string sql_update  = "UPDATE Cell SET contents = '" + cell_content + "' WHERE ssname = '" + ss_name + "' AND cell = 'A2'";

    // Returns 0 on success
    if ( mysql_query (con, sql_update.c_str()) )
      {
	std::cout << mysql_error(con) << std::endl;
	mysql_close (con);
	std::cout << "Database connection closed." << std::endl;
	return 1 ;
      }

    // The number of rows affected
    int rows_updated = mysql_affected_rows(con);
    std::cout << "Inserted " << rows_updated << " rows" << std::endl;

    // Close the connection!
    mysql_close (con);
    std::cout << "Database connection closed." << std::endl;

    return 0;
}

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


//-----------------------SERVER-----------------------------------------

/* constructor for the server, sets members and 
 * begins to accept connections from clients
 * */	
server::server(boost::asio::io_service& io_service,
	       const tcp::endpoint& endpoint,
	       MYSQL *con)
: io_service_(io_service),
  acceptor_(io_service, endpoint)
{
	connection_ = con;
	// See definitions at beggining of file for values of SERVER, USER, PASSWORD, DATABASE
	connection_ = mysql_real_connect(connection_, SERVER, USER, PASSWORD, DATABASE, 0,NULL,0);
	populate_sessions();
	if (connection_)
	{
		std::cout << "DB Connection Succeeded\n";
	}
	else
	{
		std::cout << "DB Connection Failed\n";
	}
	std::cout<<"WE GOT HERE 3"<<std::endl;

	begin_accept();
}


boost::shared_ptr<server> server::get_shared()
{
	return shared_from_this();
}

/* method populates sessions_ with all available spreadsheets from the DB
 */
void server::populate_sessions()
{
	//TODO
	MYSQL_RES *res_set;
	MYSQL_ROW row;

	// Now we will do a SELECT statement
	std::string sql_query  = "SELECT * FROM Spreadsheet";

	if ( mysql_query (connection_, sql_query.c_str()) )
	{
		std::cout << mysql_error(connection_) << std::endl;
		mysql_close (connection_);
		std::cout << "Database connection closed." << std::endl;
	}

	// Get the result of the query
	res_set = mysql_store_result(connection_);
	int numrows = mysql_num_rows(res_set);

	int i = 0;
	std::cout << "Spreadsheets in the database:" << std::endl;
	while (((row = mysql_fetch_row(res_set)) != NULL))
	{
		spreadsheet_session_ptr new_session(new spreadsheet_session(row[i]));

		sessions_.insert(new_session);
	}

	std::cout<<"WE GOT HERE 4"<<std::endl;

}



/* callback creates a new editor object, 
 * as well as begins accepting connections from clients
 * */
void server::begin_accept()
{
	std::cout<<"WE GOT HERE 7"<<std::endl;
	spreadsheet_editor_ptr new_editor(new spreadsheet_editor(io_service_, this));

	
	std::cout<<"WE GOT HERE 6"<<std::endl;
	acceptor_.async_accept(new_editor->socket(),
			       boost::bind(&server::handle_accept,
					   this,
					   new_editor,
					   boost::asio::placeholders::error)); 

	std::cout<<"WE GOT HERE 5"<<std::endl;
} 

	
/* The callback method when a connection comes in if there
 * is no error,  will call begin_accept again
 * so that other editors can connect
 * */
void server::handle_accept(spreadsheet_editor_ptr editor,
		   const boost::system::error_code& error)
{
	if (!error)
	{
		editor->start();
	}

	begin_accept();
}

void server::join_session(std::string session)
{

}

bool server::session_exists(std:: string session)
{
	return false;
}

std::set<spreadsheet_session_ptr> server::get_spreadsheets()
{
	return sessions_;
}

/* adds a session to the set of spreadsheet sessions
 * */
void server::add_session()
{
	//TODO
}




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

		//run the DB code
		MYSQL *con;
		con = mysql_init(NULL);

		if (!con)
		{
			std::cout << "Mysql Initialization Failed";
			return 1;
		}

		//io_service helps manage different thread to handle async calls 
		boost::asio::io_service io_service;

		using namespace std;

		//endpoint for establishing a connection for the server, using the command arg as the port#
		tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));

		//start the server
		server_ptr svr(new server(io_service, endpoint, con));

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


