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
// Created: MBN 10/03/89 -- Initial design and implementation
// Updated: MBN 01/03/90 -- Correctly generate numbers within specified range
// Updated: DLS 03/27/91 -- New lite version
//
// This file contains  member and friend function implementation  code  for the
// CoolRandom  class defined in  the Random.h  header  file.  Where appropriate and
// possible,  interfaces to, and  us  of,  existing   system functions has been
// incorporated. An overview of the structure of the CoolRandom class, along with a
// synopsis of each  member and friend  function, can be  found in the Random.h
// header file.
//

#if defined(DOS)
extern "C" {
#include <stdlib.h>                             // for srand() and rand()
}
#else
#include <stdlib.h>                             // for srand() and rand()
#endif

#ifndef RANDOMH                                 // If no definition for CoolRandom
#include <cool/Random.h>                        // Include the class header
#endif


// ~CoolRandom -- Destructor for the CoolRandom class (not inline because it's virtual)
// Input:     None
// Output:    None

CoolRandom::~CoolRandom () {}


// CoolRandom -- Constructor for the CoolRandom class with float arguments
// Input:    RNG type, optional seed, lower, and upper limits
// Output:   None

CoolRandom::CoolRandom (RNG_TYPE rng, int seed, float lower, float upper){
  this->s = seed;                               // Set seed value
  this->l = lower;                              // Set lower bound of range
  this->u = upper;                              // Set upper bound of range
  this->init (rng);                             // Make anti-correlation buffer
}


// init -- Build the cache of random numbers using the RNG function selected
//         to preclude any correlation of the sequence of numbers generated
// Input:  None
// Output: None

void CoolRandom::init (RNG_TYPE rng) {
  unsigned int i;                               // Temporary
  unsigned int j;                               // Temporary
  long mj,mk;                                   // Temporaries for Knuth
  this->maxran = RAND_MAX;                      // Set maximum number
  switch (rng) {                                // Select generator function
  case SIMPLE:
    this->generator = &CoolRandom::simple;              // Set pointer to function
    srand (this->s);                            // Set seed value
    break;

  case SHUFFLE:                                 // Exercise
    this->generator = &CoolRandom::simple;              // Set pointer to function
    srand (this->s);                            // Set seed value
    for (i = 0; i < RND_BUF_SIZE; i++)          // Exercise system routine for
      rand ();                                  // Small multipliers
    for (i = 0; i < RND_BUF_SIZE; i++)          // Exercise system routine for
      this->buffer[i] = rand ();                // Fill random cache
    this->prev_rnum = rand ();                  // Get first value for later
    break;

  case ONE_CONGRUENTIAL:
    this->generator = &CoolRandom::one_congruential; // Set pointer to function
    this->ix1 = (INCR-this->s) % MOD;           // Set random seed
    for (i = 1; i < RND_BUF_SIZE; i++) {        // Initialize shuffle table
      this->ix1 = (MULT*this->ix1+INCR)%MOD;    // Calculate random value
      this->buffer[i] = this->ix1;              // And store in random cache
    }
    this->ix1 = (MULT*this->ix1+INCR)%MOD;      // Calculate first value
    this->ix2 = this->ix1;                      // Get first value for later
    break;

  case THREE_CONGRUENTIAL:
    this->generator = &CoolRandom::three_congruential; // Set pointer to function
    ix1 = (MULT1*((INCR1-this->s)%MOD1)+INCR1)%MOD1; // Seed first generator
    ix2 = ix1 % MOD2;                           // Use to seed second gen
    ix1 = (MULT1*ix1+INCR1)%MOD1;               // Calculate again
    ix3 = ix1 % MOD3;                           // And use to seed third
    for (i = 1; i < RND_BUF_SIZE; i++) {        // For each cache entry
      ix1 = (MULT1*ix1+INCR1) % MOD1;           // Calculate 1st deviate
      ix2 = (MULT2*ix2+INCR2) % MOD2;           // Calculate 2nd deviate
      this->buffer[i] = (ix1+ix2*RMOD2)*RMOD1;  // Fill random cache
    }
    this->buffer[0] = 0.5; // Avoid uninited data to work around buggy algorithm
    break;

  case SUBTRACTIVE:
    this->generator = &CoolRandom::subtractive; // Set pointer to function
    mj = (MSEED - this->s) % MBIG;              // Calculate first random num
    this->k_buffer[KNUTH_SPECIAL-1] = mj;       // and copy to knuth buffer[55]
    mk = 1;
    for (i = 1; i <= 54; i++) {                 // For remaining elements
      j = (21 * i) % (KNUTH_SPECIAL - 1);       // Calculate random order
      this->k_buffer[j] = mk;                   // and use non-random number
      mk = mj - mk;
      if (mk < 0)                               // If negative number
        mk += MBIG;                             // Add big number and restart
      mj = this->k_buffer[j];                   // And calculate new seed
    }
    for (int k = 1; k <= 4; k++)                // Randomize the table by
      for (i = 1; i < KNUTH_SPECIAL; i++) {     // warming up the generator
        this->k_buffer[i] -= this->k_buffer[1+(i+30) % (KNUTH_SPECIAL-1)];
        if (this->k_buffer[i] < 0)              // If negative table entry
          this->k_buffer[i] += MBIG;            // add a very big number
      }
    ix1 = 0;                                    // Initialize loop variable
    ix2 = 31;                                   // 32 is special; see Knuth
    break;
  }
}


