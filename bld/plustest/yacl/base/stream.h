

#ifndef _stream_h_ /* Tue Feb 22 11:29:17 1994 */
#define _stream_h_





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




// This is a logically abstract class representing the notion of a
// positionable binary data stream. It provides operators and methods for
// data transfer between objects and the stream. All operators and methods
// are expressed in terms of six {\it core\/} methods: {\tt Read (uchar*,
// long}, which reads a given number of bytes from the stream; {\tt Write
// (uchar*, long}, which writes a given number of bytes to the stream;
// {\tt SeekTo (long)}, which positions the stream's cursor; {\tt
// Size()}, which returns the stream's current size in bytes; {\tt
// ChangeSize (long)}, which changes the stream's size; and {\tt Offset ()},
// which returns the cursor position.
//
// When the {\tt Remember} method is called, the Stream begins to remember
// addresses of all instances of objects derived
// from {\tt CL_Object}  that have been written to it.
// An attempt to  write an object that has already been written
// results in writing a reference to the previously-written object. This
// is so that cyclic data structures can be
// written and read from the Stream without danger of infinite recursion.
// When it is necessary to make the Stream forget the remembered object
// addresses, its {\tt Forget} method must be called.


#include "base/object.h"

#ifdef __GNUC__
#pragma interface
#endif

typedef long CL_Offset;

class CL_Stream: public CL_Object {
 
public:

    CL_Stream ();
    // Constructor.

    ~CL_Stream ();
    // Destructor.

  
    // ----------------- Read operations -------------------------

    virtual long Read (uchar* buffer, long num_bytes) const
        {return 0;}; 
    // Read from current position. Returns number of bytes read, 0 on eof, -1
    // on error.
    // This is a core method, and must be overridden by the
    // derived class.
 
    inline long Read (CL_Offset offset, uchar* buffer, long
                      num_bytes) const;
    // Read num_bytes bytes into buffer, starting at the given offset.
    // Returns number of bytes read, 0 on eof, -1 on error.

    inline bool Read (long& value) const;
    // Read a long value. Return TRUE on success, FALSE on failure
    // (i.e., end of file or i/o error).
    
    const CL_Stream& operator>> (long& value) const
        {Read (value); return *this;};
    
    inline bool Read (CL_Offset offset, long& value) const;
    // Read a long value from the given position. Return TRUE on
    // success, FALSE on failure (i.e., end of file or i/o error).
    

    inline bool Read (short& value) const;
    // Read a short value. Return TRUE on success, FALSE on failure
    
    const CL_Stream& operator>> (short& value) const
        {Read (value); return *this;};
    
    inline bool Read (char& value) const;
    // Read a char value. Return TRUE on success, FALSE on failure
    
    const CL_Stream& operator>> (char& value) const
        {Read (value); return *this;};
    
    inline bool Read (uchar& value) const;
    // Read an unsigned char value. Return TRUE on success, FALSE on failure
    
    const CL_Stream& operator>> (uchar& value) const
        {Read (value); return *this;};
    
    inline bool Read (CL_Offset offset, short& value) const;
    // Read a short value from the given position. Return TRUE on
    // success, FALSE on failure (i.e., end of file or i/o error).


    virtual bool Read (CL_Object& obj) const;
    // Generic reading: uses the virtual {\tt ReadFrom} method. The parameter's
    // {\tt ReadFrom} is called only if the object has not already been
    // read from the Stream; this is to prevent duplication.  The address
    // of each read object is remembered by the Stream.


    const CL_Stream& operator>> (CL_Object& value) const
        {Read (value); return *this;};

    bool Read (CL_Offset offset, CL_Object& obj) const;


    virtual bool Read (CL_ObjectPtr& p) const;
    // Read the value in the stream, construct an object from it, and
    // assign it to the parameter. If the stream contains the "null
    // pointer" indicator, then p is assigned NULL.
    
    const CL_Stream& operator>> (CL_ObjectPtr value) const
        {Read (value); return *this;};

    bool Read (CL_Offset offset, CL_ObjectPtr obj) const;

    
    // ----------------- Write operations ------------------------

    virtual bool Write (uchar* buffer, long num_bytes)
        {return FALSE;};
    // Write at current position. The default implementation returns FALSE
    // unconditionally.
    // This is a core method, and must be overridden by the
    // derived class.

    inline bool Write (CL_Offset offset, uchar* buffer, long num_bytes);
    // Seek to position "offset" from beginning of file; then
    // write num_bytes bytes into buffer. Offset 0 is the beginning of the
    // file.


    inline bool Write (long value);
    // Write a long value. Return TRUE on success, FALSE on failure
    // (e.g., i/o error).
    
    CL_Stream& operator<< (long value)
        {Write (value); return *this;};
    
    inline bool Write (CL_Offset offset, long value);
    // Write a long value at the given position. Return TRUE on
    // success, FALSE on failure (e.g., i/o error).
    
    inline bool Write (short value);
    // Write a short value. Return TRUE on success, FALSE on failure
    
    CL_Stream& operator<< (short value)
        {Write (value); return *this;};
    
    inline bool Write (CL_Offset offset, short value);
    // Write a short value from the given position. Return TRUE on
    // success, FALSE on failure (i.e., end of file or i/o error).

    inline bool Write (uchar value);
    // Write an unsigned char value. Return TRUE on success, FALSE on
    // failure.
    
    CL_Stream& operator<< (uchar value)
        {Write (value); return *this;};
    
    inline bool Write (char value);
    // Write a char value. Return TRUE on success, FALSE on failure.
    
    CL_Stream& operator<< (char value)
        {Write (value); return *this;};


