#include "ConstantsAndTypes.h"

#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library


using namespace std;


int PenaltyCost(int iReal, int iLimit)
{
  return (iReal > iLimit ? 50*(iReal-iLimit) : 0) ;
}


void PrintArray(IntMatrix &iIntMatrix)
{
  int n = iIntMatrix.getSize();
  for (int i = 0; i < n; i++)
  {
    int m = iIntMatrix[i].getSize();
    for (int j = 0; j < m; j++)
      cout << iIntMatrix[i][j] << "\t";
    cout << "\n";
  }
}


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


void PrintArray(NumVarMatrix &iNumVarMatrix)
{
  int n = iNumVarMatrix.getSize();
  for (int i = 0; i < n; i++)
  {
    int m = iNumVarMatrix[i].getSize();
    for (int j = 0; j < m; j++)
      cout << iNumVarMatrix[i][j] << "\t";
    cout << "\n";
  }
}


void PrintArray(IntMatrix3d &iIntMatrix3d)
{
  int n = iIntMatrix3d.getSize();
  for (int i = 0; i < n; i++)
  {
    cout << "Matrice " << i << "\n";
    PrintArray(iIntMatrix3d[i]);
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

void PrintArray(IloNumArray &iNumArray)
{
  int n = iNumArray.getSize();
  for (int i = 0; i < n; i++)
    cout << iNumArray[i] << "\t";
  cout << "\n";
}

void PrintArray(IloIntArray &iIntArray)
{
  int n = iIntArray.getSize();
  for (int i = 0; i < n; i++)
    cout << iIntArray[i] << "\t";
  cout << "\n";
}

