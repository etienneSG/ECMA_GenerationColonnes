//==============================================================================
// 
// Class ModelMaitre:
//   Definit le model maitre de la generation de colonnes
// Attention ! Le constructeur vide ne peut etre utilise !
//
//==============================================================================


#ifndef MODELMAITRE_H
#define MODELMAITRE_H

class ModelCompact;

#include "ConstantsAndTypes.h"
#include <ilcplex/ilocplex.h>

class ModelMaitre
{
public:
  /**
   * Constructeur
   * @param iEnv:     Environement cplex
   * @param iCompact: Modele compact associe au modele maitre
   */
  ModelMaitre(IloEnv iEnv, ModelCompact & iCompact);
  
  /** Destructeur */
  ~ModelMaitre();
  
  /** Donnees */
  IloEnv _Env;                    // Environnement Cplex
  IloModel _Model;                // Modele
  IntMatrix _CoutColonne;         // Cout des colonnes du probleme maitre
  IntMatrix3d _IsTacheInColonne;  // Matrice permettant du dire si une tache se trouve dans une colonne
  ModelCompact * _pCompact;       // Modele compact associe au modele maitre
  
  /** Variables */
  NumVarMatrix _Colonnes;
  
  /** Objectif et contraintes */
  IloObjective _Objectif;         // Objectif
  IloRangeArray _ConstrEqual;     // Contrainte en egalite
  IloRangeArray _ConstrInequal;   // Contrainte en inegalite
  
private:
  ModelMaitre(){};
};


#endif

