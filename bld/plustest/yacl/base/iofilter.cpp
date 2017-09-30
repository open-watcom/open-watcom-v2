



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





#include "base/iofilter.h"
#include "base/stream.h"


CL_ObjectPtr CL_ObjectIOFilter::RestoreFrom (const CL_Stream& s)
{
    CL_ObjectPtr p;
    return s.Read (p) ? p : NULL;
}

bool CL_ObjectIOFilter::RestoreFrom (const CL_Stream& s, CL_Object& o)
{
    return s.Read (o);
}

bool       CL_ObjectIOFilter::SaveTo (CL_Stream& s, const CL_Object& o) const
{
    return s.Write (o);
}


bool   CL_ObjectIOFilter::SaveTo (CL_Stream& s, CL_ObjectPtr o) const
{
    return s.Write (o);
}



template <class Base>
CL_Object* CL_IOFilter<Base>::RestoreFrom (const CL_Stream& s)
{
    Base* p = new Base; return p && p->ReadFrom (s);
}


template <class Base>
bool CL_IOFilter<Base>::RestoreFrom (const CL_Stream& s, CL_Object& o)
{
    return p.ReadFrom (s);
}


