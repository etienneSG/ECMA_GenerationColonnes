//==============================================================================
// 
// Class KcombinationIterator:
//   Iterator on the elements of the k-combinations from a set of n elements.
//
//==============================================================================

#ifndef KCOMBINATIONITERATOR_H
#define KCOMBINATIONITERATOR_H

#include <vector>
#include <assert.h>

class KcombinationIterator
{
public:
  /** Default constructor */
  KcombinationIterator();
  
  /**
   * Constructor
   * @param iK: number of elements to chose
   * @param iN: number of elements in the set
   */
  KcombinationIterator(int iK,int iN);
  
  /** Destructor */
  ~KcombinationIterator();
  
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
  int _k;               // Number of elements to chose
  int _n;               // Number of elements in the set
  std::vector<int> _v;  // Vector of indexes of current chosen elements
};


//==============================================================================
// Implementation of inline methods
//==============================================================================

inline bool KcombinationIterator::IsEnded() {
  return _v[0] >= 0;
}

inline int KcombinationIterator::operator()(int iIdx) {
  assert(0 <= iIdx && iIdx < _k);
  return _v[iIdx+1];
}

#endif
