
#ifndef _date_h_
#define _date_h_





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



// This class represents a particular date. Only dates on or after
// Jan 1, 1900 are permitted. (Under Unix, only dates after Jan 1,
// 1970 are permitted.) The class provides for conversion to and from a
// string representation in one of the following forms, encoded by
// the {\tt DatePrintForm} enumeration, as illustrated below:
// \par\medskip{\tt \begin{tabular}{ll}
//            Date_American   &   October 17, 1992\\
//            Date_Terse      &   17-Oct-1992\\
//            Date_Numbers    &   10/17/92\\
//            Date_European   &   17 October 1992\\
//            Date_EuroNumbers&   17/10/92\\
// \end{tabular}
// }\par


#include "base/defs.h"
#include "base/string.h"


#ifdef __GNUC__
#pragma interface
#endif

enum DatePrintForm { Date_American, Date_Terse, Date_Numbers,
                 Date_European, Date_EuroNumbers };

class __CLASSTYPE CL_Date: public CL_Object {

public:

    enum MonthEnum { InvalidMonth = 0, January, February, March, April,
                     May, June, July, August, September, October,
                     November, December};

    enum WeekdayEnum {InvalidDay = 0, Sunday, Monday, Tuesday, Wednesday,
                      Thursday, Friday, Saturday};
    
    //
    // ------------- Constructors and destructors ----------------------
    //

    CL_Date ();
    // Construct an "empty" date object: one that represents Jan 1, 1901.

    CL_Date (short year, MonthEnum month, short day);
    // Construct the given date, assuming that the year is given as 1992,
    // not 92, and the month is indexed from 1 (e.g. January is month 1).

    CL_Date (short year, const CL_String& month, short day);
    // Construct the given date, assuming the month specified as a
    // string, e.g., "March". Assume case-sensitive comparison and
    // completely-specified month names (e.g. "Oct" is not allowed).

    CL_Date (const CL_String& s);
    // Construct a date from the given string containing the
    // representation, in one of the forms Date_American, Date_Terse,
    // or Date_Numbers.
    

    CL_Date (const CL_Date&);
    // Copy constructor

    ~CL_Date ();
    // Destructor

    //
    // ---------------------- Access ----------------------------------
    //

    bool IsLegal () const;
    // Is this a legal date?
    
    short Year() const;
    // Return our year.

    MonthEnum Month () const;
    // Return our month.

    short Day () const;
    // Return our day of month.

    WeekdayEnum DayOfWeek () const;
    // Return our day of week. Assume that 1 is for Sunday and 7 for
    // Saturday.

    short DaysInMonth () const;
    // Return the number of days in our month.
    
    CL_String PrintString (DatePrintForm form = Date_American) const;
    // Return a printable form of this object, according to the
    // parameter.



    
    bool IsLeapYear () const;
    // Tell if ours is a leap year.


    CL_Date NextWeekday (const CL_String& weekday_name) const;
    // Return the date of the next weekday given.


    CL_Date PreviousWeekday (const CL_String& weekday_name) const;
    // Return the date of the previous weekday given.
    
    CL_Date NextWeekday (WeekdayEnum weekday_num) const;
    // Return the date of the next weekday given.

    CL_Date PreviousWeekday (WeekdayEnum weekday_num) const;
    // Return the date of the previous weekday given.
    

    //
    // --------------------- Comparison ------------------------------
    //

    bool operator<  (const CL_Date&) const;
    
    bool operator<= (const CL_Date&) const;
    
    bool operator>  (const CL_Date&) const;

    bool operator>= (const CL_Date&) const;

    bool operator== (const CL_Date&) const;

    bool operator!= (const CL_Date&) const;

    bool operator<  (const CL_Object& obj) const;

    bool operator<= (const CL_Object& obj) const;

    bool operator>  (const CL_Object& obj) const;

    bool operator>= (const CL_Object& obj) const;

    bool operator== (const CL_Object& obj) const;

    bool operator!= (const CL_Object& obj) const;

    short Compare (const CL_Date&) const;

    short Compare (const CL_Object&) const;

    //
    // -------------------------- Modification ----------------------
    //

    // Assignment
    
    virtual void operator= (const CL_Date&);
    // Assign the given date to this object.

    virtual void operator= (const CL_String&);
    // Assign our value from a string. The string must be in one of
    // the forms of Date_American, Date_Terse, or Date_Numbers, or
    // else must be six digits (characters) of the form yymmdd or
    // mmddyy. In the former three cases, the year can be either of
    // the form 92 or 1992.

