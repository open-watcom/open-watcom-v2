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
#include "d2ttime.h"

/* date extraction macros */
#define day( x )        (x & 0x001f)
#define month( x )      ((x >> 5) & 0x000f)
#define year( x )       ((x >> 9) & 0x007f)
/* time extraction macros */
#define sec2( x )       (x & 0x001f)
#define min( x )        ((x >> 5) & 0x003f)
#define hour( x )       ((x >> 11) & 0x001f)

time_t _d2ttime( unsigned short date, unsigned short time )
{
    struct tm t;

    t.tm_year = year( date ) + 80;
    t.tm_mon  = month( date ) - 1;
    t.tm_mday = day( date );
    t.tm_hour = hour( time );
    t.tm_min  = min( time );
    t.tm_sec  = sec2( time ) * 2;
    t.tm_wday = -1;
    t.tm_yday = -1;
    t.tm_isdst = -1;
    return( mktime( &t ) );
}
