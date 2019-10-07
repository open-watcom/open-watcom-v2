/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Extend segment size
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#if defined(__OS2__)
    #include <dos.h>
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined(__QNX__)
    #include <sys/types.h>
    #include <sys/seginfo.h>
#elif defined(__WINDOWS__)
    #include <dos.h>
    #include <windows.h>
#else   /* __DOS__ */
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "rtdata.h"
#include "heap.h"


int __GrowSeg( __segment seg, unsigned int amount )
{
    unsigned        num_of_paras;   /* number of paragraphs desired   */
    unsigned        new_heaplen;
    unsigned int    old_heaplen;
    unsigned int    old_num_of_paras;
    FRLPTR( seg )   pfree;
    FRLPTR( seg )   pnew;

    if( !__heap_enabled )
        return( 0 );
    old_heaplen = BHEAP( seg )->len;
    if( old_heaplen == 0 )
        return( 0 );    /* indicate failed to grow the segment */
    /* if not already 64K */
    amount += TAG_SIZE;
    if( amount < TAG_SIZE )
        amount = /*0x....ffff*/ ~0U;
    if( amount < _RWD_amblksiz )
        amount = _RWD_amblksiz;
    num_of_paras = __ROUND_UP_SIZE_TO_PARA( amount );
    if( num_of_paras == 0 )
        num_of_paras = PARAS_IN_64K;
    old_num_of_paras = __ROUND_DOWN_SIZE_TO_PARA( old_heaplen );
    num_of_paras += old_num_of_paras;
    /*
        We shouldn't extend segments to 64k if we are not going to
        use the space for this allocation.  In protected-mode
        environments, it should be possible to extend segments
        later on when we know we can use the space.
    */
#if defined(__QNX__)
    if( num_of_paras > PARAS_IN_64K )
        return( 0 );
    if( qnx_segment_realloc( seg, ((long)num_of_paras) << 4 ) == -1 )
        return( 0 );
#elif defined(__OS2__) || defined(__WINDOWS__)
    if( num_of_paras > PARAS_IN_64K ) {
        if( _osmode_PROTMODE() ) {          /* protected-mode */
            return( 0 );
        } else {
            num_of_paras = PARAS_IN_64K;
        }
    }
  #if defined(__OS2__)
    if( DosReallocSeg( num_of_paras << 4, seg ) != 0 )
        return( 0 );
  #else   /* __WINDOWS__ */
    if( old_num_of_paras < ( PARAS_IN_64K - 2 ) && num_of_paras == PARAS_IN_64K ) {
        num_of_paras = PARAS_IN_64K - 2;
    } else if( num_of_paras > ( PARAS_IN_64K - 2 ) ) {
        /*
          in Standard mode, GlobalRealloc may change selectors
          if size > 65519 (64k-17)! (p. 4-246 Windows Ref) AFS 23-apr-91
        */
        return( 0 );
    }
    {
        HANDLE hmem;

        hmem = (HANDLE)GlobalHandle( seg );
        if( hmem == NULL ) {
            return( 0 );
        }
        if( GlobalReAlloc( hmem, ((long)num_of_paras) << 4, __win_realloc_flags ) == NULL ) {
            return( 0 );
        }
    }
  #endif
#else   /* __DOS__ */
    if( num_of_paras > PARAS_IN_64K )
        num_of_paras = PARAS_IN_64K;
    if( TINY_ERROR( TinySetBlock( num_of_paras, seg ) ) )
        return( 0 );
#endif
    new_heaplen = num_of_paras << 4;
    BHEAP( seg )->len = new_heaplen;        /* put in new heap length */
    pfree = BHEAP( seg )->freehead.prev.nptr;
    if( NEXT_BLK( pfree ) != old_heaplen - TAG_SIZE * 2 ) {
        /* last free entry not at end of the heap */
        /* add a new free entry to end of list */
        pnew = (FRLPTR( seg ))( old_heaplen - TAG_SIZE * 2 );
        pnew->prev.nptr = pfree;
        pnew->next.nptr = pfree->next.nptr;
        pfree->next.nptr = pnew;
        BHEAP( seg )->freehead.prev.nptr = pnew;
        BHEAP( seg )->numfree++;
        pfree = pnew;
    }
    pfree->len = new_heaplen - _FP_OFF( pfree ) - TAG_SIZE * 2;
    if( BHEAP( seg )->largest_blk < pfree->len )
        BHEAP( seg )->largest_blk = pfree->len;
    SET_HEAP_END( seg, new_heaplen - 2 * TAG_SIZE );
    return( 1 );                /* indicate segment was grown */
}
