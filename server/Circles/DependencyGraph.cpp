/*Dependency graph for the online spreadsheet project
 *Last modified by Mark Smith 4/7/14
 *
 *This graph will be used to keep a record of dependent nodes to prevent 
 *circular dependencies
 */

#include <iostream>
#include <string>
#include "DependencyGraph.h"


using namespace std;

//Constructor, starts with an empty graph
DependencyGraph::DependencyGraph()
 {
   graphHash = new std::map<std::string, DependencyNode::DependencyNode*>();
   keyValCount = 0;
 }

//Destructor
DependencyGraph::~DependencyGraph()
{
  this->graphHash->clear();
  delete this->graphHash;
}

//takes the place of a C# property for size
int DependencyGraph::Size()
{
  return this->keyValCount;
}

  //indexer for dependencygraph, dg["a"] should return the size of a's dependees

int DependencyGraph::operator [](const std::string &s)
{
  //int to return at the end
  int toReturn;

  //search the set for the element
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  // std::string stuffnstuff = "asgdf";
  if (got == this->graphHash->end())
  {
    toReturn = 0;
  }
  else
  {
    toReturn = (((*graphHash)[s])->_dependeeList)->size();
  }
  
  //return the value
  return toReturn;
  
}


  //reports if dependents/ees is empty
bool DependencyGraph::HasDependents(std::string s)
{
  //search the set for the element
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  // if the element isn't in the graph, return false
  if (got == this->graphHash->end())
  {
    return false;
  }
  //check for dependents
  else if ((((*graphHash)[s])->_dependentList)->size() > 0)
  {
    return true;
  }
  //if we miss the elseif the depentents are empty
  return false;

}

bool DependencyGraph::HasDependees(std::string s)
{
  //search the set for the element
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  // if the element isn't in the graph, return false
  if (got == this->graphHash->end())
  {
    return false;
  }
  //check for dependees
  else if ((((*graphHash)[s])->_dependeeList)->size() > 0)
  {
    return true;
  }
  //if we miss the elseif the depentees are empty
  return false;

}

  //returns an unordered set of dependents/ees
std::unordered_set<std::string> DependencyGraph::GetDependents(std::string s)
{
  //search the set for the element
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  // if the element isn't in the graph, return an empty set
  if (got == this->graphHash->end())
  {
    return std::unordered_set<std::string>();
  }
  //otherwise return the dependent list
  else
  {
    return std::unordered_set<std::string>(*(((*graphHash)[s])->_dependentList));
  }

}

std::unordered_set<std::string> DependencyGraph::GetDependees(std::string s)
{
  //search the set for the element
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  // if the element isn't in the graph, return an empty set
  if (got == this->graphHash->end())
  {
    return std::unordered_set<std::string>();
  }
  //otherwise return the dependee list
  else
  {
    return std::unordered_set<std::string>(*(((*graphHash)[s])->_dependeeList));
  }

}
  
  //adds the ordered pair (s,t) if it doesn't exist
void DependencyGraph::AddDependency(string s, string t)
{
  //variable to track if a pair was added
  bool wasAdded = false;

  //check to see if "s" is in the table
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  if (got != this->graphHash->end())
  {
    //if so attempt to add the pair, if it was added flip the boolean
    bool tempBool = (((*graphHash)[s])->addDependent(t));
    if (tempBool)
    {
      wasAdded = true;
    }
  }
  //if the element isn't in the graph, make a new node and add it
  else
  {
    //make a new node
    DependencyNode::DependencyNode *newNode = new DependencyNode();

    //add t to the dependent list
    newNode->addDependent(t);

    //add s and the new node to the map
    std::pair<std::string, DependencyNode::DependencyNode*> *tempPair = new std::pair<std::string, DependencyNode::DependencyNode*>(s, newNode);

    graphHash->insert(*tempPair);

    //flip the bool
    wasAdded = true;
    
  }

  //check to see if "t" is in the table
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator gott = graphHash->find (t);

  if (gott != this->graphHash->end())
  {
    //if so attempt to add the pair, if it was added flip the boolean
    bool tempBool = (((*graphHash)[t])->addDependee(s));
      if (tempBool)
    {
      wasAdded = true;
    }
  }
  //if "t" is not in the table add a new item to the hash table with "t" as the key
  else
  {
    //make a new node
    DependencyNode::DependencyNode *newNode = new DependencyNode();

    //add t to the dependent list
    newNode->addDependee(s);

    //add t and the new node to the map
    std::pair<std::string, DependencyNode::DependencyNode*> *tempPair = new std::pair<std::string, DependencyNode::DependencyNode*>(t, newNode);

    graphHash->insert(*tempPair);

    //flip the bool
    wasAdded = true;
    
  }

  //if a pair was added increase the count
  if (wasAdded)
  {
    keyValCount++;
  }
  
}

  //removes the ordered pair (s,t)
