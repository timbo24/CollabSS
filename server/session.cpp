
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
	 UndoStack = new std::stack<std::string>();

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




/*Registers the values that used to be stored in cells that are about to be changed so that they can later be
 *undone.
 *
 *Parameter is a list of variable names to register the old value of.
 */
void spreadsheet_session::registerOld(std::set<std::string> cells, std::string sheet)
{
  //variable to store the final update command to push
  std::string toPush = "";
  
  //query the old contents of each cell
  for (auto addIt = cells.begin() ; addIt != cells.end() ; ++ addIt)
  {

	  std::string contents = server_->get_old(sheet, *addIt);

    //create an sql select statement
     
	  char e = 27;
	  std::string ESC(1,e);
    //now append the cell name and contents to the string
    toPush += ESC + *addIt + ESC + contents;
    
  }
  

  toPush += "\n";

    UndoStack->push(toPush);

}




/*Returns a string that can be used as a spreadsheet update to undo the last change made to the spreadsheet.
 *
 *This change should not have registerOld called on it like a normal spreadsheet update or you will just 
 *keep looping back over the same changes.
 */
std::string spreadsheet_session::undo(std::string version)
{
  //save the top element
  std::string toReturn = "UPDATE\e" + version;
  toReturn += std::string(UndoStack->top());

  //pop the stack
  UndoStack->pop();

  //return the element
  return toReturn;
  
}
