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
//

#include <cool/NT_State.h>

#include <cool/Pair.C>
#include <cool/Stack.C>

// NT_State() -- Copy constructor

CoolNT_State::CoolNT_State (const CoolNT_State& s) {    
  this->stack = s.stack;                        
  this->forward = s.forward;
}

// operator = -- Overload assignment operator

CoolNT_State& CoolNT_State::operator= (const CoolNT_State& s) { 
  this->stack = s.stack;                        // Needs memberwise copy
  this->forward = s.forward;
  return *this;
}
