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

#if defined(__SMALL_DATA__)
_WCRTLINK int _heapwalk( struct _heapinfo *entry )
    {
        return( _nheapwalk( entry ) );
    }
#endif

int __NHeapWalk( struct _heapinfo *entry, mheapptr heapbeg )
    {
        frlptr p;
        frlptr q;

        if( heapbeg == NULL ) {
            return( _HEAPEMPTY );
        }
        p = (frlptr)(entry->_pentry);
        if( p == NULL ) {
            p = (frlptr)(heapbeg + 1);
        } else {    /* advance to next entry */
            for( heapbeg = __nheapbeg;; heapbeg = heapbeg->next ) {
                if( heapbeg->next == NULL ) break;
                if( (PTR)heapbeg <= (PTR)p &&
                    (PTR)heapbeg+heapbeg->len > (PTR)p ) break;
            }
            q = (frlptr)((PTR)p + (p->len & ~1));
            if( q <= p ) {
                return( _HEAPBADNODE );
            }
            p = q;
        }
        for( ;; ) {
            if( p->len == END_TAG ) {
                if( heapbeg->next == NULL ) {
                    entry->_useflag = _USEDENTRY;
                    entry->_size    = 0;
                    entry->_pentry  = NULL;
                    return( _HEAPEND );
                } else { // We advance to next miniheapblk
                    heapbeg = heapbeg->next;
                    p = (frlptr)(heapbeg + 1);
                }
            } else {
                break;
            }
        }
        entry->_pentry  = p;
        entry->_useflag = _FREEENTRY;
        entry->_size    = p->len & ~1;
        if( p->len & 1 ) {
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

