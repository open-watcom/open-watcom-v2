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


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "heapacc.h"
#include "heap.h"

_WCRTLINK void __based(void) *_bmalloc( __segment seg, size_t amt )
{
    void __based(void)  *ptr;

    if( amt == 0 ) return( _NULLOFF );
    if( seg == _DGroup() ) {
        ptr = (void __based(void) *) _nmalloc( amt );
        return( ( ptr == NULL ) ? _NULLOFF : ptr );
    }
    _AccessFHeap();
    for(;;) {
        ptr = (void __based(void) *)__MemAllocator( amt, seg, 0 );
        if( ptr != 0 ) break;
        ptr = _NULLOFF;
        if( __GrowSeg( seg, amt ) == 0 ) break;
    }
    _ReleaseFHeap();
    return( ptr );
}
