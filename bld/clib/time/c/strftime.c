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
* Description:  Implements the ANSI/ISO strftime() function.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include <malloc.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "rtdata.h"
#include "timedata.h"


static const char awday_name[] = { "Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat" };

static const char * const wday_name[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

static const char amon_name[] = {
    "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec"
};

static const char * const mon_name[12] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};


static void TwoDigits( CHAR_TYPE *buffer, int value )
{
    buffer[0] = ( CHAR_TYPE ) ( value / 10 + '0' );
    buffer[1] = ( CHAR_TYPE ) ( value % 10 + '0' );
    buffer[2] = NULLCHAR;
}

_WCRTLINK size_t __F_NAME(strftime,wcsftime)( CHAR_TYPE *s, size_t maxsize,
             const CHAR_TYPE *format, const struct tm *timeptr )
{
    CHAR_TYPE           buffer[TZNAME_MAX + 1];
    const char          *p;
    const CHAR_TYPE     *save_format;
    size_t              len;
    int                 hour;
    size_t              amt_left;
    size_t              piece;
#if defined( __WIDECHAR__ )
    CHAR_TYPE           tmp_fmt[30];
#endif
    int                 isoweek;
    int                 isoyear;
    int                 iso_wday_jan01;
    int                 iso_timezone;

    len = 0;
    amt_left = maxsize;
    save_format = NULL;
    for( ;; ) {
        p = ( char * ) buffer;
        if( *format == '\0' ) {
            if( save_format == NULL )
                break;
            format = save_format + 1;
            save_format = NULL;
            continue;
        }
        if( *format != '%' ) {
            buffer[0] = *format;
            buffer[1] = '\0';
        } else {
            ++format;
            if( (*format == 'E') || (*format == 'O') ) {
                ++format;                      /* ignore E and O modifier */
                if( *format == '\0' ) {        /* end of formatstring? */
                    break;
                }
            }
            switch( *format ) {
            case 'a' :                         /* locale's abbreviated weekday name */
                p = &awday_name[timeptr->tm_wday * 4];
                break;
            case 'A' :                         /* locale's full weekday name */
                p = wday_name[timeptr->tm_wday];
                break;
            case 'b' :                         /* locale's abbreviated month name */
            case 'h' :
                p = &amon_name[timeptr->tm_mon * 4];
                break;
            case 'B' :                         /* locale's full month name */
                p = mon_name[timeptr->tm_mon];
                break;
            case 'c' :                         /* locale's appropriate date & time representation*/
#if 0
                sprintf( buffer, "%.3s %.3s %.2d %.2d:%.2d:%.2d %d",
                          wday_name[timeptr->tm_wday],
                          mon_name[timeptr->tm_mon],
                          timeptr->tm_mday,
                          timeptr->tm_hour,
                          timeptr->tm_min,
                          timeptr->tm_sec,
                          1900 + timeptr->tm_year );
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%a %b %d %H:%M:%S %Y" );
                continue;
#endif
            case 'C' :                         /* century  (00-99) */
                TwoDigits( buffer, (timeptr->tm_year + 1900) / 100 );
                break;
            case 'd' :                         /* day of the month (01-31) */
                TwoDigits( buffer, timeptr->tm_mday );
                break;
            case 'D' :
#if 0
                sprintf( buffer, "%.2d/%.2d/%.2d",
                          timeptr->tm_mon + 1, /* added "+ 1" 23-sep-90 */
                          timeptr->tm_mday,
                          timeptr->tm_year % 100 );
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%m/%d/%y" );
                continue;
#endif
            case 'e' :                         /* day of the month ( 1-31) */
                TwoDigits( buffer, timeptr->tm_mday );
                if( *buffer == '0' ) {
                    *buffer = ' ';
                }
                break;
            case 'F' :                         /* ISO 8601 date format  */
                save_format = format;
                format = _AToUni( tmp_fmt, "%Y-%m-%d" );
                continue;
            case 'g' :                         /* ISO Week-based year   yy */
            case 'G' :                         /* ISO Week-based year yyyy */
                iso_wday_jan01 = ( 8 + ( 6 + timeptr->tm_wday ) % 7
                                   - timeptr->tm_yday % 7
                                 ) % 7;
                if( iso_wday_jan01 == 0 ) {
                    iso_wday_jan01 = 7; /* 1=mon, ... 7=sun */
                }
                isoweek = ( 6 + timeptr->tm_yday - ( 6 + timeptr->tm_wday ) % 7
                          ) / 7 + ( 8 - iso_wday_jan01 ) / 4;
                if( isoweek == 0 ) {    /* belongs to previous year */
                    isoyear = timeptr->tm_year + 1899;
                } else {
                    if( (timeptr->tm_yday > 360) && (timeptr->tm_mday > 28) &&
                        (timeptr->tm_wday < 4) && timeptr->tm_wday &&
                        (31 - timeptr->tm_mday + timeptr->tm_wday < 4) ) {
                        isoyear = timeptr->tm_year + 1901;
                    } else {
                        isoyear = timeptr->tm_year + 1900;
                    }
                }
                if( *format == 'g' ) {
                    TwoDigits( buffer, isoyear % 100 );
                } else {
                    __F_NAME(itoa,_itow)( isoyear, buffer, 10 );
                }
                break;
            case 'H' :                         /* hour (24-hour clock) (00-23) */
                TwoDigits( buffer, timeptr->tm_hour );
                break;
            case 'I' :                         /* hour (12-hour clock) (00-12) */
                hour = timeptr->tm_hour;
                if( hour > 12 )
                    hour -= 12;
                if( hour == 0 )
                    hour = 12;                 /* 24-sep-90 */
                TwoDigits( buffer, hour );
                break;
            case 'j' :                         /* day of the year (001-366) */
                // sprintf( buffer, "%.3d", timeptr->tm_yday + 1 );
                __F_NAME(itoa,_itow)( timeptr->tm_yday + 101, buffer, 10 );
                buffer[0]--;
                // itoa( timeptr->tm_yday + 1001, buffer, 10 );
                // p = &buffer[1];         /* only want last 3 digits */
                break;
            case 'm' :                         /* month (01-12) */
                TwoDigits( buffer, timeptr->tm_mon + 1 );
                break;
            case 'M' :                         /* minute (00-59) */
                TwoDigits( buffer, timeptr->tm_min );
                break;
            case 'n' :
                p = "\n";
                break;
            case 'p' :                         /* locale's equivalent of either AM or PM */
                p = ( timeptr->tm_hour < 12 ) ? "AM" : "PM";
                break;
            case 'r' :
#if 0
                hour = timeptr->tm_hour;
                if( hour > 12 )
                    hour -= 12;
                if( hour == 0 )
                    hour = 12;                 /* 24-sep-90 */
                sprintf( buffer, "%.2d:%.2d:%.2d PM", hour, timeptr->tm_min,
                          timeptr->tm_sec );
                if( timeptr->tm_hour < 12 )
                    buffer[9] = 'A';
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%I:%M:%S %p" );
                continue;
#endif
            case 'R' :   /* hour (24-hour clock) (00-23) : minute (00-59) */
                save_format = format;
                format = _AToUni( tmp_fmt, "%H:%M" );
                continue;
            case 'S' :   /* second (00-60) */
                TwoDigits( buffer, timeptr->tm_sec );
                break;
            case 't' :
                p = "\t";
                break;
            case 'u' :                         /* ISO weekday (1-7) Monday = 1,  Sunday = 7 */
                buffer[0] = timeptr->tm_wday ?
                                (CHAR_TYPE)( timeptr->tm_wday + '0' ) :
                                (CHAR_TYPE)( '7' );
                buffer[1] = '\0';
                break;
            case 'U' :   /* week number of the year (00-53) Sun first day */
                TwoDigits( buffer, ( timeptr->tm_yday
                                + 7 - timeptr->tm_wday ) / 7 );
                break;
            case 'V' :                         /* ISO week number Week 1 includes Jan 4th */
                iso_wday_jan01 = ( 8 + ( 6 + timeptr->tm_wday ) % 7
                                   - timeptr->tm_yday % 7
                                 ) % 7;
                if( iso_wday_jan01 == 0 ) {
                    iso_wday_jan01 = 7; /* 1=mon, ... 7=sun */
                  }
                isoweek = ( 6 + timeptr->tm_yday  - ( 6 + timeptr->tm_wday ) % 7
                          ) / 7 + ( 8 - iso_wday_jan01 ) / 4;
                if( isoweek == 0 ) {    /* belongs to last week of previous year */

                    /* if isoweek is zero, date is Jan 1 to 3, and weekday is Fri to Sun */
                    /* calculate last week number of previous year ( 52 or 53 ) */

                    if( (iso_wday_jan01 == 7) ||
                        ( (iso_wday_jan01 == 6 ) && !__leapyear( (unsigned) timeptr->tm_year + 1899 ) ) ) {
                        isoweek= 52; /* if dec 31 is sat, week 52 */
                                     /* if dec 31 is fri, week 52 if no leapyear */
                    } else {
                        isoweek = 53;
                    }
                } else {
                    if( (timeptr->tm_yday > 360) && (timeptr->tm_mday > 28) &&
                        (timeptr->tm_wday < 4) && timeptr->tm_wday &&
                        (31 - timeptr->tm_mday + timeptr->tm_wday < 4) ) {
                        isoweek = 1;                     /* belongs to next year */
                    }
                }
                TwoDigits( buffer, isoweek );
                break;
            case 'w' :   /* weekday (0-6) Sunday=0 */
                buffer[0] = (CHAR_TYPE)( timeptr->tm_wday + '0' );
                buffer[1] = '\0';
                break;
            case 'W' :  /* week number of the year (00-53) Mon first day */
                TwoDigits( buffer, ( timeptr->tm_yday
                                + 7 - (timeptr->tm_wday + 6) % 7 ) / 7 );
                break;
            case 'x' :   /* locale's appropriate date representation */
#if 0
                sprintf( buffer, "%.3s %.3s %.2d, %d",
                          &awday_name[timeptr->tm_wday * 4],
                          &amon_name[timeptr->tm_mon * 4],
                          timeptr->tm_mday,
                          1900 + timeptr->tm_year );
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%a %b %d, %Y" );
                continue;
#endif
            case 'X' :   /* locale's appropriate time representation */
            case 'T' :
#if 0
                sprintf( buffer, "%.2d:%.2d:%.2d",
                          timeptr->tm_hour,
                          timeptr->tm_min,
                          timeptr->tm_sec );
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%H:%M:%S" );
                continue;
#endif
            case 'y' :   /* year without the century (00-99) */
                TwoDigits( buffer, timeptr->tm_year % 100 );
                break;
            case 'Y' :   /* year with century */
                __F_NAME(itoa,_itow)( timeptr->tm_year + 1900, buffer, 10 );
                break;
            case 'z' :                         /* timezone offset from UTC */
            /* OW  has TZ positive offsets for zones WEST of Greenwich,
                  ISO has positive offsets for zones EAST of Grenwich */
                tzset(); /* get time zone settings */
                if( timeptr->tm_isdst == -1 ) {
                    buffer[ 0 ] = '\0';        /* timezone unknown */
                } else {
                    iso_timezone =  _RWD_timezone;
                    if( iso_timezone > 0 ) {
                        buffer[ 0 ] = ( CHAR_TYPE ) ( '-' );
                    } else {
                        iso_timezone = - _RWD_timezone;
                        buffer[ 0 ] = ( CHAR_TYPE ) ( '+' );
                    }
#if 0
                    /* what about DST ????? C99 standard does not mention it */
                    if( timeptr->tm_isdst > 0 ) {
                        iso_timezone += _RWD_dst_adjust;
                    }
#endif
                    TwoDigits( &buffer[1], (iso_timezone / 3600) );    /* hours */
                    TwoDigits( &buffer[3], (iso_timezone / 60) % 60 ); /* minutes */
                }
                break;
            case 'Z' :   /* time zone name */
                tzset(); /* get time zone settings */
                p = _RWD_tzname[timeptr->tm_isdst];
                break;   /* 31-oct-90 */
            case '%' :
            default  :
                buffer[0] = *format;
                buffer[1] = NULLCHAR;
                break;
            }
        }
#if defined( __WIDECHAR__ )
        if( p != (char *)buffer ) {
            /*** Convert the MBCS string to wide chars in buffer ***/
            if( mbstowcs( buffer, p, sizeof( buffer ) / sizeof( wchar_t ) ) == (size_t)-1 )
                buffer[0] = L'\0';
            p = (const char *)buffer;
        }
#endif
        ++format;
        piece = __F_NAME(strlen,wcslen)( (const CHAR_TYPE *)p );
        if( piece > amt_left )
            piece = amt_left;
        memcpy( &s[len], (const CHAR_TYPE *)p, piece * CHARSIZE );
        amt_left -= piece;
        len += piece;
    }
    if( len < maxsize ) {
        s[len] = NULLCHAR;
        return( len );
    }
    return( 0 );
}

#ifdef __WIDECHAR__

#undef strlen

_WCRTLINK size_t _wstrftime_ms( CHAR_TYPE *s, size_t maxsize, const char *format,
                  const struct tm *timeptr )
{
    wchar_t     *auto_buf;
    int         length;

    length = _mbslen( format ) + 1;
    auto_buf = (wchar_t *)alloca( length * CHARSIZE );
    mbstowcs( auto_buf, format, length );
    return( wcsftime( s, maxsize, auto_buf, timeptr ) );
}
#endif
