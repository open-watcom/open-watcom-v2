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
* Description:  Windows implementation of clock().
*
****************************************************************************/


#include "variety.h"
#include <time.h>
#include <rtinit.h>
#include <windows.h>

/* Windows gives us time in milliseconds, so we're OK as long as
 * clock() is also expected to return milliseconds.
 */
#if CLOCKS_PER_SEC != 1000
    #error OS returned time needs adjusting!
#endif

static DWORD    init_milliseconds;

/* The system millisecond counter will wrap after ~49 days. This
 * is is not considered a real problem and consistent with
 * the behaviour of other C runtimes.
 */
_WCRTLINK clock_t clock( void )
{
    return( (clock_t)(GetTickCount() - init_milliseconds) );
}

static void __clock_init( void )
{
    init_milliseconds = GetTickCount();
}

AXI( __clock_init, INIT_PRIORITY_LIBRARY )
