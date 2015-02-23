#include "ModelCompact.h"

#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <math.h>
#include "ModelCompactIterator.h"
#include "RandomIterator.h"
#include "timetools.h"

using namespace std;


/**
 * Lit un fichier de donnees au format OPL et remplit les attributs de la classe
 * @param iFile: fichier de donnees
 * @param model: Modele
 * @param m:     Nombre de machines
 * @param n:     Nombre de taches
 * @param c:     Matrice des couts d'affectation
 * @param a:     Matrice des capacites consommees
 * @param b:     Vecteur des capacites des machines
 */
void ReadData (string iFile, IloModel model, IntMatrix c, IloInt& m, IloInt& n, IntMatrix a, IloIntArray b)
{
  IloEnv env = model.getEnv();

  char * buffer = 0;
  long length;
  FILE * f = fopen (iFile.c_str(), "rb");
  
  // Lecture du fichier de données
  if (f)
  {
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = length ? new char[length] : 0;
    if (buffer)
    {
      fread (buffer, 1, length, f);
    }
    fclose (f);
  }
  else
  {
    std::cout << "Impossible d'ouvrir le fichier !" << std::endl;
    assert(false);
  }

  if (buffer)
  {
    // Recuperation des coefficients
    vector<int> data;
    int total_w = 0;
    int w;
    int i;
    while (buffer[total_w] != '=')
      total_w++;
    while (1 == sscanf(buffer + total_w, "%*[^0123456789]%d%n", &i, &w))
    {
      total_w += w;
      data.push_back(i);
    }

    // creation des parametres
    n = data[0];
    m = data[1];

    int idx = 2;
    for (int idx_l = 0; idx_l < m; idx_l++)
    {
      IloIntArray ligne(env);
      for (int idx_c = 0; idx_c < n; idx_c++)
      {
        ligne.add(data[idx]);
        idx++;
      }
      c.add(ligne);
    }
    for (int idx_l = 0; idx_l < m; idx_l++)
    {
      IloIntArray ligne(env);
      for (int idx_c = 0; idx_c < n; idx_c++)
      {
        ligne.add(data[idx]);
        idx++;
      }
      a.add(ligne);
    }
    for (int idx_l = 0; idx_l < m; idx_l++)
    {
      b.add(data[idx]);
      idx++;
    }
  }
  else
  {
    std::cout << "Fichier de donnees vide !" << std::endl;
    assert(false);
  }
  
  if (buffer)
    delete [] buffer; buffer = 0;
}


ModelCompact::ModelCompact()
: _NbOfCopy(*(new int(0)))
{}


ModelCompact::ModelCompact(std::string iFile, IloEnv iEnv):
  _Model(iEnv),
  _m(0),
  _n(0),
  _c(iEnv),
  _a(iEnv),
  _b(iEnv),
  _bvar(iEnv),
  _x(0),
  _ActualCost(0),
  _ActualCapacity(iEnv),
  _IsAdmissible(false),
  _NbOfCopy(*(new int(0)))
{
  // lecture des donnees
  ReadData(iFile, _Model, _c, _m, _n, _a, _b);
  
  // Initialisation de la solution vide
  _x = new int[_n];
  memset(_x, 0, _n*sizeof(int));
  _ActualCapacity.add(_m, 0);
  
  // creation des variables booleennes
  for (int i = 0; i < _m; i++){
    IloBoolVarArray E(_Model.getEnv());
    for (int j = 0; j < _n; j++){
      E.add(IloBoolVar(_Model.getEnv()));
    }
    _bvar.add(E);
  }
}


ModelCompact::ModelCompact(const ModelCompact & iCompact)
: _Model(iCompact._Model),
  _m(iCompact._m),
  _n(iCompact._n),
  _c(iCompact._c),
  _a(iCompact._a),
  _b(iCompact._b),
  _bvar(iCompact._Model.getEnv()),
  _x(0),
  _ActualCost(0),
  _ActualCapacity(iCompact._Model.getEnv()),
  _IsAdmissible(false),
  _NbOfCopy(iCompact._NbOfCopy)
{
  // Initialisation de la solution vide
  _x = new int[_n];
  memset(_x, 0, _n*sizeof(int));
  _ActualCapacity.add(_m, 0);
  
  // creation des variables booleennes
  for (int i = 0; i < _m; i++){
    IloBoolVarArray E(_Model.getEnv());
    for (int j = 0; j < _n; j++){
      E.add(IloBoolVar(_Model.getEnv()));
    }
    _bvar.add(E);
  }
  ++_NbOfCopy;
}


