#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library
#include "ModelCompact.h"
#include "ConstantsAndTypes.h"
#include "LocalSearch.h"


using namespace std;

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int main (int argc, char const *argv[])
{
  std::string Argument1 = argc>1 ? argv[1] : "GAP/GAP-a10100.dat";

  IloEnv env;
  try {

    // parametres formulation compacte
    ModelCompact myCompact(Argument1, env);

    // creation des variables formulation compacte
    BoolVarMatrix x(env);
    for (int i = 0; i < myCompact._m; i++){
      IloBoolVarArray E(env);
      for (int j = 0; j < myCompact._n; j++){
        E.add(IloBoolVar(env));
      }
      x.add(E);
    }
    // Creation des contraintes formulation compacte
    IloRangeArray constrcompacte(env);
    for (int i = 0; i < myCompact._m; i++){
      IloExpr Capacite(env);
      for (int j = 0; j < myCompact._n; j++){
        Capacite+=myCompact._a[i][j]*x[i][j];
      }
      constrcompacte.add( Capacite<=myCompact._b[i] );
      Capacite.end();
    }
    for (int i = 0; i < myCompact._n; i++){
      IloExpr UniciteAffectation(env);
      for (int j = 0; j < myCompact._m; j++){
        UniciteAffectation+=x[j][i];
      }
      constrcompacte.add( UniciteAffectation == 1 );
      UniciteAffectation.end();
    }
    myCompact._Model.add(constrcompacte);
    //Objectif formulation compacte
    IloExpr ObjectifCompacte(env);
    for (int i = 0; i < myCompact._m; i++) {
      for (int j = 0; j < myCompact._n ; j++) {
        ObjectifCompacte+=myCompact._c[i][j]*x[i][j];
      }
    }
    myCompact._Model.add( IloMinimize(env, ObjectifCompacte) );
    ObjectifCompacte.end();


    // Paramètres probleme maitre
    IloModel ModelMaster(env);
    IntMatrix CoutColonne(env);
    IntMatrix3d IsTacheInColonne(env);
    // Variables probleme maitre
    BoolVarMatrix Colonne(env);
    for (int i = 0; i < myCompact._m; i++) {
      CoutColonne.add(IloIntArray(env));
      IsTacheInColonne.add(IntMatrix(env));
      Colonne.add(IloBoolVarArray(env));
    }

    //Objectif probleme maitre
    IloObjective ObjectifMaster = IloAdd(ModelMaster, IloMinimize(env));

    //Contraintes probleme maitre
    IloRangeArray ConstrMasterEqual = IloAdd(ModelMaster, IloRangeArray(env, myCompact._n, 1, 1));
    IloRangeArray ConstrMasterInequal = IloAdd(ModelMaster, IloRangeArray(env, myCompact._m, 0, 1));


    //------------------------------
    // Generation de colonnes

    //determination des colonnes initiales
    IloCplex cplexCompact(myCompact._Model);

    cplexCompact.setParam(IloCplex::IntSolLim, 1); // Valeur par defaut : 2100000000 (arret apres la premiere solution entiere)
    cplexCompact.setParam(IloCplex::NodeSel, 0);   // Valeur par defaut : 1 (parcours en profondeur)
    cplexCompact.solve();
    for (int j = 0; j < myCompact._m; j++){
      IloNumArray vals(env);
      cplexCompact.getValues(vals,x[j]);
      cout << "Affichage de la machine " << j << "\n";
      PrintArray(vals);
      myCompact.InsertSolutionOnMachine(vals, j);

      IloInt Cout(0);
      for (int i = 0; i < myCompact._n; i++){
        Cout+=vals[i]*myCompact._c[j][i];
      }

      Colonne[j].add(IloBoolVar( ObjectifMaster(Cout) + ConstrMasterEqual(vals) + ConstrMasterInequal[j](1) ));
      
      //IsTacheInColonne[j].add(vals);
     
      //CoutColonne[j].add(Cout);
    }

    LocalSearch(myCompact);
    
    //cout << "Affichage de l'objectif\n";
    //cout << cplexCompact.getObjective() << "\n";

    //IloNum ObjCompact(0);
    //cplexCompact.getObjValue(ObjCompact);
    //cout << "Valeur de l'objectif : " << ObjCompact << "\n";

    //cout << "Affichage de IsTacheInColonne\n";
    //PrintArray(IsTacheInColonne);
    //cout << "Affichage des couts de chaque colonne\n";
    //PrintArray(CoutColonne);
    cout << "Affichage des colonnes\n";
    PrintArray(Colonne);
    cout << "Affichage de l'objectif\n";
    //cout << ObjectifMaster.getExpr() << "\n";
    
    // Solveur du modele maitre
    IloCplex cplexMaster(ModelMaster);

    // Modele auxiliaire
    IloModel ModelAux(env);
    IloBoolVarArray z(env, myCompact._n);
    IloObjective ObjAux = IloAdd(ModelAux, IloMaximize(env,1));
    IloRange ConstrAux = IloAdd(ModelAux, IloScalProd(IloNumArray(env, myCompact._n) , z) <= 0);
    IloCplex cplexAux(ModelAux);

    cout << "********** bouh1 **********\n";

    while(true)
    {
      cplexMaster.solve();
      cplexMaster.solveFixed();
      IloNumArray valDualEqual(env);
      cplexMaster.getDuals(valDualEqual, ConstrMasterEqual);
      IloNumArray valDualInequal(env);
      cplexMaster.getDuals(valDualInequal, ConstrMasterInequal);
      int NbReductCostPositive = 0;

      cout << "********** bouh2 **********\n";

      for(int j = 0; j < myCompact._m; j++)
      {
        IloNumArray CoefConstrAux(env, myCompact._n);
        for (int i = 0; i < myCompact._n; i++)
          CoefConstrAux[i] = myCompact._c[j][i] - valDualEqual[i];
        ObjAux.setExpr(IloScalProd(CoefConstrAux, z));
        ConstrAux.setExpr(IloScalProd(myCompact._a[j], z));
        ConstrAux.setUB(myCompact._b[j]);
        //for(int i = 0; i < myCompact._n; i++)
        //  ObjAux += (myCompact._c[j][i] - valDualEqual[i])*z[i];
        //ModelAux.add(IloMinimize(env, ObjAux));
        //ObjAux.end();
        //IloExpr ConstrAux(env);
        //for(int i = 0; i < myCompact._n; i++)
        //  ConstrAux += myCompact._a[j][i]*z[i];
        //ModelAux.add(ConstrAux <= myCompact._b[j]);
        //ConstrAux.end();
        
        cplexAux.solve();

        IloNum ObjAuxOpt(0);
        cplexAux.getObjValue(ObjAuxOpt);
        cout << "Valeur optimale du probleme auxiliaire " << j << ": " << ObjAuxOpt - valDualInequal[j] << "\n";
        if (ObjAuxOpt < valDualInequal[j])
        {
          IloNumArray vals(env);
          cplexAux.getValues(vals, z);
          IloInt Cout(0);
          for (int i = 0; i < myCompact._n; i++){
            Cout+=vals[i]*myCompact._c[j][i];
          }
          Colonne[j].add(IloBoolVar( ObjectifMaster(Cout) + ConstrMasterEqual(vals) + ConstrMasterInequal[j](1) ));
        }
        else
          NbReductCostPositive++;
       
      }

      if (NbReductCostPositive==myCompact._m)
        break;
    }

    IloNum ObjMasterOpt(0);
    cplexMaster.getObjValue(ObjMasterOpt);
    cout << "Valeur de l'optimum obtenu par generation de colonnes : " << ObjMasterOpt << "\n";

    //------------------------------

    ModelAux.end();

    ConstrMasterInequal.end();
    ConstrMasterEqual.end();
    ObjectifMaster.end();

  }
  catch (IloException& e) {
    cerr << "Concert exception caught: " << e << endl;
  }
  catch (...) {
    cerr << "Unknown exception caught" << endl;
  }

  env.end();
  std::cout << "Well done !" << std::endl;
  return 0;

}  


