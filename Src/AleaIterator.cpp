#include "AleaIterator.h"

#include <iostream>
#include <stdlib.h>

using namespace std;


AleaIterator::AleaIterator():
  _n(0),
  _k(0),
  _c(0),
  _aK(0),
  _IdxC(0)
{
}


AleaIterator::AleaIterator(int iK,int iN, int iC, int * iaK):
  _n(iN),
  _k(iK),
  _c(iC),
  _aK(iaK),
  _NcKit(iC,iN),
  _IdxK(iC,0),
  _IdxC(0)
{
  if (iC > 1)
  {
    for (int i = 0; i < iC-1; i++)
      _IdxK[i] = _aK[_NcKit(i+1)];
    _IdxK[_c-1] = _aK[_NcKit(0)];
  }
}


AleaIterator::~AleaIterator()
{
  _aK = 0;
}


void AleaIterator::operator++()
{
  switch (_c)
  {
  case 1:
    if (_IdxK[0] < _k-1)
      _IdxK[0]++;
    else
    {
      _IdxK[0] = 0;
      ++_NcKit;
    }
    break;
    
  default:
    ++_NcKit;
    for (int i = 0; i < _c-1; i++)
      _IdxK[i] = _aK[_NcKit(i+1)];
    _IdxK[_c-1] = _aK[_NcKit(0)];
    break;
  }
  if (IsEnded())
    Reset();

}


void AleaIterator::Print()
{
  cout << "Index:\t";
  for (int i = 0; i < _c; i++)
    cout << N(i) << "\t";
  cout << endl << "Value:\t";
  for (int i = 0; i < _c; i++)
    cout << K(i) << "\t";
  cout << endl << "--------------------" << endl;
}


void AleaIterator::Reset()
{
  _IdxC = 0;
  _NcKit.Reset(false);
  switch (_c)
  {
  case 1:
    _IdxK[0] = 0;
    break;

  default:
    for (int i = 0; i < _c-1; i++)
      _IdxK[i] = _aK[_NcKit(i+1)];
    _IdxK[_c-1] = _aK[_NcKit(0)];
    break;
  }
}