ModelCompact::~ModelCompact()
{
  if (_x)
    delete [] _x; _x = 0;
  if (_NbOfCopy > 0)
    --_NbOfCopy;
  else {
    _Model.end();
    delete &_NbOfCopy;
  }
}


void ModelCompact::CreateObjectiveAndConstraintes()
{
  // Creation des contraintes formulation compacte
  IloRangeArray constrcompacte(_Model.getEnv());
  for (int i = 0; i < _m; i++){
    IloExpr Capacite(_Model.getEnv());
    for (int j = 0; j < _n; j++){
      Capacite+=_a[i][j]*_bvar[i][j];
    }
    constrcompacte.add( Capacite<=_b[i] );
    Capacite.end();
  }
  
  for (int i = 0; i < _n; i++){
    IloExpr UniciteAffectation(_Model.getEnv());
    for (int j = 0; j < _m; j++){
      UniciteAffectation+=_bvar[j][i];
    }
    constrcompacte.add( UniciteAffectation == 1 );
    UniciteAffectation.end();
  }
  _Model.add(constrcompacte);
  
  //Objectif formulation compacte
  IloExpr ObjectifCompacte(_Model.getEnv());
  for (int i = 0; i < _m; i++) {
    for (int j = 0; j < _n ; j++) {
      ObjectifCompacte+=_c[i][j]*_bvar[i][j];
    }
  }
  _Model.add( IloMinimize(_Model.getEnv(), ObjectifCompacte) );
  ObjectifCompacte.end();
}


void ModelCompact::FindFeasableSolution(ModelMaitre & iModelMaitre)
{
  IloCplex cplexCompact(_Model);

  cplexCompact.setParam(IloCplex::IntSolLim, 1); // Valeur par defaut : 2100000000 (arret apres la premiere solution entiere)
  cplexCompact.setParam(IloCplex::NodeSel, 0);   // Valeur par defaut : 1 (parcours en profondeur)
  cplexCompact.solve();
  _ActualCost = (int)cplexCompact.getObjValue();
  for (int j = 0; j < _m; j++) {
    IloNumArray vals(_Model.getEnv());
    cplexCompact.getValues(vals, _bvar[j]);
    InsertSolutionOnMachine(vals, j);
    
    IloInt Cout(0);
    for (int i = 0; i < _n; i++){
      Cout+=vals[i]*_c[j][i];
    }
    iModelMaitre._Colonnes[j].add(IloNumVar( iModelMaitre._Objectif(Cout) + iModelMaitre._ConstrEqual(vals) + iModelMaitre._ConstrInequal[j](1) ));
  }
  std::cout << "//--- Premiere solution entiere trouvee par Cplex ---\n";
  PrintCurrentSolution(2);
}


int ModelCompact::partition(vector<int>& A, int iBegin, int iEnd, int iTache)
{
  int x = _c[A[iBegin]][iTache];
  int i = iBegin;
  int j;

  for (j = iBegin+1; j < iEnd; j++) {
    if (_c[A[j]][iTache] <= x) {
      i = i+1;
      swap(A[i], A[j]);
    }
  }

  swap(A[i], A[iBegin]);
  return i;
}


void ModelCompact::SortIncreasingCost(vector<int>& A, int iBegin, int iEnd, int iTache)
{
  int pivot;
  if (iBegin < iEnd)
  {
    pivot = partition(A, iBegin, iEnd, iTache);
    SortIncreasingCost(A, iBegin, pivot, iTache);  
    SortIncreasingCost(A, pivot+1, iEnd, iTache);
  }
}


int ModelCompact::partitionCapacity(vector<int>& A, int iBegin, int iEnd, int iTache)
{
  int x = _a[A[iBegin]][iTache];
  int i = iBegin;
  int j;

  for (j = iBegin+1; j < iEnd; j++) {
    if (_a[A[j]][iTache] <= x) {
      i = i+1;
      swap(A[i], A[j]);
    }
  }

  swap(A[i], A[iBegin]);
  return i;
}


