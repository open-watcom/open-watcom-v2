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
#include <sys/timers.h>
#include <sys/timeb.h>

_WCRTLINK int ftime( struct timeb *timeptr )
{
    struct timespec ts;
    time_t          timer;
    struct tm       *tm_ptr;

    if( getclock( TIMEOFDAY, &ts ) == -1 )
        return( -1 );

    timer = ( time_t ) ts.tv_sec;
    if( ts.tv_nsec >= 500000000L )
        timer++;

    tm_ptr = localtime( &timer );

    timeptr->dstflag  = tm_ptr->tm_isdst;
    timeptr->time     = ( time_t ) ts.tv_sec;
    timeptr->millitm  = ts.tv_nsec / 1000000;
    timeptr->timezone = timezone / 60L;
    return( 1 );
}
