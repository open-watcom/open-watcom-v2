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
* Description:  Heap minimize function
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#if defined(__QNX__)
    #include <sys/seginfo.h>
#elif defined(__OS2__)
    #include <dos.h>
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined(__WINDOWS__)
    #include <dos.h>
    #include <windows.h>
#else   /* __DOS__ */
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "heap.h"
#include "heapacc.h"
#include "seterrno.h"


int __HeapMin( __segment seg, __segment one_heap )
{
    tag                 last_len;
    tag                 adjust_len;
    tag                 new_heap_len;
    __segment           next_seg;
    FRLPTR( seg )       last_free;
    FRLPTR( seg )       end_tag;
#if defined(__OS2__)
    APIRET              rc;
#elif defined(__WINDOWS__)
    HANDLE              hmem;
#elif defined(__DOS__)
    unsigned            num_of_paras;
    tiny_ret_t          rc;
#endif

    _AccessFHeap();
    for( ; seg != _NULLSEG; seg = next_seg ) {
        /* we might free this segment so get the next one now */
        next_seg = BHEAP( seg )->next.segm;
        if( BHEAP( seg )->numfree == 0 ) {      /* full heap */
            if( one_heap != _NULLSEG )
                break;
            continue;
        }
        if( BHEAP( seg )->numalloc == 0 ) {     /* empty heap */
            continue;
        }
        /* verify the last block is free */
        last_free = BHEAP( seg )->freehead.prev.nptr;
        if( IS_BLK_INUSE( last_free ) )
            continue;

        /* verify the last block is just before the end of the heap */
        end_tag = (FRLPTR( seg ))NEXT_BLK( last_free );
        if( !IS_BLK_END( end_tag ) )
            continue;

        /* adjust sizes so the last free block stays in the heap */
        last_len = last_free->len;
        if( last_len <= FRL_SIZE )
            continue;

        new_heap_len = __ROUND_UP_SIZE_PARA( BHEAP( seg )->len - ( last_len - FRL_SIZE ) );
        adjust_len = BHEAP( seg )->len - new_heap_len;
        if( adjust_len == 0 )
            continue;

#if defined(__QNX__)
        if( qnx_segment_realloc( seg, new_heap_len ) == -1 ) {
            _ReleaseFHeap();
            return( -1 );
        }
#elif defined(__WINDOWS__)
        hmem = (HANDLE)GlobalHandle( seg );
        if( hmem == NULL ) {
            _ReleaseFHeap();
            return( -1 );
        }
        if( GlobalReAlloc( hmem, new_heap_len, GMEM_ZEROINIT ) == NULL ) {
            _ReleaseFHeap();
            return( -1 );
        }
#elif defined(__OS2__)
        rc = DosReallocSeg( new_heap_len, seg );
        if( rc ) {
            _ReleaseFHeap();
            return( __set_errno_dos( rc ) );
        }
#else   /* __DOS__ */
        if( new_heap_len == 0 ) {
            num_of_paras = PARAS_IN_64K;
        } else {
            num_of_paras = __ROUND_DOWN_SIZE_TO_PARA( new_heap_len );
        }
        rc = TinySetBlock( num_of_paras, seg );
        if( TINY_ERROR( rc ) ) {
            _ReleaseFHeap();
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
#endif
        /* make the changes to the heap structure */
        BHEAP( seg )->len = new_heap_len;
        last_free->len -= adjust_len;
        SET_HEAP_END( seg, NEXT_BLK( last_free ) );
    }
    _ReleaseFHeap();
    return( _HEAPOK );
}
