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

#include <cool/Hash_Table.h>

// compare_keys_s -- Key compare
template <class Tkey, class Tval> 
Boolean (*CoolHash_Table<Tkey,Tval>::compare_keys_s)
   (const Tkey&, const Tkey&) = &CoolHash_Table_keys_equal;
//##CoolHash_Table<Tkey,Tval>::Key_Compare CoolHash_Table<Tkey,Tval>::compare_keys_s = &CoolHash_Table_keys_equal;

// compare_values_s -- Value compare
template <class Tkey, class Tval> 
Boolean (*CoolHash_Table<Tkey,Tval>::compare_values_s) (const Tval&, const Tval&) = &CoolHash_Table_values_equal;
//##CoolHash_Table<Tkey,Tval>::Value_Compare CoolHash_Table<Tkey,Tval>::compare_values_s = &CoolHash_Table_values_equal;

// CoolHash_Table -- Simple constructor with no arguments that creates a hash
//               table object with the minimal prime number of buckets and
//               uses the default hash function.
// Input:        None
// Output:       None

template <class Tkey, class Tval> 
CoolHash_Table<Tkey,Tval>::CoolHash_Table() {
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->table = new CoolHash_Table_Bucket<Tkey,Tval>[prime]; 
  this->h_function = &CoolHash_Table_default_hash;
}


// CoolHash_Table -- Simple constructor with one argument that creates a hash
//               table object with the minimal prime number of buckets that
//               holds some user-supplied number of items and uses the default
//               hash function.
// Input:        Minimal number of items table must hold
// Output:       None

template <class Tkey, class Tval> 
CoolHash_Table<Tkey,Tval>::CoolHash_Table(unsigned long n)
: CoolBase_Hash_Table(n)
{
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->table = new CoolHash_Table_Bucket<Tkey,Tval>[prime]; 
  this->h_function = &CoolHash_Table_default_hash;
}


// CoolHash_Table -- constructor that takes a reference to an existing hash table
//               and duplicates both its size and contents
// Input:        Reference to hash table object
// Output:       None

template <class Tkey, class Tval> 
CoolHash_Table<Tkey,Tval>::CoolHash_Table(const CoolHash_Table<Tkey,Tval>& h)
: CoolBase_Hash_Table(h)
{
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->table = new CoolHash_Table_Bucket<Tkey,Tval>[prime]; 
  for (long i = 0; i < prime; i++) { // For each bucket count
    for (int j = 0; j < h.items_in_buckets[i]; j++) { // For items in bucket
      this->table[i].data[j].key = h.table[i].data[j].key; // Copy key 
      this->table[i].data[j].value = h.table[i].data[j].value; // Copy value
    }
  }
  this->h_function = h.h_function;              // Use the same hash function
}


// ~CoolHash_Table -- Destructor for the CoolHash_Table class
// Input:         this*
// Output:        None

template <class Tkey, class Tval> 
CoolHash_Table<Tkey,Tval>::~CoolHash_Table() {
  delete [] this->table;                        // Free key/value storage
}


// find -- Find key/value in hash table
// Input:  Key searching for
// Output: TRUE/FALSE; current_position updated

template <class Tkey, class Tval> 
Boolean CoolHash_Table<Tkey,Tval>::find (const Tkey& key) {
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value 
  for (int i = 0; i < this->items_in_buckets[hash]; i++) { // For each entry
    if ((*this->compare_keys_s)(key,this->table[hash].data[i].key) == TRUE){
      this->curpos = SET_BUCKET_NUMBER(hash); // Set bucket number
      this->curpos |= SET_BUCKET_INDEX(i);      // Set bucket index
      return TRUE;                              // Return success
    }
  }
  return FALSE;
}


// key --  Return key at current position
// Input:  None
// Output: Reference to key at current position

