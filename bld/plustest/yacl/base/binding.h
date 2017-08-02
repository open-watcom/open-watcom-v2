

#ifndef _binding_h_ /* Mon Sep 20 08:44:42 1993 */
#define _binding_h_





/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */



// A "binding" is simply an object-method pair. The method denoted by a
// binding must have signature
// \par\begin{verbatim}
//        bool Method (CL_Object&, long)
// \end{verbatim}
//
// \noindent
// Bindings are used for notification and inter-object communication. An
// abstract binding (class CL_AbstractBinding) describes the protocol for
// a binding; the derived template class CL_Binding is used to build
// bindings for each class that needs one.
//

#include "base/object.h"

#ifdef __GNUC__
#pragma implementation
#endif


class CL_AbstractBinding: public CL_Object {

public:

    virtual bool Execute (CL_Object& o, long l) const;
    // Execute the binding; that is, invoke our contained method on the
    // object we point to, with parameters o and l. Return the return
    // value of the invoked method.

    virtual bool Valid () const;
    // Return whether this is a valid binding, i.e., whether both the
    // contained object pointer and method pointer are non-null.

    virtual const  char* ClassName() const;

};


// This is a template-based class that describes an object-method pair,
// for an object of type {\tt Base}, the type parameter.

template <class Base>
class CL_Binding: public CL_AbstractBinding {

public:
    typedef bool (Base::*MethodPtr) (CL_Object&, long);

    //  ---------------------- Construction ----------------------
    
    CL_Binding (Base* obj, MethodPtr method)
        {_obj = obj;  _method = method;};
    // Construct a binding for an object of type {\tt Base}, that has a
    // method {\tt method}.

    CL_Binding ()
        { _obj = 0; _method = 0;};
    // Construct a null binding, one whose object and method pointers are
    // both NULL, for an object of type Base.

    CL_Binding (const CL_Binding<Base>& b)
        {_obj = b._obj; _method = b._method;};
    // Copy constructor.

    bool Valid () const { return _obj != 0 && _method != 0; };
    // Return TRUE if both object and method pointers of this binding are
    // non-NULL.
    
    bool Execute (CL_Object& o, long v) const
        { return (_obj != 0 && _method != 0)
                ? (_obj->*_method) (o, v) : FALSE; };
    // Override the virtual method inherited from {\tt AbstractBinding}.

    void operator= (const CL_Object& o)
        {*this = ((const CL_Binding<Base>&) o);};
    
    CL_Binding<Base>& operator= (const CL_Binding<Base>& o)
        {_obj = o._obj; _method = o._method; return *this;};
    
    bool operator== (const CL_Object& o) const
        {return *this == ((const CL_Binding<Base>&) o);};
    // Cast the given object {\tt o} down to a Binding, and return TRUE if the
    // pointers in it are the same as the pointers in this object. 
    
    bool operator== (const CL_Binding<Base>& o) const
        { return _obj == o._obj && _method == o._method; };

    CL_Object* Clone () const { return new CL_Binding<Base> (_obj, _method);};
    // Override the method inherited from {\tt CL_Object}.

    const  char* ClassName () const {return "CL_TemplatedBinding";};
    
protected:
    Base* _obj;
    MethodPtr _method;
};





// #ifndef __GNUC__
// // template <class Base>
// // typedef bool (Base::*MethodPtr) (CL_Object&, long);
// 
// template <class Base>
// #ifdef __GNUC__
// static
// #else
// inline
// #endif
// CL_Binding<Base> MakeBinding (Base* o, CL_Binding<Base>::MethodPtr m)
// {
//     return CL_Binding<Base> (o, m);
// }
// #endif /* __GNUC__ */

inline const char* CL_AbstractBinding::ClassName() const
{
    return "CL_AbstractBinding";
}

inline bool CL_AbstractBinding::Execute (CL_Object&, long) const
{
    NotImplemented ("Execute"); return FALSE;
}

inline bool CL_AbstractBinding::Valid () const
{
    NotImplemented ("Valid"); return FALSE;
}





#endif
