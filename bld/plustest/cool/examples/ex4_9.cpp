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
// Updated: JAM 08/21/92 -- made j long and output it (so loop not optimized)

#include <iostream.h>
#include <math.h>

#include <cool/Timer.h>                         // Includer COOL timer class

int main (void) {
  CoolTimer t1;                                 // Create a CoolTimer object
  volatile long sum = 0;     // use in loop to discourage optimizations
  long check = ( 999999UL * 1000000UL ) / 2;
  t1.mark ();                                   // Set start reference point
  for (long i = 0; i < 1000000L; i++)   // Loop for 1000000 times and
    sum = sum + i;                                      // Sum up numbers
  long elapsed = t1.real() + 1; // make sure it's non-zero
  elapsed = log10(double(elapsed)) + 1;
  elapsed = log10(double(elapsed));
  cout << "Summation of integers from 0 through 1000000 (" << sum << "=" << check << ") took ";
  cout << "log10(log10(time in ms)) = " << elapsed << endl;
  return 0;                                     // Return valid competion code
}
