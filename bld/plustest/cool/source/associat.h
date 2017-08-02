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
// Updated: JCB 06/26/89 -- Implementation.
// Updated: MBN 08/19/89 -- Changed template usage to reflect new syntax
// Updated: MBN 08/24/89 -- Added conditional exception handling and base class
// Updated: LGO 10/02/89 -- Removed destructor - Vector does the work
// Updated: MBN 10/07/89 -- Removed put(Pair<Ktype,Vtype>) method and added
//                          missing set_key_compare/set_value_compare methods
// Updated: MBN 10/11/89 -- Changed "current_position" to "curpos" 
// Updated: LGO 10/16/89 -- Re-wrote operator== to be const
// Updated: MBN 10/19/89 -- Added optional argument to set_compare methods
// Updated: MBN 11/01/89 -- Added constructor with user-provided storage param
// Updated: LGO 11/09/89 -- Inherit publicly from Vector to workaround bugs
//                          in Glockenspiel C++ 1.2 for OS/2
// Updated: MBN 01/31/89 -- Added current_position() member function
// Updated: MBN 02/22/90 -- Changed size arguments from long to unsigned long
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: VDN 04/20/92 -- Search most recent first, ordering not preserved.
// Updated: JAM 08/18/92 -- modernize template syntax, remove macro hacks
// Updated: JAM 08/18/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
// Updated: JAM 09/29/92 -- index/size/pos type changed from [u]long to size_t
//                           
//
// The Association<Ktype,Vtype> class is privatly derived from the Vector<Type>
// class and  is used  to implement a  collection of  pairs  that  is privately
// derived from the Vector class.  The first of the pair is called the  key and
// the  second  of the pair is called  the value.  The Association<Ktype,Vtype>
// class implements a  single dimensional vector  parameterized over two types.
// The first type specifies the type of the key, and  the second type specifies
// the  type  of  the  value.  The Association<Ktype,Vtype>  class inherits the
// dynamic  growth  capability of  the  Vector class.    The growth size can be
// determined by the value  of a  static  variable for the class as  a whole or
// from  a growth ratio for each  instance  of the  class.   In addition, fixed
// length vectors are also supported  by  setting the  value  of the allocation
// size variable to zero.
//
// Since  Association is  a specialization of   the Vector class, there are  no
// extra data slots needed in  the private section.   The protected section has
// two static data  slots for  the class  as a whole  that contain pointers  to
// comparison functions for  the key   and the  value types, repectively.   The
// default  compare function is operator== for  the appropriate type. There are
// four constructors for the Association class.  The first constructor takes no
// arguments  and creates an  empty Association object.  The second constructor
// takes a single argument specifying the initial size of the Association.  The
// third  constructor takes two  arguments,  the first a  pointer to a block of
// user-provided  storage and the  second providing indicating  the  number  of
// elements it  can  hold.  The  fourth constructor  is  similar to  the third,
// except that   it  takes a variable number   of arguments  to  allow  for the
// initialization   of  any  number of key/value  pairs.    Finally,  the fifth
// constructor  takes a single  argument  consisting  of    a  reference to   a
// Association and duplicates its size and element values.
//
// Due to  the  private  inheritance  of  the  Vector  class,  the only methods
// available for the Association class  are those that are  explicitly defined.
// Methods are available to set the compare functions  for the key and value, a
// resize method, and get, get key,  and remove methods  used  to add, inspect,
// and remove pairs.  In addition, the Association<Ktype,Vtype> class maintains
// the notion of a current position and  provides reset, next,  prev, find, and
// value methods for working with the  pair at  the current position.  Finally,
// the assignment, equality and inequality, and output operators are overloaded
// for the Association class.
//
// Since Association is an unordered set, it should behave more like a Shuffle,
// with ordering not preserved when elements are removed or inserted. Search
// is done from the end to the begining, and so is more like most recent first.

#ifndef ASSOCIATIONH                            // If we have not defined class
#define ASSOCIATIONH                            // indicate that it is done now

#ifndef VECTORH                                 // If have not defined Vector
#include <cool/Vector.h>                                // Include header file
#endif