template <class Tkey, class Tval> 
const Tkey& CoolHash_Table<Tkey,Tval>::key () {
  if (this->curpos != INVALID) { // If valid current position
    unsigned long hash = BUCKET_NUMBER(this->curpos); // Get bucket number
    long index = BUCKET_INDEX(this->curpos);          // Get index in bucket
    return (this->table[hash].data[index].key);       // Return value
  }
  else                          // Else 
    this->key_error ("Tkey","Tval"); // Raise exception
return (this->table[0].data[0].key);
}


// value -- Return value at current position
// Input:   None
// Output:  Reference to value at current position

template <class Tkey, class Tval> 
const Tval& CoolHash_Table<Tkey,Tval>::value () {
  if (this->curpos != INVALID) { // If valid current position
    unsigned long hash = BUCKET_NUMBER(this->curpos); // Get bucket number
    long index = BUCKET_INDEX(this->curpos);          // Get index in bucket
    return (this->table[hash].data[index].value);     // Return value
  }
  else                          // Else 
    this->value_error ("Tkey","Tval"); // Raise exception
return (this->table[0].data[0].value);
}


// put -- Hash key/value pair into table if not already there
// Input: this*, key, value
// Output:TRUE when key is new, else FALSE

template <class T1, class T2> 
Boolean CoolHash_Table<T1,T2>::put (const T1& key, const T2& value) {
 retry:
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value
  int index = this->items_in_buckets[hash];
  this->curpos = SET_BUCKET_NUMBER(hash); // Save bucket number
  for (int i = 0; i < index; i++)         // For each item
    if ((*this->compare_keys_s)(key,this->table[hash].data[i].key) == TRUE) {
      this->table[hash].data[i].value = value; // Already there, update value
      this->curpos |= SET_BUCKET_INDEX(i);       // Update bucket index position
      return FALSE;                              // And return found
    }
  if (index >= BUCKET_SIZE) {   // If bucket is full
    this->resize (hash_primes[this->current_bucket+1]*BUCKET_SIZE); // Grow
    goto retry;
  }
  this->table[hash].data[index].key = key;
  this->table[hash].data[index].value = value;
  this->entry_count++;  // Increment table entry count
  this->curpos |= SET_BUCKET_INDEX(index); // Update bucket index position
  this->items_in_buckets[hash]++;          // Increment bucket item count
  return TRUE;                     // Indicate new
}


// get -- Get a value based on a key from the table
// Input: this*, reference to a key
// Output:Value for key/value pair from table
//        Returns TRUE when entry is found, else false

template <class Tkey, class Tval> 
Boolean CoolHash_Table<Tkey,Tval>::get (const Tkey& key, Tval& value) {
  Boolean result = FALSE;       // Assume we don't find entry
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value
  for (int i = 0; i < this->items_in_buckets[hash]; i++) { // For each entry
    if ((*this->compare_keys_s)(key,this->table[hash].data[i].key) == TRUE) {
      this->curpos = SET_BUCKET_NUMBER(hash); // Save bucket number
      this->curpos|= SET_BUCKET_INDEX(i);             // Save index into bucket
      value = this->table[hash].data[i].value;        // Copy value in table
      result = TRUE;                                  // Inidicate success
      break;                                          // Break out of loop
    }
  }
  return result;                // Return success/failure
}


// get_key --Get a key based on a value from the table
// Input:    this*, reference to a value, reference to place to store key
// Output:   TRUE if found with value in reference argument, else FALSE

template <class Tkey, class Tval> 
Boolean CoolHash_Table<Tkey,Tval>::get_key (const Tval& value, Tkey& key) {
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  for (long i = 0; i < prime; i++)                // For each bucket, search
    for (int j = 0; j < this->items_in_buckets[i]; j++) // For item in bucket
      if ((*this->compare_values_s)(value,this->table[i].data[j].value)==TRUE){
        this->curpos = SET_BUCKET_NUMBER(i); // Set bucket number
        this->curpos|= SET_BUCKET_INDEX(j);          // Set index into bucket
        key = this->table[i].data[j].key;                    // Return key for value
        return TRUE;                                         // Indicate success
      }
  return FALSE;         // Indicate failure
}


