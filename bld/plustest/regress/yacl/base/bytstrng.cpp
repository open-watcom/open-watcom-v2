



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





#include "base/bytstrng.h"
#include "base/string.h"
#include "base/stream.h"

#ifdef __BORLANDC__
#include <mem.h>
#ifndef __OS2__
#define MEMCMP _fmemcmp
#else
#define MEMCMP memcmp
#endif
#else
#include <string.h>
#define MEMCMP memcmp
#endif


// #define NEW_OP new (__LINE__, __FILE__)
#define NEW_OP new

#ifdef __GNUC__
#pragma implementation
#endif


CL_DEFINE_CLASS(CL_ByteString,  _CL_ByteString_CLASSID);



CL_ByteString::CL_ByteString()
    : CL_ByteArray (this)
{
    _array = NULL;
    _size = 0;
}


CL_ByteString::CL_ByteString (long size)
: CL_ByteArray (this)
{
    _array = NEW_OP uchar [_size = size];
    if (_array)
        SetAllBytesTo (0);
    else
        _size = 0;  // No memory
}


CL_ByteString::CL_ByteString (const CL_ByteString& b)
: CL_ByteArray (this)
{
    long n = b.Size();
    _array = NEW_OP uchar [n];
    if (!_array) { // No memory
        _size = 0;
        return;
    }
    CL_ByteArray q(_array, n);
    q = b;
    _size = n;
}



CL_ByteString::CL_ByteString (uchar* b, long size)
: CL_ByteArray (this)
{
    _array = NEW_OP uchar [_size = size];
    if (!_array) {
        _size = 0;
        return;
    }
    CL_ByteArray q(_array, size);
    q = b;
}



CL_ByteString::CL_ByteString (const CL_ByteArray& b)
: CL_ByteArray (this)
{
    _array = NEW_OP uchar [_size = b.Size()];
    if (!_array) {
        _size = 0;
        return;
    }
    CL_ByteArray q(_array, _size);
    q = b;
}





CL_ByteString::CL_ByteString (const CL_String& s)
: CL_ByteArray (this)
{
    _array = NEW_OP uchar [_size = s.Size() + 1];
    if (!_array) {
        _size = 0;
        return;
    }
    CL_ByteArray q(_array, _size+1);
    q = (uchar*) s.AsPtr();
}






CL_ByteString::~CL_ByteString()
{
    if (_array != 0)
        delete [] _array;
}



void CL_ByteString::operator= (const CL_ByteArray& b)
{
    if (this == &b)
        return;
    if (!PrepareToChange())
        return;
    long len = b.Size();
    if (_size < len) {
        if (_array != NULL) delete [] _array;
        _array = NEW_OP uchar [_size = len];
    }
    if (!_array) {
        _size = 0;
        return;
    }
    CL_ByteArray q(_array, _size);
    q = b;
    Notify ();
}



void CL_ByteString::operator= (const CL_String& b)
{
    if (!PrepareToChange())
        return;
    long len = b.Size() + 1;
    if (_size < len) {
        if (_array != NULL) delete [] _array;
        _array = NEW_OP uchar [_size = len];
    }
    if (!_array) {
        _size = 0;
        return;
    }
    CL_ByteArray q(_array, _size);
    q = b;
    Notify ();
}



CL_ByteArray& CL_ByteString::operator= (long p)
{
    if (_size < sizeof (long))
        if (!ChangeSize (sizeof (long) + _size))
            return *this;
    CL_ByteArray::operator = (p);
    return *this;
}



CL_ByteArray& CL_ByteString::operator= (short p)
{
    if (_size < sizeof (short))
        if (!ChangeSize (sizeof (short) + _size))
            return *this;
    CL_ByteArray::operator = (p);
    return *this;
}






CL_ByteString& CL_ByteString::operator+= (const CL_ByteArray& b)
{
    if (!PrepareToChange())
        return *this;
    long l = b.Size ();
    if (ChangeSize (_size +l)) {
        (*this) (_size - l, l) = b;
        Notify ();
    }
    return *this;
}






bool CL_ByteString::ChangeSize (long new_size)
{
    long n = new_size;
    if (n < 0)
        return FALSE;
    if (n == 0) {
        delete _array;
        _array = NULL;
        _size = 0L;
        return TRUE;
    }
    uchar* p = NEW_OP uchar [n];
    if (!p)
        return FALSE;
    if (_array) {
        memcpy (p, _array, minl (n, _size));
        delete [] _array;
    }
    _array = p;
    _size = n;
    return TRUE;
}



// -------------------- Storage and restoration --------------

long CL_ByteString::StorableFormWidth () const
{
    return sizeof (CL_ClassId) +_size + sizeof (long);
}


bool CL_ByteString::ReadFrom (const CL_Stream& s)
{
    if (!PrepareToChange())
        return FALSE;
    if (!ReadClassId (s))
        return FALSE;
    long size;
    uchar* p;
    if (!s.Read (size))
        return FALSE;
    p = new uchar [size];
    if (!p || !s.Read (p, size))
    if (_array)
        delete [] _array;
    _array = p;
    _size = size;
    Notify();
    return TRUE;
}


bool CL_ByteString::WriteTo  (CL_Stream& s) const
{
    return s.Write (ClassId())  &&
        s.Write (_size) && s.Write (_array, _size);
}



