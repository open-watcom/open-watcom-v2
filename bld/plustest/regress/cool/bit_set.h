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
// Created: MBN 06/07/89 -- Initial design
// Updated: LGO 08/09/89 -- Inherit from Generic
// Updated: MBN 09/31/89 -- Added conditional exception handling
// Updated: MBN 10/07/89 -- Changed operator[-~^&|] to allocate on stack
//                          Bit_Set::find(int) returns state of indicated bit
// Updated: MBN 10/12/89 -- Changed "current_position" to "curpos" and added
//                          the current_position() method for Iterator<Type>
// Updated: MBN 10/13/89 -- Changed from bit field to preprocessor bit macros
// Updated: LGO 11/09/89 -- Major changes to every method
// Updated: MBN 12/22/89 -- Sprinkled "const" qualifier all over the place!
// Updated: DLS 03/26/91 -- New lite version
// Updated: JAM 08/12/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/12/92 -- removed 'inline' from friend declarations
// Updated: JAM 08/18/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
// Updated: JAM 09/26/92 -- changed #define ENVELOPE_* to #undef after
//                          #include <Envelope.h> -- probably just typo
//
// The Bit Set class is publicy  derived from the  Generic class and implements
// efficient bit  sets. These bits are stored  in an arbitrary length vector of
// bytes  (unsigned char)  large  enough to represent   the specified number of
// elements.  A  Bit Set is  indexed  by integer values.  Zero   represents the
// first bit, one the second,  two third, and so  on with  each  integer  value
// actually  indicating the  zero-relative  bit  position  in the  bit  vector.
// Elements can be integers, enum values, constant symbols from the enumeration
// package, or any   other type of   object or expression  that results  in  an
// integral value.  All  operations that  involve bit shifting are performed in
// byte  increments to affect the  most efficient operation on  common hardware
// architectures.
//
// The Bit Set class is dynamic  in nature.  The  largest element integer value
// determines the most most significant bit in the bit vector.  If a particular
// value results in a bit index that is outside of the storage allocated, a new
// vector of large  enough size  is created and the  appropriate bits set.  The
// storage used  by the smaller vector is  returned  to  the heap.  The private
// data section of  a Bit Set  has a slot that  points  to  the physical memory
// allocated as unsigned  char,a  slot that maintains  the  count of  allocated
// bytes, and a slot that maintains the current position.
//
// Three different constructors are provided.   The  first constructor takes no
// arguments and creates a  Bit Set  that can accept BITSPERBYTE  elements (one
// byte).  The second constructor accepts an integer argument and creates a Bit
// Set that can accomodate at  least the specified  number of items.   Finally,
// the third constructor takes  a single argument consisting  of a reference to
// another Bit Set and duplicates its size and contents.
//
// Methods are provided to put, test, and remove an element from a set, perform
// the intersection, union, difference, and exclusive-or of two sets, calculate
// the complement of  a single set,  check equality and inequality of   two set
// objects, and determine if one set is a subset of  another.  In addition, the
// notion of a  current position within  a set is  maintained  and reset, next,
// previous, find, and  get  value functions for setting and  using the current
// position are provided.
//
// Also for use with the current position are the next union, next intersetion,
// next difference, and  next exclusive-or operations.   These  allow a user to
// access the first/next  individual  element that results   from  some logical
// operation. These  functions are  particularly efficient for  looping through
// the elements of a Bit Set, since no temporary Bit Set structure  is created.
// Finally, methods to get the count of the  number of  items in  a set, output
// the set to a stream, clear all elements from  a set, and determine  if a set
// is empty are also available.
//
// Use envelope to avoid deep copy on return by value, and do set operations
// in place.

#ifndef BIT_SETH                                // If no Set class,
#define BIT_SETH                                // define it

#include <iostream.h>

#ifndef MISCELANEOUSH                           // If no misc.h file
#include <cool/misc.h>                  // Include useful defintions
#endif  

#define BIT_SET_BLK_SZ 4

#define BS_BITSPERBYTE 8                        // Number of bits in a byte
#define BS_BYTE_OFFSET(n) ((int) (n & 0x07))    // These assume 8 bits per byte
#define BS_BYTE_NUMBER(n) ((int) (n >> 3))

class CoolBit_SetE;                             // Forward dec. of envelope

class CoolBit_Set {
public:
  typedef long IterState;                       // Current position state

  CoolBit_Set ();                               // bit set of zero byte size
  CoolBit_Set (int);                            // Bit set for at least size
  CoolBit_Set (const CoolBit_Set&);             // Copy constructor
  ~CoolBit_Set();                               // Destructor

  CoolBit_Set& operator= (const CoolBit_Set&);  // Assignment operator
  inline CoolBit_Set& operator= (CoolBit_SetE&);// Envelope back to Bit_Set

