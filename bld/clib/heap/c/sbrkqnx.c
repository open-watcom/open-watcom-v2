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
* Description:  Implementation of sbrk() for QNX.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/seginfo.h>
#include <unistd.h>
#include "rtstack.h"
#include "rterrno.h"
#include "rtdata.h"
#include "heapacc.h"
#include "heap.h"
#include "thread.h"
#ifndef _M_I86
    extern int  _brk( void * );
#endif


_WCRTLINK void_nptr __brk( unsigned brk_value )
{
    unsigned        old_brk_value;
    unsigned long   seg_size;

    if( brk_value < _STACKTOP ) {
        _RWD_errno = ENOMEM;
        return( (void_nptr)-1 );
    }
    seg_size = __ROUND_UP_SIZE_PARA( brk_value );

    /* try setting the block of memory */
    _AccessNHeap();

#ifdef _M_I86
    if( qnx_segment_realloc( _DGroup(), seg_size ) == -1 ) {
#else
    if( _brk( (void *)seg_size ) == -1 ) {
#endif
        _RWD_errno = ENOMEM;
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

#ifdef __386__

_WCRTLINK int brk( void_nptr endds )
{
    return( __brk( (unsigned)endds ) == (void_nptr)-1 ? -1 : 0 );
}

#endif

/*
 * This is used by the QNX/386 shared memory functions to tell the
 * memory manager that the break value has changed. That way things don't
 * get screwed up next time we grow the data segment.
 */
void __setcbrk( unsigned offset )
{
    _curbrk = offset;
}
