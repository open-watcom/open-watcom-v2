



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





#include "base/setimp.cpp"


#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
template class CL_Set<CL_ObjectPtr>;
template class CL_SetIterator<CL_ObjectPtr>;
#endif


#include "base/objset.h"
#include "base/objseq.h"



CL_DEFINE_CLASS(CL_ObjectSet, _CL_ObjectSet_CLASSID);


CL_ObjectSet::CL_ObjectSet (CL_ObjectIOFilter* bld)
: CL_Set<CL_ObjectPtr> (new CL_ObjectSequence (0, bld))
{
}


CL_ObjectSet::CL_ObjectSet (const CL_ObjectSet& s)
: CL_Set<CL_ObjectPtr> (s)
{
}


CL_ObjectPtr CL_ObjectSet::Find (CL_ObjectPtr p) const
{
    if (!_idata)
        return NULL;
    long l;
    CL_ObjectSequence& _data = * (CL_ObjectSequence*) _idata;
    return _data.BinarySearch (p, l) ? _data[l] : NULL;
}


void CL_ObjectSet::DestroyContents ()
{
    if (_idata)
        ((CL_ObjectSequence*) _idata)->DestroyContents();
}

