//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
// Created: MBN 09/11/89 -- Initial design and implementation
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: VDN 02/21/92 -- New lite version
//
// The parameterized CoolMatrix<Type>  class is publicly   derived from the  CoolBase_Matrix
// class and implements two dimensional arithmetic matrices of a user specified
// type.   This is accompilshed by using  the parameterized  type capability of
// C++.  The only constraint placed on the type  is  that it must  overload the
// following operators: +, -,  *,  and /. Thus, it will  be possible to have  a
// matrix of  type Complex.  The CoolMatrix<Type> class  is static in size, that is
// once a  CoolMatrix<Type> of  a particular  size has been   declared, there is no
// dynamic growth or resize method available.
//
// The CoolBase_Matrix class is the base class for the parameterized  CoolMatrix<Type> class
// and implements all non-type specific functionality. The private data section
// contains two slots to maintain  the row  and column specification. There are
// two constructors  for  the CoolBase_Matrix  class. The   first   takes  two arguments
// specifying the number of rows and columns. The second takes a reference to a
// CoolBase_Matrix object and  reproduces its state. There  are  two public methods that
// provide accessors to the row  and column slots.  Finally, there are  several
// private  methods  that  are called   by the   parameterized class to  handle
// exceptions.

#ifndef BASE_MATRIXH                            // If no CoolBase_Matrix class,
#include <cool/Base_Matrix.h>                   // Include header file
#endif  

// CoolBase_Matrix -- Constructor specifiying size of matrix
// Input:    Row, Column parameters
// Output:   None

CoolBase_Matrix::CoolBase_Matrix (int row, int col) {
#if ERROR_CHECKING
  if (row <= 0) {                               // If invalid size specified
    //RAISE (Error, SYM(CoolBase_Matrix), SYM(Invalid_Row),
    printf ("CoolBase_Matrix::CoolBase_Matrix(): Invalid size %d specified for row.\n", row);
    abort ();
  }
  if (col <= 0) {                               // If invalid size specified
    //RAISE (Error, SYM(CoolBase_Matrix), SYM(Invalid_Col),
    printf ("CoolBase_Matrix::CoolBase_Matrix(): Invalid size %d specified for column.\n", col);
    abort ();
  }
#endif
  this->num_rows = row;                         // Save row count
  this->num_cols = col;                         // Save column count
}


// CoolBase_Matrix -- constructor for reference to another CoolBase_Matrix object
// Input:    CoolBase_Matrix reference
// Output:   None

CoolBase_Matrix::CoolBase_Matrix (const CoolBase_Matrix& m) {
  this->num_rows = m.num_rows;                  // Copy row size
  this->num_cols = m.num_cols;                  // Copy column size
}


// CoolBase_Matrix -- Destructor for CoolBase_Matrix
// Input:    None
// Output:   None

CoolBase_Matrix::~CoolBase_Matrix () {;}


// row_index_error -- Raise exception for invalid row index.
// Input:           function string, type string, row specification
// Output:          None

void CoolBase_Matrix::row_index_error (const char* fcn, const char* type, int r) const {
  //RAISE (Error, SYM(CoolBase_Matrix), SYM(Invalid_Row),
  printf ("CoolBase_Matrix<%s>::%s: Invalid value %d specified for row.\n", 
          type, fcn, r);
  abort ();
}


// col_index_error -- Raise exception for invalid col index.
// Input:           function string, type string, column specification
// Output:          None

void CoolBase_Matrix::col_index_error (const char* fcn, const char* type, int c) const {
  //RAISE (Error, SYM(CoolBase_Matrix), SYM(Invalid_Col),
  printf ("CoolBase_Matrix<%s>::%s: Invalid value %d specified for column.\n", 
          type, fcn, c);
  abort ();
}

// dimension_error -- Raise exception for invalid dimensions
// Input:           function string, type string, rowXcol,rowXcol
// Output:          None

void CoolBase_Matrix::dimension_error (const char* fcn, const char* type, 
                                  int r1, int c1, int r2, int c2) const {
  //RAISE (Error, SYM(CoolBase_Matrix), SYM(Invalid_Dim),
  printf ("CoolBase_Matrix<%s>::%s: Dimensions [%d,%d] and [%d,%d] do not match.\n", 
          type, fcn, r1, c1, r2, c2);
  abort ();
}

// va_arg_error -- Raise exception for using class objects, or chars in (...)
// Input:          Type string
// Output:         None

void CoolBase_Matrix::va_arg_error (const char* Type, int n) {
  //RAISE (Error, SYM(CoolBase_Matrix), SYM(Invalid_Va_Arg),
  printf ("CoolBase_Matrix<%s>::CoolBase_Matrix<%s>(): Invalid type in ... or wrong alignment with %d bytes.\n",
          Type, Type, n);
  abort ();
}
