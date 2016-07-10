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
#include <stdlib.h>
#include <malloc.h>
#if defined(__QNX__)
    #include <sys/seginfo.h>
#elif defined(__OS2__)
    #include <dos.h>
    #define INCL_DOSMEMMGR
    #include <wos2.h>
    #include "tinyos2.h"
#elif defined(__WINDOWS__)
    #include <dos.h>
    #include <windows.h>
#else
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "heap.h"
#include "heapacc.h"
#include "seterrno.h"


int __HeapMin( __segment seg, unsigned one_seg )
{
    tag                 last_len;
    tag                 adjust_len;
    tag                 new_heap_len;
    __segment           heap_seg;
    heapblk             _WCFAR *heap;
    farfrlptr           last_free;
    farfrlptr           end_tag;
#if !( defined(__QNX__) || defined(__WINDOWS__) )
    tiny_ret_t          rc;
#endif

    _AccessFHeap();
    while( seg != _NULLSEG ) {
        heap = MK_FP( seg, 0 );
        /* we might free this segment so get the next one now */
        heap_seg = seg;
        seg = heap->nextseg;
        if( heap->numfree == 0 ) {      /* full heap */
            if( one_seg != 0 )
                break;
            continue;
        }
        if( heap->numalloc == 0 ) {     /* empty heap */
            continue;
        }
        /* verify the last block is free */
        last_free = MK_FP( heap_seg, heap->freehead.prev );
        if( (last_free->len & 1) != 0 )
            continue;

        /* verify the last block is just before the end of the heap */
        last_len = last_free->len;
        end_tag = (farfrlptr)(((FARPTR) last_free) + last_len );
        if( end_tag->len != END_TAG )
            continue;

        /* adjust sizes so the last free block stays in the heap */
        if( last_len <= FRL_SIZE )
            continue;

        new_heap_len = heap->heaplen - ( last_len - FRL_SIZE );
        new_heap_len = __ROUND_UP_SIZE( new_heap_len, 16 );
        adjust_len = heap->heaplen - new_heap_len;
        if( adjust_len == 0 )
            continue;

#if defined(__QNX__)
        if( qnx_segment_realloc( heap_seg, new_heap_len ) == -1 ) {
            _ReleaseFHeap();
            return( -1 );
        }
#elif defined(__WINDOWS__)
        {
            HANDLE hmem;

            hmem = (HANDLE)GlobalHandle( heap_seg );
            if( hmem == NULL ) {
                _ReleaseFHeap();
                return( -1 );
            }
            if( GlobalReAlloc( hmem, new_heap_len, GMEM_ZEROINIT ) == NULL){
                _ReleaseFHeap();
                return( -1 );
            }
        }
#else
  #if defined(__OS2__)
        rc = DosReallocSeg( new_heap_len, heap_seg );
  #else
        if( new_heap_len != 0 ) {
            rc = TinySetBlock( __ROUND_DOWN_SIZE_TO_PARA( new_heap_len ), heap_seg );
        } else {
            rc = TinySetBlock( PARAS_IN_64K, heap_seg );
        }
  #endif
        if( TINY_ERROR( rc ) ) {
            _ReleaseFHeap();
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
#endif

        /* make the changes to the heap structure */
        heap->heaplen = new_heap_len;
        last_free->len -= adjust_len;
        end_tag = (farfrlptr)(((FARPTR) last_free) + last_free->len );
        end_tag->len = END_TAG;
        end_tag->prev = 0;
    }
    _ReleaseFHeap();
    return( _HEAPOK );
}
