

#ifndef _timeofday_h_
#define _timeofday_h_





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




// This is an encapsulation of a particular time of day. The printable
// form of a TimeOfDay is one of the following:
// \par{\tt\begin{tabular}{ll}
//            Time_Normal    &    1:30 pm   \\
//            Time_Military  &    13:30:23  \\
// \end{tabular}}



#include "base/string.h"


#ifdef __GNUC__
#pragma interface
#endif

enum TimePrintForm { Time_Normal, Time_Military};

class __CLASSTYPE CL_TimeOfDay: public CL_Object {

public:

    //
    // ------------- Constructors and destructors ----------------------
    //

    CL_TimeOfDay ();
    // Construct an "empty" time object, representing midnight

    CL_TimeOfDay (short hour, short minute, short second);
    // Construct the given time, assuming that the hour is in the
    // range 0 to 23.

    CL_TimeOfDay (const CL_String& s);
    // Construct a time of day from the given string, assuming that it
    // contains a representation in one of Time_Normal or
    // Time_Military forms, or else either four digits (hhmm) or six
    // digits (hhmmdd).
    
    CL_TimeOfDay (long nsecs);
    // Construct a TimeOfDay with the given number of seconds since
    // midnight.
    
    CL_TimeOfDay (const CL_TimeOfDay&);
    // Copy constructor

    ~CL_TimeOfDay ();
    // Destructor

    //
    // ---------------------- Access ----------------------------------
    //

    short Hour() const;
    // Return our hour.

    short Minute () const;
    // Return our minute.

    short Second () const;
    // Return our second.

    

    CL_String PrintString (TimePrintForm form = Time_Normal) const;
    // Return a printable form of ourselves, according to the
    // parameter.


    //
    // --------------------- Comparison ------------------------------
    //

    bool operator<  (const CL_TimeOfDay&) const;

    bool operator<= (const CL_TimeOfDay&) const;

    bool operator>  (const CL_TimeOfDay&) const;

    bool operator>= (const CL_TimeOfDay&) const;

    bool operator== (const CL_TimeOfDay&) const;

    bool operator!= (const CL_TimeOfDay&) const;

    bool operator<  (const CL_Object& obj) const;

    bool operator<= (const CL_Object& obj) const;

    bool operator>  (const CL_Object& obj) const;

    bool operator>= (const CL_Object& obj) const;

    bool operator== (const CL_Object& obj) const;

    bool operator!= (const CL_Object& obj) const;

    short Compare (const CL_TimeOfDay&) const;

    short Compare (const CL_Object&) const;


    //
    // -------------------------- Modification ----------------------
    //

    // Assignment
    
    CL_TimeOfDay& operator= (const CL_TimeOfDay&);

    virtual void operator= (const CL_String&);
    // Convert the given string into a TimeOfDay via ParseAndConvert() (see
    // below), and assign the result to ourselves. If the strins is not
    // syntactically correct, the time is set to midnight.

    virtual void operator= (const CL_Object&);
    // Check that the given object is a TimeOfDay (via ClassId()), downcast
    // it and assign it to ourselves.

    //
    // -------------------------- TimeOfDay arithmetic -------------------
    //

    
    // add or subtract the given number of seconds
    
    CL_TimeOfDay  operator+  (long num_secs) const;

    CL_TimeOfDay& operator+= (long num_secs);

    CL_TimeOfDay  operator-  (long num_secs) const;

    CL_TimeOfDay& operator-= (long num_secs);

    long operator-   (const CL_TimeOfDay&) const;
    // Return the number of seconds between us and the given time.


    //
    // ------------------------ Static  methods --------------------------
    //

    static CL_TimeOfDay Now ();
    // Return a time object containing the current time

    static long ParseAndConvert (const CL_String& s);
    // Validate the given string as representing a given time of day,
    // and return either -1 if it's not a valid time, or else the
    // number of seconds since midnight. The given string must
    // contain a time representation in one of Time_Normal or
    // Time_Military forms, or else either four digits (hhmm) or six
    // digits (hhmmdd).

    // ----------------- Saving and restoration --------------------

    void FromStream (istream&);
    // Override the method inherited from {\small\tt CL_Object}. The
    // implementation skips fill characters, and them collects numerics and
    // ':' characters; the result is then assumed to be in Time_Military
    // form and parsed.
    
    long StorableFormWidth () const;
    // Override the method inherited from {\small\tt CL_Object}.

    bool ReadFrom (const CL_Stream&);
    // Override the method inherited from {\small\tt CL_Object}.

    bool WriteTo  (CL_Stream&) const;
    // Override the method inherited from {\small\tt CL_Object}.

    CL_String AsString () const;
    // Override the method inherited from {\small\tt CL_Object}.


    // ----------------------- Basic methods --------------

    const char* ClassName() const { return "CL_TimeOfDay";};
    // Return the class name of this object
    
    CL_ClassId ClassId () const {return _CL_TimeOfDay_CLASSID;};
    // Return the class id of this object

    CL_Object* Clone () const {return new CL_TimeOfDay (*this);};
    
    //
    // -------------------- End public protocol ---------------------
    //
    

protected:

    // The representation used is the number of seconds since midnight:
    long _numSecs;
};




//-------------------------Comparison operations-------------------------

// return 1 if we are less than the TimeOfDay object passed to us; 
// return 0 otherwise;

inline bool CL_TimeOfDay::operator< (const CL_TimeOfDay& a_time) const
{
    return (_numSecs < a_time._numSecs);
}

// return 1 if we are less or equal to the TimeOfDay object passed to us; 
// return 0 otherwise;

inline bool CL_TimeOfDay::operator<= (const CL_TimeOfDay& a_time) const
{
    return (_numSecs <= a_time._numSecs);
}


// return 1 if we are greater than the TimeOfDay object passed to us; 
// return 0 otherwise;
    
inline bool CL_TimeOfDay::operator> (const CL_TimeOfDay& a_time) const
{
    return (_numSecs > a_time._numSecs);
}

// return 1 if we are greater than or equal to the TimeOfDay object passed; 
// return 0 otherwise;

inline bool CL_TimeOfDay::operator>= (const CL_TimeOfDay& a_time) const
{
    return (_numSecs >= a_time._numSecs);
}

// return 1 if we are equal to the TimeOfDay object passed to us; 
// return 0 otherwise;


inline bool CL_TimeOfDay::operator== (const CL_TimeOfDay& a_time) const
{
    return (_numSecs == a_time._numSecs);
}

// return 1 if we are not equal to the TimeOfDay object passed to us; 
// return 0 otherwise;

inline bool CL_TimeOfDay::operator!= (const CL_TimeOfDay& a_time) const
{
    return (_numSecs  != a_time._numSecs);
}

inline CL_String CL_TimeOfDay::AsString() const
{
    return PrintString (Time_Military);
}


inline void CL_TimeOfDay::operator= (const CL_Object& obj)
{
    if (CheckClassType (obj, "CL_TimeOfDay::op= (CL_Object&)"))
        *this = (const CL_TimeOfDay&) obj;
}




inline short CL_TimeOfDay::Compare (const CL_Object& obj) const
{
    if (!IsA (obj))
        return (this < (CL_TimeOfDay*) &obj ? -1 :  1);
    return Compare ((const CL_TimeOfDay&) obj);
}


inline short CL_TimeOfDay::Compare (const CL_TimeOfDay& d) const
{
    return _numSecs < d._numSecs ? -1 : (_numSecs == d._numSecs ? 0 : 1);
}





#endif