// remove -- Remove element at current position from the set
// Input:    this*
// Output:   TRUE/FALSE

template <class Tkey, class Tval> 
Boolean CoolHash_Table<Tkey,Tval>::remove () {
  if (this->curpos != INVALID) { // If valid current position
    unsigned long hash = BUCKET_NUMBER(this->curpos); // Get bucket number
    long index = BUCKET_INDEX(this->curpos);          // Get index in bucket
    int count = this->items_in_buckets[hash];         // Number of items in bucket
    this->table[hash].data[index].key = this->table[hash].data[count-1].key;
    this->table[hash].data[index].value = this->table[hash].data[count-1].value;
    this->entry_count--;        // Decrement table entry count
    this->items_in_buckets[hash]--; // Decrement bucket item count
    if (this->items_in_buckets[hash]) { // If any more items in bucket
      this->curpos = SET_BUCKET_NUMBER(hash); // Save bucket number
      this->curpos |= SET_BUCKET_INDEX(this->items_in_buckets[hash]-1);
    }
    else
      this->curpos = INVALID;   // Else invalidate marker
    return TRUE;                // Return success
  }
  this->remove_error ("Tkey", "Tval"); // Raise exception
  return FALSE;              // Return failure
}


// remove -- Remove a value based on a key from the table
// Input:    this*, reference to a key
// Output:   TRUE/FALSE

template <class Tkey, class Tval> 
Boolean CoolHash_Table<Tkey,Tval>::remove (const Tkey& key) {
  long prime = hash_primes[this->current_bucket]; // Prime number of buckets
  unsigned long hash = ((*this->h_function)(key)) % prime; // Get hash value
  int count = this->items_in_buckets[hash];                // Number of items in bucket
  for (int i = 0; i < count; i++) {                        // For each entry in bucket
    if ((*this->compare_keys_s)(key,this->table[hash].data[i].key) == TRUE) {
      this->table[hash].data[i].key = this->table[hash].data[count-1].key;
      this->table[hash].data[i].value = this->table[hash].data[count-1].value;
      this->entry_count--;      // Decrement table entry count
      this->items_in_buckets[hash]--; // Decrement bucket item count
      if (this->items_in_buckets[hash]) {       // If any more items in bucket
        this->curpos = SET_BUCKET_NUMBER(hash); // Save bucket number
        this->curpos |= SET_BUCKET_INDEX(this->items_in_buckets[hash]-1);
      }
      else
        this->curpos = INVALID; // Else invalidate marker
      return TRUE;
    }
  }
  return FALSE;         // Return failure flag
}


// resize -- Resize a hash table object to hold at least some number items
// Input:    this*, minimum number of items to hold
// Output:   None

template <class Tkey, class Tval> 
void CoolHash_Table<Tkey,Tval>::resize (long n) {
#if ERROR_CHECKING
  if (n < 0)                    // If invalid size
    this->resize_error ("Tkey", "Tval", n); // Raise exception
#endif
  CoolHash_Table_Bucket<Tkey,Tval>* t2; // Temporary variable
  long old_prime = hash_primes[this->current_bucket]; // Get prime number 
  while (hash_primes[this->current_bucket]*BUCKET_SIZE < n) // Find prime big
    this->current_bucket++;                                 // ... enough for number items
  if (this->growth_ratio != 0.0) {                          // If a growth ratio is set
    int new_size = int((old_prime * BUCKET_SIZE) * (1.0 + this->growth_ratio));
    if (new_size > n)
      while (hash_primes[this->current_bucket]*BUCKET_SIZE < new_size)
        this->current_bucket++; // Enough size for growth ratio
  }
 retry:
  long new_prime = hash_primes[this->current_bucket]; // Get prime number 
  unsigned char* t1 = new unsigned char[new_prime];   // Counts items in buckets
  long i;
  for (i = 0; i < new_prime; i++)               // For each bucket count
    t1[i] = 0;                                  // Initialize to zero
  // NOTE: We should use the overloaded operator new to construct only
  //       the new buckets, and use memcpy instead of operator= for copying
  t2 = new CoolHash_Table_Bucket<Tkey,Tval>[new_prime]; 
  for (i = 0; i < old_prime; i++) { // For each bucket count
    CoolHash_Table_Pair<Tkey,Tval>* data = this->table[i].data;
    for (int j = 0; j < this->items_in_buckets[i]; j++) { // For each item
      unsigned long hash = ((*this->h_function)(data[j].key)) % new_prime;
      if (t1[hash] == BUCKET_SIZE) { // Overflow bucket -- resize
        delete [] t1;                        // Delete allocated storage
        delete [] t2;                        // Delete allocated storage
        this->current_bucket++;              // Increment bucket count
        goto retry;                          // Go retry again
      }
      t2[hash].data[t1[hash]].key = data[j].key; // Copy key into new table
      t2[hash].data[t1[hash]].value = data[j].value; //Copy value into new table
      t1[hash]++;                                            // Increment bucket item count
    }
  }
  delete [] this->items_in_buckets;             // Free up old storage
  delete [] this->table;                        // Free up old storage
  this->items_in_buckets = t1;                  // Point to new item count
  this->table = t2;                             // Point to new buckets
  this->curpos = INVALID;                       // Invalidate current position
}

