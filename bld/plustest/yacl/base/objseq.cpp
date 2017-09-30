



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

#include "base/objseq.h"
#include "base/stream.h"

CL_DEFINE_CLASS(CL_ObjectSequence, _CL_ObjectSequence_CLASSID);

#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
template class CL_Sequence<CL_ObjectPtr>;
#else
typedef CL_Sequence<CL_ObjectPtr> CL_PtrSeq;  // Don't use this: use the class
                                              // CL_ObjectSequence instead.
#endif


CL_ObjectSequence::CL_ObjectSequence (long initial_size ,
                   CL_ObjectIOFilter* bld)
: CL_Sequence<CL_ObjectPtr> (initial_size)
{
    _builder = bld;
}


CL_ObjectSequence::CL_ObjectSequence
    (const CL_ObjectPtr data[], long count, CL_ObjectIOFilter* builder)
: CL_Sequence<CL_ObjectPtr> (data, count)
{
    _builder = builder;
}

CL_ObjectSequence::CL_ObjectSequence (const CL_ObjectSequence& s)
: CL_Sequence<CL_ObjectPtr> (s)
{
    _builder = s._builder;
}


CL_ObjectSequence::~CL_ObjectSequence ()
{
}




void CL_ObjectSequence::DestroyContents ()
{
    if (!PrepareToChange())
        return;
    long n = Size();
    for (long i = 0; i < n; i++) {
        CL_ObjectPtr p = (*this)[i];
        if (p)
            delete p;
    }
    _size = 0;
    Notify ();
}


bool CL_ObjectSequence::ReadFrom (const CL_Stream& s)
{
//     if (!_builder || !ReadClassId (s) )
//         return FALSE;
//     if (!PrepareToChange())
//         return FALSE;
//     long size;
//     if (!s.Read (size))
//         return FALSE;
//     if (!ChangeSize (size))
//         return FALSE;
//     for (long i = 0; i < size; i++) {
//         CL_ObjectPtr p = _builder->BuildFrom (s);
//         if (!p)
//             return FALSE;
//         (*this)[i] = p;
//     }
//     Notify();
//     return TRUE;
    return CL_Sequence<CL_ObjectPtr>::ReadFrom (s);
}



