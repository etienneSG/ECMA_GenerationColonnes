#include "AleaIterator.h"

#include <iostream>
#include <stdlib.h>

using namespace std;


AleaIterator::AleaIterator():
  _n(0),
  _k(0),
  _c(0),
  _IdxN(1,-1),
  _IdxK(1,0),
  _IdxC(0)
{
}


AleaIterator::AleaIterator(int iK,int iN, int iC):
  _n(iN),
  _k(iK),
  _c(iC),
  _IdxN(iC+1,0),
  _IdxK(iC+1,0),
  _IdxC(0)
{
  _IdxN[0] = -1;
}


AleaIterator::~AleaIterator()
{
}


void AleaIterator::operator++()
{
  if (_IdxC < _c-1)
    _IdxC++;
  else
  {
    _IdxC = 0;
    if (_IdxK[1] < _k-1)
      _IdxK[1]++;
    else
    {
      _IdxK[1] = 0;
      if (_IdxN[1] < _n-1)
        _IdxN[1]++;
      else
      {
        Reset();
        _IdxN[0]++;
      }
    }
  }

  for (int i = 2; i <= _c; i++)
  {
    _IdxN[i] = rand()%_n;
    _IdxK[i] = rand()%_k;
  }
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
  for (int i = 1; i <= _c; i++)
  {
    _IdxN[i] = 0;
    _IdxK[i] = 0;
  }
  _IdxN[0] = -1;
  _IdxK[0] = 0;
}
