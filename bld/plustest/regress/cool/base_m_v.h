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
// Updated: JAM 08/15/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/15/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolM_Vector=>CoolBase_M_Vector
//
// The parameterized M_Vector<Type>  class is publicly   derived from the  Base_M_Vector
// class and implements one dimensional arithmetic vectors of a user specified
// type.   This is accompilshed by using  the parameterized  type capability of
// C++.  The only constraint placed on the type  is  that it must  overload the
// following operators: +, -,  *,  and /. Thus, it will  be possible to have  a
// vector of  type Complex.  The M_Vector<Type> class  is static in size, that is
// once a  M_Vector<Type> of  a particular  size has been   declared, there is no
// dynamic growth or resize method available.
//
// The Base_M_Vector class is the base class for the parameterized  M_Vector<Type> class
// and implements all non-type specific functionality. The private data section
// contains a slot to maintain  the length specification. There are
// two constructors  for  the Base_M_Vector  class. The   first   takes  two arguments
// specifying the number of elements. The second takes a reference to a
// Base_M_Vector object and  reproduces its state. There  is one pulic method that
// provides access to the length slot.  Finally, there are  several
// private  methods  that  are called   by the   parameterized class to  handle
// exceptions.

#ifndef BASE_M_VECTORH                          // If no Vec class,
#define BASE_M_VECTORH                          // define it

#ifndef STREAMH                 // If the Stream support not yet defined,
#include <iostream.h>           // include the Stream class header file
#define STREAMH
#endif

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

class CoolBase_M_Vector {
public:
  inline int length () const;                   // Return number of elements

protected:
  unsigned int num_elmts;                       // Number of elements
  
  void index_error (const char* fcn, const char* type, int) const; // Raise except.
  void dimension_error (const char* fcn, const char* type, int, int) const;
  void va_arg_error (const char*, int);         // Raise exception

  CoolBase_M_Vector (unsigned int elmts=1);             // Vec v (n);
  CoolBase_M_Vector (const CoolBase_M_Vector&);         // Vec m1(m2);
  ~CoolBase_M_Vector();                         // Destructor
};


// length -- Return the number of elements in the vector

inline int CoolBase_M_Vector::length () const {
  return this->num_elmts;                               
}


#endif                                          // End of BASE_M_VECTORH


