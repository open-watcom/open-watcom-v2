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
* Description:  Safer C Library Implemenation of wcrtomb_s(), _fwcrtomb_s().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include <mbstring.h>
#include <wchar.h>
#include "farfunc.h"
#include "rterrno.h"
#include "thread.h"


_WCRTLINK errno_t _NEARFAR(wcrtomb_s,_fwcrtomb_s)( size_t _FFAR * __restrict retval,
                                               char _FFAR * __restrict s,
                                               rsize_t smax,
                                               wchar_t wc,
                                               mbstate_t _FFAR * __restrict ps )
{
    int                 ret;
    int                 n;
    errno_t             rc = -1;
    const char          *msg = NULL;

    // Verify runtime-constraints
    // retval != NULL
    // ps     != NULL
    // if s == NULL then dstmax == 0
    // if s != NULL then dstmax > 0,
    if(__check_constraint_nullptr_msg( msg, retval ) &&
        __check_constraint_nullptr_msg( msg, ps )) {
        if(s == NULL) {                /* reset state */
            // ensure smax == 0
            if(__check_constraint_a_gt_b_msg( msg, smax, 0 )) {
                 rc = 0;
            }
        } else {
            if(wc & 0xff00) n = 2;
            else            n = 1;
            if(__check_constraint_a_gt_b_msg( msg, n, smax ) &&
                __check_constraint_maxsize_msg( msg, smax )) {
                /*** Check for a valid wide character ***/
                ret = _NEARFAR(wctomb,_fwctomb)( s, wc );
                if( ret == -1 ) {                            //encoding error
                    *retval = -1;
                } else {
                    *retval = min( ret, n );
                    rc = 0;
                }
            }
        }
    }
    if(msg != NULL) {
        // Runtime-constraints found
        // set s[0] to nullchar and *retval to -1
        if((s != NULL) && (smax > 0) && __lte_rsizmax( smax ))
            *s = '\0';
        if(retval != NULL)
            *retval = -1;
        // Now call the handler
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}
