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
// Created: MBN 06/06/89 -- Initial design and implementation
// Updated: MBN 06/28/89 -- Added protected set and get current position
// Updated: MBN 09/20/89 -- Added conditional exception handling
// Updated: MBN 09/26/89 -- Added method to return key at current position
// Updated: MBN 10/12/89 -- Changed "current_position" to "curpos" and
//                          added current_position() method for Iterator<Type>
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/18/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/18/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolHash_Table=>CoolBase_Hash_Table
// Updated: JAM 08/18/92 -- made *_state typedef a nested typedef "IterState"
//                          as per new Iterator convention
//
// The  Base Hash Table  class implements the  generic hash table functionality
// that is  required  by  the  parameterized  Hash Table class.  The Hash Table
// class is  dynamic in nature.  It's  size (ie. the  number of buckets  in the
// table) is always some prime number. Each bucket holds 8 items. No wholes are
// left in  a bucket; if a  key/value pair is  removed   from  the middle  of a
// bucket, following entries are moved up.  When a hash on  a  key ends up in a
// bucket that is full, the table is enlarged.
//
// The number of buckets currently in the table is  accessed by an index into a
// static table of selected prime  numbers. This static table  contained within
// the  class eliminates the  somewhat expensive  runtime  computation of prime
// numbers.  The table consists of prime numbers  where  the difference between
// any two successive entries gets progressively larger as you move through the
// table.  The specified range of primes results in an  arbitrary limitation of
// 2^22 entries in a single hash table.
//
// The private data section  includes  a pointer to a   byte vector   (that is,
// unsigned char) that  maintains a count  of the number   of  entries in  each
// bucket, a growth ratio that can be used to set a growth rate percentage when
// necessary, an  entry count,  an  index into  the   prime number  table  that
// indicates the number of buckets in the table,  and  a current position union
// that maintains the bucket number and index into the bucket of the  last hash
// operation.
//
// Three different constructors are provided.   The first constructor  takes no
// arguments and  creates a  hash table with  three buckets that  can contain a
// total of 24 items.   The second constructor  accepts an integer argument and
// creates a hash table of some prime number of buckets that  can accomodate at
// least the specified number of items.  Finally, the third constructor takes a
// single argument consisting of a reference to a Hash Table and duplicates its
// size and contents.
//
// The Base Hash Table class implements the notion of a  current position. This
// is useful for  iterating through the table  of hashed values.   The  current
// position is maintained in a union of either a long (used for initialization)
// or in two bit fields.  The first bit field is of length  8 and maintains the
// index of the last item in a bucket.   The  second bit field is of  length 24
// and maintains  the bucket (prime) number last  used.   Methods to reset, and
// move to the next and previous  entries  are provided in  the Base Hash Table
// class.
//
// Methods are provided to to clear all  values from the  table entirely, three
// functions to set the hash,  key compare, and  value compare functions for an
// instance of a hash table, accessor methods to get the bucket and total entry
// count, and a method to set the growth ratio are also available.

#ifndef BASE_HASH_TABLEH                        // If no Base Hash Table class,
#define BASE_HASH_TABLEH                        // define it

#ifndef STREAMH                 // If the Stream support not yet defined,
#include <iostream.h>           // include the Stream class header file
#define STREAMH
#endif

#ifndef MISCELANEOUSH           // If we have not included this file,
#include <cool/misc.h>          // include miscelaneous useful definitions.
#endif

#define BUCKET_SIZE 8

extern long hash_primes[];

// The following preprocessor macros get and set the relative bits in  a 32 bit
// field that represent the  bucket  index,  bucket number, and traversed  bit.
// This is  used instead of a  bit  field because of the non-portability issues
// surrounding bit fields in various C compilers. Also  note that the traversed
// bit macro is defined here, although it is utilized in the Set class.

#define BUCKET_INDEX(x) ((x & 0xFF000000L) >> 24)
#define BUCKET_NUMBER(x) (x & 0x007FFFFFL)
#define TRAVERSED(x) ((x & 0x00800000L) >> 23)

