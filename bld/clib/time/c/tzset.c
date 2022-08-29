/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Platform independent tzset() implementation.
*
****************************************************************************/

#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#ifdef __NT__
 #include <windows.h>
#endif
#include "rtdata.h"
#include "timedata.h"
#include "exitwmsg.h"
#include "_time.h"

/*
 * TZSET - sets the values of the variables 'timezone', 'daylight'
 *         and 'tzname' according to the setting of the environment
 *         variable "TZ". The "TZ" variable has the format
 *
 * LINUX    :characters
 *              or
 * POSIX    <std><offset>[<dst>[<offset>]][,<start>[/<time>],<end>[/<time>]]]
 *              or
 * OS/2     <std><offset><dst>,<sm>,<sw>,<sd>,<st>,<em>,<ew>,<ed>,<et>,<shift>
 *
 *          the alternate OS/2 format is evaluated, if after scanning
 *          the previous format the next char is a ','
 */

//#define TZNAME_MAX    128     /* defined in <limits.h> */

struct tm       __start_dst = { /* start of daylight savings */
    0, 0, 2,                    /* M4.1.0/02:00:00 default */
    1, 3, 0,                    /* i.e., 1st Sunday of Apr */
    0, 0, 0
};

struct tm       __end_dst = {   /* end of daylight savings */
    0, 0, 1,                    /* M10.5.0/02:00:00 default */
    5, 9, 0,                    /* i.e., last Sunday of Oct */
    0, 0, 0                     /* note that this is specified*/
};                              /* in terms of EST */
/* i.e. 02:00 EDT == 01:00 EST*/

static char     stzone[TZNAME_MAX + 1] = "EST";     /* place to store names */
static char     dtzone[TZNAME_MAX + 1] = "EDT";     /* place to store names */

_WCRTDATA char  *_WCDATA tzname[2] = { stzone, dtzone };

_WCRTDATA long  _WCDATA timezone = 5L * 60L * 60L;  /* seconds from GMT */
_WCRTDATA int   _WCDATA daylight = 1;               /* d.s.t. indicator */
int             __dst_adjust = 60 * 60;             /* daylight adjustment */

static struct {
    unsigned    cache_OS_TZ : 1;
    unsigned    have_OS_TZ : 1;
}               tzFlag = { 1, 0 };

int __DontCacheOSTZ( void )
/*************************/
{
    int old_flag;

    old_flag           = tzFlag.cache_OS_TZ;
    tzFlag.cache_OS_TZ = 0;
    tzFlag.have_OS_TZ  = 0;
    return( old_flag );
}

int __CacheOSTZ( void )
/*********************/
{
    int old_flag;

    old_flag           = tzFlag.cache_OS_TZ;
    tzFlag.cache_OS_TZ = 1;
    tzFlag.have_OS_TZ  = 0;
    return( old_flag );
}

static char *parse_sign( char *tz, int *sign )
/********************************************/
{
    int neg;

    neg = 0;
    if( *tz == '-' ) {
        ++tz;
        neg = 1;
    } else if( *tz == '+' ) {
        ++tz;
    }
    *sign = neg;
    return( tz );
}

static char *parse_number( char *tz, int *val )
/*********************************************/
{
    int value;

    value = 0;
    while( *tz >= '0' && *tz <= '9' ) {
        value = value * 10 + *tz - '0';
        ++tz;
    }
    *val = value;
    return( tz );
}

static char *parse_time( char *tz, int *hours, int *minutes, int *seconds )
/*************************************************************************/
{
    tz = parse_number( tz, hours );
    if( *tz == ':' ) {
        tz = parse_number( tz + 1, minutes );
        if( *tz == ':' ) {
            tz = parse_number( tz + 1, seconds );
        }
    }
    return( tz );
}

static char *parse_name( char *tz, char *name )
/*********************************************/
{
    int         len;
    char        ch;
    char        *tzstart;
    int         quoted;

    /*
     * parse time zone name (should be 3 or more characters)
     * examples:    PST8, EDT+6, Central Standard Time+7:00:00
     *              <-03>5
     */
    quoted = 0;
    if( *tz == '<' ) {
        tz++;
        quoted = 1;
    }
    tzstart = tz;
    for( ;; ) {
        ch = *tz;
        if( ch == '\0' )
            break;
        if( quoted ) {
            if( ch == '>' ) {
                break;
            }
        } else {
            if( ch == ',' )
                break;
            if( ch == '-' )
                break;
            if( ch == '+' )
                break;
            if( ch >= '0' && ch <= '9' ) {
                break;
            }
        }
        ++tz;
    }
    len = tz - tzstart;
    if( len > TZNAME_MAX )
        len = TZNAME_MAX;
    memcpy( name, tzstart, (size_t)len );
    name[len] = '\0';
    if( quoted && ch == '>' ) {
        tz++;
    }
    return( tz );
}

