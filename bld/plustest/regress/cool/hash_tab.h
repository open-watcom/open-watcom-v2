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
// Created: MBN 05/16/89 -- Initial design and implementation
// Updated: MBN 06/01/89 -- Implemented the notion of current position.
// Updated: MBN 06/06/89 -- Separated and derived Hash_Table from Base_Hash
//                          to reduce replication of common code.
// Updated: LGO 06/20/89 -- Use correct default hash and compare for char* keys
// Updated: LGO 07/03/89 -- Fix resize bug in the put method
// Updated: MBN 08/19/89 -- Changed template usage
// Updated: MBN 09/20/89 -- Added conditional exception handling
// Updated: MBN 09/26/89 -- Added method to return key at current position
// Updated: LGO 10/02/89 -- Substituted Tkey and Tval for T1 and T2
// Updated: MBN 10/07/89 -- Changed get() method to match Association + Symbol
// Updated: MBN 10/11/89 -- Fixed operator==() for tables with different bucket
//                          count but same elements -- tables grew separately
// Updated: MBN 10/12/89 -- Changed "current_position" to "curpos", added
//                          current_position() method for Iterator<Type>, and
//                          converted state from bit set to bit set/get macros
// Updated: LGO 10/16/89 -- Re-write operator<< to be const
// Updated: MBN 10/19/89 -- Made compare_keys and compare_values_s slots static
//                          and added optional argument to set_compare methods
// Updated: MBN 12/15/89 -- Fixed no-dereferenced pointer-to-function in find()
// Updated: LGO 02/07/90 -- Change resize to not use a tempoorary for key and
//                          value.  Avoids extra constructor and destructor
//                          calls.
// Updated: MBN 02/23/90 -- Changed size arguments from long to unsigned long
// Updated: MJF 06/30/90 -- Added base class name to constructor initializer
// Updated: VDN 02/21/92 -- New lite version
// Updated: JAM 08/18/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/18/92 -- modernized template syntax, remove macro hacks
//                          non-template classes CoolHash_Table=>CoolBase_Hash_Table
//                          *_pair and *_bucket=>CoolHash_Table_*<Tkey,Tval>
//
// The  Hash_Table<Tkey,Tval> class is  publicly  derived from  the  Hash_Table
// class and implements hash tables  of user-specified types  for both the  key
// and the  value.   This is accompilshed  by    using  the parameterized  type
// capability  of C++.   The Hash Table class is  dynamic in nature.  It's size
// (ie.  the number of buckets in the table) is always some prime number.  Each
// bucket holds 8 items.  No wholes are  left in a bucket;  if a key/value pair
// is removed   from the middle of  a  bucket, following  entries are moved up.
// When a  hash  on  a key ends up   in a bucket that  is  full, the  table  is
// enlarged.  The Hash_Table<Tkey,Tval> class  is parameterized over two types.
// The first type Tkey specifies the type of the key,  and the second type Tval
// specifies the type of the value.
//
// The private  data section of  a Hash  Table  has a  slot that points  to the
// physical memory allocated for some prime number of buckets each of which has
// memory allocated for 8 items.  The number of buckets  currently in the table
// is accessed by an index into a global table of selected prime numbers.  This
// global table eliminates the somewhat  expensive runtime computation of prime
// numbers.  The table consists of  prime numbers where  the difference between
// any two successive entries gets progressively larger as you move through the
// table.  The specified range of primes  results in an arbitrary limitation of
// 2^22 entries in a single hash table.
//
// When a hash on a key ends up in a bucket that is full, the table is enlarged
// to the next prime number of buckets or to the prime number  that is at least
// large enough  to  accommodate a user-specified  growth ratio. The entries in
// the  buckets are  then rehashed  into   the new  table.   Selection  of   an
// appropriate   hash function is crucial  to  uniform distribution through the
// table. The result of the hash function is  then divided by the  prime number
// of buckets to accomplish this. A user-supplied  function should do something
// similar.
//
// Other items in the private data section (inherited from Base Hash) include a
// pointer to a byte vector (that is, unsigned char) that  maintains a count of
// the number of entries in each bucket, a growth ratio that can be used to set
// a growth rate percentage when necessary,  an entry count,  an index into the
// prime number table that indicates the number of  buckets in the table, and a
// current position  that maintains the bucket number and index into the bucket
// of the last hash operation.
//
// Three other slots contain a pointer to a key  compare function, a pointer to
// a value compare  function, and a pointer to  a  hash function, respectively.
// The compare functions are  used  in equality  operations on key/value items.
// The default compare function is the  built-in == operator.  The default hash
// function  is either a  simple 32 bit  value if sizeof(Tkey)   is 4,  that is
// shifted left three   bits  with the result  modulo  the  number   of buckets
// determining the  hash. This is  ideal when Tkey is a  pointer to  a key.  If
// sizeof(Tkey) is greater than 4, then the 32 bit value used  is the result of
// exclusive-oring successive 32-bit values  for the  length of Tkey,  and then
// applying the same bit shift and modulo operation as before.
//
// Three different constructors are provided.   The first constructor  takes no
// arguments and  creates a  hash table with  three buckets that  can contain a
// total of 24 items.   The second constructor  accepts an integer argument and
// creates a hash table of some prime number of buckets that  can accomodate at
// least the specified number of items.  Finally, the third constructor takes a
// single argument consisting of a reference to a Hash Table and duplicates its
// size and contents.
//
// The  Hash Table  class implements the notion  of a current position. This is
// useful for  iterating through    the  table of  hashed values.   The current
// position is maintained in a a long that is accessed via several preprocessor
// macros to select bits. The first bit field is of length 24 and maintains the
// bucket  (prime) number last  used.  The second bit field is  of length 8 and
// maintains the index of the last item in a  bucket. Methods to reset, move to
// the next and previous, find, and get  the value at  the current position are
// provided.
//
// Methods are provided to put a value based on some  key into the table, get a
// value based on some key from the  table,  remove  a value  based on some key
// from the table, and to clear all values from the table entirely. The output,
// assignment, and equality operators are overloaded for hash  tables. Finally,
// two functions to  set the hash  and compare functions  for an instance of  a
// hash table, accessor methods to get the bucket and total entry count,  and a
// method to set the growth ratio are also available.

