

#ifndef _bytarray_h_
#define _bytarray_h_





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



// A ByteArray is an object that provides access to  an array of bytes.
// This class is intended to manage a byte array owned by some {\it
// client\/} object
// {\it other than\/} this object; therefore, if the client is derived
// from CL_Object, then the client's dependents must be consulted and
// notified when this ByteArray is being modified. For this reason, the
// constructors for CL_ByteArray require a pointer to the client. If this
// pointer is \NULL, then the ByteArray assumes that the client does not
// need dependent notification.

#include "base/object.h"


#ifdef __GNUC__
#pragma interface
#endif


class __CLASSTYPE CL_ByteString;

class __CLASSTYPE CL_ByteArray: public CL_Object {

public:

    //
    // ---------------- Construction, destruction, conversion --------------
    //
    
    CL_ByteArray (CL_Object* client = NULL);
    // Default constructor.

    CL_ByteArray (uchar *buf, long size, CL_Object* client = NULL);
    // Constructor: assume that the array of bytes in question begins
    // at the given address, and is of the given size.


    CL_ByteArray (const CL_ByteArray& b);
    // Copy constructor: make copy of array address and size (i.e.,
    // the {\it same\/} array is used by the copy).

    ~CL_ByteArray ();

    // ------------------------- Access -----------------------------------

    void SetAllBytesTo (uchar b);
    // Set all the bytes in the byte array to the given value.
    
    virtual long Size() const;
    // Return the number of bytes in the byte array.

    long Length () const;
    // Return the number of bytes in the byte array.

    virtual operator uchar* () const;
    // Return the address of the byte array.

    virtual uchar* AsPtr () const;
    // Return the address of the byte array.

    
    virtual uchar operator[] (long index);
    // Retrieve the byte at the given index. As usual, 
    // zero-origin indexing is assumed. Specifying an index outside
    // the legal range causes a fatal error.


    //
    // ----------------------- Conversion ---------------------------------
    //
    virtual CL_String AsString () const;
    // Return the contents of this ByteArray, with a null byte appended,
    // as a string.

    //
    // ------------------------- Comparison --------------------------------
    //
    
    virtual bool operator== (const CL_Object& b) const;

    virtual bool operator== (const CL_ByteArray& b) const;

    virtual bool operator== (const uchar* b) const;
    // This and other comparison methods that take character pointer
    // arguments assume that the size of the memory segment pointed to by
    // the parameter is the same as the
    // size of this ByteArray.

    virtual bool operator!= (const CL_Object& b) const;

    virtual bool operator!= (const CL_ByteArray& b) const;

    virtual bool operator!= (const uchar* b) const;
    

    virtual bool operator>= (const CL_Object& b) const;

    virtual bool operator>= (const CL_ByteArray& b) const;

    virtual bool operator>= (const uchar* b) const;
    

    virtual bool operator<= (const CL_Object& b) const;

    virtual bool operator<= (const CL_ByteArray& b) const;

    virtual bool operator<= (const uchar* b) const;
    

    virtual bool operator> (const CL_Object& b) const;

    virtual bool operator> (const CL_ByteArray& b) const;

    virtual bool operator> (const uchar* b) const;
    

    virtual bool operator< (const CL_Object& b) const;

    virtual bool operator< (const CL_ByteArray& b) const;

    virtual bool operator< (const uchar* b) const;


    
    virtual short Compare (const CL_ByteArray& obj) const;
    // "strcmp"-style comparison. 

    virtual short Compare (const CL_Object& obj) const;
    // Override the method inherited from {\tt CL_Object}. The parameter
    // is type-checked (via IsA) and cast into a CL_ByteArray before the
    // comparison is made. 

    //
    // ------------------ Sub-structure manipulation ------------------
    //
    
    virtual CL_ByteArray operator() (long index, long size) const;
    // Return the sub-array beginning at the given index (using
    // 0-origin indexing) and of the given size. Note that the
    // returned CL_ByteArray uses (part of) the same memory as this object,
    // so it is possible to assign to the returned sub-array to effect
    // copying into parts of this object.
    // Also, assignment to sub-arrays does not change the size of the
    // array itself.

    virtual CL_ByteArray Suffix (long index) const;
    // Return the suffix beginning at the given index (using
    // 0-origin indexing). Note that the
    // returned CL_ByteArray uses (part of) the same memory as this
    // ByteArray does.

    virtual long LongValueAt (long index) const;
    // Obtain a long value from our array, beginning at the given
    // position (i.e., treat the sizeof(long) bytes beginning at the
    // given position as a long), and return it.
    
    virtual short ShortValueAt (long index) const;
    // Extract a short value from our array, beginning at the given
    // position, and return it.

    long CommonPrefixLength (const CL_ByteArray& b);
    // Return the size of the longest common prefix with the
    // parameter.

    long CommonPrefixLength (const uchar* p);

    // ----------------- Assignment, copying, modification ----------------

    virtual void operator= (const CL_ByteArray& b);
    // Assign another byte array to this one.

    virtual void operator= (const uchar* p);
    // Copy Size() bytes, from the given pointer position.

    virtual CL_ByteArray& operator= (long);
    // Copy the given long value into ourselves, beginning at position
    // 0. Do not copy more than Size() bytes. This method, as well
    // as the operator= on short and string, can be used in conjunction with
    // the operator() to put values at arbitrary positions.

    virtual CL_ByteArray& operator= (short);

    virtual void operator= (const CL_String&);
    // Copy a string, including the null byte at the end. No more than
    // Size() bytes are copied. 


    virtual void operator= (const CL_Object&);
    // Generic assignment: Check that the parameter object has the same class
    // id as this object does, and perform a ByteArray assignment.

    virtual CL_ByteString operator+ (const CL_ByteArray& b) const;
    // Return the result of concatenating b to the end of this byte array.

    virtual long CopyFrom (uchar* buffer, long count, long position = 0) const;
    // Copy count bytes from the given buffer into our data segment,
    // beginning at the given position in our data. But do not copy any
    // more than Size()-position bytes. Return the actual number of bytes
    // copied.
    
    virtual long CopyTo (uchar* buffer, long count, long position = 0);
    // Copy count bytes of our data, beginning at position pos, into the
    // given buffer. But do not copy any more than Size()-position bytes.
    // Return the actual number of bytes copied.


    virtual bool ChangeSize (long) {return FALSE;};
    // Tell us to change our size, and return TRUE if successful.  Since
    // the ByteArray manages someone else's space and therefore cannot
    // change its size, this method always returns FALSE.

    // -------------------- Storage and restoration --------------
    
    virtual long StorableFormWidth () const;

    bool ReadFrom (const CL_Stream&);

    bool WriteTo  (CL_Stream&) const;

    //
    // --------------------- Basic methods --------------------
    //
    
    virtual const char*      ClassName () const {return "CL_ByteArray";};

    virtual CL_ClassId ClassId   () const {return _CL_ByteArray_CLASSID;};

    // ----------------------- End public protocol ------------------
    


protected:
    long       _size;
    uchar*     _array;
    CL_Object* _client;
};


inline void CL_ByteArray::operator= (const CL_Object& s)
{
    if (CheckClassType (s, "CL_ByteArray::operator= (CL_Object&)"))
        *this = (const CL_ByteArray&) s;
}


inline long CL_ByteArray::Size() const
{
    return _size;
}

inline long CL_ByteArray::Length () const
{
    return _size;
}


inline CL_ByteArray::operator uchar* () const
{
    return _array;
}


inline uchar* CL_ByteArray::AsPtr () const
{
    return _array;
}



    
#endif

