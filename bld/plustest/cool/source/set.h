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
// Created: MBN 06/05/89 -- Initial design and implementation
// Updated: MBN 08/20/89 -- Changed usage of template to reflect new syntax
// Updated: MBN 09/15/89 -- Added conditional exception handling
// Updated: MBN 10/03/89 -- Fixed put() method to update value
// Updated: MBN 10/07/89 -- Fixed double entry count with use of iterators
// Updated: MBN 10/12/89 -- Changed "current_position" to "curpos" and changed
//                          state from bit set to bit set/get macros
// Updated: LGO 10/19/89 -- Fixed operator==() for tables with different bucket
//                          count but same elements -- tables grew separately
// Updated: MBN 12/22/89 -- Sprinkled "const" qualifiers all over the place!
// Updated: MBN 02/22/90 -- Changed size arguments from long to unsigned long
// Updated: MBN 02/23/90 -- Made operators -=, ^=, |=, &= return reference 
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 09/24/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 09/24/92 -- modernize template syntax, remove macro hacks
// Updated: JAM 09/24/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
//
// The Set<Type> class implements a set  of elements  of a user-specified type.
// This is accompilshed by using the parameterized type capability of C++.  The
// Set<Type> class implements a simple one-element hash table where the key and
// the value are the same thing.  The type of the Set class is the key/value in
// the Hash Table.  The  Set<Type> class is  derived from the Hash_Table  class
// and is  dynamic  in nature.  It's size   (ie. the number of  buckets  in the
// table) is  always some prime number. Each  bucket holds 8 items.   No wholes
// are left in a bucket; if a key/value is removed from the middle of a bucket,
// following entries are moved up.   When a hash on a  key ends  up in a bucket
// that is full, the table is enlarged.
//
// The private data  section of  a Set<Type> has  a  slot that   points  to the
// physical memory allocated for some prime number of buckets each of which has
// memory allocated for 8 items.  The number of buckets  currently in the table
// is accessed by an index into a static table of  selected prime numbers. This
// static table contained within  the class eliminates  the somewhat  expensive
// runtime computation of prime numbers.  The  table consists of  prime numbers
// where the difference  between  any two successive entries gets progressively
// larger as you move through the table.  The specified range of primes results
// in an arbitrary limitation of 2^22 entries in a single hash table.
//
// When a hash on a key ends up in a bucket that is full, the table is enlarged
// to the next prime number of buckets or to the prime number  that is at least
// large enough to accommodate  a user-specified growth  ratio. The  entries in
// the  buckets are then  rehashed   into the   new  table.  Selection  of   an
// appropriate hash function  is  crucial to  uniform  distribution through the
// table. The default   hash  utilizes  the number   of  buckets  in order   to
// accomplish this. A user-supplied function should do something similar.
//
// Other items in the private data section include a pointer to a hash function
// and a  pointer to  a compare  function.   The compare   function  is used in
// equality operations on key/value items.  The default compare function is the
// built-in == operator.  The default  hash function is either a  simple 32 bit
// value if sizeof(Type) is 4, that is shifted left  three bits with the result
// modulo the number of buckets determining the  hash. This  is ideal when Type
// is a pointer..   If sizeof(Type) is greater than  4,  then the 32 bit  value
// used  is  the  result of  exclusive-oring  successive 32-bit values  for the
// length of T1, and then applying the same  bit shift  and modulo operation as
// before.
//
// Three different constructors are provided.  The  first  constructor takes no
// arguments and creates  a  Set that can  contain 24 items before  resizing is
// necessary.  The second constructor accepts an integer argument and creates a
// Set that  can accomodate at  least the specified number  of items.  Finally,
// the third constructor takes a single argument  consisting of a  reference to
// another Set and duplicates its size and contents.
//
// Methods are provided to put, query,  and remove  an item from a set, perform
// the  intersection, union,  difference, and exclusive-or  of two  sets, check
// equality and inequality of  two set  objects, and determine if  one set is a
// subset of another. In  addition, the notion of  a current position  within a
// set is maintained and reset, next, previous,  find,  and get value functions
// for setting and using the current position are provided.  Also  for use with
// the current position are the next  union, next intersetion, next difference,
// and next  exclusive-or  operations. These    allow a   user to access    the
// first/next individual item that results  from some logical  operation. These
// functions are particularly efficient, since no temporary Set<Type> structure
// is created. Finally, functions to set the  compare and hash functions for an
// instance of a set, get the count of the number of items in a set, output the
// set to a stream, clear all elements from a set,  and determine if  a  set is
// empty are also available.
//
// Use envelope to avoid deep copy and mutate in place.

