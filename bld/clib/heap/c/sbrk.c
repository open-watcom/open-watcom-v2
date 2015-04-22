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
#include <stdlib.h>
#include <errno.h>
#if defined(__OS2__)
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined(__QNX__)
    #include <sys/types.h>
    #include <sys/seginfo.h>
    #include <unistd.h>
    #if defined(__386__)
        extern int _brk(void *);
    #endif
#elif defined(__WINDOWS__)
    #include <windows.h>
#endif
#include "heapacc.h"
#include "heap.h"
#include "rtdata.h"


#if !defined(__OS2__) && !defined(__QNX__)

extern  unsigned short SS_Reg( void );
#pragma aux SS_Reg              = \
        0x8c 0xd0               /* mov ax,ss */ \
        parm caller \
        value                   [ax];

extern  int SetBlock( unsigned short, size_t );
#pragma aux SetBlock            = \
        0xb4 0x4a               /* mov ah,04ah */ \
        0xcd 0x21               /* int 021h */ \
        0x1b 0xc0               /* sbb ax,ax */ \
        parm caller             [es] [bx] \
        value                   [ax];

#endif

#if (defined(__QNX__) && defined(__386__))
_WCRTLINK void _WCNEAR *sbrk( int increment ) {
    return( __brk( _curbrk + increment ) );
}

_WCRTLINK int brk( void *endds ) {
    return( __brk( (unsigned) endds ) == (void *)-1 ? -1 : 0 );
}
#else
_WCRTLINK void _WCNEAR *sbrk( int increment ) {
    #if defined(__WINDOWS_286__)
        HANDLE h;

        if( increment > 0 ) {
            h = LocalAlloc( LMEM_FIXED, increment );
            if( h == NULL ) {
                errno = ENOMEM;
                h = (HANDLE)(-1);
            }
        } else {
            errno = EINVAL;
            h = (HANDLE)(-1);
        }
        return( (void _WCNEAR *) h );
    #else
        return( __brk( _curbrk + increment ) );
    #endif
}
#endif

#if ! defined(__WINDOWS_286__)

_WCRTLINK void _WCNEAR *__brk( unsigned brk_value )
    {
        unsigned old_brk_value;
        unsigned seg_size;
        unsigned segment;

        if( brk_value < _STACKTOP ) {
            errno = ENOMEM;
            return( (void _WCNEAR *) -1 );
        }
        seg_size = ( brk_value + 0x0f ) >> 4;
        if( seg_size == 0 ) {
            seg_size = 0x1000;
        }
        /* try setting the block of memory */
        _AccessNHeap();
        segment = _DGroup();
#if defined(__OS2__)
        if( DosReallocSeg( seg_size << 4, segment ) != 0 ) {
#elif defined(__QNX__) && defined(__386__)
        if( _brk((void *)(seg_size << 4)) == -1 ) {
#elif defined(__QNX__)
        if( qnx_segment_realloc( segment,((unsigned long)seg_size) << 4) == -1){
#else
        if( _RWD_osmode == DOS_MODE ) {                     /* 24-apr-91 */
            seg_size += SS_Reg() - _RWD_psp;/* add in code size (in paragraphs) */
            segment = _RWD_psp;
        }
        if( SetBlock( segment, seg_size ) != 0 ) {
#endif
            errno = ENOMEM;
            _ReleaseNHeap();
            return( (void _WCNEAR *) -1 );
        }

        old_brk_value = _curbrk;        /* return old value of _curbrk */
        _curbrk = brk_value;            /* set new break value */

        _ReleaseNHeap();
        return( (void _WCNEAR *) old_brk_value );
    }

#if defined(__QNX__)
/*
 * This is used by the QNX/386 shared memory functions to tell the
 * memory manager that the break value has changed. That way things don't
 * get screwed up next time we grow the data segment.
 */
void __setcbrk( unsigned offset )
{
    _curbrk = offset;
}
#endif
#endif
