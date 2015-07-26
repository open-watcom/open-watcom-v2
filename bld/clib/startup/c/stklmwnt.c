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

/* Reserved stack space we need for stack overflow processing by C run-time   */
/* library and by OS. All these values looks like empirically derived values. */
#define STACK_RESERVED_SPACE_NT     (4096 * 3)
#define STACK_RESERVED_SPACE_WIN9X  (4096 * (16 + 3))
#define STACK_RESERVED_SPACE_WIN32S (4096 * (16 + 2))

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
        low += STACK_RESERVED_SPACE_NT;
    } else if( WIN32_IS_WIN32S ) {
        low += STACK_RESERVED_SPACE_WIN32S;
    } else {    // Win 9x
        low += STACK_RESERVED_SPACE_WIN9X;
    }
    if( stacklow ) {
        *stacklow = low;
    }
    if( stacktop ) {
        *stacktop = top;
    }
}
