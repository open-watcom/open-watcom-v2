//
// Copyright (C) 1992 General Electric Company
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// General Electric Company provides this software "as is" without
// express or implied warranty.
//
// Updated: VDN 02/21/92 -- New lite version
//

#ifndef BASE_M_VECTORH                          // If no CoolBase_M_Vector class,
#include <cool/Base_M_Vector.h>                 // Include header file
#endif  

// CoolBase_M_Vector -- Constructor specifiying size of vector
// Input:    length
// Output:   None

CoolBase_M_Vector::CoolBase_M_Vector (unsigned int elmts) {
#if ERROR_CHECKING
  if (elmts <= 0)       {                       // If invalid size specified
    //RAISE (Error, SYM(CoolBase_M_Vector), SYM(Invalid_Length),
    printf ("CoolBase_M_Vector::CoolBase_M_Vector(): Invalid size %d specified for length.\n", elmts);
    abort ();
  }
#endif
  this->num_elmts = elmts;                      // Save elmt count
}


// CoolBase_M_Vector -- constructor for reference to another CoolBase_M_Vector object
// Input:    CoolBase_M_Vector reference
// Output:   None

CoolBase_M_Vector::CoolBase_M_Vector (const CoolBase_M_Vector& m) {
  this->num_elmts = m.num_elmts;                // Copy elmt count
}


// CoolBase_M_Vector -- Destructor for CoolBase_M_Vector
// Input:    None
// Output:   None

CoolBase_M_Vector::~CoolBase_M_Vector () {;}


// index_error -- Raise exception for invalid row index.
// Input:           function string, type string, index specification
// Output:          None

void CoolBase_M_Vector::index_error (const char* fcn, const char* type, int index) const {
  //RAISE (Error, SYM(CoolBase_M_Vector), SYM(Invalid_Index),
  printf ("CoolBase_M_Vector<%s>::%s: Invalid value %d specified for index.\n", 
          type, fcn, index);
  abort ();
}

// dimension_error -- Raise exception for invalid dimensions
// Input:           function string, type string, rowXcol,rowXcol
// Output:          None

void CoolBase_M_Vector::dimension_error (const char* fcn, const char* type, 
                               int l1, int l2) const {
  //RAISE (Error, SYM(CoolBase_M_Vector), SYM(Invalid_Dim),
  printf ("CoolBase_M_Vector<%s>::%s: Dimensions [%d] and [%d] do not match.\n", 
          type, fcn, l1, l2);
  abort ();
}


// va_arg_error -- Raise exception for using class objects, or chars in (...)
// Input:          Type string
// Output:         None

void CoolBase_M_Vector::va_arg_error (const char* Type, int n) {
  //RAISE (Error, SYM(CoolBase_M_Vector), SYM(Invalid_Va_Arg),
  printf ("CoolBase_M_Vector<%s>::CoolBase_M_Vector<%s>(): Invalid type in ... or wrong alignment with %d bytes.\n",
          Type, Type, n);
  abort ();
}


