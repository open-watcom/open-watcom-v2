#ifndef mpcfIST_H
#define mpcfIST_H

/* 26th December, 1992 Mayan Moudgill.
   I'm designing a class for "bolting-on" to an existing class,
   so as to give it free list based memory management.
   (I would use Mixin instead of Bolton except that the Mixin is
   used for _dynamic_ inheritance.)

   Purpose:
   Given a class Foo whose memory is to be managed in the
   following fashion:
      1. delete(f) takes f (of type Foo *), and adds it to a free list
      2. new() returns first member of free list (if any) else allocates
           from heap

   Additionally, the memory management routines contain
      1. free() returns all free list members to heap
      2. free(n) return n free list members to heap
   They both return the number actually freed.
   
   Usage:
      #include "pcfist.H"

      class Foo : pcfist<Foo> {
        .
        .
        .
      };
*/

#include <malloc.h>

template <class T>
class pcfist {
private:
   static void *     _free_list;
public:
   static int        free();
   static int        free(unsigned);

   inline void *     operator new(size_t sz)
      {
      void *  obj;
        if( _free_list == 0 ) {
           obj = malloc(sz);
        }
        else {
           obj = _free_list;
           _free_list = *((void **) obj); // Its not type safe--so what?
        }
        return obj;
      }
   inline void       operator delete(void * obj)
      {
        *((void **) obj) = _free_list;
        _free_list = obj;                 // Neither is this--so what?
      }
};

#include "pcfist.c"

#endif
