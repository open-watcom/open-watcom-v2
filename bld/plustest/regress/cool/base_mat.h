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
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/24/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/24/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolMatrix=>CoolBase_Matrix
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
// The CoolBase_Matrix class is the base class  for the parameterized CoolMatrix<Type> class
// and   implements all non-type   specific  functionality. The  protected data
// section  contains two slots  to maintain the  row and column  specification.
// There are two  constructors  for  the  CoolBase_Matrix  class.  The  first takes  two
// arguments specifying the  number of rows  and columns. The   second takes  a
// reference to a CoolBase_Matrix object and reproduces its state. There  are two public
// methods that provide accessors to the  row and column slots.  Finally, there
// are several protected methods that are called by  the parameterized class to
// handle exceptions.

#ifndef BASE_MATRIXH                            // If no CoolBase_Matrix class,
#define BASE_MATRIXH                            // define it

#ifndef STREAMH                 // If the Stream support not yet defined,
#include <iostream.h>           // include the Stream class header file
#define STREAMH
#endif

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

class CoolBase_Matrix {
public:
  inline int rows () const;                     // Return number of rows
  inline int columns () const;                  // Return number of columns

protected:
  int num_rows;                                 // Number of rows
  int num_cols;                                 // Number of columns
  
  void row_index_error (const char* fcn, const char* type, int) const; // Raise
  void col_index_error (const char* fcn, const char* type, int) const; // exception
  void dimension_error (const char* fcn, const char* type, int,int,int,int) const;
  void va_arg_error (const char*, int);         // Raise exception

  CoolBase_Matrix (int r=1, int c=1);           // CoolBase_Matrix m (r,c);
  CoolBase_Matrix (const CoolBase_Matrix&);             // m1 = m2;
  ~CoolBase_Matrix();                           // Destructor
};


// rows -- Return the number of rows in the matrix
// Input:  None
// Output: Number of rows

inline int CoolBase_Matrix::rows () const {
  return this->num_rows;                        // Return number of rows
}


// columns -- Return the number of columns in the matrix
// Intput:    None
// Output:    Number of columns

inline int CoolBase_Matrix::columns () const {
  return this->num_cols;                        // Return number of columns
}

#endif                                          // End of BASE_MATRIXH


