#include "ModelMaitre.h"
#include "ModelCompact.h"
#include "ConstantsAndTypes.h"

#include <vector>
#include <math.h>
#include <stdlib.h> 
#include <../../opt/ibm/ILOG/CPLEX_Studio125/concert/include/ilconcert/iloenv.h>

using namespace std;

struct Affectation
{
  Affectation(int iJ=0, int iI=0, double iRC=0): _j(iJ), _i(iI), _RC(iRC) {}
  int _j; // machine
  int _i; // tache
  double _RC; // Cout reduit de l'affectation
};


int partition(vector<Affectation>& A, int iBegin, int iEnd)
{
  int x = A[iBegin]._i;
  int i = iBegin;
  int j;

  for (j = iBegin+1; j < iEnd; j++) {
    if (A[j]._i <= x) {
      i = i+1;
      swap(A[i], A[j]);
    }
  }

  swap(A[i], A[iBegin]);
  return i;
}

void SortAffectation(vector<Affectation>& A, int iBegin, int iEnd)
{
  int pivot;
  if (iBegin < iEnd)
  {
    pivot = partition(A, iBegin, iEnd);
    SortAffectation(A, iBegin, pivot);  
    SortAffectation(A, pivot+1, iEnd);
  }
}


int partitionByReducedCost(vector<Affectation>& A, int iBegin, int iEnd)
{
  int x = A[iBegin]._RC;
  int i = iBegin;
  int j;

  for (j = iBegin+1; j < iEnd; j++) {
    if (A[j]._RC >= x) {
      i = i+1;
      swap(A[i], A[j]);
    }
  }

  swap(A[i], A[iBegin]);
  return i;
}

void SortAffectationByReducedCost(vector<Affectation>& A, int iBegin, int iEnd)
{
  int pivot;
  if (iBegin < iEnd)
  {
    pivot = partitionByReducedCost(A, iBegin, iEnd);
    SortAffectation(A, iBegin, pivot);  
    SortAffectation(A, pivot+1, iEnd);
  }
}




ModelMaitre::ModelMaitre(IloEnv iEnv,  ModelCompact & iCompact)
: _Env(iEnv),
  _Model(iEnv),
  _CoutColonne(iEnv),
  _IsTacheInColonne(iEnv),
  _pCompact(&iCompact),
  _Colonnes(iEnv),
  _ObjValue(_pCompact->_ActualCost)
{
  for (int i = 0; i < iCompact._m; i++)
  {
    _CoutColonne.add(IloIntArray(iEnv));
    _IsTacheInColonne.add(IntMatrix(iEnv));
    _Colonnes.add(IloNumVarArray(iEnv));
  }

  _Objectif = IloAdd(_Model, IloMinimize(iEnv));

  //Contraintes probleme maitre
  _ConstrEqual = IloAdd(_Model, IloRangeArray(iEnv, iCompact._n, 1, 1));
  _ConstrInequal = IloAdd(_Model, IloRangeArray(iEnv, iCompact._m, 0, 1));
}


ModelMaitre::~ModelMaitre()
{
  _pCompact = 0;
  _Objectif.end();
  _ConstrEqual.end();
  _ConstrInequal.end();
}


void ModelMaitre::RemoveColumn(const IloCplex & iCplex)
{
  // Taux de suppression des colonnes inutilisees
  int SuppRate = (_pCompact->_n+_pCompact->_m)/2;
  
  vector<Affectation> UselessColumn;
//   int NbVar = 0;
  for (int j = 0; j < _pCompact->_m; j++)
  {
    IloNumArray vals(_Env);
    iCplex.getValues(vals, _Colonnes[j]);
    int NbColumn = vals.getSize();
//     NbVar += NbColumn;
    for (int i = 0; i < NbColumn-1; i++) {
      double RC = iCplex.getReducedCost(_Colonnes[j][i]);
      if (fabs(vals[i]) < CST_EPS && RC >= 0)
        UselessColumn.push_back(Affectation(j,i,RC));
    }
  }
  int NbUseless = UselessColumn.size();
  int NbToRemove = UselessColumn.size() / SuppRate;
//   cout << "Colonnes inutiles : " << NbUseless << "\r";
  if (NbToRemove > 0)
  {
    SortAffectationByReducedCost(UselessColumn, 0, NbUseless);

    SortAffectation(UselessColumn, 0, NbToRemove);
    for (int k = NbToRemove-1; k >= 0; k--) {
//       cout << "Colonne a supprimer : " << UselessColumn[k]._j << " " << UselessColumn[k]._i << "\n";
      _Colonnes[ UselessColumn[k]._j ][ UselessColumn[k]._i ].end();
      _Colonnes[ UselessColumn[k]._j ].remove( UselessColumn[k]._i );
      
    }
  }
}
