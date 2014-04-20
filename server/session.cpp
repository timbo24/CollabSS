
#include "Circles/CircleChecker.h"
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

//----------SPREADSHEET_SESSION-----------------------------------------
/* class represents an editing session in a spreadsheet
 * containts a list of participants who are able to 
 * make changes to a spreadsheet
 * */
spreadsheet_session::spreadsheet_session(std::string name)
	: version_(0),
	  name_(name),
	  checker_()
{
}

/* get current version for a spreadseet
 * */
int spreadsheet_session::get_version()
{
	return version_;
}

void spreadsheet_session::increment_version()
{
	version_++;
}

std::string spreadsheet_session::get_name()
{
	return name_;
}


/* add a participlant to a session
 * */
void spreadsheet_session::join(participant_ptr prt)
{
	participants_.insert(prt);
	/*std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
		boost::bind(&participant::deliver, prt, _1));
		*/
}

/* remove a participant from the session
 * */
void spreadsheet_session::leave(participant_ptr ptr)
{
	participants_.erase(ptr);
}

bool spreadsheet_session::circular_check(std::string cell, std::string contents)
{
	return checker_.NotCircular(cell, contents);
}

/* delivers messages to clients in the session
 * */
void spreadsheet_session::deliver(const std::string& msg)
{
	recent_msgs_.push_back(msg);
	while (recent_msgs_.size() > max_recent_msgs)
		recent_msgs_.pop_front();

	std::for_each(participants_.begin(), participants_.end(),
		boost::bind(&participant::deliver, _1, boost::ref(msg)));
}
