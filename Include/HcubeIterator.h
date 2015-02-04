//==============================================================================
// 
// Class HcubeIterator:
//   Iterator on the vertex of an hypercube.
// n : dimension of the hypercube
// k : number of subdivision of the hypercube
//
//==============================================================================

#ifndef HCUBEITERATOR_H
#define HCUBEITERATOR_H

#include <vector>
#include <assert.h>

class HcubeIterator
{
public:
  /** Default constructor */
  HcubeIterator();
  
  /**
   * Constructor
   * @param iK: number of elements to chose
   * @param iN: number of elements in the set
   */
  HcubeIterator(int iK,int iN);
  
  /** Destructor */
  ~HcubeIterator();
  
  /** Iterator on the set of k-combinations from a set of n elements */
  void operator++();
  
  /**
   * Return the index of the iIdx-th chosen element
   * @param iIdx: one of the k chosen elements
   * @return index of the iIdx-th chosen element
   */
  inline int operator()(int iIdx);

  /** Print the indexes of the k current chosen elements */
  void Print();
  
  /** Return false while the iterator has not iterate on every element */
  inline bool IsEnded();
  
private:
  int _n;               // dimension of the hypercube
  int _k;               // number of subdivision of the hypercube
  std::vector<int> _v;  // Vector of indexes of current vertex
};


//==============================================================================
// Implementation of inline methods
//==============================================================================

inline bool HcubeIterator::IsEnded() {
  return _v[0] >= 0;
}

inline int HcubeIterator::operator()(int iIdx) {
  assert(0 <= iIdx && iIdx < _k);
  return _v[iIdx+1];
}

#endif
