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

#include <cool/Matrix.h>                        // COOL Matrix class

#include <cool/Matrix.C>

int main (void) {
  CoolMatrix<int> mc1(3,4),mc2(3,4);            // Two 3x4 matrices of integer
  for (int i = 0; i < 3; i++)                   // For each row in CoolMatrix
    for (int j = 0; j < 4; j++)                 // For each column in CoolMatrix
      mc1.put(i,j,(i+2)*(j+3));                 // Assign element value
  mc2 = mc1 + 5;                                // Copy CoolMatrix with added value
  mc1 = mc1+mc2;                                // Add the matrices together
  cout << mc1 << "\n" << mc2;                   // Output the starting matrices
  return (0);                                   // Exist with OK status
}
