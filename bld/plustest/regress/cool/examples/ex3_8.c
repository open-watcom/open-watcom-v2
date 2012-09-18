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

#include <cool/Rational.h>                      // Include COOL Rational class

int main (void) {
  CoolRational r1 (10,3);                       // Create CoolRational object
  CoolRational r2 (-4,27), r3;                  // Create CoolRational objects
  r3 = r1 + r2;                                 // Calculate sum of values
  cout << r1 << " + " << r2 << " = " << r3 << "\n"; // And display result
  r3 = r1 * r2;                                 // Calculate product of values
  cout << r1 << " * " << r2 << " = " << r3 << "\n"; // And display result
  r3 = r1 / r2;                                 // Calculate quotient of values
  cout << r1 << " / " << r2 << " = " << r3 << "\n"; // And display result
  r3 = r1 % r2;                                 // Calculate remainder of values
  cout << r1 << " % " << r2 << " = " << r3 << "\n"; // And display result

  double d1 = double (10.0 / 3.0);              // Create double ratio
  double d2 = double (-4.0 / 27.0), d3;         // Create double ratios
  d3 = d1 + d2;                                 // Calculate sum of values
  cout << d1 << " + " << d2 << " = " << d3 << "\n"; // And display result
  d3 = d1 * d2;                                 // Calculate product of values
  cout << d1 << " * " << d2 << " = " << d3 << "\n"; // And display result
  d3 = d1 / d2;                                 // Calculate quotient of values
  cout << d1 << " / " << d2 << " = " << d3 << "\n"; // And display result
  return 0;                                     // Return valid success code
}
