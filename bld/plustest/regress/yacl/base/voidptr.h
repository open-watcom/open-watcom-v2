

#ifndef _voidptr_h_ /* Mon Aug  8 14:02:32 1994 */
#define _voidptr_h_





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




// Template specializations for the type-specific "basic operations" on
// the CL_VoidPtr type.

inline CL_VoidPtr       CL_Basics<CL_VoidPtr>::NullValue ()
{
    return 0;
}
inline short      CL_Basics<CL_VoidPtr>::Compare (const CL_VoidPtr& o1,
                                           const CL_VoidPtr& o2)
{ 
    return o1 < o2 ? -1 : (o1 == o2 ? 0 : 1);
}

inline CL_String  CL_Basics<CL_VoidPtr>::PrintableForm (const CL_VoidPtr& o)
{
    CL_String s;
    s.AssignWithFormat ("%lx", o);
    return s;
}

inline long       CL_Basics<CL_VoidPtr>::StoreWidth (const CL_VoidPtr&)
{
    return sizeof (CL_VoidPtr);
}

inline void       CL_Basics<CL_VoidPtr>::Destroy (CL_VoidPtr)
{
    // Do nothing
}


inline void       CL_Basics<CL_VoidPtr>::Destroy (CL_ObjectPtr)
{
}

inline void       CL_Basics<CL_VoidPtr>::ReallyDestroy (const CL_VoidPtr&)
{
}

inline CL_VoidPtr& CL_Basics<CL_VoidPtr>::Deref (CL_VoidPtr& p)
{
    return p;
}


inline CL_VoidPtr    CL_Basics<CL_VoidPtr>::MakeCopy (const CL_VoidPtr& o)
{
    return o;
}

inline CL_VoidPtr    CL_Basics<CL_VoidPtr>::MakePointer (const CL_VoidPtr& o)
{
    return o;
}



#endif /* _voidptr_h_ */