// Operator= -- Assignment of one hash table to another duplicating size and
//              contents and returning old storage
// Input:       Reference to hash table object
// Output:      Reference to new hash table object

template <class Tkey, class Tval> 
CoolHash_Table<Tkey,Tval>& CoolHash_Table<Tkey,Tval>::operator= (const CoolHash_Table<Tkey,Tval>& h) {
  if (this != &h) {
    CoolBase_Hash_Table::operator=(h);
    long prime = hash_primes[this->current_bucket]; // Get prime number
    delete [] this->table;                          // Return old table storage
    this->table = new CoolHash_Table_Bucket<Tkey,Tval>[prime]; 
    for (long i = 0; i < prime; i++) {          // For each bucket count
      for (int j = 0; j < h.items_in_buckets[i]; j++) { // For each item in bucket
        this->table[i].data[j].key = h.table[i].data[j].key; // Copy key 
        this->table[i].data[j].value = h.table[i].data[j].value; // Copy value
      }
    }
    this->compare_keys_s = h.compare_keys_s;    // Use same compare function
    this->compare_values_s = h.compare_values_s; // Use same compare function
  }
  return *this;                         // Return reference
}

// operator== -- Determine if two hash tables are equal. This is accomplished
//               by seeing that for each key/value pair in table1, there is the
//               the same pair somewhere in table2.
// Input:        Reference to hash table
// Output:       TRUE/FALSE

template <class Tkey, class Tval> 
Boolean CoolHash_Table<Tkey,Tval>::operator==(const CoolHash_Table<Tkey,Tval>& h) {
  if (this->length() != h.length()) // If not same number of entries
    return FALSE;                   // Then tables are not equal
  if (this->get_bucket_count() == h.get_bucket_count()) { // If same bucket cnt
    for (long i = 0; i < this->get_bucket_count(); i++) { // for each bucket
      int count = this->get_count_in_bucket(i);
      if (count != h.get_count_in_bucket(i)) //Count eq?
        return FALSE;                        // No, tables !equal
      CoolHash_Table_Pair<Tkey,Tval>* this_bucket = this->table[i].data;
      CoolHash_Table_Pair<Tkey,Tval>* h_bucket = h.table[i].data;
      for (int j = 0; j < count; j++) { // For each item in this
        for (int k = 0; k < count; k++) {       // For each item in h
          if ((*this->compare_keys_s)(this_bucket[j].key, h_bucket[k].key)) {
            if ((*this->compare_values_s)(this_bucket[j].value, // key same,
                                          h_bucket[j].value))                                           // is value?
              goto good;
            return FALSE;               // Not the same, so tables !eql
          }
        }
      good: ;
      }
    }
    return TRUE;                // No difference, so equal
  } else {
    Tval temp;                  // Temporary storage;
    for (long i = 0; i < h.get_bucket_count (); i++) { // For each bucket
      for (int j = 0; j < h.get_count_in_bucket(i); j++) // For each item
        if (this->get (h.table[i].data[j].key, temp) == TRUE) // If key in table
          if ((*this->compare_values_s)(h.table[i].data[j].value, temp))
            continue;           // Key/value same, continue
          else
            return FALSE;       // Value different, return FALSE
        else
          return FALSE;         // Key not in table so different
    }
  }
  return TRUE;          // No difference, so equal
}


