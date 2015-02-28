#include "ColumnGeneration.h"

#include "timetools.h"
#include "KnapSack.h"
#include <iostream>
#include <vector>

#include <omp.h>

using namespace std;


void ColumnGenerationAlgorithm(ModelMaitre & iMaster, ModelCompact & iCompact)
{
  // Solveur du modele maitre
  IloCplex cplexMaster(iMaster._Model);
  cplexMaster.setOut(iMaster._Env.getNullStream());
  cplexMaster.setParam(IloCplex::CutUp, iCompact._ActualCost + CST_EPS);

  // Modele auxiliaire
  vector< vector<double> > a( iCompact._m, vector<double>(iCompact._n, 0) );  // Vecteurs des coefficients des contraintes
  for (int j = 0; j < iCompact._m; j++)
    for (int i = 0; i < iCompact._n; i++)
      a[j][i] = iCompact._a[j][i];

  int NbOfIterations = 0;
  while(true)
  {
    NbOfIterations++;
    
    cplexMaster.solve();
    iMaster._ObjValue = cplexMaster.getObjValue();
    
    cplexMaster.setParam(IloCplex::CutUp, iMaster._ObjValue + CST_EPS);
    
    if (NbOfIterations%100 == 0)
      cout << "Cout a la " << NbOfIterations << "e iteration : " << iMaster._ObjValue << "\n";

    IloNumArray valDualEqual(iMaster._Env);
    cplexMaster.getDuals(valDualEqual, iMaster._ConstrEqual);
    IloNumArray valDualInequal(iMaster._Env);
    cplexMaster.getDuals(valDualInequal, iMaster._ConstrInequal);
    int NbReductCostPositive = 0;
    
    iMaster.RemoveColumn(cplexMaster);

    IloEnv env = iMaster._Env;
    int j;
    #pragma omp parallel for schedule(dynamic) private(j, env)
    for(j = 0; j < iCompact._m; j++)
    {
      int Idx = j;

      vector<double> CoefConstrAux(iCompact._n, 0);  // Vecteur des couts de l'objectif
      for (int i = 0; i < iCompact._n; i++)
        CoefConstrAux[i] = valDualEqual[i] - iCompact._c[Idx][i];
      IloNumArray vals(env, iCompact._n);
      double ObjAuxOpt = knapSack((double)iCompact._b[Idx], a[Idx], CoefConstrAux, iCompact._n, vals);
      if (-ObjAuxOpt < valDualInequal[Idx])
      {
        IloInt Cout(0);
        for (int i = 0; i < iCompact._n; i++){
          Cout+=vals[i]*iCompact._c[Idx][i];
        }
        #pragma omp critical
        {
          iMaster._Colonnes[Idx].add(IloNumVar( iMaster._Objectif(Cout) + iMaster._ConstrEqual(vals) + iMaster._ConstrInequal[Idx](1) ));
        }
      }
      else
      {
        #pragma omp critical
        {
          NbReductCostPositive++;
        }
      }
      vals.end();
    }
    
    valDualEqual.end();
    valDualInequal.end();
    
    if (NbReductCostPositive==iCompact._m)
      break;
    
  }

  cplexMaster.solve();
  double ObjMasterOpt = cplexMaster.getObjValue();
  cout << "Valeur de l'optimum obtenu par generation de colonnes : " << ObjMasterOpt << "\n";
  for(int j = 0; j < iCompact._m; j++)
  {
    IloNumArray vals(iMaster._Env);
    cplexMaster.getValues(vals, iMaster._Colonnes[j]);
    cout << "Machine " << j << "\n";
    PrintArray(vals);
    vals.end();
  }

}


void ColumnGeneration(ModelMaitre & iMaster, ModelCompact & iCompact)
{
  cout << "*************************** Generation de colonnes ****************************\n";
  cout << "\n----- Iterations et valeur de l'objectif -----\n";
  double BeginColumnGeneration_UserTime = get_wall_time();
  double BeginColumnGeneration_CPUTime = get_cpu_time();
  ColumnGenerationAlgorithm(iMaster, iCompact);
  double EndColumnGeneration_CPUTime = get_cpu_time();
  double EndColumnGeneration_UserTime = get_wall_time();

  cout << "\n-------------- Temps de calcul ---------------\n";
  double ColumnGeneration_CPUTime = EndColumnGeneration_CPUTime - BeginColumnGeneration_CPUTime;
  cout << "Temps CPU:\t" << ColumnGeneration_CPUTime << "s\n";
  double ColumnGeneration_UserTime = EndColumnGeneration_UserTime - BeginColumnGeneration_UserTime;
  cout << "Temps utilisateur:\t" << ColumnGeneration_UserTime << "s\n";
  cout << "\n";
}
