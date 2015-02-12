#include "ModelCompactIterator.h"

#include <string.h>

ModelCompactIterator::ModelCompactIterator(ModelCompact & iModelCompact, int iVoisinageSize)
: _VSize(iVoisinageSize),
  _KcombIt(iVoisinageSize, iModelCompact._n),
  _HcubeIt(iModelCompact._m, iVoisinageSize),
  _m(iModelCompact._m),
  _n(iModelCompact._n),
  _c(&iModelCompact._c),
  _a(&iModelCompact._a),
  _b(&iModelCompact._b),
  _Cost(iModelCompact._ActualCost),
  _aCapacity(0),
  _aMachineInitiale(0),
  _InitialCost(iModelCompact._ActualCost),
  _aInitialCapacity(0)
{
  _aCapacity = new int[iModelCompact._m];
  for (int i = 0; i < iModelCompact._m; i++)
    _aCapacity[i] = iModelCompact._ActualCapacity[i];
  _aInitialCapacity = new int[iModelCompact._m];
  memcpy(_aInitialCapacity, _aCapacity, iModelCompact._m*sizeof(int));
  
  // Recuperation des machines ou sont affectees les taches
  _aMachineInitiale = new int[iModelCompact._n];
  for (int i = 0; i < iModelCompact._n; i++)
  {
    _aMachineInitiale[i] = 0;
    while (_aMachineInitiale[i] < _m && !iModelCompact._x(_aMachineInitiale[i], i))
      _aMachineInitiale[i]++;
    assert(_aMachineInitiale[i] < _m);
  }
    
  // Initialisation necessaire car l'iterateur ne part pas de la position actuelle
  if (_VSize)
  {
    for (int i = 0; i < _VSize; i++)
    {
      // Mise a jour des cout et des capacites
      _Cost = _Cost - iModelCompact._c[_aMachineInitiale[_KcombIt(i)]][_KcombIt(i)] + iModelCompact._c[_HcubeIt(i)][_KcombIt(i)];
      _aCapacity[_aMachineInitiale[_KcombIt(i)]] -=  iModelCompact._a[_aMachineInitiale[_KcombIt(i)]][_KcombIt(i)];
      _aCapacity[_HcubeIt(i)] +=  iModelCompact._a[_HcubeIt(i)][_KcombIt(i)];
    }
  }

}


ModelCompactIterator::~ModelCompactIterator()
{
  _c = 0;
  _a = 0;
  _b = 0;
  if (_aCapacity)
    delete [] _aCapacity; _aCapacity = 0;
  if (_aMachineInitiale)
    delete [] _aMachineInitiale; _aMachineInitiale = 0;
  if (_aInitialCapacity)
    delete [] _aInitialCapacity; _aInitialCapacity = 0;
}


void ModelCompactIterator::operator++()
{
  for (int i = 0; i < _VSize; i++) {
    _Cost -= (*_c)[_HcubeIt(i)][_KcombIt(i)];
    _aCapacity[_HcubeIt(i)] -= (*_a)[_HcubeIt(i)][_KcombIt(i)];
  }
  
  if (_HcubeIt.IsEnded())
  {
    for (int i = 0; i < _VSize; i++) {
      _Cost += (*_c)[_aMachineInitiale[_KcombIt(i)]][_KcombIt(i)];
      _aCapacity[_aMachineInitiale[_KcombIt(i)]] +=  (*_a)[_aMachineInitiale[_KcombIt(i)]][_KcombIt(i)];
    }
    
    _HcubeIt.Reset();
    ++_KcombIt;
    
    for (int i = 0; i < _VSize; i++) {
      _Cost = _Cost - (*_c)[_aMachineInitiale[_KcombIt(i)]][_KcombIt(i)];
      _aCapacity[_aMachineInitiale[_KcombIt(i)]] -=  (*_a)[_aMachineInitiale[_KcombIt(i)]][_KcombIt(i)];
    }
  }
  else
  {
    ++_HcubeIt;
  }
  
  for (int i = 0; i < _VSize; i++) {
    _Cost += (*_c)[_HcubeIt(i)][_KcombIt(i)];
    _aCapacity[_HcubeIt(i)] += (*_a)[_HcubeIt(i)][_KcombIt(i)];
  }

}


bool ModelCompactIterator::IsAdmissible()
{
  for (int i = 0; i < _m; i++) {
    if (_aCapacity[i] > (*_b)[i])
      return false;
  }
  return true;
}

