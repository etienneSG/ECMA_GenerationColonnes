//==============================================================================
// 
// Class RandomIterator:
//   Random iterator on the integer of an intervalle [0, N[
// N : Number of elements
//
// WARNING ! Random must be initialize in the main function !
//
//==============================================================================


#ifndef RANDOMITERATOR_H
#define RANDOMITERATOR_H

#include <vector>

class RandomIterator
{
public:
  /**
   * Constructor
   * @param iN: Number of elements in the set. (Default value: 0)
   */
  RandomIterator(int iN = 0);
  
  /** Destructor */
  ~RandomIterator();
  
  /** Reset the operator */
  void Reset();
  
  /** Increment the iterator on the elements */
  inline void operator++();
  
  /** Return false while the iterator has not iterate on every element */
  inline bool IsEnded();
  
  /** Return current random integer of [0, N[ */
  inline int getNb();
  
private:
  int _n;                         // Number of elements (N)
  std::vector<int> _v;            // Vector of N elements
  std::vector<int>::iterator _it; // Iterator on the elements of the vector
};


//==============================================================================
// Implementation of inline methods
//==============================================================================


inline void RandomIterator::operator++() {
  ++_it;
}


inline bool RandomIterator::IsEnded() {
  return (_it == _v.end());
}


inline int RandomIterator::getNb() {
  return *_it;
}

#endif


