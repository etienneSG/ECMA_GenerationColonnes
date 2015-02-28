#include "KnapSack.h"
#include "ConstantsAndTypes.h"
#include <algorithm>
#include <stdio.h>
#include <math.h>

using namespace std;


int knapSack(double W, vector<double> wt, vector<double> val, int n, IloNumArray oSol)
{
  int i, w;
  vector< vector<double> > K(n+1, vector<double>(W+1,0));
 
  // Build table K[][] in bottom up manner
  for (i = 0; i <= n; i++)
  {
    for (w = 0; w <= W; w++)
    {
      if (i==0 || w==0)
        K[i][w] = 0;
      else if (wt[i-1] <= w)
        K[i][w] = max(val[i-1] + K[i-1][w-wt[i-1]],  K[i-1][w]);
      else
        K[i][w] = K[i-1][w];
    }
  }

  // Creation of the object list
  i = n; w = W;
  while (fabs(K[i][w] - K[i][w-1]) < CST_EPS)
    w--;
  while (w > 0)
  {
    while (i > 0 && fabs(K[i][w]-K[i-1][w]) < CST_EPS) {
      oSol[i-1] = 0;
      i--;
    }
    w -= wt[i-1];
    oSol[i-1] = 1;
    i--;
  }

  return K[n][W];
}