    virtual bool Write (CL_ObjectPtr p);
    // Write the object pointed to by p to the stream. If the parameter is
    // NULL, write the "null pointer" indicator. Otherwise, use the object's
    // WriteTo method.
    
    CL_Stream& operator<< (CL_ObjectPtr value)
        {Write (value); return *this;};


    
    virtual bool Write (const CL_Object& obj);
    // Generic writing: uses the WriteTo method. The parameter's
    // {\tt WriteTo} is called only if the object has not already been
    // written to the Stream; this is to prevent duplication. The address
    // of each written object is remembered by the Stream.


    CL_Stream& operator<< (const CL_Object& value)
        {Write (value); return *this;};

    inline bool Write (CL_Offset offset, const CL_Object& obj);
    
    bool Append (uchar* buffer, long num_bytes);
    // Write at the end of the file
    
    
    // ------------- Positioning and sizing operations ----------
    
    virtual bool SeekTo (CL_Offset position) const {return FALSE;};
    // Change the current position to the given one. Returns TRUE on success.
    // This is a core method, and must be overridden by the
    // derived class. The default implementation returns FALSE
    // unconditionally.

    virtual bool SeekRelative (long change) const
        {return SeekTo (Offset() + change);};
    
    bool SeekToBegin() const
        {return SeekTo (0L);};
    // Change the current position to the beginning of the stream.

    virtual bool SeekToEnd () const {return SeekTo (Size());};
    // Change the current position to the end of the stream.

    virtual bool ChangeSize (long new_size) {return FALSE;};
    // Change the size of the stream to the given size. Return TRUE if
    // successful.
    // This is a core method, and must be overridden by the
    // derived class. The default implementation returns FALSE
    // unconditionally.

    virtual long Size () const {return 0;};
    // Return the current size of the stream.
    // This is a core method, and must be overridden by the
    // derived class. The default implementation returns 0
    // unconditionally.
    
    virtual bool Eof () const {return Size() <= Offset();};
    // Return TRUE if the stream's cursor is at the end of the stream.
    
    virtual long Offset () const {return 0;};
    // Return the current position.
    // This is a core method, and must be overridden by the
    // derived class. The default implementation returns 0
    // unconditionally.


    virtual CL_String ErrorString () const;
      // Return the error message string associated with the current error
    // status, if any.

    // ------------------- History-related operations ------------

    virtual void Remember ();
    // Begin remembering the addresses of objects written into this
    // stream.
    
    virtual void Forget ();
    // Forget the remembered addresses.

    bool IsRemembering () const {return _remembering;};
    // Return the current state, whether this stream is currently
    // remembering object addresses.
    
    // -------------- Basic methods  ----------------------

    const char* ClassName() const {return "CL_Stream";};
    // Override the method inherited from {\tt CL_Object}.

    CL_ClassId ClassId() const { return _CL_Stream_CLASSID;};
    // Override the method inherited from {\tt CL_Object}.

    // ------------------- End public protocol ------------------------ 
 
protected:

    virtual CL_ObjectPtr _BuildObject () const;
    // Read a class id from this stream, construct an empty object of
    // that type and return it. Return NULL on error (e.g., no such type).
    
    bool          _remembering;
    void*         _maps;
};
 
 


inline long CL_Stream::Read (CL_Offset offset, uchar* buffer, long
                           num_bytes) const
{
    return SeekTo (offset) ? Read (buffer, num_bytes) : -1;
}

inline bool CL_Stream::Read (long& value) const
{
    return Read ((uchar*) &value, sizeof value) == sizeof value;
}

inline bool CL_Stream::Read (CL_Offset offset, long& value) const
{
    return SeekTo (offset) && (Read ((uchar*) &value, sizeof value) ==
                               sizeof value); 
}

inline bool CL_Stream::Read (short& value) const
{
    return Read ((uchar*) &value, sizeof value) == sizeof value;
}

inline bool CL_Stream::Read (CL_Offset offset, short& value) const
{
    return SeekTo (offset) && (Read (offset, (uchar*) &value, sizeof
                                     value) == sizeof value);
}

inline bool CL_Stream::Read (char& value) const
{
    return Read ((uchar*) &value, sizeof value) == sizeof value;
}

inline bool CL_Stream::Read (uchar& value) const
{
    return Read ((uchar*) &value, sizeof value) == sizeof value;
}

inline bool CL_Stream::Read (CL_Offset offset, CL_Object& o) const
{
    return (SeekTo (offset)) &&  Read (o);
}



inline bool CL_Stream::Write (CL_Offset offset, uchar* buffer, long
                            num_bytes)
{
    return SeekTo (offset) && Write (buffer, num_bytes);
}

inline bool CL_Stream::Write (long value)
{
    return Write ((uchar*) &value, sizeof value);
}

inline bool CL_Stream::Write (CL_Offset offset, long value)
{
    return SeekTo (offset) && Write ((uchar*) &value, sizeof value);
}

inline bool CL_Stream::Write (short value)
{
    return Write ((uchar*) &value, sizeof value);
}

inline bool CL_Stream::Write (char value)
{
    return Write ((uchar*) &value, sizeof value);
}

inline bool CL_Stream::Write (uchar value)
{
    return Write ((uchar*) &value, sizeof value);
}

inline bool CL_Stream::Write (CL_Offset offset, short value)
{
    return SeekTo (offset) && Write ((uchar*) &value, sizeof value);
}


inline bool CL_Stream::Write (CL_Offset offset, const CL_Object& o)
{
    return SeekTo (offset) && Write (o);
}



inline bool CL_Stream::Append (uchar* buffer, long num_bytes)
{
    return SeekToEnd() && Write (buffer, num_bytes);
}




#endif /* _stream_h_ */