    //
    // -------------------------- Date arithmetic -------------------
    //

    
    
    virtual CL_Date  operator+  (short num_days) const;
    // Add the given number of days and return the result.

    virtual CL_Date& operator+= (short num_days);
    // Add the given number of days.

    virtual CL_Date  operator-  (short num_days) const;
    // Subtract the given number of days and return the result.

    virtual CL_Date& operator-= (short num_days);
    // Subtract the given number of days.

    virtual long operator-   (const CL_Date& date) const;
    // Return the number of days between us and the given date (which
    // may be positive or negative).
    
    CL_Date AddMonths (short num_months) const;
    // Return the Date obtained by adding {\tt num_months} to this Date's
    // month. The parameter can be positive or negative.

    CL_Date AddYears (short num_years) const;
    // Return the Date obtained by adding {\tt num_years} to this Date's
    // year. The parameter can be positive or negative.

    //
    // --------------------- Static methods ---------------------------
    //

    static CL_Date Today ();
    // Return a date object with today's date in it.

    static CL_String DayName (WeekdayEnum weekday_num);
    // Return name of given weekday.
    // Return the empty string if the parameter is invalid.

    static WeekdayEnum  DayNumber (const CL_String& weekday_name);
    // Do the opposite of the {\tt DayName}. Return -1 for an invalid name.

    static CL_String MonthName (MonthEnum month_num);
    // Return month name of given month (1 = "January", etc.). Return the
    // empty string for an invalid month number.

    static CL_String ShortMonthName (MonthEnum month_num);
    // Return a 3-character month name of given month.
    // Return the empty string for an invalid month number. 

    static MonthEnum MonthNumber (const CL_String& month_name);
    // And the opposite of the above. Return -1 for an invalid month name.

    static bool IsLeapYear (short year);
    // Is the given year a leap year?

    static short DaysInMonth  (MonthEnum month, short year);
    // Return the number of days in the given month and year.
    
    static long ParseAndConvert (const CL_String& date);
    // Validate the given string as representing a date, and return
    // either -1 if it's not a valid date, or else the number of days
    // it represents since Jan 1, 1900. The string must be in one of
    // the forms of Date_American, Date_Terse, or Date_Numbers, or
    // else must be six digits (characters) of the form yymmdd or
    // mmddyy. In the former three cases, the year can be either of
    // the form 92 or 1992. The month names can be mixed case.


    // ----------------------- Storage and restoration ----------------
    
    CL_String AsString () const
        { return PrintString (Date_Numbers);};
    // Overrides the method inherited from {\small\tt CL_Object}.


    void FromStream (istream& strm);
    // Overrides the method inherited from {\small\tt CL_Object}. The
    // implementation skips fill characters, and then collects slashes,
    // dashes, and alphabetic and numeric 
    // characters and attempts to parse the result as a date. If successful,
    // this date is modified; otherwise, the collected characters are put
    // back and this date becomes invalid.
    
    long StorableFormWidth () const;
    // Overrides the method inherited from {\small\tt CL_Object}.

    bool ReadFrom (const CL_Stream&);
    // Overrides the method inherited from {\small\tt CL_Object}.

    bool WriteTo  (CL_Stream&) const;
    // Overrides the method inherited from {\small\tt CL_Object}.


    // -------------------------- Assignment ------------------------
    

    virtual void operator= (const CL_Object&);
    // Overrides the method inherited from {\small\tt CL_Object}.

    // ------------------------ Basic methods ----------------------
    
    const char* ClassName() const { return "CL_Date";};
    // Overrides the method inherited from {\small\tt CL_Object}.
    
    CL_ClassId ClassId () const {return _CL_Date_CLASSID;};
    // Overrides the method inherited from {\small\tt CL_Object}.

    CL_Object* Clone() const {return new CL_Date (*this);};
    // Overrides the method inherited from {\small\tt CL_Object}.


    
protected:

    // The representation used is the number of days since Jan 1,
    // 1901:
    long _days;
    
    CL_Date (long num_days);
    
};




inline void CL_Date::operator= (const CL_Object& obj)
{
    if (CheckClassType (obj, "CL_Date::op= (CL_Object&)"))
        *this = (const CL_Date&) obj;
}



inline short CL_Date::Compare (const CL_Object& obj) const
{
    if (!IsA (obj))
        return (this < (CL_Date*) &obj ? -1 :  1);
    return Compare ((const CL_Date&) obj);
}


inline bool CL_Date::IsLegal () const
{
    return _days > 0;
}


#endif
