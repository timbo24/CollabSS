#include <iostream>
#include <string>
#include "DependencyGraph.h"

using namespace std;

int main()
{
  DependencyGraph *t = new DependencyGraph();

  // cout << "" << t->Size() << endl;

  t->AddDependency("a", "b");
  t->AddDependency("a", "c");
  t->AddDependency("d", "c");
  t->AddDependency("d", "c");
  //t->RemoveDependency("a", "b");

  //cout << "" << t->Size() << endl;
  /*
  cout << (t->GetDependents("a")).size() << endl;

cout << (t->GetDependents("c")).size() << endl;
cout << (t->GetDependents("d")).size() << endl;
cout << (t->GetDependents("e")).size() << endl;
cout << (t->GetDependees("a")).size() << endl;

cout << (t->GetDependees("c")).size() << endl;
cout << (t->GetDependees("d")).size() << endl;
cout << (t->GetDependees("e")).size() << endl;
  */
  //cout << (t->GetDependees("b")).size() << endl;
  //cout << (t->GetDependents("b")).size() << endl;
  
  /*
 
   std::unordered_set<std::string> *tempSet = new std::unordered_set<std::string>();
  *tempSet = t->GetDependents("a");

//search the set for the element
 std::unordered_set<std::string>::const_iterator got = tempSet->find ("b");
  cout << (got != tempSet->end()) << endl;

//search the set for the element
 std::unordered_set<std::string>::const_iterator gott = tempSet->find ("c");
  cout << (gott != tempSet->end()) << endl;

  *tempSet = t->GetDependees("b");

//search the set for the element
 std::unordered_set<std::string>::const_iterator gottt = tempSet->find ("a");
  cout << (gottt != tempSet->end()) << endl;
//search the set for the element
 std::unordered_set<std::string>::const_iterator gottttt = tempSet->find ("d");
  cout << (gottttt != tempSet->end()) << endl;
 std::unordered_set<std::string>::const_iterator gotttttt = tempSet->find ("c");
  cout << (gotttttt != tempSet->end()) << endl;
  */

  std::unordered_set<std::string> *tempSet = new std::unordered_set<std::string>({"x" , "y", "z"});
  // *tempSet = t->GetDependents("a");
  cout << "got to function call"<< endl;
  t->ReplaceDependees("c", *tempSet);
  



  
  return 0;
}
