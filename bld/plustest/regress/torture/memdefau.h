/****************************************************************************
File: memdefault.h -- JAM_MemDefault struct provides default allocation routines

Description:
   The JAM library's "Controlled" containers take a class parameter which
   will provide memory.  The class should have a "void* alloc(size_t)"
   and a "void free(void*)" static member function.  JAM_MemDefault
   just defines them as using the global operator new()/delete().

Usage:

Notes:
   
History:
01 Sep 1992 Jam      created (ref. Stroustrup2)
   
****************************************************************************/     
#ifndef JAM_MemDefault_H
#define JAM_MemDefault_H

#include <stddef.h>     // for size_t

struct JAM_MemDefault {
   static void* alloc(size_t s) { return operator new(s); }
   static void free(void* p) { operator delete(p); }
};

#endif // JAM_MemDefault_H
