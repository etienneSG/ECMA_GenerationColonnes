#include "ModelMaitre.h"
#include "ModelCompact.h"

ModelMaitre::ModelMaitre(IloEnv iEnv,  ModelCompact & iCompact)
: _Env(iEnv),
  _Model(iEnv),
  _CoutColonne(iEnv),
  _IsTacheInColonne(iEnv),
  _Colonnes(iEnv),
  _pCompact(&iCompact)
{
  for (int i = 0; i < iCompact._m; i++)
  {
    _CoutColonne.add(IloIntArray(iEnv));
    _IsTacheInColonne.add(IntMatrix(iEnv));
    _Colonnes.add(IloBoolVarArray(iEnv));
  }

  _Objectif = IloAdd(_Model, IloMinimize(iEnv));

  //Contraintes probleme maitre
  _ConstrEqual = IloAdd(_Model, IloRangeArray(iEnv, iCompact._n, 1, 1));
  _ConstrInequal = IloAdd(_Model, IloRangeArray(iEnv, iCompact._m, 0, 1));
}


ModelMaitre::~ModelMaitre()
{
  _pCompact = 0;
  _Objectif.end();
  _ConstrEqual.end();
  _ConstrInequal.end();
}

