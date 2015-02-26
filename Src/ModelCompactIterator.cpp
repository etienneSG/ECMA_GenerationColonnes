#include "ModelCompactIterator.h"

#include <string.h>

ModelCompactIterator::ModelCompactIterator(ModelCompact & iModelCompact, int iVoisinageSize)
: _VSize(iVoisinageSize),
  _AleaIt(iModelCompact._m, iModelCompact._n, iVoisinageSize),
  _m(iModelCompact._m),
  _n(iModelCompact._n),
  _c(iModelCompact._c),
  _a(iModelCompact._a),
  _b(iModelCompact._b),
  _Cost(iModelCompact._ActualCost),
  _aCapacity(0),
  _Penalties(iModelCompact._Penalties),
  _aMachineInitiale(0),
  _InitialCost(iModelCompact._ActualCost),
  _InitialPenalties(iModelCompact._Penalties),
  _aInitialCapacity(0)
{
  _aCapacity = new int[iModelCompact._m];
  for (int i = 0; i < iModelCompact._m; i++)
    _aCapacity[i] = iModelCompact._ActualCapacity[i];
  _aInitialCapacity = new int[iModelCompact._m];
  memcpy(_aInitialCapacity, _aCapacity, iModelCompact._m*sizeof(int));
  
  // Recuperation des machines ou sont affectees les taches
  _aMachineInitiale = new int[iModelCompact._n];
  memcpy(_aMachineInitiale, iModelCompact._x, _n*sizeof(int));
    
  // Initialisation necessaire car l'iterateur ne part pas de la position actuelle
  if (_VSize)
  {
    for (int i = 0; i < _VSize; i++)
    {
      // Mise a jour du cout et des capacites
      _Cost = _Cost - iModelCompact._c[_aMachineInitiale[_AleaIt.N(i)]][_AleaIt.N(i)] + iModelCompact._c[_AleaIt.K(i)][_AleaIt.N(i)];
      _aCapacity[_aMachineInitiale[_AleaIt.N(i)]] -=  iModelCompact._a[_aMachineInitiale[_AleaIt.N(i)]][_AleaIt.N(i)];
      _aCapacity[_AleaIt.K(i)] +=  iModelCompact._a[_AleaIt.K(i)][_AleaIt.N(i)];
    }
  }
  ComputePenalties();

}


ModelCompactIterator::ModelCompactIterator()
: _c(*(new IntMatrix)),
  _a(*(new IntMatrix)),
  _b(*(new IloIntArray))
{
}


ModelCompactIterator::~ModelCompactIterator()
{
  if (_aCapacity)
    delete [] _aCapacity; _aCapacity = 0;
  if (_aMachineInitiale)
    delete [] _aMachineInitiale; _aMachineInitiale = 0;
  if (_aInitialCapacity)
    delete [] _aInitialCapacity; _aInitialCapacity = 0;
}


int ModelCompactIterator::ComputePenalties()
{
  _Penalties = 0;
  for (int j = 0; j < _m; j++) {
    _Penalties += PenaltyCost(_aCapacity[j],_b[j]);
  }
  return _Penalties;
}


void ModelCompactIterator::operator++()
{
  // On enleve les couts actuels des anciennes machines
  for (int i = 0; i < _VSize; i++) {
    _Cost -= _c[_AleaIt.K(i)][_AleaIt.N(i)];
    _aCapacity[_AleaIt.K(i)] -= _a[_AleaIt.K(i)][_AleaIt.N(i)];
  }
  // On rajoute les couts initiaux pour les anciennes machines
  for (int i = 0; i < _VSize; i++) {
    _Cost += _c[_aMachineInitiale[_AleaIt.N(i)]][_AleaIt.N(i)];
    _aCapacity[_aMachineInitiale[_AleaIt.N(i)]] +=  _a[_aMachineInitiale[_AleaIt.N(i)]][_AleaIt.N(i)];
  }
  
  // On incremente
  ++_AleaIt;
  
  // On eneve les cout initiaux pour les nouvelles machines
  for (int i = 0; i < _VSize; i++) {
    _Cost -= _c[_aMachineInitiale[_AleaIt.N(i)]][_AleaIt.N(i)];
    _aCapacity[_aMachineInitiale[_AleaIt.N(i)]] -=  _a[_aMachineInitiale[_AleaIt.N(i)]][_AleaIt.N(i)];
  }
  // On ajoute les couts actuels pour les nouvelles machines
  for (int i = 0; i < _VSize; i++) {
    _Cost += _c[_AleaIt.K(i)][_AleaIt.N(i)];
    _aCapacity[_AleaIt.K(i)] += _a[_AleaIt.K(i)][_AleaIt.N(i)];
  }
  
  ComputePenalties();
}


bool ModelCompactIterator::IsAdmissible()
{
  for (int i = 0; i < _m; i++) {
    if (_aCapacity[i] > _b[i])
      return false;
  }
  return true;
}