#ifndef PAIRH                                   // If we have not defined Pair
#include <cool/Pair.h>                          // Include header file
#endif

//## BC++ bug requires you to "typedef CoolPair<Ktype,Vtype> KVHack"
template <class Ktype, class Vtype>
class CoolAssociation : private CoolVector< CoolPair<Ktype,Vtype> > {
public:
  typedef size_t IterState;             // Current position state
  typedef Boolean (*Key_Compare)(const Ktype&, const Ktype&);
  typedef Boolean (*Value_Compare)(const Vtype&, const Vtype&);

  CoolAssociation();            // Simple constructor
  CoolAssociation(size_t);      // constructor w/ element count
  CoolAssociation(void*, size_t); // Assoc with static storage
  CoolAssociation(const CoolAssociation<Ktype,Vtype>&); // Copy constr
  ~CoolAssociation();                   // Destructor

  inline void resize(size_t);                   // Resize for at least count
  inline size_t length();                               // Return number of elements
  inline size_t capacity();                     // Max. number of elements
  inline size_t set_length(size_t);                     // Set number of elements
  inline void set_growth_ratio(float);          // Set growth percentage
  inline void set_alloc_size(size_t);           // Set alloc size

  void set_key_compare (Key_Compare = NULL);
  void set_value_compare (Value_Compare = NULL);

  inline void reset();                          // Invalidate current position
  inline void clear();                          // Clear objects from assoc
  inline Boolean next();                        // Advance current position
  inline Boolean prev();                        // Backup current position
  inline Vtype& value();                        // Value at current position
  inline const Ktype& key() const;              // Key at current position
  inline IterState& current_position (); // Set/Get current position

  Boolean find(const Ktype&);                   // Find/set current position
  Boolean get(const Ktype&, Vtype&);            // Get associated value for key
  Boolean get_key(const Vtype&, Ktype&) const;  // Get associated key for value

  Boolean put(const Ktype&, const Vtype&);      // Add/update key/value pair
  Boolean remove(const Ktype&);                 // Removes pair with key
  Vtype remove();                               // Removes pair at cur pos

  inline CoolAssociation<Ktype,Vtype>& operator=(const CoolAssociation<Ktype,Vtype>&);

  Boolean operator==(const CoolAssociation<Ktype,Vtype>&) const;
  inline Boolean operator!=(const CoolAssociation<Ktype,Vtype>&) const;

  template< class K, class V >
  friend ostream& operator<< (ostream&,const CoolAssociation<K,V>&);
  template< class K, class V >
  inline friend ostream& operator<< (ostream&,const CoolAssociation<K,V>*);

protected:
  static Key_Compare compare_keys_s;
  static Value_Compare compare_values_s;

  template< class K >
  friend Boolean CoolAssociation_keys_eql(const K&, const K&);
  template< class V >
  friend Boolean CoolAssociation_values_eql(const V&,const V&);
};

// void reset () -- Set current position to INVALID.
// Input:           None
// Output:          None

template <class Ktype, class Vtype>
inline void CoolAssociation<Ktype,Vtype>::reset() {
  CoolVector< CoolPair<Ktype,Vtype> >::reset();
}


// void clear () -- Clear all objects from association
// Input:           None
// Output:          None

template <class Ktype, class Vtype>
inline void CoolAssociation<Ktype,Vtype>::clear() {
  CoolVector< CoolPair<Ktype,Vtype> >::clear();
}


// Boolean next () -- Increment current position. If INVALID, set to first.
// Input:             None
// Output:            TRUE/FALSE

template <class Ktype, class Vtype>
inline Boolean CoolAssociation<Ktype,Vtype>::next() {
  return CoolVector< CoolPair<Ktype,Vtype> >::next();
}


// Boolean prev () -- Decrement current position. If INVALID, set to last.
// Input:             None
// Output:            TRUE/FALSE

template <class Ktype, class Vtype>
inline Boolean CoolAssociation<Ktype,Vtype>::prev() {
  return CoolVector< CoolPair<Ktype,Vtype> >::prev();
}


// void resize () -- Adjust the memory size of object to accomodate some size
// Input:            Interger number of elements to hold
// Output:           None

