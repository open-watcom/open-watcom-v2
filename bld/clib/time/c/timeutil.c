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
* Description:  time utility functions
*
****************************************************************************/

#include "variety.h"
#include <time.h>
#include "rtdata.h"
#include "timedata.h"

static int time_less( const struct tm *t1, const struct tm *t2 );

static int calc_yday( const struct tm *timetm, int year )
{
    struct tm   tmptm;
    int         month_days;
    int         first_wday;
    int         nth_week;
    short const *diyr;

    if( timetm->tm_isdst == 0 ) { // M.m.n.d form
        diyr = ( __leapyear( ( unsigned ) year + 1900 ) ) ? __dilyr : __diyr;
        month_days = diyr[timetm->tm_mon + 1] - diyr[timetm->tm_mon]; 
        tmptm.tm_sec   = 0;
        tmptm.tm_min   = 0;
        tmptm.tm_hour  = 0;
        tmptm.tm_mday  = 1;
        tmptm.tm_mon   = timetm->tm_mon;
        tmptm.tm_year  = year;
        tmptm.tm_isdst = 0;
        ( void ) mktime( &tmptm );
        first_wday = ( timetm->tm_wday - tmptm.tm_wday + 7 ) % 7;
        if( timetm->tm_mday == 5 ) {
            if( ( 1 + first_wday + ( timetm->tm_mday - 1 ) * 7 ) > month_days )
                nth_week = timetm->tm_mday - 2;   // fifth req. weekday does not exist
            else 
                nth_week = timetm->tm_mday - 1;
        } else 
            nth_week = timetm->tm_mday - 1;
        return( tmptm.tm_yday + first_wday + nth_week * 7 );
    }
    if( timetm->tm_isdst == 1 )  /* if Jn form */
        return( timetm->tm_yday - 1 );
    return( timetm->tm_yday );
}

/* determine if in souther hemisphere -> start is after end */
static int check_order( const struct tm *start, const struct tm *end, int year )
{
    int start_day;
    int end_day;

    /* these quick checks should always be enough */
    if( ( start->tm_isdst == 0 ) && ( end->tm_isdst == 0 ) ) { // M.m.n.d form
        if( start->tm_mon > end->tm_mon ) 
            return( 1 ); 
        if( start->tm_mon < end->tm_mon ) 
            return( 0 );
    }
    /* start/end of daylight savings time is in the same month (rare case) */
    /* these are *expensive* calculations under NT since 2 TZ checks must be done */
    start_day = calc_yday( start, year );
    end_day = calc_yday( end, year );
    if( start_day > end_day ) 
        return( 1 );
    return( 0 );
}

