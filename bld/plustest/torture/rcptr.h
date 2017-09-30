/****************************************************************************
File: RCPtr.h -- An unconstrained reference-counting smart pointer class

Description:  class JAM_RCPtr<T>
   A JAM_RCPtr<T> object is a smart pointer which automaticly deletes
   the object pointed to when the last RCPtr referring to the object is
   destroyed.  T can be any type.  If you wish to allow RCPtr<Derived>
   to RCPtr<Base> conversions, define a specialization
   JAM_RCPtr<Derived> which inherits from JAM_RCPtr1<Derived,Base>.
   See testrcp.cpp for examples.

Usage:
   JAM_RCPtr<Foo> f = new Foo("hello");
   f->foo();
   JAM_RCPtr<Foo> g;
   g = new Foo;
   if (g) func(*g);
   if (f==g) ...;

Notes:
   "##" comments mark BC++ 3.1 bug workarounds   

History:
1992 Oct 12 Jam      created from Shared
   
****************************************************************************/     

#ifndef JAM_RCPtr_H
#define JAM_RCPtr_H

#include <assert.hpp>

//***************************************************************************
// JAM_RCPtr definition
//***************************************************************************

template<class T> struct JAM_RCPtr;

template<class T> class JAM_RCPtr0 {
public:
   JAM_RCPtr0()
   //: default constructor initializes to 0
      { ptr = 0; refsp = new_ref(1); }

   JAM_RCPtr0(T* p)
   //: initialize to p; do not delete p or use p after RCPtr is destroyed
      { ptr = p; refsp = new_ref(1); }

   JAM_RCPtr0(const JAM_RCPtr0<T>& other)
   //: this and other now refer to same object
      { ptr = other.ptr; refsp = other.refsp; ++(*refsp); }

   JAM_RCPtr0(T* p, unsigned* r)
   //: for use by RCPtr<D> to RCPtr<B> conversion operators only
      : ptr(p), refsp(r) { ++(*refsp); }

   ~JAM_RCPtr0()
   //: if an object is referenced and this is only reference, delete it
      { if (--(*refsp) == 0) { delete ptr; delete_ref(refsp); }    }

   void operator=(const JAM_RCPtr0<T>& other)
   //: detach current reference; refer to other's object
      {
      ++(*other.refsp);
      if (--(*refsp) == 0) { delete ptr; delete_ref(refsp); }
      ptr = other.ptr;
      refsp = other.refsp;
      }

   void operator=(T* p)
   //: detach current reference and refer p (see copy-ctor)
      {
      JAM_assert(p != ptr || p==0);   // make sure no weird stuff happening
      if (*refsp == 1) {  // reuse Rep
         delete ptr;
      } else {    // detach and create new Rep
         --(*refsp);
         refsp = new_ref(1);
      }
      ptr = p;
      }

   T* operator->() const
   //: access object as if this were a real pointer
      { JAM_assert(ptr); return ptr; }

   T& operator*() const
   //: access object as if this were a real pointer
      { JAM_assert(ptr); return *ptr; }

   operator const void*() const
   //: convert to a void* for tests and (rarely) to access object's address
      { return ptr; }

   /*inline##*/friend int operator==(const JAM_RCPtr<T>& a, const JAM_RCPtr<T>& b);
   //: determine if two RCPtrs refer to same object or are both 0

protected:
   T* ptr;
   unsigned* refsp;

   unsigned* new_ref(unsigned init) { unsigned* p = new unsigned(init); JAM_assert(p!=0); return p; }
   void delete_ref(unsigned* p) { JAM_assert(p!=0); /*###*/ delete p; }
   //: allow for faster allocation/deallocation (should use <Pool.h>)
};

template<class T> struct JAM_RCPtr : public JAM_RCPtr0<T> {
public:
   JAM_RCPtr() {}
   JAM_RCPtr(T* p) : JAM_RCPtr0<T>(p) {}
   JAM_RCPtr(const JAM_RCPtr0<T>& other) : JAM_RCPtr0<T>(other) {}
   void operator=(T* p) { JAM_RCPtr0<T>::operator=(p); }
//## The following two defs should not be necessary, but BC++ bug
   void operator=(const JAM_RCPtr& other) { JAM_RCPtr0<T>::operator=(other); }
   JAM_RCPtr(const JAM_RCPtr& other) : JAM_RCPtr0<T>(other) {}
protected:
   JAM_RCPtr(T* p, unsigned* r) : JAM_RCPtr0<T>(p,r) {}
};

