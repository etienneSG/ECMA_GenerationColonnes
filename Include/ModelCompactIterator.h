//==============================================================================
// 
// Class ModelCompactIterator:
//   Conserve les informations reduites du modele courant et les info du modele
// a tester.
//
//==============================================================================


#ifndef MODELCOMPACTITERATOR_H
#define MODELCOMPACTITERATOR_H

#include "ConstantsAndTypes.h"
#include "kcombinationiterator.h"
#include "HcubeIterator.h"
#include "ModelCompact.h"


class ModelCompactIterator
{
public:
  /**
   * Constructeur
   * @param iModelCompact:  Modele compact autour duqyel chercher
   * @param iVoisinageSize: Taille du voisinage a explorer
   */
  ModelCompactIterator(ModelCompact & iModelCompact, int iVoisinageSize);
  
  /** Destructeur */
  ~ModelCompactIterator();
  
  /** Iterateur sur les voisins */
  void operator++();
  
  /** Retourne vrai si la solution courante est admissible et faux sinon */
  bool IsAdmissible();
  
  /** Retourne vrai si l'iterateur a parcouru l'ensemble des voisins et faux sinon */
  inline bool IsEnded();
  
  /** Iterateurs */
  int _VSize; // Taille du voisinage
  KcombinationIterator _KcombIt;
  HcubeIterator _HcubeIt;
  
  /** Donnees du probleme */
  IloInt _m;          // Nombre de machines
  IloInt _n;          // Nombre de taches
  IntMatrix * _c;     // matrice des couts d'affectation
  IntMatrix * _a;     // matrice des capacites consommees
  IloIntArray * _b;   // vecteur des capacites des machines
  
  /** Solution courante */
  int _Cost;        // Cout de la solution courante
  int * _aCapacity; // Capacite courante occupee sur chaque machine
  
  /** Solution initiale */
  int * _aMachineInitiale;  // Machines sur lesquelles se trouvaient initialement chaque tache
  int _InitialCost;         // Prix de la solution initiale
  int * _aInitialCapacity;  // Capacite occupee initialement sur chaque machine
  
private:
  ModelCompactIterator(){};
};


//==============================================================================
// Implementation des methodes inline
//==============================================================================


inline bool ModelCompactIterator::IsEnded() {
  return _KcombIt.IsEnded();
}

#endif

