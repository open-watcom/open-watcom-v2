#ifndef mpsf2_H
#include "psf2.H"
#endif

template<size_t size>
void *   psfist_Imp<size>::_free_list = 0;

template<size_t size>
int      psfist_Imp<size>::free()
{
void *   p = _free_list;
int      n = 0;
#ifdef BUG_CONFIRM
   cout << "free():  " << (size&1) << endl;
#endif
   while( p != 0 ) {
   void *   q = p;
      p = *((void **) p);
      ::free(q);
      n++;
   }
   _free_list = 0;
   return n;
}

template<size_t size>
int      psfist_Imp<size>::free(unsigned num)
{
void *   p = _free_list;
int      n = 0;
#ifdef BUG_CONFIRM
   cout << "free(n): " << (size&1) << endl;
#endif
   while( p != 0 && n < num) {
   void *   q = p;
      p = *((void **) p);
      ::free(q);
      n++;
   }
   _free_list = p;
   return n;
}
