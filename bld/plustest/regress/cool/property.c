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
//
// In Lisp, property list provides a flexible way of storing intermediate
// information, without changing the data structures. A property list is
// implemented as a association vector between names and values.
// Values can be pointers, ints, longs, float, double, and void* of course.


#include <cool/Property.h>

#include <cool/Pair.C>
#include <cool/Vector.C>
#include <cool/Association.C>


