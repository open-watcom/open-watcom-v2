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
// Created: MBN 10/10/89 -- Initial design and implementation
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/18/92 -- modernized template syntax, remove macro hacks
//                          Type##_state=>Type::IterState
//
// The Iterator<Container> class provides  an independent mechanism  for
// maintaining the state associated with the current position of an
// instance of a container class.  In this manner, multiple iterators
// over the same instance of a class can be supported.  Each container
// class  that  supports the current position notion has some data
// structure that is used to represent the state.  This may be as simple
// as a type long,  or something more  involved as with a union of of
// bit fields  or even another class instance.  In addition, each 
// container class  has a method  to get/set the  current   position.
// Utilization of this method with the  Iterator storage will facilitate
// storage  and retrieval  of the current position.
// 
// The container-specific  data  structure used  to   hold the current
// position state is  all COOL  container  classes  will -- by  
// convention --  be a nested typedef IterState defined in the container
// class.  Thus, a user who includes Vector.h will declare an
// Iterator<Vector<T>>, and the internal   data   structure that
// maintains    the  state  will   be of  type Vector<T>::IterState (eg,
// typedef long IterState).  In this manner, the Iterator<Container>
// class can be parameterized over the container  class name (ie. 
// Bit_Set, Vector,  etc.)  and allocate a data slot of the  appropriate
// type without the user having  to know anything about internal
// implementation details.
// 
// Each instance of a container  class has  a single slot  in the private
// data section to maintain the current position. In  addition, each
// container class has a  public method that  returns a reference to
// this  iterator state  data structure.  The  six methods that support 
// current position functionality in all of  the   container classes 
// always  work  on  the current  position  as maintained in the private
// data section. A user can, at any point, change the current position
// state information  by utilizing this  method to get  and/or set the
// current position. Finally,  each state data structure implemented in
// every container class must support the assignment  of INVALID in some
// manner such that a state with  this value  (defined in <COOL/misc.h>)
// will cause an exception if used before set.
// 
// There is only one private slot in the  Iterator<Container>  class that
// contains a single  element of type  Container::IterState for the
// appropriate container class.  The single constructor invokes the
// reset() method of the  container class to initialize the state.  The
// single method  provides   a means to  efficiently access the state of
// the object. All functionality that manipulates the state information
// remains in the appropriate container  class. In this manner, the
// Iterator<Container<T>> class remains generic.

#ifndef ITERATORH                               // If we have not defined class
#define ITERATORH                               // Indicate class Iterator

template <class Container>
class CoolIterator {
public:
  /*inline##*/ CoolIterator();                  // constructor inits state
//##  inline CoolIterator(const Container::IterState&); // construct from state
  inline ~CoolIterator();     // Destructor

//##  inline CoolIterator<Container>& operator= (const Container::IterState&); // Assignment

//##  inline operator Container::IterState();           // Convert type-specific state

  CoolIterator(const Container::IterState& s) //## define here for BC++ 3.1 bug
     { this->value = s; }  //##
  CoolIterator<Container>& operator= (const Container::IterState& s) //## define here for BC++ 3.1 bug
     { this->value = s; return *this; } //##
  operator Container::IterState()  //## define here for BC++ 3.1 bug
     { return this->value; } //##
private:
  Container::IterState value;                           // Slot to hold iterator state
};


// CoolIterator<Container> -- Simple constructor for CoolIterator class
// Input:            None
// Output:           None

template<class Container>
inline CoolIterator<Container>::CoolIterator() {}


// CoolIterator<Container> --  Copy constructor
// Input:            None
// Output:           None

//## Borland doesn't allow this definition outside of class (typedef bug)
//##template<class Container>
//##inline CoolIterator<Container>::CoolIterator(const Container::IterState& s) {
//##  this->value = s;                          // Assign state
//##}

// ~Iterator  -- Delete iterator, nothing.

template<class Container>
inline CoolIterator<Container>::~CoolIterator() {}

// operator=  -- Assignment to store state into iterator
// Input:        reference to state
// Output:       none

//## Borland doesn't allow this definition outside of class (typedef bug)
//##template<class Container>
//##inline CoolIterator<Container>& CoolIterator<Container>::operator= (const Container::IterState& s) {
//##  this->value = s;
//##  return *this;
//##}

// operator Container::IterState() -- Implicit conversion of Iterator<Container> to state
//                             that type defined in each COOL container class.
// Input:                      None
// Output:                     None

//## Borland doesn't allow this definition outside of class (typedef bug)
//##template<class Container>
//##inline CoolIterator<Container>::operator Container::IterState () {
//##  return this->value;                               // Return state
//##}

#endif                                          // End of ITERATORH

