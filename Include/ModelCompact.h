//==============================================================================
// 
// Class ModelCompact:
//   Definit le model compact
// Attention ! Le constructeur vide ne peut etre utilise !
//
//==============================================================================

#ifndef MODELCOMPACT_H
#define MODELCOMPACT_H

#include <iostream>
#include <fstream>

#include "ConstantsAndTypes.h"
#include "array2d.h"
#include <ilcplex/ilocplex.h>

class ModelCompact
{
public:
  /**
   * Constructeur
   * @param iEnv:  envirronnement cplex
   * @param iFile: fichier dans lequel charger les donnees d'entree
   */
  ModelCompact(std::string iFile, IloEnv iEnv);
  
  /** Destructeur */
  ~ModelCompact();

  /** Calcule le cout de la solution courante et actualise les capacites consommees */
  int ComputeCost();
  
  /**
   * Change la solution courante
   * @param iSolution: Nouvelle solution
   */
  void InsertSolution(NumMatrix & iSolution);

  /**
   * Change la solution courante sur la machine iIdxMachine
   * @param iSolution:   Nouvelle solution
   * @param iIdxMachine: Indice de la machine dont on doit changer l'affectation
   */
  void InsertSolutionOnMachine(IloNumArray & iSolution, int iIdxMachine);
  
  /**
   * Effectue une recherche locale dans un voisinage de taille iNSize
   * et s'arrete a la premiere solution ameliorante trouvee
   * @param iNSize: taille du voisinage à considerer
   */
  bool NeighbourhoodSearch(int iNSize);
  
  /**
   * Effectue une recherche locale complete en utilisant une structure de voisinage.
   * @param iMaxSize: Taille maximale du voisinage utilise
   */
  void LocalSearchAlgorithm(int iMaxSize);
  
  /** Donnees */
  IloModel _Model;  // Modele
  IloInt _m;        // Nombre de machines
  IloInt _n;        // Nombre de taches
  IntMatrix _c;     // matrice des couts d'affectation
  IntMatrix _a;     // matrice des capacites consommees
  IloIntArray _b;   // vecteur des capacites des machines
  
  /** Variables */
  Array2d _x;                   // Solution courante
  int _ActualCost;              // Prix de la solution courante
  IloIntArray _ActualCapacity;  // Capacite occupee sur chaque machine


private:
  ModelCompact(){}; // Constructeur par defaut
};

#endif


