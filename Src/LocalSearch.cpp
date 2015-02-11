#include "LocalSearch.h"

#include <iostream>

using namespace std;

void LocalSearch(ModelCompact & iModelCompact)
{
  iModelCompact.LocalSearchAlgorithm(1);
  cout << "//--- Recherche locale ---\n";
  cout << "Valeur de la solution trouvee: " << iModelCompact._ActualCost << "\n";
  cout << "Affectation trouvee:\n";
  iModelCompact._x.Print();
}


