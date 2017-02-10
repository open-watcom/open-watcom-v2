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
* Description:  Implementation of near free() and _nfree().
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"
#if defined(__RDOS__)
#include <rdos.h>
#endif


#if defined(__SMALL_DATA__)

_WCRTLINK void free( void *cstg )
{
    _nfree( cstg );
}

#endif

mheapptr    __MiniHeapFreeRover;

/* By setting __ALLOC_DEBUG it is possible to spot memory allocation errors in RDOS target */

#if defined( __RDOS__ ) && defined( __ALLOC_DEBUG )


_WCRTLINK void _nfree( void _WCNEAR *cstg )
{
    RdosFreeDebugMem( cstg );
}

#else

_WCRTLINK void _nfree( void _WCNEAR *cstg )
{
    mheapptr        heap1;
    mheapptr        heap2;

    if( cstg == NEAR_NULL )
        return;

#if defined( __RDOS__ )
    if( RdosIsForked() ) {
        RdosFreeMem( cstg );
        return;
    }
#endif

    _AccessNHeap();
    do {
        // first try some likely locations
        heap1 = __MiniHeapFreeRover;
        if( heap1 != NULL ) {
            if( IS_IN_HEAP( cstg, heap1 ) ) {
                break;
            }
            heap2 = heap1;
            heap1 = heap1->prev;
            if( heap1 != NULL ) {
                if( IS_IN_HEAP( cstg, heap1 ) ) {
                    break;
                }
            }
            heap1 = heap2->next;
            if( heap1 != NULL ) {
                if( IS_IN_HEAP( cstg, heap1 ) ) {
                    break;
                }
            }
        }
        heap1 = __MiniHeapRover;
        if( heap1 != NULL ) {
            if( IS_IN_HEAP( cstg, heap1 ) ) {
                break;
            }
            heap2 = heap1;
            heap1 = heap1->prev;
            if( heap1 != NULL ) {
                if( IS_IN_HEAP( cstg, heap1 ) ) {
                    break;
                }
            }
            heap1 = heap2->next;
            if( heap1 != NULL ) {
                if( IS_IN_HEAP( cstg, heap1 ) ) {
                    break;
                }
            }
        }

        // not found near rover, so search the list
        for( heap1 = __nheapbeg; heap1 != NULL; heap1 = heap1->next ) {
            if( IS_IN_HEAP( cstg, heap1 ) ) {
                // break twice!
                goto found_it;
            }
        }

        // this pointer is not in the heap
        _ReleaseNHeap();
        return;
    } while( 0 );

found_it:
    // we found the miniheap, free the storage
    __MemFree( (unsigned)cstg, _DGroup(), (unsigned)heap1 );
    __MiniHeapFreeRover = heap1;
    if( heap1 < __MiniHeapRover ) {
        if( __LargestSizeB4MiniHeapRover < heap1->largest_blk ) {
            __LargestSizeB4MiniHeapRover = heap1->largest_blk;
        }
    }
    _ReleaseNHeap();
}

#endif
