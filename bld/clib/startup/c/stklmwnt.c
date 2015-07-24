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

    VirtualQuery( &dummy, &mbi, sizeof( mbi ) );

    top = ((unsigned)mbi.BaseAddress) + mbi.RegionSize;
    low = (unsigned)mbi.AllocationBase;

    if( WIN32_IS_NT ) {
        low += 4096 * 3;
    } else if( WIN32_IS_WIN32S ) {
        low += 4096 * (16 + 2);
    } else {
        // Win 95
        low += 4096 * (16 + 3);
    }

    // check to make sure our low point adjustments are sane.
    // at the time of this patch, Open Watcom defaults to 64KB stack size for Win32s.
    // Windows 3.1 actually honors the stack reservation size and gives us 64KB of stack.
    // if you do the math above for the Win32s case, you'll realize our low point adjustment
    // pushes the low point 72KB from the base which is 8KB past the top of the stack!
    // the true fix to the problem would be to set the default stack size for Win32s to
    // something larger (128KB), and to take the Windows 95 default of 1MB if targeting
    // Windows 95 or NT. But, if we were told to use a smaller stack, then we need to
    // deal with it. If the user compiled their EXE with a smaller stack then stack overflow
    // issues are their problem, not ours.
    {
        // cap the low point adjustment so that it does not go beyond 1/2 the overall stack size.
        unsigned mid = ((((unsigned)mbi.AllocationBase + top) / 2) + 0x7FF) & (~0xFFF); // align to 4K page
        if (low > mid) low = mid;
    }

    if( stacklow ) {
        *stacklow = low;
    }
    if( stacktop ) {
        *stacktop = top;
    }
}
