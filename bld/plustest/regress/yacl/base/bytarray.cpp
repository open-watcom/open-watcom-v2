



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




#include "base/defs.h"
#include "base/bytarray.h"
#include "base/bytstrng.h"
#include "base/stream.h"


// #define NEW_OP new (__LINE__, __FILE__)
#define NEW_OP new




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

#ifdef __GNUC__
#pragma implementation
#endif


CL_DEFINE_CLASS(CL_ByteArray, _CL_ByteArray_CLASSID);


CL_ByteArray::CL_ByteArray(CL_Object* client)
{
    _array = 0L;
    _size = 0L;
    _client = client;
}


//
// Constructor: assume that the array of bytes in question begins
// at the given address, and is of the given size
//
CL_ByteArray::CL_ByteArray (uchar *buf, long size, CL_Object* client)
{
    _array  = buf;
    _size = size;
    _client = client;
}




CL_ByteArray::CL_ByteArray (const CL_ByteArray& b)
{
    _array  = b._array;
    _size = b._size;
    _client = b._client;
}



CL_ByteArray::~CL_ByteArray ()
{
}

    


void CL_ByteArray::SetAllBytesTo (uchar b)
{
    if (!PrepareToChange() || (_client && !_client->PrepareToChange()))
        return;
    if (_size > 0 && _array != NULL) {
        memset (_array, b, _size);
        Notify ();
        if (_client)
            _client->Notify();
    }
}


//
// Retrieve the byte at the given index. As usual, 
// zero-origin indexing is assumed. It is an error to specify an
// index outside the legal range.
uchar CL_ByteArray::operator[] (long index)
{
    assert ((index >= 0 && index < _size),
            ("CL_ByteArray::operator[]: Bad "
             "index %ld size %ld\n", index, _size));
    if (!_array) { // Because of memory problems?
        uchar c = 0;
        return c;
    }
    return _array[index];
}



//
// Comparison methods
// For the methods taking character pointer arguments, it is
// assumed that the size of the byte array is the same as our
// size.
//

bool CL_ByteArray::operator== (const uchar* b) const
{
    return MEMCMP (_array, (uchar*) b, _size) == 0;
}


bool CL_ByteArray::operator== (const CL_Object& b) const
{
    if (!IsA (b))
        return FALSE;
    register const CL_ByteArray& o = (const CL_ByteArray&) b;
    return o._size == _size && MEMCMP (_array, o._array, _size) == 0;
}


bool CL_ByteArray::operator== (const CL_ByteArray& o) const
{
    return o._size == _size && MEMCMP (_array, o._array, _size) == 0;
}








bool CL_ByteArray::operator!= (const CL_Object& b) const
{
    if (!IsA (b))
        return TRUE;
    return MEMCMP (_array, ((const CL_ByteArray&) b).AsPtr(),
                   _size) != 0; 
}


bool CL_ByteArray::operator!= (const CL_ByteArray& o) const
{
    return o._size != _size || MEMCMP (_array, o._array, _size) != 0;
}


bool CL_ByteArray::operator!= (const uchar* b) const
{
    return MEMCMP (_array, b, _size) != 0;
}





bool CL_ByteArray::operator>= (const CL_Object& b) const
{
    if (!IsA (b))
        return this >= (CL_ByteArray*) &b;
    return (MEMCMP (_array, ((const CL_ByteArray&) b).AsPtr(),
                    _size) >= 0);
}

bool CL_ByteArray::operator>= (const CL_ByteArray& o) const
{
    register long sz = minl (_size, o._size);
    return MEMCMP (_array, o._array, sz) >= 0;
}


bool CL_ByteArray::operator>= (const uchar* b) const
{
    return MEMCMP (_array, b, _size) >= 0;
}





bool CL_ByteArray::operator<= (const CL_Object& b) const
{
    if (!IsA (b))
        return this <= (CL_ByteArray*) &b;
    return MEMCMP (_array, ((const CL_ByteArray&) b).AsPtr(),
                   _size) <= 0; 
}

bool CL_ByteArray::operator<= (const CL_ByteArray& o) const
{
    register long sz = minl (_size, o._size);
    return MEMCMP (_array, o._array, sz) <= 0;
}


bool CL_ByteArray::operator<= (const uchar* b) const
{
    return MEMCMP (_array, b, _size) <= 0;
}





bool CL_ByteArray::operator> (const CL_Object& b) const
{
    if (!IsA (b))
        return this > (CL_ByteArray*) &b;
    short result = MEMCMP (_array, ((const CL_ByteArray&) b).AsPtr(),
                           _size);
    return  result > 0 || result == 0 &&
            _size > ((const CL_ByteArray&) b).Size();
}

bool CL_ByteArray::operator> (const CL_ByteArray& o) const
{
    register long sz = minl (_size, o._size);
    return MEMCMP (_array, o._array, sz) > 0;
}


bool CL_ByteArray::operator> (const uchar* b) const
{
    return MEMCMP (_array, b, _size) > 0;
}





bool CL_ByteArray::operator< (const CL_Object& b) const
{
    if (!IsA (b))
        return this < (CL_ByteArray*) &b;
    short result = MEMCMP (_array, ((const CL_ByteArray&) b).AsPtr(),
                   _size);
    return result < 0 || result == 0 &&
            _size < ((const CL_ByteArray&) b).Size();
}

bool CL_ByteArray::operator< (const CL_ByteArray& o) const
{
    register long sz = minl (_size, o._size);
    return MEMCMP (_array, o._array, sz) < 0;
}


bool CL_ByteArray::operator< (const uchar* b) const
{
    return MEMCMP (_array, b, _size) < 0;
}