void ModelCompact::SortIncreasingCapacity(vector<int>& A, int iBegin, int iEnd, int iTache)
{
  int pivot;
  if (iBegin < iEnd)
  {
    pivot = partitionCapacity(A, iBegin, iEnd, iTache);
    SortIncreasingCost(A, iBegin, pivot, iTache);  
    SortIncreasingCost(A, pivot+1, iEnd, iTache);
  }
}


int ModelCompact::PushToEndInadmissibleMachines(vector<int>& vIndex, int iTache)
{
  int left = -1;
  int right = vIndex.size();
  
  while (true)
  {
    do right--; while( left < right && _a[vIndex[right]][iTache] > _b[vIndex[right]] - _ActualCapacity[vIndex[right]] );
    do left++; while( left < right && _a[vIndex[left]][iTache] <= _b[vIndex[left]] - _ActualCapacity[vIndex[left]] );
    
    if (left < right)
      swap(vIndex[left], vIndex[right]);
    else break;
  }
  
  return left;
}


void ModelCompact::GRASP(int iRCL, int iHelpFeasability)
{
  int RCL = std::min(iRCL,(int)_m);
  
  vector<int> Index(_m, 0);
  for (int j = 0; j < _m; j++)
    Index[j] = j;
  
  RandomIterator RdIt(_n);
  for (int i = 0; i < _n; i++)
  {
    int pivot = PushToEndInadmissibleMachines(Index, RdIt.getNb());
    int ChosenMachine = 0;

    switch (iHelpFeasability)
    {
    case 0:
      SortIncreasingCost(Index, 0, pivot, RdIt.getNb());
      if (pivot < RCL)
        SortIncreasingCost(Index, pivot, _m, RdIt.getNb());
      ChosenMachine = rand()%RCL;
      break;

    case 1:
      SortIncreasingCost(Index, 0, pivot, RdIt.getNb());
      ChosenMachine = rand()%std::max(std::min(RCL,pivot),1);
      break;

    case 2:
      SortIncreasingCost(Index, 0, pivot, RdIt.getNb());
      SortIncreasingCapacity(Index, 0, std::min(RCL,pivot), RdIt.getNb());
      ChosenMachine = 0;
      break;

    case 3:
      SortIncreasingCapacity(Index, 0, pivot, RdIt.getNb());
      ChosenMachine = 0;
      break;

    default:
      SortIncreasingCapacity(Index, 0, pivot, RdIt.getNb());
      ChosenMachine = rand()%std::max(std::min(RCL,pivot),1);
      break;
    }

    _x[i] = Index[ChosenMachine];
    _ActualCapacity[ChosenMachine] += _a[ChosenMachine][RdIt.getNb()];
    _ActualCost += _c[ChosenMachine][RdIt.getNb()];
    ++RdIt;
  }

  // Calcul du cout de l'affectation obtenus
  ComputeCost();
}


int ModelCompact::ComputeCost()
{
  _ActualCost = 0;
  int idx_l;
  for (idx_l = 0; idx_l < _m; idx_l++) {
    _ActualCapacity[idx_l] = 0;
  }
  int idx_c;
  for (idx_c = 0; idx_c < _n; idx_c++) {
    _ActualCost += _c[_x[idx_c]][idx_c];
    _ActualCapacity[_x[idx_c]] += _a[_x[idx_c]][idx_c];
  }
  return _ActualCost;
}


bool ModelCompact::IsAdmissible()
{
  if (!_IsAdmissible)
  {
    for (int j = 0; j < _m; j++) {
      if (_ActualCapacity[j] > _b[j])
        return false;
    }
    _IsAdmissible = true;
  }
  return _IsAdmissible;
}


void ModelCompact::InsertSolution(NumMatrix & iSolution)
{
  for (int i = 0; i < _n; i++)
  {
    int ActualMachine = 0;
    while (ActualMachine < _m && fabs(iSolution[ActualMachine][i]) < CST_EPS)
      ActualMachine++;
    assert(ActualMachine < _m);
    _x[i] = ActualMachine;
  }
}


