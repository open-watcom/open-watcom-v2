/****************************************************************************
File: VPSet.cpp

Description:   class JAM_ControlledVoidPtrSet<Mem>
               class JAM_VoidPtrSet

Usage:

Notes:
   
History:
   
****************************************************************************/     
#ifndef JAM_VoidPtrSet_CPP
#define JAM_VoidPtrSet_CPP

#include <VPSet.h>

//************************************************************************
// JAM_ControlledVoidPtr member functions
//************************************************************************

template<class Mem>
size_t JAM_ControlledVoidPtrSet<Mem>::nextelem(size_t start) const
{
   size_t n = _arr.length();
   while (start<n && _arr[start]==0)
      ++start;
   if (start<n) return start;
   else return size_t(-1);
}

template<class Mem>
size_t JAM_ControlledVoidPtrSet<Mem>::prevelem(size_t start) const
{
   JAM_assert(start<_arr.length());
   while (_arr[start]==0) {
      if (start==0) return size_t(-1);
      else --start;
      }
   return start;
}

template<class Mem>
void JAM_ControlledVoidPtrSet<Mem>::enter(void* p)
{
   JAM_assert(p!=0);
   if (!contains(p)) {
      size_t n = _arr.length();
      size_t i = 0;
      while (i<n && _arr[i]!=0)
         ++i;
      if (i<n) {
         _arr[i] = p;   // fill an empty slot
      } else {
         if (_arr.allocated()==n) {    // need to grow array
            if (_arr.allocated()==_arr.max())   // can't increase size
               JAM_crash("JAM_ControlledVoidPtrSet is full.");
            else if (n>_arr.max()/2) _arr.allocate(_arr.max());
            else _arr.allocate(n*2);
         }
         _arr.append(p);
      }
   }
}

template<class Mem>
void JAM_ControlledVoidPtrSet<Mem>::remove(void* p)
{
   JAM_assert(p!=0);
   size_t n = _arr.length();
   size_t i = 0;
   while (i<n && _arr[i]!=p)
      ++i;
   JAM_assert(i<n);  // assert found
   _arr[i] = 0;
}

template<class Mem>
int JAM_ControlledVoidPtrSet<Mem>::contains(void* p) const
{
   JAM_assert(p!=0);
   size_t n = _arr.length();
   for (size_t i=0; i<n; ++i)
      if (_arr[i]==p) return 1;
   return 0;
}


//************************************************************************
// JAM_ControlledVoidPtr functions
//************************************************************************


#endif // JAM_VoidPtrSet_CPP

