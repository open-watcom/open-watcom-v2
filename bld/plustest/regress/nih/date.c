/* Date.c -- implementation of Gregorian calendar dates

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        Edward M. Persky
        Bg. 12A, Rm. 2031
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-2963
        uucp: uunet!nih-csl!tpersky
        Internet: tpersky@alw.nih.gov

Function:
        Provides an object that contains a date, stored as a Julian Day Number.
        Note: Julian Day Number for Jan. 29, 1988 is not 88029; it is different.
        
log:    DATE.C $
Revision 1.1  92/11/10  13:18:02  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:23  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Date.h"
#include "String.ho"
#include <iomanip.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "nihclIO.h"

#define THIS    Date
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()
#define PARTIAL 0
#define FULL    1

DEFINE_CLASS(Date,2,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\DATE.C 1.1 92/11/10 13:18:02 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_BADMODAY,NIHCL_BADDAYNAM,NIHCL_BADMONAM,NIHCL_BADMONTH,NIHCL_BADDAY,
        NIHCL_RDFAIL;

static const unsigned char days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31 };
static const Date::dayTy first_day_of_month[12] = {1,32,60,91,121,152,182,213,244,274,305,335 };
static const char* month_names[12] = {"January","February","March","April","May","June",
        "July","August","September","October","November","December" };
static const char* uc_month_names[12] = {"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE",
        "JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER" };
static const char* week_day_names[7] = {"Monday","Tuesday","Wednesday",
        "Thursday","Friday","Saturday","Sunday" };
static const char* uc_week_day_names[7] = {"MONDAY","TUESDAY","WEDNESDAY",
        "THURSDAY","FRIDAY","SATURDAY","SUNDAY" };
static const unsigned int seconds_in_day = 24*60*60;

Date::julTy Date::jday(monthTy m, dayTy d, yearTy y)
/*
Convert Gregorian calendar date to the corresponding Julian day number
j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
(Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
This function not valid before that.
*/
{
        unsigned long c, ya;
        if (m > 2)
                m -= 3;
        else {
                m += 9;
                y--;
        } /* else */
        c = y / 100;
        ya = y - 100*c;
        return ((146097*c)>>2) + ((1461*ya)>>2) + (153*m + 2)/5 + d + 1721119;
} /* jday */

void Date::mdy(monthTy& m, dayTy& d, yearTy& y) const
/*
Convert a Julian day number to its corresponding Gregorian calendar
date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
(Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
This function not valid before that.
*/
{
        julTy j = julnum - 1721119;
        y = (yearTy) (((j<<2) - 1) / 146097);
        j = (j<<2) - 1 - 146097*y;
        d = (dayTy)(j>>2);
        j = ((d<<2) + 3) / 1461;
        d = (dayTy)((d<<2) + 3 - 1461*j);
        d = (d + 4)>>2;
        m = (5*d - 3)/153;
        d = 5*d - 3 - 153*m;
        d = (d + 5)/5;
        y = (yearTy)(100*y + j);
        if (m < 10)
                m += 3;
        else {
                m -= 9;
                y++;
        } /* else */
} /* mdy */

Date::Date()
/*
        Construct a Date for today's date.
*/
{
        /* WATCOM long */ time_t clk = time(0);
        const struct tm* now = localtime(&clk);
        julnum = jday(now->tm_mon+1, now->tm_mday, now->tm_year+1900);
}

Date::Date(long dayCount, yearTy referenceYear)
/*
        The base date for this computation is Dec. 31 of the previous year.  That
        is "day zero" in relation to the addition which is to take place.  Therefore,
        we call jday() with the base date and then perform the addition.  Note
        that dayCount may be positive or negative and that this function will
        work the same in either case.
*/
{
        julnum = jday(12, 31, referenceYear-1) + dayCount;  
}


Date::Date(long dayCount)
/*
        Constructs a date with Jan. 1, 1901 as the "day zero".  Date(-1) = Dec. 31, 1900
        and Date(1) = Jan. 2, 1901.
*/
{
        julnum = jday(1, 1, 1901) + dayCount;
}

