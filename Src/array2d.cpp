#include "array2d.h"

#include <string.h>
#include <iostream>


Array2d::Array2d():
_NbRow(0),
_NbColumn(0),
_aTab(0)
{
}


Array2d::Array2d(int iNbRow, int iNbColumn):
_NbRow(iNbRow),
_NbColumn(iNbColumn),
_aTab(0)
{
  int nbBox = _NbRow*_NbColumn;
  if (nbBox>0)
  {
    _aTab = new bool[nbBox];
    memset(_aTab, 0, nbBox*sizeof(bool));
  }
}


Array2d::~Array2d()
{
  if (_aTab)
    delete [] _aTab; _aTab=0;
}

void Array2d::Resize(int iNbRow, int iNbColumn)
{
  if (_aTab)
    delete [] _aTab; _aTab=0;
  _NbRow = iNbRow;
  _NbColumn = iNbColumn;
  int nbBox = _NbRow*_NbColumn;
  if (nbBox>0)
  {
    _aTab = new bool[nbBox];
    memset(_aTab, 0, nbBox*sizeof(bool));
  }
}


void Array2d::Print()
{
  if (_aTab)
  {
    for (int i = 0; i < _NbRow; i++) {
      for (int j = 0; j < _NbColumn; j++) {
        if (_aTab[_NbColumn*i+j])
          std::cout << "1 ";
        else
          std::cout << "0 ";
        std::cout << "\n";
      }
    }
  }
}


