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
// Changed: VDN 04/15/92 -- Lice version
// Updated: JAM 08/19/92 -- modernized template syntax, remove macro hacks
//
// In Lisp, property list provides a flexible way of storing intermediate
// information, without changing the data structures. A property list is
// implemented as a association vector between names and values.
// Values can be pointers, ints, longs, float, double, and void* of course.

#ifndef PROPERTYH
#define PROPERTYH

#ifndef STRINGH
#include <cool/String.h>
#endif

#ifndef VALUEH
#include <cool/Value.h>
#endif

#ifndef ASSOCIATIONH
#include <cool/Association.h>
#endif

typedef CoolPair<CoolString, CoolValue> HackProperty; //## BC++ requires this for Association
typedef CoolAssociation<CoolString, CoolValue> CoolProperty;

#endif




