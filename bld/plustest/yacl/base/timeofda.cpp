



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
#include <ctype.h>
#include <time.h>
#include <stdlib.h>


#include "base/timeofda.h"
#include "base/bytstrng.h"
#include "base/string.h"
#include "base/error.h"
#include "base/stream.h"

#ifdef __GNUC__
#pragma implementation
#endif


// This constant is used of out of bound error checking ( 24 hrs * 3600 secs)

#define MAX_SECS 86400L

CL_DEFINE_CLASS(CL_TimeOfDay, _CL_TimeOfDay_CLASSID);


//-------------------Constructors and Destructors----------------------

CL_TimeOfDay::CL_TimeOfDay()
{
    _numSecs = 0;
}

CL_TimeOfDay::CL_TimeOfDay (long nsecs)
{
    _numSecs = nsecs % MAX_SECS;
}



CL_TimeOfDay CL_TimeOfDay::Now ()
{
    time_t long_secs;
    struct tm *time_struct_ptr;
    long_secs = time(NULL);
    time_struct_ptr = localtime(&long_secs);
    return CL_TimeOfDay (time_struct_ptr->tm_hour * 3600L +
               time_struct_ptr->tm_min * 60L +
               time_struct_ptr->tm_sec);
}


CL_TimeOfDay::CL_TimeOfDay (short hour, short min, short sec)
{
    _numSecs = hour * 3600L + min * 60 + sec;
    if (! (hour >= 0 && hour < 24) ) {
        CL_Error::Warning
             ("CL_TimeOfDay constructor: invalid hour %d", hour);
        _numSecs = 0;
    }
    if (! (min >= 0 && min < 60) ) {
        CL_Error::Warning
             ("CL_TimeOfDay constructor: invalid minutes %d", min);
        _numSecs = 0;
    }
    if (! (sec >= 0 && sec < 60) ) {
        CL_Error::Warning
             ("CL_TimeOfDay constructor: invalid seconds %d", sec);
        _numSecs = 0;
    }
}

CL_TimeOfDay::CL_TimeOfDay (const CL_String& s)
{
    *this = s;
}

void CL_TimeOfDay::operator= (const CL_String& s)
{
    if (!PrepareToChange())
        return;
    if (s.Size() == 0) {
        _numSecs = 0;
        Notify ();
        return;
    }
    long l = ParseAndConvert (s);
    if (l < 0) {
        // CL_Error::Warning ("CL_TimeOfDay::op= (const CL_String&): "
        //                    "Invalid form '%s'", (char*) s);
        _numSecs = 0;
    }
    else
        _numSecs = l;
    Notify ();
}


long CL_TimeOfDay::ParseAndConvert (const CL_String& s)
{
    CL_String fld[4];
    short hour, min, sec;

    short n;
    if ((n = s.Split (fld, 4, ":")) == 2 || n == 3) {
        hour = (short) fld[0].AsLong ();
        min  = (short) fld[1].AsLong ();
        sec  = (short) fld[2].AsLong ();
    }
    else if (s.Split (fld, 4, ": ") == 3) {
        hour = (short) fld[0].AsLong ();
        min  = (short) fld[1].AsLong ();
        sec = 0;
        if (fld[2] == "pm")
            hour += 12;
    }
    else {
        // Assume that it's either four or six digits, and try to parse it.
        for (short i = 0; i < s.Size(); i++) {
            if (s[i] > '9' || s[i] < '0') {
                return -1;
            }
        }
        hour = (s[0]-'0')*10 + s[1]-'0';
        min  = (s[2]-'0')*10 + s[3]-'0';
        if (s.Size() == 6) {
            sec  = (s[4]-'0')*10 + s[5]-'0';
        }
        else
            sec = 0;
    }
    if (! (hour >= 0 && hour < 24) ) {
        return -1;
    }
    if (! (min >= 0 && min < 60) ) {
        return -1;
    }
    if (! (sec >= 0 && sec < 60) ) {
        return -1;
    }
    return (hour * 3600L + min * 60 + sec);
}    

        
        
        


CL_TimeOfDay::CL_TimeOfDay (const CL_TimeOfDay& a_time)
{
    _numSecs = a_time._numSecs;
}


CL_TimeOfDay::~CL_TimeOfDay()
{
}




short CL_TimeOfDay::Hour () const
{
    return _numSecs/3600;
}


short CL_TimeOfDay::Minute() const
{
    return (_numSecs%3600)/60;
}


short CL_TimeOfDay::Second() const
{
    return (_numSecs%3600)%60; 
}



//-----------------------Printable string conversion ----------------



