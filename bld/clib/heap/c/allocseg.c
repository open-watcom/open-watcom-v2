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
* Description:  Memory block allocator.
*                (16-bit code only)
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
#else
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "rtdata.h"
#include "heap.h"

#if defined(__QNX__)
extern unsigned         __qnx_alloc_flags;
#endif

__segment __AllocSeg( unsigned int amount )
{
    unsigned    n;              /* number of paragraphs desired   */
    __segment   seg;
    heapstart   _WCFAR *p;
    tag         _WCFAR *last_tag;
#if defined(__OS2__)
#elif defined(__QNX__)
    unsigned    rc;
#elif defined(__WINDOWS__)
#else
    tiny_ret_t  rc;
#endif

    if( !__heap_enabled )
        return( _NULLSEG );
    /*               heapinfo + frl + frl,       end tags */
    if( amount > - ( sizeof( heapstart ) + TAG_SIZE * 2 ) ) {
        return( _NULLSEG );
    }
    /*        heapinfo + frl,        allocated blk,  end tags */
    amount += sizeof( heapblk ) + TAG_SIZE + TAG_SIZE * 2;
    if( amount < _amblksiz )
        amount = _amblksiz;
    n = ( amount + 0x0f ) >> 4;
    if( n == 0 )
        n = 0x1000;
#if defined(__OS2__)
    seg = _NULLSEG;
    if( DosAllocSeg( n << 4, (PSEL)&seg, 0 ) )
        return( _NULLSEG );
#elif defined(__QNX__)
    rc = qnx_segment_alloc_flags( ((long)n) << 4, __qnx_alloc_flags );
    if( rc == (unsigned)-1 )
        return( _NULLSEG );
    seg = (__segment)rc;
#elif defined(__WINDOWS__)
    {
        HANDLE hmem;
        LPSTR p;

        hmem = GlobalAlloc( __win_alloc_flags, ((long)n) << 4 );
        if( hmem == NULL )
            return( _NULLSEG );
        p = GlobalLock( hmem );
        if( p == NULL ) {
            GlobalFree( hmem );
            return( _NULLSEG );
        }
  #if 0
        /* code generator can't handle this */
        if( FP_OFF( p ) != 0 ) {    /* in case, Microsoft changes Windows */
            GlobalUnlock( hmem );   /* in post 3.1 versions */
            GlobalFree( hmem );
            return( _NULLSEG );
        }
  #endif
        seg = FP_SEG( p );
    }
#else
    rc = TinyAllocBlock( n );
    if( TINY_ERROR( rc ) ) {
        return( _NULLSEG );
    }
    seg = TINY_INFO( rc );
#endif
    p = (heapstart _WCFAR *)MK_FP( seg, 0 );
    p->h.heaplen = n << 4;
    p->h.prevseg = _NULLSEG;
    p->h.nextseg = _NULLSEG;
    p->h.rover   = offsetof( heapstart, first );
    p->h.b4rover  = 0;
    p->h.numalloc = 0;
    p->h.numfree  = 1;
    p->h.freehead.len  = 0;
    p->h.freehead.prev = offsetof( heapstart, first );
    p->h.freehead.next = offsetof( heapstart, first );
    p->first.len  = p->h.heaplen - sizeof( heapblk ) - TAG_SIZE * 2;
    p->h.largest_blk = p->first.len;
    p->first.prev = offsetof( heapblk, freehead );
    p->first.next = offsetof( heapblk, freehead );
    last_tag = MK_FP( seg, p->h.heaplen - TAG_SIZE * 2 );
    *last_tag = END_TAG;
    last_tag[1] = 0;        /* link to next piece of near heap */
    return( seg );          /* return allocated segment */
}
