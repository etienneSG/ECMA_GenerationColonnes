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
  std::string Argument2 = argc>2 ? argv[2] : "LocalSearch";
  
  IloEnv env;
  try {

    // parametres formulation compacte
    ModelCompact myCompact(Argument1, env);

    // Creation de l'objectif et des contraintes
    myCompact.CreateObjectiveAndConstraintes();
    
    if (Argument2 == "ColumnGen")
    {
      ModelMaitre myMaster(env, myCompact);     // Creation du modele maitre
      myCompact.FindFeasableSolution(myMaster); // Determination des colonnes initiales
      ColumnGeneration(myMaster, myCompact);    // Generation de colonnes
    }
    else if (Argument2 ==  "LocalSearch")
    {
      LocalSearch(myCompact);                   // Recherche locale d'une meilleure solution
    }
    else if (Argument2 ==  "SpecificLocalSearch")
    {
      SpecificLocalSearch(myCompact);           // Recherche locale a partir d'une "bonne" solution Cplex
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


