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
// Updated: MBN 10/12/89 -- Added current_position() method for Iterator<Type>
// Updated: LGO 02/02/90 -- Re-wrote practically everything
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 09/23/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 09/23/92 -- renamed from CoolQueue to CoolBase_Queue
// Updated: JAM 09/23/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
// Updated: JAM 09/29/92 -- index/size/pos type changed from [u]long to [u]int
//
// The  CoolQueue class is  publicly derived from the Generic  class and is used to
// implement non-type specific functionality for the parameterized CoolQueue class.
// In this manner, code  common  to all instances  of  the  CoolQueue class  can be
// shared to  reduce code replication.  The CoolQueue  class implements  a circular
// buffer of  a user-specified   type.   This is  accomplished by   using   the
// parameterized  type capability of  C++. The CoolQueue will  grow  dynamically as
// necessary with the amount of growth determined by the value of an allocation
// size slot. Fixed  length queues are also  supported by setting  the value of
// the allocation size slot to INVALID.
//
// The (Base) CoolQueue object contains in, out and limit indices into data
// which is in the CoolQueue<Type> object.
// If in == out, the buffer is empty.
// If in > out, the area from out to in-1 contains available data.
// If out > in, the area from out to limit-1 contains the first part of
// the available data, and the area from first to in-1 contains the rest.
// Note that if the buffer was completely full, in would equal out. We
// we never let this happen, to preserve a simple empty check.
//
// There are  three  constructors  for the CoolQueue class.   The first constructor
// takes no arguments and creates an empty  CoolQueue object of the specified type.
// The second constructor takes an argument specifying the  initial size of the
// CoolQueue.  Finally, the third constructor takes a single argument consisting of
// a reference to a CoolQueue and duplicates its size and values.
//
// The CoolQueue class provides generic, type-independent methods to reset, move to
// the  next, and   move to the  previous  element   via  the current  position
// mechanism.  Methods to report  the number of  items in the queue,  check the
// empty status,  and  clear all items from the  queue are  also provided.  The
// assignment operator  is overloaded and three methods   to set the allocation
// growth size, growth raito, and length (ie.   largest valid index  for random
// access) of  the queue are  available. Finally,  exception handling functions
// called by the  parameterized CoolQueue class are  located in  the  base class to
// facilitate code sharing of common functionality.
//

#ifndef BASE_QUEUEH                             // If no class definition
#define BASE_QUEUEH                             // Indiciate we have done it

#include <iostream.h>           // include the Stream class header file

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

#define QUEUE_MEM_BLK_SZ 100

class CoolBase_Queue {
protected:
  int in;                                       // 1+ last element inserted
  int out;                                      // next element to remove
  int limit;                                    // Size of allocated storage
  int curpos;                                   // Keeps current position
  int alloc_size;                               // Allocation size for growth
  static float growth_ratio_s;                  // If non-zero, growth ratio 

  void value_error (const char*);               // Raise exception 
  void look_error (const char*);                // Raise exception
  void resize_error (const char*);              // Raise exception
  void assign_error (const char*);              // Raise exception
  
public:
  typedef int IterState;                        // Current position state

  CoolBase_Queue ();                                    // CoolBase_Queue q;
  CoolBase_Queue (int);                                 // CoolBase_Queue q(10);
  CoolBase_Queue (const CoolBase_Queue&);                               // CoolBase_Queue q = q2;
  ~CoolBase_Queue ();                                   // CoolBase_Queue destructor

  inline void reset ();                         // Invalidate current position
  inline Boolean next ();                       // Advance current position
  inline Boolean prev ();                       // Backup current position 
  
  inline Boolean is_empty() const;              // Is queue empty ?
  inline void clear();                          // Empty queue
  inline int capacity() const;                  // Max. number of elements
  inline IterState& current_position ();        // Set/get current position

  int length() const;                           // Number of items in queue
  void set_growth_ratio (float, const char*);   // Set growth percentage
  void set_alloc_size (int, const char*);       // Set alloc size
};


// capacity -- Return maximum number of elements object can hold
// Input:      None
// Output:     Integer value of maximum number of elements

inline int CoolBase_Queue::capacity () const {
  return (this->limit);
}


// Boolean is_empty() -- Return TRUE if this CoolBase_Queue is empty
// Input:                None
// Output:               TRUE or FALSE

inline Boolean CoolBase_Queue::is_empty () const {
  return in == out;
}


// void reset () -- Invalidate current position
// Input:           None
// Output:          None

inline void CoolBase_Queue::reset () {
  this->curpos = in;
}


// Boolean next () -- Return TRUE if able to advance current position by one;
//                    advance in this case means towards the front of the CoolBase_Queue
// Input:             None
// Output:            TRUE or FALSE

inline Boolean CoolBase_Queue::next () {    
  if (curpos == in)
    curpos = out;
  else if (++curpos >= limit)
    curpos = 0;         // increment and wrap
  return curpos != in;
}


// Boolean prev () -- Return TRUE if able to backup current position by one;
//                    backup means going towards the back of the CoolBase_Queue
// Input:             None
// Output:            TRUE or FALSE

inline Boolean CoolBase_Queue::prev() {
  if (--curpos < 0)
    curpos = limit - 1; // decrement and wrap
  return curpos != in;  
}


// void clear() -- Empty this CoolBase_Queue  
// Input:  None
// Output: None

inline void CoolBase_Queue::clear () {
  curpos = in = out = 0;
}


// current_position () -- Return current position state
// Input:                 None
// Output:                Reference to current position state

inline CoolBase_Queue::IterState& CoolBase_Queue::current_position () {
  return this->curpos;
}

#endif                                          // End of BASE_QUEUEH