#ifndef HASH_TABLEH                             // If no Hash Table class,
#define HASH_TABLEH                             // define it

#ifndef BASE_HASH_TABLEH                        // If no Base Hash class,
#include <cool/Base_Hash.h>                     // define it
#endif  


template <class Tkey, class Tval>
struct CoolHash_Table_Pair {            // Structure for hash/value
  Tkey key;
  Tval value;
};

template <class Tkey, class Tval>
struct CoolHash_Table_Bucket {          // Structure for bucket
  struct CoolHash_Table_Pair<Tkey,Tval> data[BUCKET_SIZE];
};

template <class Tkey, class Tval>
class CoolHash_Table : public CoolBase_Hash_Table {
public:
  typedef Boolean
     (*Key_Compare) (const Tkey&, const Tkey&);
  typedef Boolean
     (*Value_Compare) (const Tval&, const Tval&);
  typedef unsigned long (*Hash) (const Tkey&);

  CoolHash_Table();                     // Hash table of default size
  CoolHash_Table(unsigned long);        // Hash table for at least size
  CoolHash_Table(const CoolHash_Table<Tkey,Tval>&); // Copy constructor
  ~CoolHash_Table();                    // Destructor

  Boolean put (const Tkey&, const Tval&);       // Hash key/value
  Boolean get (const Tkey&, Tval&);             // Get associated value for key
  Boolean get_key (const Tval&, Tkey&);         // Get associated key for value
  Boolean remove (const Tkey&);                 // Remove key/value from table
  void resize (long);                           // Resize for at least count
  
  Boolean find (const Tkey&);                   // Set current position
  const Tkey& key ();                           // Get key at current position
  Boolean remove ();                            // Remove key/value at curpos
  const Tval& value ();                         // value at current position

  CoolHash_Table<Tkey,Tval>& operator= (const CoolHash_Table<Tkey,Tval>&);
  Boolean operator== (const CoolHash_Table<Tkey,Tval>&); // is equal
  inline Boolean operator!= (const CoolHash_Table<Tkey,Tval>&); // is not eq

  inline void set_hash (Hash); // Set hash function
  void set_key_compare (Key_Compare = NULL); 
  void set_value_compare (Value_Compare = NULL);

  template< class K, class V >
  friend ostream& operator<< (ostream&, const CoolHash_Table<K,V>&);
  template< class K, class V >
  inline friend ostream& operator<< (ostream&, const CoolHash_Table<K,V>*);

protected:
  CoolHash_Table_Bucket<Tkey,Tval>* table;      // Pointer to key/value buckets
  Hash h_function;      // Pointer to hash function
  static Key_Compare compare_keys_s; // Key compare
  static Value_Compare compare_values_s; // Value compare
  template< class K >
  friend Boolean CoolHash_Table_keys_equal (const K&, const K&);
  template< class V >
  friend Boolean CoolHash_Table_values_equal (const V&, const V&);
  template< class K >
  friend unsigned long CoolHash_Table_default_hash (const K& key);
};

// operator<< -- Overload the output operator to provide a crude print
//               capability for hash table objects
// Input:        ostream reference, hash table pointer
// Output:       None

template <class Tkey, class Tval>
inline ostream& operator<< (ostream& os, const CoolHash_Table<Tkey,Tval>* h) {
  return operator<< (os, *h);
}




// operator!= -- Determine if two hash tables are unequal
// Input:        this*, reference to second hash table
// Output:       TRUE/FALSE

template <class Tkey, class Tval> 
inline Boolean CoolHash_Table<Tkey,Tval>::operator!= (const CoolHash_Table<Tkey,Tval>& t) {
  return (!operator== (t));
}




// Set_hash -- Set the hash function for this instance
// Input:      Pointer to hash function
// Output:     None

template <class Tkey, class Tval> 
inline void CoolHash_Table<Tkey,Tval>::set_hash (
   /*##Hash*/unsigned long (*h) (const Tkey&)) {
  this->h_function = h;
}

#endif                          // End #ifdef of HASH_TABLEH
