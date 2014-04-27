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
#include "seterrno.h"
#define INCL_DOSMEMMGR
#include <wos2.h>
#include <stdlib.h>
#include "heap.h"


void _os2zero64k( unsigned ax, unsigned cx, unsigned es, unsigned di );
#pragma aux _os2zero64k = 0xf3 0xab /* rep stosw */     \
        parm caller [ ax ] [ cx ] [ es ] [ di ]                     \
        modify [ cx di ];

void _os2zero_rest( unsigned ax, unsigned cx, unsigned es, unsigned di );
#pragma aux _os2zero_rest = 0xf3 0xaa /* rep stosb */   \
        parm caller [ ax ] [ cx ] [ es ] [ di ]                     \
        modify [ cx di ];


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

_WCRTLINK void _WCHUGE * halloc( unsigned long n, unsigned size )
{
    unsigned long len;
    USHORT      error, tseg;
    SEL         seg;
    USHORT      number_segments, remaining_bytes;
    USHORT      increment;

    len = n * size;
    if( len == 0 ) return( (void _WCHUGE *)0 );
    if( n > 65536 && ! only_one_bit( size ) ) return( (void _WCHUGE *)0 );
    error = DosGetHugeShift( &increment );
    if( error ) {
        __set_errno_dos( error );
        return( ( void _WCHUGE *)0 );
    }
    number_segments = len >> 16;
    remaining_bytes = len & 0xffff;
    error = DosAllocHuge( number_segments, remaining_bytes, &seg, 0, 0 );
    if( error ) {
        __set_errno_dos( error );
        return( (void _WCHUGE *)0 );  /* allocation failed */
    }
    tseg = seg;
    increment = 1 << increment;
    while( number_segments-- ) {
        _os2zero64k( 0, 0x8000, tseg, 0 );
        tseg += increment;
    }
    if( remaining_bytes != 0 ) {                        /* 30-apr-91 */
        _os2zero_rest( 0, remaining_bytes, tseg, 0 );
    }
    return( (void _WCHUGE *)((unsigned long )seg << 16) );
}

#pragma aux hfree modify [es]
_WCRTLINK void hfree( void _WCHUGE *ptr )
    {
        __FreeSeg( FP_SEG( ptr ) );
    }