short CL_ByteArray::Compare (const CL_Object& o) const
{
    if (!IsA (o))
        return this < (CL_ByteArray*) &o ? -1 : 1;
    return MEMCMP (_array, ((const CL_ByteArray&) o)._array, _size);
}



short CL_ByteArray::Compare (const CL_ByteArray& o) const
{
    return MEMCMP (_array, o._array, _size);
}




//
// Sub-array extraction
//
// Return the sub-array beginning at the given index (using
// 0-origin indexing) and of the given size. Note that the
// returned CL_ByteArray uses (part of) the same memory as we do.
//
CL_ByteArray CL_ByteArray::operator() (long start, long size) const
{
    CL_ByteArray p (_array+start, size, _client);
    return p;
}



        
CL_ByteString CL_ByteArray::operator+ (const CL_ByteArray& b) const
{
    CL_ByteString b1 (_size + b.Size());
    b1(0, _size) = _array;
    b1.Suffix (_size) = b;
    return b1;
}





//
// Return the length of the longest common prefix with the
// parameter.
long CL_ByteArray::CommonPrefixLength (const CL_ByteArray& b)
{
    if (!_array)
        return 0; // No memory
    long i;
    uchar* p = b.AsPtr();
    for (i = 0; i < _size; i++) 
        if (_array[i] != p[i]) return i;
    return _size;
}


long CL_ByteArray::CommonPrefixLength (const uchar* p)
{
    if (!_array)
        return 0; // No memory
    long i;
    for (i = 0; i < _size; i++) 
        if (_array[i] != p[i]) return i;
    return _size;
}



CL_ByteArray CL_ByteArray::Suffix (long index) const
{
    return CL_ByteArray ( (*this) (index, (_size > index) ?
                                   (_size - index) : 0L));
}





long CL_ByteArray::LongValueAt (long index) const
{
    if (!_array)
        return 0; // No memory
    long value;
    
    memcpy ((uchar*) &value, &_array[index], sizeof (long));
    return value;
}

    
    


short CL_ByteArray::ShortValueAt (long index) const
{
    if (!_array)
        return 0; // No memory
    short value  = 0;
    
    if (_array)
        memcpy ((uchar*) &value, &_array[index], sizeof (short));
    return value;
}


long CL_ByteArray::CopyTo (uchar* buffer, long count, long pos)
{
    if (count > 0 && _array && pos >= 0 && pos < _size) {
        long n = minl (_size - pos, count);
        memcpy (buffer, _array + pos, n);
        return n;
    }
    return 0;
}


    
long CL_ByteArray::CopyFrom (uchar* buffer, long count, long pos) const
{
    if (count > 0 && _array && pos >= 0 && pos < _size) {
        long n = minl (_size - pos, count);
        memcpy (_array + pos, buffer, n);
        return n;
    }
    return 0;
}


    
CL_String CL_ByteArray::AsString () const
{
    if (!_array)
        return ""; // No memory
    if (_size <= 0)
        return "";
    uchar* p = NEW_OP uchar [_size+1];
    if (!p)
        return "";
    memcpy (p, _array, _size);
    p[_size] = '\0';
    CL_String s ((char*) p);
    delete [] p;
    return s;
}





//
// Assignment
//
void CL_ByteArray::operator= (const CL_ByteArray& b)
{
    if (this == &b)
        return;
    if (!PrepareToChange() || (_client && !_client->PrepareToChange()))
        return;
    if (!_array)
        return; // No memory
    if (!b._array)
        return; // Null second array
    if (_array != b._array)  {
        memcpy (_array, b.AsPtr(), minl (_size, b.Size()));
        Notify ();
        if (_client)
            _client->Notify();
    }
}

void CL_ByteArray::operator= (const uchar* p)
{
    if (!PrepareToChange() || (_client && !_client->PrepareToChange()))
        return;
    if (!_array)
        return; // No memory
    if (!p) return;
    if (_array != p)  {
        memcpy (_array, p, _size);
        Notify ();
        if (_client)
            _client->Notify();
    }
}


void CL_ByteArray::operator= (const CL_String& p)
{
    if (!PrepareToChange() || (_client && !_client->PrepareToChange()))
        return;
    if (!_array)
        return; // No memory
    if (_array != (uchar*) p.AsPtr())  {
        memcpy (_array, p.AsPtr(), minl (_size, p.Size()+1));
        Notify ();
        if (_client)
            _client->Notify();
    }
}


CL_ByteArray& CL_ByteArray::operator= (short value)
{
    if (!PrepareToChange() || (_client && !_client->PrepareToChange()))
        return *this;
    if (!_array)
        return *this; // No memory
    memcpy (_array, &value, minl (_size, sizeof value));
    Notify ();
    if (_client)
        _client->Notify();
    return *this;
}



CL_ByteArray& CL_ByteArray::operator= (long value)
{
    if (!PrepareToChange() || (_client && !_client->PrepareToChange()))
        return *this;
    if (!_array)
        return *this; // No memory
    memcpy (_array, &value, minl (_size, sizeof value));
    Notify ();
    if (_client)
        _client->Notify();
    return *this;
}





long CL_ByteArray::StorableFormWidth() const
{
    return sizeof (CL_ClassId) + _size;
}

bool CL_ByteArray::ReadFrom (const CL_Stream& s)
{
    if (!PrepareToChange() || !ReadClassId(s)
        || !s.Read (_array, _size))
        return FALSE;
    Notify();
    if (_client)
        _client->Notify();
    return TRUE;
}


bool CL_ByteArray::WriteTo (CL_Stream& s) const
{
    return s.Write (ClassId())  && s.Write (_array, _size);
}




