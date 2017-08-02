



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





#ifndef __BORLANDC__
#include "base/seqimp.cpp"
#endif

#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
template class CL_Sequence<CL_VoidPtr>;
#endif

#include "base/genseq.h"


CL_GenericSequence::CL_GenericSequence (long initial_size,
                                        CL_AbstractComparator*  cmp)
: CL_Sequence<CL_VoidPtr> (initial_size), _cmp (cmp)
{
}

CL_GenericSequence::CL_GenericSequence (const CL_GenericSequence& s)
: CL_Sequence<CL_VoidPtr> (s)
{
    _cmp = s._cmp;
}



short CL_GenericSequence::_Compare (const CL_VoidPtr& p1, const CL_VoidPtr& p2)
    const
{
    return _cmp ? (*_cmp) (p1, p2) : CL_Basics<CL_VoidPtr>::Compare (p1, p2);
}

