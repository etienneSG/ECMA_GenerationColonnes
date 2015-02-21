//==============================================================================
// 
// Fonctions permettant d'effectuer la recherche locale
//
//==============================================================================


#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "ModelCompact.h"


/**
 * Effectue une recherche locale a partir de la solution courante contenue dans iModelCompact
 * @param iModelCompact: Modele sur lequel effectuer la recherche locale
 */
void LocalSearchAlgorithm(ModelCompact & iModelCompact);


/**
 * Effectue une recherche locale a partir de la solution courante contenue dans iModelCompact
 * et enregistre les traces (temps d'execution, affichage de la solution, etc.)
 * @param iModelCompact: Modele sur lequel effectuer la recherche locale
 */
void LocalSearch(ModelCompact & iModelCompact);


#endif


