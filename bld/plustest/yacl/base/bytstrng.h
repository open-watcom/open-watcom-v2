


#ifndef _bytstrng_h_
#define _bytstrng_h_





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



// The {\small\tt ByteString} class differs from the base {\small\tt
// ByteArray} class in that the former owns the memory it works with. As such,
// the {\small\tt ByteString} supports growing and shrinking of the byte
// array and concatenation of {\small\tt ByteString} objects.


#include "base/bytarray.h"
#include "base/string.h"


#ifdef __GNUC__
#pragma interface
#endif


class __CLASSTYPE CL_ByteString: public CL_ByteArray {

public:

    // ------------------ Construction and destruction ---------------
    
    CL_ByteString();
    // Default constructor: create with zero length.

    CL_ByteString (long length);
    // Constructor: build a CL_ByteString of the given length. All bytes
    // of the byte string will be zero.

    CL_ByteString (const CL_ByteArray& b);
    // Convert from a CL_ByteArray.
    
    CL_ByteString (uchar* b, long length);
    // Copy (convert) from a uchar array.

    CL_ByteString (const CL_String& s);
    // Convert from a String. This ByteString's length will be one more than
    // that of the String, and its contents will include the null terminator
    // byte.
    
    CL_ByteString (const CL_ByteString& b);
    // Copy constructor.

    ~CL_ByteString();


    // ------------------------ Byte string operations ------------
    
    
    virtual void operator= (const CL_ByteArray& b);
    // Assignment operators: inherited from ByteArray. We need to
    // declare these here because of the peculiar definition of
    // inheritance for the assignment operator.

    void operator= (const uchar* p) {CL_ByteArray::operator= (p);};
  
    virtual void  operator= (const CL_String&);
    // Copy a string, including the null byte at the end. Grow
    // ourselves if necessary.
    
    virtual CL_ByteArray& operator= (long p);
    // Copy the given long value into ourselves, beginning at position 0.

    virtual CL_ByteArray& operator= (short p);

    virtual void operator= (const CL_Object& p);


    virtual CL_ByteString& operator+= (const CL_ByteArray& b);
    // Append the contents of b to this ByteString, thereby growing the
    // latter.


    virtual bool ChangeSize (long new_size);
    // Tell us to change our size.  If the new size is less than our
    // current size, we lose the appropriate suffix. The return value
    // tells if the change was successful; a FALSE return value
    // indicates memory allocation failure.

    // -------------------- Storage and restoration --------------
    
    virtual long StorableFormWidth () const;

    bool ReadFrom (const CL_Stream&);

    bool WriteTo  (CL_Stream&) const;

    // ---------------- Basic methods ----------------------
    
    const char* ClassName() const {return "CL_ByteString";};

    CL_ClassId ClassId() const {return _CL_ByteString_CLASSID;};

    CL_Object* Clone() const {return new CL_ByteString (*this);}
    // Return a copy of this ByteString.
    
    
};


inline void CL_ByteString::operator= (const CL_Object& s)
{
    if (CheckClassType (s, "CL_ByteString::operator= (CL_Object&)"))
        *this = (const CL_ByteString&) s;
}




#endif

