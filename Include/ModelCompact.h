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

  /** Attributs */
  IloModel _Model;  // Modele
  NumMatrix _c;     // matrice des couts d'affectation
  NumMatrix _a;     // matrice des capacites consommees
  IloIntArray _b;   // vecteur des capacites des machines

private:
  ModelCompact(){}; // Constructeur par defaut
};

#endif


