



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



//
// A class representing a particular date. Only dates on or after
// Jan 1, 1901 are permitted.
//
// Author: M. A. Sridhar
// Date:   November 9th, 1992

#include "base/date.h"
#include "base/error.h"
#include "base/bytstrng.h"
#include "base/stream.h"

#include <iostream.h>
#include <ctype.h>

#include <time.h>
#ifdef MSDOS
#include <dos.h>
#endif


#ifdef __GNUC__
#pragma implementation
#endif



// A few static (local) helper functions: IntoDays and IntoYMD
struct DateStruct {
    short y,d;
    CL_Date::MonthEnum m;
};

static DateStruct IntoYMD (long num_days);
static long       IntoDays (const DateStruct&);



CL_DEFINE_CLASS(CL_Date, _CL_Date_CLASSID);

//
// ------------- Constructors and destructors ----------------------
//

// Construct today's date
CL_Date::CL_Date ()
{
    _days = 0;
}


/*---------------------------------------------------*/

// Construct the given date, assuming that:
//        the year is given as 1992, not 92
//        the month is indexed from 1 (e.g. January is month 1)
CL_Date::CL_Date (short year, MonthEnum month, short day)
{
    if (!(year >= 1901)) {
        CL_Error::Warning ("CL_Date constructor: invalid year %d", year);
        _days = 0;
        return;
    }
    if (!(month >= January && month <= December)) {
        CL_Error::Warning ("CL_Date constructor: invalid month %d",
                           month);
        _days = 0;
        return;
    }
    if (day < 1 || day > DaysInMonth (month, year)) {
        CL_Error::Warning ("CL_Date constructor: invalid day %d", day);
        _days = 0;
        return;
    }
    
    DateStruct my_date;
    my_date.y = year;
    my_date.m = month;
    my_date.d = day;
    _days = IntoDays (my_date);
}


/*---------------------------------------------------*/


// Construct the given date, assuming the month specified as a
// string, e.g., "March". Assume case-sensitive comparison and
// completely-specified month names (e.g. "Oct" is not allowed).
CL_Date::CL_Date (short year, const CL_String& month, short day)
{
    DateStruct my_date;
    my_date.y = year;
    my_date.m = MonthNumber (month);
    my_date.d = day;
    _days = IntoDays (my_date);
}


/*---------------------------------------------------*/

// Construct a date from the given string containing the
// representation in one of the allowed forms.
CL_Date::CL_Date (const CL_String& s)
{
    *this = s;
}

void CL_Date::operator= (const CL_String& s)
{
    if (!PrepareToChange())
        return;
    if (s.Size() == 0) {
        _days = 0;
        Notify ();
        return;
    }
    long d = ParseAndConvert (s);
    if (d < 0) {
        _days = 0;
        Notify ();
        return;
    }
    _days = d;
    Notify ();
}


long CL_Date::ParseAndConvert (const CL_String& s)
{
    CL_String fld[4];
    DateStruct dt;

    if (s.Split (fld, 4, " ,") == 3) { // Date_American
        fld[0].WordCapitalize ();
        dt.m = MonthNumber (fld[0]);
        short yr = (short) fld[2].AsLong();
        dt.y = (yr >= 1900) ? yr : yr + 1900;
        dt.d = (short) fld[1].AsLong();
    }
    else if (s.Split (fld, 4, "-") == 3) { // Date_Terse
        fld[1].WordCapitalize ();
        dt.m = MonthNumber (fld[1]);
        short yr = (short) fld[2].AsLong();
        dt.y = (yr >= 1900) ? yr : yr + 1900;
        dt.d = (short) fld[0].AsLong();
    }
    else if (s.Split (fld, 4, "/") == 3) {// Date_Numbers
        dt.m = (MonthEnum) fld[0].AsLong();
        dt.y = (short) fld[2].AsLong() + 1900;
        dt.d = (short) fld[1].AsLong();
    }
    else {
        // Assume that it's six digits, and try to parse it.
        if (s.Size() != 6) {
            return -1;
        }
        for (short i = 0; i < 6; i++) {
            if (s[i] > '9' || s[i] < '0') {
                return -1;
            }
        }
        short f1 = (s[0]-'0')*10 + s[1]-'0';
        short f2 = (s[2]-'0')*10 + s[3]-'0';
        short f3 = (s[4]-'0')*10 + s[5]-'0';
        if (f1 >= 13) { // Assume yymmdd
            dt.m = (MonthEnum) f2;
            dt.y = f1 + 1900;
            dt.d = f3;
        }
        else { // Assume mmddyy
            dt.m = (MonthEnum) f1;
            dt.y = f3 + 1900;
            dt.d = f2;
        }
    }
    if (dt.m <= 0 || dt.m >= 13) {
        return -1;
    }
    if (dt.y <= 1899) {
        return -1;
    }
    if (dt.d <= 0 || dt.d >= 32) {
        return -1;
    }
    
    return IntoDays (dt);
}

    
/*---------------------------------------------------*/
// Copy constructor
CL_Date::CL_Date (const CL_Date& d)
{
    _days = d._days;
}


