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
#include <errno.h>
#if defined(__OS2__)
    #include <dos.h>
    #include "tinyos2.h"
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined(__QNX__)
    #include <sys/types.h>
    #include <sys/seginfo.h>
    #include <i86.h>
#elif defined(__WINDOWS__) && defined( _M_I86 )
    #include <dos.h>
    #include "windows.h"
#else
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "heap.h"

#if defined(__WINDOWS_286__)
extern unsigned long _WCNEAR __win_alloc_flags;
#elif defined(__QNX__)
extern unsigned __qnx_alloc_flags;
#endif

unsigned __AllocSeg( unsigned int amount )
    {
        unsigned n;             /* number of paragraphs desired   */
        short seg;
        struct heapstart _WCFAR *p;
        tag _WCFAR *last_tag;
#if !(defined(__QNX__) || defined(__WINDOWS_286__) )
        tiny_ret_t rc;
#endif

        if( !__heap_enabled ) return( 0 );
        /*               heapinfo + frl + frl,       end tags */
        if( amount > - (sizeof(struct heapstart) + TAG_SIZE*2 ) ) {
            return( 0 );
        }
        /*        heapinfo + frl,        allocated blk,  end tags */
        amount += sizeof(struct heapblk) + TAG_SIZE + TAG_SIZE * 2;
        if( amount < _amblksiz )  amount = _amblksiz;
        n = ( amount + 0x0f ) >> 4;
        if( n == 0 )  n = 0x1000;                       /* 23-may-89 */
#if defined(__OS2__)
        rc = DosAllocSeg( n << 4, (PSEL)&seg, 0 );      /* 21-aug-91 */
        if( rc != 0 ) return( 0 );
#elif defined(__QNX__)
        seg = qnx_segment_alloc_flags( ((long)n) << 4, __qnx_alloc_flags );
        if( seg == -1 ) return( 0 );
#elif defined(__WINDOWS_286__)
        {
            HANDLE hmem;
            LPSTR p;

            hmem = GlobalAlloc( __win_alloc_flags, ((long)n) << 4 );
            if( hmem == NULL ) return( 0 );
            p = GlobalLock( hmem );
            if( p == NULL ) {
                GlobalFree( hmem );
                return( 0 );
            }
#if 0       /* code generator can't handle this */
            if( FP_OFF( p ) != 0 ) {    /* in case, Microsoft changes Windows */
                GlobalUnlock( hmem );   /* in post 3.1 versions */
                GlobalFree( hmem );
                return( 0 );
            }
#endif
            seg = FP_SEG( p );
        }
#else
        rc = TinyAllocBlock( n );
        if( TINY_ERROR( rc ) ) {
            return( 0 );
        }
        seg = TINY_INFO( rc );
#endif
        p = (struct heapstart _WCFAR *)MK_FP( seg, 0 );
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
        p->first.len  = p->h.heaplen - sizeof(struct heapblk) - TAG_SIZE*2;
        p->h.largest_blk = p->first.len;
        p->first.prev = offsetof( struct heapblk, freehead );
        p->first.next = offsetof( struct heapblk, freehead );
        last_tag = MK_FP( seg, p->h.heaplen - TAG_SIZE*2 );
        *last_tag = END_TAG;
        last_tag[1] = 0;        /* link to next piece of near heap */
        return( seg );          /* return allocated segment */
    }
