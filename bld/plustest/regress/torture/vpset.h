/****************************************************************************
File: VPSet.h

Description:   class JAM_ControlledVoidPtrSet<Mem>
               class JAM_VoidPtrSet
               class JAM_PtrSet<T>

Usage:

Notes:
   0 (NULL) is not allowed in the set.

History:
30 Sep 1992 Jam      created for use in debugging new/delete
20 Oct 1992 Jam      renamed file VPSet.h
20 Oct 1992 Jam      added non-controlled VoidPtrSet and PtrSet<T>
   
****************************************************************************/     
#ifndef JAM_VoidPtrSet_H
#define JAM_VoidPtrSet_H

#include <stddef.h>
#include <Vector.h>
#include <MemDefau.h> //## lt

//************************************************************************
// JAM_ControlledVoidPtrSet definition
//************************************************************************

inline void JAM_construct(void*& dst, void*const & src) //## BC++ 3.1 requires this
  { dst = src; }

template<class Mem>
class JAM_ControlledVoidPtrSet {
public:
   JAM_ControlledVoidPtrSet()
      : _arr(0, 100) {}
   void enter(void* p);
   void remove(void* p);
   int contains(void* p) const;

   typedef size_t IterState;
   typedef void* IterItemType;
   size_t nextelem(size_t start) const; 
   size_t prevelem(size_t start) const; 
   static int valid(const IterState& i)
      { return i != size_t(-1); }
   void gofirst(IterState& i) const
      { if (_arr.length()==0) i = size_t(-1);
        else i = nextelem(0); }
   void golast(IterState& i) const
      { if (_arr.length()==0) i = size_t(-1);
        else i = prevelem(_arr.length()-1); }
   void next(IterState& i) const
      { if (_arr.length()==0 || i==_arr.length()-1) { i = size_t(-1); }
        else { if (valid(i)) i=nextelem(i+1); else i=nextelem(0); } }
   void prev(IterState& i) const
      { if (_arr.length()==0 || i==0) { i = size_t(-1); }
        else { if (valid(i)) i=prevelem(i-1); else i=prevelem(_arr.length()-1); } }
   void* val(const IterState& i) const
      { JAM_assert(i<_arr.length()); return _arr[i]; }

protected:
   JAM_ControlledVector<void*, Mem> _arr;
};

//************************************************************************
// JAM_VoidPtrSet definition
//************************************************************************

class JAM_VoidPtrSet : public JAM_ControlledVoidPtrSet<JAM_MemDefault> {};


//************************************************************************
// JAM_PtrSet definition
//************************************************************************

template<class T>
class JAM_PtrSet : private JAM_VoidPtrSet {
   typedef JAM_VoidPtrSet inherit;
public:
   void enter(T* p)
      { inherit::enter(p); }
   void remove(T* p)
      { inherit::remove(p); }
   int contains(T* p) const
      { return inherit::contains(p); }

   typedef inherit::IterState IterState;
   typedef T* IterItemType;
   static int valid(const IterState& i)
      { return inherit::valid(i); }
   void gofirst(IterState& i) const
      { inherit::gofirst(i); }
   void golast(IterState& i) const
      { inherit::golast(i); }
   void next(IterState& i) const
      { inherit::next(i); }
   void prev(IterState& i) const
      { inherit::prev(i); }
   T* val(const IterState& i) const
      { return (T*)inherit::val(i); }
};


//**************************************************************************
// JAM_ControlledVoidPtrSet functions
//**************************************************************************


//**************************************************************************
// JAM_ControlledVoidPtrSet inline member functions
//**************************************************************************

//**************************************************************************
// Non-inlined functions
//**************************************************************************

#ifndef DONT_INCLUDE_CPP
#include <VPSet.cpp>
#endif

#endif // JAM_VoidPtrSet_H

