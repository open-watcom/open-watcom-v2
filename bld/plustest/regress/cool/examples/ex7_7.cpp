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

#include <cool/Association.h>                   // Include Association class
#include <cool/Gen_String.h>                    // Include COOL String class
#include <cool/Iterator.h>                      // Include COOL Iterator class
#include "paragraph.h"                          // Include Stroustrup text

#include <cool/Pair.C>
#include <cool/Vector.C>
#include <cool/Association.C>

int main (void) {
  typedef CoolPair<CoolGen_String,int> HackSI; //##
  CoolAssociation<CoolGen_String,int> a1;       // Declare CoolAssociation variable
  CoolGen_String s;                             // Temporary string variable
  int counter = 0, max_count = 0;               // Initialize word counters
  cout << text;                                 // Output paragraph
  text.compile ("[a-zA-Z]+");                   // Match any alphabetical word
  while (text.find ()) {                        // While still more words
    text.sub_string (s, text.start (), text.end ()); // Get word from paragraph
    if (a1.find (s))                                 // If word already found
      ++a1.value ();                                 // Increment use count
    else a1.put (s, 1);                              // Else add word 
  }
  a1.reset ();                                  // Invalidate current position
  CoolIterator<CoolAssociation<CoolGen_String,int> > i1;;               // CoolIterator object
  while (a1.next ()) {                          // While there are still nodes
    counter += a1.value ();                     // Sum number of words used
    if (a1.value () > max_count) {              // If most used word so far
      i1 = a1.current_position ();                      // Save position in list
      max_count = a1.value ();                          // And keep track of usage
    }
  }
  cout << "There are " << counter << " words\n";
  cout << "There are " << a1.length () << " unique words\n";
  a1.current_position () = i1;                  // Set position of most used word
  cout << "The most common word is `" << a1.key () << "' and is used " << a1.value () << " times\n";
  return (0);                                   // Exit with successful status
}



