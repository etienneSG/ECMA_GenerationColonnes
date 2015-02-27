//==============================================================================
// 
// Class AleaIterator:
//   Pseudo-aleatoire iterator on "n choose c" elements and each of them can
// take a integer value in [0, k[.
// n : Number of elements in the set
// c : In n choose c elements
// k : Range of the values taken by each element
//   This iterator do not iterate on every value but on O(n*k*c) values.
//
//==============================================================================

#ifndef ALEAITERATOR_H
#define ALEAITERATOR_H

#include "kcombinationiterator.h"

#include <vector>
#include <assert.h>


class AleaIterator
{
public:
  /** Default constructor */
  AleaIterator();
  
  /**
   * Constructor
   * @param iK: number of elements in box
   * @param iN: number of elements in the set
   * @param iC: number of elements to chose in [0, iN[
   * @param iDataK: array of N value in [0, iK[
   */
  AleaIterator(int iK,int iN, int iC, int * iaDataK = 0);
  
  /** Destructor */
  ~AleaIterator();
  
  /** Iterator on the set of k-combinations from a set of n elements */
  void operator++();
  
  /**
   * Return the index of the iIdx-th element (0 <= iIdx < c)
   * @param iIdx: one of the iIdx-th element
   * @return index of the iIdx-th element (in [0, n[)
   */
  inline int N(int iIdx);

  /**
   * Return the value of the iIdx-th element (0 <= iIdx < c)
   * @param iIdx: value of the iIdx-th element
   * @return index of the iIdx-th element (in [0, k[)
   */
  inline int K(int iIdx);
  
  /**
   * Print the indexes of the c current chosen elements (1st line)
   * and their values (2nd line)
   */
  void Print();
  
  /** Return false while the iterator has not iterate on every element */
  inline bool IsEnded();
  
  /** Reset the iterator at the begining */
  void Reset();
  
private:
  int _n;                 // Number of elements in the set
  int _k;                 // Range of the values taken by each element
  int _c;                 // In n choose c elements
  int * _aK;              // Array of N value in [0, iK[
  
  KcombinationIterator _NcKit; // Iterator on the element of "N choose K"
  std::vector<int> _IdxK;
  int _IdxC;

};


//==============================================================================
// Implementation of inline methods
//==============================================================================

inline bool AleaIterator::IsEnded() {
  return _NcKit.IsEnded();
}

inline int AleaIterator::N(int iIdx) {
  return _NcKit(iIdx);
}

inline int AleaIterator::K(int iIdx) {
  assert(0 <= iIdx && iIdx < _c);
  return _IdxK[iIdx];
}

#endif
