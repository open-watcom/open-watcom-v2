



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
#include "base/seqimp.cpp"    // Include this first, to appease GNU C++

// This is not included under BC++ because it implicitly instantiates
// CL_Sequence<long> here as well as in strgseq.obj, causing multiple
// definitions.
#endif

#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
template class CL_Sequence<CL_String>;
#endif

#include "base/strgseq.h"



CL_DEFINE_CLASS(CL_StringSequence, _CL_StringSequence_CLASSID);


CL_StringSequence::CL_StringSequence (long initial_size)
: CL_Sequence<CL_String> (initial_size)
{
}

CL_StringSequence::CL_StringSequence (const CL_String data[], long count)
: CL_Sequence<CL_String> (data, count)
{
}


CL_StringSequence::CL_StringSequence (const char* data[], long count)
{
    for (register long i = 0; i < count && data[i] != NULL; i++)
        Add (data[i]);
}


CL_StringSequence::CL_StringSequence (const CL_Sequence<CL_String>& s)
: CL_Sequence<CL_String> (s)
{
}

