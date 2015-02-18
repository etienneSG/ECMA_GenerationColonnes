#include "RandomIterator.h"
#include <iostream>
#include <algorithm>


RandomIterator::RandomIterator(int iN):
  _n(iN),
  _v(iN, 0),
  _it(_v.begin())
{
  for (int i = 0; i < _n; i++)
    _v[i] = i;
  std::random_shuffle(_v.begin(), _v.end());
}


RandomIterator::~RandomIterator()
{
}


void RandomIterator::Reset()
{
  std::random_shuffle(_v.begin(), _v.end());
  _it = _v.begin();
}

