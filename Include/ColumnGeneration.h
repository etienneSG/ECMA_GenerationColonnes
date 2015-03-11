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
 * en resolvant les programmes auxiliaires a l'aide de la programmation dynamique
 * @param iMaster:      Model maitre contenant les colonnes creees
 * @param iCompact:     Model compact contenant les donnees du probleme
 * @param icplexMaster: Solveur associe au programme maitre
 */
void ResolutionOfAuxProgramsByDynProg(ModelMaitre & iMaster, ModelCompact & iCompact, IloCplex & iCplexMaster);

/**
 * Effectue une recherche par generation de colonnes
 * en resolvant les programmes auxiliaires avec CPlex
 * @param iMaster:      Model maitre contenant les colonnes creees
 * @param iCompact:     Model compact contenant les donnees du probleme
 * @param icplexMaster: Solveur associe au programme maitre
 */
void ResolutionOfAuxProgramsByCplex(ModelMaitre & iMaster, ModelCompact & iCompact, IloCplex & iCplexMaster);


#endif

