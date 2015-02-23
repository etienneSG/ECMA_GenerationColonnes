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
   * @param iRCL:             Taille de la liste de machine dans laquelle choisir
   * @param iHelpFeasability: Pour reunir les conditions favorables a l'emergence
   *   d'une solution realisable (plus la valeur est elevee, plus on est exigeant) 
   */
  void GRASP(int iRCL, int iHelpFeasability = 0);
  
  /** Calcule le cout de la solution courante et actualise les capacites consommees */
  int ComputeCost();
  
  /** Retourne vrai si la solution courante est admissible et faux sinon */
  bool IsAdmissible();
  
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
  
  /**
   * Affiche la solution courante
   * @param iMode: Mode d'affichage. Valeurs possibles :
   *    <br> 0 (Mode silencieux) : seulement le cout
   *    <br> 1 (Mode etendu) : machine par machine avec la capacite occupe a droite
   *    <br> 2 (Mode compact) : un seul vecteur contenant en indice i le numero de la machine j
   *                            ou la tache i est affectee
   */
  void PrintCurrentSolution(int iMode = 0);
  
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
  bool _IsAdmissible;           // Vrai si la solution est admissible et faux sinon
  
  // FOR DEBUG ONLY !
  /** Calcule une borne superieure de l'objectif en sommant tous les couts de _c */
  int ComputeUBforObjective();

private:
  int & _NbOfCopy;  // Nombre de copies construites
  
  ModelCompact(); // Constructeur par defaut
  
  /**
   * Trie un vecteur d'indice par ordre croissant du cout d'affectation de la tache iTache sur les machines
   * @param A:      Vecteur d'indice a trier
   * @param iBegin: Indice de debut du tri
   * @param iEnd:   Indice de fin du tri
   * @param iTache: Indice de la tache a considerer
   */
  void SortIncreasingCost(std::vector<int>& A, int iBegin, int iEnd, int iTache);
  
  /** Fonction auxiliaire du tri precedent */
  int partition(std::vector<int>& A, int iBegin, int iEnd, int iTache);

  /**
   * Trie un vecteur d'indice par ordre croissant de capacite occupee par la tache iTache sur les machines
   * @param A:      Vecteur d'indice a trier
   * @param iBegin: Indice de debut du tri
   * @param iEnd:   Indice de fin du tri
   * @param iTache: Indice de la tache a considerer
   */
  void SortIncreasingCapacity(std::vector<int>& A, int iBegin, int iEnd, int iTache);

  /** Fonction auxiliaire du tri precedent */
  int partitionCapacity(std::vector<int>& A, int iBegin, int iEnd, int iTache);
  
  /**
   * Range à la fin du vecteur les machines dont la capacite disponible ne peut recevoir la tache iTache
   * @param vIndex: Vecteur d'indice du tableau
   * @param iTAche: Indice de la tache
   * @return: premiere position d'une machine inadmissible
   */
  int PushToEndInadmissibleMachines(std::vector<int>& vIndex, int iTache);
  
};

#endif


