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
* Description:  Date parsing routines for wtouch and wpack.
*
****************************************************************************/


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include "wio.h"
#include "watcom.h"
#include "banner.h"
#if defined( __OS2__ )
#include <direct.h>
#include <sys/utime.h>
#define INCL_DOSNLS
#include "os2.h"
#elif defined( __NT__ )
#include <direct.h>
#include <sys/utime.h>
#include <windows.h>
#elif defined( __UNIX__ )
#include <dirent.h>
#include <utime.h>
#else
#include <direct.h>
#include <sys/utime.h>
#include "tinyio.h"
#endif
#include "bool.h"
#include "touch.h"
#include "wtmsg.h"

static char * (*firstParse)( char * );
static char * (*secondParse)( char * );
static char * (*thirdParse)( char * );

touchflags  TouchFlags;
timestruct  TimeAdjust;
datestruct  DateAdjust;
char        timeSeparator;
char *      timeFormat;
char        dateSeparator;
char *      dateFormat;

/* external function prototypes */

extern void Error( int, char * );

extern void DoDOption( char *date )
/*********************************/
{
    date = firstParse( date );
    if( date == NULL ) {
        return;
    }
    date = secondParse( date );
    if( date == NULL ) {
        return;
    }
    date = thirdParse( date );
    if( date == NULL ) {
        return;
    }
    TouchFlags.date_specified = 1;
}

static void badCharInTime( char c )
/*********************************/
{
    char    cbuff[2];

    cbuff[0] = c;
    cbuff[1] = '\0';
    Error( MSG_INV_TIME, cbuff );
}

extern void DoTOption( char *time )
/*********************************/
{
    unsigned hours;
    unsigned minutes;
    unsigned seconds;
    unsigned digits;
    char *p;
    char c;

    p = time;
    digits = 2;
    hours = 0;
    for(;;) {
        c = tolower( *p );
        if( digits == 0 || c == timeSeparator || c=='\0' || c=='a' || c=='p' ) {
            if( hours > 23 ) {
                Error( MSG_INV_HOUR, NULL );
                return;
            }
            TimeAdjust.hours = hours;
            if( c == timeSeparator ) {
                ++p;
            }
            break;
        }
        if( isdigit( c ) ) {
            hours *= 10;
            hours += c - '0';
            --digits;
        } else {
            badCharInTime( *p );
            return;
        }
        ++p;
    }
    digits = 2;
    minutes = 0;
    for(;;) {
        c = tolower( *p );
        if( digits == 0 || c == timeSeparator || c=='\0' || c=='a' || c=='p' ) {
            if( minutes > 60 ) {
                Error( MSG_INV_MINUTE, NULL );
                return;
            }
            TimeAdjust.minutes = minutes;
            if( c == timeSeparator ) {
                ++p;
            }
            break;
        }
        if( isdigit( c ) ) {
            minutes *= 10;
            minutes += c - '0';
            --digits;
        } else {
            badCharInTime( *p );
            return;
        }
        ++p;
    }
    digits = 2;
    seconds = 0;
    for(;;) {
        c = tolower( *p );
        if( digits == 0 || c == timeSeparator || c=='\0' || c=='a' || c=='p' ) {
            if( seconds > 60 ) {
                Error( MSG_INV_SECOND, NULL );
                return;
            }
            TimeAdjust.seconds = seconds;
            if( c == timeSeparator ) {
                ++p;
            }
            break;
        }
        if( isdigit( c ) ) {
            seconds *= 10;
            seconds += c - '0';
            --digits;
        } else {
            badCharInTime( *p );
            return;
        }
        ++p;
    }
    if( TouchFlags.time_24hr == 0 && TimeAdjust.hours < 13 ) {
        c = tolower( *p );
        if( c == 'a' ) {
            /* 12:xx:xxAM is just after midnight */
            if( TimeAdjust.hours == 12 ) {
                TimeAdjust.hours = 0;
            }
        } else if( c == 'p' ) {
            /* 12:xx:xxPM is just after noon */
            if( TimeAdjust.hours != 12 ) {
                TimeAdjust.hours += 12;
            }
        }
    }
    TouchFlags.time_specified = 1;
}

static void badCharInDate( char c )
/*********************************/
{
    char    cbuff[2];

    cbuff[0] = c;
    cbuff[1] = '\0';
    Error( MSG_INV_DATE, cbuff );
}

static char *parseYear( char *p )
/*******************************/
{
    unsigned year;
    unsigned digits;

    digits = 4;
    year = 0;
    for(;;) {
        if( digits == 0 || *p == dateSeparator || *p == '\0' ) {
            if( year < 1900 ) {
                year %= 100;
                year += 1900;
            }
            if( year < 1980 ) {
                Error( MSG_INV_YEAR, NULL );
                return( NULL );
            }
            DateAdjust.year = year;
            if( *p == dateSeparator ) {
                ++p;
            }
            return( p );
        }
        if( isdigit( *p ) ) {
            year *= 10;
            year += *p - '0';
            --digits;
        } else {
            badCharInDate( *p );
            return( NULL );
        }
        ++p;
    }
}

