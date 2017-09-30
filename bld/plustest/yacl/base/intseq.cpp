



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





#if !defined(__BORLANDC__)
#include "base/seqimp.cpp"
// Don't include this under Borland C because the CL_Sequence<long> is
// instantiated indirectly in the CL_Sequence<CL_Object*> class.
#endif

#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
template class CL_Sequence<long>;
#endif

#include "base/intseq.h"

CL_DEFINE_CLASS(CL_IntegerSequence, _CL_IntegerSequence_CLASSID);


CL_IntegerSequence::CL_IntegerSequence (long initial_size)
: CL_Sequence<long> (initial_size)
{
}


CL_IntegerSequence::CL_IntegerSequence (long data[], long count)
{
    for (register long i = 0; i < count; i++)
        Add (data[i]);
}


CL_IntegerSequence::CL_IntegerSequence (const CL_Sequence<long>& s)
: CL_Sequence<long> (s)
{
}

