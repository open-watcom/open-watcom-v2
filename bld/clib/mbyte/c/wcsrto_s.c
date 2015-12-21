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
#include "saferlib.h"
#include <mbstring.h>
#include <wchar.h>
#include "farfunc.h"


_WCRTLINK errno_t _NEARFAR(wcsrtombs_s,_fwcsrtombs_s)(
                           size_t _FFAR * __restrict retval,
                           char _FFAR * __restrict dst,
                           rsize_t dstmax,
                           const wchar_t _FFAR * _FFAR * __restrict src,
                           rsize_t len,
                           mbstate_t _FFAR * __restrict ps )
{
    int                     bytesConverted = 0;
    const wchar_t _FFAR *   wcPtr;
    unsigned char           mbc[MB_LEN_MAX+1];
    int                     ret = 0;

    errno_t                 rc = -1;
    const char             *msg = NULL;
    const wchar_t _FFAR    *srcend;
    char _FFAR             *dststart = dst;
    char _FFAR             *dstend;

    // Verify runtime-constraints
    // retval != NULL
    // src    != NULL
    // *src   != NULL
    // ps     != NULL
    // if dst == NULL then dstmax == 0
    // if dst != NULL then dstmax > 0
    if(__check_constraint_nullptr_msg( msg, retval ) &&
        __check_constraint_nullptr_msg( msg, src ) &&
        __check_constraint_nullptr_msg( msg, *src ) &&
        __check_constraint_nullptr_msg( msg, ps )) {
        wcPtr =  *src;
        srcend = *src + len;
        if(dst == NULL) {                /* get required size */
            // ensure dstmax == 0
            if(__check_constraint_a_gt_b_msg( msg, dstmax, 0 )) {
                for( ;; ) {
                    if( *wcPtr != '\0' ) {
                        if(srcend < wcPtr) break;             //no null found
                        ret = _NEARFAR(wcrtomb,_fwcrtomb)( (char *)mbc, *wcPtr, ps );
                        if( ret > 0 ) {
                            wcPtr++;
                            bytesConverted += ret;
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
                  *retval = bytesConverted;
                  rc = 0;
                }
            }
        } else {
            // more runtime-constraints
            // len    <= RSIZE_MAX
            // dstmax <= RSIZE_MAX
            // dstmax >  0
            if(__check_constraint_maxsize_msg( msg, dstmax ) &&
               __check_constraint_maxsize_msg( msg, len ) &&
               __check_constraint_a_gt_b_msg( msg, 1, dstmax )) {
                dstend = dst + dstmax;
                /*** Process the characters, one by one ***/
                for( ;; ) {

                    ret = _NEARFAR(wcrtomb,_fwcrtomb)( (char *)mbc, *wcPtr, ps );
                    if( ret > 0 ) {
                        if( *mbc != '\0' ) {
                            if( len >= ret ) {
                                _NEARFAR(_mbccpy,_fmbccpy)( (unsigned char _FFAR *)dst, mbc );
                                dst = (char _FFAR *)_NEARFAR(_mbsinc,_fmbsinc)( (unsigned char _FFAR *)dst );
                                wcPtr++;
                                bytesConverted += ret;
                                len -= ret;
                            } else {
                                break;
                            }
                            if( !__check_constraint_a_gt_b_msg( msg, dst, dstend )) {
                                break;
                            }
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                if( msg == NULL ) {
                    if( ret >= 0 ) {                          //no encoding error
                      *retval = bytesConverted;
                      rc = 0;
                    }
                }
            }
            if( msg == NULL ) {
                if( *dst == '\0' ) {
                    *src = NULL;
                } else {
                    *src = wcPtr;
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
