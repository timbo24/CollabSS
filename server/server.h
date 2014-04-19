
#ifndef SERVER_H
#define SERVER_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "editor.h"
#include <mysql/mysql.h>

using boost::asio::ip::tcp;

class spreadsheet_editor;
class spreadsheet_session;

typedef boost::shared_ptr<spreadsheet_session> spreadsheet_session_ptr;
typedef boost::shared_ptr<spreadsheet_editor> spreadsheet_editor_ptr;

/* server class establishes a connection to a port, and begins accepting
 * connections from clients
 * */
class server
: public boost::enable_shared_from_this<server>
{
public:

	server(boost::asio::io_service& io_service,
		    const tcp::endpoint& endpoint,
		    MYSQL * con);
	boost::shared_ptr<server> get_shared();
	void populate_sessions();
	void begin_accept();
	std::set<spreadsheet_session_ptr> get_spreadsheets();
	void handle_accept(spreadsheet_editor_ptr editor,
			   const boost::system::error_code& error);
	void join_session(std::string session);
	bool session_exists(std:: string session);
	void add_session();
private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	MYSQL *connection_;
	std::set<spreadsheet_session_ptr> sessions_;
};

typedef boost::shared_ptr<server> server_ptr;
typedef std::list<server_ptr> server_list;

#endif
