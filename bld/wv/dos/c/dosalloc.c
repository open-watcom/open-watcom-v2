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


#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <dos.h>
#include "tinyio.h"
#include "dosheap.h"

#include "dbgswtch.h"
extern struct heapstart         *LastSeg;
extern void                     *SyMemBeg;
extern void                     *SyMemEnd;

#pragma aux __AllocSeg modify[];
unsigned __AllocSeg( unsigned int amount )
    {
        unsigned n;             /* number of paragraphs desired   */
        short seg;
        struct heapstart far *p;
        tag far *last_tag;
        tiny_ret_t rc;

        /*               heapinfo + frl + frl,       end tags */
        if( amount > - (sizeof(struct heapstart) + sizeof(tag)*2 ) ) {
            return( 0 );
        }
        /*        heapinfo + frl,        allocated blk,  end tags */
        amount += sizeof(struct heapblk) + sizeof(tag) + sizeof(tag) * 2;
        if( amount < _amblksiz )  amount = _amblksiz;
        n = ( amount + 0x0f ) >> 4;
        if( n == 0 )  n = 0x1000;                       /* 23-may-89 */
        if( LastSeg == NULL ) {
            seg = FP_SEG( SyMemBeg );
        } else if( LastSeg->h.heaplen == 0 ) {
            seg = FP_SEG( LastSeg ) + 0x1000;
        } else {
            seg = FP_SEG( LastSeg ) + ( LastSeg->h.heaplen >> 4 );
        }
        if( seg + n > FP_SEG( SyMemEnd ) || seg + n < seg ) {
            if( _IsOn( SW_REMOTE_LINK ) ) {
                rc = TinyAllocBlock( n );
                if( TINY_ERROR( rc ) ) {
                    return( 0 );
                }
                seg = TINY_INFO( rc );
            } else {
                return( 0 );
            }
        } else {
            LastSeg = MK_FP( seg, 0 );
        }
        p = (struct heapstart far *)MK_FP( seg, 0 );
        p->h.heaplen = n << 4;
        p->h.prevseg = 0;
        p->h.nextseg = 0;
        p->h.rover   = offsetof( struct heapstart, first );
        p->h.b4rover     = 0;
        p->h.numalloc = 0;
        p->h.numfree  = 1;
        p->h.freehead.len  = 0;
        p->h.freehead.prev = offsetof( struct heapstart, first );
        p->h.freehead.next = offsetof( struct heapstart, first );
        p->first.len  = p->h.heaplen - sizeof(struct heapblk) - sizeof(tag)*2;
        p->h.largest_blk = p->first.len;
        p->first.prev = offsetof( struct heapblk, freehead );
        p->first.next = offsetof( struct heapblk, freehead );
        last_tag = MK_FP( seg, p->h.heaplen - sizeof(tag)*2 );
        *last_tag = END_TAG;
        last_tag[1] = 0;        /* link to next piece of near heap */
        return( seg );          /* return allocated segment */
    }

/*
 * these stubs prevent the memory manager from trying to allocate near
 * memory
 */

frlptr near __nheap;

void __near *_nmalloc( size_t amt ) {
    amt = amt;
    return( 0 );
}

void _nfree( void __near *p )
{
    p = p;
}

void _nheapgrow( void )
{
}

void __near *_nexpand( void __near *p, size_t size )
{
    p = p; size = size;
    return( NULL );
}

