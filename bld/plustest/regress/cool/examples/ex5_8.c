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

#include <cool/Vector.h>                        // Include vector class

#include <cool/Vector.C>

int main (void) {
  CoolVector<int> v1;                           // Declare CoolVector object
  for (int i = 0; i < 10; i++)                  // Copy 10 integers into CoolVector
    v1.push(i);                                 // Add element
  cout << v1 << "\n";                           // Output CoolVector elements
  return (0);                                   // Exit with successful status
}
