#include "LocalSearch.h"
#include "timetools.h"

#include <iostream>
#include <vector>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <../../opt/ibm/ILOG/CPLEX_Studio125/concert/include/ilconcert/iloenv.h>

#include <omp.h> // Open Multi-Processing Library (Linux only)

using namespace std;


void LocalSearchAlgorithm(ModelCompact & iModelCompact)
{
  // Taille maximale des voisinages explores
  int VSize = 2;
  // Nombre de solutions initiales et de recherches locales a faire
  int PopSize = 8;
  // Taille de l'exploration dans la construction gloutonne-aleatoire
  int RCL = std::max((int)iModelCompact._m / 5, 3);

  assert(PopSize > 0);
  vector<ModelCompact> aCompact(PopSize, ModelCompact(iModelCompact));

  // Strategie de construction gloutonne aleatoire
  int BuiltStrategy = 0;

  int ValueGlouton = iModelCompact.ComputeUBforObjective();
  vector<int> AdmSolution;

  int IdxBest = PopSize;
  while (IdxBest == PopSize)
  {
    cout << "******************** Construction des solutions initiales *********************\n";
    int i;
    #pragma omp parallel for schedule(dynamic)
    for (i = 1; i < PopSize; i++)
    {
      int Idx = i;
      switch (Idx)
      {
      case 1:
        aCompact[0].FindFeasableSolution(1);
        aCompact[1].FindFeasableSolution(0);
        #pragma omp critical
        {
          cout << "----- Solution " << 0 << " -----\n";
          aCompact[0].PrintCurrentSolution(0);
        }
        break;
        
      case 2:
        aCompact[Idx].GRASP(1, BuiltStrategy);
        break;

      default:
        aCompact[Idx].GRASP(RCL, BuiltStrategy);
        break;
      }
      #pragma omp critical
      {
        if (Idx >= 2 && aCompact[Idx].IsAdmissible() && aCompact[Idx]._ActualCost < ValueGlouton)
          ValueGlouton = aCompact[Idx]._ActualCost;
        cout << "----- Solution " << i << " -----\n";
        aCompact[Idx].PrintCurrentSolution(0);
      }
    }
    
    cout << "\n****************************** Recherche Locale *******************************\n";
    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i < PopSize; i++)
    {
      int Idx = i;
      if (aCompact[Idx].IsAdmissible())
      {
        #pragma omp critical
        {
          AdmSolution.push_back(Idx);
        }
      }
      aCompact[Idx].LocalSearchAlgorithm(VSize);
      #pragma omp critical
      {
        cout << "----- Solution " << i << " -----\n";
        aCompact[Idx].PrintCurrentSolution(0);
      }
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
      iModelCompact._Penalties = aCompact[IdxBest]._Penalties;
      for (int j = 0; j < iModelCompact._m; j++)
        iModelCompact._ActualCapacity[j] = aCompact[IdxBest]._ActualCapacity[j];
    }
  }

  
  cout << "\n************************************ Bilan ************************************\n";
  cout << "Meilleure valeure obtenue par le glouton : " << ValueGlouton << "\n";
  cout << "Solutions admissibles avant la recherche locale : ";
  for (unsigned int k = 0; k < AdmSolution.size(); k++) {
      cout << AdmSolution[k] << " ";
  }
  cout << "\n";
  cout << "Solutions admissibles apres la recherche locale : ";
  for (int i = 0; i < PopSize; i++) {
    if (aCompact[i].IsAdmissible())
      cout << i << " ";
  }
  cout << "\n";
  
}


void LocalSearch(ModelCompact & iModelCompact)
{
  double BeginLocalSearch_UserTime = get_wall_time();
  double BeginLocalSearch_CPUTime = get_cpu_time();
  LocalSearchAlgorithm(iModelCompact);
  double EndLocalSearch_CPUTime = get_cpu_time();
  double EndLocalSearch_UserTime = get_wall_time();

  cout << "\n----- Optimum local -----\n";
  iModelCompact.PrintCurrentSolution(2);
  
  cout << "\n---- Temps de calcul ----\n";
  double LocalSearch_CPUTime = EndLocalSearch_CPUTime - BeginLocalSearch_CPUTime;
  cout << "Temps CPU:         " << LocalSearch_CPUTime << "s\n";
  double LocalSearch_UserTime = EndLocalSearch_UserTime - BeginLocalSearch_UserTime;
  cout << "Temps utilisateur: " << LocalSearch_UserTime << "s\n";
  cout << "\n";
}


void SpecificLocalSearch(ModelCompact & iModelCompact)
{
  // Taille maximale des voisinages explores
  int VSize = 3;

  cout << "************************** Cplex et recherche locale **************************\n";

  double BeginCplex_UserTime = get_wall_time();
  double BeginCplex_CPUTime = get_cpu_time();
  iModelCompact.FindFeasableSolution(0);
  double EndCplex_CPUTime = get_cpu_time();
  double EndCplex_UserTime = get_wall_time();

  if (iModelCompact.IsAdmissible())
  {
    cout << "Cplex trouve une solution admissible en moins de 7 secondes.\n";
    iModelCompact.PrintCurrentSolution(0);
    cout << "\n";

    double BeginLocalSearch_UserTime = get_wall_time();
    double BeginLocalSearch_CPUTime = get_cpu_time();
    iModelCompact.LocalSearchAlgorithm(VSize);
    double EndLocalSearch_CPUTime = get_cpu_time();
    double EndLocalSearch_UserTime = get_wall_time();
    cout << "Solution trouvee en cherchant localement depuis la precedente :\n";
    iModelCompact.PrintCurrentSolution(2);

    cout << "\n************************************ Bilan ************************************\n";
    cout << "\n---- Recherche Cplex -----\n";
    double Cplex_CPUTime = EndCplex_CPUTime - BeginCplex_CPUTime;
    cout << "Temps CPU:         " << Cplex_CPUTime << "s\n";
    double Cplex_UserTime = EndCplex_UserTime - BeginCplex_UserTime;
    cout << "Temps utilisateur: " << Cplex_UserTime << "s\n";
    cout << "\n---- Recherche locale ----\n";
    double LocalSearch_CPUTime = EndLocalSearch_CPUTime - BeginLocalSearch_CPUTime;
    cout << "Temps CPU:         " << LocalSearch_CPUTime << "s\n";
    double LocalSearch_UserTime = EndLocalSearch_UserTime - BeginLocalSearch_UserTime;
    cout << "Temps utilisateur: " << LocalSearch_UserTime << "s\n";
    cout << "\n";

  }
  else
    cout << "Aucune solution admissible trouvee par Cplex en moins de 7 secondes.\n";
}


