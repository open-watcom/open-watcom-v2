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
* Description:  Safer C Library  Implementation of mbsrtowcs_s(),
*                                                _fmbsrtowcs_s().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include <mbstring.h>
#include <wchar.h>
#include "farfunc.h"


_WCRTLINK errno_t _NEARFAR(mbsrtowcs_s,_fmbsrtowcs_s)( size_t _FFAR * __restrict retval,
                           wchar_t _FFAR * __restrict dst, rsize_t dstmax,
                           const char _FFAR * _FFAR * __restrict src, rsize_t len,
                           mbstate_t _FFAR * __restrict ps )
{
    wchar_t             wc;
    int                 charsConverted = 0;
    const char _FFAR *  mbcPtr;
    int                 ret = 0;

    errno_t             rc = -1;
    const char          *msg = NULL;
    const char _FFAR    *srcend;
    wchar_t _FFAR       *dststart = dst;
    rsize_t             maxlen;

    // Verify runtime-constraints
    // retval != NULL
    // src    != NULL
    // *src   != NULL
    // ps     != NULL
    // if dst == NULL then dstmax == 0
    // if dst != NULL then dstmax != 0
    if(__check_constraint_nullptr_msg( msg, retval ) &&
        __check_constraint_nullptr_msg( msg, src ) &&
        __check_constraint_nullptr_msg( msg, *src ) &&
        __check_constraint_nullptr_msg( msg, ps )) {
        mbcPtr =  *src;
        if(dst == NULL) {                /* get required size */
            // ensure dstmax == 0
            if(__check_constraint_a_gt_b_msg( msg, dstmax, 0 )) {
                for( ;; ) {
                    if( *mbcPtr != '\0' ) {
                        ret = _NEARFAR(mbrtowc,_fmbrtowc)( &wc, mbcPtr, MB_LEN_MAX, ps );
                        if( ret > 0 ) {
                            mbcPtr += ret;
                            charsConverted++;
                        } else if( ret == 0) {
                            break;
                        } else {
                            *retval = -1;
                            break;               //encoding error
                        }
                    } else {
                        break;
                    }
                }
                if( ret >= 0 ) {                          //no encoding error
                  *retval = charsConverted;
                  rc = 0;
                }
            }
        } else {
            // more runtime-constraints
            // len    <= RSIZE_MAX
            // dstmax <= RSIZE_MAX
            if(__check_constraint_maxsize_msg( msg, dstmax ) &&
               __check_constraint_maxsize_msg( msg, len ) &&
               __check_constraint_a_gt_b_msg( msg, 1, dstmax )) {

                srcend = *src + len;
                /*** Process the characters, one by one ***/
                for( maxlen = min(len, dstmax); maxlen > 0; maxlen-- ) {
                    if( *mbcPtr != '\0' ) {
                        if(srcend < mbcPtr) break;            //no null found
                        ret = _NEARFAR(mbrtowc,_fmbrtowc)( &wc, mbcPtr, MB_LEN_MAX, ps );
                        if( ret > 0 ) {
                            *dst++ = wc;
                            mbcPtr += ret;
                            charsConverted++;
                        } else if( ret == 0) {
                            break;
                        } else {
                            *retval = -1;
                            break;               //encoding error
                        }
                    } else {
                        break;
                    }
                }
                if( (dstmax > len) ||
                    __check_constraint_toosmall_msg( msg, dst, maxlen )) {
                    *dst = L'\0';            // terminate string
                }
                if( (msg == NULL) && __check_constraint_a_gt_b_msg( msg, mbcPtr, srcend )) {
                    if( ret >= 0 ) {                      //no encoding error
                      *retval = charsConverted;
                      rc = 0;
                      if( ret == 0 ) {
                          *src = NULL;
                      } else {
                          *src = mbcPtr;
                      }
                    }
                }
            }
        }
    }
    if(msg != NULL) {
        // Runtime-constraint found
        // set dst[0] to nullchar and *retval to -1
        if((dst != NULL) && (dstmax > 0) && __lte_rsizmax( dstmax ))
            *dststart = L'\0';
        if(retval != NULL)
            *retval = -1;
        // Now call the handler
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}
