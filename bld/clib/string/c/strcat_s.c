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
* Description:  Implementation of strcat_s() - bounds-checking strcat().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include "widechar.h"
#include <string.h>
#include <wchar.h>


_WCRTLINK errno_t __F_NAME(strcat_s,wcscat_s)( CHAR_TYPE * __restrict s1,
                          rsize_t s1max, const CHAR_TYPE * __restrict s2 )
/************************************************************************/
{
    errno_t     rc = -1;
    const char  *msg;

    // strnlen_s is safe to use as it has no rt constraints
    rsize_t     m = s1max - __F_NAME(strnlen_s,wcsnlen_s)( s1, s1max );

    // Verify runtime-constraints
    // s1 not NULL
    // s2 not NULL
    // s1max <= RSIZE_MAX
    // s1max != 0
    // m     != 0
    // m     >  strnlen_s( s2, m )
    // s1 and s2 no overlap
    if( __check_constraint_nullptr_msg( msg, s1 ) &&
        __check_constraint_nullptr_msg( msg, s2 ) &&
        __check_constraint_maxsize_msg( msg, s1max ) &&
        __check_constraint_zero_msg( msg, s1max ) &&
        __check_constraint_zero_msg( msg, m ) &&
        __check_constraint_a_gt_b_msg( msg,
            __F_NAME(strnlen_s,wcsnlen_s)( s2, m ), m - 1 ) &&
        __check_constraint_overlap_msg( msg, s1, s1max, s2,
                           __F_NAME(strnlen_s,wcsnlen_s)( s2, s1max ) ) ) {

         while( *s1 != NULLCHAR ) ++s1;       // find end of field
         while( *s1++ = *s2++ );           // append source field

         rc = 0;
    } else {
        // Runtime-constraints found, store zero in receiving field
        if( (s1 != NULL) && (s1max > 0) && __lte_rsizmax( s1max ) ) {
            s1[0] = NULLCHAR;
        }
        // Now call the handler
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}
