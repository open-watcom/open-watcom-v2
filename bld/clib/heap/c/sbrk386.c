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
* Description:  Implementation of sbrk() for DOS, OS/2 and Windows.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <dos.h>
#ifdef __WINDOWS_386__
 #include "tinyio.h"
#else
 #include "extender.h"
 #if defined(__OS2__)
  #include <wos2.h>
 #else
  #include "tinyio.h"
 #endif
#endif

extern  unsigned                _curbrk;
extern  unsigned                _STACKTOP;

#ifdef __WINDOWS_386__
 extern void * __pascal DPMIAlloc( unsigned long );
#else

extern  int                     SetBlock( unsigned short selector, int size );
#pragma aux SetBlock            = \
        "push   es"             \
        "mov    es,ax"          \
        "mov    ah,04ah"        \
        "int    021h"           \
        "rcl    eax,1"          \
        "ror    eax,1"          \
        "pop    es"             \
        parm caller             [ax] [ebx] \
        modify                  [ebx] \
        value                   [eax];

extern  int                     SegInfo( unsigned short selector );
#pragma aux SegInfo             = \
        "mov    ah,0edH"        \
        "int    021h"           \
        "shl    eax,31"         \
        "and    edi,0000FFFFh"  \
        "or     edi,eax"        \
        parm caller             [ebx] \
        value                   [edi] \
        modify exact            [eax ecx edx esi ebx edi];

extern  int                     SegmentLimit( void );
#pragma aux SegmentLimit        = \
        "xor    eax,eax"        \
        "mov    ax,ds"          \
        "lsl    eax,ax"         \
        "inc    eax"            \
        value                   [eax] \
        modify exact            [eax];
#endif

extern  unsigned short          GetDS( void );
#pragma aux GetDS               = \
        "mov    ax,ds"          \
        value                   [ax];

_WCRTLINK void _WCNEAR *__brk( unsigned brk_value );

_WCRTLINK void _WCNEAR *sbrk( int increment )
{
#if defined(__OS2__)
    if( increment > 0 ) {
        PBYTE       p;

        increment = ( increment + 0x0fff ) & ~0x0fff;
        if( !DosAllocMem( (PPVOID)&p, increment, PAG_COMMIT|PAG_READ|PAG_WRITE ) ) {
            return( p );
        }
        errno = ENOMEM;
    } else {
        errno = EINVAL;
    }
    return( (void _WCNEAR *) -1 );
#elif defined(__WINDOWS_386__)                          /* 26-may-93 */
    increment = ( increment + 0x0fff ) & ~0x0fff;
    return DPMIAlloc( increment );
#elif defined(__CALL21__)                               /* 10-aug-93 */
    increment = ( increment + 0x0fff ) & ~0x0fff;
    return (void *)TinyMemAlloc( increment );
#else
    if( _IsRationalZeroBase() || _IsCodeBuilder() ) {
        void _WCNEAR *p;

        if( increment > 0 ) {
            increment = ( increment + 0x0fff ) & ~0x0fff;
            if( _IsRational() ) {
                p = TinyDPMIAlloc( increment );
            } else {
                p = TinyCBAlloc( increment );
            }
            if( p == NULL ) {
                errno = ENOMEM;
                p = (void _WCNEAR *) -1;
            }
        } else {
            errno = EINVAL;
            p = (void _WCNEAR *) -1;
        }
        return( p );
    } else if( _IsPharLap() ) {             /* 17-sep-93 */
        _curbrk = SegmentLimit();
    }
    return( __brk( _curbrk + increment ) );
#endif
}


#if !defined(__OS2__) && !defined(__WINDOWS_386__)
_WCRTLINK void _WCNEAR *__brk( unsigned brk_value )
{
    unsigned        old_brk_value;
#if !defined(__CALL21__)
    unsigned        seg;
    int             parent;
#endif

    if( brk_value < _STACKTOP ) {
        errno = ENOMEM;
        return( (void _WCNEAR *) -1 );
    }
#if !defined(__CALL21__)
    if( _IsOS386() ) {
        seg = ( brk_value + 15U ) / 16U;
        if( seg == 0 ) seg = 0x0FFFFFFF;            /* 26-sep-89 */
        parent = SegInfo( GetDS() );
        if( parent < 0 ) {
            if( SetBlock( parent & 0xffff, seg ) < 0 ) {
                errno = ENOMEM;
                return( (void _WCNEAR *) -1 );
            }
        }
        if( SetBlock( GetDS(), seg ) < 0 ) {
            errno = ENOMEM;
            return( (void _WCNEAR *) -1 );
        }
    } else {        /* _IsPharLap() || IsRationalNonZeroBase() */
        seg = ( brk_value + 4095U ) / 4096U;
        if( seg == 0 ) seg = 0x000FFFFF;            /* 26-sep-89 */
        if( _IsRationalNonZeroBase() ) {
            // convert from 4k pages to paragraphs
            seg = seg * 256U;
        }
        if( SetBlock( GetDS(), seg ) < 0 ) {
            errno = ENOMEM;
            return( (void _WCNEAR *) -1 );
        }
    }
#endif
    old_brk_value = _curbrk;        /* return old value of _curbrk */
    _curbrk = brk_value;            /* set new break value */
    return( (void _WCNEAR *) old_brk_value );
}
#endif
