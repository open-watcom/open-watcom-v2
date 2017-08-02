//
// Copyright (C) 1992 General Electric Company.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// General Electric Company provides this software "as is" without
// express or implied warranty.
//
// Created: VDN 03/25/92 -- Initial design
// Updated: JAM 08/12/92 -- removed 'static' from static memb func def
//
// Shared is a mixin slot, which store the number of references and 
// handles pointed at an object. Shared does not have a virtual destructor,
// and so the deletion of the object must be done by the caller of dereference.

#ifndef SHAREDH
#define SHAREDH

#ifndef MISCH
#include <cool/misc.h>                          // for INVALID marker
#endif

class CoolShared {
public:
  inline CoolShared();                          // Initialize ref_count=0
  inline ~CoolShared();                         // Destructor

  inline int reference_count();                 // query current ref_count
  inline int reference();                       // inc ref_count
  inline int dereference();                     // dec ref_count
  
  static inline int reference(CoolShared* ptr); // noop if ptr=NULL
  static inline int dereference(CoolShared* ptr);

private:
  int ref_count;                                // count of active references
};

// CoolShared() -- Default constructor initializes ref_count to 0

inline CoolShared::CoolShared()
: ref_count(0) {}                               // ref_count initially 0

// ~CoolShared -- Destructor is not virtual, so caller of dereference
//               must call delete on object ptr, with correct type.

inline CoolShared::~CoolShared() {}             // nothing

// reference_count() -- Query current ref_count

inline int CoolShared::reference_count() {
  return ref_count;
}

// reference() -- increment ref_count and return new count.

inline int CoolShared::reference() {
  return ++ref_count;
}

// dereference() -- decrement ref_count and return new count.

inline int CoolShared::dereference(){
  return --ref_count;
}

// Check pointers first, noop if ptr == NULL.
  
inline int CoolShared::reference (CoolShared* ptr) {
  if (ptr)
    return ptr->reference();
  else
    return INVALID;
}

inline int CoolShared::dereference (CoolShared* ptr) {
  if (ptr)
    return ptr->dereference();
  else
    return INVALID;
}



#endif                                          // SHAREDH
