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
 *          :characters
 *              or
 *          <std><offset>[<dst>[<offset>]][,<start>[/<time>],<end>[/<time>]]]
 *
 * for OS/2 the alternate format is evaluated, if after scanning the previous
 *          format the next char is a ','
 *
 *          <std><offset><dst>,<sm>,<sw>,<sd>,<st>,<em>,<ew>,<ed>,<et>,<shift>
 *
 *
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

static char     stzone[TZNAME_MAX + 1] = "EST";       /* place to store names */
static char     dtzone[TZNAME_MAX + 1] = "EDT";       /* place to store names */

_WCRTDATA char  * _WCDATA tzname[2] = { stzone, dtzone };

_WCRTDATA long  _WCDATA timezone = 5L * 60L * 60L;      /* seconds from GMT */
_WCRTDATA int   _WCDATA daylight = 1;                   /* d.s.t. indicator */
int             __dst_adjust = 60 * 60;         /* daylight adjustment */

static struct {
    unsigned    cache_OS_TZ : 1;
    unsigned    have_OS_TZ : 1;
    unsigned    format_TZ : 2;  /* format: 0= yet unknown, 1= OS/2, 2= OW  */
}               tzFlag = { 1, 0, 0 };

int __DontCacheOSTZ( void )
/*************************/
{
    int old_flag;

    old_flag           = tzFlag.cache_OS_TZ;
    tzFlag.cache_OS_TZ = 0;
    tzFlag.have_OS_TZ  = 0;
    tzFlag.format_TZ   = 0;
    return( old_flag );
}

int __CacheOSTZ( void )
/*********************/
{
    int old_flag;

    old_flag           = tzFlag.cache_OS_TZ;
    tzFlag.cache_OS_TZ = 1;
    tzFlag.have_OS_TZ  = 0;
    tzFlag.format_TZ   = 0;
    return( old_flag );
}

static char *parse_time( char *tz, int *val )
/*******************************************/
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

