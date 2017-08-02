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
// Created: MJF 05/22/89 -- Initial design.
// Updated: JCB 06/05/89 -- Implementation.
// Updated: LGO 08/09/89 -- Inherit from Generic
// Updated: MBN 08/20/89 -- Changed usage of template to reflect new syntax
// Updated: MBN 08/24/89 -- Added conditional exception handling and base class
// Updated: MBN 10/07/89 -- Fixed element count bug in get() method
// Updated: MBN 10/11/89 -- Changed "current_position" to "curpos" 
// Updated: MBN 10/19/89 -- Added optional argument to set_compare method
// Updated: MBN 11/01/89 -- Added constructor with user-provided storage param
// Updated: MBN 11/15/89 -- Fixed put() for case when there is no storage
// Updated: MBN 11/17/89 -- Fixed unget() for case when there is no storage
// Updated: LGO 02/02/90 -- Re-wrote practically everything
// Updated: MBN 02/22/90 -- Changed size arguments from long to unsigned long
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 05/31/90 -- Use "delete [size] data"
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/19/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//                          non-template classes Cool_Stack=>CoolBase_Stack
// Updated: JAM 09/29/92 -- index/size/pos type changed from [u]long to [u]int
//
// The Queue<Type> class is  publicly  derived from   the base Queue class  and
// implements a circular buffer of a user-specified type.  This is accomplished
// by  using the  parameterized type capability   of C++. The Queue  will  grow
// dynamically as necessary with the amount of growth  determined by  the value
// of an allocation   size slot. Fixed length queues    are also supported   by
// setting the value of the allocation size slot to INVALID.
//
// The (Base) Queue object contains in, out and limit indices into data
// which is in the Queue<Type> object.
// If in == out, the buffer is empty.
// If in > out, the area from out to in-1 contains available data.
// If out > in, the area from out to limit-1 contains the first part of
// the available data, and the area from first to in-1 contains the rest.
// Note that if the buffer was completely full, in would equal out. We
// we never let this happen, to preserve a simple empty check.
//
// There  are four   constructors   for the   Queue<Type> class.   The    first
// constructor takes  no arguments and creates   an  empty  Queue object of the
// specified type.  The second constructor takes a required argument specifying
// the initial  size  of the queue.  The  third takes a pointer   to a block of
// user-defined storage    and  the number of  elements  the   queue  can hold.
// Finally,  the   third constructor  takes a  single argument  consisting of a
// reference to a Queue<Type> and duplicates its size and element values.
//
// Methods are  provided to get  and unget an item  from the queue, and put and
// unput and item from the queue. This provides addition and removal operations
// for both ends of the queue. A look  method allows an  application to examine
// the item at the head of the queue without removing it. Methods to report the
// number of items in the queue, check the empty  status,  and clear  all items
// from the  queue  are also provided.  The   assignment, output, and  equality
// operators are overloaded and two methods to  set  the allocation growth size
// and compare function are available. Finally, find and  remove  methods allow
// an application  to search for  a particular item and/or  remove   it  from a
// queue.
//

#ifndef QUEUEH                                  // If no Queue class definition
#define QUEUEH                                  // Indiciate we have done it

#ifndef BASE_QUEUEH                             // If no definition for class
#include <cool/Base_Queue.h>                            // include definition file
#endif

template <class Type>
class CoolQueue : public CoolBase_Queue {
public:
  typedef Boolean (*Compare) (const Type&, const Type&);
  CoolQueue();                          // CoolQueue q;
  CoolQueue(unsigned int);              // CoolQueue q(10);
  CoolQueue(void*, unsigned int);       // CoolQueue with static storage
  CoolQueue(const CoolQueue<Type>&);    // CoolQueue q = q2;
  ~CoolQueue();                         // CoolQueue destructor

  Type& get ();                                 // Get and remove first-in item
  Boolean get (Type& result);                   // Get and remove first-in item
  Boolean unget (const Type&);                  // Put back first-in item 
  Boolean put (const Type&);                    // Put new last-in item
  Type& unput();                                // Remove last-in item 
  Boolean unput (Type& result);                 // Remove last-in item 
  inline Type& look();                          // Just return next item to get

  inline Type& value ();                        // Get current position value
  Boolean find (const Type&);                   // Find/set current position 
  Boolean remove ();                            // Remove current position item
  Boolean remove (const Type&);                 // Find/remove item 
  
  CoolQueue<Type>& operator= (const CoolQueue<Type>&);  // Assignment q = q2;
  Boolean operator== (const CoolQueue<Type>&) const; // is equal
  inline Boolean operator!= (const CoolQueue<Type>&) const; // is not equal

  void resize (int);                            // Resize for at least count
  inline void set_growth_ratio (float);         // Set growth percentage
  inline void set_alloc_size (int);             // Set alloc size
  void set_compare(Compare = NULL); // Set compare function

  template< class U >
  friend ostream& operator<< (ostream&, const CoolQueue<U>&);
  template < class U >
  inline friend ostream& operator<< (ostream&, const CoolQueue<U>*);

protected:
  Type* data;                                   // Pointer to allocated storage
  static Compare compare_s;     // Pointer operator== function
  Boolean grow ();                              // Make the queue bigger
  ostream& qprint(ostream& os) const;           // Print a queue

private:
  template< class U >
  friend Boolean CoolQueue_is_data_equal (const U&, const U&);
};


// Type& look () -- Return first-in item in this CoolQueue
// Input:           None
// Output:          Reference to the first-in item 

template<class Type> 
inline Type& CoolQueue<Type>::look () {
#if ERROR_CHECKING
  if (in == out)                                // If there are no elements
    this->look_error (#Type);                   // Raise exception
#endif
  return (this->data[out]);
}


// Type& value () -- Return item at current position
// Input:            None
// Output:           Reference to item at current position 

template<class Type> 
inline Type& CoolQueue<Type>::value () {
#if ERROR_CHECKING
  if (this->curpos == in)                       // If invalid current position
    this->value_error (#Type);                  // Raise exception
#endif
  return (this->data[curpos]);
}


// Boolean operator!= (CoolQueue<Type>&) -- Compare this CoolQueue with another CoolQueue
// Input:  Reference to a CoolQueue
// Output: TRUE or FALSE

template<class Type> 
inline Boolean CoolQueue<Type>::operator!= (const CoolQueue<Type>& q) const {
  return (!operator==(q));
}


// void set_growth_ratio(float) -- Set growth percentage of this CoolQueue
// Input:                          Float ratio
// Output:                         None

template<class Type> 
inline void CoolQueue<Type>::set_growth_ratio (float ratio) {
  this->CoolBase_Queue::set_growth_ratio (ratio, "Type");       // Pass size/type to base class
}


// void set_alloc_size(int) -- Set the default allocation size growth size
// Input:  Integer size
// Output: None

template<class Type> 
inline void CoolQueue<Type>::set_alloc_size (int size) {
  this->CoolBase_Queue::set_alloc_size (size, "Type");  // Pass size/type to base
}

// operator<< -- Overload the output operator for CoolQueue
// Input:        ostream reference, queue pointer
// Output:       CoolQueue data is output to ostreamn

template<class Type>
inline ostream& operator<< (ostream& os, const CoolQueue<Type>* q) {
  return q->qprint(os);
}


#endif                          // End #ifdef of QUEUEH

