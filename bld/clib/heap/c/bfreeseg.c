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


#include "dll.h"        // needs to be first
#include "variety.h"
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"

#if defined(__OS2_286__)
    #if defined(__BIG_DATA__)
        #define MODIFIES ds es
    #else
        #define MODIFIES es
    #endif
#elif defined(__WINDOWS_286__)
    #define MODIFIES es
#endif

#if defined(__WINDOWS_286__) || defined(__OS2_286__)
    #pragma aux _bfreeseg modify [MODIFIES]
#endif

_WCRTLINK int _bfreeseg( __segment seg )
{
    __segment   heap_seg;
    __segment   prev_seg;
    heapblk     _WCFAR *heap;
    heapblk     _WCFAR *next_heap;
    heapblk     _WCFAR *prev_heap;

    _AccessFHeap();
    heap = MK_FP( seg, 0 );
    heap_seg = seg;
    seg = heap->nextseg;
    /* unlink from heap list */
    prev_seg = heap->prevseg;
    if( seg != 0 ) {
        next_heap = MK_FP( seg, 0 );
        next_heap->prevseg = prev_seg;
    }
    if( prev_seg == _NULLSEG ) {
        __bheap = seg;
    } else {
        prev_heap = MK_FP( prev_seg, 0 );
        prev_heap->nextseg = seg;
    }
    _ReleaseFHeap();
    return( __FreeSeg( heap_seg ) );
}
