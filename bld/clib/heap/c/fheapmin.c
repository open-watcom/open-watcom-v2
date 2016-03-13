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
* Description:  Implementation of far _heapmin() and _fheapmin().
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"

#if defined(__OS2__)
    #if defined( _M_I86 )
        #if defined(__BIG_DATA__)
            #define MODIFIES ds es
        #else
            #define MODIFIES es
        #endif
    #endif
#elif defined(__WINDOWS__)
    #define MODIFIES es
#endif

#if defined(__BIG_DATA__)

#if defined(__WINDOWS_286__) || defined(__OS2_286__)
    #pragma aux _heapmin modify [MODIFIES]
    #pragma aux _heapshrink modify [MODIFIES]
#endif

_WCRTLINK int _heapshrink( void )
{
    return( _fheapshrink() );
}

_WCRTLINK int _heapmin( void )
{
    return( _fheapshrink() );
}

#endif

#if defined(__WINDOWS_286__) || defined(__OS2_286__)
    #pragma aux _fheapmin modify [MODIFIES]
    #pragma aux _fheapshrink modify [MODIFIES]
#endif

_WCRTLINK int _fheapmin( void )
{
    return( _fheapshrink() );
}

_WCRTLINK int _fheapshrink( void )
{
    __segment   seg;
    __segment   heap_seg;
    __segment   prev_seg;
    int         heap_status;
    heapblk     _WCFAR *heap;
    heapblk     _WCFAR *next_heap;
    heapblk     _WCFAR *prev_heap;

    heap_status = __HeapMin( __fheap, 0 );
    _AccessFHeap();
    for( seg = __fheap; seg != _NULLSEG; ) {
        heap = MK_FP( seg, 0 );
        /* we might free this segment so get the next one now */
        heap_seg = seg;
        seg = heap->nextseg;
        if( heap->numalloc == 0 ) {     /* empty heap */
            /* unlink from heap list */
            prev_seg = heap->prevseg;
            if( seg != _NULLSEG ) {
                next_heap = MK_FP( seg, 0 );
                next_heap->prevseg = prev_seg;
            }
            if( prev_seg == _NULLSEG ) {
                __fheap = seg;
            } else {
                prev_heap = MK_FP( prev_seg, 0 );
                prev_heap->nextseg = seg;
            }
            __fheapRover = __fheap;
            heap_status = __FreeSeg( heap_seg );
        }
    }
    _ReleaseFHeap();
    return( heap_status );
}