static char *parse_offset( char *tz, char *name, long *offset )
/*************************************************************/
{
    int         hours;
    int         minutes;
    int         seconds;
    int         neg;
    int         len;
    char        ch;
    char const  *tzstart;

    if( *tz == ':' ) {
        tz++;
        tzFlag.format_TZ = 2;  /* OW format, can't be OS/2 format */
    }
    /* remember where time zone name string begins */
    tzstart = tz;
    /* parse time zone name (should be 3 or more characters) */
    /* examples:    PST8, EDT+6, Central Standard Time+7:00:00 */
    for( ;; ) {
        ch = *tz;
        if( ch == '\0' )
            break;
        if( ch == ',' )
            break;
        if( ch == '-' )
            break;
        if( ch == '+' )
            break;
        if( ch >= '0' && ch <= '9' )
            break;
        ++tz;
    }
    len = tz - tzstart;
    if( len > TZNAME_MAX )
        len = TZNAME_MAX;
    memcpy( name, tzstart, ( size_t ) len );
    name[len] = '\0';

    neg = 0;
    if( ch == '-' ) {
        neg = 1;
        ++tz;
    } else if( ch == '+' )
        ++tz;
    ch = *tz;
    if( ch >= '0' && ch <= '9' ) {
        hours = minutes = seconds = 0;
        tz = parse_time( tz, &hours );
        if( *tz == ':' ) {
            tz = parse_time( tz + 1, &minutes );
            if( *tz == ':' )
                tz = parse_time( tz + 1, &seconds );
        }
        *offset = seconds + ( ( minutes + ( hours * 60 ) ) * 60L );
        if( neg )
            *offset = -*offset;
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
    if( *tz == 'J' ) { /* Jn 1-365 (no leap days) */
        date_form = 1;
        tzFlag.format_TZ = 2;  /* OW format, can't be OS/2 format */
        tz++;
    }
    if( *tz == 'M' ) { /* Mm.n.d n'th day of month */
        date_form = 0;
        tzFlag.format_TZ = 2;  /* OW format, can't be OS/2 format */
        tz++;
    }
    timeptr->tm_isdst = date_form;
    tz = parse_time( tz, &days );
    if( date_form != 0 )
        timeptr->tm_yday = days;
    else {
        timeptr->tm_mon = days - 1;             /* 1-12 for M form */
        if( *tz == '.' ) {
            tz = parse_time( tz + 1, &days );   /* 1-5 for M form */
            timeptr->tm_mday = days;
            if( *tz == '.' ) {
                tz = parse_time( tz + 1, &days );/* 0-6 for M form */
                timeptr->tm_wday = days;
            }
        }
        timeptr->tm_yday = 0;
    }

    hours = 2;
    minutes = seconds = 0;
    if( *tz == '/' ) {
        tzFlag.format_TZ = 2;  /* OW format, can't be OS/2 format */
        tz = parse_time( tz + 1, &hours );
        if( *tz == ':' ) {
            tz = parse_time( tz + 1, &minutes );
            if( *tz == ':' )
                tz = parse_time( tz + 1, &seconds );
        }
    }
    timeptr->tm_sec = seconds;
    timeptr->tm_min = minutes;
    timeptr->tm_hour = hours;
    return( tz );
}

static char *parse_rule_OS2( char *tz, struct tm *timeptr )
/*********************************************************/
{
    int         days;
    int         month;
    int         week;
    int         seconds;
    char    *   tzptr;
    int         neg;

    tzptr = tz;
    tz = parse_time( tz, &month ); /* month 1 - 12 */
    if( (tzptr == tz) || (*tz != ',') ) { /* parsing error */
        return( tzptr );
    }

    if( *(tz + 1) == '-' ) {
        neg = 1;
        tz++;
    } else {
        neg = 0;
        if( *(tz + 1) == '+' ) {
            tz++;
        }
    }
    tzptr = tz + 1;
    tz = parse_time( tzptr, &week ); /* week -1, 0, 1, 2, 3, 4 */
    if( (tzptr == tz) || (*tz != ',') || (neg && week != 1) ) {
        return( tzptr ); /* parsing error */
                         /* or unsupported: week -2, -3, -4 */
    }
    if ( neg && (week == 1) ) {
        week = 5; /* week -1 in OS/2 is week 5 in OW format */
    }

    tzptr = tz + 1;
    tz = parse_time( tzptr, &days ); /* days 0-6 or 1-31 */
    if( (tzptr == tz) || (*tz != ',') ) { /* parsing error */
        return( tzptr );
    }

    if( week == 0 ) { /* days is day of month */
        timeptr->tm_yday = days + __diyr[month - 1]; /*Julian day */
        timeptr->tm_isdst = 1;                  /* simulated Jn form */
    } else { /* days is day of week */
        timeptr->tm_isdst = 0;                  /* simulated M form */
        timeptr->tm_mon = month - 1;            /* 1-12 for M form */
        timeptr->tm_mday = week;                /* 1-5  for M form */
        timeptr->tm_wday = days;
    }

    tzptr = tz + 1;
    tz = parse_time( tzptr, &seconds ); /* seconds after midnight */

    if( (tzptr == tz) || (*tz != ',') ) { /* parsing error */
        return( tzptr );
    }

    timeptr->tm_hour = seconds / 3600;
    timeptr->tm_min  = ( seconds / 60 ) % 60;
    timeptr->tm_sec  = seconds % 60;

    return( tz );
}

/***************************************************************************/
/*                                                                         */
/* OS/2 format is checked if the format is not yet determined and the next */
/* character is a ,                                                        */
/* To be valid all fields from sm to shift must be specified               */
/*                                                                         */
/*      <std><offset><dst>,<sm>,<sw>,<sd>,<st>,<em>,<ew>,<ed>,<et>,<shift> */
/*                                starting            ending       seconds */
/*                           month                month                    */
/*                                week     time       week     time        */
/*                                     day                 day             */
/*                                                                         */
/***************************************************************************/
static char * parse_OS2( char *tz, struct tm *time1, struct tm *time2,
                         long *day_zone )
/**************************************/
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
    char    *   tzptr;
    int         shift;

    tzptr = tz;
    tz = parse_rule_OS2( tz, &start_dst ); /* daylight starting time */
    if( (tzptr == tz) || (*tz != ',') ) { /* parsing error */
        return( tzptr );
    }

    tzptr = tz + 1;
    tz = parse_rule_OS2( tzptr, &end_dst ); /*daylight ending time */
    if( (tzptr == tz) || (*tz != ',') || (*(tz + 1) == '\0') ) { /* error */
        return( tzptr );
    }

    tz = parse_time( tz + 1, &shift );     /* shift value */
    if( *tz == '\0' ) { /* format is ok */
        memcpy( time1, &start_dst, sizeof( *time1 ) );
        memcpy( time2, &end_dst, sizeof( *time2 ) );
        *day_zone = _RWD_timezone - shift;
    }
    return( tz );
}