#ifndef SETH                                    // If no Set class,
#define SETH                                    // define it

#ifndef BASE_HASH_TABLEH                        // If no Base Hash class
#include <cool/Base_Hash.h>                     // define it
#endif

#include <cool/String.h>   //## only because of hack()

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope_H
#define CoolEnvelope CoolEnvelope_Set

template<class CoolLetter> class CoolEnvelope;

template <class Type>
class CoolSet : public CoolBase_Hash_Table {    // Define CoolSet
public:
  typedef long IterState;                       // Current position state
  typedef Boolean (*Compare) (const Type&, const Type&);
  typedef long (*Hash) (const Type&);

  CoolSet();                            // Set of default size
  CoolSet(unsigned long);               // Set for at least size
  CoolSet(const CoolSet<Type>&);                // Set with reference
  ~CoolSet();                           // Destructor

  CoolSet<Type>& operator= (const CoolSet<Type>&); // Assignment
  inline CoolSet<Type>& operator= (CoolEnvelope< CoolSet<Type> >&); // Envelope back to set
  
  Boolean find (const Type&);                   // Set current position
  Type& value ();                               // Get value at current
  Boolean remove ();                            // Remove item current position
  
  Boolean put (const Type&);                    // Put element into set
  Boolean remove (const Type&);                 // Remove element from set
  Boolean search (CoolSet<Type>&);              // Subset operator
  Boolean resize (long);                        // Resize for at least count
  
  CoolSet<Type>& operator|= (const CoolSet<Type>&);     // Union of two sets
  CoolSet<Type>& operator-= (const CoolSet<Type>&);     // Difference of two sets
  CoolSet<Type>& operator^= (const CoolSet<Type>&);     // Exclusive-or of two sets
  CoolSet<Type>& operator&= (const CoolSet<Type>&);     // Intersection of two sets

//   Use envelope to avoid deep copy on return by value, and mutate in place
//   inline friend CoolSet<Type> operator| (const CoolSet<Type>&,const CoolSet<Type>&);
//   inline friend CoolSet<Type> operator- (const CoolSet<Type>&,const CoolSet<Type>&);
//   inline friend CoolSet<Type> operator^ (const CoolSet<Type>&,const CoolSet<Type>&);
//   inline friend CoolSet<Type> operator& (const CoolSet<Type>&,const CoolSet<Type>&);
  
  inline void set_union (const CoolSet<Type>&); // Union of two sets       
  inline void set_difference (const CoolSet<Type>&); // Difference of two sets  
  inline void set_xor (const CoolSet<Type>&);        // Exclusive-or of two sets
  inline void set_intersection (const CoolSet<Type>&); // Intersection of two sets
  
  Boolean next_union (CoolSet<Type>&);          // Return next union item
  Boolean next_difference (CoolSet<Type>&);     // Return next difference item
  Boolean next_xor (CoolSet<Type>&);            // Return next exclusive-or
  Boolean next_intersection (CoolSet<Type>&);   // Return next intersection
  
  template< class U >
  friend ostream& operator<< (ostream&, const CoolSet<U>&); // Overload output
  template< class U >
  inline friend ostream& operator<< (ostream&, const CoolSet<U>*); 

  Boolean operator== (const CoolSet<Type>&) const; // Set equality test
  inline Boolean operator!= (const CoolSet<Type>&) const; // Set inequality test
  
  inline void set_hash (Hash);  // Set the hash function
  void set_compare (Compare = NULL); // Set compare function

private:
  Type next_data;                               // Slot to hold next_union data

  struct Bucket {                       // Structure for bucket
    Type data[BUCKET_SIZE];
  };

  Bucket* table;                                // Pointer to key buckets
  Hash h_function;              // Pointer to hash function
  Compare compare;              // Pointer operator== function
  template< class U >
  friend Boolean CoolSet_are_keys_equal (const U&, const U&);
  template< class U >
  friend long CoolSet_default_hash (const U&);
  
  Boolean do_find (const Type&) const;          // CoolSet current position
};

//## BC++ 3.1 bug
void hack(CoolSet<int>);
void hack(CoolSet<double>);
void hack(CoolSet<char*>);
void hack(CoolSet<CoolString>);
#include <cool/Gen_String.h>
void hack(CoolSet<CoolGen_String>);

//## add your type above
#include <cool/Envelope.h>    //## BC++ 3.1 bug prevents from moving to top

// Use envelope to avoid deep copy on return by value, and mutate in place
template<class Type>
inline CoolEnvelope< CoolSet<Type> > operator| (const CoolSet<Type>&arg1,const CoolSet<Type>&arg2)
   { return (CoolEnvelope< CoolSet<Type> > &) CoolEnvOp(vertical)(arg1, arg2); }