static char *parse_offset( char *tz, long *offset )
/*************************************************/
{
    int         hours;
    int         minutes;
    int         seconds;
    int         neg;
    char        ch;

    tz = parse_sign( tz, &neg );
    ch = *tz;
    if( ch >= '0' && ch <= '9' ) {
        hours = minutes = seconds = 0;
        tz = parse_time( tz, &hours, &minutes, &seconds );
        *offset = seconds + ( ( minutes + ( hours * 60 ) ) * 60L );
        if( neg ) {
            *offset = -*offset;
        }
    }
    return( tz );
}

static char *parse_rule( char *tz, struct tm *timeptr )
/*****************************************************/
{
    int         date_form;
    int         days;
    int         hours;
    int         minutes;
    int         seconds;

    date_form = -1;                         /* n  0-365 */
    if( *tz == 'J' ) {
        /*
         * Jn 1-365 (no leap days)
         */
        date_form = 1;
        tz++;
    }
    if( *tz == 'M' ) {
        /*
         * Mm.n.d n'th day of month
         */
        date_form = 0;
        tz++;
    }
    timeptr->tm_isdst = date_form;
    tz = parse_number( tz, &days );
    if( date_form != 0 ) {  /* J form */
        /*
         * day of year 0-365
         */
        timeptr->tm_yday = days;
    } else {                /* M form */
        /*
         * month 1-12
         */
        timeptr->tm_mon = days - 1;
        if( *tz == '.' ) {
            tz = parse_number( tz + 1, &days );
            /*
             * week of month 1-5
             */
            timeptr->tm_mday = days;
            if( *tz == '.' ) {
                /*
                 * day of week 0-6
                 */
                tz = parse_number( tz + 1, &days );
                timeptr->tm_wday = days;
            }
        }
        timeptr->tm_yday = 0;
    }

    hours = 2;
    minutes = seconds = 0;
    if( *tz == '/' ) {
        tz = parse_time( tz, &hours, &minutes, &seconds );
    }
    timeptr->tm_sec = seconds;
    timeptr->tm_min = minutes;
    timeptr->tm_hour = hours;
    return( tz );
}

static void fix_rule( struct tm *tm, int adjust )
/***********************************************/
{
    /*
     * convert rule to be in terms of Standard Time
     * rather than Daylight Time
     */
    tm->tm_hour -= adjust / 3600;
    tm->tm_min -= ( adjust / 60 ) % 60;
    tm->tm_sec -= adjust % 60;
}

static char *parse_rule_OS2( char *tz, struct tm *timeptr )
/*********************************************************/
{
    int         days;
    int         month;
    int         week;
    int         seconds;
    char        *tzptr;
    int         neg;

    /*
     * month 1 - 12
     */
    tzptr = tz;
    tz = parse_number( tz, &month );
    if( (tzptr == tz) || (*tz != ',') ) { /* parsing error */
        return( tzptr );
    }
    tz++;   /* skip ',' character */
    /*
     * week -1, 0, 1, 2, 3, 4
     */
    tzptr = tz;
    tz = parse_sign( tz, &neg );
    tz = parse_number( tz, &week );
    if( (tzptr == tz) || (*tz != ',') || (neg && week != 1) ) {
        return( tzptr ); /* parsing error */
                         /* or unsupported: week -2, -3, -4 */
    }
    tz++;   /* skip ',' character */
    /*
     * convert to POSIX week 1-5
     */
    if( neg && (week == 1) ) {
        /*
         * week -1 in OS/2 is week 5 in POSIX format
         */
        week = 5;
    }

    /*
     * days 0-6 (week) or 1-31 (month)
     */
    tzptr = tz;
    tz = parse_number( tz, &days );
    if( (tzptr == tz) || (*tz != ',') ) { /* parsing error */
        return( tzptr );
    }
    tz++;   /* skip ',' character */

    if( week == 0 ) {   /* days is day of month */
        timeptr->tm_yday = days + __diyr[month - 1]; /*Julian day */
        timeptr->tm_isdst = 1;                  /* simulated Jn form */
    } else {            /* days is day of week */
        timeptr->tm_isdst = 0;                  /* simulated M form */
        timeptr->tm_mon = month - 1;            /* 1-12 for M form */
        timeptr->tm_mday = week;                /* 1-5  for M form */
        timeptr->tm_wday = days;
    }

    /*
     * parse seconds after midnight
     */
    tzptr = tz;
    tz = parse_number( tz, &seconds );
    if( (tzptr == tz) || (*tz != ',') ) { /* parsing error */
        return( tzptr );
    }

    fix_rule( timeptr, -seconds );

    return( tz );
}

