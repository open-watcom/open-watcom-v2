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
* Description:  Implementation of strerror_s() - bounds checking strerror().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include "widechar.h"
#include <string.h>
#include <wchar.h>
#include <errno.h>
#include "errstr.h"


_WCRTLINK errno_t __F_NAME(strerror_s,wcserror_s)( CHAR_TYPE *s,
                                                   rsize_t maxsize,
                                                   errno_t errnum )
/*****************************************************************/
{
    errno_t     rc = -1;

#ifdef __WIDECHAR__
    wchar_t     Wide_Error_String[ 128 ];
#endif
    char        *msg;
    CHAR_TYPE   *p;
    CHAR_TYPE   *p2;
    size_t      msglen;
    int         k;

    /* runtime-constraints */
    // s not NULL
    // maxsize <= RSIZE_MAX
    // maxsize != 0
    if( __check_constraint_nullptr( s ) &&
        __check_constraint_maxsize( maxsize ) &&
        __check_constraint_zero( maxsize ) ) {

        if( errnum < 0 || errnum >= _sys_nerr ) {
            msg = UNKNOWN_ERROR;
        } else {
            msg = _sys_errlist[ errnum ];
        }

        msglen = strlen( msg );

#ifdef __WIDECHAR__
        _AToUni( Wide_Error_String, msg );
        p2 = Wide_Error_String;
#else
        p2 = msg;
#endif
        p = s;
        k = 0;
        // copy string up to end of string or end of receiving field
        while ( (k < maxsize - 1) && (*p2 != NULLCHAR) ) {
            *p++ = *p2++;
            ++k;
        };

        if( (maxsize > 3) && (msglen > maxsize - 4) ) {
            /* msg does not fit, indicate it's incomplete */
            /* and terminate string                       */
            __F_NAME(strcpy,wcscpy)( p - 3, STRING( "..." ) );
        } else {
            *p = NULLCHAR;                                  // terminate string
        }
        if( msglen < maxsize) {
            rc = 0;                                         // msg not truncated
        }
    }
    return( rc );
}
