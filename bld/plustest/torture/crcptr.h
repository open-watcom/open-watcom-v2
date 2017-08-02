/****************************************************************************
File: CRCPtr.h -- constrained reference-counting smart-pointer class

Description:  JAM_ReferenceCounter class definition
              JAM_CRCPtr template class
   
   A JAM_CRCPtr<T> object is just like a pointer but you don't have to
   worry about memory leaks because it counts references.  Because
   JAM_CRCPtrs behave just like regular pointers, they can alias the
   same object.  T must derive from public JAM_ReferenceCounter (defined
   below).  

Notes:
   I have no idea how any of this would work when dealing with virtual
   bases (MI), but for starters you should probably inherit virtually
   from JAM_ReferenceCounter.


History:
1991 Dec 20 Jam      created from my <generic.h> macro
1992 Feb 12 Jam      had to move defs of all SmartPtr funcs out of class
                     because of a BC++ 3.0 bug when using functors
1992 Feb 14 Jam      made ~JAM_ReferenceCounter protected to prevent
                     users from using delete
1992 Mar 20 Jam      changed operator void* to const void* for safety
1992 Sep 02 Jam      had to move defs of all SmartPtr funcs BACK into class
                     because of a BC++ 3.1 bug when using functors!!
1992 Oct 05 Jam      renamed to CRCPtr from SmartPtr, other minor mods
   
****************************************************************************/     

#ifndef JAM_CRCPtr_H
#define JAM_CRCPtr_H

#include <assert.hpp>

//***************************************************************************
// JAM_ReferenceCounter definition
//***************************************************************************

class JAM_ReferenceCounter {
private:
   unsigned int _refs;     // number of CRCPtrs referencing me
   JAM_ReferenceCounter(const JAM_ReferenceCounter&); // hide me -- can't copy
   void operator=(const JAM_ReferenceCounter&);     // hide me -- can't copy
protected:
      // keep dtors protected so user can't `delete p;' -- we
      // delete ourselves if you only use CRCPtrs
   virtual ~JAM_ReferenceCounter() { JAM_assert(_refs==0); }
public:
   JAM_ReferenceCounter() : _refs(0) {}
   int numRefs() const { return _refs; }
   void incRefs() { ++_refs; }
   void decRefs() {
      JAM_assert(_refs>0);
      if (--_refs==0) delete this;
      }
};


//***************************************************************************
// JAM_CRCPtc definition
//***************************************************************************

template<class T> class JAM_CRCPtr {
public:
   JAM_CRCPtr()
      : _ptr(0) {}

   /* itemp MUST BE ON HEAP.  Do not use itemp after assignment. */
   JAM_CRCPtr(T* itemp)
      : _ptr(itemp) { if (_ptr) _ptr->incRefs(); }

   JAM_CRCPtr(const JAM_CRCPtr<T>& smrt)
      : _ptr(smrt._ptr) { if (_ptr) _ptr->incRefs(); }

   ~JAM_CRCPtr()
      { if (_ptr) { _ptr->decRefs(); _ptr=0; } }

   void operator=(const JAM_CRCPtr<T>& smrt) {
      if (this==&smrt || _ptr==smrt._ptr) return;
      if (_ptr) _ptr->decRefs();
      _ptr = smrt._ptr;
      if (_ptr) _ptr->incRefs();
      }

   /* itemp MUST BE ON HEAP.  Do not use itemp after assignment */
   void operator=(T* itemp) {
      JAM_assert(_ptr!=itemp); /* weird stuff happening */
      if (_ptr) _ptr->decRefs();
      _ptr = itemp;
      if (_ptr) _ptr->incRefs();
      }

   /* does *NOT* call T == T */
   int operator==(const JAM_CRCPtr<T>& smrt)
      { return _ptr==smrt._ptr; }

   const T* operator->() const
      { JAM_assert(_ptr != 0); return _ptr; }
   const T& operator*() const
      { JAM_assert(_ptr != 0); return *_ptr; }

   T* operator->()
      { JAM_assert(_ptr != 0); return _ptr; }
   T& operator*()
      { JAM_assert(_ptr != 0); return *_ptr; }

   operator const void*() const
      { return _ptr; }

protected:
   T* _ptr;
};


//***************************************************************************
// JAM_CRCPtr inlines
//***************************************************************************


#endif // JAM_CRCPtr_H

