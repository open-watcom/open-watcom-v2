/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Near heap set routines.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__SMALL_DATA__)
_WCRTLINK int _heapset( unsigned int fill )
{
    return( _nheapset( fill ) );
}
#endif

_WCRTLINK int _nheapset( unsigned int fill )
{
    heapblk_nptr    heap;
    freelist_nptr   frl;
    int             heap_status;

    heap_status = _heapchk();
    if( heap_status != _HEAPOK ) {
        return( heap_status );
    }
    _AccessNHeap();

    for( heap = __nheapbeg; heap != NULL; heap = heap->next.nptr ) {
        for( frl = heap->freehead.next.nptr; frl != (freelist_nptr)&heap->freehead; frl = frl->next.nptr ) {
            memset( frl + 1, fill, frl->len - sizeof( freelist ) );
        }
    }
    _ReleaseNHeap();
    return( _HEAPOK );
}
