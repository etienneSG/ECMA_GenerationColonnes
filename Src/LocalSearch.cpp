#include "LocalSearch.h"

#include <iostream>

using namespace std;

void LocalSearch(ModelCompact & iModelCompact)
{
  iModelCompact.LocalSearchAlgorithm(1);
  cout << iModelCompact._ActualCost << "\n";
}


