/****************************************************************************
File: Array.h

Description:   class JAM_Array<T>
   See JAM_ControlledArray.  This is just like it but uses the
   default memory allocator MemDefault.

Usage:

Notes:
   
History:
25 Dec 1991 Jam      created from my <generic.h> macros
24 Feb 1992 Jam      made to use JAM_assert
24 Feb 1992 Jam      removed search(),contains(), etc member funcs
02 Mar 1992 Jam      bugfix -- made clear() set _arr,_size=0 again
10 Mar 1992 Jam      resize() now fails if new_size will be too big
10 Mar 1992 Jam      added static max() to check if resize() would fail
28 Apr 1992 Jam      moved inlines inside class because of BC++ 3.0 bug
05 Oct 1992 Jam      reimplemented using ControlledArray
   
****************************************************************************/     
#ifndef JAM_Array_H
#define JAM_Array_H

#include <CtrldArr.h>
#include <MemDefau.h> //## lt

//************************************************************************
// JAM_Array definition
//************************************************************************

template<class T>
class JAM_Array : public JAM_ControlledArray<T,JAM_MemDefault> {
public:
   JAM_Array()
   //: constructs zero element array
      : JAM_ControlledArray<T,JAM_MemDefault>() {}

   JAM_Array(size_t length)
   //: constructs "length" element array using T's default constructor
      : JAM_ControlledArray<T,JAM_MemDefault>(length) {}

   JAM_Array(size_t length, const T& init)
   //: copy constructs each element with an initial value
      : JAM_ControlledArray<T,JAM_MemDefault>(length, init) {}

   // inherits all public members of JAM_ControlledArray
};

#endif // JAM_Array_H

