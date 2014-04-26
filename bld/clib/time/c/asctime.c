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

#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <time.h>
#include "rtdata.h"
#include "asctime.h"

static void convDec( int num, int off, CHAR_TYPE *buf )
{
    div_t        tens_units;

    tens_units   = div( num, 10 );
    buf[off]     = ( CHAR_TYPE ) ( tens_units.quot + '0' );
    buf[off + 1] = ( CHAR_TYPE ) ( tens_units.rem + '0' );
}


_WCRTLINK CHAR_TYPE *__F_NAME( _asctime, __wasctime ) ( const struct tm *tm, CHAR_TYPE *buf )
{
    static const char   months[] = {
        'J', 'F', 'M', 'A', 'M', 'J', 'J', 'A', 'S', 'O', 'N', 'D',
        'a', 'e', 'a', 'p', 'a', 'u', 'u', 'u', 'e', 'c', 'o', 'e',
        'n', 'b', 'r', 'r', 'y', 'n', 'l', 'g', 'p', 't', 'v', 'c'
    };
    static const char   weekdays[] = {
        'S', 'M', 'T', 'W', 'T', 'F', 'S',
        'u', 'o', 'u', 'e', 'h', 'r', 'a',
        'n', 'n', 'e', 'd', 'u', 'i', 't'
    };
    div_t               hundreds_units;
    int                 i;

    i = tm->tm_wday;
    buf[0] = weekdays[i];
    buf[1] = weekdays[i + 7];
    buf[2] = weekdays[i + 14];
    buf[3] = ' ';
    i = tm->tm_mon;
    buf[4] = months[i];
    buf[5] = months[i + 12];
    buf[6] = months[i + 24];
    buf[7] = ' ';
    convDec( tm->tm_mday, 8, buf );                     /* 8-9 */
    if( buf[8] == '0' )
        buf[8]= ' ';                                    /* day of month padding */
    buf[10] = ' ';
    convDec( tm->tm_hour, 11, buf );                    /* 11-12 */
    buf[13] = ':';
    convDec( tm->tm_min, 14, buf );                     /* 14-15 */
    buf[16] = ':';
    convDec( tm->tm_sec, 17, buf );                     /* 17-18 */
    buf[19] = ' ';                                      /* 02-nov-90 */
    hundreds_units = div( tm->tm_year, 100 );
    convDec( hundreds_units.quot + 19, 20, buf );       /* 20-21 */
    convDec( hundreds_units.rem, 22, buf );             /* 22-23 */
    buf[24] = '\n';
    buf[25] = NULLCHAR;

    return( buf );
}

#ifndef __NETWARE__
_WCRTLINK CHAR_TYPE *__F_NAME( asctime, _wasctime ) ( const struct tm *tm )
{
    _INITRESULT;
    return( __F_NAME( _asctime, __wasctime ) ( tm, ( CHAR_TYPE* ) _RWD_asctime ) );
}


_WCRTLINK CHAR_TYPE *__F_NAME( _ctime, __wctime ) ( const time_t *timer, CHAR_TYPE *buf )
{
    struct tm   tm;

    return( __F_NAME( _asctime, __wasctime ) ( _localtime( timer, &tm ), buf ) );
}


_WCRTLINK CHAR_TYPE *__F_NAME( ctime, _wctime ) ( const time_t *timer )
{
    return( __F_NAME( asctime, _wasctime ) ( localtime( timer ) ) );
}
#endif
