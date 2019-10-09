/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__BIG_DATA__)

_WCRTLINK void free( void *cstg )
{
    _ffree( cstg );
}

#endif

_WCRTLINK void _ffree( void_fptr cstg )
{
    __segment   seg;

    seg = _FP_SEG( cstg );
    if( seg == _NULLSEG ) {
        return;
    }
    if( seg == _DGroup() ) {
        _nfree( (void_nptr)_FP_OFF( cstg ) );
        return;
    }
    _AccessFHeap();
    __MemFree( (void_bptr)cstg, seg, 0 );
    if( seg != __fheapRover ) {
        // seg might be after the __fheapRover, but we don't know that
        // and it might be expensive to find out. We will just update
        // __LargestSizeB4Rover anyway. The worst that will happen is
        // that _fmalloc will start searching from __fheapbeg when it could
        // have started at __fheapRover.
        if( __LargestSizeB4Rover < BHEAP( seg )->largest_blk ) {
            __LargestSizeB4Rover = BHEAP( seg )->largest_blk;
        }
    }
    _ReleaseFHeap();
}