/*---------------------------------------------------*/


// Destructor
CL_Date::~CL_Date ()
{
}


/*---------------------------------------------------*/


//
// ---------------------- Access ----------------------------------
//

// Return our year
short CL_Date::Year() const
{
    return IntoYMD (_days).y;
}


/*---------------------------------------------------*/


    

// Return our month
CL_Date::MonthEnum CL_Date::Month () const
{
    return IntoYMD (_days).m;
}

/*---------------------------------------------------*/



// Return our day of month
short CL_Date::Day () const
{
    return IntoYMD (_days).d;
}


/*---------------------------------------------------*/


// Return our day of week. Assume that 1 is for Sunday and 7 for
// Saturday.
CL_Date::WeekdayEnum CL_Date::DayOfWeek () const
{
    // Take advantage of the fact that Jan 1, 1901 was a Tuesday.
    return (WeekdayEnum) (((2 + _days) % 7) + 1);
}



/*---------------------------------------------------*/

short CL_Date::DaysInMonth () const
{
    DateStruct dt = IntoYMD (_days);
    return DaysInMonth (dt.m, dt.y);
}

/*---------------------------------------------------*/


CL_String CL_Date::PrintString (DatePrintForm form) const
{
    if (_days == 0) // Invalid date
        return "";
    DateStruct my_date = IntoYMD (_days);

    CL_String s, t;
    switch (form) {

    case Date_American:
        s =  MonthName (my_date.m) + " "
            + CL_String (my_date.d) + ", "
            + CL_String (my_date.y);
        return s;

    case Date_Terse:
        t = CL_String ((long) my_date.d, 2) + "-";
        t = t + ShortMonthName (my_date.m)  + "-";
        t = t + CL_String ((long) my_date.y);
        return t;

    case Date_Numbers:
        return CL_String ((long) my_date.m, 2, '0') + "/"
            + CL_String ((long) my_date.d, 2, '0') + "/"
            + CL_String ((long) my_date.y - 1900L, 2);

    case Date_European:
        return CL_String ((long) my_date.d) + " "
            + MonthName (my_date.m) + " "
            +  CL_String ((long) my_date.y);

    case Date_EuroNumbers:
        return CL_String ((long) my_date.d, 2) + "/"
            + CL_String ((long) my_date.m, 2) + "/"
            + CL_String ((long) my_date.y - 1900L);

    default:
        // CL_Error::Warning ("CL_Date::PrintString: Invalid form %d", form);
        return "";
    }
}






/*---------------------------------------------------*/



void CL_Date::FromStream (istream& s)
{
    CL_String rep;
    char c;
    long count = 0;
    
    char fill_char = s.fill ();
    
    while (s.get (c) && c == fill_char);
    if (!s.good() || s.eof()) {
        _days = 0;
        return;
    }
    do {
        if (isalnum (c) || c == '/' || c == '-') {
            rep.Append (c);
            count++;
        }
        else
            break;
    } while (s.get (c));

    long n = ParseAndConvert (rep);
    if (n > 0) {
        if (!s.eof ())
            s.putback (c);
        _days = n;
    }
    else {
        s.seekg (s.tellg() - count, istream::cur);
        _days = 0;
    }
}

/*---------------------------------------------------*/



// Tell if ours is a leap year

bool CL_Date::IsLeapYear () const
{
    return IsLeapYear (IntoYMD (_days).y);
}


/*---------------------------------------------------*/


// Return the date of the next weekday given

CL_Date CL_Date::NextWeekday (const CL_String& weekday_name) const
{
    short incr = DayNumber (weekday_name) - DayOfWeek();
    return CL_Date (_days + ((incr >= 0) ? incr : incr + 7));
}


/*---------------------------------------------------*/


// Return the date of the previous weekday given

CL_Date CL_Date::PreviousWeekday (const CL_String& weekday_name) const
{
    short incr = DayNumber (weekday_name) - DayOfWeek();
    return CL_Date (_days - ((incr > 0) ? 7 - incr : -incr));
}


CL_Date CL_Date::NextWeekday (WeekdayEnum weekday_num) const
{
    short incr = weekday_num - DayOfWeek();
    return CL_Date (_days + ((incr >= 0) ? incr : incr + 7));
}


/*---------------------------------------------------*/


// Return the date of the previous weekday given

CL_Date CL_Date::PreviousWeekday (WeekdayEnum weekday_num) const
{
    short incr = weekday_num - DayOfWeek();
    return CL_Date (_days - ((incr > 0) ? 7 - incr : -incr));
}



