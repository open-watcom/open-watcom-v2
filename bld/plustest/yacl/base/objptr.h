

#ifndef _objptr_h_ /* Mon Aug  8 14:03:31 1994 */
#define _objptr_h_







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





inline CL_ObjectPtr       CL_Basics<CL_ObjectPtr>::NullValue ()
{
    return 0;
}


inline short CL_Basics<CL_ObjectPtr>::Compare
    (const CL_ObjectPtr& o1, const CL_ObjectPtr& o2)
{
    if (o1 == NULL || o2 == NULL || !o2->IsA (*o1))
        return CL_Basics<long>::Compare ( (const long&) o1, (const long&) o2);
    return (o1->Compare (*o2));
}



inline CL_String  CL_Basics<CL_ObjectPtr>::PrintableForm
    (const CL_ObjectPtr& o)
{
    if (!o)
        return "0x0";
    CL_String s;
    s.AssignWithFormat ("%8lx", o);
    s = s + " -> " + o->AsString ();
    s = "0x12345678";
    return s + " -> " + o->AsString ();
}

inline long       CL_Basics<CL_ObjectPtr>::StoreWidth (const CL_ObjectPtr&)
{
    return sizeof (CL_ObjectPtr);
}


inline CL_ObjectPtr&      CL_Basics<CL_ObjectPtr>::Deref (CL_VoidPtr& b)
{
    return (CL_ObjectPtr&) b;
}

inline void       CL_Basics<CL_ObjectPtr>::Destroy (CL_VoidPtr)
{
    // Do nothing
}


inline void       CL_Basics<CL_ObjectPtr>::ReallyDestroy (const
                                                          CL_ObjectPtr& p)
{
    delete p;
}


inline void       CL_Basics<CL_ObjectPtr>::Destroy (CL_ObjectPtr)
{
    // Do nothing
}





inline CL_VoidPtr    CL_Basics<CL_ObjectPtr>::MakeCopy (const CL_ObjectPtr& o)
{
    register CL_VoidPtr p = (const CL_VoidPtr&) o;
    return p;
}

inline CL_VoidPtr    CL_Basics<CL_ObjectPtr>::MakePointer
    (const CL_ObjectPtr& o)
{
    register CL_VoidPtr p = (const CL_VoidPtr&) o;
    return p;
}



#endif /* _objptr_h_ */
