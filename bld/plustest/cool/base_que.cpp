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
// Updated: MBN 10/11/89 -- Changed "current_position" to "curpos" 
// Updated: LGO 02/02/90 -- Re-wrote practically everything
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 09/23/92 -- added def for growth_ratio_s
//
// The  CoolQueue class is publicly derived from the Generic class  and is used
// to implement  non-type specific  functionality  for the  parameterized CoolQueue
// class. In this  manner, code common to all  instances of the CoolQueue class can
// be shared to reduce code replication. The CoolQueue class implements  a circular
// buffer of  a   user-specified type.   This is accomplished   by   using  the
// parameterized type capability  of C++. The CoolQueue  will grow  dynamically  as
// necessary with the amount of growth determined by the value of an allocation
// size slot. Fixed length  queues  are also supported by  setting the value of
// the allocation size slot to INVALID.
//

#ifndef BASE_QUEUEH                             // If no class definition
#include <cool/Base_Queue.h>                    // Include definition file
#endif

// growth_ratio_s -- If non-zero, growth ratio 
float CoolBase_Queue::growth_ratio_s;


// int length() -- Return the number of elements in this CoolBase_Queue
// Input:           None
// Output:          Integer representing number of elements

int CoolBase_Queue::length () const {
  int len = in - out;
  return (len < 0) ? limit + len : len;
}

// void set_growth_ratio(float) -- Set growth percentage of this CoolBase_Queue
// Input:                          Float ratio and character string type
// Output:                         None

void CoolBase_Queue::set_growth_ratio (float ratio, const char* Type) {
#if ERROR_CHECKING
  if (ratio < 0.0) {                            // If non-positive growth
    //RAISE (Error, SYM(CoolQueue), SYM(Negative_Ratio),
    printf ("CoolQueue<%s>::set_growth_ratio(): Negative growth ratio %f.\n",
            Type, ratio);
    abort ();
  }
#endif
  this->growth_ratio_s = ratio;                 // Set growth ratio
}


// void set_alloc_size(int) -- Set the default allocation size growth rate
// Input:                      Integer size and character string type
// Output:                     None

void CoolBase_Queue::set_alloc_size (int size, const char* Type) {
#if ERROR_CHECKING
  if (size < 0 && size != INVALID) {            // If index out of range
    //RAISE (Error, SYM(CoolQueue), SYM(Negative_Size),
    printf ("CoolQueue<%s>::set_alloc_size(): Negative growth size %d.\n",
            Type, size);
    abort ();
  }
#endif
  this->alloc_size = size;                      // Set growth size
}


// CoolBase_Queue () -- Empty constructor for the CoolBase_Queue class
// Input:      None
// Output:     None

CoolBase_Queue::CoolBase_Queue () {
  this->limit = 0;                              // Initialize size
  this->in = this->out = 0;                     // Intialize first/last pointer
  this->alloc_size = QUEUE_MEM_BLK_SZ;          // Default
}


// CoolBase_Queue (int) -- Constructor that specifies number of elements
// Input:          Integer number of elements
// Output:         None

CoolBase_Queue::CoolBase_Queue (int n) {
  this->limit = n;                              // Element capacity
  this->in = this->out = 0;                     // Intialize first/last pointer
  this->alloc_size = QUEUE_MEM_BLK_SZ;          // Default
}


// CoolBase_Queue (CoolBase_Queue&) -- Constructor for reference to another CoolBase_Queue
// Input:            CoolBase_Queue reference
// Output:           None

CoolBase_Queue::CoolBase_Queue (const CoolBase_Queue& s) {
  alloc_size = s.alloc_size;                    // Set alloc size
  if (alloc_size == INVALID)
    alloc_size = QUEUE_MEM_BLK_SZ;
  this->in = s.in;                              // Set in
  this->out = s.out;                            // Set out
  this->limit = s.limit;                        // Set limit
}


// ~CoolBase_Queue -- Destructor for CoolBase_Queue class that frees up storage
// Input:    None
// Output:   None

CoolBase_Queue::~CoolBase_Queue () {
}


// look_error -- Error message for parameterized CoolQueue<Type>::look() method
// Input:        Character string of type
// Output:       None

void CoolBase_Queue::look_error (const char* Type) {
  //RAISE (Error, SYM(CoolQueue), SYM(No_Elements),
  printf ("CoolQueue<%s>::look(): No elements in queue.\n", Type);
  abort ();
}


// value_error -- Error message for parameterized CoolQueue<Type>::value() method
// Input:         Character string of type
// Output:        None

void CoolBase_Queue::value_error (const char* Type) {
  //RAISE (Error, SYM(CoolQueue), SYM(Invalid_Cpos),
  printf ("CoolQueue<%s>::value(): Invalid current position.\n", Type);
  abort ();
}


// resize_error -- Error message for parameterized CoolQueue<Type>::resize() method
// Input:          Character string of type
// Output:         None

void CoolBase_Queue::resize_error (const char* Type) {
  //RAISE (Error, SYM(CoolQueue), SYM(Static_Size),
  printf ("CoolQueue<%s>::resize(): Static-size queue.\n", Type);
  abort ();
}


// assign_error -- Error message for parameterized CoolQueue<Type>::operator=()
// Input:          Character string of type
// Output:         None

void CoolBase_Queue::assign_error (const char* Type) {
  //RAISE (Error, SYM(CoolQueue), SYM(Static_Size),
  printf ("CoolQueue<%s>::operator=(): Static-size queue.\n", Type);
  abort ();
}
