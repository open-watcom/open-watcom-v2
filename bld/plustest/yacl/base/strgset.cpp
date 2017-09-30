



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





#if defined(__GNUC__)
#pragma implementation
#endif


#include "base/setimp.cpp"

template class CL_Set<CL_String>;
template class CL_SetIterator<CL_String>;


typedef CL_Sequence<CL_String> StringSequence;

#include "base/strgset.h"

CL_StringSet CL_StringSet::StringsWithPrefix (const CL_String& s) const
{
    CL_StringSet ret;
    if (!_idata)
        return ret;
    StringSequence& _data = (* (StringSequence*) _idata);
    long pos = 0;
    bool b = _data.BinarySearch (s, pos);
    register long n = _data.Size();
    for (register long i = b ? pos : pos+1; i < n; i++) {
        if (!s.IsPrefixOf (_data[i]))
            break;
        ret.Add (_data[i]);
    }
    return ret;
}


