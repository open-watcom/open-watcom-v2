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
* Description:  Platform independent mktime() implementation.
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "rtdata.h"
#include "timedata.h"

_WCRTLINK time_t mktime( struct tm *t )
{
    long        days;
    long        seconds;
    long        day_seconds;

    seconds = __local_mktime( t, &days, &day_seconds );
    __brktime( (unsigned long) days, (time_t) day_seconds, 0L, t );
    tzset();
    seconds += _RWD_timezone; /* add in seconds from GMT */
#ifdef __LINUX__
    if( t->tm_isdst < 0 )
        __check_tzfile( seconds, t );
#endif
    /* if we are in d.s.t. then subtract __dst_adjust from seconds */
    if( __isindst( t ) )  /* - determine if we are in d.s.t. */
        seconds -= _RWD_dst_adjust;
#ifdef __UNIX__               /* time_t is signed */
    if( seconds < 0 )
        return( ( time_t ) -1 );
#else /* time_t is unsigned, special day check needed for 31 dec 1969 */
    /* check for overflow; days == 75277 && seconds == 23296 returns 0, but
       adjusted for dst may still be fine */
    if( days >= 75279 ||
        ( seconds >= 0 && days >= 75276 ) ||
        ( seconds < 0 && days <= DAYS_FROM_1900_TO_1970 ) )
        return( (time_t) -1 );
    /* 0 is year = 206, mon = 1, mday = 7, hour = 6, min = 28, sec = 16 */
#endif
    return( ( time_t ) seconds );
}
