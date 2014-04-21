
#include <boost/lexical_cast.hpp>
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
spreadsheet_session::spreadsheet_session(std::string name, server* server)
	: version_(0),
	  name_(name),
	  server_(server),
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




/*Registers the values that used to be stored in cells that are about to be changed so that they can later be
 *undone.
 *
 *Parameter is a list of variable names to register the old value of.
 */
void spreadsheet_session::register_old(std::string cell)
{
	//variable to store the final update command to push
	std::string to_push = "";

	//query the old contents of each cell

	std::string contents = server_->get_old(name_, cell);


	//now append the cell name and contents to the string
	to_push += static_cast<char>(27);
	to_push += cell;
	to_push += static_cast<char>(27);
	to_push += contents;


	undo_stack_.push(to_push);


}



bool spreadsheet_session::undo_empty()
{
	return undo_stack_.empty();
}

/*Returns a string that can be used as a spreadsheet update to undo the last change made to the spreadsheet.
 *
 *This change should not have registerOld called on it like a normal spreadsheet update or you will just 
 *keep looping back over the same changes.
 */
std::string spreadsheet_session::undo()
{

	char delimiter = static_cast<char>(27);

	std::string update = undo_stack_.top();
	undo_stack_.pop();
	std::string temp = update;

	temp.erase(0,1);
	size_t pos = 0;
	pos = temp.find(delimiter);
	std::string cell = temp.substr(0,pos);

	temp.erase(0, pos + 1);

	server_->update(name_, cell, temp);

	//save the top element
	std::string to_return = "UPDATE";
	to_return += static_cast<char>(27);
	to_return += boost::lexical_cast<std::string>(version_);
	to_return += update;
	to_return += "\n";

	//return the element
	return to_return;
}
