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
// Created: MBN 08/29/89 -- Initial design and implementation
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 09/23/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 09/23/92 -- renamed from CoolStack to CoolBase_Stack
// Updated: JAM 09/23/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
//
// The CoolStack class is  publicly  derived from the  Generic class and is used to
// implement non-type specific functionality for the parameterized CoolStack class.
// In  this manner, code common  to  all instances  of the  CoolStack  class can be
// shared to reduce code replication.  The  CoolStack<Type> class  implements a one
// dimensional vector of a user-specified type.  This  is accomplished by using
// the parameterized type  capability of C++.  The  stack will grow dynamically
// as necessary  with   the amount  of  growth determined  by  the value  of an
// allocation size slot.  Fixed length stacks are also supported by setting the
// value of the allocation size slot to zero.
//
// Each CoolStack object contains a protected data section that has  a slot to hold
// the current size of  the  stack,  a slot to  thold  the  number of  elements
// currently on the  stack,  a static specificying  the allocation growth size,
// and a float that may be set to some percentage to specify a growth ratio for
// this instance of a queue
//
// There are three  constructors for the   CoolStack class.  The  first constructor
// takes no arguments and creates an empty CoolStack object of the  specified type.
// The second constructor takes an argument specifying the initial size  of the
// stack.  Finally, the third constructor takes a single argument consisting of
// a reference to a CoolStack and duplicates its size and values.
//
// The CoolStack class  provides generic, type-independent  methods to  remport the
// number of items in  the stack, check the empty  status,  and clear all items
// from the stack. The assignment operator is  overloaded and the three methods
// to set the allocation growth size for the class as a whole, the growth ratio
// for a specific instance of  a stack, and the length  (ie. the  largest valid
// zero-relative index for random access) of the  stack are available. Finally,
// exception handling functions  called by the parameterized CoolStack<Type>  class
// are located   in the  base  class  to  facilitate code    charing  of common
// functionality.
//

#ifndef BASE_STACKH             // If no definition for CoolStack
#define BASE_STACKH             // Define stack symbol

#ifndef STREAMH                 // If the Stream support not yet defined,
#include <iostream.h>           // include the Stream class header file
#define STREAMH
#endif

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

#define STACK_MEM_BLK_SZ 100

class CoolBase_Stack {
protected:
  long size;                                    // Size of allocated storage
  long number_elements;                         // Number of elements in stack
  static int alloc_size_s;                          // Allocation size for growth
  float growth_ratio;                           // If non-zero, growth ratio

  void top_error (const char*);                 // Raise exception
  void pop_error (const char*);                 // Raise exception
  void bracket_error (const char*, long);       // Raise exception
  void push_error (const char*);                // Raise exception
  void popn_error (const char*, long);          // Raise exception
  void resize_error (const char*, long);        // Raise exception
  void assign_error (const char*);              // Raise exception

public:
  CoolBase_Stack ();                                    // Simple constructor
  CoolBase_Stack (long);                                        // CoolBase_Stack of initial size
  CoolBase_Stack (const CoolBase_Stack&);                               // Duplicate another stack
  ~CoolBase_Stack ();                                   // CoolBase_Stack destructor

  CoolBase_Stack& operator= (const CoolBase_Stack& s);          // Assignment s = s2;

  inline Boolean is_empty () const;             // Is stack empty?
  inline void clear ();                         // Clears all values from stack
  inline long length () const;                  // Return number of stack items
  inline long capacity() const;                 // Max. number of elements

  long set_length (long, const char*);          // Set number of elements
  void set_growth_ratio (float, const char*);   // Set growth percentage
  void set_alloc_size (int, const char*);       // Set alloc size
};


// long length() -- Return the number of elements in this stack
// Input:           None
// Output:          Integer representing number of elements

inline long CoolBase_Stack::length () const {
  return this->number_elements;
}

// capacity -- Return maximum number of elements object can hold
// Input:      None
// Output:     Integer value of maximum number of elements

inline long CoolBase_Stack::capacity () const {
  return (this->size);                          // Return max number of values
}


// Boolean is_empty() -- Return TRUE if this stack is empty
// Input:                None
// Output:               TRUE or FALSE

inline Boolean CoolBase_Stack::is_empty () const {
  return (this->number_elements == 0 ? TRUE : FALSE);
}


// void clear() -- Empty this stack  
// Input:          None
// Output:         None

inline void CoolBase_Stack::clear () {
  this->number_elements = 0;
}

#endif                                          // End BASE_STACKH