bool Date::dayWithinMonth(monthTy month, dayTy day, yearTy year)
{
        if (day <= 0) return NO;
        unsigned daysInMonth  = days_in_month[month-1];
        if (leapYear(year) && month == 2) daysInMonth++;
        if (day > daysInMonth) return NO;
        return YES;
}

Date::Date(dayTy day, const char* monthName, yearTy year)
/*
      Construct a Date for the given day, monthName, and year.
*/
{
        monthTy m = numberOfMonth(monthName);
        if (year <= 99) year += 1900;
        if (!dayWithinMonth(m, day, year))
                setError(NIHCL_BADMODAY,DEFAULT,this,day,monthName,year);
        julnum = jday(m, day, year);                    
}

static void skipDelim(istream& strm)
{
        char c;
        if (!strm.good()) return;
        strm >> c;
        while (strm.good() && !isalnum(c)) strm >> c;
        if (strm.good()) strm.putback(c);
}

static const char* parseMonth(istream& strm)
/*
        Parse the name of a month from input stream.
*/
{
        static char month[10];
        register char* p = month;
        char c;
        skipDelim(strm);
        strm.get(c);
        while (strm.good() && isalpha(c) && (p != &month[10])) {
                *p++ = c;
                strm.get(c);
        }
        if (strm.good()) strm.putback(c);
        *p = '\0';
        return month;
}

Date::julTy Date::parseDate(istream& strm)
/*
        Parse a date from the specified input stream.  The date must be in one
        of the following forms: dd-mmm-yy, mm/dd/yy, or mmm dd,yy; e.g.: 10-MAR-86,
        3/10/86, or March 10, 1986.  Any non-alphanumeric character may be used as
        a delimiter.
*/
{
        unsigned d,m,y;
        const char* mon;                // name of month 
        if (strm.good()) {
                skipDelim(strm);
                strm >> m;              // try to parse day or month number 
                skipDelim(strm);
                if (strm.eof()) return 0;
                if (strm.fail()) {      // parse <monthName><day><year> 
                        strm.clear();
                        mon = parseMonth(strm); // parse month name 
                        skipDelim(strm);
                        strm >> d;              // parse day 
                }
                else {                  // try to parse day number 
                        strm >> d;
                        if (strm.eof()) return 0;
                        if (strm.fail()) {      // parse <day><monthName><year> 
                                d = m;
                                strm.clear();
                                mon = parseMonth(strm);         // parse month name 
                        }
                        else {                  // parsed <monthNumber><day><year> 
                                mon = nameOfMonth(m);
                        }
                }
                skipDelim(strm);
                strm >> y;
        }
        if (!strm.good()) return 0;
        return Date(d,mon,y).julnum;
}

Date::Date(istream& strm)       { julnum = parseDate(strm); }

Date::dayTy Date::dayOfWeek(const char* nameOfDay)
/*
        Returns the number, 1-7, of the day of the week named nameOfDay.
*/
{
        {
                String s(nameOfDay);
                register unsigned len = s.size();
                if (len > 2) {
                        s.toUpper();
                        register const char* p = s;
                        for (register unsigned i =0; i<7; i++)
                        if (strncmp(p,uc_week_day_names[i],len)==0) return i+1;
                }
        }
        setError(NIHCL_BADDAYNAM,DEFAULT,nameOfDay);
        return 0;       // never executed 
}

Date::dayTy Date::daysInYear(yearTy year)
/*
        How many days are in the given yearTy year?
*/
{
        if (leapYear(year))
                return 366;
        else
                return 365;
}

Date::monthTy Date::numberOfMonth(const char* nameOfMonth)
/*
        Returns the number, 1-12, of the month named nameOfMonth.
*/
{
        {
                String s(nameOfMonth);
                register unsigned len = s.size();
                if (len > 2) {
                        s.toUpper();
                        register const char* p = s;
                        for (register unsigned i =0; i<12; i++)
                                if (strncmp(p,uc_month_names[i],len)==0) return i+1;
                }
        }
        setError(NIHCL_BADMONAM,DEFAULT,nameOfMonth);
        return 0;       // never executed 
}

bool Date::leapYear(yearTy year)
{
/*
        Algorithm from K & R, "The C Programming Language", 1st ed.
*/
        if ((year&3) == 0 && year%100 != 0 || year % 400 == 0)
                return YES;
        else
                return NO;
}

