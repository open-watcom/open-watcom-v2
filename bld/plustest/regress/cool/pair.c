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

#ifndef PAIRC
#define PAIRC

#include <cool/Pair.h>

// compare_s -- Pointer operator== function
template<class T1, class T2> 
Boolean (*CoolPair<T1,T2>::compare_s) (const CoolPair<T1,T2>&, const CoolPair<T1,T2>&) = &is_data_equal;

// CoolPair -- Empty constructor
// Input:  None.
// Output: None.

template<class T1, class T2> 
CoolPair<T1,T2>::CoolPair() {
}


// CoolPair (T1,T2) -- Constructor that initializes the first and second elements
// Input:          Reference to first value, reference to second value
// Output:         None.  

template<class T1, class T2> 
CoolPair<T1,T2>::CoolPair(const T1& first, const T2& second) {
  this->firstd = first;
  this->secondd = second;
}


// CoolPair (CoolPair) -- Constructor for reference to another pair
// Input:         Reference to another pair
// Output:        None.

template<class T1, class T2> 
CoolPair<T1,T2>::CoolPair(const CoolPair<T1,T2>& p) {
  this->firstd = p.firstd;
  this->secondd = p.secondd;
}


// ~CoolPair -- Destructor for pair does nothing
// Input:         None.
// Output:        None.

template<class T1, class T2> 
CoolPair<T1,T2>::~CoolPair() {}



// operator= -- Assigns this pair to another
// Input:  Reference to another pair
// Output: Reference to the copied pair

template<class T1, class T2> 
CoolPair<T1,T2>& CoolPair<T1,T2>::operator= (const CoolPair<T1,T2>& p) {
  this->firstd = p.firstd;
  this->secondd = p.secondd;
  return *this;
}


// operator<< -- Output a pair
// Input:        Reference to an ostream, reference to a pair
// Output:       Reference to the modified ostream

template<class T1, class T2>
ostream& operator<< (ostream& os, const CoolPair<T1,T2>& p) {
  os << "[" << p.firstd << " " << p.secondd << "] ";
  return os;
}


// is_data_equal -- Default data comparison function if user has not provided
//                  another one. 
// Input:           Two references to pairs
// Output:          TRUE or FALSE

template<class T1, class T2>
Boolean is_data_equal (const CoolPair<T1,T2>& p1, const CoolPair<T1,T2>& p2) {
  return ((p1.firstd == p2.firstd) && (p1.secondd == p2.secondd));
}

// set_compare -- Specify the comparison function to be used in logical tests
//                of the elements of pairs
// Input:         Pointer to compare function
// Output:        None

template<class T1, class T2> 
void CoolPair<T1,T2>::set_compare ( register Boolean (*cf) (const CoolPair<T1,T2>&, const CoolPair<T1,T2>&)) {
  if (cf == NULL)                       // if no compare function
    this->compare_s = &is_data_equal; // default
  else
    this->compare_s = cf;
}


// print --  terse print function for CoolPair
// Input:    reference to output stream
// Output:   none

template<class T1, class T2>
void CoolPair<T1,T2>::print (ostream& os) {
  os << "/* CoolPair " << (long) this << " */";
}

#endif // PAIRC


