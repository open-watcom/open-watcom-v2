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
* Description:  RDOS gettimeofday() implementation.
*
****************************************************************************/


#include "variety.h"
#include <time.h>
#include <sys/time.h>
#include "rdos.h"
#include "_rdos.h"

_WCRTLINK int gettimeofday( struct timeval *tv, struct timezone *tz )
{
    long long longtime;
    long long basetime;
    unsigned long msb;

    /* unused parameters */ (void)tz;

    msb = RdosCodeMsbTics( 2010, 1, 1, 0 );
    basetime = (long long)msb;
    basetime = basetime << 32;
    longtime = RdosGetLongTime();
    longtime -= basetime;

    tv->tv_sec = (long)(longtime / 1193182);
    longtime = longtime - (long long)(tv->tv_sec) * 1193182;
    longtime = longtime * 1000000 / 1193182;
    tv->tv_usec = (long)longtime;

    while( tv->tv_usec < 0 ) {
        tv->tv_usec += 1000000;
        tv->tv_sec--;
    }

    while( tv->tv_usec >= 1000000 ) {
        tv->tv_usec -= 1000000;
        tv->tv_sec++;
    }

    return( 0 );
}
