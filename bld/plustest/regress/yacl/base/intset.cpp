



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
template class CL_Set<long>;
template class CL_SetIterator<long>;
#endif

#include "base/string.h"
#include "base/intset.h"
#include "base/sequence.h"




CL_DEFINE_CLASS(CL_IntegerSet, _CL_IntegerSet_CLASSID);


CL_IntegerSet::CL_IntegerSet (long lo, long hi)
{
    for (long i = lo; i <= hi; i++)
        Add (i);
}


CL_String CL_IntegerSet::AsString() const
{
    if (!_idata)
        return  "{}";
    CL_String s ("{");
    long n = Size();
    for (long i = 0; i < n; i++) {
        s += CL_String (ItemWithRank (i));
        if (i < n-1)
            s += ", ";
    }
    s += "}";
    return s;
}

long CL_IntegerSet::SmallestNonMember () const
{
    long n = Size();
    if (n <= 0)
        return -1;
    long i;
    for (i = 0; i < n; i++)
        if (ItemWithRank(i) != i)
            break;
    return i;
}



