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
* Description:  Implementation of wctomb_s() - bounds-checking wctomb().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#ifdef __NT__
    #include <windows.h>
    #include <winnls.h>
    #include "mbchar.h"
#endif
#include <mbstring.h>
#include "farfunc.h"



/****
***** Convert a wide character to a multibyte character.
****/

_WCRTLINK errno_t _NEARFAR(wctomb_s,_fwctomb_s)( int _FFAR * __restrict status,
                                                 char _FFAR * __restrict s,
                                                 rsize_t smax, wchar_t wc)
/**************************************************************************/
{
#ifdef __NT__
    int         rcnt;
#endif
    errno_t     rc = -1;
    const char  *msg = NULL;
    rsize_t     n;                     //needed byte count for multibyte char

    // Verify runtime-constraints
    // smax <= RSIZE_MAX
    // if s == NULL then smax = 0
    // if s != NULL then smax >= n
    if(__check_constraint_maxsize_msg( msg, smax )) {
        if(s == NULL) {
            if(__check_constraint_a_gt_b_msg( msg, smax, 0 )) {
                *status = 0; /* no state-dependant encodings */
                rc = 0;
            }
        } else {

            if(wc & 0xff00) n = 2;
            else            n = 1;

            if(__check_constraint_a_gt_b_msg( msg, n, smax )) {
                /*** Convert the character ***/
                #ifdef __NT__
                    rcnt = WideCharToMultiByte( __MBCodePage, WC_COMPOSITECHECK,
                                              (LPCWSTR)&wc, 1, (LPSTR)s,
                                              MB_CUR_MAX, NULL, NULL );
                    if( rcnt != FALSE ) {
                        if(rcnt <= min( MB_CUR_MAX, smax )) {
                            *status = rcnt;
                            rc = 0;
                        }
                    }
                #else                               /* OS/2 and others */
                    if( wc & 0xFF00 ) {
                        s[0] = (wc & 0xFF00) >> 8;      /* store lead byte */
                        s[1] = wc & 0x00FF;             /* store trail byte */
                        *status = 2;                    /* size in bytes */
                    } else {
                        s[0] = wc & 0x00FF;             /* store char byte */
                        *status = 1;                    /* size in bytes */
                    }
                    rc = 0;
                #endif
            }
        }

    }
    if(msg != NULL) {
        // Runtime-constraints found
        // Now call the handler
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}
