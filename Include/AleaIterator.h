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
   */
  AleaIterator(int iK,int iN, int iC);
  
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
  std::vector<int> _IdxN;
  std::vector<int> _IdxK;
  int _IdxC;
  
  std::vector<int> _Integer;  // Vecteur contenant les entiers de [0,N[
  
  /**
   * Swap (c-1) random number different from _IdxN[1] of _Integer to the begining
   * of the vector
   */
  void AleaNchooseK();
};


//==============================================================================
// Implementation of inline methods
//==============================================================================

inline bool AleaIterator::IsEnded() {
  return _IdxN[0] >= 0;
}

inline int AleaIterator::N(int iIdx) {
  assert(0 <= iIdx && iIdx < _c);
  return _IdxN[iIdx+1];
}

inline int AleaIterator::K(int iIdx) {
  assert(0 <= iIdx && iIdx < _c);
  return _IdxK[iIdx+1];
}

#endif
