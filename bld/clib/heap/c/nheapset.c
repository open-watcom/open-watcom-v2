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

#if defined(_M_IX86)
    #pragma intrinsic(_fmemset)
#endif

_WCRTLINK int _nheapset( unsigned int fill )
{
    mheapptr mhp;
    frlptr  curr;
    int test_heap;

    test_heap = _heapchk();
    if( test_heap != _HEAPOK ) {
        return( test_heap );
    }
    fill |= fill << 8;
    _AccessNHeap();

    for( mhp = __nheapbeg; mhp != NULL; mhp = mhp->next ) {
        curr = mhp->freehead.next;
        for( ;; ) {
            if( curr == (frlptr) &mhp->freehead ) break;
#if defined(_M_IX86)
            _fmemset( (void _WCFAR *)(curr + 1), fill, curr->len - sizeof(frl) );
#else
            memset( (void *)(curr + 1), fill, curr->len - sizeof(frl) );
#endif
            curr = curr->next;
        }
    }
    _ReleaseNHeap();
    return( _HEAPOK );
}
