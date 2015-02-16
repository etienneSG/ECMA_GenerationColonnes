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

#ifdef __linux__
#include <omp.h> // Open Multi-Processing Library (Linux only)
#endif

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
    buffer = new char[length];
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
    cout << m << " " << n << "\n";

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
  
}


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
  _ActualCapacity(iEnv)
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
: _m(iCompact._m),
  _n(iCompact._n),
  _c(iCompact._c),
  _a(iCompact._a),
  _b(iCompact._b),
  _bvar(iCompact._Model.getEnv()),
  _x(0),
  _ActualCost(0),
  _ActualCapacity(iCompact._Model.getEnv())
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
}


ModelCompact::~ModelCompact()
{
  if (_x)
    delete [] _x; _x = 0;
  _Model.end();
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
  for (int j = 0; j < _m; j++) {
    IloNumArray vals(_Model.getEnv());
    cplexCompact.getValues(vals, _bvar[j]);
    cout << "Affichage de la machine " << j << "\n";
    PrintArray(vals);
    InsertSolutionOnMachine(vals, j);
    
    IloInt Cout(0);
    for (int i = 0; i < _n; i++){
      Cout+=vals[i]*_c[j][i];
    }
    iModelMaitre._Colonnes[j].add(IloNumVar( iModelMaitre._Objectif(Cout) + iModelMaitre._ConstrEqual(vals) + iModelMaitre._ConstrInequal[j](1) ));
  }
}


void exchange(int * tab, int a, int b)
{
  int temp = tab[a];
  tab[a] = tab[b];
  tab[b] = temp;
}


void ModelCompact::SortIncreasingCost(int * iaIndex, int iBegin, int iEnd, int iTache, int iLenght)
{
  int left = iBegin-1;
  int right = iEnd+1;
  int pivot = iaIndex[iBegin];
  
  if(iBegin >= iEnd)
    return;
  
  while(1)
  {
    do right--; while( _c[pivot][iTache] < _c[iaIndex[right]][iTache] );
    do left++; while( _c[iaIndex[left]][iTache] < _c[pivot][iTache] );
    
    if(left < right)
      exchange(iaIndex, left, right);
    else break;
  }
  
  SortIncreasingCost(iaIndex, iBegin, right, iTache, iLenght);
  if (right+1 < iLenght)
    SortIncreasingCost(iaIndex, right+1, iEnd, iTache, iLenght);
}


void ModelCompact::GRASP(int iRCL)
{
  int RCL = std::min(iRCL,(int)_m);
  
  int * aIndex = new int[_m];
  for (int j = 0; j < _m; j++)
    aIndex[j] = j;
  
  for (int i = 0; i < _n; i++)
  {
    SortIncreasingCost(aIndex, 0, _m-1, i, RCL);
    _x[i] = aIndex[rand()%RCL];
  }
  
  if (aIndex)
    delete [] aIndex; aIndex = 0;
}


int ModelCompact::ComputeCost()
{
  _ActualCost = 0;
  int idx_l;
  //#ifdef __linux__
  //#pragma omp parallel for reduction(+:_ActualCost) schedule(static,1)
  //#endif
  for (idx_l = 0; idx_l < _m; idx_l++) {
    _ActualCapacity[idx_l] = 0;
  }
  //#ifdef __linux__
  //#pragma omp parallel for reduction(+:_ActualCost) schedule(static,1)
  //#endif
  int idx_c;
  for (idx_c = 0; idx_c < _n; idx_c++) {
    _ActualCost += _c[_x[idx_c]][idx_c];
    _ActualCapacity[_x[idx_c]] +=+ _a[_x[idx_c]][idx_c];
  }
  return _ActualCost;
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


void ModelCompact::PrintCurrentSolution()
{
  std::cout << "Cout de la solution ameliorante : " << _ActualCost << "\n";
/*  std::cout << "Affectation et capacite\n";
  for (int j = 0; j < _m; j++) {
    for (int i = 0; i < _n; i++) {
      if (_x[i] == j)
        std::cout << "1 ";
      else
        std::cout << "0 ";
    }
    std::cout << "\t|  " << _ActualCapacity[j] << "\n";
  }*/
}


bool ModelCompact::NeighbourhoodSearch(int iNSize)
{
  if (iNSize <= 0 || iNSize > std::min(3, (int)_n) ) {
    std::cout << "It is a stupid use of this method !" << std::endl;
    return false;
  }
  
  bool FindABetterSolution = false;

  // Calcul du cout de l'affectation courante
  _ActualCost = ComputeCost();
  
  // FOR DEBUG : TO REMOVE !
  //PrintCurrentSolution();
  
  // Iterateur sur les solutions voisines
  ModelCompactIterator ModelCompactIt(*this, iNSize);
  while (!ModelCompactIt.IsEnded())
  {
    if (_ActualCost > ModelCompactIt._Cost && ModelCompactIt.IsAdmissible())
    {
      // Modification de la solution courante
      for (int i = 0; i < iNSize; i++)
        _x[ModelCompactIt._KcombIt(i)] = ModelCompactIt._HcubeIt(i);
      
      _ActualCost = ModelCompactIt._Cost;
      
      for (int j = 0; j < _m; j++)
        _ActualCapacity[j] = ModelCompactIt._aCapacity[j];
      
      // FOR DEBUG : TO REMOVE !
      cout << "Taille du voisinage explore : " << iNSize << "\n";
      PrintCurrentSolution();

      FindABetterSolution = true;
    }
    
    ++ModelCompactIt;
  }

  return FindABetterSolution;
}


void ModelCompact::LocalSearchAlgorithm(int iMaxSize)
{
  if (iMaxSize < 1)
    return;
  
  int NeighbourhoodSize = 1;
  while (true)
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