// set_key_compare -- Set the compare function for this instance
// Input:             Pointer to compare function
// Output:            None

template <class Tkey, class Tval> 
void CoolHash_Table<Tkey,Tval>::set_key_compare (register Boolean (*c) (const Tkey&, const Tkey&)) {
  if (c == NULL)                // If no method supplied
    this->compare_keys_s = &CoolHash_Table_keys_equal;
  else
    this->compare_keys_s = c;
}


// set_value_compare -- Set the compare function for this instance
// Input:               Pointer to compare function
// Output:              None

template <class Tkey, class Tval> 
void CoolHash_Table<Tkey,Tval>::set_value_compare(register Boolean (*c) (const Tval&, const Tval&)) {
  if (c == NULL)
    this->compare_values_s = &CoolHash_Table_values_equal;
  else
    this->compare_values_s = c;
}

// operator<< -- Overload the output operator to provide a crude print
//               capability for hash table objects
// Input:        ostream reference, hash table reference
// Output:       None

template <class Tkey, class Tval>
ostream& operator<< (ostream& os, const CoolHash_Table<Tkey,Tval>& h) {
  for (long i = 0; i < h.get_bucket_count(); i++) { // For each bucket
    for (int j = 0; j < h.get_count_in_bucket(i); j++) { // For each key/pair
      os << "(" << h.table[i].data[j].key;               // Output the key
      os << "," << h.table[i].data[j].value;             // Output the value
      os << ")\n";                                       // And a newline
    }
  }
  return os;                    // Return refererence to stream
}

// included to define their default equality and hash functions
#include <string.h>  // strcmp(const char*, const char*)
#include <cool/String.h>
#include <cool/Gen_String.h>

// specific
Boolean CoolHash_Table_keys_equal (char* const& v1, char* const& v2)
  { return !strcmp (v1, v2); }
unsigned long CoolHash_Table_default_hash (char* const& key)
  { return sxhash(key); }

unsigned long CoolHash_Table_default_hash (const CoolString& key)
  { return sxhash(key); }

unsigned long CoolHash_Table_default_hash (const CoolGen_String& key)
  { return sxhash(key); }


// are_keys_equal -- Compares two keys using the user supplied comparison
//                   function or the built-in operator== otherwise
// Input:            References to two keys
// Output:           TRUE/FALSE

template <class Tkey>
Boolean CoolHash_Table_keys_equal (const Tkey& k1, const Tkey& k2) {
  return (k1 == k2);
}

// default_hash -- Implements the hash mechanism 
// Input:          Reference to a key
// Output:         Hash value (0-relative index into based table)

template <class Tkey>
unsigned long CoolHash_Table_default_hash (const Tkey& key) {
  if (sizeof (Tkey) <= 4)
    return (((unsigned long) key) >> 2);
  else {
    int nlongs = sizeof(Tkey)/sizeof(long);
    register const unsigned long* objp = (const unsigned long*) &key;
    register unsigned long hash = *objp++;
    while (--nlongs > 0) hash ^= *objp++;
    return hash &= 0x7fffffffL;                 // Make sure bit 32 is zero
  }
}

// are_values_equal -- Compares values using the default operator == function
// Input:              References to two values
// Output:             TRUE/FALSE

template <class Tval>
Boolean CoolHash_Table_values_equal (const Tval& v1, const Tval& v2) {
  return (v1 == v2);
}

