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

#include <cool/Gen_String.h>                    // Include Gen_String header file
#include <cool/Regexp.h>                        // Include Regexp header file

int main (void) {
  CoolGen_String s1 = "Hello";                  // Create string object
  cout << "s1 reads: " << s1 << "\n";           // Display string value
  cout << "s1 has " << strlen (s1) << " characters\n"; // Display char count
  s1 = s1 + " " + "world!";                     // Concatenate characters
  cout << "s1 reads: " << s1 << "\n";           // Display string value
  cout << "s1 has " << strlen (s1) << " characters\n"; // Display char count
  s1.reverse ();                                // Reverse character order
  cout << "s1 backwards reads: " << s1 << "\n"; // Output reversed string
  s1.reverse ();                                // Get normal ordering back
  cout << "s1 upper case: " << upcase (s1) << "\n"; // Display uppercase value
  cout << "s1 lower case: " << downcase (s1) << "\n";// Display downcase value
  cout << "s1 capitalized: " << capitalize (s1) << "\n"; // Display capitalized value
  s1.insert ("Oh, ", 0);                        // Insert at start of string
  cout << "s1 reads: " << s1 << "\n";           // Display string value
  s1.replace ("Goodbye", 4, 9);                 // Replace `hello' with `goodbye'
  cout << "s1 reads: " << s1 << "\n";           // Display string value
  s1.remove (4, 12);                            // Remove `goodbye'
  cout << "s1 reads: " << s1 << "\n";           // Display string value
  s1.compile("Hi There");                       // Define simple pattern
  s1 = "Garbage Hi There garbage";              // Set string to search
  cout << "The pattern `Hi There' ";            // Output start of sentence
  if (s1.find () == TRUE)                       // Pattern found in string?
    cout << "is";                               // Yes, indicate afirmative
  else
    cout << "is not";                           // Else indicate failure
  cout << " found in `" << s1 << "'\n";         // And complete output
  cout << "The pattern begins at zero-relative index " << s1.start ();
  cout << " and ends at index " << s1.end () << "\n";
  s1.compile("[^ab1-9]");                       // Complex pattern
  s1 = "ab123QQ59ba";                           // Another string to search
  cout << "The pattern `[^ab1-9]' ";            // Output start of sentence
  if (s1.find () == TRUE)                       // Pattern found in string?
    cout << "is";                               // Yes, indicate afirmative
  else
    cout << "is not";                           // Else indicate failure
  cout << " found in `" << s1 << "'\n";         // And complete output
  cout << "The pattern begins at zero-relative index " << s1.start ();
  cout << " and ends at index " << s1.end () << "\n";
  s1.compile("O(.*r)");                         // New complex pattern
  s1 = "That's OK for me. OK for you?";         // Another string to search
  cout << "The pattern `O(.*r)' ";              // Output start of sentence
  if (s1.find () == TRUE)                       // Pattern found in string?
    cout << "is";                               // Yes, indicate afirmative
  else
    cout << "is not";                           // Else indicate failure
  cout << " found in `" << s1 << "'\n"; // And complete output
  cout << "The pattern begins at zero-relative index " << s1.start ();
  cout << " and ends at index " << s1.end () << "\n";
  return (0);                                   // Exit with OK status
}
