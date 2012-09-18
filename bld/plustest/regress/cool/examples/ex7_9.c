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

#include <cool/Hash_Table.h>                    // Include Hash_Table class
#include <cool/Gen_String.h>                    // Include COOL String class
#include <cool/Iterator.h>                      // Include COOL Iterator class
#include "paragraph.h"                          // Include Stroustrup text

#include <cool/Hash_Table.C>

int main (void) {
  CoolHash_Table<CoolGen_String,int> h1;        // Declare CoolHash_Table variable
  CoolGen_String s;                             // Temporary string variable
  int counter = 0, max_count = 0;               // Initialize word counters
  cout << text;                                 // Output paragraph
  text.compile ("[a-zA-Z]+");                   // Match any alphabetical word
  while (text.find ()) {                        // While still more words
    text.sub_string (s, text.start (), text.end ()); // Get word from paragraph
    if (h1.find (s))                                 // If word already found
      h1.put (h1.key (), h1.value ()+1);             // Update use count
    else 
      h1.put (s, 1);                            // Else add word 
  }
  h1.reset ();                                  // Invalidate current position
  CoolIterator<CoolHash_Table<CoolGen_String,int> > i1;;                // CoolIterator object
  while (h1.next ()) {                          // While there are still nodes
    counter += h1.value ();                     // Sum number of words used
    if (h1.value () > max_count) {              // If most used word so far
      i1 = h1.current_position ();                      // Save position in list
      max_count = h1.value ();                          // And keep track of usage
    }
  }
  cout << "There are " << counter << " words\n";
  cout << "There are " << h1.length () << " unique words\n";
  h1.current_position () = i1;                  // Set position of most used word
  cout << "The most common word is `" << h1.key () << "' and is used " << h1.value () << " times\n";
  return (0);                                   // Exit with successful status
}
