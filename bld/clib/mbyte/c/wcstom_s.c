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
* Description:  Safer C Library Implementation of wcstombs_s() _fwcstombs_s()
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include <mbstring.h>
#include <string.h>
#include "farfunc.h"



/****
***** Convert a wide character string to a multibyte character string.
****/

_WCRTLINK errno_t _NEARFAR(wcstombs_s,_fwcstombs_s)( size_t _FFAR * __restrict retval,
                                                     char _FFAR * __restrict dst,
                                                     rsize_t dstmax,
                                                     const wchar_t _FFAR * __restrict src,
                                                     rsize_t len )
{
    char                mbc[MB_LEN_MAX+1];
    size_t              numBytes = 0;
    size_t              ret = 0;

    errno_t             rc = -1;
    const char          *msg = NULL;
    const wchar_t _FFAR *srcend = src + len;
    char _FFAR          *dststart = dst;
    rsize_t             maxlen;

    // Verify runtime-constraints
    // retval != NULL
    // src    != NULL
    // if dst == NULL then dstmax == 0
    // if dst != NULL then dstmax != 0
    if(__check_constraint_nullptr_msg( msg, retval ) &&
        __check_constraint_nullptr_msg( msg, src )) {
        if(dst == NULL) {                /* get required size */
            // ensure dstmax == 0
            if(__check_constraint_a_gt_b_msg( msg, dstmax, 0 )) {
                for( ;; ) {
                    if( *src != L'\0' ) {
                        if(srcend < src) break;               //no null found
                        ret = wctomb( mbc, *src );
                        if( ret == -1 )  {
                            *retval = -1;
                            break;                           //encoding error
                        }
                        numBytes += ret;
                        src++;
                    } else {
                        break;
                    }
                }
                if( ret != -1 ) {                         //no encoding error
                  *retval = numBytes;
                  rc = 0;
                }
            }
        } else {                                    /* convert the string */
            // more runtime-constraints
            // len    <= RSIZE_MAX
            // dstmax <= RSIZE_MAX
            if(__check_constraint_maxsize_msg( msg, dstmax ) &&
               __check_constraint_maxsize_msg( msg, len ) &&
               __check_constraint_a_gt_b_msg( msg, 1, dstmax )) {

                for( maxlen = min(len, dstmax); maxlen > 0; maxlen-- ) {

                    if( *src != L'\0' ) {
                        if(srcend < src) break;               //no null found
                        ret = wctomb( mbc, *src );
                        if( ret == -1 )  {
                            *retval = -1;
                            break;               //encoding error
                        }
                        _NEARFAR(memcpy,_fmemcpy)( dst, mbc, ret );
                        dst += ret;
                        numBytes += ret;
                        src++;
                    } else {
                        break;
                    }
                }
                *dst = '\0';            // terminate string
                if(__check_constraint_a_gt_b_msg( msg, src, srcend )) {
                    if( ret != -1 ) {                         //no encoding error
                      *retval = numBytes;
                      rc = 0;
                    }
                }
            }
        }
    }
    if(msg != NULL) {
        // Runtime-constraint found
        // set dst[0] to nullchar and *retval to -1
        if((dst != NULL) && (dstmax > 0) && __lte_rsizmax( dstmax ))
            *dststart  = '\0';
        if(retval != NULL)
            *retval = -1;
        // Now call the handler
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}
