#ifndef CONSTANTSANDTYPE_H
#define CONSTANTSANDTYPE_H

#include <ilcplex/ilocplex.h>

typedef IloArray<IloIntArray> IntMatrix;
typedef IloArray<IloNumArray> NumMatrix;
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<IntMatrix> IntMatrix3d;
typedef IloArray<NumMatrix> NumMatrix3d;

/** Affiche le contenu des matrices */
void PrintArray(IloIntArray &iIntArray);
void PrintArray(IloNumArray &iNumArray);
void PrintArray(IntMatrix &iIntMatrix);
void PrintArray(NumMatrix &iNumMatrix);
void PrintArray(BoolVarMatrix &iBoolVarMatrix);
void PrintArray(NumVarMatrix &NumVarMatrix);
void PrintArray(IntMatrix3d &iIntMatrix3d);
void PrintArray(NumMatrix3d &iNumMatrix3d);

/** Constantes du programme */
#define CST_EPS 1e-7

#ifdef __linux__
#include <omp.h>
#define NB_PROC omp_get_num_procs()
#endif

/** Pour limiter le temps de la recherche locale */
//#define MAX_TIME_FOR_LOCAL_SEARCH 60

#endif


