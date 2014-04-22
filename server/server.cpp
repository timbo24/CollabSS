/*Online Spreadsheet Server for team Malloc written by 
 *Tim Knutson 
 *Modeled after http://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio/example/chat/chat_server.cpp
 */

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
#include <boost/thread.hpp>
#include <functional> 
#include <cctype>
#include <locale>
#include <mutex>
#include <boost/lexical_cast.hpp>
#include "editor.h"
#include "session.h"
#include "server.h"

#include <mysql/mysql.h>
#define SERVER "atr.eng.utah.edu"
#define USER "cs4540_tpayne"
#define PASSWORD "502365727"
#define DATABASE "cs4540_tpayne"

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
	  //	std::cout << "DB Connection Succeeded\n";
	}
	else
	{
	  //	std::cout << "DB Connection Failed\n";
	}

	begin_accept();
}


/* method populates sessions_ with all available spreadsheets from the DB
 */
void server::populate_sessions()
{
	MYSQL_RES *res_set;
	MYSQL_ROW row;

	// Now we will do a SELECT statement
	std::string sql_query  = "SELECT * FROM Spreadsheet";

	if ( mysql_query (connection_, sql_query.c_str()) )
	{
		mysql_close (connection_);
		//	std::cout << "Database connection closed." << std::endl;
	}

	// Get the result of the query
	res_set = mysql_store_result(connection_);
	int numrows = mysql_num_rows(res_set);

	while (((row = mysql_fetch_row(res_set)) != NULL))
	{
		spreadsheet_session_ptr new_session(new spreadsheet_session(row[0], this));

		sessions_.insert( std::pair<std::string, spreadsheet_session_ptr>(row[0],new_session));
	}
}



/* callback creates a new editor object, 
 * as well as begins accepting connections from clients
 * */
