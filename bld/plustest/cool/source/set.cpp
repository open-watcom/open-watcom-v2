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

#include <cool/Set.h>

// CoolSet -- Simple constructor with no arguments that creates a CoolSet object
//        with the minimal prime number of buckets and uses the default
//        hash function.
// Input: None
// Output:None

template <class Type> 
CoolSet<Type>::CoolSet() {
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->table = new Bucket[prime];       // Allocate buckets
  this->h_function = &CoolSet_default_hash; // Setup hash 
  this->compare = &CoolSet_are_keys_equal;  // Setup compare
}


// CoolSet -- Simple constructor with one argument that creates a CoolSet object
//        with the minimal prime number of buckets that holds some
//        user-supplied number of items and uses the default hash function
// Input: Minimal number of items table must hold
// Output:None

template <class Type> 
CoolSet<Type>::CoolSet(unsigned long n)
 : CoolBase_Hash_Table(n)
{
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->table = new Bucket[prime];        // Allocate buckets
  this->h_function = &CoolSet_default_hash; // Setup hash 
  this->compare = &CoolSet_are_keys_equal;  // Setup compare
}


// CoolSet -- constructor that takes a reference to an existing CoolSet object and
//        duplicates both its size and contents
// Input: Reference to CoolSet object
// Output:None

template <class Type> 
CoolSet<Type>::CoolSet(const CoolSet<Type>& s)
 : CoolBase_Hash_Table(s)
{
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->table = new Bucket[prime];       // Allocate bucksgs
  for (long i = 0; i < prime; i++) {             // For each bucket count
    for (int j = 0; j < s.items_in_buckets[i]; j++) // For each item in bucket
      this->table[i].data[j] = s.table[i].data[j];  // Copy key/value
  }
  this->h_function = s.h_function;              // Use the same hash function
  this->compare = s.compare;                    // Use same compare function
}


// ~CoolSet -- Destructor for the CoolSet class
// Input:  this*
// Output: None

template <class Type> 
CoolSet<Type>::~CoolSet() {
  delete [] this->table;                        // Free key/value storage
}

// Operator== -- Determine if two hash tables are equal. This is accompilished
//               by seeing that for each key/value in SeType, the same
//               key/value also exists in Set2
// Input:        this*, reference to second set
// Output:       TRUE/FALSE

template <class Type> 
Boolean CoolSet<Type>::operator== (const CoolSet<Type>& s) const {
  if (this->length() != s.length())            // If not same number of entries
    return FALSE;                              // Then tables are not equal
  if (this->get_bucket_count() == s.get_bucket_count()) { // If same bucket cnt
    for (long i = 0; i < this->get_bucket_count(); i++) { // for each bucket
      int count = this->get_count_in_bucket(i);
      if (count != s.get_count_in_bucket(i))    //Count eq?
        return FALSE;                           // No, tables !equal
      Type* this_bucket = this->table[i].data;
      Type* s_bucket = s.table[i].data;
      for (int j = 0; j < count; j++) {         // For each item in this
        for (int k = 0; k < count; k++)         // For each item in s
          if ((*this->compare)(this_bucket[j], s_bucket[k]))
            goto good;
        return FALSE;                           // Not the same, so tables !eq
      good: ;
      }
    }
  } else {
    for (long i = 0; i < s.get_bucket_count (); i++)      // For each bucket
      for (int j = 0; j < s.get_count_in_bucket(i); j++)  // For each item
        if (!this->do_find(s.table[i].data[j]))           // If key in table
          return FALSE;                        // Key not in table so different
  }
  return TRUE;                                    // No difference, so equal
}


// do_find -- Find key/value in CoolSet
// Input:  Key searching for
// Output: TRUE/FALSE, current_position not updated

