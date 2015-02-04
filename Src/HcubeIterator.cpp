#include "HcubeIterator.h"
#include <iostream>

HcubeIterator::HcubeIterator():
  _n(0),
  _k(0)
{
}


HcubeIterator::HcubeIterator(int iK,int iN):
  _n(iN),
  _k(iK),
  _v(iN+1, 0)
{
  _v[0]=-1;
}


HcubeIterator::~HcubeIterator()
{
}


void HcubeIterator::operator++()
{
  int l=_n;
  _v[l]++;
  while (l > 0 && _v[l] >= _k )
  {
    _v[l-1]++;
    _v[l]=0;
    l--;
  }
}

void HcubeIterator::Print()
{
  unsigned int i;
  for(i = 1; i < _v.size(); i++)
    std::cout<<_v[i]<<" ";
  std::cout<<"\n";   
}