void __parse_tz( char * tz )
/**************************/
{
    long        dayzone;
    char    *   dststart;

    _RWD_daylight = 0;
    tzFlag.format_TZ   = 0;
    tz = parse_offset( tz, stzone, &_RWD_timezone );
    if( *tz == '\0' ) {
        dtzone[0] = '\0';
        return;
    }
    _RWD_daylight = 1;
    dayzone = _RWD_timezone - ( 60*60 );              /* 16-aug-91 */

    dststart = tz; /* remember for possible OS/2 format check */

    tz = parse_offset( tz, dtzone, &dayzone );
    _RWD_dst_adjust = _RWD_timezone - dayzone;

    /* parse daylight changing rules */
    if( *tz == ',' )
        tz = parse_rule( tz + 1, &__start_dst );
    if( *tz == ',' ) {
        tz = parse_rule( tz + 1, &__end_dst );
        /* convert rule to be in terms of Standard Time */
        /* rather than Daylight Time */
        __end_dst.tm_hour -= _RWD_dst_adjust / 3600;
        __end_dst.tm_min -= ( _RWD_dst_adjust / 60 ) % 60;
        __end_dst.tm_sec -= _RWD_dst_adjust % 60;
    }

    if( tzFlag.format_TZ < 2 && *tz != '\0' ) { /* not yet sure about tz format*/
                                                /* try OS/2 format */
        while( *dststart != ',' ) { /* over dst zone name */
            if( (*dststart >= '0') && (*dststart <= '9') ) {
                tzFlag.format_TZ = 2;  /* cannot be OS/2 format */
                break;
            }
            dststart++;
        }
        if( tzFlag.format_TZ < 2 ) {
            tz = parse_OS2( dststart + 1, &__start_dst, &__end_dst, &dayzone );
            if( *tz == '\0' ) {
                tzFlag.format_TZ = 1; /* correct OS/2 format */
                _RWD_dst_adjust = _RWD_timezone - dayzone;
                /* convert rule to be in terms of Standard Time */
                /* rather than Daylight Time */
                __end_dst.tm_hour -= _RWD_dst_adjust / 3600;
                __end_dst.tm_min -= ( _RWD_dst_adjust / 60 ) % 60;
                __end_dst.tm_sec -= _RWD_dst_adjust % 60;
            }
        }
    }
}

static int tryOSTimeZone( const char *tz )
/****************************************/
{
    if( tz == NULL ) {
        /* calling OS can be expensive; many programs don't care */
        if( tzFlag.cache_OS_TZ && tzFlag.have_OS_TZ )
            return( 1 );
        /* Assume that even if we end up not getting the TZ from OS,
            we won't have any better luck if we try later. */
        tzFlag.have_OS_TZ = 1;
    } else {
        tzFlag.have_OS_TZ = 0;
#ifndef __LINUX__
        return( 0 );
#endif
    }
#if defined( __NT__ )
    {
        auto TIME_ZONE_INFORMATION  tz_info;
        LPSYSTEMTIME                st;
        size_t                      rc;

        _RWD_daylight = 1;                  // assume daylight savings supported
        switch( GetTimeZoneInformation( &tz_info ) ) {
        case TIME_ZONE_ID_UNKNOWN:          // returned by Windows NT/2000
        case TIME_ZONE_ID_STANDARD:         // returned by Windows 95
        case TIME_ZONE_ID_DAYLIGHT:
            _RWD_timezone = ( tz_info.Bias + tz_info.StandardBias ) * 60L;
            _RWD_dst_adjust = ( tz_info.DaylightBias - tz_info.StandardBias ) * -60L;
            if( tz_info.DaylightBias == 0 )
                _RWD_daylight = 0;  // daylight savings not supported

            rc = wcstombs( stzone, tz_info.StandardName, sizeof( stzone ) - 1 );
            if( rc == (size_t)-1 )  // cannot convert string
                stzone[ 0 ] = '\0';
            else  // ensure null-terminated
                stzone[ rc ] = '\0';

            rc = wcstombs( dtzone, tz_info.DaylightName, sizeof( dtzone ) - 1 );
            if( rc == (size_t)-1 )  // cannot convert string
                dtzone[ 0 ] = '\0';
            else  // ensure null-terminated
                dtzone[ rc ] = '\0';

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
            __start_dst.tm_sec  = st->wSecond;
            __start_dst.tm_min  = st->wMinute;
            __start_dst.tm_hour = st->wHour;
            __start_dst.tm_mday = st->wDay;         // 1st, 2nd, 3rd, 4th week
            __start_dst.tm_mon  = st->wMonth - 1;   // 0-11
            __start_dst.tm_year = st->wYear;
            __start_dst.tm_wday = st->wDayOfWeek;   // 0-6
            __start_dst.tm_yday = 0;                // 0-365 -> 0-365
            __start_dst.tm_isdst= 0;

            st = &tz_info.StandardDate;
            __end_dst.tm_sec  = st->wSecond;
            __end_dst.tm_min  = st->wMinute;
            __end_dst.tm_hour = st->wHour;
            __end_dst.tm_mday = st->wDay;           // 1st, 2nd, 3rd, 4th week
            __end_dst.tm_mon  = st->wMonth - 1;     // 0-11
            __end_dst.tm_year = st->wYear;
            __end_dst.tm_wday = st->wDayOfWeek;     // 0-6
            __end_dst.tm_yday = 0;                  // 0-365 -> 0-365
            __end_dst.tm_isdst= 0;
            break;

        default:
            // assume Eastern (North America) time zone
            _RWD_timezone = 5L * 60L * 60L;
            _RWD_dst_adjust = 60L * 60L;
        }
        return( 1 );
    }
#elif defined( __LINUX__ )
    return( __read_tzfile( tz ) );
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
    if( !tryOSTimeZone( tz ) && tz != NULL )
        __parse_tz( tz );
    #endif
}
