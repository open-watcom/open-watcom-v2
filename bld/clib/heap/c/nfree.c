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
#include <stddef.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__SMALL_DATA__)

_WCRTLINK void free( void *stg ) {
    _nfree( stg );
}

#endif

struct miniheapblkp _WCNEAR     *__MiniHeapFreeRover;

_WCRTLINK void _nfree( void _WCNEAR *stg ) {
    mheapptr            p1,p2;

    if( !stg ) return;

    _AccessNHeap();
    do {
        // first try some likely locations
        p1 = __MiniHeapFreeRover;
        if( p1 ) {
            if( (PTR)p1 <= (PTR)stg && (PTR)p1+p1->len > (PTR)stg ) {
                break;
            }
            p2 = p1;
            p1 = p1->prev;
            if( p1 ) {
                if( (PTR)p1 <= (PTR)stg && (PTR)p1+p1->len > (PTR)stg ) {
                    break;
                }
            }
            p1 = p2->next;
            if( p1 ) {
                if( (PTR)p1 <= (PTR)stg && (PTR)p1+p1->len > (PTR)stg ) {
                    break;
                }
            }
        }
        p1 = __MiniHeapRover;
        if( p1 ) {
            if( (PTR)p1 <= (PTR)stg && (PTR)p1+p1->len > (PTR)stg ) {
                break;
            }
            p2 = p1;
            p1 = p1->prev;
            if( p1 ) {
                if( (PTR)p1 <= (PTR)stg && (PTR)p1+p1->len > (PTR)stg ) {
                    break;
                }
            }
            p1 = p2->next;
            if( p1 ) {
                if( (PTR)p1 <= (PTR)stg && (PTR)p1+p1->len > (PTR)stg ) {
                    break;
                }
            }
        }

        // not found near rover, so search the list
        for( p1 = __nheapbeg; p1; p1 = p1->next ) {
            if( (PTR)p1 <= (PTR)stg && (PTR)p1+p1->len > (PTR)stg ) {
                // break twice!
                goto found_it;
            }
        }

        // this pointer is not in the heap
        _ReleaseNHeap();
        return;
    } while(0);

found_it:
    // we found the miniheap, free the storage
    __MemFree( (unsigned)stg, _DGroup(), (unsigned) p1 );
    __MiniHeapFreeRover = p1;
    if( p1 < __MiniHeapRover ) {
        if( p1->largest_blk > __LargestSizeB4MiniHeapRover ) {
            __LargestSizeB4MiniHeapRover = p1->largest_blk;
        }
    }
    __nheap_clean = 0;
    _ReleaseNHeap();
}
