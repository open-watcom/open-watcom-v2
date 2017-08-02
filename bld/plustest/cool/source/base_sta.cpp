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
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: JAM 09/23/92 -- added def for alloc_size_s and initialized
//
// The CoolStack class is  publicly  derived from the  Generic class and is used to
// implement non-type specific functionality for the parameterized CoolStack class.
// In  this manner, code common  to  all instances  of the  CoolStack  class can be
// shared to reduce code replication.  The  CoolStack class  implements a one
// dimensional vector of a user-specified type.  This  is accomplished by using
// the parameterized type  capability of C++.  The  stack will grow dynamically
// as necessary  with   the amount  of  growth determined  by  the value  of an
// allocation size slot.  Fixed length stacks are also supported by setting the
// value of the allocation size slot to zero.
//

#ifndef BASE_STACKH                             // If no definition for CoolStack
#include <cool/Base_Stack.h>                    // Include definition file
#endif

// int alloc_size_s -- Allocation size for growth
int CoolBase_Stack::alloc_size_s = STACK_MEM_BLK_SZ;

// long set_length(long, char*) -- Change number of elements in this stack
// Input:                          Integer number of elements and
//                                 character string of type
// Output:                         Number of elements

long CoolBase_Stack::set_length (long n, const char* Type) {
#if ERROR_CHECKING
  if (n < 0) {                                  // If index out of range
    //RAISE (Error, SYM(CoolStack), SYM(Negative_Length),
    printf ("CoolStack<%s>::set_length(): Negative length %d.\n", Type, n);
    abort ();
  }
#endif
  if (n <= size)                                // If not greater than size
    this->number_elements = n;                  // Set new length
  else
    this->number_elements = size;               // Else set to maximum possible
  return this->number_elements;                 // Return value
}


// void set_growth_ratio(float, char*) -- Set growth percentage of this stack
// Input:                                 Float ratio and character string type
// Output:                                None

void CoolBase_Stack::set_growth_ratio (float ratio, const char* Type) {
#if ERROR_CHECKING
  if (ratio <= 0.0) {                           // If non-positive growth
    //RAISE (Error, SYM(CoolStack), SYM(Negative_Ratio),
    printf ("CoolStack<%s>::set_growth_ratio(): Negative growth ratio %f.\n",
            Type, ratio);
    abort ();
  }
#endif
  this->growth_ratio = ratio;                   // Adjust instance ration
} 


// void set_alloc_size(int, char*) -- Set the default allocation size 
// Input:                             Integer size and character string type
// Output:                            None

void CoolBase_Stack::set_alloc_size (int size, const char* Type) {
#if ERROR_CHECKING
  if (size < 0) {                                       // If index out of range
    //RAISE (Error, SYM(CoolStack), SYM(Negative_Size),
    printf ("CoolStack<%s>::set_alloc_size(): Negative growth size %d.\n",
            Type, size);
    abort ();
  }
#endif
  this->alloc_size_s = size;                    // Set growth size
}


// CoolBase_Stack () -- Empty constructor for the CoolBase_Stack class
// Input:            None
// Output:           None

CoolBase_Stack::CoolBase_Stack () {
  this->size = 0;                               // Initialize size
  this->number_elements = 0;                    // Initialize element count
  this->growth_ratio = 0.0;                     // Intialize growth ratio
}


// CoolBase_Stack (long) -- constructor that specifies number of elements
// Input:          Integer number of elements
// Output:         None

CoolBase_Stack::CoolBase_Stack (long n) {
  this->size = n;                               // Element capacity
  this->number_elements = 0;                    // No elements
  this->growth_ratio = 0.0;                     // Initialize growth ratio 
}


// CoolBase_Stack (CoolBase_Stack&) -- constructor for reference to another stack
// Input:                        CoolBase_Stack reference
// Output:                       None

CoolBase_Stack::CoolBase_Stack (const CoolBase_Stack& s) {
  this->growth_ratio = s.growth_ratio;          // New growth ratio
  this->size = s.size;                          // New size
  this->number_elements = s.number_elements;    // New number of elements
}


// ~CoolBase_Stack -- Destructor for CoolBase_Stack class that frees up storage
// Input:          None
// Output:         None

CoolBase_Stack::~CoolBase_Stack () {
}


// CoolBase_Stack& operator= (CoolBase_Stack&) -- Assigns this stack to another stack
// Input:                       Reference to a stack
// Output:                      Reference to modified this

CoolBase_Stack& CoolBase_Stack::operator= (const CoolBase_Stack& s) {
  this->number_elements = s.number_elements;    // New number of elements
  this->growth_ratio = s.growth_ratio;          // New growth ratio
  return *this;                                 // Return reference
}


// assign_error -- Error message for parameterized CoolBase_Stack<Type>::operator=()
// Input:          Character string of type
// Output:         None

void CoolBase_Stack::assign_error (const char* Type) {
  //RAISE (Error, SYM(CoolStack), SYM(Static_Size),
  printf ("CoolStack<%s>::operator=(): Static-size stack.\n", Type);
  abort ();
}


// top_error -- Error message for parameterized CoolStack<Type>::top()
// Input:       Character string of type
// Output:      None

void CoolBase_Stack::top_error (const char* Type) {
  //RAISE (Error, SYM(CoolStack), SYM(No_Elements),
  printf ("CoolStack<%s>::top(): No elements in stack.\n", Type);
  abort ();
}


// pop_error -- Error message for parameterized CoolStack<Type>::pop()
// Input:       Character string of type
// Output:      None

void CoolBase_Stack::pop_error (const char* Type) {
  //RAISE (Error, SYM(CoolStack), SYM(No_Elements),
  printf ("CoolStack<%s>::pop(): No elements in stack.\n", Type);
  abort ();
}


// bracket_error -- Error message for parameterized CoolStack<Type>::operator[]()
// Input:           Character string of type and index
// Output:          None

void CoolBase_Stack::bracket_error (const char* Type, long n) {
  //RAISE (Error, SYM(CoolStack), SYM(Out_Of_Range),
  printf ("CoolStack<%s>::operator[](): Index %d out of range.\n", Type, n);
  abort ();
}


// push_error -- Error message for parameterized CoolStack<Type>::push()
// Input:        Character string of type
// Output:       None

void CoolBase_Stack::push_error (const char* Type) {
  //RAISE (Error, SYM(CoolStack), SYM(Static_Size),
  printf ("CoolStack<%s>::push(): Static-size stack.\n", Type);
  abort ();
}


// popn_error -- Error message for parameterized CoolStack<Type>::popn()
// Input:        Character string of type and index
// Output:       None

void CoolBase_Stack::popn_error (const char* Type, long n) {
  //RAISE (Error, SYM(CoolStack), SYM(Static_Size),
  printf ("CoolStack<%s>::popn(): Negative stack index %d.\n", Type, n);
  abort ();
}


// resize_error -- Error message for parameterized CoolStack<Type>::resize()
// Input:          Character string of type and size
// Output:         None

void CoolBase_Stack::resize_error (const char* Type, long new_size) {
  //RAISE (Error, SYM(CoolStack), SYM(Negative_Size),
  printf ("CoolStack<%s>::resize(): Negative resize %d.\n", Type, new_size);
  abort ();
}
