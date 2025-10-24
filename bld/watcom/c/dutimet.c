/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  conversion function for DOS date/time stamps (UTC timezone)
*
****************************************************************************/


#ifdef __WATCOMC__
    #if defined( BOOTSTRAP )
        #define IMPLEMENT_MKGMTIME
    #endif
#elif defined( _MSC_VER )
#else
    #if defined( __OSX__ )
        #define IMPLEMENT_MKGMTIME
    #else
        #define _DEFAULT_SOURCE
        #define _BSD_SOURCE
        #define _mkgmtime timegm
    #endif
#endif

#include "dutimet.h"


enum {
    TIME_SEC_B  = 0,
    TIME_SEC_F  = 0x001f,
    TIME_MIN_B  = 5,
    TIME_MIN_F  = 0x07e0,
    TIME_HOUR_B = 11,
    TIME_HOUR_F = 0xf800
};

enum {
    DATE_DAY_B  = 0,
    DATE_DAY_F  = 0x001f,
    DATE_MON_B  = 5,
    DATE_MON_F  = 0x01e0,
    DATE_YEAR_B = 9,
    DATE_YEAR_F = 0xfe00
};

/****************************************************************************
*
* Description:  Implementation of _mkgmtime.
*
****************************************************************************/

#ifdef IMPLEMENT_MKGMTIME

#define SECONDS_FROM_1900_TO_1970       2208988800UL
#define SECONDS_PER_DAY                 (24UL * 60UL * 60UL)
#define DAYS_FROM_1900_TO_1970          (SECONDS_FROM_1900_TO_1970 / SECONDS_PER_DAY)

enum {
    TIME_SEC_B  = 0,
    TIME_SEC_F  = 0x001f,
    TIME_MIN_B  = 5,
    TIME_MIN_F  = 0x07e0,
    TIME_HOUR_B = 11,
    TIME_HOUR_F = 0xf800
};

enum {
    DATE_DAY_B  = 0,
    DATE_DAY_F  = 0x001f,
    DATE_MON_B  = 5,
    DATE_MON_F  = 0x01e0,
    DATE_YEAR_B = 9,
    DATE_YEAR_F = 0xfe00
};

static short const month_start_days[] = {
    0,                                                          /* Jan */
    31,                                                         /* Feb */
    31 + 28,                                                    /* Mar */
    31 + 28 + 31,                                               /* Apr */
    31 + 28 + 31 + 30,                                          /* May */
    31 + 28 + 31 + 30 + 31,                                     /* Jun */
    31 + 28 + 31 + 30 + 31 + 30,                                /* Jul */
    31 + 28 + 31 + 30 + 31 + 30 + 31,                           /* Aug */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,                      /* Sep */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,                 /* Oct */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,            /* Nov */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,       /* Dec */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31   /* Jan, next year */
};

static int is_leapyear( unsigned year )
{
    if( year & 3 )
        return( 0 );
    if( ( year % 100 ) != 0 )
        return( 1 );
    if( ( year % 400 ) == 0 )
        return( 1 );
    return( 0 );
}

static unsigned long years_days( unsigned year )
{
    return( year * 365L                         /* # of days in the years */
        + ( ( year + 3L ) / 4L )                /* add # of leap years before year */
        - ( ( year + 99L ) / 100L )             /* sub # of leap centuries */
        + ( ( year + 399L - 100L ) / 400L ) );  /* add # of leap 4 centuries */
                                                /* adjust for 1900 offset */
                                                /* note: -100 == 300 (mod 400) */
}

static time_t _mkgmtime( struct tm *t )
/*********************************************
 * used internaly then no checks to simplify
 * it suppose tm structure contains valid data
 */
{
    unsigned long   days;
    unsigned        month_start;

    month_start = month_start_days[t->tm_mon];
    if( t->tm_mon > 1
      && is_leapyear( t->tm_year + 1900U ) ) {
        month_start++;
    }
    days = years_days( t->tm_year ) /* # of days in the years + leap years days */
        + month_start               /* # of days to 1st of month*/
        + t->tm_mday - 1;           /* day of the month */
    if( days < ( DAYS_FROM_1900_TO_1970 - 1 ) )
        return( (time_t)-1 );
    return( ( days - DAYS_FROM_1900_TO_1970 ) * SECONDS_PER_DAY
            + ( t->tm_hour * 60UL + t->tm_min ) * 60UL + t->tm_sec );
}

#endif  /* IMPLEMENT_MKGMTIME */

time_t _INTERNAL __dosu2timet( unsigned short dos_date, unsigned short dos_time )
/*******************************************************************************/
{
    struct tm       t;

    t.tm_year  = ((dos_date & DATE_YEAR_F) >> DATE_YEAR_B) + 80;
    t.tm_mon   = ((dos_date & DATE_MON_F) >> DATE_MON_B) - 1;
    t.tm_mday  = (dos_date & DATE_DAY_F) >> DATE_DAY_B;

    t.tm_hour  = (dos_time & TIME_HOUR_F) >> TIME_HOUR_B;
    t.tm_min   = (dos_time & TIME_MIN_F) >> TIME_MIN_B;
    t.tm_sec   = ((dos_time & TIME_SEC_F) >> TIME_SEC_B) * 2;

    t.tm_wday  = -1;
    t.tm_yday  = -1;
    t.tm_isdst = -1;

    return( _mkgmtime( &t ) );
}

#ifndef DUTIMET_ONLY_READ
time_t _INTERNAL __timet2dosu( time_t stamp )
/*******************************************/
{
    struct tm       *t;
    unsigned short  dos_time;
    unsigned short  dos_date;

    t = gmtime( &stamp );

    dos_date = ( ( t->tm_year - 80 ) << DATE_YEAR_B )
             | ( ( t->tm_mon + 1 ) << DATE_MON_B )
             | ( t->tm_mday << DATE_DAY_B );
    dos_time = ( ( t->tm_hour ) << TIME_HOUR_B )
             | ( t->tm_min << TIME_MIN_B )
             | ( ( t->tm_sec / 2 ) << TIME_SEC_B );
    return( dos_date * 0x10000UL + dos_time );
}
#endif
