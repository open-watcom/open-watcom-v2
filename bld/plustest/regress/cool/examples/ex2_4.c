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

#include <cool/String.h>                        // Include CoolString header file

int main (void) {
    cout << __FILE__ << endl;
  CoolString s1 = "Hello";                      // Create CoolString object
  cout << "s1 reads: " << s1 << "\n";           // Display CoolString value
  cout << "s1 has " << strlen (s1) << " characters\n"; // Display char count
  s1 = s1 + " " + "world!";                            // Concatenate characters
  cout << "s1 reads: " << s1 << "\n";                  // Display CoolString value
  cout << "s1 has " << strlen (s1) << " characters\n"; // Display char count
  s1.reverse ();                                       // Reverse character order
  cout << "s1 backwards reads: " << s1 << "\n";        // Output reversed CoolString
  s1.reverse ();                                       // Get normal ordering back
  cout << "s1 upper case: " << upcase (s1) << "\n";    // Display uppercase value
  cout << "s1 lower case: " << downcase (s1) << "\n";  // Display downcase value
  cout << "s1 capitalized: " << capitalize (s1) << "\n"; // Display capitalized value
  s1.insert ("Oh, ", 0);                                 // Insert at start of CoolString
  cout << "s1 reads: " << s1 << "\n";                    // Display CoolString value
  s1.replace ("Goodbye", 4, 9);                          // Replace `hello' with `goodbye'
  cout << "s1 reads: " << s1 << "\n";                    // Display CoolString value
  s1.remove (4, 12);                                     // Remove `goodbye'
  cout << "s1 reads: " << s1 << "\n";                    // Display CoolString value
  return (0);                                            // Exit with OK status
}

