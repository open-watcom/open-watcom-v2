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
* Description:  Re-allocate memory block
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <malloc.h>
#include <string.h>
#include "heap.h"


extern void _mymemcpy( void_fptr, void_fptr, size_t );
#if defined(__SMALL_DATA__) || defined(__WINDOWS__)
#pragma aux _mymemcpy = \
        "push ds"       \
        "mov ds,dx"     \
        memcpy_i86      \
        "pop ds"        \
    __parm __caller     [__es __di] [__dx __si] [__cx] \
    __value             \
    __modify __exact    [__si __di __cx]
#else
#pragma aux _mymemcpy = \
        memcpy_i86      \
    __parm __caller     [__es __di] [__ds __si] [__cx] \
    __value             \
    __modify __exact    [__si __di __cx]
#endif

_WCRTLINK void_bptr _brealloc( __segment seg, void_bptr cstg_old, size_t size )
{
    void_bptr   cstg_new;
    size_t      old_size;

    if( cstg_old == _NULLOFF )
        return( _bmalloc( seg, size ) );
    if( size == 0 ) {
        _bfree( seg, cstg_old );
        return( _NULLOFF );
    }
    old_size = _bmsize( seg, cstg_old );
    cstg_new = _bexpand( seg, cstg_old, size );
    if( cstg_new == _NULLOFF ) {                /* if it couldn't be expanded */
        cstg_new = _bmalloc( seg, size );       /* - allocate new block */
        if( cstg_new != _NULLOFF ) {            /* - if we got one */
            _mymemcpy( seg :> cstg_new, seg :> cstg_old, old_size );
            _bfree( seg, cstg_old );
        } else {
            _bexpand( seg, cstg_old, old_size );
        }
    }
    return( cstg_new );
}
