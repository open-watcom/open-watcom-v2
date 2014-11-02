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
// Created: MBN 06/06/89 -- Initial implementation
// Updated: LGO 09/19/89 -- Split into one file per method
// Updated: VDN 02/21/92 -- New lite version
// 
//
// This file contains member and  friend function implementation code  for the
// Base Hash   Table class  defined  in the  Base_Hash.h  header file.   Where
// appropriate and  possible,  interfaces   to, and   us of, existing   system
// functions has been incorporated.  An overview of  the structure of the Base
// Hash Table class, along with a synopsis of each member and friend function,
// can be found in the Base_Hash_Table.h header file.

#include <cool/Base_Hash.h>

#include <string.h>                             // for strcmp()

long hash_primes[] = {3, 7, 13, 19, 29, 41, 53, 67, 83, 97, 113, 137,
                      163, 191, 223, 263, 307, 349, 401, 461, 521,
                      653, 719, 773, 839, 911, 983, 1049, 1123, 1201,
                      1279, 1367, 1459, 1549, 1657, 1759, 1861, 1973,
                      2081, 2179, 2281, 2383, 2503, 2617, 2729, 2843,
                      2963, 3089, 3203, 3323, 3449, 3571, 3697, 3833,
                      3967, 4099, 4241, 4391, 4549, 4703, 4861, 5011,
                      5171, 5333, 5483, 5669, 5839, 6029, 6197, 6361,
                      6547, 6761, 6961, 7177, 7393, 7517, 7727, 7951,
                      8101, 8209, 16411, 32771, 65537, 131301, 262147,
                      524287};

// CoolBase_Hash_Table -- Simple constructor with no arguments that creates a hash
//               table object with the minimal prime number of buckets and
//               uses the default hash function.
// Input:        None
// Output:       None

CoolBase_Hash_Table::CoolBase_Hash_Table () {
  this->growth_ratio = 0.0;                     // Grow to next prime number
  this->entry_count = 0;                        // No entries in table
  this->current_bucket = 0;                     // Index to number of buckets
  this->curpos = INVALID;                       // Invalidate current position
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->items_in_buckets = new unsigned char[prime]; // Counts items in buckets
  for (long i = 0; i < prime; i++)              // For each bucket count
    this->items_in_buckets[i] = 0;              // Initialize to zero
}

// CoolBase_Hash_Table -- Simple constructor with one argument that creates a hash
//               table object with the minimal prime number of buckets that
//               holds some user-supplied number of items and uses the default
//               hash function.
// Input:        Minimal number of items table must hold
// Output:       None

CoolBase_Hash_Table::CoolBase_Hash_Table (long n) {
  this->growth_ratio = 0.0;                     // Grow to next prime number
  this->entry_count = 0;                        // No entries in table
  this->current_bucket = 0;                     // Index to number of buckets
  this->curpos = INVALID;                       // Invalidate current position
  while (hash_primes[this->current_bucket]*BUCKET_SIZE < n) // Find prime big
    this->current_bucket++;                     // ... enough for number items
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->items_in_buckets = new unsigned char[prime]; // Counts items in buckets
  for (long i = 0; i < prime; i++)               // For each bucket count
    this->items_in_buckets[i] = 0;               // Initialize to zero
}


// CoolBase_Hash_Table -- constructor that takes a reference to an existing hash table
//               and duplicates both its size and contents
// Input:        Reference to hash table object
// Output:       None

CoolBase_Hash_Table::CoolBase_Hash_Table (const CoolBase_Hash_Table& h) {
  this->growth_ratio = h.growth_ratio;          // Grow to next prime number
  this->entry_count = h.entry_count;            // No entries in table
  this->current_bucket = h.current_bucket;      // Index to number of buckets
  this->curpos = INVALID;                       // Invalidate current position
  long prime = hash_primes[this->current_bucket]; // Get prime number 
  this->items_in_buckets = new unsigned char[prime]; // Counts items in buckets
  for (long i = 0; i < prime; i++)              // For each bucket count
    this->items_in_buckets[i] = h.items_in_buckets[i]; // Copy item count
}


// ~CoolBase_Hash_Table -- Destructor for the CoolBase_Hash_Table class
// Input:         this*
// Output:        None

CoolBase_Hash_Table::~CoolBase_Hash_Table () {
  delete [] this->items_in_buckets;             // Free bucket count storage
}


// Operator= -- Assignment of one hash table to another duplicating size and
//              contents and returning old storage
// Input:       Reference to hash table object
// Output:      Reference to new hash table object

