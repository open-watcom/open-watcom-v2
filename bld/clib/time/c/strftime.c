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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <mbstring.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include "rtdata.h"

#ifndef TZNAME_MAX
#define TZNAME_MAX 128  // not defined for netware
#endif

static const char awday_name[] = { "Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat" };

static const char * const wday_name[7] = {  "Sunday",
                                     "Monday",
                                     "Tuesday",
                                     "Wednesday",
                                     "Thursday",
                                     "Friday",
                                     "Saturday" };

static const char amon_name[] = {
        "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec" };

static const char * const mon_name[12] = { "January",
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
                                    "December" };

static void TwoDigits( CHAR_TYPE *buffer, int value )
{
    buffer[0] = value / 10 + '0';
    buffer[1] = value % 10 + '0';
    buffer[2] = NULLCHAR;
}



_WCRTLINK size_t __F_NAME(strftime,wcsftime)( CHAR_TYPE *s, size_t maxsize,
                const CHAR_TYPE *format, const struct tm *timeptr )
{
    auto CHAR_TYPE      buffer[TZNAME_MAX+1];
    const char *        p;
    const CHAR_TYPE *   save_format;
    size_t              len;
    int                 hour;
    size_t              amt_left;
    size_t              piece;
#if defined(__WIDECHAR__)
    CHAR_TYPE           tmp_fmt[30];
#endif

    len = 0;
    amt_left = maxsize;
    save_format = NULL;
    for(;;) {
        p = (char *)&buffer[0];
        if( *format == '\0' ) {
            if( save_format == NULL ) break;
            format = save_format + 1;
            save_format = NULL;
            continue;
        }
        if( *format != '%' ) {
            buffer[0] = *format;
            buffer[1] = '\0';
        } else {
            ++format;
            switch( *format ) {
            case 'a' :      /* locale's abbreviated weekday name */
                p = &awday_name[ timeptr->tm_wday * 4 ];
                break;
            case 'A' :      /* locale's full weekday name */
                p = wday_name[ timeptr->tm_wday ];
                break;
            case 'b' :      /* locale's abbreviated month name */
            case 'h' :
                p = &amon_name[ timeptr->tm_mon * 4 ];
                break;
            case 'B' :      /* locale's full month name */
                p = mon_name[ timeptr->tm_mon ];
                break;
            case 'c' :  /* locale's appropriate date & time representation*/
#if 0
                sprintf( buffer, "%.3s %.3s %.2d %.2d:%.2d:%.2d %d",
                          wday_name[ timeptr->tm_wday ],
                          mon_name[ timeptr->tm_mon ],
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
            case 'd' :      /* day of the month (01-31) */
                TwoDigits( buffer, timeptr->tm_mday );
                break;
            case 'D' :
#if 0
                sprintf( buffer, "%.2d/%.2d/%.2d",
                          timeptr->tm_mon + 1,  /* added "+ 1" 23-sep-90 */
                          timeptr->tm_mday,
                          timeptr->tm_year % 100 );
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%m/%d/%y" );
                continue;
#endif
            case 'H' :      /* hour (24-hour clock) (00-23) */
                TwoDigits( buffer, timeptr->tm_hour );
                break;
            case 'I' :      /* hour (12-hour clock) (00-12) */
                hour = timeptr->tm_hour;
                if( hour > 12 ) hour -= 12;
                if( hour == 0 ) hour = 12;                  /* 24-sep-90 */
                TwoDigits( buffer, hour );
                break;
            case 'j' :      /* day of the year (001-366) */
//              sprintf( buffer, "%.3d", timeptr->tm_yday + 1 );
                __F_NAME(itoa,_itow)( timeptr->tm_yday + 101, buffer, 10 );
                buffer[0] --;
//              itoa( timeptr->tm_yday + 1001, buffer, 10 );
//              p = &buffer[1];         /* only want last 3 digits */
                break;
            case 'm' :      /* month (01-12) */
                TwoDigits( buffer, timeptr->tm_mon + 1 );
                break;
            case 'M' :      /* minute (00-59) */
                TwoDigits( buffer, timeptr->tm_min );
                break;
            case 'n' :
                p = "\n";
                break;
            case 'r' :
#if 0
                hour = timeptr->tm_hour;
                if( hour > 12 ) hour -= 12;
                if( hour == 0 ) hour = 12;                  /* 24-sep-90 */
                sprintf( buffer, "%.2d:%.2d:%.2d PM",
                          hour,
                          timeptr->tm_min,
                          timeptr->tm_sec );
                if( timeptr->tm_hour < 12 )  buffer[9] = 'A';
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%I:%M:%S %p" );
                continue;
#endif
            case 'p' :      /* locale's equivalent of either AM or PM */
                if( timeptr->tm_hour < 12 ) {
                    p = "AM";
                } else {
                    p = "PM";
                }
                break;
            case 'S' :      /* second (00-60) */
                TwoDigits( buffer, timeptr->tm_sec );
                break;
            case 't' :
                p = "\t";
                break;
            case 'U' :  /* week number of the year (00-53) Sun first day */
                TwoDigits( buffer, ( timeptr->tm_yday
                                + 7 - timeptr->tm_wday ) /7 );
                break;
            case 'w' :      /* weekday (0-6) Sunday=0 */
                buffer[0] = timeptr->tm_wday + '0';
                buffer[1] = '\0';
                break;
            case 'W' :  /* week number of the year (00-53) Mon first day */
                TwoDigits( buffer,
                   ( 6 - (timeptr->tm_yday % 7 - timeptr->tm_wday + 7 ) % 7
                                        + timeptr->tm_yday ) / 7 );
                break;
            case 'x' :      /* locale's appropriate date representation */
#if 0
                sprintf( buffer, "%.3s %.3s %.2d, %d",
                          &awday_name[ timeptr->tm_wday * 4 ],
                          &amon_name[ timeptr->tm_mon * 4 ],
                          timeptr->tm_mday,
                          1900 + timeptr->tm_year );
                break;
#else
                save_format = format;
                format = _AToUni( tmp_fmt, "%a %b %d, %Y" );
                continue;
#endif
            case 'X' :      /* locale's appropriate time representation */
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
            case 'y' :      /* year without the century (00-99) */
                TwoDigits( buffer, timeptr->tm_year % 100 );
                break;
            case 'Y' :      /* year with century */
                __F_NAME(itoa,_itow)( timeptr->tm_year + 1900, buffer, 10 );
                break;
            case 'Z' :      /* time zone name */
            case 'z' :
                tzset();    /* get time zone settings */
                p = _RWD_tzname[ timeptr->tm_isdst ];
                break;                                      /* 31-oct-90 */
            case '%' :
            default  :
                buffer[0] = *format;
                buffer[1] = NULLCHAR;
                break;
            }
        }
#if defined(__WIDECHAR__)
        if( p != (char *)buffer ) {
            /*** Convert the MBCS string to wide chars in buffer ***/
            if( mbstowcs( buffer, p, _mbslen( p ) + 1 )  ==  (size_t)-1 ) {
                buffer[0] = L'\0';
            }
            p = (const char*)buffer;
        }
#endif
        ++format;
        piece = __F_NAME(strlen,wcslen)( (const CHAR_TYPE*)p );
        if( piece > amt_left )  piece = amt_left;
        memcpy( &s[len], (const CHAR_TYPE*)p, piece * CHARSIZE );
        amt_left -= piece;
        len += piece;
    }
    if( len < maxsize ) {
        s[len] = NULLCHAR;
        return( len );
    } else {
        return( 0 );
    }
}

#ifdef __WIDECHAR__

#undef strlen

_WCRTLINK size_t _wstrftime_ms( CHAR_TYPE *s, size_t maxsize, const char *format,
                  const struct tm *timeptr )
{
    wchar_t *           auto_buf;
    int                 length;

    length = _mbslen( format ) + 1;
    auto_buf = (wchar_t*) alloca( length * CHARSIZE );
    mbstowcs( auto_buf, format, length );
    return( wcsftime( s, maxsize, auto_buf, timeptr ) );
}
#endif
