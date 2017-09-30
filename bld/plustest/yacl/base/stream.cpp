



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




#include <stdarg.h>
#include <stdio.h>

#include "base/stream.h"
#include "base/error.h"
#include "base/string.h"
#include "base/map.h"


// Have to resort to allocating this struct here, rather than making them
// instance variables, just to please the #@!$% GCC compiler's template
// instantiation quirks. -- MAS 7/16/94

struct MapStruct { 
    CL_IntIntMap _readMap;
    CL_IntIntMap _writeMap;
};


const short REAL_OBJECT = 'O', OBJ_REF = 'R', NULL_POINTER = 'N';

CL_Stream::CL_Stream ()
{
    _remembering = FALSE;
    _maps = new MapStruct;
}



CL_Stream::~CL_Stream ()
{
    if (_maps)
        delete (MapStruct*) _maps;
}




bool CL_Stream::Read (CL_Object& obj) const
{
    short flag;
    CL_Offset  offset, tmp;
    CL_ClassId id;
    if (!Read (flag))
        return FALSE;
    switch (flag) {
    case REAL_OBJECT:
        Read (id); // Ignore the class id we wrote
        return obj.ReadFrom (*this);

    case OBJ_REF:
        CL_Error::Warning ("CL_Stream::Read: reading object, found pointer");
        return FALSE;

    default: // Invalid flag
        CL_Error::Warning ("CL_Stream::Read: invalid flag: %d", flag);
        return FALSE;
    }
}




bool CL_Stream::Read (CL_ObjectPtr& p) const
{
    short flag;
    CL_Offset  offset, tmp;
    if (!Read (flag))
        return FALSE;
    switch (flag) {
    case REAL_OBJECT:
        p = _BuildObject ();
        tmp = Offset ();
        if (p && p->ReadFrom (*this)) {
            (((MapStruct*) _maps)->_readMap).Add (tmp, (long) p);
            return TRUE;
        }
        return FALSE;

    case OBJ_REF:
        if (!Read (offset) || offset < 0) // Read the offset of the object
            return FALSE;
        p = (CL_ObjectPtr) (((MapStruct*) _maps)->_readMap) [offset];
        return p ? TRUE : FALSE;

    case NULL_POINTER:
        p = NULL;
        return TRUE;
        
    default: // Invalid flag
        CL_Error::Warning ("CL_Stream::Read: invalid flag: %d", flag);
        return FALSE;
    }
}




bool CL_Stream::Write (const CL_Object& obj)
{
    if (_remembering) {
        CL_Offset offset = (((MapStruct*) _maps)->_writeMap) [(long) &obj]-1;
        // --------------------------------------------------------------^^^-
        // This -1, and the corresponding +1 indicated below, are used to
        // ensure that the value associated with each key
        // in the map is positive, since the map returns 0 (null value) if the
        // key is not in it. Of course, we could use the map's IncludesKey
        // method to avoid this adjustment, but that would mean the
        // inefficiency of two map lookups.
        if (offset >= 0) {
            // The writeMap contained this object, so we just write a
            // reference.
            return Write (OBJ_REF) && Write (offset);
        }
        // Othewise, write the object, and remember it. The offset
        // maintained in the map is the offset o at which the object lives, not
        // the offset o-4 at which we wrote the class id.
        return Write (REAL_OBJECT) && Write (obj.ClassId()) 
            && (((MapStruct*) _maps)->_writeMap).Add ((long) &obj, Offset()+1)
            && obj.WriteTo (*this);
        // -------------------------------------------------------------^^^
        // See comment above
    }
    // Not currently remembering addresses:
    return Write (REAL_OBJECT) && Write (obj.ClassId()) && obj.WriteTo (*this);
}


bool CL_Stream::Write (CL_ObjectPtr p)
{
    return p ?  Write (*p) : Write (NULL_POINTER);
}

    

void CL_Stream::Remember ()
{
    (((MapStruct*) _maps)->_writeMap).MakeEmpty ();
    _remembering = TRUE;
}

void CL_Stream::Forget ()
{
    (((MapStruct*) _maps)->_writeMap).MakeEmpty ();
    _remembering = FALSE;
}


CL_String CL_Stream::ErrorString () const
{
    return "";
}


typedef CL_Object* (*Creator) ();


CL_ObjectPtr CL_Stream::_BuildObject () const
{
    CL_ClassId id;
    if (!Read (id))
        return NULL;
    Creator f = (Creator) (CL_Object::_ClassIdMap [id]);
    if (!f)
        return NULL;
    CL_Object* p = (*f) ();
    return p;
}