CoolBase_Hash_Table& CoolBase_Hash_Table::operator= (const CoolBase_Hash_Table& h) {
  this->growth_ratio = h.growth_ratio;          // Grow to next prime number
  this->entry_count = h.entry_count;            // No entries in table
  this->current_bucket = h.current_bucket;      // Index to number of buckets
  long prime = hash_primes[this->current_bucket]; // Get prime number
  delete [] this->items_in_buckets;             // Return old count storage
  this->items_in_buckets = new unsigned char[prime]; // Counts items in buckets
  for (long i = 0; i < prime; i++)              // For each bucket count
    this->items_in_buckets[i] = h.items_in_buckets[i]; // Copy item count
  this->curpos = INVALID;                              // Invalidate current position
  return *this;                                 // Return reference
}

// next -- Increment current position. If INVALID, set to first
// Input:  this*
// Output: TRUE/FALSE

Boolean CoolBase_Hash_Table::next () {
  long prime = this->get_bucket_count ();       // Prime number of buckets
  if (this->curpos == INVALID) {                // If INVALID current position
    if (this->entry_count == 0)                 // If no entries in table
      return FALSE;                             // Return failure
    long i;
    for (i = 0; i < this->get_bucket_count(); i++) // For each bucket
      if (this->get_count_in_bucket(i) != 0)    // If the bucket has an item
        break;
    this->curpos = SET_BUCKET_NUMBER(i);        // Set bucket number
    this->curpos |= SET_BUCKET_INDEX(0);        // Set index into bucket
    return TRUE;                                // Return success
  }
  else if (TRAVERSED(this->curpos))             // If already traversed set
    return FALSE;                               // Inidicate failure
  else {
    long hash = BUCKET_NUMBER(this->curpos);    // Get bucket number
    long index = BUCKET_INDEX(this->curpos);    // Get index in bucket
    if (++index < this->get_count_in_bucket(hash)){// If more items in bucket
      this->curpos = SET_BUCKET_NUMBER(hash);      // Update bucket hash bits
      this->curpos |= SET_BUCKET_INDEX(index);  // Update bucket index bits
      return TRUE;                              // And return success
    }
    long i;
    for (i = hash+1; i < prime; i++)            // For remaining buckets
      if (this->get_count_in_bucket(i) != 0)    // If the bucket has item
        break;
    if (i == prime) {                           // If no more items
      this->curpos = INVALID;                   // Invalidate pointer
      return FALSE;                             // Return failure
    }
    this->curpos = SET_BUCKET_NUMBER(i);        // Set bucket number
    this->curpos |= SET_BUCKET_INDEX(0);        // Set index into bucket
    return TRUE;                                // Return success
  }
}
    




// prev -- Decrement current position. If INVALID, set to last
// Input:  this*
// Output: TRUE/FALSE

Boolean CoolBase_Hash_Table::prev () {
  long prime = this->get_bucket_count ();       // Prime number of buckets
  if (this->curpos == INVALID) {                // If INVALID current position
    if (this->entry_count == 0)                 // If no entries in table
      return FALSE;                             // Return failure
    long i;
    for (i = prime-1; i >= 0; i--)              // For each bucket, search
      if (this->get_count_in_bucket (i) != 0)   // If the bucket has an item
        break;
    this->curpos = SET_BUCKET_NUMBER (i);       // Set bucket number
    this->curpos |= SET_BUCKET_INDEX ((this->get_count_in_bucket (i)-1));
    return TRUE;                                // Return success
  }
  else if (TRAVERSED(this->curpos))             // If already traversed set
    return FALSE;                               // Inidicate failure
  else {
    long hash = BUCKET_NUMBER(this->curpos);    // Get bucket number
    long index = BUCKET_INDEX(this->curpos);    // Get index in bucket
    if (index > 0) {                            // If more items in bucket
      this->curpos = SET_BUCKET_NUMBER (hash);  // Update bucket hash bits
      this->curpos |= SET_BUCKET_INDEX ((index-1)); // Update bucket index bits
      return TRUE;                              // And return success
    }
    long i;
    for (i = hash-1; i >= 0; i--)               // For remaining buckets
      if (this->items_in_buckets[i] != 0)       // If the bucket has an item
        break;
    if (i < 0) {                                // If no more items
      this->curpos = INVALID;                   // Invalidate pointer
      return FALSE;                             // Return failure
    }
    this->curpos = SET_BUCKET_NUMBER(i);        // Set bucket number
    this->curpos |= SET_BUCKET_INDEX((this->get_count_in_bucket (i)-1));
    return TRUE;                                // Return success
  }
}

// statistics -- Make a pass through the Hash Table and print out statistics
//               for the number of entries in each bucket.
// Input:   this*
// Output   None

