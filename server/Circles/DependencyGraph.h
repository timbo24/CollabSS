/*Dependency graph for the online spreadsheet project
 *Last modified by Mark Smith 4/7/14
 *
 *This graph will be used to keep a record of dependent nodes to prevent 
 *circular dependencies
 */

#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <string>
#include <iostream>
#include <unordered_set>
#include <map>
#include "DependencyNode.h"

using namespace std;

class DependencyGraph
{
 public:
  //map to hold the graph
  std::map<std::string, DependencyNode::DependencyNode*> *graphHash;
  //number of key/value pairs in the map
  int keyValCount;

  //constructor, creates an empty graph
  DependencyGraph();
  ~DependencyGraph();

  //takes the place of a C# property for size
  int Size();
  //indexer for dependencygraph, dg["a"] should return the size of a's dependees
  // int this[std::string s];
  int operator [](const std::string &s);

  //reports if dependents/ees is empty
  bool HasDependents(std::string s);
  bool HasDependees(std::string s);

  //returns an unordered set of dependents/ees
  std::unordered_set<std::string> GetDependents(std::string s);
  std::unordered_set<std::string> GetDependees(std::string s);
  
  //adds the ordered pair (s,t) if it doesn't exist
  void AddDependency(string s, string t);
  //removes the ordered pair (s,t)
  void RemoveDependency(string s, string t);

  //Removes all existing ordered pairs (s,r) and replaced them with (s,t)
  void ReplaceDependents(string s, const std::unordered_set<std::string>& newDependents);
  
  //Removes all ordered pairs (r,s) and replaces them with (r,t)
  void ReplaceDependees(string s, const std::unordered_set<std::string>& newDependees);



 private:

  //internal method for removing a key with no depentents and dependees.  This method does not 
  //decriment the count.  Only use it inside another function that does
  void KillKeyValPair(string s);

  

};

#endif
