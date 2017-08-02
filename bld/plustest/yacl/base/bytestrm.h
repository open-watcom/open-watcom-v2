

#ifndef _bytestrm_h_ /* Tue Feb 22 12:15:35 1994 */
#define _bytestrm_h_





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



// This class defines a ByteArray-based in-memory data stream.

#include "base/stream.h"
#include "base/bytstrng.h"

class CL_ByteStreamData;

class CL_ByteStream: public CL_Stream {

public:
    CL_ByteStream (CL_ByteArray& b);
    // Create a ByteStream that uses the given ByteArray.

    
    // ----------------- Read operations -------------------------

    virtual long Read (uchar* buffer, long num_bytes) const; 
    // Read from current position. Returns number of bytes read, 0 on eof,
    // -1 on error.
 
    bool Read (CL_Object& obj) const
        {return CL_Stream::Read (obj);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    bool Read (CL_ObjectPtr& obj) const
        {return CL_Stream::Read (obj);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    // ----------------- Write operations ------------------------

    virtual bool Write (uchar* buffer, long num_bytes);
    // Write at current position. Return TRUE on success, FALSE if fewer
    // than num_bytes were written.

    bool Write (const CL_Object& p)
        {return CL_Stream::Write (p);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    bool Write (CL_ObjectPtr p)
        {return CL_Stream::Write (p);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    
    // ----------------- Seek operations -------------------------
    
    virtual bool SeekTo (CL_Offset position) const;
    // Change the current position. Returns TRUE on success.
 
    virtual bool SeekToEnd () const;

    virtual bool SeekRelative (long change) const;
    
    virtual bool ChangeSize (long new_size);

    virtual long Size () const;
    // Return the current size of the stream.
    
    virtual bool Eof () const;
    // Are we at the end of the stream?
    
    virtual long Offset () const;
    // Return the current position.


    // ------------------ Basic functions ------------------------------
    
    const char* ClassName() const {return "CL_ByteStream";};

    CL_ClassId ClassId() const { return _CL_ByteStream_CLASSID;};

    // ------------------- End public protocol ------------------------ 
 



    
protected:
    CL_ByteArray& _buffer;
    long          _position;
};





// ---------------------- Querying ----------------------

// Are we at the end of the file?
inline bool CL_ByteStream::Eof () const
{
    return _position >= _buffer.Size();
}


// Return the current position
inline long CL_ByteStream::Offset () const
{
    return _position;
}


// Return the size of the byte block
inline long CL_ByteStream::Size () const
{
    return _buffer.Size();
}


inline bool CL_ByteStream::ChangeSize (long new_size)
{
    return _buffer.ChangeSize (new_size);
}


#endif /* _bytestrm_h_ */
