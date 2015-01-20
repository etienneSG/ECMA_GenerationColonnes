#include "ConstantsAndTypes.h"

#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library


using namespace std;

void PrintArray(NumMatrix &iNumMatrix)
{
  int n = iNumMatrix.getSize();
  for (int i = 0; i < n; i++)
  {
    int m = iNumMatrix[i].getSize();
    for (int j = 0; j < m; j++)
      cout << iNumMatrix[i][j] << "\t";
    cout << "\n";
  }
}


void PrintArray(BoolVarMatrix &iBoolVarMatrix)
{
  int n = iBoolVarMatrix.getSize();
  for (int i = 0; i < n; i++)
  {
    int m = iBoolVarMatrix[i].getSize();
    for (int j = 0; j < m; j++)
      cout << iBoolVarMatrix[i][j] << "\t";
    cout << "\n";
  }
}

void PrintArray(NumMatrix3d &iNumMatrix3d)
{
  int n = iNumMatrix3d.getSize();
  for (int i = 0; i < n; i++)
  {
    cout << "Matrice " << i << "\n";
    PrintArray(iNumMatrix3d[i]);
  }
}