template<class Type>
inline CoolEnvelope< CoolSet<Type> > operator- (const CoolSet<Type>&arg1,const CoolSet<Type>&arg2)
   { return (CoolEnvelope< CoolSet<Type> > &) CoolEnvOp(minus)(arg1, arg2); }
template<class Type>
inline CoolEnvelope< CoolSet<Type> > operator^ (const CoolSet<Type>&arg1,const CoolSet<Type>&arg2)
   { return (CoolEnvelope< CoolSet<Type> > &) CoolEnvOp(caret)(arg1, arg2); }
template<class Type>
inline CoolEnvelope< CoolSet<Type> > operator& (const CoolSet<Type>&arg1,const CoolSet<Type>&arg2)
   { return (CoolEnvelope< CoolSet<Type> > &) CoolEnvOp(ampersand)(arg1, arg2); }

// operator=  -- Assignment from an envelope back to real set
// Input:     envelope reference
// Output:    Set reference with contents in envelope being swapped over

template<class Type>
inline CoolSet<Type>& CoolSet<Type>::operator= (CoolEnvelope< CoolSet<Type> >& env){
  env.shallow_swap((CoolEnvelope< CoolSet<Type> >*)this, &env); // same physical layout
  return *this;
}


// set_union -- Determine the union of two sets
// Input:       Reference to a Bit Set object
// Output:      None

template <class Type> 
inline void CoolSet<Type>::set_union (const CoolSet<Type>& b) {
  this->operator|= (b);
}


// set_difference -- Determine the difference of two sets
// Input:            Reference to a Bit Set object
// Output:           None

template <class Type> 
inline void CoolSet<Type>::set_difference (const CoolSet<Type>& b) {
  this->operator-= (b);
}


// set_xor -- Determine the exclusive-OR of two sets
// Input:     Reference to a Bit Set object
// Output:    None

template <class Type> 
inline void CoolSet<Type>::set_xor (const CoolSet<Type>& b) {
  this->operator^= (b);
}


// set_intersection -- Determine the intersection of two sets
// Input:              Reference to a Bit Set object
// Output:             None

template <class Type> 
inline void CoolSet<Type>::set_intersection (const CoolSet<Type>& b) {
  this->operator&= (b);
}

  
// operator!= -- Return logical result of not equal comparison test
// Input:        Reference to another Bit Set object
// Output:       Boolean TRUE/FALSE

template <class Type> 
inline Boolean CoolSet<Type>::operator!= (const CoolSet<Type>& b) const {
  return (!(this->operator== (b)));
}


// Set_hash -- Set the hash function for this instance
// Input:      Pointer to hash function
// Output:     None

template <class Type> 
inline void CoolSet<Type>::set_hash (register long (*h) (const Type&)) {
  this->h_function = h;
}


// 
// // operator| -- Return the union of two sets, that is all elements in each set
// // Input:       Reference to a set
// // Output:      New Set object containing union of two sets
// 
// template <class Type> CoolSet {
// inline CoolSet<Type> operator| (const CoolSet<Type>& s1, const CoolSet<Type>& s2) {
//   CoolSet<Type> result(s1);                  // Temporary variable
//   result.operator|=(s2);                     // Mutate temp with union
//   return result;                             // Return resulting union
// }}
// 
// 
// // operator- -- Return the difference of two sets, that is all elements in
// //              the first set that are not in the second
// // Input:       Reference to a set
// // Output:      New CoolSet object containing difference of two sets
// 
// template <class Type> CoolSet{
// inline CoolSet<Type> operator- (const CoolSet<Type>& s1, const CoolSet<Type>& s2) {
//   CoolSet<Type> result(s1);                  // Temporary variable
//   result.operator-=(s2);                     // Mutate temp with difference
//   return result;                             // Return resulting union
// }}
// 
// 
// // operator^ -- Return the exclusive-OR of two sets, that is all elements in
// //              the first set that are not in the second and all elements in the
// //              second set that are not in the first
// // Input:       Reference to set
// // Output:      New CoolSet object containing XOR of two sets
// 
// template <class Type> CoolSet {
// inline CoolSet<Type> operator^ (const CoolSet<Type>& s1, const CoolSet<Type>& s2) {
//   CoolSet<Type> result(s1);                  // Temporary variable
//   result.operator^=(s2);                     // Mutate temp with xor
//   return result;                             // Return resulting xor
// }}
// 
// 
// // operator& -- Return the intersection of two sets, that is all elements that
// //              are in both sets
// // Input:       Reference to CoolSet object
// // Output:      New CoolSet object containing intersection of two sets
// 
// template <class Type> CoolSet {
// inline CoolSet<Type> operator& (const CoolSet<Type>& s1, const CoolSet<Type>& s2) {
//   CoolSet<Type> result(s1);                  // Temporary variable
//   result.operator&=(s2);                     // Mutate with intersection
//   return result;                             // Return resulting intersection
// }}
 

