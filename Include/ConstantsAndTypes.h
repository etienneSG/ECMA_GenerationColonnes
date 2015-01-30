#ifndef CONSTANTSANDTYPE_H
#define CONSTANTSANDTYPE_H

#include <ilcplex/ilocplex.h>

typedef IloArray<IloNumArray> NumMatrix;
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<NumMatrix> NumMatrix3d;

/* Affiche le contenu des matrices */
void PrintArray(NumMatrix &iNumMatrix);
void PrintArray(BoolVarMatrix &iBoolVarMatrix);
void PrintArray(NumMatrix3d &iNumMatrix3d);
void PrintArray(IloNumArray &iNumArray);

#endif