bool Date::leap() const
{
        return leapYear(year());
}

const char* Date::nameOfMonth(monthTy monthNumber)
/*
        Returns a string name for the month number.
*/
{
        if (monthNumber > 12)
                setError(NIHCL_BADMONTH, DEFAULT, monthNumber);
        return month_names[--monthNumber];
}

const char* Date::nameOfDay(dayTy weekDayNumber)
/*
        Returns a string name for the weekday number.
        Monday == 1, ... , Sunday == 7
*/
{
        if (weekDayNumber > 7)
                setError(NIHCL_BADDAY, DEFAULT, weekDayNumber);
        return week_day_names[--weekDayNumber];
}

const char* Date::nameOfMonth() const   { return nameOfMonth(month()); }

Date::monthTy Date::month() const
{
        monthTy m; dayTy d; yearTy y;
        mdy(m, d, y);
        return m;
}

Date::dayTy Date::firstDayOfMonth(monthTy month) const
{
        if (month > 12)
                setError(NIHCL_BADMONTH, DEFAULT, month);
        unsigned firstDay = first_day_of_month[month-1];
        if (month > 2 && leap()) firstDay++;
        return firstDay;
}

Date Date::previous(const char* nameOfDay) const
{
        dayTy this_day_Of_Week, desired_day_Of_Week;
        julTy j;

//      Set the desired and current day of week to start at 0 (Monday)
//      and end at 6 (Sunday).

        desired_day_Of_Week = dayOfWeek(nameOfDay) - 1; // These functions return a value
        this_day_Of_Week    = weekDay() - 1;            // from 1-7.  Subtract 1 for 0-6.
        j = julnum;

//      Have to determine how many days difference from current day back to
//      desired, if any.  Special calculation under the 'if' statement to
//      effect the wraparound counting from Monday (0) back to Sunday (6).

        if (desired_day_Of_Week > this_day_Of_Week)
                this_day_Of_Week += 7 - desired_day_Of_Week;
        else
                this_day_Of_Week -= desired_day_Of_Week;
        j -= this_day_Of_Week; // Adjust j to set it at the desired day of week.
        return Date(j);
}

Date::dayTy Date::weekDay() const
/*
        Although this seems a little strange, it works.  (julnum + 1) % 7 gives the
        value 0 for Sunday ... 6 for Saturday.  Since we want the list to start at
        Monday, add 6 (mod 7) to this.  Now we have Monday at 0 ... Sunday at
        6.  Simply add 1 to the result to obtain Monday (1) ... Sunday (7).
*/
{
        return ((((julnum + 1) % 7) + 6) % 7) + 1;
}

Date::yearTy Date::year() const
/*
        Returns the year of this Date.
*/
{
        monthTy m; dayTy d; yearTy y;
        mdy(m, d, y);
        return y;
}

Date::dayTy Date::day() const
/*
        Returns the day of the year of this Date.  First we need to find what year we
        are talking about, and then subtract this julnum from the julnum for December 31
        of the preceeding year.
*/
{
        return julnum - jday(12, 31, year()-1);
}

Date::dayTy Date::dayOfMonth() const
{
        monthTy m; dayTy d; yearTy y;
        mdy(m, d, y);
        return d;
}

int Date::compare(const Object& ob) const
{
        assertArgSpecies(ob,classDesc,"compare");
        return julnum - castdown(ob).julnum;
}

void Date::deepenShallowCopy()  {}

unsigned Date::hash() const
{
        return julnum;
}

bool Date::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* Date::species() const { return &classDesc; }

void Date::printOn(ostream& strm) const
{
        strm << setfill(' ') << setw(2) << dayOfMonth() << '-';
        strm.write(nameOfMonth(), 3);
        strm << '-' << setfill('0') << setw(2) << (year() % 100);
}

void Date::scanFrom(istream& strm)      { julnum = parseDate(strm); }

Date::Date(OIOin& strm)
        : BASE(strm)
{
        strm >> julnum;
}

void Date::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << julnum;
}

Date::Date(OIOifd& fd)
        : BASE(fd)
{
        fd >> julnum;
}

void Date::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << julnum;
}




