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
#include "ModelMaitre.h"
#include "array2d.h"
#include <vector>
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
  
  /**
   * Constructeur par copie
   * @param iCompact: Modele compact a copier
   */
  ModelCompact(const ModelCompact & iCompact);
  
  /** Destructeur */
  ~ModelCompact();

  /** Cree la fonction objectif et les contraintes du modele compact */
  void CreateObjectiveAndConstraintes();
  
  /**
   * Trouve une solution realisable du probleme en effectuant un
   * branch-and-bound avec un parcours en profondeur
   * @param iModelMaitre: Model maitre a initialiser avec la solution
   */
  void FindFeasableSolution(ModelMaitre & iModelMaitre);
  
  /**
   * Algorithme d'initialisation glouton aleatoire
   */
  void GRASP(int iRCL);
  
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
  
  /** Affiche la solution courante */
  void PrintCurrentSolution();
  
  /** Donnees */
  IloModel _Model;  // Modele
  IloInt _m;        // Nombre de machines
  IloInt _n;        // Nombre de taches
  IntMatrix _c;     // matrice des couts d'affectation
  IntMatrix _a;     // matrice des capacites consommees
  IloIntArray _b;   // vecteur des capacites des machines
  
  /** Variables de decision du modele compact */
  BoolVarMatrix _bvar;
  
  /** Variables */
  int * _x;                     // Solution courante (pour chaque tache est stocke le numero de la machine)
  int _ActualCost;              // Prix de la solution courante
  IloIntArray _ActualCapacity;  // Capacite occupee sur chaque machine


private:
  ModelCompact(){}; // Constructeur par defaut
  
  /**
   * Trie un tableau d'indice par ordre croissant du cout d'affectation de la tache iTAche sur les machine
   * @param iaIndex: Tableau d'indice a trier
   * @param iBegin:  Indice de debut du tri
   * @param iEnd:    Indice de fin du tri
   * @param iTache:  Indice de la tache a considerer
   * @param iLenght: on ne veut que les iLenght premiers elements
   */
  void SortIncreasingCost(int * iaIndex, int iBegin, int iEnd, int iTache, int iLenght);
  
};

#endif


