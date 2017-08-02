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
// Updated: VDN 02/21/92 -- Separate NT_State which must be implemented once
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//


#ifndef N_TREE_STATEH                           // If no definition for class
#define N_TREE_STATEH

#ifndef STACKH                                  // If no definition for class
#include <cool/Stack.h>                         // include definition file
#endif

#ifndef PAIRH                                   // If no definition for class
#include <cool/Pair.h>                          // include definition file
#endif

typedef CoolPair<long,int> CoolNT_Stack_Entry;
typedef CoolStack< CoolPair<long,int> > CoolNT_Stack;


// Simple class that bundles an iterator stack with the direction 
// of the traversal (forward or backward).  This is the current_position
// state that can be saved and restored 

class CoolNT_State {                            // State bundles Stack&Boolean
public:
  inline CoolNT_State () {};                    // Simple constructor
  CoolNT_State (const CoolNT_State& s);         // Copy constructor
  inline ~CoolNT_State () {};                   // Destructor
  
  CoolNT_State& operator= (const CoolNT_State& s); // overload = operator

  CoolNT_Stack stack;                           // should be protected
  Boolean forward;
};

#endif                                          // N_TREE_STATEH




