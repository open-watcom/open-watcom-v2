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
// Created: MJF 03/27/89 -- Initial design
// Updated: MBN 04/18/89 -- Modification to support dynamic vectors, provide
//                          mechanism to define default allocation size, and
//                          separation of arithmetic vector to another class.
// Updated: MBN 05/08/89 -- Parameterized the class Vector<Type>
// Updated: MBN 05/26/89 -- Removed the non-destructive forms of the methods
//                          and implemented the notion of current position.
// Updated: MBN 06/07/89 -- Added growth ratio slot and resize method
// Updated: MBN 06/20/89 -- Changed returns types from Vector<Type>& to void
// Updated: LGO 07/13/89 -- Inherit from Generic
// Updated: MBN 08/15/89 -- Added const qualifer to arguments where necessary
// Updated: MBN 08/20/89 -- Changed usage of template to reflect new syntax
// Updated: MBN 08/31/89 -- Added conditional exception handling and base class
// Updated: LGO 10/02/89 -- Make destructor inline
// Updated: MBN 10/10/89 -- Changed "current_position" to "curpos" 
// Updated: MBN 10/19/89 -- Added optional argument to set_compare method
// Updated: MBN 11/01/89 -- Added constructor with user-provided storage param
// Updated: LGO 11/09/89 -- Initialize alloc_size and growth_ratio
// Updated: MBN 11/13/89 -- Optimize sort for one element case
// Updated: LGO 12/05/89 -- Performance enhancements to most methods
// Updated: LGO 12/05/89 -- Make sort/merge predicate ANSI compatable
// Updated: LGO 12/12/89 -- Make copy, fill and search take exclusive end args.
// Updated: LGO 01/02/89 -- Add get and put methods
// Updated: MBN 02/22/90 -- Changed size arguments from long to unsigned long
// Updated: MJF 05/31/90 -- Use memcpy in resize
// Updated: MJF 05/31/90 -- Use "delete [] data"
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version, copy data with = of Type class
// Updated: JAM 08/12/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/12/92 -- modernize template syntax, remove macro hacks
// Updated: JAM 09/28/92 -- index/size/pos type changed from [u]long to size_t
// 
//
// The Vector<Type> class is publicly derived from  the  Vector class and
// implements single dimensional vectors   of a user specified type.    This is
// accomplished by using    the   parameterized type capability   of C++.   The
// Vector<Type>  class   is  dynamic  in the  sense  that an   object can  grow
// dynamically if necessary.   The growth size is determined  by the value of a
// static  allocation  size variable  for  the  class.  However,  fixed  length
// vectors are also supported by setting this variable to INVALID.
// 
// Each vector object contains a protected data section that has a slot to hold
// the current  size  of the  vector and a pointer  to an allocated block large
// enough to hold "size" elements of type "Type". A  slot to hold the number of
// elements is also provided. A single protected  slot contains  a pointer to a
// compare  function to be  used in equality operations.  The  default function
// used is the built-in == operator.  Finally, a current  position slot is used
// to maintain the index of the last element affected by an operation.
//
// Five different constructors are  provided.  The  first constructor  takes no
// arguments and creates an empty Vector object of a specified type. The second
// constructor takes  a single  argument  specifying the  initial   size of the
// Vector.  The third   constructor takes two  required   arguments,  the first
// specifiying  the inital  size  of  the Vector  and  the  second providing an
// initialization value to  assign to  each element.  The fourth constructor is
// similar to  the third, except that it  takes a  variable number of arguments
// after the size to allow for the initialization of any  number of elements of
// the Vector to different  arbitrary values.  Finally, the  fifth  constructor
// takes a single argument consisting of a reference to a Vector and duplicates
// its size and element values.
// 
// The Vector class implements the notion of a current position. This is useful
// for iterating through the  elements  of a  Vector.  The current  position is
// maintained in an  integer and  is   set  or reset  by all methods  affecting
// elements   in the Vector  class. Methods  to   reset,  move to the  next and
// previous, find, and get the value at the current position are provided.
//
// Methods are provided  to  destructively   perform reverse, remove,  replace,
// prepend, append, merge, sort, and insert operations on a Vector object.   In
// addition, several miscelaneous methods  support  resize, copy, fill, length,
// search,  push, push_new, pop,  and   position functions.  The assignment and
// element accessor functions allow for individual elements of the vector to be
// set and read.  Finally, both equality and non-equality tests are implemented
// via  a user-defined  comparison function  as  well as  overloaded input  and
// output operators.
//