void server::begin_accept()
{
	spreadsheet_editor_ptr new_editor(new spreadsheet_editor(io_service_, this));

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
void server::handle_accept(spreadsheet_editor_ptr editor,
		   const boost::system::error_code& error)
{
	if (!error)
	{
		editor->start();
	}

	begin_accept();
}

/* returns the spreadsheet map
 * */
std::map<std::string,spreadsheet_session_ptr> server::get_spreadsheets()
{
	return sessions_;
}


/* returns true if the spreadsheet already exists
 * in sessions_ and false otherwise
 * */
bool server::spreadsheet_exists(std::string name)
{
	if(sessions_.find(name) != sessions_.end())
		return true;
	return false;
}

/* return the pointer to a spreadsheet with the given
 * name
 * */
spreadsheet_session* server::get_spreadsheet(std::string name)
{
	sessions_[name].get();
}


/* called in order to load an existing spreadsheet
 * and create an update commant for a client
 * */
std::string server::load(std::string name)
{
	std::string load_msg = "";

	//first append the version #
	std::string version = boost::lexical_cast<std::string>(sessions_[name]->get_version());
	load_msg += version;

	MYSQL_RES *res_set;
	MYSQL_ROW row;

	//select statement to get all cell values for a given spreadsheet
	std::string sql_query  = "SELECT cell, contents FROM Cell WHERE ssname = \"" + name+ "\"";
	if ( mysql_query (connection_, sql_query.c_str()) )
	{
	  //	std::cout << mysql_error(connection_) << std::endl;
		mysql_close (connection_);
		//	std::cout << "Database connection closed." << std::endl;
	}

	res_set = mysql_store_result(connection_);
	int numrows = mysql_num_rows(res_set);

	while (((row = mysql_fetch_row(res_set)) != NULL))
	{
		sessions_[name]->circular_check(row[0], row[1]);
		std::string col1(row[0]);
		std::string col2(row[1]);
		load_msg += static_cast<char>(27);
	        load_msg += col1;
		load_msg += static_cast<char>(27);
		load_msg += col2;
	}

	//we don't want the last \\e so return the length -2
	return load_msg;
}


/* create and add a new spreadsheet to sessions_
 * and return the spreadsheet_session
 * */
spreadsheet_session* server::add_spreadsheet(std::string name)
{
	//Insert the new name into the Data Base
	std::string sql_update  = "INSERT into Spreadsheet (ssname) VALUES (\"" + name + "\")";

	if ( mysql_query (connection_, sql_update.c_str()) )
	{
	  //	std::cout << mysql_error(connection_) << std::endl;
		mysql_close (connection_);
		//	std::cout << "Database connection closed." << std::endl;
	}

	//add this sessions to the list
	spreadsheet_session_ptr new_session(new spreadsheet_session(name, this));

	sessions_.insert( std::pair<std::string, spreadsheet_session_ptr>(name, new_session));

	return new_session.get();
}


/* handles updating the DB with a new value, aswell as incrementing the
 * session version to reflect the update
 * */
void server::update(std::string ssname, std::string cell, std::string contents)
{
	std::string sql_update = "";
	if (trim(contents) == "")
	{
		sql_update  = "DELETE from Cell WHERE ssname = \"" + ssname + "\"" +
		                                                         " and cell = \"" + cell + "\"";
	}
	else
	{
		//Escape both " and \ characters so they don't interfere with MySQL query
		size_t pos = 0;
		while (pos < contents.length() && contents.find('\\', pos) != string::npos)
		{
			pos = contents.find('\\', pos);
			contents.insert(pos, "\\");
			pos += 2;
		}

		pos = 0;
		while (pos < contents.length() && contents.find('"', pos) != string::npos)
		{
			pos = contents.find('"', pos);
			contents.insert(pos, "\\");
			pos += 2;
		}

		//update the database with new cell contents
		sql_update  = "INSERT into Cell (ssname, cell, contents) VALUES ( \"" + ssname +   "\"," + 
											     "\"" + cell +     "\"," +
											     "\"" + contents + "\")" +
					   "ON DUPLICATE KEY UPDATE contents = \"" + contents + "\"";
	}

	if ( mysql_query (connection_, sql_update.c_str()) )
	{
	  //	std::cout << mysql_error(connection_) << std::endl;
		mysql_close (connection_);
		//	std::cout << "Database connection closed." << std::endl;
	}

}

/*
 * used to get the current contents of a spreadsheet cell from the DB
 * in order to save them for an undo
 * */
std::string server::get_old(std::string sheet, std::string cellname)
{
    std::string undo_query = "SELECT contents FROM Cell WHERE ssname = '" + sheet + "' AND cell = '" + cellname + "'";

    //variable to store the old cell contents
    std::string contents = "";

    //if there was an error, do something
    if ( mysql_query (connection_, undo_query.c_str()) )
    {
      //	std::cout << mysql_error(connection_) << std::endl;
		mysql_close (connection_);
		//		std::cout << "Database connection closed." << std::endl;
	    //TODO fill in if query fails or DB disconnect
    }

    MYSQL_RES *cellCon;
    MYSQL_ROW resultRow;

    // Get the result of the query
    cellCon = mysql_store_result(connection_);
    int foundCell = mysql_num_rows(cellCon);

    //if the cell was there, save the contents
    if (foundCell == 1)
    {
      if (((resultRow = mysql_fetch_row(cellCon)) != NULL))
	{
	  contents = resultRow[0];
	}
    }
    else
    {
	    contents += "  ";
    }

    return contents;
}


/* Function to get input from command line
 */
void server::getInput()
{
  std::string input;
  std::cin >> input;

  // Keep looping until we get the command to quit
  while (input != "quit")
    {
        std::cin >> input;
    }

  mysql_close (this->connection_);
  // std::cout << "Database connection closed." << std::endl;
  // std::cout<<"Closing server"<<std::endl;
  io_service_.stop();
	  acceptor_.close();

  // Exits the program
  std::exit(EXIT_SUCCESS);

  return;
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
		  //	std::cout << "Mysql Initialization Failed";
			return 1;
		}

		//io_service helps manage different thread to handle async calls 
		boost::asio::io_service io_service;

		using namespace std;

		//endpoint for establishing a connection for the server, using the command arg as the port#
		tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));

		//start the server
		server_ptr svr(new server(io_service, endpoint, con));

		//	std::cout<<"Server up and running..."<<std::endl;

		//boost::thread* inputThread = new boost::thread(boost::bind(&server::getInput, svr));
		boost::thread inputThread(&server::getInput, svr);

		//being the io_service
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}


