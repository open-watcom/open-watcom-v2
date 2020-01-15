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
* Description:  Huge allocation/deallocation routines for OS/2
*               (16-bit code only)
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <malloc.h>
#include <dos.h>
#include "seterrno.h"
#define INCL_DOSMEMMGR
#include <wos2.h>
#include "heap.h"


void _os2zero64k( unsigned ax, unsigned cx, unsigned es, unsigned di );
#pragma aux _os2zero64k = \
        "rep stosw"     \
    __parm __caller [__ax] [__cx] [__es] [__di] \
    __value         \
    __modify        [__cx __di]

void _os2zero_rest( unsigned ax, unsigned cx, unsigned es, unsigned di );
#pragma aux _os2zero_rest = \
        "rep stosb"     \
    __parm __caller [__ax] [__cx] [__es] [__di] \
    __value         \
    __modify        [__cx __di]

static int only_one_bit( size_t x )
{
    if( x == 0 ) {
        return( 0 );
    }
    /* turns off lowest 1 bit and leaves all other bits on */
    if( (x & ( x - 1 )) != 0 ) {
        return 0;
    }
    /* only one bit was on! */
    return( 1 );
}

_WCRTLINK void_hptr halloc( long n, unsigned size )
{
    unsigned long   amount;
    USHORT          error, tseg;
    SEL             seg;
    USHORT          number_segments, remaining_bytes;
    USHORT          increment;

    amount = (unsigned long)n * size;
    if( amount == 0 )
        return( NULL );
    if( OVERFLOW_64K( (unsigned long)n ) && !only_one_bit( size ) )
        return( NULL );
    error = DosGetHugeShift( &increment );
    if( error ) {
        __set_errno_dos( error );
        return( NULL );
    }
    number_segments = amount >> 16;
    remaining_bytes = amount & 0xffff;
    error = DosAllocHuge( number_segments, remaining_bytes, &seg, 0, 0 );
    if( error ) {
        __set_errno_dos( error );
        return( NULL );     /* allocation failed */
    }
    tseg = seg;
    increment = 1 << increment;
    while( number_segments-- ) {
        _os2zero64k( 0, 0x8000, tseg, 0 );
        tseg += increment;
    }
    if( remaining_bytes != 0 ) {
        _os2zero_rest( 0, remaining_bytes, tseg, 0 );
    }
    return( (void_hptr)((unsigned long)seg << 16) );
}

_WCRTLINK void hfree( void_hptr cstg )
{
    __FreeSeg( _FP_SEG( cstg ) );
}
