/*Circular dependency checker for the online spreadsheet project
 *Last modified by Mark Smith 4/15/14
 *
 *An object of this class contains a dependency graph and a public  	
 *function to check if a cell change causes a circular dependency
 */

#ifndef CIRCLECHECKER_H
#define CIRCLECHECKER_H

#include <string>
#include <iostream>
#include <unordered_set>
#include <map>
#include <queue>
#include <boost/xpressive/xpressive.hpp>
#include "DependencyGraph.h"
using namespace std;
class CircleChecker
{
public:
	//constructor and destructor
	CircleChecker();
	~CircleChecker();
	//returns false if the formula would result in a circular dependency in that cell.  The graph is not changed on a false return.
	//If the formula would not result in a circular dependency it adds the needed dependency's to the graph and returns true
	bool NotCircular(std::string cell, std::string form);

	


private:
	//returns a list of cells directly dependent on this one
	std::unordered_set<std::string> DirectDep(std::string s);
	
	//helper method for parsing a formula string into individual variables
	std::queue<std::string> ParseVar(std::string s);

	//the underlying dependency graph
	DependencyGraph::DependencyGraph *TheGraph; 

};
#endif
