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
// Created: MBN 08/31/89 -- Initial design and implementation
// Updated: VDN 02/21/92 -- new lite version
// Updated: JAM 08/12/92 -- removed #ifdef __cplusplus for static data
//                          member growth_rate definition and initialization
//
// The CoolBaseVector class is no longer derived from the Generic class  and is used to
// implement non-type specific functionality for the parameterized CoolVector<Type>
// class. In this manner, code common to all instances of  the CoolBaseVector class can
// be shared to reduce code  replication. The CoolVector<Type>  class is dynamic in
// the sense that an object can grow dynamically if necessary.  The growth size
// is determined by the  value  of a  static  allocation size variable  for the
// class.  However, fixed length vectors  are also  supported by  setting  this
// variable  to INVALID.   The CoolBaseVector class  implements the notion of a current
// position. This is useful  for iterating through  the  elements of a  vector.
// The current position is maintained in an integer and  is set or reset by all
// methods  affecting elements in the  CoolBaseVector class. Methods to reset,  move to
// the next and previous, find,  and get the value at  the current position are
// provided.
//

#ifndef BASE_VECTORH                            // If no CoolBaseVector class
#include <cool/Base_Vector.h>                   // Include the class header
#endif

float CoolBaseVector::growth_ratio = 0.0;               // Initialize growth ratio

// CoolBaseVector () -- Empty constructor for the CoolBaseVector class.
// Input:       None
// Output:      None

CoolBaseVector::CoolBaseVector () {
  this->size = 0;                               // No storage allocated
  this->number_elements = 0;                    // No elements in CoolBaseVector
  this->curpos = INVALID_POS();                 // Invalidate current position
  alloc_size = MEM_BLK_SZ;                      // Set the default size 
}

// CoolBaseVector (size_t) -- Constructor that specifies number of elements.
// Input:           Number of elements 
// Output:          None

CoolBaseVector::CoolBaseVector (size_t n) {
  this->size = n;                               // Element capacity
  this->number_elements = 0;                    // No elements in Vector
  this->curpos = INVALID_POS();                 // Invalidate current position
  alloc_size = MEM_BLK_SZ;                      // Set memory block size
}

// CoolBaseVector (CoolBaseVector&) -- Constructor for reference to another CoolBaseVector 
// Input:              CoolBaseVector reference
// Output:             None

CoolBaseVector::CoolBaseVector (const CoolBaseVector& v) {
  alloc_size = v.alloc_size;                    // Set the default size 
  if (alloc_size == INVALID_ALLOCSZ())                  // If invalid allocation size
    alloc_size = MEM_BLK_SZ;                    // Set default size
  this->size = v.size;                          // Copy element capacity
  this->number_elements = v.number_elements;    // Same number of elements
  this->curpos = INVALID_POS();                 // Invalidate current position
}

// ~CoolBaseVector () -- Destructor for CoolBaseVector class that frees up storage.
// Input:        None
// Output:       None

CoolBaseVector::~CoolBaseVector() {
}

// CoolBaseVector& operator= () -- Overload the assignment operator
// Input:                  Reference to CoolBaseVector object
// Output:                 Reference to copied Vector object

CoolBaseVector& CoolBaseVector::operator= (const CoolBaseVector& v) {
  this->number_elements = v.number_elements;    // Copy number of elements
  this->growth_ratio = v.growth_ratio;          // Copy the growth ratio
  this->curpos = INVALID_POS();                 // Invalidate current position
  return *this;                                 // Return CoolBaseVector reference
}

// clear() -- Removes all elements from this CoolBaseVector
//            (included for compatability with List)
// Input:     None.
// Output:    None.

void CoolBaseVector::clear() {
  this->reset();                                // make current position invalid   
  this->number_elements = 0;
}

// is_empty() -- Indicates empty CoolBaseVector
//               (included for compatability with List)
// Input:        None.
// Output:       TRUE or FALSE.

Boolean CoolBaseVector::is_empty() {
  return this->number_elements == 0;
}

// size_t set_length () -- Set the number of elements in a CoolBaseVector object.
//                       If there is not enough storage allocated, set to
//                       maximum size for storage.
// Input:                Length, type 
// Output:               Integer representing number of elements

size_t CoolBaseVector::set_length (size_t n, const char* Type) {
  if (n <= size)                                // If not greater than size
    this->number_elements = n;                  // Set new length
  else
    this->number_elements = size;               // Else set to maximum possible
  return this->number_elements;                 // Return value
}

// void set_growth_ratio (float) -- Set the growth percentage for the CoolBaseVector
//                                  object.
// Input:                           Float ratio, type
// Output:                          None

