



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

#include "base/basicops.h"
#include "base/iofilter.h"
#include "base/stream.h"



bool CL_RestoreFrom (long& b, const CL_Stream& s, CL_ObjectIOFilter* )
{
    return s.Read (b);
}


bool CL_RestoreFrom (CL_ObjectPtr& b, const CL_Stream& s, CL_ObjectIOFilter* f)
{
    if (f) {
        b = f->RestoreFrom (s);
        return b != NULL;
    }
    return s.Read (b);
}




bool CL_RestoreFrom (CL_Object& b, const CL_Stream& s, CL_ObjectIOFilter* f)
{
    return f ? f->RestoreFrom (s, b) : s.Read (b);
}



bool CL_SaveTo (const CL_Object& b, CL_Stream& s, CL_ObjectIOFilter* f)
{
    return f ? f->SaveTo (s, b) : s.Write (b);
}



bool CL_SaveTo (const long& b, CL_Stream& s, CL_ObjectIOFilter*)
{
    return  s.Write (b);
}


bool CL_SaveTo (const CL_ObjectPtr& b, CL_Stream& s,
                CL_ObjectIOFilter* f)
{
    return f ? f->SaveTo (s, b) : s.Write (b);
}



