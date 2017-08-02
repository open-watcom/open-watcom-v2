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
// Created: MBN 09/29/89 -- Initial design and implementation
// Updated: MBN 01/03/90 -- Correctly generate numbers within specified range
//
// The Random  class is  publicly derived from  Generic and implements  several
// varieties  of  general-purpose  random number  generators   as described  in
// Chapter 7 of "Numerical Recipes in C".  The  ANSI C draft standard specifies
// the rand() function that allows  an application to obtain successive  random
// numbers in  a sequence by repeated  calls.  However,  system-supplied random
// number generators in the form of the  rand() function  are generally of poor
// quality, particularly  with  regards to their  randomness of distribution of
// random numbers. Specifically,  system  random number generators   are almost
// always  linear congruential genarators whose period  is not very  large. The
// ANSI C  draft  specification only requires a  modulus of 32767, which can be
// disasterous for such uses as a Monte Carlo integration over 10^6 points.
//
// The Random class allows an application to select one of five types of random
// number generators based upon the usage requirements. Each generator function
// has different characteristics. The first two use the system rand() function,
// while the last  three  are  self-contained,  portable  implementations.  The
// first, SIMPLE, uses the system-supplied rand()  function where speed  is the
// predominant concern.  Although  sequential correlation of  successive random
// values is still a high probability, this function at  least corrects for the
// weakness of many system  random  generator functions where the value's least
// significant bits are  very often much less random  than the most significant
// bits. The second, SHUFFLE,   uses  the    rand() function  and   a shuffling
// procedure where random numbers are cached in  a buffer and selected randomly
// to break up  sequential correlation  in the system-supplied   function.  The
// third, ONE_CONGRUENTIAL, uses one linear  congruential generator  instead of
// the rand() function to   implement an  efficient,  portable  random   number
// generator that guarantees   there is no  sequential  correlation between the
// random values returned.   The fourth, THREE_CONGRUENTIAL, uses three  linear
// congruential  generators  to implement  a  portable random number  generator
// whose period is  essentially infinite  and has no   sequential correlations.
// Finally, SUBTRACTIVE  is   an implementation of   a  portable  random number
// generator  as suggested  by  Knuth in Volume   two of "The  Art of  Computer
// Programming" that does not use linear congruential generators, but rather an
// original subtractive method.
//
// The Random class contains  several  private data slots used  by  the various
// random number generator functions.  In addition,  each instance contains the
// lower and upper bounds of the range of random numbers desired and  a pointer
// a   specific generator  function.  One  public  constructors  is provided to
// facilitate the  creation  of floating point random number generator objects.
// Methods  are available  to  get and  set the    seed,  select the  generator
// function, and get the next integer or floating point random number.


#ifndef RANDOMH                                 // If no definition for Random
#define RANDOMH                                 // define the random symbol

const int RND_BUF_SIZE = 128;                   // Anti-correlation buffer size

class CoolRandom;                               // Forward reference class
typedef double (CoolRandom::*RNG)();            // CoolRandom number function type
enum RNG_TYPE {SIMPLE,SHUFFLE,ONE_CONGRUENTIAL,THREE_CONGRUENTIAL,SUBTRACTIVE};

#define MOD 714025                              // Modulus for shuffle 
#define MULT 1366                               // Multiplier for shuffle
#define INCR 150889                             // Increment for shuffle

#define MOD1 259200                             // Modulus 1 for 3 cong gen  
#define MULT1 7141                              // Multiplier 1 for 3 cong gen
#define INCR1 54773                             // Increment 1 for 3 cong gen 
#define RMOD1 (1.0/MOD1)                        // Factor for 3 cong gen   
#define MOD2 134456                             // Modulus 2 for 3 cong gen  
#define MULT2 8121                              // Multiplier 2 for 3 cong gen
#define INCR2 28411                             // Increment 2 for 3 cong gen 
#define RMOD2 (1.0/MOD2)                        // Factor 2 for 3 cong gen   
#define MOD3 243000                             // Modulus 3 for 3 cong gen  
#define MULT3 4561                              // Multiplier 3 for 3 cong gen
#define INCR3 51349                             // Increment 3 for 3 cong gen 

#define MBIG 1000000000                         // Big constant for Knuth
#define MSEED 161803398                         // Seed constant for Knuth
#define FAC (1.0/MBIG)                          // Factor for Knuth
#define KNUTH_SPECIAL 56                        // Table size for Knuth

class CoolRandom {
public:
  CoolRandom (RNG_TYPE, int seed=1, float lower=0.0, float upper=100.0);
  ~CoolRandom ();                                       // Destructor

  inline int get_seed () const;                 // Get current seed value
  inline void set_seed (int);                   // Set seed and initialize
  inline void set_rng (RNG_TYPE);               // Set generator function
  inline double next ();                        // Return next random number

private:
  float l,u;                                    // Lower/upper inclusive bounds
  RNG generator;                                // Pointer to generator funn
  int s;                                        // Shared seed value
  long ix1, ix2, ix3;                           // 3 generator sequence numbers
  double maxran;                                // Maximum random number
  double prev_rnum;                             // Retains previous random num
  long k_buffer[KNUTH_SPECIAL];                 // Buffer for Knuth's algorithm
  double buffer[RND_BUF_SIZE];                  // Promote random distribution
  
  double simple ();                             // System-based RNG 
  double shuffle ();                            // Shuffle RNG
  double one_congruential ();                   // 1 linear congruential RNG
  double three_congruential ();                 // 3 linear congruential RNG
  double subtractive ();                        // Knuth subtractive RNG
  void init (RNG_TYPE);                         // Initialize random generator
};


// get_seed -- Return the current seed for the RNG
// Input:      None
// Output:     Current seed value

inline int CoolRandom::get_seed () const {
  return this->s;
}


// set_seed -- Set the seed for the RNG and force a recalculation of the
//             randomized cache to preclude correlation in the results
// Input:      New integer seed value
// Output:     None

inline void CoolRandom::set_seed (int seed) {
  this->s = seed;                               // Set new seed value
}


// set_rng -- Set the type of random number generator function 
// Input:     RNG type
// Output:    None

inline void CoolRandom::set_rng (RNG_TYPE rng) {
  this->init (rng);                             // Make anti-correlation buffer
}


// next -- Generate another random number
// Input:  None
// Output: Random number

inline double CoolRandom::next () {
  return (this->*generator)();
}

#endif

