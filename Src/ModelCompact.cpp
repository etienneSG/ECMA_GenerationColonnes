#include "ModelCompact.h"

#include <stdio.h>   // Standard Input/Output Header
#include <stdlib.h>  // C Standard General Utilities Library
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <math.h>
//#include "kcombinationiterator.h"
//#include "HcubeIterator.h"
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
  _ActualCost(0),
  _ActualCapacity(iEnv)
{
  ReadData(iFile, _Model, _c, _m, _n, _a, _b);
  _x.Resize(_m, _n);
  _ActualCapacity.add(_m, 0);
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
  for (idx_l = 0; idx_l < _m; idx_l++) {
    int idx_c;
    for (idx_c = 0; idx_c < _n; idx_c++) {
      if (_x(idx_l, idx_c)) {
        _ActualCost = _ActualCost + _c[idx_l][idx_c];
        _ActualCapacity[idx_l] = _ActualCapacity[idx_l] + _a[idx_l][idx_c];
      }
    }
  }
  return _ActualCost;
}


void ModelCompact::InsertSolution(NumMatrix & iSolution)
{
  for (int idx_l = 0; idx_l < _m; idx_l++)
    for (int idx_c = 0; idx_c < _n; idx_c++)
      _x(idx_l, idx_c) = fabs(iSolution[idx_l][idx_c]) > CST_EPS ? true : false;
}


void ModelCompact::InsertSolutionOnMachine(IloNumArray & iSolution, int iIdxMachine)
{
  for (int idx_c = 0; idx_c < _n; idx_c++)
    _x(iIdxMachine, idx_c) = fabs(iSolution[idx_c]) > CST_EPS ? true : false;
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
/*
  int * aBestTache = new int[iNSize];
  int * aBestMachine = new int[iNSize];
  for (int i = 0; i < iNSize; i++) {
    aBestTache[i] = -1;
    aBestMachine[i] = -1;
  }
*/
  
  // Iterateur sur les solutions voisines
  ModelCompactIterator ModelCompactIt(*this, iNSize);
  while (!ModelCompactIt.IsEnded())
  {
    if (_ActualCost > ModelCompactIt._Cost && ModelCompactIt.IsAdmissible())
    {
      // Modification de la solution courante
      for (int i = 0; i < iNSize; i++)
      {
        int ActualMachine = 0;
        while (ActualMachine < _m && !_x(ActualMachine, ModelCompactIt._KcombIt(i)))
          ActualMachine++;
        assert(ActualMachine < _m);
       _x(ActualMachine, ModelCompactIt._KcombIt(i)) = false;
       _x(ModelCompactIt._HcubeIt(i), ModelCompactIt._KcombIt(i)) = true;
      }
      
      _ActualCost = ModelCompactIt._Cost;
      
      for (int j = 0; j < _m; j++) {
        _ActualCapacity[j] = ModelCompactIt._aCapacity[j];
      }
      
      FindABetterSolution = true;
    }
    
    ++ModelCompactIt;
  }
/*
  // Iterateur sur les solutions voisines
  KcombinationIterator NeighbourhoodIt(iNSize, _n);
  while (!FindABetterSolution && !NeighbourhoodIt.IsEnded())
  {
    NeighbourhoodIt.Print();
    // Recuperation des machines ou sont affectees les taches
    int * aActualMachine = new int[iNSize];
    for (int i = 0; i < iNSize; i++) {
      aActualMachine[i] = 0;
      while (aActualMachine[i] < _m && !_x(aActualMachine[i], NeighbourhoodIt(i)))
        aActualMachine[i]++;
      assert(aActualMachine[i] < _m);
    }
    // Calcul du cout sans les affectations precedentes
    int PartialCost = _ActualCost;
    for (int i = 0; i < iNSize; i++) {
      PartialCost -= _c[aActualMachine[i]][NeighbourhoodIt(i)];
    }
    // Calcul des ressources consommees sans les affectations precedentes
    int * aPartialRessource = new int[_m];
    for (int j = 0; j < _m; j++)
      aPartialRessource[j] = _ActualCapacity[j];
    std::cout << "ActualCapacity\n";
    PrintArray(_ActualCapacity);
    for (int i = 0; i < iNSize; i++) {
      PartialRessource[aActualMachine[i]] -= _a[aActualMachine[i]][NeighbourhoodIt(i)];
    }
    std::cout << "Ressource partielle\n";
    PrintArray(PartialRessource);
    std::cout << "Affectation choisie\n";
    _x.Print();
    
    HcubeIterator MachinesIt(_m, iNSize);
    while (!FindABetterSolution && !MachinesIt.IsEnded())
    {
      int NewCost = PartialCost;
      for (int i = 0; i < iNSize; i++) {
        NewCost += _c[MachinesIt(i)][NeighbourhoodIt(i)];
      }
      if (NewCost < _ActualCost)
      {
        // On verifie que la solution est admissible
        IloIntArray NewRessource(PartialRessource);
        for (int i = 0; i < iNSize; i++) {
          NewRessource[MachinesIt(i)] += _a[MachinesIt(i)][NeighbourhoodIt(i)];
        }
        bool IsAdmissible = true;
        for (int j = 0; j < _m; j++) {
          if (NewRessource[j] > _b[j]) {
            IsAdmissible = false;
            break;
          }
        }
        if (IsAdmissible) {
          _ActualCost = NewCost;
          for (int i = 0; i < iNSize; i++) {
            _x(MachinesIt(i), NeighbourhoodIt(i)) = true;
            _x(aActualMachine[i], NeighbourhoodIt(i)) = false;
          }
          for (int j = 0; j < _m; j++) {
            _ActualCapacity[j] = NewRessource[j];
          }
          FindABetterSolution = true;
          break;
        }
      }
      else
        ++MachinesIt;
    }
    
    if (aPartialRessource)
      delete [] aPartialRessource; aPartialRessource = 0;
    if (aActualMachine)
      delete [] aActualMachine; aActualMachine = 0;
    
    ++NeighbourhoodIt;
  }
*/
/*
  if (aBestTache)
    delete [] aBestTache; aBestTache = 0;
  if (aBestMachine)
    delete [] aBestMachine; aBestMachine = 0;
*/
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


ModelCompact::~ModelCompact()
{
}

