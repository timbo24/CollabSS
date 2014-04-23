/*Node for a dependency graph for the online spreadsheet project
 *Last modified by Mark Smith 4/7/14
 *
 */

#ifndef DEPENDENCYNODE_H
#define DEPENDENCYNODE_H

#include <string>
#include <iostream>
#include <unordered_set>

using namespace std;

class DependencyNode
{
 public:
  //constructor and destructor
  DependencyNode();
  ~DependencyNode();

  //functions to add and remove dependets from this node
  bool addDependent(std::string newDependent);
  bool removeDependent(std::string killDependent);

  //functions to add and remove dependees from this node
  bool addDependee(std::string newDependee);
  bool removeDependee(std::string killDependee);


 private:

  //allow the graph to view private vatiables
  friend class DependencyGraph;

  //c++ version of a Hash Set
  std::unordered_set<std::string> *_dependentList;
  std::unordered_set<std::string> *_dependeeList;

};

#endif
