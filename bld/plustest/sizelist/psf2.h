#ifndef mpsf2_H
#define mpsf2_H

#ifndef mpsf1_H
#include "psf1.H"
#endif

#include <iostream.h>

/* 26th December, 1992 Mayan Moudgill.
   See pcf1.H for details
*/

template <size_t size>
class psfist_Imp {
private:
   static void *     _free_list;
public:
   static int        free();
   static int        free(unsigned);

   static
   inline void *     alloc(size_t sz)
      {
      void *  obj;
#ifdef BUG_CONFIRM
        cout << "alloc:   " << ( size & 1 ) << endl;
#endif
        (void) sz;
        if( _free_list == 0 ) {
           obj = malloc(size);              // is sz+sz%4 == size? it should.
        }
        else {
           obj = _free_list;
           _free_list = *((void **) obj); // Its not type safe--so what?
        }
        return obj;
      }
   static
   inline void       dealloc(void * obj)
      {
#ifdef BUG_CONFIRM
        cout << "dealloc: " << ( size & 1 ) << endl;
#endif
        *((void **) obj) = _free_list;
        _free_list = obj;                 // Neither is this--so what?
      }
};

template<class T>
void *    psfist<T>::operator new(size_t sz)
{
   return psfist_Imp<unsigned(sizeof(T))>::alloc(sz);
}

template<class T>
void      psfist<T>::operator delete(void * obj)
{
   psfist_Imp<unsigned(sizeof(T))>::dealloc(obj);
}

template<class T>
int       psfist<T>::free()
{
   return psfist_Imp<unsigned(sizeof(T))>::free();
}

template<class T>
int       psfist<T>::free(unsigned n)
{
   return psfist_Imp<unsigned(sizeof(T))>::free(n);
}

#include "psf2.cpp"

#endif
