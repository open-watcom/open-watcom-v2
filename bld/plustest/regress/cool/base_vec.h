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
// Updated: MBN 10/03/89 -- Predecrement instead of postdecrement on prev()
// Updated: MBN 10/10/89 -- Added current_position() method for Iterator<Type>
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/12/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/12/92 -- renamed from CoolVector to CoolBaseVector
// Updated: JAM 08/18/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
// Updated: JAM 09/28/92 -- index/size type changed from [u]long to size_t
//
// The CoolBaseVector class is no longer derived from the Generic class and is used to
// implement non-type specific functionality for the parameterized CoolVector<Type>
// class. In this manner, code common to all instances of  the CoolBaseVector class can
// be shared to reduce code  replication. The CoolVector<Type>  class is dynamic in
// the sense that an object can grow dynamically if necessary.  The growth size
// is determined by the  value  of a  static  allocation size variable  for the
// class.  However, fixed length CoolBaseVectors  are also  supported by  setting  this
// variable  to INVALID.   The CoolBaseVector class  implements the notion of a current
// position. This is useful  for iterating through  the  elements of a  CoolBaseVector.
// The current position is maintained in an integer and  is set or reset by all
// methods  affecting elements in the  CoolBaseVector class. Methods to reset,  move to
// the next and previous, find,  and get the value at  the current position are
// provided.
// 
// Each CoolBaseVector object contains a protected data section that has a slot to hold
// the current  size  of the  CoolBaseVector and a pointer  to an allocated block large
// enough to hold "size" elements of type "Type". A  slot to hold the number of
// elements is also provided. A single protected  slot contains  a pointer to a
// compare  function to be  used in equality operations.  The  default function
// used is the built-in == operator.  Finally, a current  position slot is used
// to maintain the index of the last element affected by an operation.
//
// Three different constructors are  provided.  The first constructor  takes no
// arguments and creates an empty CoolBaseVector object. The second constructor takes a
// single argument specifying  the  initial  size  of  the  CoolBaseVector.   The third
// constructor takes takes a  single argument consisting of  a  reference  to a
// CoolBaseVector and duplicates its size and element values.
// 
// Methods are provided  to  destructively   perform reverse, remove,  replace,
// prepend, append, merge, sort, and insert operations on a CoolBaseVector object.   In
// addition, several miscelaneous methods  support  resize, copy, fill, length,
// search,  push, push_new, pop,  and   position functions.  The assignment and
// element accessor functions allow for individual elements of the CoolBaseVector to be
// set and read.  Finally, both equality and non-equality tests are implemented
// via  a user-defined  comparison function  as  well as  overloaded input  and
// output operators.
//

#ifndef BASE_VECTORH                    // If no definition for CoolBaseVector
#define BASE_VECTORH                    // define the CoolBaseVector symbol

#include <iostream.h>

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif


#define MEM_BLK_SZ 100

class CoolBaseVector {
public:
  typedef size_t IterState;                     // Current position state

  inline void reset ();                         // Invalidate current position
  inline Boolean next ();                       // Advance current position
  inline Boolean prev ();                       // Backup current position 
  inline size_t position () const;              // Return current position
  inline IterState& current_position ();// Set/Get current position

  CoolBaseVector& operator= (const CoolBaseVector&);    // Overload assignment operator
  void clear();                                 // Removes all elements
  Boolean is_empty();                           // Any elements in Vector?
  inline size_t length () const;                        // Number of elements
  inline size_t capacity () const;              // Maximum number of elements

  size_t set_length (size_t, const char*);              // Set number of elements
  void set_growth_ratio (float, const char*);   // Set growth percentage
  void set_alloc_size (size_t, const char*);    // Set alloc size

protected:
  size_t size;                                  // Size of allocated storage 
  size_t number_elements;                               // Number of elements in CoolBaseVector
  size_t alloc_size;                            // Allocation size for growth
  static float growth_ratio;                    // If non-zero, growth ratio 
  IterState curpos;                             // Keeps current position

  static size_t INVALID_POS() { return (size_t)-1; }
  static size_t INVALID_ALLOCSZ() { return (size_t)-1; }

  void bracket_error (const char*, const long) const;   // Raise exception
  void value_error (const char*) const;                 // Raise exception
  void resize_error (const char*, const long) const;    // Raise exception
  void static_error (const char*) const;                // Raise exception
  void assign_error (const char*) const;                // Raise exception
  void fill_start_error(const char*,const long) const;  // Raise exception
  void fill_end_error (const char*, const long) const;  // Raise exception
  void copy_start_error(const char*,const long) const;  // Raise exception
  void copy_end_error (const char*, const long) const;  // Raise exception
  void copy_error (const char*);                        // Raise exception
  void remove_error (const char*) const;                // Raise exception
  void va_arg_error (const char*, int);         // Raise exception

  CoolBaseVector ();                            // CoolBaseVector v; 
  CoolBaseVector (size_t);                              // CoolBaseVector v(10);
  CoolBaseVector (const CoolBaseVector&);               // CoolBaseVector v = y;
  ~CoolBaseVector ();                           // Destructor
};


// void reset () -- Set current position to INVALID.
// Input:           None
// Output:          None

inline void CoolBaseVector::reset () {
  this->curpos = INVALID_POS();                 // Invalidate current position
}


// Boolean next () -- Increment current position. If INVALID, set to first.
// Input:             None
// Output:            TRUE/FALSE

inline Boolean CoolBaseVector::next () {
  if (this->number_elements==0)                      // If empty vector
    this->curpos = INVALID_POS();             //    Set position to invalid
  else if (this->curpos == INVALID_POS())     // Else if already invalid
    this->curpos = 0;                         //    Set position to first
  else if (++this->curpos >= this->number_elements) // Else advance
    this->curpos = INVALID_POS();             //   Set invalid if off end
  return this->curpos!=INVALID_POS();         // Return true if on active item
}
    

// Boolean prev () -- Decrement current position. If INVALID, set to last.
// Input:             None
// Output:            TRUE/FALSE

inline Boolean CoolBaseVector::prev () {
  if (this->number_elements==0)                      // If empty vector
    this->curpos = INVALID_POS();             //    Set position to invalid
  else if (this->curpos == INVALID_POS())     // Else if already invalid
    this->curpos = this->number_elements-1;   //    Set position to last
  else if (this->curpos == 0)                 // Else if already at first
    this->curpos = INVALID_POS();             //   Set invalid
  else                                        
    --this->curpos;                           // Else decrement
  return this->curpos!=INVALID_POS();         // Return true if on active item
}


// size_t length () -- Return the number of elements in a CoolBaseVector object.
// Input:            None
// Output:           Integer representing number of elements

inline size_t CoolBaseVector::length () const {
  return (this->number_elements);
}


// size_t capacity () -- Return maximum number of elements CoolBaseVector obj can hold.
// Input:              None
// Output:             Integer value of maximum number of elements

inline size_t CoolBaseVector::capacity () const {
  return (this->size);                          // Maximum number of elements
}


// size_t position () -- Return current position.
// Input:              None
// Output:             Integer representing current position

inline size_t CoolBaseVector::position () const {
  return this->curpos;
}


// current_position () -- Return current position state
// Input:                 None
// Output:                Reference to current position state

inline CoolBaseVector::IterState& CoolBaseVector::current_position () {
  return this->curpos;
}

#endif                                          // End of BASE_VECTORH

