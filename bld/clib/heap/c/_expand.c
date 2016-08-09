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
* Description:  clib internal heap expansion routine.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


#define FRL_BPTR(seg,p,s)   ((freelistp SEG_BPTR( seg ))((char VOID_BPTR)p + s))

int __HeapManager_expand( __segment seg, unsigned offset, size_t req_size, size_t *growth_size )
{
    miniheapblkp    SEG_BPTR( seg ) hblk;
    freelistp       SEG_BPTR( seg ) p1;
    freelistp       SEG_BPTR( seg ) p2;
    freelistp       SEG_BPTR( seg ) pnext;
    freelistp       SEG_BPTR( seg ) pprev;
    size_t          new_size;
    size_t          old_size;
    size_t          free_size;

    /* round (new_size + tag) to multiple of pointer size */
    new_size = __ROUND_UP_SIZE( req_size + TAG_SIZE, ROUND_SIZE );
    if( new_size < req_size )
        new_size = ~0; //go for max
    if( new_size < FRL_SIZE ) {
        new_size = FRL_SIZE;
    }
    p1 = FRL_BPTR( seg, offset, -TAG_SIZE );
    old_size = MEMBLK_SIZE( p1 );
    if( new_size > old_size ) {
        /* enlarging the current allocation */
        p2 = FRL_BPTR( seg, p1, old_size );
        *growth_size = new_size - old_size;
        for( ;; ) {
            if( p2->len == END_TAG ) {
                return( __HM_TRYGROW );
            } else if( IS_MEMBLK_USED( p2 ) ) { /* next piece is allocated */
                break;
            } else {
                free_size = p2->len;
                pnext = p2->next;
                pprev = p2->prev;

                if( seg == _DGroup() ) { // near heap
                    for( hblk = __nheapbeg; hblk->next != NULL; hblk = hblk->next ) {
                        if( FRL_BPTR( seg, hblk, 0 ) <= FRL_BPTR( seg, offset, 0 )
                          && FRL_BPTR( seg, hblk, hblk->len ) > FRL_BPTR( seg, offset, 0 ) ) {
                            break;
                        }
                    }
#if defined( _M_I86 )
                } else {      // Based heap
                    hblk = 0;
#endif
                }

                if( hblk->rover == p2 ) {
                    hblk->rover = p2->prev;
                }
                if( free_size < *growth_size || free_size - *growth_size < FRL_SIZE ) {
                    /* unlink small free block */
                    pprev->next = pnext;
                    pnext->prev = pprev;
                    p1->len += free_size;
                    hblk->numfree--;
                    if( free_size >= *growth_size ) {
                        return( __HM_SUCCESS );
                    }
                    *growth_size -= free_size;
                    p2 = FRL_BPTR( seg, p2, free_size );
                } else {
                    p2 = FRL_BPTR( seg, p2, *growth_size );
                    p2->len = free_size - *growth_size;
                    p2->prev = pprev;
                    p2->next = pnext;
                    pprev->next = p2;
                    pnext->prev = p2;
                    p1->len += *growth_size;
                    return( __HM_SUCCESS );
                }
            }
        }
        /* no suitable free blocks behind, have to move block */
        return( __HM_FAIL );
    } else {
        /* shrinking the current allocation */
        if( old_size - new_size >= FRL_SIZE ) {
            /* block big enough to split */
            SET_MEMBLK_SIZE_USED( p1, new_size );
            p1 = FRL_BPTR( seg, p1, new_size );
            SET_MEMBLK_SIZE_USED( p1, old_size - new_size );
            if( seg == _DGroup() ) { // near heap
                for( hblk = __nheapbeg; hblk->next != NULL; hblk = hblk->next ) {
                    if( FRL_BPTR( seg, hblk, 0 ) <= FRL_BPTR( seg, offset, 0 )
                      && FRL_BPTR( seg, hblk, hblk->len ) > FRL_BPTR( seg, offset, 0 ) ) {
                        break;
                    }
                }
#if defined( _M_I86 )
            } else {    // Based heap
                hblk = 0;
#endif
            }
            /* _bfree will decrement 'numalloc' 08-jul-91 */
            hblk->numalloc++;
#if defined( _M_I86 )
            _bfree( seg, FRL_BPTR( seg, p1, TAG_SIZE ) );
            /* free the top portion */
#else
            _nfree( FRL_BPTR( seg, p1, TAG_SIZE ) );
#endif
        }
    }
    return( __HM_SUCCESS );
}
