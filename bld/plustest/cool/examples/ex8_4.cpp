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

#include <cool/String.h>                        // COOL String class
#include <cool/Set.h>                           // COOL Set class

#include <cool/Set.C>

Boolean my_compare (const CoolString& s1, const CoolString& s2) {
  return ((strcmp (s1, s2) == 0) ? TRUE : FALSE);
}

static CoolString color_table[] = { "RED", "YELLOW", "PINK", "GREEN",
                                    "ORANGE", "PURPLE", "BLUE" };

int main (void) {
  CoolSet<CoolString> a(5), b(5);               // Declare two CoolSet objects
  a.set_compare (&my_compare);                  // Establish compare function
  for (int i = 0; i < 5; i++) {                 // For each color defined
    a.put (color_table[i]);                     // Add object to first CoolSet
    b.put (color_table[6-i]);                   // Add end object to second CoolSet
  }
  cout << "Set A contains: " << a;              // Elements of CoolSet 1
  cout << "Set B contains: " << b;              // Elements of CoolSet 2
  cout << "A | B: " << (a | b);                 // Display union
  cout << "A & B: " << (a & b);                 // Display intersection
  cout << "A ^ B: " << (a ^ b);                 // Display XOR
  cout << "A - B: " << (a - b);                 // Display difference
  return (0);                                   // Exit with OK status
}
