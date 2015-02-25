#include "LocalSearch.h"
#include "timetools.h"

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


void LocalSearchAlgorithm(ModelCompact & iModelCompact)
{
  // Taille maximale des voisinages explores
  int VSize = 4;
  // Nombre de solutions initiales et de recherches locales a faire
  int PopSize = 24;
  // Taille de l'exploration dans la construction gloutonne-aleatoire
  int RCL = std::max((int)iModelCompact._m / 5, 3);

  assert(PopSize > 0);
  vector<ModelCompact> aCompact(PopSize, ModelCompact(iModelCompact));
  
  // Strategie de construction gloutonne aleatoire
  int BuiltStrategy = 0;
  // Nombre d'iterations
  int NbIteration = 1;
  
  /*if (BuiltStrategy==-1)
  {
    aCompact[0].FindFeasableSolution(1);
    for (int i = 1; i < PopSize; i++)
      aCompact[i].FindFeasableSolution(1);
  }*/

  int ValueGlouton = iModelCompact.ComputeUBforObjective();
  
  int IdxBest = PopSize;
  while (IdxBest == PopSize && NbIteration < 1000)
  {
    int i;
    #ifdef __linux__
    #pragma omp parallel for schedule(dynamic,1)
    #endif
    for (i = 1; i < PopSize; i++)
    {
      int Idx = i;
      if (Idx == 1)
      {
        aCompact[0].FindFeasableSolution(0);
        aCompact[0].ComputeCost();
        aCompact[1].FindFeasableSolution(1);
        aCompact[1].ComputeCost();
      }
      else
      {
        switch (Idx)
        {
        case 2:
          aCompact[Idx].GRASP(1, BuiltStrategy);
          #pragma omp critical
          {
            if (aCompact[Idx].IsAdmissible() && aCompact[Idx]._ActualCost < ValueGlouton)
              ValueGlouton = aCompact[Idx]._ActualCost;
          }
          break;

        default:
          aCompact[Idx].GRASP(RCL, BuiltStrategy);
          #pragma omp critical
          {
            if (aCompact[Idx].IsAdmissible() && aCompact[Idx]._ActualCost < ValueGlouton)
              ValueGlouton = aCompact[Idx]._ActualCost;
          }
          break;
        }
      }

    }
    
    #ifdef __linux__
    #pragma omp parallel for schedule(dynamic,1)
    #endif
    for (i = 0; i < PopSize; i++)
    {
      int Idx = i;
      if (aCompact[Idx].IsAdmissible())
      {
        #pragma omp critical
        {
          cout << "Solution " << Idx << " admissible avant recherche locale.\n";
        }
      }
      else
      {
        #pragma omp critical
        {
          cout << "Solution " << Idx << " inadmissible avant recherche locale.\n";
        }
      }
      aCompact[Idx].LocalSearchAlgorithm(VSize);
    }
    
    // Recherche de la meilleure solution trouvee
    IdxBest = 0;
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
    NbIteration++;

    cout << "Solution Inadmissible apres recherche locale : ";
    for (i = 0; i < PopSize; i++) {
      if (aCompact[i].IsAdmissible())
        cout << i << " ";
    }
    cout << "\n";
  }
  
  cout << "Meilleure valeure obtenue par la glouton : " << ValueGlouton << "\n";
}


void LocalSearch(ModelCompact & iModelCompact)
{
  double BeginLocalSearch_UserTime = get_wall_time();
  double BeginLocalSearch_CPUTime = get_cpu_time();
  LocalSearchAlgorithm(iModelCompact);
  double EndLocalSearch_CPUTime = get_cpu_time();
  double EndLocalSearch_UserTime = get_wall_time();

  cout << "//---------- Recherche locale ----------\n";
  iModelCompact.PrintCurrentSolution(2);
  double LocalSearch_CPUTime = EndLocalSearch_CPUTime - BeginLocalSearch_CPUTime;
  cout << "Temps CPU:\t" << LocalSearch_CPUTime << "s\n";
  double LocalSearch_UserTime = EndLocalSearch_UserTime - BeginLocalSearch_UserTime;
  cout << "Temps utilisateur:\t" << LocalSearch_UserTime << "s\n";
  cout << "\n";
}

