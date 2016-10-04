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
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "heapacc.h"
#include "heap.h"
#include "rtdata.h"
#include "linuxsys.h"


void _WCNEAR *__brk( unsigned brk_value )
{
    unsigned old_brk_value;
    unsigned sys_brk_value;

    /* try setting the block of memory */
    _AccessNHeap();

    sys_brk_value = sys_brk( brk_value );
    if( sys_brk_value == -1 ) {
        _RWD_errno = ENOMEM;
        _ReleaseNHeap();
        return( (void _WCNEAR *)-1 );
    }
    if( _curbrk == 0 ) {
        _curbrk = sys_brk_value;
        brk_value = sys_brk_value;
    }

    old_brk_value = _curbrk;        /* return old value of _curbrk */
    _curbrk = brk_value;            /* set new break value */

    _ReleaseNHeap();
    return( (void _WCNEAR *)old_brk_value );
}

_WCRTLINK void _WCNEAR *sbrk( int increment )
{
    return( __brk( _curbrk + increment ) );
}

_WCRTLINK int brk( void *endds )
{
    return( __brk( (unsigned)endds ) == (void *)-1 ? -1 : 0 );
}