CL_String CL_TimeOfDay::PrintString (TimePrintForm form) const
{
    CL_String time_string;
    long hour = Hour();
    long min = Minute();
    long sec = Second();
    switch (form) {
    case Time_Military:
    {
        CL_String smin (min, 2, '0'), ssec (sec, 2, '0');
        time_string =  CL_String(hour, 2, '0') + ":" + smin + ":" + ssec;
        break;
    }

    
    case Time_Normal:
    default:
    {
        CL_String tmin (min, 2);
        if (hour > 12) {
            hour -= 12;
            time_string = CL_String(hour) + ":" + tmin + " pm";
        }
        else if (hour == 12)
            time_string = CL_String(hour) + ":" + tmin + " pm";
        else
            time_string = CL_String(hour) + ":" + tmin + " am";
        break;
    }
        
    }
    return time_string;
}





//----------------------Modification---------------------------------


CL_TimeOfDay& CL_TimeOfDay::operator= (const CL_TimeOfDay& a_time)
{
    if (this == &a_time || !PrepareToChange())
        return *this;
    _numSecs = a_time._numSecs;
    Notify ();
    return *this;
}

//-------------------TimeOfDay arithmetic---------------------------


CL_TimeOfDay CL_TimeOfDay::operator+ (long seconds) const
{
    CL_TimeOfDay new_time;
    new_time._numSecs = _numSecs + seconds;
    if (new_time._numSecs >= MAX_SECS)
        new_time._numSecs -= MAX_SECS;
    return new_time;
}



CL_TimeOfDay& CL_TimeOfDay::operator+= (long seconds)
{
    if (!PrepareToChange())
        return *this;
    _numSecs += seconds;
    if (_numSecs >= MAX_SECS)
        _numSecs -= MAX_SECS;
    Notify ();
    return *this;
}

 

CL_TimeOfDay CL_TimeOfDay::operator- (long seconds) const
{
    CL_TimeOfDay new_time;
    new_time._numSecs = _numSecs - seconds;
    if (new_time._numSecs < 0)
        new_time._numSecs += MAX_SECS;
    return new_time;
}


CL_TimeOfDay& CL_TimeOfDay::operator-= (long seconds)
{
    if (!PrepareToChange())
        return *this;
    _numSecs -= seconds;
    if (_numSecs < 0)
        _numSecs += MAX_SECS;
    Notify ();
    return *this;
}



long CL_TimeOfDay::operator- (const CL_TimeOfDay& a_time) const
{
    return (_numSecs - a_time._numSecs);
}










void CL_TimeOfDay::FromStream (istream& s)
{
    CL_String rep;
    char c;
    long count = 0;
    char fill_char = s.fill ();
    
    while (s.get (c) && c == fill_char);
    if (!s.good() || s.eof()) {
        _numSecs = 0;
        return;
    }
    do {
        if (isalnum (c) || c == ':') {
            rep.Append (c);
            count++;
        }
        else
            break;
    } while (s.get (c));

    long n = ParseAndConvert (rep);
    if (n > 0) {
        if (!s.eof())
            s.putback (c);
        _numSecs = n;
    }
    else {
        s.seekg (s.tellg() - count, istream::cur);
        _numSecs = 0;
    }
}




bool CL_TimeOfDay::ReadFrom (const CL_Stream& s)
{
    if (!PrepareToChange() || !ReadClassId (s) || !s.Read (_numSecs))
        return FALSE;
    Notify();
    return TRUE;
}



bool CL_TimeOfDay::WriteTo (CL_Stream& s) const
{
    return s.Write (ClassId())  && s.Write (_numSecs);
}

long CL_TimeOfDay::StorableFormWidth() const
{
    return sizeof (CL_ClassId) + sizeof (long);
}



bool CL_TimeOfDay::operator<  (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return *this < ((const CL_TimeOfDay&) o);
}


bool CL_TimeOfDay::operator<= (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return *this <= ((const CL_TimeOfDay&) o);
}


bool CL_TimeOfDay::operator>  (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return *this > ((const CL_TimeOfDay&) o);
}


bool CL_TimeOfDay::operator>= (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return *this >= ((const CL_TimeOfDay&) o);
}



bool CL_TimeOfDay::operator== (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return *this == ((const CL_TimeOfDay&) o);
}



bool CL_TimeOfDay::operator!= (const CL_Object& o) const
{
    if (!IsA (o))
        return FALSE;
    return *this != ((const CL_TimeOfDay&) o);
}






#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#include "base/cmparatr.h"
#include "base/basicops.h"
#include "base/binding.h"

template class CL_Binding<CL_TimeOfDay>;
template class CL_Comparator<CL_TimeOfDay>;
template class CL_Basics<CL_TimeOfDay>;
#endif
