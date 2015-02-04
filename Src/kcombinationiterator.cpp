#include "kcombinationiterator.h"

#include <iostream>

KcombinationIterator::KcombinationIterator():
  _k(0),
  _n(0)
{
}


KcombinationIterator::KcombinationIterator(int iK,int iN):
  _k(iK),
  _n(iN),
  _v(iK+1,iN)
{
  for (int i = 0; i < iK+1; i++)
    _v[i]=i-1;
}


KcombinationIterator::~KcombinationIterator()
{
}


void KcombinationIterator::operator++()
{
  int l=_k;
  _v[l]++;
  while (l > 0 && _v[l] >= (_n+l-_k) )
  {
    _v[l-1]++;
    _v[l]=0;
    l--;
  }
  unsigned int i;
  for (i = 1 ; i < _v.size(); i++)
  {
    if (_v[i]<=_v[i-1])
      _v[i]=_v[i-1]+1;
  }
}

void KcombinationIterator::Print()
{
  unsigned int i;
  for(i = 1; i < _v.size(); i++)
    std::cout<<_v[i]<<" ";
  std::cout<<"\n";   
}

