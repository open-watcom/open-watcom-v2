/****************************************************************************
File: PlaceNew.h -- Overloaded operator new() with placement syntax

Description:
   Provides overloaded 'operator new()' with placement syntax for
   constructing objects at a specified location.
   Three template functions are also provided to explicitly construct
   and destroy objects.

Usage:
   char storage[sizeof(Foo)];
   Foo* p = new ((JAM_PL*)storage) Foo(1,2,3);    // (char*)p==storage
   JAM_destroy(*p);     // calls p->~Foo()
   Foo& r = *(Foo*)storage;
   JAM_construct(r);    // calls default constructor
   char storage2[sizeof(Foo)];
   JAM_construct((Foo&)storage2, r);  // copy-constructs from r to storage2

Notes:
   Contains #ifdef for Borland C++ 3.1 bug.  "i.~int();" syntax new in ARM.
   This file will probably not be necessary if ANSI C++ decides to put
      inline void* operator new(size_t,void*p) { return p; }
   in <new.h>.  Watch out for other libraries already defining this
   function.
   
History:
1992 Sep 01 Jam      created
   
****************************************************************************/     
#ifndef JAM_PlacementNew_H
#define JAM_PlacementNew_H

#include <stddef.h>     // for size_t

inline void* operator new(size_t, void* p) { return p; }

template<class T> /*inline###*/ void JAM_construct(T& dst)
   { new ((void*)&dst) T; }

template<class T> /*inline###*/ void JAM_construct(T& dst, const T& src)
   { new ((void*)&dst) T(src); }

template<class T> inline void JAM_destroy(T& t)
   { t.~T(); }


#endif // JAM_PlacementNew_H
