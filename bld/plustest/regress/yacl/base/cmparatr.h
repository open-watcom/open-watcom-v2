

#ifndef _cmparatr_h_ /* Wed Aug  3 16:17:51 1994 */
#define _cmparatr_h_




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


#include "base/basicops.h"

// An AbstractComparator encapsulates an algorithm for comparing two objects
// pointed to by void pointers. The comparison is done using the operator(),
// and returns a {\tt short} value that is -1, 0 or +1 according to whether
// the first operand is kess than, equal to or greater than the second.
//
// The default implementation uses comparison of pointer values.

class CL_AbstractComparator {

public:
    virtual short operator() (CL_VoidPtr p1, CL_VoidPtr p2) const
        {return CL_Basics<CL_VoidPtr>::Compare (p1, p2);};
    
};


// A Comparator is derived from an AbstractComparator, and is a template
// class used for comparing two objects of the type of its template class.

template <class Base>
class CL_Comparator: public CL_AbstractComparator {

public:
    virtual short operator() (CL_VoidPtr p1, CL_VoidPtr p2) const;

};


// BC++ 3.1 gives a strange "argument not used" warning here --
// looks like a compiler bug. The pragma's don't remove the warning. :-(
// That's the reason for the next two pragmas.

#ifdef __BORLANDC__
#pragma warn -par
#pragma warn -aus
#endif

template <class Base>
inline short CL_Comparator<Base>::operator() (CL_VoidPtr p1, CL_VoidPtr p2)
    const
{
    if (p1 && p2) {
        register const Base& r1 = CL_Basics<Base>::Deref (p1);
        register const Base& r2 = CL_Basics<Base>::Deref (p2);
        return CL_Basics<Base>::Compare (r1, r2);
    }
    return CL_Basics<CL_VoidPtr>::Compare (p1, p2);
}

// And if we undo the pragmas, the warnings are still emitted!! So we have
// to leave them out, so some legitimate warnings might not be emitted.
// Too bad.
//
// #ifdef __BORLANDC__
// #pragma warn .par
// #pragma warn .aus
// #endif




#endif /* _cmparatr_h_ */

