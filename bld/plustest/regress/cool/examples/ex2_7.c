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

#include <cool/Regexp.h>                        // Include Regexp header file

CoolRegexp r1("Hi There");                      // Define simple pattern

int main (void) {
    cout << __FILE__ << endl;
  char dummy[] = "Garbage Hi There garbage";    // Dummy string to search
  cout << "The pattern `Hi There' ";            // Output start of sentence
  if (r1.find (dummy) == TRUE)                  // Pattern found in string?
    cout << "is";                               // Yes, indicate afirmative
  else
    cout << "is not";                           // Else indicate failure
  cout << " found in `" << dummy << "'\n";      // And complete output
  cout << "The pattern begins at zero-relative index " << r1.start ();
  cout << " and ends at index " << r1.end () << "\n";
  r1.compile("[^ab1-9]");                       // Complex pattern
  strcpy (dummy, "ab123QQ59ba");                // Another string to search
  cout << "The pattern `[^ab1-9]' ";            // Output start of sentence
  if (r1.find (dummy) == TRUE)                  // Pattern found in string?
    cout << "is";                               // Yes, indicate afirmative
  else
    cout << "is not";                           // Else indicate failure
  cout << " found in `" << dummy << "'\n";      // And complete output
  cout << "The pattern begins at zero-relative index " << r1.start ();
  cout << " and ends at index " << r1.end () << "\n";
  r1.compile("O(.*r)");                         // New complex pattern
  strcpy (dummy, "That's OK for me. OK for you?"); // Another string to search
  cout << "The pattern `O(.*r)' ";              // Output start of sentence
  if (r1.find (dummy) == TRUE)                  // Pattern found in string?
    cout << "is";                               // Yes, indicate afirmative
  else
    cout << "is not";                           // Else indicate failure
  cout << " found in `" << dummy << "'\n";      // And complete output
  cout << "The pattern begins at zero-relative index " << r1.start ();
  cout << " and ends at index " << r1.end () << "\n";
  return (0);                                   // Exit with OK status
}