template <class Type> 
Boolean CoolSet<Type>::do_find (const Type& key) const {
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value
  for (long i = 0; i < this->items_in_buckets[hash]; i++) { // For each entry
    if (((*this->compare)(key,this->table[hash].data[i])) == TRUE)
      return TRUE;                              // Return success
  }
  return FALSE;
}

// find -- Find key/value in Set
// Input:  Key searching for
// Output: TRUE/FALSE; current_position updated

template <class Type> 
Boolean CoolSet<Type>::find (const Type& key) {
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value
  for (long i = 0; i < this->items_in_buckets[hash]; i++) { // For each entry
    if (((*this->compare)(key,this->table[hash].data[i])) == TRUE){
      this->curpos = SET_BUCKET_NUMBER(hash);   // Set bucket number
      this->curpos |= SET_BUCKET_INDEX(i);      // Set index into bucket
      this->curpos |= SET_TRAVERSED(FALSE);     // Reset traverse flag
      return TRUE;                              // Return success
    }
  }
  return FALSE;
}


// value -- Return value at current position
// Input:   None
// Output:  Reference to value at current position

template <class Type> 
Type& CoolSet<Type>::value () {
#if ERROR_CHECKING
  if (this->curpos == INVALID) {                // If invalid current positio
    //RAISE Error, SYM(CoolSet), SYM(Invalid_Cpos)
    printf ("CoolSet<%s>::value(): Invalid current position.\n", #Type);
    abort ();
  }
#endif
  if (TRAVERSED(this->curpos))                  // If data in 2nd set
     return this->next_data;                    // Return saved value
  else {
    unsigned long hash = BUCKET_NUMBER(this->curpos); // Get bucket number
    long index = BUCKET_INDEX(this->curpos);          // Get Bucket index
    return (this->table[hash].data[index]);           // Return value
  }
}


// ***** bug? remove does not leave current position pointed at next element?
// remove -- Remove element at current position from the set
// Input:    this*
// Output:   TRUE/FALSE

template <class Type> 
Boolean CoolSet<Type>::remove () {
  if (this->curpos == INVALID) {                // If valid current position
#if ERROR_CHECKING
    //RAISE Error, SYM(CoolSet), SYM(Invalid_Cpos)
    printf ("CoolSet<%s>::remove(): Invalid current position.\n", #Type);
    abort ();
#endif
    return FALSE;                               // Return failure
  }
  unsigned long hash = BUCKET_NUMBER(this->curpos); // Get bucket number
  long index = BUCKET_INDEX(this->curpos);          // Get index in bucket
  int count = this->items_in_buckets[hash];     // Number of items in bucket
  this->table[hash].data[index] = this->table[hash].data[count-1];
  this->entry_count--;                          // Decrement table entry count
  this->items_in_buckets[hash]--;               // Decrement bucket item count
  if (this->items_in_buckets[hash]) {           // If any more items in bucket
    this->curpos = SET_BUCKET_NUMBER(hash);     // Save bucket number
    this->curpos |= SET_BUCKET_INDEX(this->items_in_buckets[hash]-1);
    this->curpos |= SET_TRAVERSED(FALSE);       // Reset traverse flag
  }
  else
    this->curpos = INVALID;                     // Else invalidate marker
  return TRUE;                                  // Return success
}


// search -- Determine if one CoolSet is a subset of another
// Input:    Reference to a CoolSet object
// Output:   TRUE/FALSE

template <class Type> 
Boolean CoolSet<Type>::search (CoolSet<Type>& s) {
  if (this->length() < s.length())              // If more elements in 2nd set
    return FALSE;                               // Then it is not a subset
  for (s.reset(); s.next(); )                   // For each entry in 2nd set
    if (this->find (s.value()) == FALSE)        // If not in 1st set
      return FALSE;                             // Then it's not a subset
  return TRUE;                                  // Else it's a subset
}


// put -- Hash key/value pair into table if not already there
// Input: this*, key, value
// Output:TRUE/FALSE

template <class Type> 
Boolean CoolSet<Type>::put (const Type& key) {
retry:  
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value
  if (this->items_in_buckets[hash] == BUCKET_SIZE) {       // If bucket is full
    this->resize (hash_primes[this->current_bucket++]*BUCKET_SIZE);
    goto retry;                                 // Try again 
  }
  this->table[hash].data[this->items_in_buckets[hash]] = key;
  this->entry_count++;                          // Increment table entry count
  this->curpos = SET_BUCKET_NUMBER(hash);       // Save bucket number
  this->curpos |= SET_BUCKET_INDEX(this->items_in_buckets[hash]);
  this->curpos |= SET_TRAVERSED(FALSE);         // Reset traverse flag
  this->items_in_buckets[hash]++;               // Increment bucket item count
  return TRUE;                                  // Indicate success
}


// remove -- Remove an element from the set
// Input:    this*, reference to a key
// Output:   TRUE/FALSE

template <class Type> 
Boolean CoolSet<Type>::remove (const Type& key) {
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value
  int count = this->items_in_buckets[hash];     // Number of items in bucket
  for (int i = 0; i < count; i++) {             // For each entry in bucket
    if ((*this->compare)(key,this->table[hash].data[i]) == TRUE) {
      this->table[hash].data[i] = this->table[hash].data[count-1];
      this->entry_count--;                      // Decrement table entry count
      this->items_in_buckets[hash]--;           // Decrement bucket item count
      if (this->items_in_buckets[hash]) {       // If any more items in bucket
        this->curpos = SET_BUCKET_NUMBER(hash); // Save bucket number
        this->curpos |= SET_BUCKET_INDEX(this->items_in_buckets[hash]-1);
        this->curpos |= SET_TRAVERSED(FALSE);   // Reset traverse flag
      }
      else
        this->curpos = INVALID;                 // Else invalidate marker
      return TRUE;
    }
  }
  return FALSE;                                 // Return failure flag
}


// resize -- Resize a CoolSet object to hold at least some number items
// Input:    this*, minimum number of items to hold
// Output:   TRUE/FALSE

template <class Type> 
Boolean CoolSet<Type>::resize (long n) {
#if ERROR_CHECKING
  if (n < 0) {                                  // If invalid size specified
    //RAISE (Error, SYM(CoolSet), SYM(Negative_Size)),
    printf ("CoolSet<%s>::resize(): Negative resize %d.\n", #Type, n);
    abort ();
  }
#endif
  Bucket* t2;                           // Temporary variable
  long old_prime = hash_primes[this->current_bucket]; // Get prime number 
  while (hash_primes[this->current_bucket]*BUCKET_SIZE < n) // Find prime big
    this->current_bucket++;                     // ... enough for number items
  if (this->growth_ratio != 0.0) {              // If a growth ratio is set
    long new_size = long((old_prime*BUCKET_SIZE) * (1.0 + this->growth_ratio));
    if (new_size > n)
      while (hash_primes[this->current_bucket]*BUCKET_SIZE < new_size)
        this->current_bucket++;                 // Enough size for growth ratio
  }
 retry:
  long new_prime = hash_primes[this->current_bucket];// Get prime number 
  unsigned char* t1 = new unsigned char[new_prime];  // Counts items in buckets
  long i;
  for (i = 0; i < new_prime; i++)               // For each bucket count
    t1[i] = 0;                                       // Initialize to zero
  t2 = new Bucket[new_prime];                // Allocate new buckets
  for (i = 0; i < old_prime; i++) {                  // For each bucket count
    for (int j = 0; j < this->items_in_buckets[i]; j++) { // For each item
      Type key = this->table[i].data[j];        // Get key from table
      unsigned long hash = ((*this->h_function)(key)) % new_prime; // Get hash 
      if (t1[hash] == BUCKET_SIZE) {            // Overflow bucket -- resize
        delete [] t1;                           // Delete allocated storage
        delete [] t2;                           // Delete allocated storage
        this->current_bucket++;                 // Increment bucket count
        goto retry;                             // Go retry again
      }
      t2[hash].data[t1[hash]] = key;            // Copy key into new table
      t1[hash]++;                               // Increment item count
    }
  }
  delete [] this->items_in_buckets;             // Free up old storage
  delete [] this->table;                        // Free up old storage
  this->items_in_buckets = t1;                  // Point to new item count
  this->table = t2;                             // Point to new buckets
  this->curpos = INVALID;                       // Invalidate current position
  return TRUE;                                  // Return success
}

// operator|= -- Determine the union of two sets, that is all elements in each
//               set and modify the source with the result
// Input:        Reference to a set
// Output:       Updated CoolSet object containing union of two sets

template <class Type> 
CoolSet<Type>& CoolSet<Type>::operator|= (const CoolSet<Type>& s) {
  CoolSet<Type>& s2 = (CoolSet<Type>&) s;       // Locally cast away const
  IterState s2_pos = s2.curpos;         // Save curpos of s
  for (s2.reset (); s2.next (); )               // For each entry in 2nd set
    if (this->find (s2.value()) == FALSE)       // If not in 1st set
      this->put (s2.value ());                  // Put to result set
  s2.curpos = s2_pos;                           // Put back the original curpos
  this->curpos = INVALID;                       // Invalidate current position
  return *this;                                 // Return reference
}


// operator-= -- Determine the difference of two sets, that is all elements in
//               the first set that are not in the second and modify the source
//               with the result
// Input:        Reference to a set
// Output:       Updated CoolSet object containing difference of two sets

template <class Type> 
CoolSet<Type>& CoolSet<Type>::operator-= (const CoolSet<Type>& s) {
  CoolSet<Type>& s2 = (CoolSet<Type>&) s;       // Locally cast away const
  IterState s2_pos = s2.curpos;         // Save curpos of s
  for (s2.reset (); s2.next (); )               // For each entry in 2nd set
    if (this->find (s2.value()) == TRUE)        // If in 1st set
      this->remove ();                          // Remove from result set
  s2.curpos = s2_pos;                           // Put back the original curpos
  this->curpos = INVALID;                       // Invalidate current position
  return *this;                                 // Return reference
}

// operator^= -- Determine the exclusive-OR of two sets, that is all elements 
//               in the first set that are not in the second and all elements 
//               in the second set that are not in the first and modify the
//               source with the result
// Input:        Reference to set
// Output:       Updated CoolSet object containing XOR of two sets

template <class Type> 
CoolSet<Type>& CoolSet<Type>::operator^= (const CoolSet<Type>& s) {
  CoolSet<Type>& s2 = (CoolSet<Type>&) s;       // Locally cast away const
  IterState s2_pos = s2.curpos;         // Save curpos of s
  for (s2.reset (); s2.next (); )               // For each entry in 2nd set
    if (this->find (s2.value()) == TRUE)        // If in 1st set
      this->remove ();                          // Remove from result set
    else
      this->put (s2.value());                   // Else, put into result set
  s2.curpos = s2_pos;                           // Put back the original curpos
  this->curpos = INVALID;                       // Invalidate current position
  return *this;                                 // Return reference
}


// operator&= -- Determine the intersection of two sets, that is all elements
//               that are in both sets and modify the source with the result
// Input:        Reference to Set object
// Output:       Updated CoolSet object containing intersection of two sets

template <class Type> 
CoolSet<Type>& CoolSet<Type>::operator&= (const CoolSet<Type>& s) {
  CoolSet<Type>& s2 = (CoolSet<Type>&) s;       // Locally cast away const
  IterState s2_pos = s2.curpos;         // Save curpos of s
  CoolSet<Type> temp(*this);                    // Iterator interacts with remove
  for (temp.reset(); temp.next(); )             // For each entry in 1st set
    if (s2.find (temp.value()) == FALSE)        // If not in 2nd set
      this->remove(temp.value());               // Remove from result set
  s2.curpos = s2_pos;                           // Put back the original curpos
  this->curpos = INVALID;                       // Invalidate current position
  return *this;                                 // Return reference
}

// Operator= -- Assignment of one CoolSet to another duplicating size and
//              contents and returning old storage
// Input:       Reference to CoolSet object
// Output:      Reference to new CoolSet object

template <class Type>
CoolSet<Type>& CoolSet<Type>::operator= (const CoolSet<Type>& s) {
  if (this != &s) {
    CoolBase_Hash_Table::operator=(s);
    long prime = hash_primes[this->current_bucket]; // Get prime number
    delete [] this->table;                          // Return old table storage
    this->table = new Bucket[prime];        // Allocate bucket storage
    for (long i = 0; i < prime; i++)                // For each bucket count
      for (int j = 0; j < s.items_in_buckets[i]; j++) // For each item in bucket
        this->table[i].data[j] = s.table[i].data[j];  // Copy key 
    this->compare = s.compare;                        // Use same compare func
  }
  return *this;                         // Return reference
}


// operator<< -- Overload the output operator to provide a crude print
//               capability for CoolSet objects
// Input:        ostream reference, CoolSet reference
// Output:       None

template <class Type>
ostream& operator<< (ostream& os, const CoolSet<Type>& s) {
  os << "[ ";                                   // Output opening bracket
  for (int i = 0; i < s.get_bucket_count(); i++) // For each bucket
    for (int j = 0; j < s.get_count_in_bucket(i); j++) // For each key/pair
      os << s.table[i].data[j] << " ";           // Output the key
  os << "]\n";                                   // Output bracket
  return os;                                     // Return stream
}


// set_key_compare -- Set the compare function for this instance
// Input:             Pointer to compare function
// Output:            None

template <class Type> 
void CoolSet<Type>::set_compare (register Boolean (*cf) (const Type&, const Type&)) {
  if (cf == NULL)
    this->compare = &CoolSet_are_keys_equal; // Default compare
  else
    this->compare = cf;
}

// included to define their default equality and hash functions
#include <string.h>  // strcmp(const char*, const char*)
#include <cool/String.h>
#include <cool/Gen_String.h>

Boolean CoolSet_are_keys_equal (const CoolGen_String& v1, const CoolGen_String& v2) {
   return !strcmp (v1, v2);
}
long CoolSet_default_hash (const CoolGen_String& key) {
    return sxhash(key);
}

Boolean CoolSet_are_keys_equal (const CoolString& v1, const CoolString& v2) {
   return !strcmp (v1, v2);
}
long CoolSet_default_hash (const CoolString& key) {
    return sxhash(key);
}

Boolean CoolSet_are_keys_equal (char* const& v1, char* const& v2) {
   return !strcmp (v1, v2);
}
long CoolSet_default_hash (char* const& key) {
    return sxhash(key);
}

// are_keys_equal -- Compares two keys using the user supplied comparison
//                   function or the built-in operator== otherwise
// Input:            References to two keys
// Output:           TRUE/FALSE

template<class Type>
Boolean CoolSet_are_keys_equal (const Type& k1, const Type& k2) {
  return (k1 == k2);
}


// default_hash -- Implements the hash mechanism 
// Input:          Reference to a key
// Output:         Hash value (0-relative index into based table)

template<class Type>
long CoolSet_default_hash (const Type& key) {
  if (sizeof (key) <= 4)
    return (((long)(key)) >> 3);
  else {
    int hash_value = 0;
    char* temp = (char*) &key;
    for (int i = 0; i < sizeof (key); i++) {
      hash_value = hash_value ^ temp[i];
      if (hash_value < 0)
        hash_value = -hash_value;
    }
    return (hash_value >> 3);
  }
}


