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
* Description:  Far heap reallocation routines.
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "heap.h"


extern void _WCNEAR *_mymemcpy( void _WCFAR *, void _WCFAR *, size_t );
#pragma aux _mymemcpy = \
        "push ds"       \
        "mov ds,dx"     \
        "shr cx,1"      \
        "rep movsw"     \
        "adc cx,cx"     \
        "rep movsb"     \
        "pop ds"        \
    parm caller [es di] [dx si] [cx] \
        value [si] modify exact [si di cx]

#if defined(__BIG_DATA__)

_WCRTLINK void *realloc( void *stg, size_t amount )
{
    return( _frealloc( stg, amount ) );
}

#endif


_WCRTLINK void _WCFAR *_frealloc( void _WCFAR *stg, size_t req_size )
{
    size_t  old_size;
    void    _WCFAR *p;

    if( stg == NULL ) {
        return( _fmalloc( req_size ) );
    }
    if( req_size == 0 ) {
        _ffree( stg );
        return( NULL );
    }
    old_size = _fmsize( stg );
    if( FP_SEG( stg ) == _DGroup() ) {
        p = stg;
        if( _nexpand( (void _WCNEAR *)stg, req_size ) == NULL ) {
            p = NULL;
        }
    } else {
        p = _fexpand( stg, req_size );
    }
    if( p == NULL ) {       /* couldn't be expanded inline */
        p = _fmalloc( req_size );
        if( p != NULL ) {
            _mymemcpy( p, stg, old_size );
            _ffree( stg );
        } else {
            if( FP_SEG( stg ) == _DGroup() ) {
                _nexpand( (void _WCNEAR *)FP_OFF( stg ), old_size );
            } else {
                _fexpand( stg, old_size );
            }
        }
    }
    return( p );
}
