

#ifndef _mapdef_h_ /* Thu Nov  4 12:27:19 1993 */
#define _mapdef_h_





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





// Maps:


// #include "base/string.h"
// #include "base/integer.h"

//  IntStringMap is a map with {\tt long} keys and Strings as values.

class CL_IntStringMap: public CL_Map<long, CL_String> {

public:
    CL_IntStringMap () {};

    CL_IntStringMap (CL_MapAssoc<long, CL_String>
                     a[], long cnt) : CL_Map<long, CL_String> (a, cnt) {};
    
    const char* ClassName () const {return "CL_IntStringMap";};

    CL_ClassId ClassId () const {return _CL_IntStringMap_CLASSID;};

    CL_Object* Clone () const {return new CL_IntStringMap (*this); };

};

typedef CL_MapAssoc<long, CL_String> CL_IntStringAssoc;
typedef CL_MapIterator<long, CL_String> CL_IntStringMapIterator;




// IntIntMap maps {\tt long} values to {\tt long} values.



class CL_IntIntMap: public CL_Map<long, long> {

public:
    CL_IntIntMap () {};

    CL_IntIntMap (CL_MapAssoc<long, long> a[], long cnt) : CL_Map<long, long>
    (a, cnt) {};

    const char* ClassName () const {return "CL_IntIntMap";};

    CL_ClassId ClassId () const {return _CL_IntIntMap_CLASSID;};

    CL_Object* Clone () const {return new CL_IntIntMap (*this); };

};


typedef CL_MapAssoc<long, long> CL_IntIntAssoc;
typedef CL_MapIterator<long, long> CL_IntIntMapIterator;

//  IntPtrMap has {\tt long} as its key type, and the corresponding values
//  are pointers to {\tt CL_Object}.



class CL_IntPtrMap: public CL_Map<long, CL_ObjectPtr> {

public:
    CL_IntPtrMap () {};

    CL_IntPtrMap (CL_MapAssoc<long, CL_ObjectPtr>
                  a[], long cnt) : CL_Map<long, CL_ObjectPtr> (a, cnt) {};
    
    const char* ClassName () const {return "CL_IntPtrMap";};

    CL_ClassId ClassId () const {return _CL_IntPtrMap_CLASSID;};

    CL_Object* Clone () const {return new CL_IntPtrMap (*this); };

};


typedef CL_MapAssoc<long, CL_ObjectPtr> CL_IntPtrAssoc;
typedef CL_MapIterator<long, CL_ObjectPtr> CL_IntPtrMapIterator;


// The StringStringMap maps Strings to Strings.


class CL_StringStringMap: public CL_Map<CL_String, CL_String> {

public:
    CL_StringStringMap () {};

    CL_StringStringMap (CL_MapAssoc<CL_String, CL_String> a[], long cnt) :
    CL_Map<CL_String, CL_String> (a, cnt) {};
    
    const char* ClassName () const {return "CL_StringStringMap";};

    CL_ClassId ClassId () const {return _CL_StringStringMap_CLASSID;};

    CL_Object* Clone () const {return new CL_StringStringMap (*this); };

};


typedef CL_MapAssoc<CL_String, CL_String> CL_StringStringAssoc;
typedef CL_MapIterator<CL_String, CL_String> CL_StringStringMapIterator;



// StringIntMap maps Strings to {\tt long} values.

class CL_StringIntMap: public CL_Map<CL_String, long> {

public:
    CL_StringIntMap () {};

    CL_StringIntMap (CL_MapAssoc<CL_String, long>
                     a[], long cnt) : CL_Map<CL_String, long> (a, cnt) {};
    
    const char* ClassName () const {return "CL_StringIntMap";};

    CL_ClassId ClassId () const {return _CL_StringIntMap_CLASSID;};

    CL_Object* Clone () const {return new CL_StringIntMap (*this); };

};

typedef CL_MapAssoc<CL_String, long> CL_StringIntAssoc;
typedef CL_MapIterator<CL_String, long> CL_StringIntMapIterator;



//  The StringPtrMap has Strings as keys, and its values are pointers to
//  {\tt CL_Object}.


class CL_StringPtrMap: public CL_Map<CL_String, CL_ObjectPtr> {

public:
    CL_StringPtrMap () {};

    CL_StringPtrMap (CL_MapAssoc<CL_String, CL_ObjectPtr>
                     a[], long cnt) :
                     CL_Map<CL_String, CL_ObjectPtr> (a, cnt) {};
    
    const char* ClassName () const {return "CL_StringPtrMap";};

    CL_ClassId ClassId () const {return _CL_StringPtrMap_CLASSID;};

    CL_Object* Clone () const {return new CL_StringPtrMap (*this); };

};


typedef CL_MapAssoc<CL_String, CL_ObjectPtr> CL_StringPtrAssoc;
typedef CL_MapIterator<CL_String, CL_ObjectPtr> CL_StringPtrMapIterator;

/* ----------------------- GenericMap ---------------------------------
 * Commented out to prevent the "out of memory" error from Borland's tlib.
 * --- Sridhar 9/15/94
 *
 * class CL_GenericMap: public CL_Map<CL_ObjectPtr, CL_ObjectPtr> {
 *
 * public:
 *     CL_GenericMap () {};
 * 
 *     CL_GenericMap (CL_MapAssoc<CL_ObjectPtr, CL_ObjectPtr>
 *                    a[], long cnt) : CL_Map<CL_ObjectPtr,
 *     CL_ObjectPtr> (a, cnt) {};
 *     
 *     const char* ClassName () const {return "CL_GenericMap";};
 * 
 *     CL_ClassId ClassId () const {return _CL_GenericMap_CLASSID;};
 * 
 *     CL_Object* Clone () const {return new CL_GenericMap (*this); };
 * 
 * };
 * 
 * 
 * typedef CL_MapAssoc<CL_ObjectPtr, CL_ObjectPtr> CL_GenericAssoc;
 * typedef CL_MapIterator<CL_ObjectPtr, CL_ObjectPtr> CL_GenericMapIterator;
 * 
 */
 
// PtrIntMap uses pointers to {\tt CL_Object} as keys and {\tt long}s as
// values. It uses the key's {\tt Compare} method for comparing.


class CL_PtrIntMap: public CL_Map<CL_ObjectPtr, long> {

public:
    CL_PtrIntMap () {};

    CL_PtrIntMap (CL_MapAssoc<CL_ObjectPtr, long> a[],
                  long cnt) : CL_Map<CL_ObjectPtr, long>
    (a, cnt) {};
    
    const char* ClassName () const {return "CL_PtrIntMap";};

    CL_ClassId ClassId () const {return _CL_PtrIntMap_CLASSID;};

    CL_Object* Clone () const {return new CL_PtrIntMap (*this); };

};


typedef CL_MapAssoc<CL_ObjectPtr, long> CL_PtrIntAssoc;
typedef CL_MapIterator<CL_ObjectPtr, long> CL_PtrIntMapIterator;

#endif