static char *parseMonth( char *p )
/********************************/
{
    unsigned month;
    unsigned digits;

    digits = 2;
    month = 0;
    for(;;) {
        if( digits == 0 || *p == dateSeparator || *p == '\0' ) {
            if( month == 0 ) {
                month = 1;
            } else if( month > 12 ) {
                Error( MSG_INV_MONTH, NULL );
                return( NULL );
            }
            DateAdjust.month = month;
            if( *p == dateSeparator ) {
                ++p;
            }
            return( p );
        }
        if( isdigit( *p ) ) {
            month *= 10;
            month += *p - '0';
            --digits;
        } else {
            badCharInDate( *p );
            return( NULL );
        }
        ++p;
    }
}

static char *parseDay( char *p )
/******************************/
{
    unsigned day;
    unsigned digits;

    digits = 2;
    day = 0;
    for(;;) {
        if( digits == 0 || *p == dateSeparator || *p == '\0' ) {
            if( day == 0 ) {
                day = 1;
            } else if( day > 31 ) {
                Error( MSG_INV_DAY, NULL );
                return( NULL );
            }
            DateAdjust.day = day;
            if( *p == dateSeparator ) {
                ++p;
            }
            return( p );
        }
        if( isdigit( *p ) ) {
            day *= 10;
            day += *p - '0';
            --digits;
        } else {
            badCharInDate( *p );
            return( NULL );
        }
        ++p;
    }
}

extern void WhereAmI( void )
/**************************/
{
    TouchFlags.time_24hr = 0;
    dateSeparator = '-';
    dateFormat = "mm-dd-yy";
    timeSeparator = ':';
    timeFormat = "hh:mm:ss";
    firstParse = parseMonth;
    secondParse = parseDay;
    thirdParse = parseYear;
#if defined( __OS2__ )
    if( !TouchFlags.usa_date_time ) {
        COUNTRYINFO country;
        COUNTRYCODE code;
#if defined(__386__)
        ULONG  amount;

        if( DosQueryCtryInfo( sizeof(country), &code, &country, &amount ) ) {
            return;
        }
#else
        USHORT amount;

        if( DosGetCtryInfo( sizeof(country), &code, &country, &amount ) ) {
            return;
        }
#endif
        switch( country.fsDateFmt ) {
        case 0x0000:
            firstParse = parseMonth;
            secondParse = parseDay;
            thirdParse = parseYear;
            dateFormat = "mm-dd-yy";
            break;
        case 0x0001:
            firstParse = parseDay;
            secondParse = parseMonth;
            thirdParse = parseYear;
            dateFormat = "dd-mm-yy";
            break;
        case 0x0002:
            firstParse = parseYear;
            secondParse = parseMonth;
            thirdParse = parseDay;
            dateFormat = "yy-mm-dd";
            break;
        }
        dateSeparator = country.szDateSeparator[0];
        timeSeparator = country.szTimeSeparator[0];
        if( country.fsTimeFmt == 0x0001 ) {
            TouchFlags.time_24hr = 1;
        }
    }
#elif defined( __NT__ )
    if( !TouchFlags.usa_date_time ) {
        char wbuff[16];

        GetLocaleInfoA( CTRY_DEFAULT, LOCALE_SDATE, wbuff, sizeof(wbuff)/sizeof(char) );
        dateSeparator = (char) wbuff[0];
        GetLocaleInfoA( CTRY_DEFAULT, LOCALE_STIME, wbuff, sizeof(wbuff)/sizeof(char) );
        timeSeparator = (char) wbuff[0];
        GetLocaleInfoA( CTRY_DEFAULT, LOCALE_ITIME, wbuff, sizeof(wbuff)/sizeof(char) );
        if( wbuff[0] == '1' ) {
            TouchFlags.time_24hr = 1;
        }
        GetLocaleInfoA( CTRY_DEFAULT, LOCALE_IDATE, wbuff, sizeof(wbuff)/sizeof(char) );
        switch( wbuff[0] ) {
        case '0':
            firstParse = parseMonth;
            secondParse = parseDay;
            thirdParse = parseYear;
            dateFormat = "mm-dd-yy";
            break;
        case '1':
            firstParse = parseDay;
            secondParse = parseMonth;
            thirdParse = parseYear;
            dateFormat = "dd-mm-yy";
            break;
        case '2':
            firstParse = parseYear;
            secondParse = parseMonth;
            thirdParse = parseDay;
            dateFormat = "yy-mm-dd";
            break;
        }
    }
#elif defined( __DOS__ )
    if( !TouchFlags.usa_date_time ) {
        tiny_country_info country;
        tiny_ret_t rc;

        rc = TinyGetCountry( &country );
        if( TINY_ERROR( rc ) ) {
            return;
        }
        if( _osmajor >= 2 ) {
            switch( country.ms2.date_format ) {
            case TDATE_M_D_Y:
                firstParse = parseMonth;
                secondParse = parseDay;
                thirdParse = parseYear;
                dateFormat = "mm-dd-yy";
                break;
            case TDATE_D_M_Y:
                firstParse = parseDay;
                secondParse = parseMonth;
                thirdParse = parseYear;
                dateFormat = "dd-mm-yy";
                break;
            case TDATE_Y_M_D:
                firstParse = parseYear;
                secondParse = parseMonth;
                thirdParse = parseDay;
                dateFormat = "yy-mm-dd";
                break;
            }
        }
        if( _osmajor >= 3 ) {
            dateSeparator = country.ms3.date_separator[0];
            timeSeparator = country.ms3.time_separator[0];
            if( country.ms3.time_format == TTIME_24_HOUR ) {
                TouchFlags.time_24hr = 1;
            }
        }
    }
#endif
}


