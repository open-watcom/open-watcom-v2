



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





#include <iostream.h>

#include "base/clntset.h"
#include "base/stream.h"
#include "base/integer.h"
#include "base/map.h"

#ifdef __GNUC__
#pragma implementation
#endif



static CL_Map<long,long>* TheClassIdMap = NULL;

#ifdef DEBUG
#include "base/memory.h"
static CL_MemoryLeakChecker* check;
#endif

CL_Map<long,long>& CL_Object::_ClassIdMap =
TheClassIdMap ? *TheClassIdMap
: *(TheClassIdMap = new CL_Map<long,long>);

CL_DEFINE_CLASS(CL_Object, _CL_Object_CLASSID);

CL_ClassIdEntryMaker::CL_ClassIdEntryMaker (CL_ClassId id, CL_Creator func)
{
    if (!TheClassIdMap) {
#ifdef DEBUG
        check = new CL_MemoryLeakChecker (cout);
#endif
        TheClassIdMap = new CL_Map<long,long>;
    }
    TheClassIdMap->Add (id, (long) func);
}


CL_ClassIdEntryMaker::~CL_ClassIdEntryMaker ()
{
    if (TheClassIdMap) {
        delete TheClassIdMap;
        TheClassIdMap = NULL;
#ifdef DEBUG
        delete check;
#endif
    }
}


CL_String CL_Object::AsString() const
{
    NotImplemented ("CL_Object::AsString");
    return "";
}



istream& operator>> (istream& s, CL_Object& o)
{
    o.FromStream (s);
    return s;
}



ostream& operator<< (ostream& s, const CL_Object& o)
{
    o.IntoStream (s);
    return s;
}




void CL_Object::IntoStream (ostream& o) const
{
    CL_String s = AsString ();
    o << s.AsPtr();
}


bool CL_Object::ReadClassId (const CL_Stream& s) const
{
    long id;
    if (!s.Read (id))
        return FALSE;
    if (id != ClassId()) {
        s.SeekTo (s.Offset() - sizeof id);
        return FALSE;
    }
    return TRUE;
}

bool CL_Object::ReadFrom  (const CL_Stream&)
{
    NotImplemented ("ReadFrom");
    return FALSE;
}


bool CL_Object::WriteTo  (CL_Stream&) const
{
    NotImplemented ("WriteTo");
    return FALSE;
}




bool CL_Object::CompareWith (const CL_Object& obj,
                             CL_Object::ComparisonOperator op) const
{
    short result;
    if (!IsA (obj))
        result = (this < &obj ? -1 : 1);
    else
        result = Compare (obj);
    switch (op) {
    case CL_Object::OP_EQUAL:
        return result == 0;
        
    case CL_Object::OP_LESSTHAN:
        return result < 0;
        
    case CL_Object::OP_GTRTHAN:
        return result > 0;
        
    case CL_Object::OP_LESSEQ:
        return result <= 0;
        
    case CL_Object::OP_GTREQ:
        return result >= 0;
        
    case CL_Object::OP_NOTEQUAL:
        return result != 0;
        
    case CL_Object::OP_PREFIX:
    case CL_Object::OP_CONTAINS:
        return AsString().CompareWith (obj.AsString(), op);

    default:
        CL_Error::Warning ("CL_Object::CompareWith: bad operator %d",
                           (short) op);
        break;
    }
    return FALSE;
}



bool CL_Object::CompareWith (const CL_String& obj,
                             CL_Object::ComparisonOperator op) const
{
    return AsString().CompareWith (obj, op);
}










// ---------------------- Dependency methods ------------------------

static long __NullValue = 0;

struct CL_DependStruct {
    CL_ClientSet postChange;
    CL_ClientSet preChange;
};

    


CL_Object::~CL_Object()
{
    if (_dependSet) {
        delete  _dependSet;
    }
}



        

void CL_Object::Notify ()
{
    if (_dependSet) {
        ( _dependSet)->postChange.NotifyAll (*this);
    }
}



void CL_Object::AddDependent (const CL_AbstractBinding& binding, long code)
{
    if (!_dependSet) {
        _dependSet = new CL_DependStruct;
        if (!_dependSet)
            return; // No memory
    }
    ( _dependSet)->postChange.Add (binding, code);
}




void CL_Object::RemoveDependent (const CL_AbstractBinding& binding)
{
    if (_dependSet) {
        ( _dependSet)->postChange.Remove (binding);
    }
}



bool CL_Object::HasDependent (const CL_AbstractBinding& o) const
{
    return _dependSet &&
        ( _dependSet)->postChange.Includes (o);
}

long& CL_Object::PostChangeCode (const CL_AbstractBinding& b) const
{
    if (_dependSet) {
        CL_ClientSet* post_change =  &( _dependSet)->postChange;
        return (post_change->CodeFor (b));
    }
    __NullValue = 0;
    return __NullValue;
}



// Pre-change notification methods



bool CL_Object::PrepareToChange ()
{
    if (_dependSet) {
        return  ( _dependSet)->preChange.Permits
            (*this);
    }
    return TRUE;
}



void CL_Object::AddPreChangeDependent (const CL_AbstractBinding& binding,
                                       long code)
{
    if (!_dependSet) {
        _dependSet = new CL_DependStruct;
        if (!_dependSet)
            return; // No memory
    }
    ( _dependSet)->preChange.Add (binding, code);
}


// Remove a binding from our dependent list
void CL_Object::RemovePreChangeDependent (const CL_AbstractBinding& b)
{
    if (_dependSet) {
        ( _dependSet)->preChange.Remove (b);
    }
}


// Is the given object dependent on us?
bool CL_Object::HasPreChangeDependent (const CL_AbstractBinding& b) const
{
    return _dependSet &&
        ( _dependSet)->preChange.Includes (b);
}


long& CL_Object::PreChangeCode (const CL_AbstractBinding& b) const
{
    if (_dependSet) {
        CL_ClientSet* pre_change = &( _dependSet)->preChange;
        return (pre_change->CodeFor (b));
    }
    __NullValue = 0;
    return __NullValue;
}


#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
template class CL_Binding<CL_Object>;
#endif

