#ifndef CONSTANTSANDTYPE_H
#define CONSTANTSANDTYPE_H

#include <ilcplex/ilocplex.h>

typedef IloArray<IloIntArray> IntMatrix;
typedef IloArray<IloNumArray> NumMatrix;
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IntMatrix> IntMatrix3d;
typedef IloArray<NumMatrix> NumMatrix3d;

/** Affiche le contenu des matrices */
void PrintArray(IntMatrix &iIntMatrix);
void PrintArray(NumMatrix &iNumMatrix);
void PrintArray(BoolVarMatrix &iBoolVarMatrix);
void PrintArray(IntMatrix3d &iIntMatrix3d);
void PrintArray(NumMatrix3d &iNumMatrix3d);
void PrintArray(IloNumArray &iNumArray);

/** Constantes du programme */
#define CST_EPS 1e-7

#endif


