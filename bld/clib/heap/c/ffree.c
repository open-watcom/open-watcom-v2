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
* Description:  Implementation of far free() and _ffree().
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include <i86.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__BIG_DATA__)

_WCRTLINK void free( void *stg )
{
    _ffree( stg );
}

#endif

_WCRTLINK void _ffree( void _WCFAR *stg )
{
    heapblk     _WCFAR *p;
    __segment   seg;

    seg = FP_SEG( stg );
    if( seg == _NULLSEG ) {
        return;
    }
    if( seg == _DGroup() ) {
        _nfree( (void _WCNEAR *)FP_OFF( stg ) );
        return;
    }
    _AccessFHeap();
    __MemFree( FP_OFF( stg ), seg, 0 );
    if( seg != __fheapRover ) {                         /* 02-dec-92 */
        // seg might be after the __fheapRover, but we don't know that
        // and it might be expensive to find out. We will just update
        // __LargestSizeB4Rover anyway. The worst that will happen is
        // that _fmalloc will start searching from __fheap when it could
        // have started at __fheapRover.
        p = MK_FP( seg, 0 );
        if( p->largest_blk > __LargestSizeB4Rover ) {
            __LargestSizeB4Rover = p->largest_blk;
        }
    }
    _ReleaseFHeap();
}
