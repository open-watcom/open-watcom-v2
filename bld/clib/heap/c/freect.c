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

/* return the number of times that _nmalloc can be called to allocate
   and item "size" bytes from the near heap. */

_WCRTLINK unsigned int _freect( size_t size )
{
    unsigned int    count;
    size_t          memsize;
    size_t          size_of_chunk;
    frlptr          pnext;
    mheapptr        mhp;

    count = 0;
    size_of_chunk = (size + TAG_SIZE + ROUND_SIZE) & ~ROUND_SIZE;
    if( size_of_chunk < size )
        return( 0 );
    if( size_of_chunk < FRL_SIZE ) {
        size_of_chunk = FRL_SIZE;
    }
    _AccessNHeap();
    for( mhp = __nheapbeg; mhp != NULL; mhp = mhp->next ) {
        pnext = mhp->freehead.next;
        while( pnext != (frlptr)&mhp->freehead ) {
            memsize = pnext->len;
            count += memsize / size_of_chunk;
            pnext = pnext->next;
        }
    }
    _ReleaseNHeap();
    return( count );
}
