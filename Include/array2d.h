//==============================================================================
// 
// Class Array2d: (2-Dimensional Array)
// - creation of 2D-array
//
//==============================================================================

#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <assert.h>

class Array2d
{
public:
  /** Default constructor */
  Array2d();
  
  /**
   * Constructor by specifying numbers of rows and columns
   * @param iNbRow:    number of array's rows
   * @param iNbColumn: number of array's column
   */
  Array2d(int iNbRow, int iNbColumn);
  
  /** Constructor by copy */
  Array2d(Array2d & iArray2d);
  
  
  /** Destructor */
  ~Array2d();
  
  /**
   * Resize the array. (/!\ DELETE THE WHOLE DATA IN THE ARRAY /!\)
   * @param iNbRow:    new number of array's rows
   * @param iNbColumn: new number of array's column
   */
  void Resize(int iNbRow, int iNbColumn);
  
  /** Return the number of rows of the array */
  inline int GetNbRow();

  /** Return the number of columns of the array */
  inline int GetNbColumn();

  /** Print the array */
  void Print();

  /**
   * Reading and writing access to coefficients
   * @param i: row's index
   * @param j: column's index
   */
  inline bool& operator()(int i, int j);
  inline const bool& operator()(int i, int j)const;
  
protected:
  int _NbRow;     // Number of rows
  int _NbColumn;  // Number of columns
  bool* _aTab;    // Array of double
};


//==============================================================================
// Implementation of inline methods
//==============================================================================

inline int Array2d::GetNbRow() {
  return _NbRow;
}

inline int Array2d::GetNbColumn() {
  return _NbColumn;
}

inline bool& Array2d::operator()(int i, int j)
{
  assert(0<=i && i<_NbRow && 0<=j && j<_NbColumn);
  return _aTab[_NbColumn*i+j];
}

inline const bool& Array2d::operator()(int i, int j)const
{
  assert(0<=i && i<_NbRow && 0<=j && j<_NbColumn);
  return _aTab[_NbColumn*i+j];
}


#endif