  inline void reset ();                         // Invalidate current position
  inline int value ();                          // Value at current position
  Boolean next ();                              // Advance current position
  Boolean prev ();                              // Backup current position 
  Boolean find (int);                           // Return state of bit
  inline IterState& current_position (); // Return current position
  
  Boolean put (int);                            // Put element to set
  Boolean put (int, int);                       // Put range of elements to set
  
  Boolean remove ();                            // Remove item current position
  Boolean remove (int);                         // Remove element from set
  Boolean remove (int, int);                    // Remove range from set

  inline Boolean is_empty () const;             // Return empty status
  Boolean search (const CoolBit_Set&) const;    // Subset operator

  CoolBit_SetE operator- ();                    // Complement of set
  CoolBit_Set& operator|= (const CoolBit_Set&); // Union of two sets
  CoolBit_Set& operator-= (const CoolBit_Set&); // Difference of two sets
  CoolBit_Set& operator^= (const CoolBit_Set&); // Exclusive-or of two sets
  CoolBit_Set& operator&= (const CoolBit_Set&); // Intersection of two sets

  inline CoolBit_SetE operator~ ();                     // Complement of a set

// avoid deep copy and mutate in place with envelope
//   inline friend CoolBit_SetE operator| (const CoolBit_Set&, const CoolBit_Set&);
//   inline friend CoolBit_SetE operator- (const CoolBit_Set&, const CoolBit_Set&);
//   inline friend CoolBit_SetE operator^ (const CoolBit_Set&, const CoolBit_Set&);
//   inline friend CoolBit_SetE operator& (const CoolBit_Set&, const CoolBit_Set&);

  inline void set_union (const CoolBit_Set&);   // Union of two sets       
  inline void set_difference (const CoolBit_Set&); // Difference of two sets  
  inline void set_xor (const CoolBit_Set&);        // Exclusive-or of two sets
  inline void set_intersection (const CoolBit_Set&); // Intersection of two sets

  Boolean next_union (const CoolBit_Set&);              // Next union item
  Boolean next_difference (const CoolBit_Set&); // Next differenceitem
  Boolean next_xor (const CoolBit_Set&);                // Next exclusive-or 
  Boolean next_intersection (const CoolBit_Set&);       // Next intersextion item

  int length () const;                          // Return number of elements
  void clear ();                                // Empty the set
  void resize (int);                            // Resize for at least count
  inline int capacity () const;                 // Return maximum capacity
  void set_growth_ratio (float);                // Set growth percentage
  void set_alloc_size (int);                    // Set alloc size

  inline Boolean operator[] (int) const;        // Return bit present status

  friend ostream& operator<< (ostream&, const CoolBit_Set&); // Overload output
  friend ostream& operator<< (ostream&, const CoolBit_Set*);

  friend Boolean operator== (const CoolBit_Set&, const CoolBit_Set&);// Set equality test
  friend Boolean operator!= (const CoolBit_Set&, const CoolBit_Set&); // Set inequality test

private:
  int size;                                     // Number of BYTES allocated
  int number_elements;                          // Index of last BYTE used
  static int alloc_size_s;                      // Allocation size for growth
  float growth_ratio;                           // If non-zero, growth ratio 
  unsigned char* data;                          // Allocated bit-vector
  long curpos;                                  // Bit index

  void grow (int min_size);                     // Make the bitset bigger
  void value_error () const;                    // Raise exception
  void bracket_error (int) const;               // Raise exception
  void find_error (int) const;                  // Raise exception
  void put_error (int, int) const;              // Raise exception
  void remove_error () const;                   // Raise exception
  void rem_start_end_error (int, int) const;    // Raise exception
};

// Avoid deep copy, and do set operations in place with envelope

#define ENVELOPE_VERTICAL                       // |= can be done in place
#define ENVELOPE_AMPERSAND                      // &= can be done in place
#define ENVELOPE_MINUS                          // -= can be done in place
#define ENVELOPE_CARET                          // ^= can be done in place

#define CoolLetter CoolBit_Set
#define CoolEnvelope CoolBit_SetE

#include <cool/Envelope.h>                      // Include envelope macros

#undef ENVELOPE_VERTICAL        
#undef ENVELOPE_AMPERSAND       
#undef ENVELOPE_MINUS           
#undef ENVELOPE_CARET           

#undef CoolLetter
#undef CoolEnvelope


// operator=  -- Assignment from an envelope back to real set
// Input:     envelope reference
// Output:    Bit_Set reference with contents in envelope being swapped over

inline CoolBit_Set& CoolBit_Set::operator= (CoolBit_SetE& env){
  env.shallow_swap((CoolBit_SetE*)this, &env);  // same physical layout
  return *this;
}


// reset -- Reset the current position index
// Input:   None
// Output:  None