void DependencyGraph::RemoveDependency(string s, string t)
{
  //bool to track if something was removed
  bool wasRemoved = false;

  //check to see if "s" is in the table
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  if (got != this->graphHash->end())
  {
    //if so try to remove dependent t
    bool tempBool = (((*graphHash)[s])->removeDependent(t));
    if (tempBool)
    {
      wasRemoved = true;

      //remove the key if it contains an empty node
      KillKeyValPair(s);
    }
  }

  //check to see if "t" is in the table
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator gott = graphHash->find (t);

  if (gott != this->graphHash->end())
  {
    //if so try to remove dependee s
    bool tempBool = (((*graphHash)[s])->removeDependee(s));
    if (tempBool)
    {
      wasRemoved = true;

      //remove the key if it contains an empty node
      KillKeyValPair(t);
    }
  }

  //if a pair was removed change the count
  if (wasRemoved)
  {
    keyValCount--;
  }
}

  //Removes all existing ordered pairs (s,r) and replaced them with (s,t)
void DependencyGraph::ReplaceDependents(string s, std::unordered_set<std::string> newDependents)
{
  //check to see if "s" is in the table
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  if (got != this->graphHash->end())
  {
    //temp copy of the new list
    //std::unordered_set<std::string> *tempSet = new std::unordered_set<std::string>(newDependents);

    //remove all the dependents
    for (auto killIt = (((*graphHash)[s])->_dependentList)->begin() ; killIt != (((*graphHash)[s])->_dependentList)->end() ; ++killIt )
    {
      RemoveDependency(s, *killIt);
    }
    //add the new ones
    for (auto addIt = (newDependents).begin() ; addIt != (newDependents).end() ; ++addIt )
    {
      AddDependency(s, *addIt);
    }
  
    
    
  }
  //if s isn't in the table just add all the new pairs
  else
  {
    for (auto addIt = (newDependents).begin() ; addIt != (newDependents).end() ; ++addIt )
    {
      AddDependency(s, *addIt);
    } 
  }

}
  
  //Removes all ordered pairs (r,s) and replaces them with (r,t)
void DependencyGraph::ReplaceDependees(string s, std::unordered_set<std::string> newDependees)
{
  //check to see if "s" is in the table
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  if (got != this->graphHash->end())
  {
    //temp copy of the new list
    //std::unordered_set<std::string> *tempSet = new std::unordered_set<std::string>(newDependents);

    //remove all the dependees
    for (auto killIt = (((*graphHash)[s])->_dependeeList)->begin() ; killIt != (((*graphHash)[s])->_dependeeList)->end() ; ++killIt )
    {
      RemoveDependency(*killIt, s);
    }
    //add the new ones
    for (auto addIt = (newDependees).begin() ; addIt != (newDependees).end() ; ++addIt )
    {
      AddDependency(*addIt, s);
    }
  
    
    
  }
  //if s isn't in the table just add all the new pairs
  else
  {
     for (auto addIt = (newDependees).begin() ; addIt != (newDependees).end() ; ++addIt )
    {
      AddDependency(*addIt, s);
    }
  }
  

}


  //internal method for removing a key with no depentents and dependees.  This method does not 
  //decriment the count.  Only use it inside another function that does
void DependencyGraph::KillKeyValPair(string s)
{
  //check to see if the key is in the graph
  std::map<std::string, DependencyNode::DependencyNode*>::const_iterator got = graphHash->find (s);

  if (got != this->graphHash->end())
  {
    //if so, and if both it's dependent count and dependee count are zero remove it from the graph
    if ( ((((*graphHash)[s])->_dependeeList)->size() == 0) &&
	 ((((*graphHash)[s])->_dependentList)->size() == 0) )
    {
      graphHash->erase(s);
    }
  }

}
