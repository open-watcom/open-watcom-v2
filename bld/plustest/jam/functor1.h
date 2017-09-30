/****************************************************************************
File: Functor1.h

Description:
   Defines Functor1<R, P1> and Functor1v<P1> class templates and
   functor(O*, R (O::*)(P1)) and functorv(O*, void (O::*)(P1)) function
   templates which create Functor objects.  A Functor is used as if
   it were a pointer to a function taking a P1 parameter and returning
   an R.  Functor1v is just like Functor1 except it returns void
   (there is no R).
   
   Functors actually contain a pointer to an object and a pointer to one
   of the object's member functions.  These two pointers are specified
   when calling functor().  The member function must take a parameter P1
   and returns R.  When the Functor is executed, that member function is
   called on the object.  Since the code using a Functor only has to
   specify the type of the member function parameters and return type,
   not the type of the object actually receiving the message, using
   Functors makes the code more general.


Usage:
   class Foo {
   public:
      int addstr(const String& newstr);
      void scroll(double percent);
   } afoo;
   Functor1<int, const String&> f1 = functor(&afoo, &Foo::addstr);
   if ( f1("new entry")==0 ) cerr << "Couldn't add string.\n";
   Functor1v<double> f2 = functorv(&afoo, &Foo::scroll);
   f2(0.5);    // scroll Foo by 50%
   Functor1v<double> f3 = 0;
   if (f3!=0) f3(1.23);

Notes:
   Below, the template parameter O is the type of the object whose
   member function will be called.  The return type of the member
   function is R (but there is none for Functor1v).  The member
   function takes one parameter of type P1.

History:
12 Feb 1992 Jam      created; referenced Coplien
13 Feb 1992 Jam      added operator() functionality to a derived SmartPtr
13 Feb 1992 Jam      added Functor1v for functors returning void (no R)
14 Feb 1992 Jam      renamed functor[v]() functor1[v]() because of Functor2.h
19 Feb 1992 Jam      renamed functorN() back to functor(), overloadable:-)
20 Mar 1992 Jam      added null() and void* conversion: `if (f1)'
20 Mar 1992 Jam      restructured -- Functor1 now contains a ptr
                     instead of inheriting from it
08 Nov 1992 Jam      renamed JAM_*, made to use <CRCPtr> instead of <SmartPtr>
   
****************************************************************************/     
#ifndef JAM_Functor1_H
#define JAM_Functor1_H

#include <jam/crcptr.h>

//**************************************************************************
// Defines JAM_Functor1<R, P1>, functor(O*, R (O::*)(P1)) and helper classes
//**************************************************************************

   template<class R, class P1>
class JAM_AbstractFunctor1 : public JAM_ReferenceCounter {
public:
   virtual R call(P1 p1) const = 0;
   virtual int null() const = 0;
};

   template<class R, class P1>
class JAM_Functor1 {
   JAM_CRCPtr< JAM_AbstractFunctor1<R, P1> > _ptr;
public:
   JAM_Functor1(JAM_AbstractFunctor1<R, P1>* ptr = 0) : _ptr(ptr) {}
   R operator()(P1 p1) const { return _ptr->call(p1); }
   operator const void*() const
      { return (_ptr==0 || _ptr->null()) ? 0 : this; }
   //## explicit defs should not be necessary
   JAM_Functor1& operator=(const JAM_Functor1& f) { _ptr = f._ptr; return *this; }
};

   template<class O, class R, class P1>
class JAM_ConcreteFunctor1 : public JAM_AbstractFunctor1<R, P1> {
public:
   JAM_ConcreteFunctor1(O* object, R (O::*method)(P1))
      : _object(object), _method(method) {}
   virtual R call(P1 p1) const { return (_object->*_method)(p1); }
   virtual int null() const { return _object==0 || _method==0; }
protected:
   O* _object;
   R (O::*_method)(P1 p1);
};

   template<class O, class R, class P1> inline
JAM_Functor1<R, P1> functor(O* object, R (O::*method)(P1)) {
   return new JAM_ConcreteFunctor1<O, R, P1>(object, method);
}


//**************************************************************************
// Defines JAM_Functor1v<P1>, functorv(O*, void (O::*)(P1)) and helper classes
//**************************************************************************

   template<class P1>
class JAM_AbstractFunctor1v : public JAM_ReferenceCounter {
public:
   virtual void call(P1 p1) const = 0;
   virtual int null() const = 0;
};

   template<class P1>
class JAM_Functor1v {
   JAM_CRCPtr< JAM_AbstractFunctor1v<P1> > _ptr;
public:
   JAM_Functor1v(JAM_AbstractFunctor1v<P1>* ptr) : _ptr(ptr) {}
   void operator()(P1 p1) const { _ptr->call(p1); }
   operator const void*() const
      { return (_ptr==0 || _ptr->null()) ? 0 : this; }
};

   template<class O, class P1>
class JAM_ConcreteFunctor1v : public JAM_AbstractFunctor1v<P1> {
public:
   JAM_ConcreteFunctor1v(O* object, void (O::*method)(P1))
      : _object(object), _method(method) {}
   virtual void call(P1 p1) const { (_object->*_method)(p1); }
   virtual int null() const { return _object==0 || _method==0; }
protected:
   O* _object;
   void (O::*_method)(P1 p1);
};

   template<class O, class P1> inline
JAM_Functor1v<P1> functorv(O* object, void (O::*method)(P1)) {
   return new JAM_ConcreteFunctor1v<O, P1>(object, method);
}


#endif // JAM_Functor1_H