void CoolBase_Hash_Table::statistics () {
  long arry[BUCKET_SIZE+1];
  char str[30];
  float pct_full;
  int i;
  for (i = 0; i <= BUCKET_SIZE; i++)
    arry[i]=0;
  long buckets = get_bucket_count();
  long total_slots = buckets * BUCKET_SIZE;

  for (i = 0; i < buckets; i++)                 // For each bucket
    arry[this->items_in_buckets[i] ]++;         //  Bump array per slots used

  // Print out general info
  pct_full = (float)(entry_count*100.0) / (float)total_slots;
  sprintf(str,"%.1f",pct_full);                 
  cout << "\n" << buckets << " Buckets, " << BUCKET_SIZE << " slots each = "
     << total_slots << " slots available.  " << entry_count << " slots used ("
     << str << "%)\n";

  if(entry_count > 0) {                        // Print out slot usage
    cout << "  Distribution of buckets with n slots filled:\n";
    for (i = 0; i <= BUCKET_SIZE; i++) {
      pct_full = (float)(arry[i]*100) / (float)buckets;
      sprintf(str,"%.1f",pct_full);
      cout << i << "=" << str << "% ";
    }
    cout << "\n\n";
  }
}

// clear -- Empty the hash table, but don't change allocated space
// Input:   this*
// Output   None

void CoolBase_Hash_Table::clear () {
  for (int i = 0; i < hash_primes[this->current_bucket]; i++) // For each bucket
    this->items_in_buckets[i] = 0;                            // Zero count
  this->entry_count = 0;                                      // Entries to 0
  this->curpos = INVALID;                                     // Invalid curpos
}

// sxhash -- Hash function for char*
// Input:       Character string
// Output:      unsigned long hash value

unsigned long sxhash(const char* string) {
  register unsigned long hash = *string++;
  if(*string != END_OF_STRING) {
    hash = (hash << 7) ^ *string++;
    if (*string != END_OF_STRING) {
      hash = (hash << 7) ^ *string++;
      if (*string != END_OF_STRING) {
        hash = (hash << 7) ^ *string++;
        while (*string != END_OF_STRING) {// rotate hash left 7 bits & xor char
        #ifdef DOS
          hash = _lrotl(hash, 7) ^ *string++;
        #else
          long rest = hash >> 25;
          hash = ((hash << 7) | rest) ^ *string++;
        #endif
        }
      }
    }
  }
  hash &= 0x7fffffffL;                          // Make sure bit 32 is zero
  return hash;
}

Boolean charP_compare (char* const& s1, char* const& s2) {
  return !strcmp (s1, s2);
}


// ratio_error -- Raise exception for CoolBase_Hash_Table::set_growth_ratio()
// Input:         Bad ratio specification
// Output:        None

void CoolBase_Hash_Table::ratio_error (float r) {
  //RAISE (Error, SYM(CoolBase_Hash_Table), SYM(Negative_Ratio),
  printf ("CoolBase_Hash_Table::set_growth_ratio(): Negative growth ratio %f.\n", r);
  abort ();
}


// resize_error -- Raise exception for CoolBase_Hash_Table::resize()
// Input:          Bad size specification
// Output:         None

void CoolBase_Hash_Table::resize_error (const char* T1, const char* T2, long s) {
  //RAISE (Error, SYM(CoolBase_Hash_Table), SYM(Negative_Size),
  printf ("CoolBase_Hash_Table<%s,%s>::resize(): Negative resize %d.\n", T1, T2, s);
  abort ();
}


// value_error -- Raise exception for CoolBase_Hash_Table::value()
// Input:        None
// Output:       None
     
void CoolBase_Hash_Table::value_error (const char* T1, const char* T2) {
  //RAISE (Error, SYM(CoolBase_Hash_Table), SYM(Invalid_Cpos),
  printf ("CoolBase_Hash_Table<%s,%s>::value(): Invalid current position.\n", T1, T2);
  abort ();
}


// key_error -- Raise exception for CoolBase_Hash_Table::key()
// Input:       None
// Output:      None
     
void CoolBase_Hash_Table::key_error (const char* T1, const char* T2) {
  //RAISE (Error, SYM(CoolBase_Hash_Table), SYM(Invalid_Cpos),
  printf ("CoolBase_Hash_Table<%s,%s>::key(): Invalid current position.\n", T1, T2);
  abort ();
}


// remove_error -- Raise exception for CoolBase_Hash_Table::remove()
// Input:          Bad lookup value
// Output:         None

void CoolBase_Hash_Table::remove_error (const char* T1, const char* T2) {
  //RAISE (Error, SYM(CoolBase_Hash_Table), SYM(Invalid_Cpos),
  printf ("CoolBase_Hash_Table<%s,%s>::remove(): Invalid current position.\n", T1, T2);
  abort ();
}