/* determine if daylight savings time */
int __isindst( struct tm *t )
{
    int                 month;
    int                 dst;
    int                 n1;
    int                 n2;
    int                 month_days;
    int                 time_check;
    int                 south;
    struct tm const     *start;
    struct tm const     *end;
    short const         *diyr;

    // already determined -- if we are sure
    if( t->tm_isdst >= 0 ) 
        return( t->tm_isdst );
    dst = 0;
    // if zone doesn't have a daylight savings period
    if( _RWD_daylight == 0 ) 
        return( t->tm_isdst = dst );
    //  // check for no daylight savings time rule
    //  if( tzname[1][0] == '\0' ) {    // doesn't work since Win32 says
    //      return( t->tm_isdst = dst );// daylight zone name = standard zone name
    //  }

    south = check_order( &_RWD_start_dst, &_RWD_end_dst, t->tm_year );
    if( south ) {
        // if southern hemisphere
        // invert start and end dates and then invert return value
        start = &_RWD_end_dst;
        end = &_RWD_start_dst;
    } else {
        start = &_RWD_start_dst;
        end = &_RWD_end_dst;
    }
    month = t->tm_mon;
    diyr = ( __leapyear( ( unsigned ) t->tm_year + 1900 ) ) ? __dilyr : __diyr;
    month_days = diyr[month + 1] - diyr[month]; 
    time_check = 0;
    /*
     * M.m.n.d form
     * m = start->tm_mon  (month 0-11)
     * n = start->tm_mday (n'th week day 1-5)
     * d = start->tm_wday (week day 0-6)
     */
    if( start->tm_isdst == 0 ) { /* if Mm.n.d form */
        if( month > start->tm_mon ) 
            dst = 1;                        /* assume dst for now */ 
        else if( month == start->tm_mon ) {
            /* calculate for current day */
            n1 = t->tm_mday - ( t->tm_wday + 7 - start->tm_wday ) % 7;
            /* calculate for previous day */
            n2 = t->tm_mday - 1 - ( t->tm_wday - 1 + 7 - start->tm_wday ) % 7;
            //  n_ stands for the day of the month that is past &&
            //  is closest to today && is the required weekday
            if( start->tm_mday == 5 ) {
                if( n1 > month_days - 7 ) {
                    dst = 1;                /* assume dst for now */
                    if( n2 <= month_days - 7 ) 
                        time_check = 1;
                }
            } else {
                if( n1 >= 7 * ( start->tm_mday - 1 ) + 1 ) {
                    dst = 1;                /* assume dst for now */
                    if( n2 < 7 * ( start->tm_mday - 1 ) + 1 ) 
                        time_check = 1;
                }
            }
        }
    } else {
        n1 = start->tm_yday;
        if( start->tm_isdst == 1 ) { /* if Jn form */
            if( __leapyear( ( unsigned ) t->tm_year + 1900 ) ) {
                if( n1 > __diyr[2] )
                    n1++;      /* past Feb 28 */
            }
            n1--;
        }
        if( t->tm_yday >= n1 ) {
            dst = 1;                        /* assume dst for now */
            if( t->tm_yday == n1 )
                time_check = 1;
        }
    }
    /* if it is the day for a switch-over then check the time too */
    if( time_check )
        dst = !time_less( t, start );

    /* if we are certain that it is before daylight saving then return */
    if( dst == 0 ) {
        if( south )
            dst = south - dst;  /* invert value of dst */
        return( t->tm_isdst = dst );
    }

    /* now see if it is after daylight saving */
    time_check = 0;
    if( end->tm_isdst == 0 ) { /* if Mm.n.d form */
        if( month > end->tm_mon ) 
            dst = 0;                        /* not dst */ 
        else if( month == end->tm_mon ) {
            dst = 0;
            /* calculate for current day */
            n1 = t->tm_mday - ( t->tm_wday + 7 - end->tm_wday ) % 7;
            /* calculate for previous day */
            n2 = t->tm_mday - 1 -
                ( t->tm_wday - 1 + 7 - end->tm_wday ) % 7;
            if( end->tm_mday == 5 ) {
                if( n1 <= month_days - 7 ) 
                    dst = 1; 
                else if( n2 <= month_days - 7 ) 
                    time_check = 1;
            } else {
                if( n1 < 7 * ( end->tm_mday - 1 ) + 1 ) 
                    dst = 1; 
                else if( n2 < 7 * ( end->tm_mday - 1 ) + 1 ) 
                    time_check = 1;
            }
        }
    } else {
        n1 = end->tm_yday;
        if( end->tm_isdst == 1 ) { /* if Jn form */
            if( __leapyear( ( unsigned ) t->tm_year + 1900 ) ) {
                if( n1 > __diyr[2] )
                    n1++;      /* past Feb 28 */
            }
            n1--;
        }
        if( t->tm_yday >= n1 ) {
            dst = 0;
            if( t->tm_yday == n1 )
                time_check = 1;
        }
    }
    /* if it is the day for a switch-over then check the time too */
    if( time_check )
        dst = time_less( t, end );
    if( south )
        dst = south - dst;      /* invert value of dst */
    return( t->tm_isdst = dst );
}

static int time_less( const struct tm *t1, const struct tm *t2 )
{
    int before;

    before = 0;
    if( t1->tm_hour < t2->tm_hour ) 
        before = 1; 
    else if( t1->tm_hour == t2->tm_hour ) {
        if( t1->tm_min < t2->tm_min
        ||  t1->tm_min == t2->tm_min && t1->tm_sec < t2->tm_sec )
                before = 1;
    }
    return( before );
}
