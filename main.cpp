#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library
#include <time.h>    //time
#include "ModelCompact.h"
#include "ModelMaitre.h"
#include "ConstantsAndTypes.h"
#include "LocalSearch.h"
#include "ColumnGeneration.h"


using namespace std;

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int main (int argc, char const *argv[])
{
  /* Initilisation de l'aleatoire */
  srand (time(NULL));
  
  std::string Argument1 = argc>1 ? argv[1] : "GAP/GAP-test.dat";

  IloEnv env;
  try {

    // parametres formulation compacte
    ModelCompact myCompact(Argument1, env);

    // Creation de l'objectif et des contraintes
    myCompact.CreateObjectiveAndConstraintes();
    
    // Creation du modele maitre
    ModelMaitre myMaster(env, myCompact);

    //determination des colonnes initiales
    myCompact.FindFeasableSolution(myMaster);
    
    // Recherche locale d'une meilleure solution
    LocalSearch(myCompact);
    
    // Generation de colonnes
    //ColumnGeneration(myMaster, myCompact);
    
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


