/****************************************************************************
File: Functor0.h

Description:
   Defines the Functor0<R> class template and Functor0v class and the
   functor(O*, R (O::*)()) and functorv(O*, void (O::*)()) function
   templates which create the Functor objects.  These are Functors
   taking zero parameters.  See Functor1.h for a description of Functors.

Usage:
   See Functor1.h

Notes:
   See Functor1.h.  Functor0v is not a template.

History:
14 Feb 1992 Jam      created from Functor1.h -- replaced "{, }@class P1",
                     "P1 p1", "{, }P1>", "P1)", "p1"
19 Feb 1992 Jam      renamed functorN() back to functor(), overloadable:-)
20 Mar 1992 Jam      added null() and void* conversion: `if (f1)'
20 Mar 1992 Jam      restructured -- Functor1 now contains ptr
08 Nov 1992 Jam      renamed JAM_*, made to use <CRCPtr> instead of <SmartPtr>
   
****************************************************************************/     
#ifndef JAM_Functor0_H
#define JAM_Functor0_H

#include <jam/crcptr.h>

//**************************************************************************
// Defines JAM_Functor0<R>, functor(O*, R (O::*)()) and helper classes
//**************************************************************************

   template<class R>
class JAM_AbstractFunctor0 : public JAM_ReferenceCounter {
public:
   virtual R call() const = 0;
   virtual int null() const = 0;
};

   template<class R>
class JAM_Functor0 {
   JAM_CRCPtr< JAM_AbstractFunctor0<R> > _ptr;
public:
   JAM_Functor0(JAM_AbstractFunctor0<R>* ptr = 0) : _ptr(ptr) {}
   R operator()() const { return _ptr->call(); }
   operator const void*() const
      { return (_ptr==0 || _ptr->null()) ? 0 : this; }
};

   template<class O, class R>
class JAM_ConcreteFunctor0 : public JAM_AbstractFunctor0<R> {
public:
   JAM_ConcreteFunctor0(O* object, R (O::*method)())
      : _object(object), _method(method) {}
   virtual R call() const { return (_object->*_method)(); }
   virtual int null() const { return _object==0 || _method==0; }
protected:
   O* _object;
   R (O::*_method)();
};

   template<class O, class R> inline
JAM_Functor0<R> functor(O* object, R (O::*method)()) {
   return new JAM_ConcreteFunctor0<O, R>(object, method);
}


//**************************************************************************
// Defines JAM_Functor0v, functorv(O*, void (O::*)()) and helper classes
//**************************************************************************

class JAM_AbstractFunctor0v : public JAM_ReferenceCounter {
public:
   virtual void call() const = 0;
   virtual int null() const = 0;
};

class JAM_Functor0v {
   JAM_CRCPtr< JAM_AbstractFunctor0v > _ptr;
public:
   JAM_Functor0v(JAM_AbstractFunctor0v* ptr = 0) : _ptr(ptr) {}
   void operator()() const { _ptr->call(); }
   operator const void*() const
      { return (_ptr==0 || _ptr->null()) ? 0 : this; }
};

   template<class O>
class JAM_ConcreteFunctor0v : public JAM_AbstractFunctor0v {
public:
   JAM_ConcreteFunctor0v(O* object, void (O::*method)())
      : _object(object), _method(method) {}
   virtual void call() const { (_object->*_method)(); }
   virtual int null() const { return _object==0 || _method==0; }
protected:
   O* _object;
   void (O::*_method)();
};

   template<class O> inline
JAM_Functor0v functorv(O* object, void (O::*method)()) {
   return new JAM_ConcreteFunctor0v<O>(object, method);
}


#endif // JAM_Functor0_H

