



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





#include "base/bytestrm.h"


CL_ByteStream::CL_ByteStream (CL_ByteArray& s)
: _buffer (s)
{
    _position = 0;
}


// ----------------- Read operations -------------------------

// Read from current position. Returns # bytes read, 0 on eof, -1
// on error.
long CL_ByteStream::Read (uchar* output, long num_bytes) const
{
    long ret = maxl (0, minl (_buffer.Size() - _position, num_bytes));
    if (ret > 0) {
        _buffer.CopyTo (output, ret, _position);
        ((CL_ByteStream*) this)->_position += ret; // const cast away
    }
    return ret;
}

    
// ----------------- Write operations ------------------------

// Write at current position.
bool CL_ByteStream::Write (uchar* buffer, long num_bytes)
{
    long size = _buffer.Size();
    if (size - _position < num_bytes) {
        long new_size = _position + num_bytes;
        if (!_buffer.ChangeSize (new_size))
            return FALSE;
    }
        
    if (num_bytes > 0) {
        _buffer.CopyFrom (buffer, num_bytes, _position);
        _position += num_bytes;
    }
    return num_bytes >= 0;
}



// ----------------- Seek operations -------------------------

#define DEFAULT_EXPANSION 64

// Change the current position. Returns TRUE on success.
bool CL_ByteStream::SeekTo (CL_Offset position) const
{
    if (position < 0)
        return FALSE;
    long size = _buffer.Size();
    if (position > size && !_buffer.ChangeSize (size + DEFAULT_EXPANSION))
        return FALSE;
    ((CL_ByteStream*) this)->_position = position; // const cast away
    return TRUE;
}


bool CL_ByteStream::SeekToEnd () const
{
    ((CL_ByteStream*) this)->_position = _buffer.Size(); // Cast away const
    return TRUE;
}


bool CL_ByteStream::SeekRelative (long change) const
{
    CL_Offset new_pos = maxl (0, _position + change);
    ((CL_ByteStream*) this)->_position = new_pos; // Cast away const
    return TRUE;
}