static char *parse_OS2( char *tz, struct tm *time1, struct tm *time2, long *day_zone )
/*************************************************************************************
 * OS/2 format is checked if the format is not yet determined and the next
 * character is a ','
 * To be valid all fields from sm to shift must be specified
 *
 * <std><offset><dst>,<sm>,<sw>,<sd>,<st>,<em>,<ew>,<ed>,<et>,<shift>
 *                      |    |    |   |     |    |    |   |
 *                    month  |    | time  month  |    | time
 *                         week   | seconds    week   | seconds
 *                              day                 day
 *                        starting            ending
 */
{
    struct tm       start_dst = {   /* start of daylight savings */
        0, 0, 0,                    /* no default as all fields  */
        0, 0, 0,                    /* have to be specified for  */
        0, 0, 0                     /* a valid OS/2 format       */
    };

    struct tm       end_dst = {     /* end of daylight savings */
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };
    char        *tzptr;
    int         shift;

    /*
     * parse daylight starting time
     */
    tzptr = tz;
    tz = parse_rule_OS2( tz, &start_dst );
    if( (tzptr == tz) || (*tz != ',') ) {   /* parsing error */
        return( tzptr );
    }
    tz++;   /* skip ',' character */

    /*
     * parse daylight ending time
     */
    tzptr = tz;
    tz = parse_rule_OS2( tz, &end_dst );
    if( (tzptr == tz) || (*tz != ',') || (*(tz + 1) == '\0') ) { /* error */
        return( tzptr );
    }
    tz++;   /* skip ',' character */

    /*
     * parse daylight shift value
     */
    tz = parse_number( tz, &shift );
    if( *tz == '\0' ) {                     /* format is ok */
        memcpy( time1, &start_dst, sizeof( *time1 ) );
        memcpy( time2, &end_dst, sizeof( *time2 ) );
        *day_zone = _RWD_timezone - shift;
    }
    return( tz );
}

void __parse_tz( char *tz )
/*************************/
{
    long        dayzone;

    _RWD_daylight = 0;
    if( *tz == ':' ) {
        tz++;
    }
    tz = parse_name( tz, stzone );
    tz = parse_offset( tz, &_RWD_timezone );
    if( *tz == '\0' ) {
        dtzone[0] = '\0';
        return;
    }
    _RWD_daylight = 1;
    _RWD_dst_adjust = 60 * 60;
    dayzone = _RWD_timezone - _RWD_dst_adjust;

    tz = parse_name( tz, dtzone );
    if( *tz == ',' ) {
        /*
         * parse OS/2 format
         *
         * parse daylight info
         */
        tz = parse_OS2( tz + 1, &_RWD_start_dst, &_RWD_end_dst, &dayzone );
        if( *tz == '\0' ) {
            _RWD_dst_adjust = _RWD_timezone - dayzone;
            fix_rule( &_RWD_end_dst, _RWD_dst_adjust );
        }
    } else {
        /*
         * parse POSIX format
         *
         * parse daylight ending offset
         */
        tz = parse_offset( tz, &dayzone );
        _RWD_dst_adjust = _RWD_timezone - dayzone;
        if( *tz == ',' ) {
            /*
             * parse daylight starting time
             */
            tz = parse_rule( tz + 1, &_RWD_start_dst );
            if( *tz == ',' ) {
                /*
                 * parse daylight ending time
                 */
                tz = parse_rule( tz + 1, &_RWD_end_dst );
                fix_rule( &_RWD_end_dst, _RWD_dst_adjust );
            }
        }
    }
}

