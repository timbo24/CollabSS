/*Node for a dependency graph for the online spreadsheet project
 *Last modified by Mark Smith 4/7/14
 *
 *This is a helper class for the dependency graph, a single node stores
 *all the dependents and dependees for a particular cell.  Nodes are intended to
 *be the value part of a key, value pair used in a map.
 */

#include <string>
#include <iostream>
//#include <unordered_set>
#include "DependencyNode.h"

using namespace std;


  //constructor, creates an empty dependency node
DependencyNode::DependencyNode()
  {
    //set of nodes that depend on this node
    _dependentList = new unordered_set<std::string>();
    //set of nodes that this node depends on
    _dependeeList = new unordered_set<std::string>();

  }

//destructor
DependencyNode::~DependencyNode()
  {
    _dependentList->clear();
    _dependeeList->clear();

    delete this->_dependentList;
    delete this->_dependeeList;
    
  }

  //function to add dependents to this node
bool DependencyNode::addDependent(std::string newDependent)
  {
    //search the set for the element
    std::unordered_set<std::string>::const_iterator got = _dependentList->find (newDependent);
    //if the element is not int the list, add it and return true
                if ((got == this->_dependentList->end()))
                {
                    this->_dependentList->insert(newDependent);
                    return true;
                }
                //if not return false as no change was made
                else
                {
                    return false;
                }
  }

//function to remove dependents from this node
bool DependencyNode::removeDependent(std::string killDependent)
  {
//search the set for the element
    std::unordered_set<std::string>::const_iterator got = _dependentList->find (killDependent);

                //check to see if the element exists, if so remove it an return true
                if (!(got == this->_dependentList->end()))
                {
                    this->_dependentList->erase(killDependent);
                    return true;
                }
                //if not return false as no change was made
                else
                {
                    return false;
                }

  }

  //functions to add dependees to this node
bool DependencyNode::addDependee(std::string newDependee)
  {
    //search the set for the element
    std::unordered_set<std::string>::const_iterator got = _dependeeList->find (newDependee);
    //if the element is not int the list, add it and return true
                if ((got == this->_dependeeList->end()))
                {
                    this->_dependeeList->insert(newDependee);
                    return true;
                }
                //if not return false as no change was made
                else
                {
                    return false;
                }

  }

//function to remove dependees from this node
bool DependencyNode::removeDependee(std::string killDependee)
  {
//search the set for the element
    std::unordered_set<std::string>::const_iterator got = _dependeeList->find (killDependee);

                //check to see if the element exists, if so remove it an return true
                if (!(got == this->_dependeeList->end()))
                {
                    this->_dependeeList->erase(killDependee);
                    return true;
                }
                //if not return false as no change was made
                else
                {
                    return false;
                }
    
  }
 
