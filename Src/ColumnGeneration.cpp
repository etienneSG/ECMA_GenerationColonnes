#include "ColumnGeneration.h"

#include "timetools.h"
#include <iostream>

using namespace std;


void ColumnGenerationAlgorithm(ModelMaitre & iMaster, ModelCompact & iCompact)
{
  // Solveur du modele maitre
  IloCplex cplexMaster(iMaster._Model);
  cplexMaster.setOut(iMaster._Env.getNullStream());
  cplexMaster.setParam(IloCplex::CutUp, iCompact._ActualCost + CST_EPS);

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
    
    cplexMaster.solve();
    cplexMaster.setParam(IloCplex::CutUp, cplexMaster.getObjValue() + CST_EPS);
    
    iMaster._ObjValue = cplexMaster.getObjValue();
    if (NbOfIterations%100 == 0)
      cout << "Cout a la " << NbOfIterations << "e iteration : " << iMaster._ObjValue << "\n";

    IloNumArray valDualEqual(iMaster._Env);
    cplexMaster.getDuals(valDualEqual, iMaster._ConstrEqual);
    IloNumArray valDualInequal(iMaster._Env);
    cplexMaster.getDuals(valDualInequal, iMaster._ConstrInequal);
    int NbReductCostPositive = 0;
    
    iMaster.RemoveColumn(cplexMaster);

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

  cplexMaster.solve();
  double ObjMasterOpt = cplexMaster.getObjValue();
  cout << "Valeur de l'optimum obtenu par generation de colonnes : " << ObjMasterOpt << "\n";
  for(int j = 0; j < iCompact._m; j++)
  {
    IloNumArray vals(iMaster._Env);
    cplexMaster.getValues(vals, iMaster._Colonnes[j]);
    cout << "Machine " << j << "\n";
    PrintArray(vals);
  }

  ModelAux.end();

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
