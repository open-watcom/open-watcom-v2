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

#include <cool/Bit_Set.h>                       // COOL Bit Set class

enum colors { RED=1, YELLOW, PINK, GREEN, ORANGE, PURPLE, BLUE };

static colors color_table[] = {RED, YELLOW, PINK, GREEN, ORANGE, PURPLE, BLUE};

int main (void) {
    cout << __FILE__ << endl;
  CoolBit_Set a, b;                             // Declare two bit set objects
  for (int i = 0; i < 5; i++) {                 // For each color defined
    a.put (color_table[i]);                     // Add object to first set
    b.put (color_table[6-i]);                   // Add end object to second set
  }
  cout << "Set A contains: " << a;              // Elements of set 1
  cout << "Set B contains: " << b;              // Elements of set 2
  cout << "A | B: " << (a | b);                 // Display union
  cout << "A & B: " << (a & b);                 // Display intersection
  cout << "A ^ B: " << (a ^ b);                 // Display XOR
  cout << "A - B: " << (a - b);                 // Display difference
  return (0);                                   // Exit with OK status
}
