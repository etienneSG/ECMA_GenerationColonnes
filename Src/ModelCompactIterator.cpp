#include "ModelCompactIterator.h"


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
  _aCapacity(0)
{
  _aCapacity = new int[iModelCompact._m];
  for (int i = 0; i < iModelCompact._m; i++)
    _aCapacity[i] = iModelCompact._ActualCapacity[i];
  
  // Initialisation necessaire car l'iterateur ne part pas de la position actuelle
  if (_VSize)
  {
    for (int i = 0; i < _VSize; i++)
    {
      // Recuperation des machines ou sont affectees les taches
      int ActualMachine = 0;
      while (ActualMachine < _m && !iModelCompact._x(ActualMachine, _KcombIt(i)))
        ActualMachine++;
      assert(ActualMachine < _m);
      
      // Mise a jour des cout et des capacites
      _Cost = _Cost - iModelCompact._c[ActualMachine][_KcombIt(i)] + iModelCompact._c[_HcubeIt(i)][_KcombIt(i)];
      _aCapacity[ActualMachine] -=  iModelCompact._a[ActualMachine][_KcombIt(i)];
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
}


void ModelCompactIterator::operator++()
{
  for (int i = 0; i < _VSize; i++) {
    _Cost -= (*_c)[_HcubeIt(i)][_KcombIt(i)];
    _aCapacity[_HcubeIt(i)] -= (*_a)[_HcubeIt(i)][_KcombIt(i)];
  }
  
  if (_HcubeIt.IsEnded()) {
    _HcubeIt.Reset();
    ++_KcombIt;
  }
  else
    ++_HcubeIt;
  
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

