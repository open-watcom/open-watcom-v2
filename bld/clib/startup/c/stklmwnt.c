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
* Description:  Stack limit setup for Win32.
*
****************************************************************************/


#include "variety.h"
#include "exitwmsg.h"
#include "osver.h"
#include <stdlib.h>
#include <windows.h>
#include "stacklow.h"

void __init_stack_limits( unsigned *stacklow, unsigned *stacktop )
{
    unsigned                    low;
    unsigned                    top;
    int                         dummy;
    MEMORY_BASIC_INFORMATION    mbi;

    if (VirtualQuery( &dummy, &mbi, sizeof( mbi ) ) == 0)
        __fatal_runtime_error( "cannot determine stack information", 127 );

    top = ((unsigned)mbi.BaseAddress) + mbi.RegionSize;
    low = (unsigned)mbi.AllocationBase;

    if( WIN32_IS_NT ) {
        low += 4096 * 3;
    } else if( WIN32_IS_WIN32S ) {
	/* Windows 3.1 appears to only offer 64KB of stack. Assume a buffer zone of 16KB and tread carefully. */
        low += 4096 * 4;
    } else {
        // Win 95
        low += 4096 * (16 + 3);
    }
    if( stacklow ) {
        *stacklow = low;
    }
    if( stacktop ) {
        *stacktop = top;
    }
}
