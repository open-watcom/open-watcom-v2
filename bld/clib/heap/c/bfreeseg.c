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
* Description:  Free memory block
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


_WCRTLINK int _bfreeseg( __segment seg )
{
    __segment       next_seg;
    __segment       prev_seg;

    _AccessFHeap();
    /* unlink from heap list */
    prev_seg = BHEAP( seg )->prev.segm;
    next_seg = BHEAP( seg )->next.segm;
    if( next_seg != _NULLSEG ) {
        BHEAP( next_seg )->prev.segm = prev_seg;
    }
    if( prev_seg == _NULLSEG ) {
        __bheapbeg = next_seg;
    } else {
        BHEAP( prev_seg )->next.segm = next_seg;
    }
    _ReleaseFHeap();
    return( __FreeSeg( seg ) );
}
