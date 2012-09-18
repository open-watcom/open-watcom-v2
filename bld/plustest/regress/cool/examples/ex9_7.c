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

#include <cool/AVL_Tree.h>                      // Include AVL tree class
#include <cool/String.h>                        // Include COOL String class
#include <cool/Gen_String.h>                    // Include COOL Gen_String class

#include <cool/Binary_Node.C>
#include <cool/Binary_Tree.C>
#include <cool/AVL_Tree.C>

static CoolGen_String text ("\n\
     A programming language serves two related purposes: it provides a\n\
     vehicle for the programmer to specify actions to be executed and a\n\
     set of concepts for the programmer to use when thinking about what\n\
     can be done.");

int main (void) {
  CoolAVL_Tree<CoolString> avl1;                // Declare tree variable
  CoolGen_String s;                             // Temporary CoolString variable
  text.compile ("[a-zA-Z]+");                   // Match any alphabetical word
  while (text.find ()) {                        // While still more words
    text.sub_string (s, text.start (), text.end ()); // Get word from paragraph
    avl1.put (*(new CoolString(upcase (s))));        // And add to tree
  }
  //Bug here, AVL Tree is not completely balanced.
  //it needs one extra balance call to match structure in manual.
  //avl1.balance ();
  cout << avl1;                                 // Output tree structure
  return (0);                                   // Exit with successful status
}
