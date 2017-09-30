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
//
// Created: JCB 06/22/89 -- Design and implementation
// Updated: LGO 08/15/89 -- Removed operator>>
// Updated: MBN 08/20/89 -- Changed usage of template to reflect new syntax
// Updated: LGO 10/15/89 -- Added first & second
// Updated: LGO 10/15/89 -- Made get_first and get_second const 
// Updated: MBN 12/15/89 -- Added optional argument to set_compare method
// Updated: MJF 07/31/90 -- Added terse print
// Updated: DLS 03/27/91 -- New lite version
// Updated: JAM 08/14/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/14/92 -- modernized template syntax, remove macro hacks
//
// The parameterized Pair<KeyT, ValueT> class implements an association between
// one object  and another. The objects  may  be of  different  types, with the
// first representing the  "key" of the  pair  and the second  representing the
// "value" of the pair. The Pair<KeyT, ValueT> class is used by the Association
// class to  implement  an a-list,  that  is a  vector   of pairs of associated
// values.
//
// The Pair<KeyT,  ValueT>  class is relatively  simple,  having only two  data
// slots. The first slot retains the key of the pair,  and the second holds the
// value.  There are three constructors for the class: a constructor that takes
// no arguments and creates an empty pair object; a  constructor that takes two
// initial values,  one  each for the  key  and  the value of  the  pair; and a
// constructor that takes a refernece to another Pair<KeyT, ValueT>  object and
// copies the values.
//
// Methods are provided to get and set the value of  the key and value objects,
// assign one Pair<KeyT, ValueT> object to anothher,  and test for equality and
// inequality between  two pairs. Finally  the output operator is overloaded to
// provide a means to display the value of a pair object.
//

#ifndef PAIRH
#define PAIRH

#include <iostream.h>

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif


template <class T1, class T2>
class CoolPair {
public:
  CoolPair();                           // CoolPair p;
  CoolPair(const T1&, const T2&);       // CoolPair p = (foo,bar);
  CoolPair(const CoolPair<T1,T2>&);     // CoolPair p1 = p2;
  ~CoolPair();                          // Destructor
  
  inline const T1& get_first () const;          // Get first element of pair
  inline const T2& get_second () const;         // Get second element of pair
  inline void set_first (const T1&);            // Set first element of pair
  inline void set_second (const T2&);           // Set second element of pair
  inline T1& first ();                          // Get reference to 1st element
  inline T2& second ();                         // Get reference to 2nd element
  
  CoolPair<T1,T2>& operator= (const CoolPair<T1,T2>&); // Assignment p1 = p2;
  
  inline Boolean operator== (const CoolPair<T1,T2>&) const; // is equal
  inline Boolean operator!= (const CoolPair<T1,T2>& p) const; // is not equal
  
  void set_compare(Boolean (*) (const CoolPair<T1,T2>&, const CoolPair<T1,T2>&) = NULL); // Set compare function
  
  template< class U1, class U2 >
  friend ostream& operator<< (ostream&, const CoolPair<U1,U2>&); // Output operator
  template< class U1, class U2 >
  inline friend ostream& operator<< (ostream&, const CoolPair<U1,U2>*);
  
  void print(ostream&);                         // terse print

private:
  T1 firstd;                                    // First data slot
  T2 secondd;                                   // Second data slot
  static Boolean (*compare_s) (const CoolPair<T1,T2>&, const CoolPair<T1,T2>&); // Pointer operator== function
  template< class U1, class U2 >
  friend Boolean is_data_equal (const CoolPair<U1,U2>&, const CoolPair<U1,U2>&); 
};


// get_first -- Return the first element of the pair
// Input:       None
// Output:      const Reference to the first element of the pair

template<class T1, class T2> 
inline const T1& CoolPair<T1,T2>::get_first () const {
  return this->firstd;
}


// get_second -- Return the second element of the pair
// Input:        None.
// Output:       const Reference to the second element of the pair

template<class T1, class T2> 
inline const T2& CoolPair<T1,T2>::get_second () const {
  return this->secondd;
}


// first -- Return the first element of the pair
// Input:       None
// Output:      Reference to the first element of the pair

template<class T1, class T2> 
inline T1& CoolPair<T1,T2>::first () {
  return this->firstd;
}


// second -- Return the second element of the pair
// Input:        None.
// Output:       Reference to the second element of the pair

template<class T1, class T2> 
inline T2& CoolPair<T1,T2>::second () {
  return this->secondd;
}


// set_first -- Set the first element of the pair
// Input:       Reference to a first element value
// Output:      None.

template<class T1, class T2> 
inline void CoolPair<T1,T2>::set_first (const T1& first) {
  this->firstd = first;
}


// set_second -- Set the first element of the pair
// Input:        Reference to a second element value
// Output:       None.

template<class T1, class T2> 
inline void CoolPair<T1,T2>::set_second (const T2& second) {
  this->secondd = second;
}


// operator== -- Return TRUE if this pair and another specified are equal
// Input:        Reference to a pair
// Output:       TRUE or FALSE

template<class T1, class T2> 
inline Boolean CoolPair<T1,T2>::operator== (const CoolPair<T1,T2>& p) const {
  return (*this->compare_s)(*this, p);
}


// operator!= -- Return TRUE if this pair and another specified are not equal
// Input:        Reference to a pair
// Output:       TRUE or FALSE

template<class T1, class T2> 
inline Boolean CoolPair<T1,T2>::operator!= (const CoolPair<T1,T2>& p) const {
  return !(*this->compare_s)(*this, p);
}


// operator<< -- Overload output operator for the pair class
// Input:        Pointer to a pair object, reference to an output stream
// Output:       Reference to an output stream

template<class T1, class T2>
inline ostream& operator<< (ostream& os, const CoolPair<T1,T2>* p) {
  return operator<< (os, *p);
}

#include <cool/Pair.C>   // required for most template implementations

#endif                                          // End #ifndef PAIRH