#ifndef VECTORH                                 // If no definition for Vector
#define VECTORH                                 // define the vector symbol


#ifndef BASE_VECTORH                            // If no definition for base
#include <cool/Base_Vector.h>                   // Include the class header
#endif

#include <stddef.h>                             // for size_t
#include <stdarg.h>                             // for variable arglist
#include <new.h>                                // include the new header file

template <class Type>
class CoolVector : public CoolBaseVector {
public:
  CoolVector ();                                // Vector v; 
  CoolVector (size_t);          // Vector v(10);
  CoolVector (void*, size_t);   // Vector with static storage
  CoolVector (size_t, const Type&); // Vector v(10,x);
  CoolVector (size_t, size_t, Type, ...); // Vector v(10,3,x,y,z);
  CoolVector (const CoolVector&);        // CoolVector v = y;
  ~CoolVector ();                                // Destructor
  
  CoolVector& operator= (const CoolVector&); // v = vec1;
  CoolVector& operator= (const Type&);           // v = 0; (set elements)
  inline Type& operator[](size_t) const;                 // return element
  inline Type& get(size_t n);                            // Return element, set curpos
  inline Boolean put(const Type& x, size_t n);           // Sets nth data-node to x
  
  Boolean operator== (const CoolVector&) const; // Compare 2 CoolVectors 
  inline Boolean operator!= (const CoolVector&) const; // Compare 2 CoolVectors
  
  template< class U >
  friend ostream& operator<< (ostream&, const CoolVector<U>&); // output
  template< class U >
  inline friend ostream& operator<< (ostream&, const CoolVector<U>*);
  
  inline Type& value ();                        // Value at current position
  inline size_t position () const;              // Return current position 
  inline size_t set_length (size_t);            // Set length of CoolVector
  inline void set_growth_ratio (float);         // Set growth ration
  inline void set_alloc_size (size_t);          // Set allocation size
  void set_compare (Boolean (*comp)(const Type&, const Type&) = NULL); // Compare function
  
  void resize (size_t);                         // Resize vector
  void fill (const Type&, size_t start, size_t end); // set elements
  void fill (const Type& value, size_t start=0) // set elements thru length
    { fill(value, start, length()); }

  void copy (const CoolVector&, size_t start, size_t end);
  void copy (const CoolVector& value, size_t start = 0)
    { copy(value, start, length()); }

  inline size_t position (const Type& , int dir = +1); // Index of 1st occurrence
  Boolean find (const Type&, size_t start, int dir = +1); 
  Boolean search (const CoolVector& sequence, size_t start, size_t end);
  Boolean search (const CoolVector& sequence, size_t start=0)
    { return search(sequence, start, length()); }

  Type& pop ();                                 // Remove/return last element
  Boolean push (const Type&);                   // Add element to end of vector
  Boolean push_new (const Type&);               // Append element if not there

  Boolean replace (const Type&, const Type&, int dir = +1);// Replace first occur
  Boolean replace_all (const Type&, const Type&); // Replace all occurrences
  Boolean prepend (const CoolVector&);  // Prepend vector 
  Boolean append (const CoolVector&);   // Append vector
  void reverse ();                              // Reverse order of elements
  void merge (const CoolVector&, int (*pred)(const Type&, const Type&)); // Merge
  void sort (Boolean (*pred)(const Type&, const Type&));                               // Sort 
  
  Type remove ();                               // Remove item at curpos 
  Boolean remove (const Type&, int dir = +1);   // Shifts to preserve relative order
  Boolean remove_duplicates ();                 // O(n^2) time.
  Boolean insert_before (const Type&);  // before curpos
  Boolean insert_after (const Type&);   // after curpos
  Boolean insert_before (const Type&, size_t index); // before index
  Boolean insert_after (const Type&, size_t index);  // after index

  // shuffle current and last elmt instead of shifting elmts to preserve order
  Type shuffle_remove ();                       // Remove and shuffle last elmt up
  Boolean shuffle_remove (const Type&, int dir = +1); // to fill hole.
  Boolean shuffle_insert_before (const Type&); // before curpos
  Boolean shuffle_insert_after (const Type&);  // after curpos
  Boolean shuffle_insert_before (const Type&, size_t index); // before index
  Boolean shuffle_insert_after (const Type&, size_t index);  // after index

protected:
  Type* data;                                   // Pointer to allocated storage
  static Boolean (*compare_s)(const Type&, const Type&);        // Pointer operator== function
  void grow (size_t min_size);                  // Make the CoolVector bigger

private:
  template< class U >
  friend Boolean CoolVector_is_data_equal (const U& t1, const U& t2);
};

