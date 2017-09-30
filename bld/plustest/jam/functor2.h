/****************************************************************************
File: Functor2.h

Description:
   Defines the Functor2<R, P1, P2> and Functor2v<P1, P2> class
   templates and the functor(O*, R (O::*)(P1, P2)) and
   functorv(O*, void (O::*)(P1, P2)) function templates which create
   the Functor objects.  These are Functors taking two parameters
   See Functor1.h for a description of Functors.


Usage:
   See Functor1.h

Notes:
   See Functor1.h

History:
14 Feb 1992 Jam      created from Functor1.h -- replaced "class P1",
                     "P1 p1", "P1>", "P1)", "p1)"
14 Feb 1992 Jam      had to rename functor[v]() functor2[v]()
19 Feb 1992 Jam      renamed functorN() back to functor(), overloadable:-)
20 Mar 1992 Jam      added null() and void* conversion: `if (f1)'
20 Mar 1992 Jam      restructured -- Functor1 now contains ptr
08 Nov 1992 Jam      renamed JAM_*, made to use <CRCPtr> instead of <SmartPtr>

****************************************************************************/     
#ifndef JAM_Functor2_H
#define JAM_Functor2_H

#include <jam/crcptr.h>

//**************************************************************************
// Defines JAM_Functor2<R, P1, P2>, functor(O*, R (O::*)(P1, P2)) and helper classes
//**************************************************************************

   template<class R, class P1, class P2>
class JAM_AbstractFunctor2 : public JAM_ReferenceCounter {
public:
   virtual R call(P1 p1, P2 p2) const = 0;
   virtual int null() const = 0;
};

   template<class R, class P1, class P2>
class JAM_Functor2 {
   JAM_CRCPtr< JAM_AbstractFunctor2<R, P1, P2> > _ptr;
public:
   JAM_Functor2(JAM_AbstractFunctor2<R, P1, P2>* ptr = 0) : _ptr(ptr) {}
   R operator()(P1 p1, P2 p2) const { return _ptr->call(p1, p2); }
   operator const void*() const
      { return (_ptr==0 || _ptr->null()) ? 0 : this; }
};

   template<class O, class R, class P1, class P2>
class JAM_ConcreteFunctor2 : public JAM_AbstractFunctor2<R, P1, P2> {
public:
   JAM_ConcreteFunctor2(O* object, R (O::*method)(P1, P2))
      : _object(object), _method(method) {}
   virtual R call(P1 p1, P2 p2) const { return (_object->*_method)(p1, p2); }
   virtual int null() const { return _object==0 || _method==0; }
protected:
   O* _object;
   R (O::*_method)(P1 p1, P2 p2);
};

   template<class O, class R, class P1, class P2> inline
JAM_Functor2<R, P1, P2> functor(O* object, R (O::*method)(P1, P2)) {
   return new JAM_ConcreteFunctor2<O, R, P1, P2>(object, method);
}


//**************************************************************************
// Defines JAM_Functor2v<P1, P2>, functorv(O*, void (O::*)(P1, P2)) and helper classes
//**************************************************************************

   template<class P1, class P2>
class JAM_AbstractFunctor2v : public JAM_ReferenceCounter {
public:
   virtual void call(P1 p1, P2 p2) const = 0;
   virtual int null() const = 0;
};

   template<class P1, class P2>
class JAM_Functor2v {
   JAM_CRCPtr< JAM_AbstractFunctor2v<P1, P2> > _ptr;
public:
   JAM_Functor2v(JAM_AbstractFunctor2v<P1, P2>* ptr = 0) : _ptr(ptr) {}
   void operator()(P1 p1, P2 p2) const { _ptr->call(p1, p2); }
   operator const void*() const
      { return (_ptr==0 || _ptr->null()) ? 0 : this; }
};

   template<class O, class P1, class P2>
class JAM_ConcreteFunctor2v : public JAM_AbstractFunctor2v<P1, P2> {
public:
   JAM_ConcreteFunctor2v(O* object, void (O::*method)(P1, P2))
      : _object(object), _method(method) {}
   virtual void call(P1 p1, P2 p2) const { (_object->*_method)(p1, p2); }
   virtual int null() const { return _object==0 || _method==0; }
protected:
   O* _object;
   void (O::*_method)(P1 p1, P2 p2);
};

   template<class O, class P1, class P2> inline
JAM_Functor2v<P1, P2> functorv(O* object, void (O::*method)(P1, P2)) {
   return new JAM_ConcreteFunctor2v<O, P1, P2>(object, method);
}


#endif // JAM_Functor2_H

