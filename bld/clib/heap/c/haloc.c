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
* Description:  Huge allocation/deallocation routines for DOS and Windows
*               (16-bit code only)
*
****************************************************************************/


#include "variety.h"
#include <dos.h>
#include <string.h>
#if defined(__WINDOWS__)
  #include <windows.h>
#endif
#include <malloc.h>
#include "tinyio.h"
#include "heap.h"


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

_WCRTLINK void_hptr halloc( long numb, unsigned size )
{
    unsigned long   amount;
#if defined(__WINDOWS__)
    HANDLE          hmem;
    LPSTR           cstg;

    amount = (unsigned long)numb * size;
    if( amount == 0 )
        return( NULL );
    if( OVERFLOW_64K( amount ) && !only_one_bit( size ) )
        return( NULL );
    hmem = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, amount );
    if( hmem == NULL )
        return( NULL );
    cstg = GlobalLock( hmem );
    return( cstg );
#else
    tiny_ret_t      rc;
    unsigned int    num_of_paras;
    void_hptr       hp;

    amount = (unsigned long)numb * size;
    if( amount == 0  || amount >= 0x100000 )
        return( NULL );
    if( OVERFLOW_64K( amount ) && !only_one_bit( size ) )
        return( NULL );
    num_of_paras = __ROUND_UP_SIZE_TO_PARA( amount );
    rc = TinyAllocBlock( num_of_paras );
    if( TINY_ERROR( rc ) )
        return( NULL );  /* allocation failed */
    hp = (void_hptr)_MK_FP( TINY_INFO( rc ), 0 );
    for( ;; ) {
        size = 0x8000;
        if( num_of_paras < 0x0800 )
            size = num_of_paras << 4;
        _fmemset( hp, 0, size );
        if( num_of_paras < 0x0800 )
            break;
        hp = (char _WCHUGE *)hp + size;
        num_of_paras -= 0x0800;
    }
    return( (void_hptr)_MK_FP( (unsigned short)rc, 0 ) );
#endif
}

_WCRTLINK void hfree( void_hptr cstg )
{
#if defined(__WINDOWS__)
    __FreeSeg( _FP_SEG( cstg ) );
#else
    _TinyFreeBlock( _FP_SEG( cstg ) );
#endif
}
