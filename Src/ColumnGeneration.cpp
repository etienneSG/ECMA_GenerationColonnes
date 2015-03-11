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

  ResolutionOfAuxProgramsByDynProg(iMaster, iCompact, cplexMaster);
  
  //ResolutionOfAuxProgramsByCplex(iMaster, iCompact, cplexMaster);
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


void ResolutionOfAuxProgramsByDynProg(ModelMaitre & iMaster, ModelCompact & iCompact, IloCplex & iCplexMaster)
{
  cout << "\n---- Resolution des programmes auxiliaires par programmation dynamique ----\n";
  
  // Modele auxiliaire
  vector< vector<int> > a( iCompact._m, vector<int>(iCompact._n, 0) );  // Vecteurs des coefficients des contraintes
  for (int j = 0; j < iCompact._m; j++)
    for (int i = 0; i < iCompact._n; i++)
      a[j][i] = iCompact._a[j][i];
  vector<int> b( iCompact._m, 0);
  for (int j = 0; j < iCompact._m; j++) // Vecteur des capacites des machines
    b[j] = iCompact._b[j];

  int NbOfIterations = 0;
  while(true)
  {
    NbOfIterations++;
    
    iCplexMaster.solve();
    iMaster._ObjValue = iCplexMaster.getObjValue();
    
    iCplexMaster.setParam(IloCplex::CutUp, iMaster._ObjValue + CST_EPS);
    
    if (NbOfIterations%100 == 0)
      cout << "Cout a la " << NbOfIterations << "e iteration : " << iMaster._ObjValue << "\n";

    IloNumArray valDualEqual(iMaster._Env);
    iCplexMaster.getDuals(valDualEqual, iMaster._ConstrEqual);
    IloNumArray valDualInequal(iMaster._Env);
    iCplexMaster.getDuals(valDualInequal, iMaster._ConstrInequal);
    int NbReductCostPositive = 0;
    
    iMaster.RemoveColumn(iCplexMaster);

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
      vector<int> Solution(iCompact._n, 0);
      double ObjAuxOpt = knapSack<double>(b[Idx], a[Idx], CoefConstrAux, iCompact._n, Solution);
      for (int i = 0; i < iCompact._n; i++)
        vals[i] = Solution[i];
      if (-ObjAuxOpt < valDualInequal[Idx] - CST_EPS)
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

  iCplexMaster.solve();
  double ObjMasterOpt = iCplexMaster.getObjValue();
  cout << "Valeur de l'optimum obtenu par generation de colonnes : " << ObjMasterOpt << "\n";
  for(int j = 0; j < iCompact._m; j++)
  {
    IloNumArray vals(iMaster._Env);
    iCplexMaster.getValues(vals, iMaster._Colonnes[j]);
    cout << "Machine " << j << "\n";
    PrintArray(vals);
    vals.end();
  }
}


void ResolutionOfAuxProgramsByCplex(ModelMaitre & iMaster, ModelCompact & iCompact, IloCplex & iCplexMaster)
{
  cout << "\n---------- Resolution des programmes auxiliaires par Cplex -----------\n";
  
  // Modele auxiliaire
  IloModel ModelAux(iMaster._Env);
  IloBoolVarArray z(iMaster._Env, iCompact._n);
  IloObjective ObjAux = IloAdd(ModelAux, IloMinimize(iMaster._Env,1));
  IloRange ConstrAux = IloAdd(ModelAux, IloScalProd(IloNumArray(iMaster._Env, iCompact._n) , z) <= 0);
  IloCplex cplexAux(ModelAux);
  cplexAux.setOut(iMaster._Env.getNullStream());

  int NbOfIterations = 0;
  while(true)
  {
    NbOfIterations++;
    
    iCplexMaster.solve();
    iCplexMaster.setParam(IloCplex::CutUp, iCplexMaster.getObjValue() + CST_EPS);
    
    iMaster._ObjValue = iCplexMaster.getObjValue();
    if (NbOfIterations%100 == 0)
      cout << "Cout a la " << NbOfIterations << "e iteration : " << iMaster._ObjValue << "\n";

    IloNumArray valDualEqual(iMaster._Env);
    iCplexMaster.getDuals(valDualEqual, iMaster._ConstrEqual);
    IloNumArray valDualInequal(iMaster._Env);
    iCplexMaster.getDuals(valDualInequal, iMaster._ConstrInequal);
    int NbReductCostPositive = 0;
    
    iMaster.RemoveColumn(iCplexMaster);

    for(int j = 0; j < iCompact._m; j++)
    {
      IloNumArray CoefConstrAux(iMaster._Env, iCompact._n);
      for (int i = 0; i < iCompact._n; i++)
        CoefConstrAux[i] = iCompact._c[j][i] - valDualEqual[i];
      ObjAux.setExpr(IloScalProd(CoefConstrAux, z));
      ConstrAux.setExpr(IloScalProd(iCompact._a[j], z));
      ConstrAux.setUB(iCompact._b[j]);
      
      cplexAux.solve();

      double ObjAuxOpt = cplexAux.getObjValue();
      if (ObjAuxOpt < valDualInequal[j] - CST_EPS)
      {
        IloNumArray vals(iMaster._Env);
        cplexAux.getValues(vals, z);
        IloInt Cout(0);
        for (int i = 0; i < iCompact._n; i++){
          Cout+=vals[i]*iCompact._c[j][i];
        }
        iMaster._Colonnes[j].add(IloNumVar( iMaster._Objectif(Cout) + iMaster._ConstrEqual(vals) + iMaster._ConstrInequal[j](1) ));
      }
      else
        NbReductCostPositive++;
    }
    
    if (NbReductCostPositive==iCompact._m)
      break;
    
  }

  iCplexMaster.solve();
  double ObjMasterOpt = iCplexMaster.getObjValue();
  cout << "Valeur de l'optimum obtenu par generation de colonnes : " << ObjMasterOpt << "\n";
  for(int j = 0; j < iCompact._m; j++)
  {
    IloNumArray vals(iMaster._Env);
    iCplexMaster.getValues(vals, iMaster._Colonnes[j]);
    cout << "Machine " << j << "\n";
    PrintArray(vals);
  }

  ModelAux.end();
}
