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
* Description:  Free memory block
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


_WCRTLINK int _bfreeseg( __segment curr_seg )
{
    __segment   next_seg;
    __segment   prev_seg;
    heapblk     _WCFAR *curr_heap;
    heapblk     _WCFAR *next_heap;
    heapblk     _WCFAR *prev_heap;

    _AccessFHeap();
    curr_heap = MK_FP( curr_seg, 0 );
    /* unlink from heap list */
    next_seg = curr_heap->nextseg;
    prev_seg = curr_heap->prevseg;
    if( next_seg != _NULLSEG ) {
        next_heap = MK_FP( next_seg, 0 );
        next_heap->prevseg = prev_seg;
    }
    if( prev_seg == _NULLSEG ) {
        __bheapbeg = next_seg;
    } else {
        prev_heap = MK_FP( prev_seg, 0 );
        prev_heap->nextseg = next_seg;
    }
    _ReleaseFHeap();
    return( __FreeSeg( curr_seg ) );
}