template <class Ktype, class Vtype>
inline void CoolAssociation<Ktype,Vtype>::resize (size_t n) {
  CoolVector< CoolPair<Ktype,Vtype> >::resize(n);
}


// size_t length () -- Return the number of elements in object.
// Input:            None
// Output:           Integer representing number of elements

template <class Ktype, class Vtype>
inline size_t CoolAssociation<Ktype,Vtype>::length() {
  return CoolVector< CoolPair<Ktype,Vtype> >::length();
}


// size_t capacity () -- Return maximum number of elements object can hold.
// Input:              None
// Output:             Integer value of maximum number of elements

template <class Ktype, class Vtype>
inline size_t CoolAssociation<Ktype,Vtype>::capacity () {
  return this->CoolVector< CoolPair<Ktype,Vtype> >::capacity();
}


// size_t set_length () -- Set the number of elements in a object.
// Input:                Length value
// Output:               Integer representing number of elements

template <class Ktype, class Vtype>
inline size_t CoolAssociation<Ktype,Vtype>::set_length(size_t n) {
  return CoolVector< CoolPair<Ktype,Vtype> >::set_length(n);
}


// void set_growth_ratio () -- Set the growth percentage for the object
// Input:                      Float ratio
// Output:                     None

template <class Ktype, class Vtype>
inline void CoolAssociation<Ktype,Vtype>::set_growth_ratio(float n) {
  CoolVector< CoolPair<Ktype,Vtype> >::set_growth_ratio(n);
}


// void set_alloc_size () -- Set the default allocation size growth rate.
// Input:                    Growth size in number of elements
// Output:                   None

template <class Ktype, class Vtype>
inline void CoolAssociation<Ktype,Vtype>::set_alloc_size(size_t n){
  CoolVector< CoolPair<Ktype,Vtype> >::set_alloc_size(n);
}


// Ktype& key () -- Get key (first) item in pair object at current position
// Input:           None
// Output:          Reference to Key value of pair

template <class Ktype, class Vtype>
inline const Ktype& CoolAssociation<Ktype,Vtype>::key() const {
  return (this->data[this->curpos].get_first());
}


// Vtype& value () -- Return value at current position.
// Input:             None
// Output:            Type reference to value at current position

template <class Ktype, class Vtype>
inline Vtype& CoolAssociation<Ktype,Vtype>::value() {
  return (this->data[this->curpos].second());
}


// Boolean operator!= -- Test for inequality of the data of two objects
// Input:                Reference to objects
// Output:               TRUE/FALSE

template <class Ktype, class Vtype>
inline Boolean CoolAssociation<Ktype,Vtype>::operator!=(const CoolAssociation<Ktype, Vtype>& a) const {
  return (!this->operator==(a));
}



// operator= -- Overload the assignment operator to copy the elements
//              in one CoolAssociation to another. If there are more elements in
//              the source than the destination and the destination is
//              not of static size, then storage will be allocated and
//              the destination CoolAssociation will grow.
// Input:       Reference to CoolAssociation
// Output:      Reference to copied CoolAssociation object

template <class Ktype, class Vtype>
inline CoolAssociation<Ktype,Vtype>& CoolAssociation<Ktype,Vtype>::operator=(const CoolAssociation<Ktype,Vtype>& a) {
  CoolVector< CoolPair<Ktype,Vtype> >::operator=(a);
  return *this;
}


// operator<< -- Overload the output operator for pointer to CoolAssociation
// Input:        ostream reference, Assocition pointer
// Output:       CoolAssociation data is output to ostream

template <class Ktype, class Vtype>
inline ostream& operator<< (ostream& os, const CoolAssociation<Ktype,Vtype>* v) {
  return operator<< (os, *v);
}

  
// Set/Get current position
template <class Ktype, class Vtype>
inline /*CoolAssociation<Ktype,Vtype>::IterState##*/size_t& CoolAssociation<Ktype,Vtype>::current_position () {
  return CoolVector< CoolPair<Ktype,Vtype> >::current_position();
}

#include <cool/Association.C>   // required for most template implementations

#endif                          // End #ifdef of ASSOCIATIONH