void CoolBit_Set::reset () {
  this->curpos = INVALID;                       // Invalidate current position
}


// value -- Return "position" of element at current position
// Input:   None
// Output:  Position of element in Set

inline int CoolBit_Set::value () {
#if ERROR_CHECKING
  if (this->curpos == INVALID)                  // If INVALID current position
    this->value_error ();                       // Raise exception
#endif
  return (int) this->curpos;
}


// current_position () -- Return current position state
// Input:                 None
// Output:                Reference to current position state

inline CoolBit_Set::IterState& CoolBit_Set::current_position () {
  return this->curpos;
}


// is_empty -- Return value indicating whether set contains any elements
// Input:      None
// Output:     TRUE/FALSE

inline Boolean CoolBit_Set::is_empty () const {
  return ((this->length() == 0) ? TRUE : FALSE);
}


// operator~ -- Return complement of a set
// Input:       None
// Output:      Bit Set object containing complement of this set

inline CoolBit_SetE CoolBit_Set::operator~ () {
  return (CoolBit_SetE &) (this->operator- ());
}


// // operator| -- Return the union of two sets, that is all elements in each set
// // Input:       Reference to a bit set
// // Output:      New Bit Set object containing union of two sets
// 
// inline CoolBit_Set CoolBit_Set::operator| (const CoolBit_Set& b) {
//   CoolBit_Set result(*this);
//   return result.operator|=(b);
// }
// 
// 
// // operator- -- Return the difference of two sets, that is all elements in the
// //              the first set that are not in the second
// // Input:       Reference to bit set
// // Output:      New Bit Set object containing union of two sets
// 
// inline CoolBit_Set CoolBit_Set::operator- (const CoolBit_Set& b) {
//   CoolBit_Set result(*this);
//   return result.operator-=(b);
// }
// 
// 
// // operator^ -- Return the exclusive-OR of two sets, that is all elements in
// //              the first set that are not in the second and all elements in
// //              the second set that are not in the first
// // Input:       Reference to bit set
// // Output:      New Bit Set object containing XOR of two sets
// 
// inline CoolBit_Set CoolBit_Set::operator^ (const CoolBit_Set& b) {
//   CoolBit_Set result(*this);
//   return result.operator^=(b);
// }
// 
// 
// // operator& -- Return the intersection of two sets, that is all elements that
// //              are in both sets
// // Input:       Reference to Bit Set object
// // Output:      New Bit Set object containing intersection of two sets
// 
// inline CoolBit_Set CoolBit_Set::operator& (const CoolBit_Set& b) {
//   CoolBit_Set result(*this);
//   return result.operator&=(b);
// }


// set_union -- Determine the union of two sets
// Input:       Reference to a Bit Set object
// Output:      None

inline void CoolBit_Set::set_union (const CoolBit_Set& b) {
  this->operator|= (b);
}


// set_difference -- Determine the difference of two sets
// Input:            Reference to a Bit Set object
// Output:           None

inline void CoolBit_Set::set_difference (const CoolBit_Set& b) {
  this->operator-= (b);
}


// set_xor -- Determine the exclusive-OR of two sets
// Input:     Reference to a Bit Set object
// Output:    None

inline void CoolBit_Set::set_xor (const CoolBit_Set& b) {
  this->operator^= (b);
}


// set_intersection -- Determine the intersection of two sets
// Input:              Reference to a Bit Set object
// Output:             None

inline void CoolBit_Set::set_intersection (const CoolBit_Set& b) {
  this->operator&= (b);
}

  
// operator!= -- Return logical result of not equal comparison test
// Input:        Reference to another Bit Set object
// Output:       Boolean TRUE/FALSE

inline Boolean operator!= (const CoolBit_Set& b1, const CoolBit_Set& b2){
  return (!(b1 == b2));
}


// capacity -- Return maximum number of elements object can hold
// Input:      None
// Output:     Integer value of maximum number of elements

inline int CoolBit_Set::capacity () const {
  return (this->size * BS_BITSPERBYTE);         // Maximum number of elements
}


// operator[] -- Return value of specified element 
// Input:        Element to return value (really, it's just an integer value
// Output:       None

inline Boolean CoolBit_Set::operator[] (int n) const {
#if ERROR_CHECKING
  if (n >= this->size * BS_BITSPERBYTE)         // If bit out of range
    this->bracket_error (n);                    // Raise exception
#endif
  return((this->data[BS_BYTE_NUMBER(n)]>>(BS_BYTE_OFFSET(n))) & 0x01);
}


// operator<< -- Overload the output operator for Bit Set objects
// Input:        Reference to stream, pointer to Bit Set object
// Output:       Reference to stream

inline ostream& operator<< (ostream& os, const CoolBit_Set* b) {
  return operator<< (os, *b);
}

#endif                                          // End #ifdef of CoolBit_SetH
