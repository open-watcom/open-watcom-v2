/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  __brktime() is an internal function to convert time to struct tm
*
****************************************************************************/

#include "variety.h"
#include <time.h>
#if defined( __OS2__ )
#include <wos2.h>
#endif
#include "rtdata.h"
#include "thetime.h"
#include "timedata.h"
#include "thread.h"

// #define DAYS_IN_4_YRS   ( 365 + 365 + 365 + 366 )
// #define DAYS_IN_400_YRS ( ( 100 * DAYS_IN_4_YRS ) - 3 )

//  #define SECONDS_PER_DAY ( 24 * 60 * 60 )
//  extern  short   __diyr[], __dilyr[];

/*
 The number of leap years from year 1 to year 1900 is 460.
 The number of leap years from year 1 to current year is
 expressed by "years/4 - years/100 + years/400". To determine
 the number of leap years from current year to 1900, we subtract
 460 from the formula result. We do this since "days" is the
 number of days since 1900.
*/

static unsigned long __DaysToJan1( unsigned year )
{
    unsigned    years = 1900 + year - 1;
    unsigned    leap_days = years / 4 - years / 100 + years / 400 - 460;

    return( year * 365UL + leap_days );
}

/*  __brktime breaks down a calendar time (clock) into a struct tm t */

struct tm *__brktime( unsigned long     days,
                      time_t            wallclock,
                      long              gmtdelta,       // localtime - gmtime
                      struct tm         *t )
{
    unsigned long       secs;
    unsigned            year;
    int                 day_of_year;
    int                 month;
    short const         *month_start;

    /*
        If date is Jan 1, 1970 0:00 to 12:00 UTC and we are west of UTC
        then add a day to wallclock, subtract the gmtdelta value, and
        decrement the calculated days. This prevents local times
        such as "Wed Dec 31 19:00:00 1969 (EST)" from being
        erroneously reported as "Sun Feb 6 01:28:16 2106 (EST)"
        since (wallclock - gmtdelta) wraps (i.e., wallclock < gmtdelta).
    */
    if( wallclock < 12 * 60 * 60UL && gmtdelta > 0 )
        wallclock += SECONDS_PER_DAY, days--; /* days compensated for wallclock one day ahead */
    wallclock -= ( time_t ) gmtdelta;
    days      += wallclock / SECONDS_PER_DAY;
    secs       = wallclock % SECONDS_PER_DAY;
    t->tm_hour = ( int ) ( secs / 3600 ) ;
    secs       = secs % 3600;
    t->tm_min  = ( int ) ( secs / 60 );
    t->tm_sec  = secs % 60;

    // The following two lines are not needed in the current implementation
    // because the range of values for days does not exceed DAYS_IN_400_YRS.
    // Even if it did, the algorithm still computes the correct values.
    //
    //    unsigned  year400s;
    //
    //    year400s = (days / DAYS_IN_400_YRS) * 400;
    //    days %= DAYS_IN_400_YRS;
    //
    // It is OK to reduce days to a value less than DAYS_IN_400_YRS, because
    // DAYS_IN_400_YRS is exactly divisible by 7. If it wasn't divisible by 7,
    // then the following line which appears at the bottom, should be computed
    // before the value of days is range reduced.
    //    t->tm_wday = (days + 1) % 7;                /* 24-sep-92 */
    //
    year = days / 365;
    day_of_year = ( int ) ( days - __DaysToJan1( year ) );
    while( day_of_year < 0 ) {
        --year;
        day_of_year += __leapyear( year + 1900 ) + 365;
    }
    // year += year400s;

    t->tm_yday = day_of_year;
    t->tm_year = ( int ) year;
    month_start = __diyr;
    if( __leapyear( year + 1900 ) )
        month_start = __dilyr;
    month = day_of_year / 31;               /* approximate month */
    if( day_of_year >= month_start[month + 1] )
        ++month;
    t->tm_mon  = month;
    t->tm_mday = day_of_year - month_start[month] + 1;

    /*  Calculate the day of the week */
    /*   Jan 1,1900 is a Monday */

    t->tm_wday = ( days + 1 ) % 7;                /* 24-sep-92 */
    return( t );
}

_WCRTLINK struct tm *_gmtime( const time_t *timer, struct tm *tm )
{
    tm->tm_isdst = 0;          /* assume not */
    return __brktime( DAYS_FROM_1900_TO_1970, *timer, 0L, tm );
}

_WCRTLINK struct tm *gmtime( const time_t *timer )
{
    _INITTHETIME;
    return( _gmtime( timer, &_THE_TIME ) );
}