static int tryOSTimeZone( const char *tz )
/****************************************/
{
    if( tz == NULL ) {
        /*
         * calling OS can be expensive; many programs don't care
         */
        if( tzFlag.cache_OS_TZ && tzFlag.have_OS_TZ )
            return( 1 );
        /*
         * Assume that even if we end up not getting the TZ from OS,
         * we won't have any better luck if we try later.
         */
        tzFlag.have_OS_TZ = 1;
    } else {
        tzFlag.have_OS_TZ = 0;
#ifndef __LINUX__
        return( 0 );
#endif
    }
#if defined( __LINUX__ )
    return( __read_tzfile( tz ) );
#elif defined( __NT__ )
    {
        TIME_ZONE_INFORMATION   tz_info;
        LPSYSTEMTIME            st;
        size_t                  rc;

        _RWD_daylight = 1;              // assume daylight savings supported
        switch( GetTimeZoneInformation( &tz_info ) ) {
        case TIME_ZONE_ID_UNKNOWN:      // returned by Windows NT/2000
        case TIME_ZONE_ID_STANDARD:     // returned by Windows 95
        case TIME_ZONE_ID_DAYLIGHT:
            _RWD_timezone = ( tz_info.Bias + tz_info.StandardBias ) * 60L;
            _RWD_dst_adjust = ( tz_info.DaylightBias - tz_info.StandardBias ) * -60L;
            if( tz_info.DaylightBias == 0 )
                _RWD_daylight = 0;      // daylight savings not supported

            rc = wcstombs( stzone, tz_info.StandardName, sizeof( stzone ) - 1 );
            if( rc == (size_t)-1 ) {    // cannot convert string
                stzone[0] = '\0';
            } else {                    // ensure null-terminated
                stzone[rc] = '\0';
            }
            rc = wcstombs( dtzone, tz_info.DaylightName, sizeof( dtzone ) - 1 );
            if( rc == (size_t)-1 ) {    // cannot convert string
                dtzone[0] = '\0';
            } else {                    // ensure null-terminated
                dtzone[rc] = '\0';
            }
            /*
            StandardDate for Eastern
                wYear = 0
                wMonth = 11
                wDayOfWeek = 0
                wDay = 1
                wHour = 2
                wMinute = 0
                wSecond = 0
                wMilliseconds = 0
            DaylightDate for Eastern
                wYear = 0
                wMonth = 3
                wDayOfWeek = 0
                wDay = 2
                wHour = 2
                wMinute = 0
                wSecond = 0
                wMilliseconds = 0
            */
            st = &tz_info.DaylightDate;
            _RWD_start_dst.tm_sec  = st->wSecond;
            _RWD_start_dst.tm_min  = st->wMinute;
            _RWD_start_dst.tm_hour = st->wHour;
            _RWD_start_dst.tm_mday = st->wDay;         // 1st, 2nd, 3rd, 4th week
            _RWD_start_dst.tm_mon  = st->wMonth - 1;   // 0-11
            _RWD_start_dst.tm_year = st->wYear;
            _RWD_start_dst.tm_wday = st->wDayOfWeek;   // 0-6
            _RWD_start_dst.tm_yday = 0;                // 0-365 -> 0-365
            _RWD_start_dst.tm_isdst= 0;

            st = &tz_info.StandardDate;
            _RWD_end_dst.tm_sec  = st->wSecond;
            _RWD_end_dst.tm_min  = st->wMinute;
            _RWD_end_dst.tm_hour = st->wHour;
            _RWD_end_dst.tm_mday = st->wDay;           // 1st, 2nd, 3rd, 4th week
            _RWD_end_dst.tm_mon  = st->wMonth - 1;     // 0-11
            _RWD_end_dst.tm_year = st->wYear;
            _RWD_end_dst.tm_wday = st->wDayOfWeek;     // 0-6
            _RWD_end_dst.tm_yday = 0;                  // 0-365 -> 0-365
            _RWD_end_dst.tm_isdst= 0;

            fix_rule( &_RWD_end_dst, _RWD_dst_adjust );
            break;

        default:
            /*
             * assume Eastern (North America) time zone
             */
            _RWD_timezone = 5L * 60L * 60L;
            _RWD_dst_adjust = 60L * 60L;
        }
        return( 1 );
    }
#else
    return( 1 );
#endif
}

_WCRTLINK void tzset( void )
/**************************/
{
#ifndef __NETWARE__
    char        *tz;

    tz = getenv( "TZ" );
    if( !tryOSTimeZone( tz ) && tz != NULL ) {
        __parse_tz( tz );
    }
#endif
}
