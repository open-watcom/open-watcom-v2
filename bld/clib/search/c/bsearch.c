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
#undef __INLINE_FUNCTIONS__
#include <stddef.h>
#include <stdlib.h>
#include "extfunc.h"

/*
    The old bsearch could SIGSEGV if the compare is done on elements
    which are not within the given segment.  The error occurs if the search
    key is larger than the 'high' element (or lower than the 'low' member)
    and 'low','mid' and 'high' are all equal (either from the start or
    after several iterations).
*/

typedef int bcomp();
#if defined(_M_IX86)
    #pragma aux (__outside_CLIB) bcomp;
#endif

_WCRTLINK void * bsearch( const void * key,
                const void * base,
                size_t nmemb,
                size_t size,
                int (*cmp)() )
    {
        char *low;
        char *high;
        char *mid;
        int cond;
        bcomp *compar = cmp;

        if( nmemb == 0 ) {
            return( NULL );
        }
        low = (char *) base;
        high = low + (nmemb-1) * size;
// JBS  while( low <= high ) {
        while( low < high ) {
            mid = low + ( (high-low)/size/2 ) * size;
            cond = (*compar)( key, mid );
            if( cond == 0 ) return( mid );
            if( cond < 0 ) {    /* key < mid */
// JBS          high = mid - size;
                high = mid;
            } else {            /* key > mid */
                low = mid + size;
            }
        }
        if (low == high) return( (*compar)(key, low) ? NULL : low );
        return( NULL );
    }