void ModelCompact::InsertSolutionOnMachine(IloNumArray & iSolution, int iIdxMachine)
{
  for (int idx_c = 0; idx_c < _n; idx_c++)
    if (fabs(iSolution[idx_c]) > CST_EPS)
      _x[idx_c] = iIdxMachine;
}


void ModelCompact::PrintCurrentSolution(int iMode)
{
  std::cout << "Cout de la solution courante : " << _ActualCost << "\n";
  switch (iMode)
  {
  case 0:  // Mode silencieux : seulement le cout
    break;

  case 1:  // Mode etendu : machine par machine avec la capacite occupe a droite
    std::cout << "Affectation et capacite\n";
    for (int j = 0; j < _m; j++) {
      for (int i = 0; i < _n; i++) {
        if (_x[i] == j)
          std::cout << "1 ";
        else
          std::cout << "0 ";
      }
      std::cout << "\t|  " << _ActualCapacity[j] << "\n";
    }
    break;

  case 2:  // Mode compact : un seul vecteur contenant en indice i le numero de la machine j ou la tache i est affectee
    std::cout << "Affectation\n";
    for (int i = 0; i < _n; i++) {
      std::cout << _x[i] << "\t";
    }
    std::cout << "\n";
    break;

  default:
    break;
  }
}


bool ModelCompact::NeighbourhoodSearch(int iNSize)
{
  if (iNSize <= 0 || iNSize > std::min(2, (int)_n) ) {
    std::cout << "It is a stupid use of this method !" << std::endl;
    return false;
  }
  // FOR DEBUG : TO REMOVE !
  cout << "Tentative de voisinage de taille " << iNSize << "\n";

  bool FindABetterSolution = false;
  
  // Memoire des dernieres taches dont l'affectation a change
  int * aLastChanges = new int[iNSize];
  for (int i = 0; i < iNSize; i++)
    aLastChanges[i] = -1;
  
  // Iterateur sur les solutions voisines
  ModelCompactIterator ModelCompactIt(*this, iNSize);
  while (!ModelCompactIt.IsEnded())
  {
    if ( _ActualCost > ModelCompactIt._Cost && ModelCompactIt.IsAdmissible() )
    {
      // Modification de la solution courante
      if (aLastChanges[0] != -1) {
        for (int i = 0; i < iNSize; i++)
          _x[aLastChanges[i]] = ModelCompactIt._aMachineInitiale[aLastChanges[i]];
      }
      for (int i = 0; i < iNSize; i++) {
        _x[ModelCompactIt._KcombIt(i)] = ModelCompactIt._HcubeIt(i);
        aLastChanges[i] = ModelCompactIt._KcombIt(i);
      }
      
      _ActualCost = ModelCompactIt._Cost;
      
      for (int j = 0; j < _m; j++)
        _ActualCapacity[j] = ModelCompactIt._aCapacity[j];
      
      // FOR DEBUG : TO REMOVE !
      PrintCurrentSolution();
      FindABetterSolution = true;
    }
    
    ++ModelCompactIt;
  }

  if (aLastChanges)
    delete [] aLastChanges; aLastChanges = 0;

  return FindABetterSolution;
}


void ModelCompact::LocalSearchAlgorithm(int iMaxSize)
{
  if (iMaxSize < 1 || !IsAdmissible())
    return;

  int NeighbourhoodSize = 1;
#ifdef MAX_TIME_FOR_LOCAL_SEARCH
  double EndTime = get_wall_time() + MAX_TIME_FOR_LOCAL_SEARCH;
  while (get_wall_time() < EndTime)
#else
  while (true)
#endif
  {
    if (NeighbourhoodSearch(NeighbourhoodSize))
    {
      NeighbourhoodSize = 1;
      continue;
    }
    else if (NeighbourhoodSize < iMaxSize)
    {
      NeighbourhoodSize++;
      continue;
    }
    else
    {
      break;
    }
  }
}


int ModelCompact::ComputeUBforObjective()
{
  int Cost = 0;
  for (int j = 0; j < _m; j++) {
    for (int i = 0; i < _n; i++) {
      Cost += _c[j][i];
    }
  }
  return Cost;
}

