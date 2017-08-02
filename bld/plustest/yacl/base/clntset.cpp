



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




#include "base/clntset.h"


class CL_BindingElement: public CL_Object {

public:
    CL_BindingElement (const CL_AbstractBinding& bind, long parameter)
        :_bind ((CL_AbstractBinding*) bind.Clone()), _param (parameter) {};
    ~CL_BindingElement ()
        {delete _bind;};

    bool operator== (const CL_Object& o) const;
    const char* ClassName () const {return "CL_BindingElement";};


    CL_AbstractBinding* _bind;
    long                _param;
};


bool CL_BindingElement::operator== (const CL_Object& o) const
{
    if (!_bind)
        return FALSE;
    return (*_bind == *(((const CL_BindingElement&) o)._bind));
}



CL_ClientSet::~CL_ClientSet ()
{
    DestroyContents();
}


bool CL_ClientSet::Add (const CL_AbstractBinding& b, long p)
{
    CL_BindingElement* element = new CL_BindingElement (b, p);
    if (CL_ObjectSet::Add (element))
        return TRUE;
    else {
        delete element;
        return FALSE;
    }
}



bool CL_ClientSet::Remove (const CL_AbstractBinding& b)
{
    CL_BindingElement e (b, 0); // We don't care about the second
                                // parameter, since it's not used for
                                // comparison
    CL_BindingElement* element = (CL_BindingElement*)
        CL_ObjectSet::Remove (&e);
    if (element) {
        delete element;
        Notify ();
        return TRUE;
    }
    return FALSE;
}


bool CL_ClientSet::Includes (const CL_AbstractBinding& b) const
{
    CL_BindingElement e (b, 0);
    return CL_ObjectSet::Includes (&e);
}



static long __NullVal = 0;

long& CL_ClientSet::CodeFor (const CL_AbstractBinding& b) const
{
    CL_BindingElement e (b, 0);
    
    long i;
    CL_BindingElement* element = (CL_BindingElement*) Find (&e);
    if (!element) {
        __NullVal = 0;
        return __NullVal;
    }
    return element->_param;
}


void CL_ClientSet::NotifyAll (CL_Object& source) const
{
    CL_BindingElement* e;
    long n = Size();
    for (short i = 0; i < n; i++) {
        e = (CL_BindingElement*) ItemWithRank (i);
        e->_bind->Execute (source, e->_param);
    }
}


bool CL_ClientSet::Permits (CL_Object& source) const
{
    CL_BindingElement* e;
    long n = Size();
    // Stop after the first FALSE return
    for (short i = 0; i < n; i++) {
        e = (CL_BindingElement*) ItemWithRank (i);
        if (!e->_bind->Execute (source, e->_param))
            return FALSE;
    }
    return TRUE;
}


