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


#include "variety.h"
#include <dos.h>
#include <string.h>
#if defined(__WINDOWS__)
  #include <windows.h>
#endif
#include <malloc.h>
#include "heap.h"

extern  long _dosalloc(unsigned);
#pragma aux     _dosalloc = 0xb4 0x48   /* mov ah,48h */        \
                            0xcd 0x21   /* int 21h    */        \
                            0x19 0xd2   /* sbb dx,dx  */        \
                        parm caller [bx] value [dx ax] modify [dx ax];

extern  void    _dosfree(void _WCHUGE *);
#pragma aux     _dosfree  = 0xb4 0x49   /* mov ah,49h */        \
                            0xcd 0x21   /* int 21h    */        \
                        parm caller [ax es] modify [es];

#define HUGE_NULL       ((void _WCHUGE *)NULL)

static int only_one_bit( size_t x )
{
    if( x == 0 ) {
        return 0;
    }
    /* turns off lowest 1 bit and leaves all other bits on */
    if(( x & ( x - 1 )) != 0 ) {
        return 0;
    }
    /* only one bit was on! */
    return 1;
}

_WCRTLINK void _WCHUGE *halloc( long n, unsigned size )
    {
        unsigned long len;
#if defined(__WINDOWS__)
        HANDLE hmem;
        LPSTR p;

        len = (unsigned long)n * size;
        if( len == 0 ) return( HUGE_NULL );
        if( len > 65536 && ! only_one_bit( size ) ) return( HUGE_NULL );
        hmem = GlobalAlloc( GMEM_MOVEABLE|GMEM_ZEROINIT, len );
        if( hmem == NULL ) return( HUGE_NULL );
        p = GlobalLock( hmem );
        return( p );
#else
        long seg;
        unsigned int paras;
        char _WCHUGE *hp;

        len = (unsigned long)n * size;
        if( len == 0  || len >= 0x100000 ) return( HUGE_NULL );
        if( len > 65536 && ! only_one_bit( size ) ) return( HUGE_NULL );
        paras = (len + 15) >> 4;
        seg = _dosalloc( paras );
        if( seg < 0 ) return( HUGE_NULL );  /* allocation failed */
        hp = (char _WCHUGE *)MK_FP( (unsigned short)seg, 0 );
        for(;;) {
            size = 0x8000;
            if( paras < 0x0800 )  size = paras << 4;
            _fmemset( hp, 0, size );
            if( paras < 0x0800 )  break;
            hp = hp + size;
            paras -= 0x0800;
        }
        return( (void _WCHUGE *)MK_FP( (unsigned short)seg, 0 ) );
#endif
    }

#if defined(__WINDOWS__)
  #pragma aux hfree modify [es]
#endif
_WCRTLINK void hfree( void _WCHUGE *ptr )
    {
#if defined(__WINDOWS__)
        __FreeSeg( FP_SEG( ptr ) );
#else
        _dosfree( ptr );
#endif
    }
