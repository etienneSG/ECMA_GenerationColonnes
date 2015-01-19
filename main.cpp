#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library
#include "ReadData.h"
#include "ConstantsAndTypes.h"

using namespace std;

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int main (int argc, char const *argv[])
{
  IloEnv env;
  try {

    // parametres formulation compacte
    IloModel ModelCompact(env);
    NumMatrix c(env);
    NumMatrix a(env);
    IloIntArray b(env);
    // Lecture du fichier de donnees
    ReadData("GAP/GAP-a05100.dat", ModelCompact, c, a, b);
    int NbMachines=c.getSize();
    int NbTaches=c[0].getSize();
    // creation des variables formulation compacte
    BoolVarMatrix x(env);
    for (int i=0;i<NbMachines;i++){
      IloBoolVarArray E(env);
      for (int j=0;j<NbTaches;j++){
        E.add(IloBoolVar(env));
      }
      x.add(E);
    }
    // Creation des contraintes formulation compacte
    IloRangeArray constrcompacte(env);
    for (int i=0;i<NbMachines;i++){
      IloExpr Capacite(env);
      for (int j=0;j<NbTaches;j++){
        Capacite+=a[i][j]*x[i][j];
      }
      constrcompacte.add(Capacite<=b[i]);
      Capacite.end();
    }
    for (int i=0;i<NbTaches;i++){
      IloExpr UniciteAffectation(env);
      for (int j=0;j<NbMachines;j++){
        UniciteAffectation+=x[j][i];
      }
      constrcompacte.add(UniciteAffectation == 1);
      UniciteAffectation.end();
    }
    ModelCompact.add(constrcompacte);
    //Objectif formulation compacte
    IloExpr ObjectifCompacte(env);
    for (int i=0;i<NbMachines;i++){
      for (int j=0;j<NbTaches;j++){
        ObjectifCompacte+=c[i][j]*x[i][j];
      }
    }
    ModelCompact.add(IloMinimize(env,ObjectifCompacte));
    ObjectifCompacte.end();


    // Paramètres probleme maitre
    IloModel ModelMaitre(env);
    NumMatrix CoutColonne(env);
    NumMatrix3d IsTacheInColonne(env);
    // Varialbes probleme maitre
    BoolVarMatrix Colonne(env);
    //Contraintes probleme maitre
    IloRangeArray constrmaitre(env);
    for (int k=0;k<NbTaches;k++){
      IloExpr Egalite(env);
      for (int i=0;i<CoutColonne.getSize();i++){
        for (int j=0;j<CoutColonne[i].getSize();j++){
          Egalite+=IsTacheInColonne[i][j]*Colonne[i][j];
        }
      }
      constrmaitre.add(Egalite == 1);
      Egalite.end();
    }

    for (int i=0;i<CoutColonne.getSize();i++){
      IloExpr Inegalite(env);
      for (int j=0;j<CoutColonne[i].getSize();i++){
          Inegalite+=Colonne[i][j];
        }
      constrmaitre.add(Inegalite <= 1);
     Inegalite.end();
    }
    ModelMaitre.add(constrmaitre);

    //Objectif probleme maitre
    IloExpr ObjectifMaitre(env);
    for (int i=0;i<CoutColonne.getSize();i++){
      for (int j=0;j<CoutColonne[i].getSize();i++){
        ObjectifMaitre+=CoutColonne[i][j]*Colonne[i][j];
      }
    }
    ModelMaitre.add(IloMinimize(env,ObjectifMaitre));
    ObjectifMaitre.end();


    //------------------------------
    // Generation de colonnes

    //determination des colonnes initiales
    IloCplex cplex(ModelCompact);
    // cplex.IntParam.IntSolLim=1;
    cplex.solve();
    for (i =0;i<NbMachines;i++){
      IloIntArray vals(env);
      cplex.getValues(vals,x[i]);
     
      Colonne[i].add(IloBoolVar(env));
      
      IsTacheInColonne[i].add(vals);
     
      IloInt Cout(env);
      for (j=0;j<NbTaches;j++){
        Cout+=vals[j]*c[i][j];
      }
      CoutColonne[i].add(Cout);

      //------------------------------


    }
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