// Type& value () -- Return value at current position.
// Input:            None
// Output:           Type reference to value at current position

template<class Type> 
inline Type& CoolVector<Type>::value () {
#if ERROR_CHECKING
  if (this->curpos == INVALID_POS)                      // If INVALID current position
    this->value_error (#Type);                  // Raise exception
#endif
  return (this->data[this->curpos]);
}


// Type& operator[] () -- Access an individual element from a vector.
//                        Range checking is not performed.
// Input:                 Integer index of element to access
// Output:                Type reference of element

template<class Type> 
inline Type& CoolVector<Type>::operator[] (size_t n) const {
  return this->data[n];
}


// get() -- Returns the element of THIS.
// Input:   A positive integer index.
// Output:  A Type reference of data in the nth element of THIS.

template <class Type> 
inline Type& CoolVector<Type>::get(size_t n) {
#if ERROR_CHECKING  
  if (n >= this->number_elements) // If index out of range
    this->bracket_error (#Type, n);             // Raise exception
#endif
  this->curpos = n;                             // Update current position
  return this->data[n];
}

// put() -- Replaces the data slot of the CoolVector element and if
//          successful, returns the  new data item.
// Input:   A Type reference and a positive integer index.
// Output:  TRUE if nth element exists, FALSE otherwise.

template <class Type> 
inline Boolean CoolVector<Type>::put(const Type& x, size_t n) {
  if (n >= this->number_elements)               // False if index out of range
    return FALSE;
  else {
    this->curpos = n;                           // Update current position
    this->data[n] = x;                          // Store data
    return TRUE;
  }
}

  
// Boolean operator!= -- Test for inequality of the data of two vectors
// Input:                Reference to CoolVector object
// Output:               TRUE/FALSE

template<class Type> 
inline Boolean CoolVector<Type>::operator!= (const CoolVector<Type>& v) const {
  return (!operator== (v));
}


// size_t position () -- Return current position.
// Input:              None
// Output:             Integer representing current position

template<class Type> 
inline size_t CoolVector<Type>::position () const {
  return this->CoolBaseVector::position ();
}


// size_t position () -- Find first occurrence of element in a CoolVector,
//                     from start or end of vector respectively if dir = +1, -1.
// Input:              Element value searching for, and search direction
// Output:             Integer representing index or -1 if not found

template<class Type> 
inline size_t CoolVector<Type>::position (const Type& value, int dir) {
  size_t start = 0;                     // start from beginning
  if (dir == -1) start = this->number_elements - 1; // or end of array
  if (this->find(value, start, dir))            // search least or most 
    return this->curpos;                        // recent first
  else 
    return INVALID_POS();
}

// size_t set_length () -- Set the number of elements in a CoolVector object.
//                       If there is not enough storage allocated, set to
//                       maximum size for storage.
// Input:                Length value
// Output:               Integer representing number of elements

template<class Type> 
inline size_t CoolVector<Type>::set_length (size_t n) {
  this->CoolBaseVector::set_length (n, "Type"); // Call base class with type
  return this->length();                        // Return length
}


// void set_alloc_size () -- Set the default allocation size growth rate.
// Input:                    Growth size in number of elements
// Output:                   None

template<class Type> 
inline void CoolVector<Type>::set_alloc_size (size_t n) {
  this->CoolBaseVector::set_alloc_size (n, "Type");     // Call base class with size
}


// void set_growth_ratio () -- Set the growth percentage for the CoolVector object
// Input:                      Float ratio
// Output:                     None

template<class Type> 
inline void CoolVector<Type>::set_growth_ratio (float ratio) {
  this->CoolBaseVector::set_growth_ratio (ratio, "Type"); // Call base class with size
}

// ostream& operator<< () -- Overload the output operator for CoolVector pointer
// Input:                    Ostream reference and CoolVector pointer
// Output:                   Ostream reference

template<class Type>
inline ostream& operator<< (ostream& os, const CoolVector<Type>* v) {
  return operator<< (os, *v);
}

#include <cool/Vector.C>   // required for most template implementations

#endif                                          // End ifdef of VECTORH

