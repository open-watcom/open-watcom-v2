
#ifndef _integer_h_
#define _integer_h_





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



// This class is intended to make an integer look like a first-class YACL
// object, complete with notification capabilities. It supports all of the
// usual integer operators as well as the protocol inherited from
// CL_Object.


#include "base/object.h"
#include "base/string.h"
    

#ifdef __GNUC__
#pragma interface
#endif



class __CLASSTYPE CL_Integer: public CL_Object {

public:

    //
    // ------------------------ Construction and destruction -----------------
    //
    CL_Integer (long l)
        {_value = l;}

    CL_Integer ()
        {_value = 0;}

    CL_Integer (const CL_Integer& i)
        {_value = i.Value();};


    ~CL_Integer();


    //
    // ------------------------- Conversions ------------------------------
    //

    long Value() const {return _value;};

    operator long() const {return _value;};

    //
    // Comparison
    //
    bool operator<  (const CL_Object& o) const;

    bool operator<= (const CL_Object& o) const;

    bool operator>  (const CL_Object& o) const;

    bool operator>= (const CL_Object& o) const;

    bool operator== (const CL_Object& o) const;

    bool operator!= (const CL_Object& o) const;

    bool operator<  (const CL_Integer& o) const
        {return _value < o.Value();};

    bool operator<= (const CL_Integer& o) const
         {return _value <= o.Value();};

    bool operator>  (const CL_Integer& o) const
         {return _value > o.Value();};

    bool operator>= (const CL_Integer& o) const
         {return _value >= o.Value();};

    bool operator== (const CL_Integer& o) const
         {return _value == o.Value();};

    bool operator!= (const CL_Integer& o) const
         {return _value != o.Value();};

    short Compare (const CL_Object& o) const;

    short Compare (const CL_Integer& o) const;

    //
    // Arithmetic
    //
    CL_Integer operator+ (const CL_Integer& i) const
        {return CL_Integer(_value+i.Value());};

    CL_Integer operator- (const CL_Integer& i) const
        {return CL_Integer(_value-i.Value());};

    CL_Integer operator* (const CL_Integer& i) const
        {return CL_Integer(_value*i.Value());};

    CL_Integer operator/ (const CL_Integer& i) const
        {return CL_Integer(_value/i.Value());};

    CL_Integer operator% (const CL_Integer& i) const
        {return CL_Integer(_value%i.Value());};

    CL_Integer operator+ (const long& i) const
        {return CL_Integer(_value+i);};

    CL_Integer operator- (const long& i)  const
        {return CL_Integer(_value-i);};

    CL_Integer operator* (const long& i)  const
        {return CL_Integer(_value*i);};

    CL_Integer operator/ (const long& i)  const
        {return CL_Integer(_value/i);};

    CL_Integer operator% (const long& i)  const
        {return CL_Integer(_value%i);};

    //
    // ------------------ Assignments of various kinds --------------------
    //
    virtual void operator= (const CL_Object&);

    virtual void operator= (const CL_String&);

    virtual CL_Integer& operator= (const CL_Integer&);
    

    CL_Integer& operator+= (const CL_Integer& i) 
        {_value = _value+i.Value(); return *this;};

    CL_Integer& operator-= (const CL_Integer& i) 
        {_value = _value-i.Value(); return *this;};

    CL_Integer& operator*= (const CL_Integer& i) 
        {_value = _value*i.Value(); return *this;};

    CL_Integer& operator/= (const CL_Integer& i) 
        {_value = _value/i.Value(); return *this;};

    CL_Integer& operator%= (const CL_Integer& i) 
        {_value = _value%i.Value(); return *this;};

    virtual CL_Integer& operator= (const long& i);

    CL_Integer& operator+= (const long& i) 
        {_value = _value+i; return *this;};

    CL_Integer& operator-= (const long& i) 
        {_value = _value-i; return *this;};

    CL_Integer& operator*= (const long& i) 
        {_value = _value*i; return *this;};

    CL_Integer& operator/= (const long& i) 
        {_value = _value/i; return *this;};

    CL_Integer& operator%= (const long& i) 
        {_value = _value%i; return *this;};



    // --------------- Storage and retrieval ----------------------------

    long StorableFormWidth () const
        { return sizeof (CL_ClassId) + sizeof  _value; };

    bool ReadFrom (const CL_Stream&);

    bool WriteTo  (CL_Stream&) const;

    CL_String AsString () const;

    void FromStream (istream& stream);
    // Override the method inherited from {\small\tt CL_Object}. 

    // --------------------------- Basic methods -------------------
    
    CL_ClassId ClassId() const { return _CL_Integer_CLASSID;};
    
    const char* ClassName() const {return "CL_Integer";};

    CL_Object* Clone() const {return new CL_Integer (*this);};

    // ------------------ End public protocol ----------------------

    

protected:
    long _value;
};



inline short CL_Integer::Compare (const  CL_Object& o) const
{
    return IsA (o) ? Compare ((const CL_Integer&) o)
        : (this < (CL_Integer*) &o ? -1 :  1);
}

inline short CL_Integer::Compare (const  CL_Integer& o) const
{
    return (_value < o._value  ? -1 :  (_value == o._value) ? 0 : 1);
}

inline bool CL_Integer::operator< (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return _value < ((CL_Integer&) o).Value();
}

    

inline bool CL_Integer::operator<= (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return _value <= ((CL_Integer&) o).Value();
}

    

inline bool CL_Integer::operator== (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return _value == ((CL_Integer&) o).Value();
}

    

inline bool CL_Integer::operator>= (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return _value >= ((CL_Integer&) o).Value();
}

    

inline bool CL_Integer::operator> (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return _value > ((CL_Integer&) o).Value();
}

    

inline bool CL_Integer::operator!= (const CL_Object& o) const
{
    if (!IsA (o))
        return TRUE;
    return _value != ((CL_Integer&) o).Value();
}


inline void CL_Integer::operator= (const CL_Object& s)
{
    if (CheckClassType (s, "CL_Integer::operator="))
        *this = (const CL_Integer&) s;
}

    

inline void CL_Integer::operator= (const CL_String& s)
{
    *this = s.AsLong ();
}



#endif
    

