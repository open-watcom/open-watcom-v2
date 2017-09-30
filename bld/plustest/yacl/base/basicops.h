

#ifndef _basicops_h_ /* Sun May 22 12:44:53 1994 */
#define _basicops_h_




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




// The "basic operations" class provides inline functions for primitive
// operations on data types, namely null values, comparison and archival.


// #ifdef __GNUC__
// #pragma implementation
// #endif


#include "base/defs.h"
#include "base/string.h"

class CL_ObjectIOFilter;

template <class Base>
class CL_Basics {

public:
    static Base       NullValue () ;

    static short      Compare (const Base& o1, const Base& o2);

    static CL_String  PrintableForm (const Base& o) ;

    static long       StoreWidth (const Base& o) ;

//     static bool       RestoreFrom (Base& b, const CL_Stream& s,
//                                    CL_ObjectIOFilter* f);
//     
//     static bool       SaveTo      (const Base& b, CL_Stream& s,
//                                    CL_ObjectIOFilter* f);

    static Base&      Deref (CL_VoidPtr& b) ;
    
    static void       Destroy (CL_VoidPtr p);

    static void       Destroy (CL_ObjectPtr p);

    static void       ReallyDestroy (const Base&);

    static CL_VoidPtr    MakeCopy (const Base& o) ;

    static CL_VoidPtr    MakePointer (const Base& o) ;

};



// Warning: Place the template specialization functions textually BEFORE the
// general template-based definitions (e.g., CL_Basics<long>::Compare is
// defined BEFORE template <class Base> CL_Basics<Base>::Compare), otherwise
// Borland C++ complains!!

// First, include the template specializations for these static methods:

#include "base/voidptr.h"
#include "base/long.h"
#include "base/objptr.h"

// Now the real template methods:

template <class Base>
inline Base       CL_Basics<Base>::NullValue ()
{
    Base p; return p;
}

inline short CL_Basics<CL_Object>::Compare
    (const CL_Object& o1, const CL_Object& o2)
{
    if (!o1.IsA (o2))
        return &o1 < &o2 ? -1 : 1;
    return o1.Compare (o2);
}
    

template <class Base>
inline short CL_Basics<Base>::Compare (const Base& o1, const Base& o2)
{
    return o1.Compare (o2);
}
    


template <class Base>
inline CL_String      CL_Basics<Base>::PrintableForm (const Base& o) 
{
    return o.AsString ();
}

template <class Base>
inline long       CL_Basics<Base>::StoreWidth (const Base& o) 
{
    return o.StorableFormWidth();
}

template <class Base>
inline void       CL_Basics<Base>::Destroy (CL_VoidPtr p)
{
    if (p) delete (Base*) p;
}


template <class Base>
inline void       CL_Basics<Base>::Destroy (CL_ObjectPtr p)
{
    if (p) delete (Base*) p;
}

template <class Base>
inline void       CL_Basics<Base>::ReallyDestroy (const Base&)
{
}

    



template <class Base>
inline CL_VoidPtr    CL_Basics<Base>::MakeCopy (const Base& o) 
{
    return  (CL_VoidPtr) new Base (o);
}

template <class Base>
inline CL_VoidPtr    CL_Basics<Base>::MakePointer (const Base& o)
{
    return (CL_VoidPtr) &o;
}


template <class Base>
inline Base&      CL_Basics<Base>::Deref (CL_VoidPtr& b)
{
    return *(Base*&) b;
}








// ---------------------- Storage and retrieval operations ---------



bool CL_RestoreFrom (long& b, const CL_Stream& s, CL_ObjectIOFilter* );

bool CL_RestoreFrom (CL_ObjectPtr& b, const CL_Stream& s, 
                     CL_ObjectIOFilter* f = 0);

bool CL_RestoreFrom (CL_Object& b, const CL_Stream& s,
                     CL_ObjectIOFilter* f = 0);

inline bool CL_RestoreFrom (CL_VoidPtr&, const CL_Stream&,
                            CL_ObjectIOFilter*)
{
    return FALSE; // Do nothing
}



bool CL_SaveTo (const CL_Object& b, CL_Stream& s,
                CL_ObjectIOFilter* f = 0);

bool CL_SaveTo (const long& b, CL_Stream& s,
                CL_ObjectIOFilter*  = 0);

bool CL_SaveTo (const CL_ObjectPtr& b, CL_Stream& s,
                CL_ObjectIOFilter* f = 0);

inline bool CL_SaveTo (const CL_VoidPtr&, CL_Stream&,
                       CL_ObjectIOFilter* = 0)
{
    return FALSE; // Do nothing
}






#endif /* _basicops_h_ */