//
// --------------------- Comparison ------------------------------
//

bool CL_Date::operator<  (const CL_Date& d) const
{
    return _days < d._days;
}


/*---------------------------------------------------*/

bool CL_Date::operator<= (const CL_Date& d) const
{
    return _days <= d._days;
}

/*---------------------------------------------------*/


bool CL_Date::operator>  (const CL_Date& d) const
{
    return _days > d._days;
}

/*---------------------------------------------------*/


bool CL_Date::operator>= (const CL_Date& d) const
{
    return _days >= d._days;
}


/*---------------------------------------------------*/

bool CL_Date::operator== (const CL_Date& d) const
{
    return _days == d._days;
}


/*---------------------------------------------------*/

bool CL_Date::operator!= (const CL_Date& d) const
{
    return _days != d._days;
}


/*---------------------------------------------------*/



//
// -------------------------- Modification ----------------------
//

// Assignment

void CL_Date::operator= (const CL_Date& d)
{
    if (this == &d)
        return;
    if (!PrepareToChange())
        return;
    _days = d._days;
    Notify ();   // Notify all our clients
}




//
// -------------------------- Date arithmetic -------------------
//


// add or subtract the given number of days

CL_Date  CL_Date::operator+  (short num_days) const
{
    return CL_Date (_days + num_days);
}


/*---------------------------------------------------*/

CL_Date& CL_Date::operator+= (short num_days) 
{
    if (!PrepareToChange())
        return *this;
    _days += num_days;
    Notify();
    return *this;
}


/*---------------------------------------------------*/

CL_Date  CL_Date::operator-  (short num_days) const
{
    return CL_Date (_days - num_days);
}


/*---------------------------------------------------*/

CL_Date& CL_Date::operator-= (short num_days)
{
    if (!PrepareToChange())
        return *this;
    _days -= num_days;
    Notify();
    return *this;
}



/*---------------------------------------------------*/

// Return the number of days between us and the given date (which
// may be positive or negative)
long CL_Date::operator-   (const CL_Date& date) const
{
    return _days - date._days;
}


CL_Date CL_Date::AddMonths (short num_months) const
{
    DateStruct dt = IntoYMD (_days);
    long new_months = dt.y * 12 + dt.m + num_months - 1;
    short ny = new_months / 12;
    if (ny < 1901) {
        CL_Error::Warning ("CL_Date::AddMonths: parameter %d too large",
                           num_months);
        return *this;
    }
    MonthEnum nm = (MonthEnum) (new_months % 12 + 1);
    return CL_Date (ny, nm, minl (dt.d, CL_Date::DaysInMonth (nm, ny)));
}


        
CL_Date CL_Date::AddYears (short num_years) const
{
    DateStruct dt = IntoYMD (_days);
    return CL_Date (dt.y + num_years, dt.m, dt.d);
}





//
// --------------------- Static methods ---------------------------
//

#if defined (__GNUC__)
extern "C" time_t time (time_t*);
#endif

CL_Date CL_Date::Today ()
{
    time_t timer;
    struct tm *tblock;
    DateStruct my_date;

    timer = time ((time_t*) NULL);
    tblock = localtime (&timer);
    my_date.y = tblock->tm_year + 1900; 
    my_date.m = (MonthEnum) (tblock->tm_mon + 1); // Correct for 0-11
    my_date.d = tblock->tm_mday;
    return CL_Date (IntoDays (my_date));
}



static char* week_day [7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday"
    };
// Return name of given weekday (1 = "Sunday", ..., 7 = "Saturday")
CL_String CL_Date::DayName (WeekdayEnum weekday_num)
{
    if (Sunday <= weekday_num && weekday_num <= Saturday)
        return CL_String (week_day [weekday_num - Sunday]);
    else return "";
}


/*---------------------------------------------------*/


// Do the opposite of the above:
CL_Date::WeekdayEnum  CL_Date::DayNumber (const CL_String& weekday_name)
{
    for (short i = 0; i < 7; i++) {
        if (weekday_name == week_day[i]) return (WeekdayEnum) (i+1);
    }
//     CL_Error::Warning ("CL_Date::DayNumber: Invalid weekday name '%s'",
//                      (const char*) weekday_name);
    return InvalidDay;
}



/*---------------------------------------------------*/

// Return month name of given month (1 = "January", etc.)
static char* months [12] = {
    "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
    };

static char* short_months [12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
    "Aug", "Sep", "Oct", "Nov", "Dec"
    };

CL_String CL_Date::MonthName (MonthEnum month_num)
{
    return (month_num >= 1 && month_num <= 12) ?
        months[month_num-1] : "";
}


/*---------------------------------------------------*/
CL_String CL_Date::ShortMonthName (MonthEnum month_num)
{
    return (month_num >= 1 && month_num <= 12) ? 
        (short_months[month_num-1]) : ""; 
}


