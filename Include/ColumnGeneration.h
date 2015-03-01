//==============================================================================
// 
// Fonctions permettant d'effectuer la generation de colonnes
//
//==============================================================================


#ifndef COLUMNGENERATION_H
#define COLUMNGENERATION_H

#include "ModelCompact.h"
#include "ModelMaitre.h"
#include <ilcplex/ilocplex.h>

/**
 * Effectue une recherche par generation de colonnes
 * et enregistre les traces (temps d'execution, affichage de la solution, etc.)
 * @param iMaster:  Model maitre contenant les colonnes creees
 * @param iCompact: Model compact contenant les donnees du probleme
 */
void ColumnGeneration(ModelMaitre & iMaster, ModelCompact & iCompact);


/**
 * Effectue une recherche par generation de colonnes
 * @param iMaster:  Model maitre contenant les colonnes creees
 * @param iCompact: Model compact contenant les donnees du probleme
 */
void ColumnGenerationAlgorithm(ModelMaitre & iMaster, ModelCompact & iCompact);

/**
 * Effectue une recherche par generation de colonnes
 * en resolvant de maniere approchee les programmes auxiliaires
 * @param iMaster:      Model maitre contenant les colonnes creees
 * @param iCompact:     Model compact contenant les donnees du probleme
 * @param icplexMaster: Solveur associe au programme maitre
 */
void ApproxResolutionOfAuxPrograms(ModelMaitre & iMaster, ModelCompact & iCompact, IloCplex & iCplexMaster);

/**
 * Effectue une recherche par generation de colonnes
 * en resolvant de maniere approchee les programmes auxiliaires
 * @param iMaster:      Model maitre contenant les colonnes creees
 * @param iCompact:     Model compact contenant les donnees du probleme
 * @param icplexMaster: Solveur associe au programme maitre
 */
void ExactResolutionOfAuxPrograms(ModelMaitre & iMaster, ModelCompact & iCompact, IloCplex & iCplexMaster);


#endif

