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

#include <cool/Random.h>                        // Include Random number class
#include <iostream.h>

int main (void) {
    cout << __FILE__ << endl;

  CoolRandom r1 (SIMPLE, 1, 3.0, 9.0);          // Simple rand() generator
  CoolRandom r2 (THREE_CONGRUENTIAL, 1, 5.0, 11.5);     // Highly random generator

  cout << "Simple random number generator:\n";  // Output banner title 
  int i;
  for (i = 0; i < 10; i++)                      // Generate 10 random numbers
    cout << "  Random number " << i << " is: " << r1.next () << "\n";
  cout << "\nThree congruential linear random number generator:\n"; // Banner
  for (i = 0; i < 10; i++)                      // Generate 10 random numbers
    cout << "  Random number " << i << " is: " << r2.next () << "\n";
  return (0);                                   // Exit with OK status
}
