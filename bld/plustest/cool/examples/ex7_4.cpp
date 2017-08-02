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

#include <cool/List.h>                          // Include list header file
#include <cool/Gen_String.h>                    // Include COOL String class
#include <cool/Iterator.h>                      // Include COOL Iterator class
#include "paragraph.h"                          // Include Stroustrup text

#include <cool/List.C>

int main (void) {
  CoolList<CoolGen_String> l1;                          // Declare CoolList variable
  CoolGen_String s;                                     // Temporary string variable
  int max_count = 0;                            // Temporary counting variable
  cout << text;                                 // Output paragraph
  text.compile ("[a-zA-Z]+");                   // Match any alphabetical word
  while (text.find ()) {                        // While still more words
    text.sub_string (s, text.start (), text.end ()); // Get word from paragraph
    l1.push (s);                                     // And add to CoolList
  }
  l1.reset ();                                  // Invalidate current position
  while (l1.next ()) {                          // While there are still nodes
    int counter = 0;                            // Initialize counter
    CoolGen_String cur_word;                    // Temporary string variable
    CoolIterator<CoolList<CoolGen_String> > i1 = l1.current_position ();        // Save current position
    cur_word = l1.value ();                     // Get word to be counted
    l1.reset ();                                // Invalidate current position

    while (l1.next ())                          // While there are still nodes
      if (l1.value () == cur_word)              // If word appears in CoolList
        counter++;                              // Increment usage count
    if (counter > max_count) {                  // If most used word so far
      max_count = counter;                      // Update maximum count
      s = cur_word;                             // And save word
    }
    l1.current_position () = i1;                // Restore old current position
  }
  cout << "There are " << l1.length () << " words\n";
  l1.remove_duplicates ();                      // Remove duplicate words
  cout << "There are " << l1.length () << " unique words\n";
  cout << "The most common word is `" << s << "' and is used " << max_count << " times\n";
  return (0);                                   // Exit with successful status
}
