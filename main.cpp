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
    IloModel model(env);
    NumMatrix c(env);
    NumMatrix a(env);
    IloIntArray b(env);

    ReadData("GAP/GAP-a05100.dat", model, c, a, b);

    int m = b.getSize();
    cout << m << "\n";
    for (int idx_l = 0; idx_l < m; idx_l++)
    {
      cout << b[idx_l] << " ";
      cout << "\n";
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


