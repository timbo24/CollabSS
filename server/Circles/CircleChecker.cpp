/*Circular dependency checker for the online spreadsheet project
 *Last modified by Mark Smith 4/15/14
 *
 *An object of this class contains a dependency graph and a public  	
 *function to check if a cell change causes a circular dependency
 */

#include <string>
#include <iostream>
//#include <boost/regex.hpp>
#include "CircleChecker.h"

using namespace boost::xpressive;
using namespace std;


/*Circular dependency checker.  Contains a dependency graph and 
*a public function for checking if a cell change would cause a 
*circular dependency in that graph.  All other functions are 
*helpers for this functionality.
*/
CircleChecker::CircleChecker()
{
	TheGraph = new DependencyGraph::DependencyGraph();
}

CircleChecker::~CircleChecker()
{
	delete this->TheGraph;
}

	/*returns false if the formula would result in a circular 	      	*dependency in that cell.  The graph is not changed on a false 	*return.
	*If the formula would not result in a circular dependency it adds 	*the needed dependency's to the graph and returns true
	*/
	bool CircleChecker::NotCircular(std::string cell, std::string form)
	{
		//set for tracking all the nodes we have visited while checking dependency.  
		//This set will be used to keep from repreating visits as well as adding dependency 
		//if the function returns true.
		std::unordered_set<std::string> visited = std::unordered_set<std::string>();

		//queue of cells to visit to check for dependents.
		std::queue<std::string> toVisit = ParseVar(form);

		//store a list of all variables parsed out of the formula
		//toVisit = *(

		//loop through toVisit and check the direct dependents of each variable.
		while (!(toVisit.empty()))
		{
			//if the cell we are changing is ever found in the dependent list there is a 
			//circular dependency
			if (toVisit.front() == cell)
			{
				return false;
			}

			//insure we are not revisiting nodes
			std::unordered_set<std::string>::const_iterator got = (visited.find((toVisit.front())));

  			if (got == visited.end())
			{
				//add the node to the list of visited nodes
				visited.insert(toVisit.front());
				
				//new nodes to add to the queue
				std::unordered_set<std::string> toAdd = DirectDep(toVisit.front());
				//add them
				for (auto addIt = toAdd.begin() ; addIt != toAdd.end() ; ++addIt)
				{
					toVisit.push(*addIt);
					
				}
			}
		//progress to the next dependent
		toVisit.pop();
		}
	
	//if we did visit nodes, they are the new dependents of our cell
	if (!visited.empty())
	{
		TheGraph->ReplaceDependents(cell, visited);
	}
	
	//if we never returned false then we never encountered the cell in the list of the dependents 
	//and there is no circular dependency.
	return true;

	}

	


	/*returns a list of cells directly dependent on this one*/
	std::unordered_set<std::string> CircleChecker::DirectDep(std::string s)
	{
		return TheGraph->GetDependents(s);
	}
	
        /*helper method for parsing a formula string into individual variables, returns an empty queue if 
	 *the string is not a formula or if there are no variables.
	 *
	 *Regex code modeled after Boost library example:
	 *http://boost-sandbox.sourceforge.net/libs/xpressive/doc/html/boost_xpressive/user_s_guide/examples.html#boost_xpressive.user_s_guide.examples.see_if_a_string_contains_a_sub_string_that_matches_a_regex
	*/
	std::queue<std::string> CircleChecker::ParseVar(std::string s)
	{
		//store variables as we find them
		std::queue<std::string> toReturn = std::queue<std::string>();

		//If it is not a formula, return an empty set
		if (s[0] != '=')
		{
			return toReturn;
		}

		//store the variables from the parsed string
		smatch allVars;
		//match the pattern of one or more letters followed by one or more numbers.
		sregex cellNames = sregex::compile("[[:upper:]]+[[:digit:]]+");

		//iterators to walk the array
		sregex_iterator cur ( s.begin(), s.end(), cellNames);
		sregex_iterator end;

		//loop, find, add matches to the expression
		for ( ; cur != end ; ++cur)
		{
		  smatch const &thisVar = *cur;
		  toReturn.push(thisVar[0]);
		  //cout << thisVar[0] << endl;
		}

		/*
	        
		//search for variables and store them all
		regex_search( s, allVars, cellNames);

		//add them to the return list
		for (int i = 0 ; i < allVars.size() ; i++)
		{
		  cout << allVars[i] << endl;
		  toReturn.push(allVars[i]);
		}

		*/

		//return the list at the end
		return toReturn;
	}
