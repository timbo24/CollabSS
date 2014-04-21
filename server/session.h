
#ifndef SESSION_H
#define SESSION_H

#include "editor.h"
#include "server.h"
#include "Circles/CircleChecker.h"

class participant;
class server;

typedef std::deque<std::string> message_queue;
typedef boost::shared_ptr<participant> participant_ptr;

/* class represents an editing session in a spreadsheet
 * containts a list of participants who are able to 
 * make changes to a spreadsheet
 * */
class spreadsheet_session
{
	public:
		spreadsheet_session(std::string name, server* server);
		void join(participant_ptr prt);
		void leave(participant_ptr prt);
		void deliver(const std::string& msg);
		int get_version();
		void increment_version();
		std::string get_name();
		bool circular_check(std::string cell, std::string contents);
		std::string undo();
		void register_old(std::string cell);
		bool undo_empty();
	private:
		std::set<participant_ptr> participants_;

		server* server_;
		enum { max_recent_msgs = 100 };
		message_queue recent_msgs_;
		int version_;
		std::string name_;
		CircleChecker checker_;
		std::stack<std::string> undo_stack_;
};


#endif
