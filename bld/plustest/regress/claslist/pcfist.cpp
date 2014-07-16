#ifndef mpcfIST_H
#include "pcfist.H"
#endif

template<class T>
void *   pcfist<T>::_free_list = 0;

template<class T>
int      pcfist<T>::free()
{
void *   p = _free_list;
int      n = 0;
   while( p != 0 ) {
   void *   q = p;
      p = *((void **) p);
      ::free(q);
      n++;
   }
   _free_list = 0;
   return n;
}

template<class T>
int      pcfist<T>::free(unsigned num)
{
void *   p = _free_list;
int      n = 0;
   while( p != 0 && n < num) {
   void *   q = p;
      p = *((void **) p);
      ::free(q);
      n++;
   }
   _free_list = p;
   return n;
}
