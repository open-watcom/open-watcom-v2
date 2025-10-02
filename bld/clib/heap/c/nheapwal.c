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
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"


#define FIRST_FRL(h)    ((freelist_nptr)(h + 1))

#if defined(__SMALL_DATA__)
_WCRTLINK int _heapwalk( struct _heapinfo *entry )
{
    return( _nheapwalk( entry ) );
}
#endif

int __NHeapWalk( struct _heapinfo *entry, heapblk_nptr heap )
{
    freelist_nptr frl;
    freelist_nptr frl_next;

    if( heap == NULL ) {
        return( _HEAPEMPTY );
    }
    frl = FAR2NEAR( void, entry->_pentry );
    if( frl == NULL ) {
        frl = FIRST_FRL( heap );
    } else {    /* advance to next entry */
        for( heap = __nheapbeg; heap->next.nptr != NULL; heap = heap->next.nptr ) {
            if( IS_IN_HEAP( frl, heap ) ) {
                break;
            }
        }
        frl_next = (freelist_nptr)NEXT_BLK_A( frl );
        if( frl_next <= frl ) {
            return( _HEAPBADNODE );
        }
        frl = frl_next;
    }
    for( ; IS_BLK_END( frl ); ) {
        // We advance to next miniheapblk
        heap = heap->next.nptr;
        if( heap == NULL ) {
            entry->_useflag = _USEDENTRY;
            entry->_size    = 0;
            entry->_pentry  = NULL;
            return( _HEAPEND );
        }
        frl = FIRST_FRL( heap );
    }
    entry->_pentry  = frl;
    entry->_useflag = _FREEENTRY;
    entry->_size    = GET_BLK_SIZE( frl );
    if( IS_BLK_INUSE( frl ) ) {
        entry->_useflag = _USEDENTRY;
    }
    return( _HEAPOK );
}

_WCRTLINK int _nheapwalk( struct _heapinfo *entry )
{
    int     heap_status;

    _AccessNHeap();
    heap_status = __NHeapWalk( entry, __nheapbeg );
    _ReleaseNHeap();
    return( heap_status );
}
