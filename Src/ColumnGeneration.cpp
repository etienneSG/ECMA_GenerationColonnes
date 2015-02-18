#include "ColumnGeneration.h"

#include <iostream>

using namespace std;


void ColumnGeneration(ModelMaitre & iMaster, ModelCompact & iCompact)
{
  // Solveur du modele maitre
  IloCplex cplexMaster(iMaster._Model);

  // Modele auxiliaire
  IloModel ModelAux(iMaster._Env);
  IloBoolVarArray z(iMaster._Env, iCompact._n);
  IloObjective ObjAux = IloAdd(ModelAux, IloMinimize(iMaster._Env,1));
  IloRange ConstrAux = IloAdd(ModelAux, IloScalProd(IloNumArray(iMaster._Env, iCompact._n) , z) <= 0);
  IloCplex cplexAux(ModelAux);
  cplexAux.setOut(iMaster._Env.getNullStream());

  while(true)
  {
    cplexMaster.solve();
    
    IloNumArray valDualEqual(iMaster._Env);
    cplexMaster.getDuals(valDualEqual, iMaster._ConstrEqual);
    IloNumArray valDualInequal(iMaster._Env);
    cplexMaster.getDuals(valDualInequal, iMaster._ConstrInequal);
    int NbReductCostPositive = 0;

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
      //cout << "Valeur optimale du probleme auxiliaire " << j << ": " << ObjAuxOpt - valDualInequal[j] << "\n";
      if (ObjAuxOpt < valDualInequal[j])
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

  double ObjMasterOpt = cplexMaster.getObjValue();
  cout << "Valeur de l'optimum obtenu par generation de colonnes : " << ObjMasterOpt << "\n";

  //------------------------------

  ModelAux.end();

}

