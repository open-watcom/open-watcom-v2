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
* Description:  Implementation of memcpy_s() - bounds-checking memcpy().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include "widechar.h"
#include <string.h>
#include  <wchar.h>
#include "xstring.h"

_WCRTLINK errno_t __F_NAME(memcpy_s,wmemcpy_s)( VOID_WC_TYPE * __restrict s1,
                                                rsize_t s1max,
                                          const VOID_WC_TYPE * __restrict s2,
                                                rsize_t n )
/*****************************************************************************/
{
    errno_t     rc = -1;
    const char  *msg;

    // Verify runtime-constraints
    // s1 not NULL
    // s2 not NULL
    // s1max <= RSIZE_MAX
    // n     <= RSIZE_MAX
    // n     <= s1max
    // s1 and s2 no overlap
    if( __check_constraint_nullptr_msg( msg, s1 ) &&
        __check_constraint_nullptr_msg( msg, s2 ) &&
        __check_constraint_maxsize_msg( msg, s1max ) &&
        __check_constraint_maxsize_msg( msg, n ) &&
        __check_constraint_a_gt_b_msg( msg, n, s1max ) &&
        __check_constraint_overlap_msg( msg, s1, s1max, s2, n ) ) {

        // now it's safe to use memcpy
         __F_NAME(memcpy,wmemcpy)( s1, s2, n );
         rc = 0;
    } else {
        // Runtime-constraints violated, zero out destination array
        if( (s1 != NULL) && __lte_rsizmax( s1max ) ) {
            __F_NAME(memset,wmemset)( s1, 0, s1max );
        }
        // Now call the handler
        __rtct_fail( __func__, msg, NULL );
    }

    return( rc );
}