void CoolBaseVector::set_growth_ratio (float ratio, const char* Type) {
#if ERROR_CHECKING
  if (ratio <= 0.0) {                           // If non-positive growth
    //RAISE (Error, SYM(CoolBaseVector), SYM(Negative_Ratio),
    printf ("CoolVector<%s>::set_growth_ratio(): Negative growth ratio %f.\n",
            Type, ratio);
    abort ();
  }
#endif
  this->growth_ratio = ratio;                   // Adjust ration
}


// void set_alloc_size (size_t) -- Set the default allocation size growth rate.
// Input:                       Growth size in number of elements, type
// Output:                      None

void CoolBaseVector::set_alloc_size (size_t n, const char* Type) {
  this->alloc_size = n;                         // Set growth size
}

//----------------------------no error exception----------------------------


// bracket_error -- Raise exception for CoolVector<Type>::operator[]()
// Input:           Type string
// Output:          None

void CoolBaseVector::bracket_error (const char* Type, const long n) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Out_Of_Range),
  printf ("CoolVector<%s>::operator[](): Index %d out of range.\n", Type, n);
  abort ();
}

// value_error -- Raise exception for CoolVector<Type>::value()
// Input:         Type string
// Output:        None

void CoolBaseVector::value_error (const char* Type) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Invalid_Cpos),
  printf ("CoolVector<%s>::value(): Invalid current position.\n", Type);
  abort ();
}

// resize_error -- Raise exception for CoolVector<Type>::resize()
// Input:          Type string, size argument
// Output:         None

void CoolBaseVector::resize_error (const char* Type, const long n) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Negative_Size),
  printf ("CoolVector<%s>::resize(): Negative resize %d.\n", Type, n);
  abort ();
}

// static_error -- Raise exception for CoolVector<Type>::resize()
// Input:          Type string, size argument
// Output:         None

void CoolBaseVector::static_error (const char* Type) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Static_Size),
  printf ("CoolVector<%s>::resize(): Static-size CoolBaseVector.\n", Type);
  abort ();
}

// assign_error -- Raise exception for CoolVector<Type>::operator=()
// Input:          Type string
// Output:         None

void CoolBaseVector::assign_error (const char* Type) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Static_Size),
  printf ("CoolVector<%s>::operator=(): Static-size CoolBaseVector.\n", Type);
  abort ();
}

// fill_start_error -- Raise exception for CoolVector<Type>::fill()
// Input:              Type string, start argument
// Output:             None

void CoolBaseVector::fill_start_error (const char* Type, const long start) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Out_Of_Range),
  printf ("CoolVector<%s>::fill(): Start index %d out of range.\n", Type, start);
  abort ();
}


// fill_end_error -- Raise exception for CoolVector<Type>::fill()
// Input:            Type string, end argument
// Output:           None

void CoolBaseVector::fill_end_error (const char* Type, const long end) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Out_Of_Range),
  printf ("CoolVector<%s>::fill(): End index %d out of range.\n", Type, end);
  abort ();
}

// copy_start_error -- Raise exception for CoolVector<Type>::copy()
// Input:              Type string, start argument
// Output:             None

void CoolBaseVector::copy_start_error (const char* Type, const long start) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Out_Of_Range),
  printf ("CoolVector<%s>::copy(): Start index %d out of range.\n", Type, start);
  abort ();
}


// copy_end_error -- Raise exception for CoolVector<Type>::copy()
// Input:            Type string, end argument
// Output:           None

void CoolBaseVector::copy_end_error (const char* Type, const long end) const {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Out_Of_Range),
  printf ("CoolVector<%s>::copy(): End index %d out of range.\n", Type, end);
  abort ();
}

// copy_error -- Raise exception for CoolVector<Type>::copy()
// Input:        Type string
// Output:       None

void CoolBaseVector::copy_error (const char* Type) {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Static_Size),
  printf ("CoolVector<%s>::copy(): Static-size CoolBaseVector.\n", Type);
  abort ();
}


// remove_error -- Raise exception for CoolVector<Type>::remove()
// Input:          Type string
// Output:         None

void CoolBaseVector::remove_error (const char* Type) const { 
  //RAISE (Error, SYM(CoolBaseVector), SYM(Invalid_Cpos),
  printf ("CoolVector<%s>::remove(): Invalid current position.\n", Type);
  abort ();
}

// va_arg_error -- Raise exception for using class objects, or chars in (...)
// Input:          Type string
// Output:         None

void CoolBaseVector::va_arg_error (const char* Type, int n) {
  //RAISE (Error, SYM(CoolBaseVector), SYM(Invalid_Va_Arg),
  printf ("CoolVector<%s>::CoolVector<%s>(): Invalid type in ... or wrong alignment with %d bytes.\n",
          Type, Type, n);
  abort ();
}

