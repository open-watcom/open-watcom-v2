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
* Description:  RDOS implementation of clock().
*
****************************************************************************/


#include "variety.h"
#include <time.h>
#include <rtinit.h>
#include <rdos.h>


static int    init_milliseconds;

static unsigned long __get_tick( void )
{
    unsigned long           tick_count;
    unsigned long           msb;
    unsigned long           lsb;

    RdosGetSysTime(&msb, &lsb);

    tick_count = lsb / 1193;
    tick_count += msb * 3538944;

    return( tick_count );
}

/* The system millisecond counter will wrap after ~49 days. This
 * is is not considered a real problem and consistent with
 * the behaviour of other C runtimes.
 */
_WCRTLINK clock_t clock( void )
{
    return( (clock_t)(__get_tick() - init_milliseconds) );
}

static void __clock_init( void )
{
    init_milliseconds = __get_tick();
}

AXI( __clock_init, INIT_PRIORITY_LIBRARY )
