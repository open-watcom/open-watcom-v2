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

#include <cool/String.h>
#include <cool/Range.h>                         // Include range header file

#include <cool/Range.C>
#if 0
IMPLEMENT CoolRange<double,2.5,8.8>;            // Implement CoolRange of doubles
DECLARE CoolRange<char*,"D", "K">;              // Declare CoolRange of strings
IMPLEMENT CoolRange<char*, "D", "K">;           // Implement CoolRange of strings
#else
typedef CoolRange<double,2.5,8.8> __define_CoolRange_doubles;
typedef CoolRange<double,"D","K"> __define_CoolRange_strings;
#endif

extern int my_compare (const charP& s1, const charP& s2) {
  return (strcmp (s1, s2));
}

int main (void) {
    cout << __FILE__ << endl;
  CoolRange<double,2.5,8.8> r1;                 // CoolRange-checked double
  r1.set(4.3);                                  // Assign value
  cout << "r1 has an inclusive low bound of " << r1.low(); // Output low and
  cout << ", an inclusive high bound of " << r1.high() << ",\n"; // High bounds
  cout << "and a value of " << (double)r1 << "\n"; // Output value
  double d1 = 1.9;                              // Declare a double
  cout << (double)r1 << " * " << d1 << " = ";   // Output equation
  r1.set (d1 * r1);                             // Calculate value
  cout << (double)r1 << "\n";                   // And display it
  CoolRange<charP,"D","K"> r2;                  // CoolRange-checked string
  r2.set_compare (&my_compare);                 // Set compare function
  r2.set("EFG");                                // Assign value
  cout << "r2 has an inclusive low bound of " << r2.low();
  cout << ", n inclusive high bound of " << r2.high() << ",\n";
  cout << "a value of " << (char*)r2;           // Output string value
  cout << ", and a length of " << strlen (r2) << "\n"; // Output length
  return 0;                                     // Exit with OK status
}
