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


#define FRLBASEDPTR(seg,p,s)    ((freelistp BASED_SEG_PTR( seg ))((char BASED_VOID_PTR)p + s))

int __HeapManager_expand( __segment seg, unsigned offset, size_t req_size, size_t *growth_size )
{
    miniheapblkp    BASED_SEG_PTR( seg ) hblk;
    freelistp       BASED_SEG_PTR( seg ) p1;
    freelistp       BASED_SEG_PTR( seg ) p2;
    freelistp       BASED_SEG_PTR( seg ) pnext;
    freelistp       BASED_SEG_PTR( seg ) pprev;
    size_t          new_size;
    size_t          old_size;
    size_t          free_size;

    /* round (new_size + tag) to multiple of pointer size */
    new_size = (req_size + TAG_SIZE + ROUND_SIZE) & ~ROUND_SIZE;
    if( new_size < req_size )
        new_size = ~0; //go for max
    if( new_size < FRL_SIZE ) {
        new_size = FRL_SIZE;
    }
    p1 = FRLBASEDPTR( seg, offset, -TAG_SIZE );
    old_size = p1->len & ~1;
    if( new_size > old_size ) {
        /* enlarging the current allocation */
        p2 = FRLBASEDPTR( seg, p1, old_size );
        *growth_size = new_size - old_size;
        for( ;; ) {
            free_size = p2->len;
            if( p2->len == END_TAG ) {
                return( __HM_TRYGROW );
            } else if( free_size & 1 ) { /* next piece is allocated */
                break;
            } else {
                pnext = p2->next;
                pprev = p2->prev;

                if( seg == _DGroup() ) { // near heap
                    for( hblk = __nheapbeg; hblk->next; hblk = hblk->next ) {
                        if( FRLBASEDPTR( seg, hblk, 0 ) <= FRLBASEDPTR( seg, offset, 0 )
                          && FRLBASEDPTR( seg, hblk, hblk->len ) > FRLBASEDPTR( seg, offset, 0 ) ) {
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
                    p2 = FRLBASEDPTR( seg, p2, free_size );
                } else {
                    p2 = FRLBASEDPTR( seg, p2, *growth_size );
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
            p1->len = new_size | 1;
            p1 = FRLBASEDPTR( seg, p1, new_size );
            p1->len = (old_size - new_size) | 1;
            if( seg == _DGroup() ) { // near heap
                for( hblk = __nheapbeg; hblk->next; hblk = hblk->next ) {
                    if( FRLBASEDPTR( seg, hblk, 0 ) <= FRLBASEDPTR( seg, offset, 0 )
                      && FRLBASEDPTR( seg, hblk, hblk->len ) > FRLBASEDPTR( seg, offset, 0 ) ) {
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
            _bfree( seg, FRLBASEDPTR( seg, p1, TAG_SIZE ) );
            /* free the top portion */
#else
            _nfree( FRLBASEDPTR( seg, p1, TAG_SIZE ) );
#endif
        }
    }
    return( __HM_SUCCESS );
}