#define SET_BUCKET_INDEX(x) ((x & 0x000000FFL) << 24)
#define SET_BUCKET_NUMBER(x) (x & 0x007FFFFFL)
#define SET_TRAVERSED(x) ((x & 0x00000001L) << 23)
 
class CoolBase_Hash_Table {                             // Base Hash class definition
protected:
  unsigned char* items_in_buckets;              // Count of entries in buckets
  float growth_ratio;                           // Growth ratio
  long entry_count;                             // Number of entries in table
  long current_bucket;                          // Index to number of buckets
  long curpos;                                  // BUCKET_NUM*BUCKET_SIZE+INDEX

  CoolBase_Hash_Table ();                               // Hash table of default size
  CoolBase_Hash_Table (long);                           // Hash table for at least size
  CoolBase_Hash_Table (const CoolBase_Hash_Table&);             // Hash table with reference

  void ratio_error (float);                          // Raise exception
  void resize_error (const char*, const char*, long); // Raise exception
  void value_error (const char*, const char*);       // Raise exception
  void key_error (const char*, const char*);         // Raise exception
  void remove_error (const char*, const char*);      // Raise exception

public:
  typedef long IterState;                       // Current position state

  ~CoolBase_Hash_Table();                               // Destructor
  CoolBase_Hash_Table& operator= (const CoolBase_Hash_Table&);  // Assignment

  inline long length () const;                  // Return number of entries
  inline long capacity () const;                // Return max number of entries
  inline Boolean is_empty () const;             // Determine empty/nonempty 
  inline long get_bucket_count () const;        // Return number of buckets
  inline void set_ratio (float);                // Set growth ratio
  inline int get_count_in_bucket(long) const;   // Used to return item count
  inline IterState& current_position ();        // Get/set current position

  inline void reset ();                         // Make current position invalid
  Boolean next ();                              // Advance current position by 1
  Boolean prev ();                              // Backup current position by 1
  void statistics ();                           // Print Table Statistics
  void clear ();                                // Empty the hash table
};


// Get_bucket_count -- Return number of buckets in hash table
// Input:              this*
// Output:             Number of buckets in hash table           

inline long CoolBase_Hash_Table::get_bucket_count () const {
  return hash_primes[this->current_bucket];
}


// length -- Return number of entries in hash table
// Input:    this*
// Output:   Number of entries in hash table

inline long CoolBase_Hash_Table::length () const {
  return this->entry_count;
}


// current_position () -- Return current position state
// Input:                 None
// Output:                Reference to current position state

inline CoolBase_Hash_Table::IterState& CoolBase_Hash_Table::current_position () {
  return this->curpos;
}


// capacity -- Return maximum number of elements object can hold
// Input:      None
// Output:     Integer value of maximum number of elements

inline long CoolBase_Hash_Table::capacity () const {
  return hash_primes[this->current_bucket] * BUCKET_SIZE; //Max number entries
}


// is_empty -- Return empty status of hash table
// Input:      this*
// Output:     TRUE/FALSE

inline Boolean CoolBase_Hash_Table::is_empty () const {
  return this->entry_count == 0;
}


// set_ratio -- Set the growth ratio
// Input:       this*, growth ratio
// Output:      None

inline void CoolBase_Hash_Table::set_ratio (float ratio) {
#if ERROR_CHECKING
  if (ratio < 0.0)                              // If invalid ratio
    this->ratio_error (ratio);                  // Raise exception
#endif
  this->growth_ratio = ratio;                   // Set growth ratio
}


// get_count_in_bucket -- Return the number of items in specific bucket
// Input:                 Bucket number
// Output:                Number of items in bucket

inline int CoolBase_Hash_Table::get_count_in_bucket (long i) const {
  return items_in_buckets[i];
}


// reset -- Set current position to INVALID
// Input:   this*
// Output:  None

inline void CoolBase_Hash_Table::reset () {
  this->curpos = INVALID;                       // Invalidate current position
}

extern unsigned long sxhash (const char*);      // Calculate hash for string
extern Boolean charP_compare (char* const &, char* const&);// Key compare for char*

#endif                                          // End of BASE_HASH_TABLEH