/*---------------------------------------------------*/

// And the opposite of the above:
CL_Date::MonthEnum CL_Date::MonthNumber (const CL_String& month_name)
{
    short i;
    for (i = 0; i < 12; i++) {
        if (month_name == months[i]) return (MonthEnum) (i+1);
    }
    for (i = 0; i < 12; i++) {
        if (month_name == short_months[i]) return (MonthEnum) (i+1);
    }
    return InvalidMonth;
}


/*---------------------------------------------------*/

// Is the given year a leap year?
bool CL_Date::IsLeapYear (short year)
{
    return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) != 0);
}



/*---------------------------------------------------*/


long CL_Date::StorableFormWidth () const
{
    return sizeof (CL_ClassId) + sizeof (long);
}


bool CL_Date::ReadFrom (const CL_Stream& s)
{
    if (!PrepareToChange() || !ReadClassId (s) ||
        !s.Read (_days))
        return FALSE;
    Notify();
    return TRUE;
}



bool CL_Date::WriteTo (CL_Stream& s) const
{
    return s.Write (ClassId())  && s.Write (_days);
}




bool CL_Date::operator<  (const CL_Object& obj) const
{
    if (!IsA (obj))
        return FALSE;
    return operator< ((const CL_Date&) obj);
}


bool CL_Date::operator<= (const CL_Object& obj) const
{
    if (!IsA (obj))
        return FALSE;
    return operator<= ((const CL_Date&) obj);
}


bool CL_Date::operator>  (const CL_Object& obj) const
{
    if (!IsA (obj))
        return FALSE;
    return operator> ((const CL_Date&) obj);
}


bool CL_Date::operator>= (const CL_Object& obj) const
{
    if (!IsA (obj))
        return FALSE;
    return operator>= ((const CL_Date&) obj);
}


bool CL_Date::operator== (const CL_Object& obj) const
{
    if (!IsA (obj))
        return FALSE;
    return operator== ((const CL_Date&) obj);
}


bool CL_Date::operator!= (const CL_Object& obj) const
{
    if (!IsA (obj))
        return FALSE;
    return operator!= ((const CL_Date&) obj);
}



short CL_Date::Compare (const CL_Date& d) const
{
    return _days < d._days ? -1 : (_days == d._days ? 0 : 1);
}



// Protected methods

CL_Date::CL_Date (long num_days)
{
    _days = num_days;
}




//
// Helper functions
//


static short month_days[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

short CL_Date::DaysInMonth (MonthEnum month, short year)
{
    if (month != February || ! CL_Date::IsLeapYear (year))
        return (month >= January && month <= December)
            ? month_days[month-1] : -1;
    else
        return 29;
}


static DateStruct IntoYMD   (long num_days)
{
    DateStruct date;
    long temp_days;

    temp_days = num_days - (num_days / 1460)   
                         + (num_days / 36500)
                         - (num_days / 146000);

    date.y = (short) (temp_days / 365);
    num_days -= (date.y * 365L) + (date.y / 4) - date.y/100 + date.y /
        400;
    date.y += 1901;
    num_days++; // Correct for: Jan 1, 1901 has days = 0
    // Set number of days for February
    CL_Date::MonthEnum i;
    for (i = CL_Date::January; i <= CL_Date::December; ((int&) i)++) {
        short ndays = CL_Date::DaysInMonth (i, date.y);
        if (num_days > ndays)
            num_days -= ndays;
        else {
            date.m = i;
            date.d = (short) num_days;
            break;
        }
    }
    return date;
}

        

/*---------------------------------------------------*/

    
static long       IntoDays  (const DateStruct& s)
{
    long n, p;
    CL_Date::MonthEnum i;

    if (s.m < CL_Date::January || s.m > CL_Date::December)
        return 0;
    if (s.y < 1901 || s.y > 10000) // Wonder if this library will be around
                                   // after 10,000 AD? <grin>
        return 0;
    if (s.d <= 0 || s.d > CL_Date::DaysInMonth (s.m, s.y))
        return 0;
    for (i = CL_Date::January, p = 0; i <= s.m - 1; ((int&) i)++)
        p += CL_Date::DaysInMonth (i, s.y);
    n =  (s.y - 1901L) * 365 + p + s.d - 1;
        // Jan 1, 1901 gives 0 days
    n += (s.y - 1901L) / 4 + 3 * (s.y - 1901L) / 400;
        // Add number of leap years between 1901 and s.y
    return n;
}



#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#include "base/cmparatr.h"
#include "base/basicops.h"
#include "base/binding.h"

template class CL_Binding<CL_Date>;
template class CL_Comparator<CL_Date>;
template class CL_Basics<CL_Date>;
#endif
