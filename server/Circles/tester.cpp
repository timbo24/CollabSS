#include <iostream>
#include <string>
#include "DependencyGraph.h"
#include "CircleChecker.h"

using namespace std;

int main()
{
  DependencyGraph *t = new DependencyGraph();

  // cout << "" << t->Size() << endl;

  // t->AddDependency("a", "b");
  //t->AddDependency("a", "c");
  //t->AddDependency("d", "c");
  //t->AddDependency("d", "c");
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

  /*
  std::unordered_set<std::string> *tempSet = new std::unordered_set<std::string>({"x" , "y", "z"});
  // *tempSet = t->GetDependents("a");
  t->ReplaceDependees("c", *tempSet);

 std::unordered_set<std::string> *temppSet = new std::unordered_set<std::string>({"x" , "y", "z"});
  *temppSet = t->GetDependees("c");

 std::unordered_set<std::string>::const_iterator gottt = temppSet->find ("a");
  cout << (gottt != temppSet->end()) << endl;
//search the set for the element
 std::unordered_set<std::string>::const_iterator gottttt = temppSet->find ("d");
  cout << (gottttt != temppSet->end()) << endl;
 std::unordered_set<std::string>::const_iterator gotttttt = temppSet->find ("b");
  cout << (gotttttt != temppSet->end()) << endl;
std::unordered_set<std::string>::const_iterator go = temppSet->find ("x");
  cout << (go != temppSet->end()) << endl;
//search the set for the element
 std::unordered_set<std::string>::const_iterator goo = temppSet->find ("y");
  cout << (goo != temppSet->end()) << endl;
 std::unordered_set<std::string>::const_iterator gooo = temppSet->find ("z");
  cout << (gooo != temppSet->end()) << endl;

cout << (t->GetDependees("c")).size() << endl;
  */
  
  /*
  std::unordered_set<std::string> *tempSet = new std::unordered_set<std::string>({"x" , "y", "z"});
  // *tempSet = t->GetDependents("a");
  t->ReplaceDependents("c", *tempSet);

 std::unordered_set<std::string> *temppSet = new std::unordered_set<std::string>({"x" , "y", "z"});
  *temppSet = t->GetDependents("c");

 std::unordered_set<std::string>::const_iterator gottt = temppSet->find ("a");
  cout << (gottt != temppSet->end()) << endl;
//search the set for the element
 std::unordered_set<std::string>::const_iterator gottttt = temppSet->find ("d");
  cout << (gottttt != temppSet->end()) << endl;
 std::unordered_set<std::string>::const_iterator gotttttt = temppSet->find ("b");
  cout << (gotttttt != temppSet->end()) << endl;
std::unordered_set<std::string>::const_iterator go = temppSet->find ("x");
  cout << (go != temppSet->end()) << endl;
//search the set for the element
 std::unordered_set<std::string>::const_iterator goo = temppSet->find ("y");
  cout << (goo != temppSet->end()) << endl;
 std::unordered_set<std::string>::const_iterator gooo = temppSet->find ("z");
  cout << (gooo != temppSet->end()) << endl;

cout << (t->GetDependents("c")).size() << endl;

 t->RemoveDependency("q", "u");
  */

  CircleChecker *c = new CircleChecker();

  std::string one = "A1";
  std::string two = "=B1";

  //cout <<  << endl;
  (c->NotCircular(one, two));

  std::string three = "B1";
  std::string four = "=C1";

  //cout <<  << endl;
  (c->NotCircular(three, four));

  std::string five = "C1";
  std::string six = "=3++G1";

  cout << (c->NotCircular(five, six))  << endl;

  std::string seven = "G1";
  std::string eight = "=3* F1 + 1";

  cout << (c->NotCircular(seven, eight))  << endl;

  std::string nine = "F1";
  std::string ten = "=3* W1 +1";

  cout << (c->NotCircular(nine, ten))  << endl;

std::string nine1 = "W1";
  std::string ten1 = "=3* (B1)";

  cout << (c->NotCircular(nine1, ten1))  << endl;
  

  

  
  
  return 0;
}
