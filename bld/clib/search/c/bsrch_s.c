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
* Description:  Implementation of bsearch_s() - bounds-checking bsearch().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#undef __INLINE_FUNCTIONS__
#include <stddef.h>
#include <stdlib.h>
#include "extfunc.h"

typedef int bcomp(const void *, const void *, void *);
#ifdef _M_IX86
    #pragma aux (__outside_CLIB) bcomp;
#endif

_WCRTLINK void * bsearch_s( const void * key, const void * base,
                            rsize_t nmemb, rsize_t size,
            int (*compar)(const void *x, const void *y, void *context),
                            void *context )
/***********************************************************************/
{
    char    *low;
    char    *high;
    char    *mid;
    int     cond;
    bcomp   *comp = compar;

    /* runtime-constraints */
    // nmemb <= RSIZE_MAX
    // size  <= RSIZE_MAX
    // if nmemb > 0 then key, base, compar not NULL
    if( __check_constraint_maxsize( nmemb ) &&
        __check_constraint_maxsize( size ) &&
        ( ( nmemb == 0 ) || __check_constraint_nullptr( key ) &&
                          __check_constraint_nullptr( base ) &&
                          __check_constraint_nullptr( compar )) ) {

        if( nmemb == 0 ) {                      /* empty array - nothing to do */
            return( NULL );
        }
        low = (char *)base;
        high = low + ( nmemb - 1 ) * size;
        while( low < high ) {
            mid = low + ( ( high - low ) / size / 2 ) * size;
            cond = (*comp)( key, mid, context );
            if( cond == 0 )
                return( mid );
            if( cond < 0 ) {    /* key < mid */
                high = mid;
            } else {            /* key > mid */
                low = mid + size;
            }
        }
        if( low == high ) {
           return( (*comp)( key, low, context ) ? NULL : low );
        }
    }
    return( NULL );
}
