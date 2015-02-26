//==============================================================================
// 
// Fonctions permettant d'effectuer la recherche locale
//
//==============================================================================


#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "ModelCompact.h"


/**
 * Effectue une recherche locale a partir de la solutions construite avec la methode GRASP
 * et avec les deux premieres solutions entieres trouvees par Cplex
 * @param iModelCompact: Modele sur lequel effectuer la recherche locale
 */
void LocalSearchAlgorithm(ModelCompact & iModelCompact);


/**
 * Effectue une recherche locale a partir de la solution courante contenue dans iModelCompact
 * et enregistre les traces (temps d'execution, affichage de la solution, etc.)
 * @param iModelCompact: Modele sur lequel effectuer la recherche locale
 */
void LocalSearch(ModelCompact & iModelCompact);


/**
 * Effectue une recherche locale a partir d'une "bonne" solution entiere trouvee par Cplex
 * en moins de 7 secondes
 * @param iModelCompact: Modele sur lequel effectuer la recherche locale
 */
void SpecificLocalSearch(ModelCompact & iModelCompact);


#endif


