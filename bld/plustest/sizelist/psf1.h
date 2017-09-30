#ifndef mpsf1_H
#define mpsf1_H

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
      #include "psf1.H"

      class Foo : psfist<Foo> {
        .
        .
        .
      };

      #include "psf2.H"

   This class differs from pcfist in that there is one free_list
   per *size* of allocated, not one per class. Thus, if you have 2 classes,
   both of the same size, they will use the same free list if memory operations
   are bolted on using psfist.
*/

/* There is an extremely nasty bug in cfront 3.0.1 which forces the
   use of the _dispatch() to actually call the right functions from
   psfist_Imp (see psf2.H). The problem
   causes all but the first member function in ParSizefist to use
   the wrong sized psfist_Imp. To check wether your complier
   has a similar bug define BUG_CONFIRM, compile and run, and see whether
   all the sizes are the same.

   A workaround (sent by USL support) has been applied. It may or
   may not work with your compiler.
*/

/* uncomment next line, compile and run to see if bug exists */
#define BUG_CONFIRM

#include <malloc.h>

template<class T>
class psfist {
private:
public:
   inline
   void *            operator new(size_t);
   inline
   void              operator delete(void *);
   static inline
   int               free();
   static inline
   int               free(unsigned);
};

#endif