// simple -- Encapsulated system-supplied random number generator used when
//           speed is the predominant concern. Although sequential correlation
//           of successive random values is still a high probability, this
//           function called from one of the next methods at least corrects for
//           the weakness of many system random generator functions where the
//           value's least significant bits are very often much less random
//           than the most significant bit
// Input:    None
// Output:   Random number

double CoolRandom::simple () {
  return ((((double)(this->l))+((((double)(rand ()))*(this->u-this->l)))/
           (this->maxran+1.0)));
}


// shuffle -- A shuffle-algorithm random number generator as described in
//            in chapter 7 of "Numerical  Recipes in C". Shuffle uses  the
//            system-supplied rand() function and a shuffling procedure where
//            random numbers are cached in a buffer and selected randomly to
//            break up sequential correlation in the system-supplied function.
// Input:     None
// Output:    Random number

double CoolRandom::shuffle () {
  int index = int(1+double(RND_BUF_SIZE)*this->prev_rnum/this->maxran);
  this->prev_rnum = this->buffer[index];        // Get random number from table
  this->buffer[index] = rand ();                // New random table entry 
  return((((double)(this->l))+(((this->prev_rnum/this->maxran)*(this->u-this->l)))/
          (this->maxran+1.0)));
}


// one_congruential -- Uses one linear congruential generator instead of the
//                     rand() function to implement an efficient, portable
//                     random number generator that guarantees there is no
//                     sequential correlation between the values returned
// Input:              None
// Output:             Random number

double CoolRandom::one_congruential () {
  int index = int(1+(((double)(RND_BUF_SIZE))*this->ix2)/MOD); // Next index
  index %= RND_BUF_SIZE;
  this->ix2 = long(this->buffer[index]);        // Get random number from table
  this->ix1 = (MULT * this->ix1 + INCR) % MOD;  // New seed
  this->buffer[index] = this->ix1;              // New random tbale entry
  return (((double)(this->l))+((((double)this->ix2 / MOD)*(this->u-this->l))));
}


// three_congruential -- uses three linear congruential generators to implement
//                       a portable random number generator whose period is
//                       essentially infinite and has no sequential correlation
// Input:                None
// Output:               Random number

double CoolRandom::three_congruential () {
  this->ix1 = (MULT1 * this->ix1 + INCR1) % MOD1; // Next number for sequence
  this->ix2 = (MULT2 * this->ix2 + INCR2) % MOD2; // Next number for sequence
  this->ix3 = (MULT3 * this->ix3 + INCR3) % MOD3; // Next number for sequence
  int index = int(1+((((double)(RND_BUF_SIZE))*this->ix3)/MOD3)); // Generate index
  index %= RND_BUF_SIZE;
  this->prev_rnum = this->buffer[index];        // Get random number from table
  this->buffer[index] = (this->ix1+this->ix2*RMOD2)*RMOD1; // New table entry
  return (((double)(this->l))+((this->prev_rnum*(this->u-this->l))));
}


// subtractive -- A portable random number generator as suggested by Knuth in
//                Volume two of "The Art of Computer Programming" that does not
//                use linear congruential generators, but rather an original
//                subtractive method. According to Knuth, any large MBIG and
//                any smaller but still large MSEED can be substituted.
//                However, the value of KNUTH_SPECIAL is, as its name suggests,
//                special!
// Input:         None
// Output:        Random number

double CoolRandom::subtractive () {
  long temp;                                    // Temporary
  if (++this->ix1 == KNUTH_SPECIAL)             // If at end of table
    this->ix1 = 1;                              // Start over
  if (++this->ix2 == KNUTH_SPECIAL)             // If at end of table
    this->ix2 = 1;                              // Start over
  temp = this->k_buffer[this->ix1]-this->k_buffer[this->ix2];
  if (temp < 0)                                 // If negative result
    temp += MBIG;                               // Compensate for range
  this->k_buffer[this->ix1] = temp;             // New table entry
  return (((double)(this->l))+((temp*FAC*(this->u-this->l))));
}
