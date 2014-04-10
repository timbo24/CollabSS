#include <iostream>
#include <string>
#include "DependencyGraph.h"

using namespace std;

int main()
{
  DependencyGraph *t = new DependencyGraph();

  t->AddDependency("a", "b");
  t->AddDependency("a", "b");
t->AddDependency("a", "b");

  cout << "" << t->Size() << endl;
  
  return 0;
}
