#include "LocalSearch.h"

#include <iostream>
#include <vector>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <../../opt/ibm/ILOG/CPLEX_Studio125/concert/include/ilconcert/iloenv.h>

#ifdef __linux__
#include <omp.h> // Open Multi-Processing Library (Linux only)
#endif

using namespace std;

void LocalSearch(ModelCompact & iModelCompact)
{
  // Taille maximale des voisinages explores
  int VSize = 2;
  // Nombre de solution initiale et de recherches locales a faire
  int PopSize = 8;
  // Taille de l'exploration dans la construction gloutonne-aleatoire
  int RCL = std::max((int)iModelCompact._m / 5, 3);
  
  ModelCompact testCompact(iModelCompact);
  testCompact.GRASP(1);
  
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
  #ifdef __linux__
  int nbThread = std::max(1, PopSize/NB_PROC);
  #pragma omp parallel for schedule(dynamic,nbThread)
  #endif
  for (i = 0; i < PopSize; i++) {
    aCompact[i].LocalSearchAlgorithm(VSize);
  }
  
  // Recherche de la meilleure solution trouvee
  int IdxBest = 0;
  while (IdxBest < PopSize && !aCompact[IdxBest].IsAdmissible())
    IdxBest++;
  for (i = IdxBest+1; i < PopSize; i++) {
    if (aCompact[i].IsAdmissible() && aCompact[i]._ActualCost < aCompact[IdxBest]._ActualCost)
      IdxBest = i;
  }
  if (IdxBest < PopSize)
  {
    memcpy(iModelCompact._x, aCompact[IdxBest]._x, iModelCompact._n*sizeof(int));
    iModelCompact._ActualCost = aCompact[IdxBest]._ActualCost;
    for (int j = 0; j < iModelCompact._m; j++)
      iModelCompact._ActualCapacity[j] = aCompact[IdxBest]._ActualCapacity[j];
  }
  
  cout << "//--- Recherche locale ---\n";
  cout << "Valeur de la solution trouvee: " << iModelCompact._ActualCost << "\n";
  cout << "Affectation trouvee:\n";
}


