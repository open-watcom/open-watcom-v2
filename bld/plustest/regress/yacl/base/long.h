

#ifndef _long_h_ /* Mon Aug  8 14:03:07 1994 */
#define _long_h_





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

inline long  CL_Basics<long>::NullValue ()
{
    return 0;
}

inline short      CL_Basics<long>::Compare (const long& o1, const long&
                                            o2)
{
    return o1 < o2 ? -1 : (o1 == o2 ? 0 : 1);
}

inline CL_String  CL_Basics<long>::PrintableForm (const long& o)
{
    return CL_String (o);
}

inline long       CL_Basics<long>::StoreWidth (const long& l)
{
    return sizeof l;
}


inline void       CL_Basics<long>::Destroy (CL_VoidPtr)
{
    // Do nothing
}


inline void       CL_Basics<long>::Destroy (CL_ObjectPtr)
{
}

inline void       CL_Basics<long>::ReallyDestroy (const long&)
{
}


inline long& CL_Basics<long>::Deref (CL_VoidPtr& p)
{
    return (long&) p;
}


inline CL_VoidPtr    CL_Basics<long>::MakeCopy (const long& o)
{
    register CL_VoidPtr p = (const CL_VoidPtr&) o;
    return p;
}

inline CL_VoidPtr    CL_Basics<long>::MakePointer (const long& o)
{
    register CL_VoidPtr p = (const CL_VoidPtr&) o;
    return p;
}



#endif /* _long_h_ */
