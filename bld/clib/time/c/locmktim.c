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
* Description:  mktime() without timezone and struct tm fields adjustment.
*               used by mktime() and DOS clock()
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "rtdata.h"
#include "timedata.h"

#define MONTH_YR        ( 12 )
#define DAY_YR          ( 365 )
#define HOUR_YR         ( DAY_YR * 24 )
#define MINUTE_YR       ( HOUR_YR * 60 )
#define SECOND_YR       ( MINUTE_YR * 60 )
#define __MONTHS        ( INT_MIN / MONTH_YR )
#define __DAYS          ( INT_MIN / DAY_YR )

// these ones can underflow in 16bit environments,
// so check the relative values first
#if ( HOUR_YR ) < ( INT_MAX / 60 )
 #define __MINUTES      ( INT_MIN / MINUTE_YR )
 #if ( MINUTE_YR ) < ( INT_MAX / 60 )
  #define __SECONDS     ( INT_MIN / SECOND_YR )
 #else
  #define __SECONDS     ( 0 )
 #endif
#else
 #define __MINUTES      ( 0 )
 #define __SECONDS      ( 0 )
#endif

#define SMALLEST_YEAR_VALUE ( __MONTHS + __DAYS + __MINUTES + __SECONDS )

time_t __local_mktime( const struct tm *t, long *pdays, long *pseconds )
{
    int         month;
    int         year;
    long        days;
    long        seconds;
    short const *month_start;

    month_start = __diyr;
    month = t->tm_mon % 12; /* put tm_mon into range */
    year = t->tm_year;
    if( year < SMALLEST_YEAR_VALUE )
        return( ( time_t ) -1 );
    year += t->tm_mon / 12;
    while( month < 0 )
        --year, month += 12;
    if( year < 0 )
        return( ( time_t ) -1 );
    if( __leapyear( ( unsigned ) ( year + 1900 ) ) )
        month_start = __dilyr;
    days = year * 365L                   /* # of days in the years */
        + ( ( year + 3 ) / 4 )           /* add # of leap years before year */
        - ( ( year + 99 ) / 100 )        /* sub # of leap centuries */
        + ( ( year + 399 - 100 ) / 400 ) /* add # of leap 4 centuries */
                                         /* adjust for 1900 offset */
                                         /* note: -100 == 300 (mod 400) */
        + month_start[month]             /* # of days to 1st of month*/
        + t->tm_mday - 1;                /* day of the month */
    seconds = ( ( ( long ) ( t->tm_hour ) ) *60L + ( long ) ( t->tm_min ) ) *60L + t->tm_sec;
                                         /* seconds needs to be positive for __brktime */
    while( seconds < 0 )
        days -= 1, seconds += ( long ) SECONDS_PER_DAY;
    while( seconds >= ( long ) SECONDS_PER_DAY )
        days += 1, seconds -= ( long ) SECONDS_PER_DAY;
    if( days < ( DAYS_FROM_1900_TO_1970 - 1 ) )
        return( ( time_t ) -1 );
    if ( pdays ) *pdays = days;
    if ( pseconds ) *pseconds = seconds;
    return( seconds + ( days - DAYS_FROM_1900_TO_1970 ) * ( long ) SECONDS_PER_DAY );
}