// next_intersection -- Position at the next intersection of two Sets.
// Input:               Reference to CoolSet object
// Output:              TRUE/FALSE, current position updated

template <class Type> 
Boolean CoolSet<Type>::next_intersection (CoolSet<Type>& s) {
  if (this->curpos != INVALID && TRAVERSED(this->curpos)) // Traversed already?
    return FALSE;                               // Indicate no more elements
  while (this->next () == TRUE) {               // While there are elements
    if (BUCKET_NUMBER(this->curpos) >= s.get_bucket_count())
      return FALSE;                             // Return failure status
    if (s.find(this->value()) == TRUE)          // If element in 2nd set
      return TRUE;                              // Return success status
  }
  this->curpos = INVALID;                       // Invalidate current position
  return FALSE;                                 // Return failure status
}


// next_union -- Position at the next union of two Sets.
// Input:        Reference to CoolSet object
// Output:       TRUE/FALSE, current position updated

template <class Type> 
Boolean CoolSet<Type>::next_union (CoolSet<Type>& s) {
  if ((this->curpos != INVALID && !TRAVERSED(this->curpos)) ||
      this->curpos == INVALID) {
    if (this->next () == TRUE)                  // If more elements in 1st set
      return TRUE;                              // Return success status
    else                                        // Else set traversed flag
      this->curpos = SET_TRAVERSED(TRUE);
  }
  while (s.next () == TRUE) {                   // While more elements in 2nd 
    if (this->find(s.value()) == FALSE) {       // If element not in 1st set
      this->curpos |= SET_TRAVERSED(TRUE);      // Reset flag zeroed by find
      this->next_data = s.value();              // Refer to next piece of data
      return TRUE;                              // Return success status
    }
  }
  this->curpos = INVALID;                       // Invalidate current position
  return FALSE;                                 // Return failure status
}


// next_difference -- Position at the zero-relative integer of the next bit in 
//                    the difference of two CoolSets.
// Input:             Reference to CoolSet object
// Output:            TRUE/FALSE, current position updated

template <class Type> 
Boolean CoolSet<Type>::next_difference (CoolSet<Type>& s) {
  if (this->curpos != INVALID && TRAVERSED(this->curpos)) // Traversed already?
    return FALSE;                               // Indicate no more elements
  while (this->next () == TRUE) {               // While there are elements
    if (BUCKET_NUMBER(this->curpos) >= s.get_bucket_count())
      return FALSE;                             // Return failure status
    if (s.find(this->value()) == FALSE)         // If element not in 2nd set
      return TRUE;                              // Return success status
  }
  this->curpos = INVALID;                       // Invalidate current position
  return FALSE;                                 // Return failure status
}


// next_xor -- Position at the zero-relative integer of the next bit in 
//             the XOR of two Sets.
// Input:      Reference to CoolSet object
// Output:     TRUE/FALSE, current position updated

template <class Type> 
Boolean CoolSet<Type>::next_xor (CoolSet<Type>& s) {
  if ((this->curpos != INVALID && !TRAVERSED(this->curpos)) ||
      this->curpos == INVALID) {
        if (this->next_difference (s) == TRUE)  // If more elements in 1st set
          return TRUE;                          // Return success status
        else {                                  // Else set traversed flag
          this->curpos = SET_TRAVERSED(TRUE);
          s.reset();                            // Reset current position
        }
      }
  if ((s.curpos != INVALID && !TRAVERSED(s.curpos)) || s.curpos == INVALID) {
    this->reset();                              // Reset 1st set pointer
    if (s.next_difference (*this)) {            // If any other elements in 2nd
      this->curpos |= SET_TRAVERSED(TRUE);      // Reset flag set by find
      this->next_data = s.value();              // Save data for value()
      return TRUE;                              // Return success status
    }
  }
  this->curpos = INVALID;                       // Invalidate current position
  return FALSE;                                 // Return failure status
}


// operator<< -- Overload the output operator to provide a crude print
//               capability for CoolSet objects
// Input:        ostream reference, CoolSet pointer
// Output:       None

template <class Type>
inline ostream& operator<< (ostream& os, const CoolSet<Type>* s) {
  return operator<< (os, *s);
}

//## hack to workaround BC++ 3.1 Envelope bug
#undef CoolEnvelope

#endif                                          // End of SETH