template<class T> /*inline##*/
int operator==(const JAM_RCPtr<T>& a, const JAM_RCPtr<T>& b)
   { return a.ptr==b.ptr; }
//###   { return ((const JAM_RCPtr0<T>&)a).ptr==((const JAM_RCPtr0<T>&)b).ptr; }

template<class T, class B>
struct RCPtrHack {   //## BC++ 3.1 requires struct and function
   static int equal(const JAM_RCPtr<T>& a, const JAM_RCPtr<B>& b)
      { return (JAM_RCPtr<B>)a==b; }
};

template<class T, class B1>
struct JAM_RCPtr1 : public JAM_RCPtr0<T> {
   JAM_RCPtr1() {}
   JAM_RCPtr1(T* p) : JAM_RCPtr0<T>(p) {}
   JAM_RCPtr1(const JAM_RCPtr0<T>& other) : JAM_RCPtr0<T>(other) {}
   void operator=(T* p) { JAM_RCPtr0<T>::operator=(p); }
   operator JAM_RCPtr<B1>() const { return JAM_RCPtr0<B1>(ptr,refsp); }
   friend int operator==(const JAM_RCPtr<T>& a, const JAM_RCPtr<B1>& b)
//##  { return (JAM_RCPtr<B1>)a==b; }
      { return RCPtrHack<T,B1>::equal(a,b); }
   friend int operator==(const JAM_RCPtr<B1>& a, const JAM_RCPtr<T>& b)
      { return b==a; }
};
template<class T, class B1, class B2>
struct JAM_RCPtr2 : public JAM_RCPtr1<T,B2> {
   JAM_RCPtr2() {}
   JAM_RCPtr2(T* p) : JAM_RCPtr1<T,B2>(p) {}
   JAM_RCPtr2(const JAM_RCPtr0<T>& other) : JAM_RCPtr1<T,B2>(other) {}
   void operator=(T* p) { JAM_RCPtr0<T>::operator=(p); }
   operator JAM_RCPtr<B1>() const { return JAM_RCPtr0<B1>(ptr,refsp); }
   friend int operator==(const JAM_RCPtr<T>& a, const JAM_RCPtr<B1>& b)
      { return RCPtrHack<T,B1>::equal(a,b); }
   friend int operator==(const JAM_RCPtr<B1>& a, const JAM_RCPtr<T>& b)
      { return b==a; }
};
template<class T, class B1, class B2, class B3>
struct JAM_RCPtr3 : public JAM_RCPtr2<T,B2,B3> {
   JAM_RCPtr3() {}
   JAM_RCPtr3(T* p) : JAM_RCPtr2<T,B2,B3>(p) {}
   JAM_RCPtr3(const JAM_RCPtr0<T>& other) : JAM_RCPtr2<T,B2,B3>(other) {}
   void operator=(T* p) { JAM_RCPtr0<T>::operator=(p); }
   operator JAM_RCPtr<B1>() const { return JAM_RCPtr0<B1>(ptr,refsp); }
   friend int operator==(const JAM_RCPtr<T>& a, const JAM_RCPtr<B1>& b)
      { return RCPtrHack<T,B1>::equal(a,b); }
   friend int operator==(const JAM_RCPtr<B1>& a, const JAM_RCPtr<T>& b)
      { return b==a; }
};
template<class T, class B1, class B2, class B3, class B4>
struct JAM_RCPtr4 : public JAM_RCPtr3<T,B2,B3,B4> {
   JAM_RCPtr4() {}
   JAM_RCPtr4(T* p) : JAM_RCPtr3<T,B2,B3,B4>(p) {}
   JAM_RCPtr4(const JAM_RCPtr0<T>& other) : JAM_RCPtr3<T,B2,B3,B4>(other) {}
   void operator=(T* p) { JAM_RCPtr0<T>::operator=(p); }
   operator JAM_RCPtr<B1>() const { return JAM_RCPtr0<B1>(ptr,refsp); }
   friend int operator==(const JAM_RCPtr<T>& a, const JAM_RCPtr<B1>& b)
      { return RCPtrHack<T,B1>::equal(a,b); }
   friend int operator==(const JAM_RCPtr<B1>& a, const JAM_RCPtr<T>& b)
      { return b==a; }
};

#endif // JAM_RCPtr_H

