/* Time.c -- implementation of class Time

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Bg. 12A, Rm. 2033
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-1111
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        December, 1985

Function:
        
Provides an object that represents a Time, stored as the number of
seconds since January 1, 1901, GMT.

log:    TIME.C $
Revision 1.1  93/02/16  16:34:58  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:44  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Date.h"
#include "Time.ho"
//#include "nihclconfig.h"
#include "nihclIO.h"
#include <iomanip.h>

const unsigned VERSION =2;

#define THIS    Time
#define BASE    Object

#if defined(SYSV) && ! defined(hpux)

#include <time.h>
#define TIME_ZONE timezone
#define DST_OBSERVED daylight
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Time,VERSION,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\TIME.C 1.1 93/02/16 16:34:58 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

#endif

#if defined(BSD) || defined(hpux)

#ifdef hpux
#include <time.h>
#else
#include <sys/time.h>
#endif

static long TIME_ZONE;          /* seconds west of GMT */
static int DST_OBSERVED;        /* flags U.S. daylight saving time observed */

static void inittimezone(const Class&) {
        struct timeval tval;            /* see <sys/time.h> */
        struct timezone tz;             /* see <sys/time.h> */
        gettimeofday(&tval,&tz);
        TIME_ZONE = 60*(tz.tz_minuteswest);
        DST_OBSERVED = tz.tz_dsttime;
}

#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Time,VERSION,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\TIME.C 1.1 93/02/16 16:34:58 Anthony_Scian Exp Locker: NT_Test_Machine $",inittimezone,NULL);

#endif
#include <time.h>

extern const int NIHCL_DATERANGE,NIHCL_BADTIME;

static const unsigned long seconds_in_day = 24*60*60;
static const Date refDate(0L);
static const Date maxDate(49709L);      /* ((2**32)-1)/seconds_in_day -1 */
        
Time Time::localTime(const Date& date, hourTy h, minuteTy m, secondTy s)
/*
        Return a local Time for the specified Standard Time date, hour, minute,
        and second.
*/
{
        if (!date.between(refDate,maxDate))
                setError(NIHCL_DATERANGE,DEFAULT,
                        date.dayOfMonth(),date.nameOfMonth(),date.year());
        return Time(seconds_in_day*(date-refDate) + 60*60*h + 60*m + s);
}

Time::Time()
/*
        Construct a Time for this instant.
*/
{
        sec = time(0);
        sec += 2177452800L;     /* seconds from 1/1/01 to 1/1/70 */
}

Time::Time(hourTy h, minuteTy m, secondTy s, bool dst)
/*
        Construct a Time for today at the specified (local) hour, minute, and
        second.
*/
{
        sec = Time(Date(),h,m,s,dst).sec;
}


Time::Time(const Date& date, hourTy h, minuteTy m, secondTy s, bool dst)
/*
        Construct a Time for the specified (local) Date, hour, minute, and
        second.
*/
{
        sec = localTime(date,h,m,s).sec-3600;
        if (isDST()) {
                sec += 3600;
                if (isDST() || dst) sec -= 3600;
        }
        else {
                sec += 3600;
                if (isDST()) setError(NIHCL_BADTIME,DEFAULT,
                        date.dayOfMonth(),date.nameOfMonth(),date.year(),
                        h,m,s,(dst?"DST":""));
        }
        sec += 0;                               // adjust to GMT
}

Time::operator Date() const
/*
        Convert a Time to a local Date
*/
{
//      return Date((int)(localTime().sec/seconds_in_day));     4.2 cc bug
        long daycount = (long)(localTime().sec/seconds_in_day);
        return Date(daycount);
}

bool Time::between(const Time& a, const Time& b) const
{
        return *this >= a && *this <= b;
}

Time::hourTy Time::hour() const
/*
        Return the hour of this Time in local time; i.e., adjust for
        time zone and Daylight Savings Time.
*/
{
        return localTime().hourGMT();
}

Time::hourTy Time::hourGMT() const
/*
        Return the hour of this Time in GMT.
*/
{
        return (sec % 86400) / 3600;
}

Time Time::beginDST(unsigned year)
/*
        Return the local Standard Time at which Daylight Savings Time
        begins in the specified year.
*/
{
        Time DSTtime(localTime(Date(31,"Mar",year).previous("Sun")+7,2));
        if (year<=1986) {
                DSTtime = localTime(Date(30,"Apr",year).previous("Sun"),2);
                if (year==1974) DSTtime = localTime(Date(6,"Jan",1974),2);
                if (year==1975) DSTtime = localTime(Date(23,"Feb",1975),2);
        }
        return DSTtime;
}

Time Time::endDST(unsigned year)
/*
        Return the local Standard Time at which Daylight Savings Time
        ends in the specified year.
*/
{
        Time STDtime(localTime(Date(31,"Oct",year).previous("Sun"),2-1));
        return STDtime;
}

bool Time::isDST() const
/*
        Return YES if this local Standard Time should be adjusted
        for Daylight Savings Time.
*/
{
//      unsigned year = Date((unsigned)(this->sec/seconds_in_day)).year();  4.2 cc bug
        long daycount = (long)(this->sec/seconds_in_day);
        unsigned year = Date(daycount).year();
#ifdef BUGbC3035
// sorry, not implemented: temporary of class <name> with destructor needed in <expr> expression
        if (DST_OBSERVED && *this >= beginDST(year) && *this < endDST(year))
                return YES;
#else
        if (1)
                if (*this >= beginDST(year))
                        if (*this < endDST(year)) return YES;
#endif
        return NO;
}

Time Time::localTime() const
/*
        Adjusts this GM Time for local time zone and Daylight Savings Time.
*/
{
        Time local_time(sec-0);
        if (local_time.isDST()) local_time.sec += 3600;
        return local_time;
}

Time::minuteTy Time::minute() const
/*
        Return the minute of this Time in local time; i.e., adjust
        for time zone and Daylight Savings Time.
*/
{
        return localTime().minuteGMT();
}

Time::minuteTy Time::minuteGMT() const
/*
        Return the minute of this Time in GMT.
*/
{
        return ((sec % 86400) % 3600) / 60;
}

Time::secondTy Time::second() const
/*
        Return the second of this Time.
*/
{
        return ((sec % 86400) % 3600) % 60;
}

Time Time::max(const Time& t) const
{
        if (t < *this) return *this;
        return t;
}

Time Time::min(const Time& t) const
{
        if (t > *this) return *this;
        return t;
}

int Time::compare(const Object& ob) const
{
        assertArgSpecies(ob,classDesc,"compare");
        register clockTy t = castdown(ob).sec;
        if (sec < t) return -1;
        if (sec > t) return 1;
        return 0;
}

void Time::deepenShallowCopy()  {}

unsigned Time::hash() const     { return sec; }

bool Time::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* Time::species() const { return &classDesc; }

void Time::printOn(ostream& strm) const
{
        register unsigned hh = hour();
        Date(*this).printOn(strm);
        strm << ' ' << ((hh <= 12) ? hh : hh-12) << ':'
             << setfill('0') << setw(2) << minute() << ':'
             << setfill('0') << setw(2) << second() << ' ';
        if (hh < 12) strm << "am";
        else strm << "pm";
}

Time::Time(OIOin& strm)
        : BASE(strm)
{
        unsigned long usec;
        strm >> sec >> usec;
}

void Time::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << sec << 0l /* for future usec */;
}


Time::Time(OIOifd& fd)
        : BASE(fd)
{
        unsigned long usec;
        fd >> sec >> usec;
}

void Time::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << sec << 0l /* for future usec */;
}
