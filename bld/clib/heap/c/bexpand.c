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
* Description:  Expand memory block
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"

_WCRTLINK void __based( void ) *_bexpand( __segment seg, void __based( void ) *mem, size_t req_size )
{
    struct {
        unsigned expanded : 1;
    }       flags;
    int     retval;
    size_t  growth_size;

    flags.expanded = 0;
    _AccessFHeap();
    for( ;; ) {
        retval = __HeapManager_expand( seg, (unsigned)mem, req_size, &growth_size );
        if( retval == __HM_SUCCESS ) {
            _ReleaseFHeap();
            return( mem );
        }
        if( retval == __HM_FAIL )
            break;
        if( retval == __HM_TRYGROW ) {
            if( flags.expanded )
                break;
            if( __GrowSeg( seg, growth_size ) == 0 )
                break;
            flags.expanded = 1;
        }
    }
    _ReleaseFHeap();
    return( _NULLOFF );
}
