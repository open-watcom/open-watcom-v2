

#ifndef _genseq_h_ /* Thu Sep 15 09:56:37 1994 */
#define _genseq_h_






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




// This is a "generic" sequence of void pointers.


#include "base/sequence.h"
#include "base/cmparatr.h"

class CL_GenericSequence: public CL_Sequence <CL_VoidPtr> {

public:
    CL_GenericSequence (long initial_size = 0, CL_AbstractComparator*
                        cmp = NULL);
    // Create a generic sequence of size 0. The second parameter specifies
    // a comparator to be used for the sorting and searching methods; if
    // none is specified, {\tt CL_Basics <CL_VoidPtr> :: Compare} is used
    // for comparison. If one is given, it is assumed to be borrowed from
    // the caller of this constructor, and the latter must ensure that it
    // exists as long as this Sequence exists.

    CL_GenericSequence (const CL_GenericSequence& s);
    // Copy constructor.

    const char* ClassName () const {return "CL_GenericSequence";};

    CL_Object* Clone () const {return new CL_GenericSequence (*this);};

protected:
    short _Compare (const CL_VoidPtr&, const CL_VoidPtr&) const;
    // Override the inherited virtual method.

    virtual bool       _ReadElement (const CL_Stream&, long)
        {NotImplemented ("_ReadElement"); return FALSE;};
    
    virtual bool       _WriteElement (CL_Stream&, long) const
        {NotImplemented ("_WriteElement"); return FALSE;};

    CL_AbstractComparator* _cmp;
};


#endif /* _genseq_h_ */
