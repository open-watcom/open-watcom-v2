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

_WCRTLINK void free( void *stg )
{
    _nfree( stg );
}

#endif

mheapptr    __MiniHeapFreeRover;

/* By setting __ALLOC_DEBUG it is possible to spot memory allocation errors in RDOS target */

#if defined( __RDOS__ ) && defined( __ALLOC_DEBUG )


_WCRTLINK void _nfree( void _WCNEAR *stg )
{
    RdosFreeDebugMem( stg );
}

#else

_WCRTLINK void _nfree( void _WCNEAR *stg )
{
    mheapptr        mhp1;
    mheapptr        mhp2;

    if( !stg )
        return;

#if defined( __RDOS__ )
    if( RdosIsForked() ) {
        RdosFreeMem( stg );
        return;
    }
#endif

    _AccessNHeap();
    do {
        // first try some likely locations
        mhp1 = __MiniHeapFreeRover;
        if( mhp1 != NULL ) {
            if( (PTR)mhp1 <= (PTR)stg && (PTR)mhp1 + mhp1->len > (PTR)stg ) {
                break;
            }
            mhp2 = mhp1;
            mhp1 = mhp1->prev;
            if( mhp1 != NULL ) {
                if( (PTR)mhp1 <= (PTR)stg && (PTR)mhp1 + mhp1->len > (PTR)stg ) {
                    break;
                }
            }
            mhp1 = mhp2->next;
            if( mhp1 != NULL ) {
                if( (PTR)mhp1 <= (PTR)stg && (PTR)mhp1 + mhp1->len > (PTR)stg ) {
                    break;
                }
            }
        }
        mhp1 = __MiniHeapRover;
        if( mhp1 != NULL ) {
            if( (PTR)mhp1 <= (PTR)stg && (PTR)mhp1 + mhp1->len > (PTR)stg ) {
                break;
            }
            mhp2 = mhp1;
            mhp1 = mhp1->prev;
            if( mhp1 != NULL ) {
                if( (PTR)mhp1 <= (PTR)stg && (PTR)mhp1 + mhp1->len > (PTR)stg ) {
                    break;
                }
            }
            mhp1 = mhp2->next;
            if( mhp1 != NULL ) {
                if( (PTR)mhp1 <= (PTR)stg && (PTR)mhp1 + mhp1->len > (PTR)stg ) {
                    break;
                }
            }
        }

        // not found near rover, so search the list
        for( mhp1 = __nheapbeg; mhp1 != NULL; mhp1 = mhp1->next ) {
            if( (PTR)mhp1 <= (PTR)stg && (PTR)mhp1 + mhp1->len > (PTR)stg ) {
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
    __MemFree( (unsigned)stg, _DGroup(), (unsigned)mhp1 );
    __MiniHeapFreeRover = mhp1;
    if( mhp1 < __MiniHeapRover ) {
        if( __LargestSizeB4MiniHeapRover < mhp1->largest_blk ) {
            __LargestSizeB4MiniHeapRover = mhp1->largest_blk;
        }
    }
    _ReleaseNHeap();
}

#endif
