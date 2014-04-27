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
#include <time.h>
#ifndef __NETWARE__
 #include <dos.h>
 #include <dosfunc.h>
 #include "tinyio.h"
#endif
#include "timedata.h"

#ifdef __NETWARE__

#define BUFF_SIZE       8

extern void             GetFileServerDateAndTime( void * );

int __getctime( struct tm *ti )
{
    char        buff[BUFF_SIZE];

    GetFileServerDateAndTime( buff );
    ti->tm_year  = buff[0];
    ti->tm_mon   = buff[1];
    ti->tm_mday  = buff[2];
    ti->tm_hour  = buff[3];
    ti->tm_min   = buff[4];
    ti->tm_sec   = buff[5];
    ti->tm_wday  = buff[6];
    ti->tm_isdst = -1;
    return( 0 );
}

#else

int __getctime( struct tm *ti )
{
    int         milliseconds;
    tiny_date_t d, tmp;
    tiny_time_t t;

    d = TinyGetDate();
    ti->tm_year  = d.year;
    ti->tm_mon   = d.month - 1;
    ti->tm_mday  = d.day_of_month;
    t = TinyGetTime();
    ti->tm_hour  = t.hour;
    ti->tm_min   = t.minutes;
    ti->tm_sec   = t.seconds;
    milliseconds = t.hundredths * 10;    /* 19-dec-90 */

    tmp = TinyGetDate();
    /* check for midnight rollover */
    if( tmp.day_of_month != d.day_of_month && ti->tm_hour != 23 ) {
        ti->tm_year = tmp.year;
        ti->tm_mon  = tmp.month - 1;
        ti->tm_mday = tmp.day_of_month;
    }
    ti->tm_isdst = -1;
    return( milliseconds );
}

#endif
