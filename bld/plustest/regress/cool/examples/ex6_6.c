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

#include <cool/Stack.h>                         // COOL Stack class

#include <cool/Stack.C>

int main (void) {
    cout << __FILE__ << endl;
  CoolStack<int> s1(10);                        // Declare CoolStack of integers
  s1.push( -1 );
  int i;
  for (i = 1; i <= 5; i++)                      // In a small loop, push "n"
    s1.pushn (i,i);                             // copies of an integer value
  for (i = 0; i < 5; i++) {                     // In another similar loop upto
    for (int j = 0; j < s1.top(); j++)          // the top element value, get
      cout << s1.pop();                         // a value from CoolStack and print
    cout << "\n";                               // Now output a newline and repeat
  }
  return (0);                                   // Exit with OK status
}
