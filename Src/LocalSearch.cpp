#include "LocalSearch.h"

#include <iostream>
#include <vector>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <../../opt/ibm/ILOG/CPLEX_Studio125/concert/include/ilconcert/iloenv.h>

using namespace std;

void LocalSearch(ModelCompact & iModelCompact)
{
  // Taille maximale des voisinages explores
  int VSize = 2;
  // Nombre de solution initiale et de recherches locales a faire
  int PopSize = 8;
  // Taille de l'exploration dans la construction gloutonne-aleatoire
  int RCL = std::max((int)iModelCompact._m / 5, 3);
  
  
  assert(PopSize > 0);
  vector<ModelCompact> aCompact(PopSize, ModelCompact(iModelCompact));

  // Le premier modele compact est initialise avec la premiere solution entiere Cplex
  memcpy(aCompact[0]._x, iModelCompact._x, iModelCompact._n*sizeof(int));
  
  // Le deuxieme est construit de maniere deterministe
  if (PopSize > 1)
    aCompact[1].GRASP(1);
  
  int i;
  // Les suivants sont initialises avec de l'aleatoire
  for (i = 2; i < PopSize; i++) {
    aCompact[i].GRASP(RCL);
  }
  
  // Algorithme de recherche locale autour de chaque solution
  for (i = 0; i < PopSize; i++) {
    aCompact[i].LocalSearchAlgorithm(VSize);
  }
  
  // Recherche de la meilleure solution trouvee
  int IdxBest = 0;
  for (i = 1; i < PopSize; i++) {
    if (aCompact[i]._ActualCost < aCompact[IdxBest]._ActualCost)
      IdxBest = i;
  }
  memcpy(iModelCompact._x, aCompact[IdxBest]._x, iModelCompact._n*sizeof(int));
  
  
  cout << "//--- Recherche locale ---\n";
  cout << "Valeur de la solution trouvee: " << iModelCompact._ActualCost << "\n";
  cout << "Affectation trouvee:\n";
}


