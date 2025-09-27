/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of sbrk() for OS/2.
*
****************************************************************************/


#include "dll.h"
#include "variety.h"
#include "seterrno.h"
#include <stddef.h>
#include <stdlib.h>
#define INCL_DOSMEMMGR
#include <wos2.h>
#include "roundmac.h"
#include "rtstack.h"
#include "rtdata.h"
#include "heap.h"
#include "heapacc.h"
#include "thread.h"


#ifdef _M_I86

_WCRTLINK void_nptr __brk( unsigned brk_value )
{
    unsigned    old_brk_value;
    unsigned    num_of_paras;
    __segment   segment;

    if( brk_value < _STACKTOP ) {
        lib_set_errno( ENOMEM );
        return( (void_nptr)-1 );
    }
    num_of_paras = __ROUND_UP_SIZE_TO_PARA( brk_value );
    if( num_of_paras == 0 ) {
        num_of_paras = PARAS_IN_64K;
    }
    /* try setting the block of memory */
    _AccessNHeap();
    segment = _DGroup();
    if( DosReallocSeg( num_of_paras << 4, segment ) != 0 ) {
        lib_set_errno( ENOMEM );
        _ReleaseNHeap();
        return( (void_nptr)-1 );
    }

    old_brk_value = _curbrk;        /* return old value of _curbrk */
    _curbrk = brk_value;            /* set new break value */

    _ReleaseNHeap();
    return( (void_nptr)old_brk_value );
}

_WCRTLINK void_nptr sbrk( int increment )
{
    return( __brk( _curbrk + increment ) );
}

#else

_WCRTLINK void_nptr sbrk( int increment )
{
    if( increment > 0 ) {
        PBYTE       p;

        increment = __ROUND_UP_SIZE_4K( increment );
        if( !DosAllocMem( (PPVOID)&p, increment, PAG_COMMIT|PAG_READ|PAG_WRITE ) ) {
            return( p );
        }
        lib_set_errno( ENOMEM );
        return( (void_nptr)-1 );
    }
    return( (void_nptr)lib_set_EINVAL() );
}

#endif
