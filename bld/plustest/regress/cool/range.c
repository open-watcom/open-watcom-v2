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
//
// Updated: JAM 08/14/92 -- added static data member defs; fixed anach. form()

#include <cool/Range.h>

template<class Bounds>
void CoolRange<Bounds>::report_set_error () const {
  printf ("CoolRange out of bounds error.");
  abort();
}

