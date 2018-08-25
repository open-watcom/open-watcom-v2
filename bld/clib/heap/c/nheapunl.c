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
* Description:  internal function for unlink near heap.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include "heap.h"


void __UnlinkNHeap( heapblk_nptr heap, heapblk_nptr prev_heap, heapblk_nptr next_heap )
{
    if( __nheapbeg == heap )
        __nheapbeg = next_heap;
    // Update rovers
    if( __MiniHeapRover == heap ) {
        __MiniHeapRover = prev_heap;
        if( __MiniHeapRover == NULL ) {
            __MiniHeapRover = __nheapbeg;
            __LargestSizeB4MiniHeapRover = 0;
        }
    }
    if( __MiniHeapFreeRover == heap ) {
        __MiniHeapFreeRover = NULL;
    }
    if( prev_heap != NULL ) {
        prev_heap->next.nptr = next_heap;
    }
    if( next_heap != NULL ) {
        next_heap->prev.nptr = prev_heap;
    }
}
