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

#include <cool/Bignum.h>                        // Include Bignum class

int main (void) {
    cout << __FILE__ << endl;
  CoolBignum b1;                                // Create CoolBignum object
  CoolBignum b2 = "0xFFFFFFFF";                 // Create CoolBignum object
  CoolBignum b3 = "12345e30";                   // Create CoolBignum object
  cout << "b2 = " << b2 << "\n";                // Display value of b2
  cout << "b3 = " << b3 << "\n";                // Display value of b3
  b1 = b2 + b3;                                 // Add b2 and b3
  cout << "b2 + b3 = " << b1 << "\n";           // Display result
  b1 = b2 - b3;                                 // Subtract b3 from b2
  cout << "b2 - b3 = " << b1 << "\n";           // Display result
  b1 = b2 * b3;                                 // Multiply b2 and b3
  cout << "b2 * b3 = " << b1 << "\n";           // Display result
  b1 = b3 / b2;                                 // Divide b2 into b3
  cout << "b3 / b2 = " << b1 << "\n";           // Display result
  b1 = b3 % b2;                                 // Get b3 modulo b2
  cout << "b3 % b2 = " << b1 << "\n";           // Display result
  return 0;                                     // Exit with status code
}
