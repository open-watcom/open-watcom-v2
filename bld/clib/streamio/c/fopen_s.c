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
* Description:  Implementation of fopen_s() - safe version of fopen().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include "widechar.h"
#include <wchar.h>
#include <stdio.h>
#include <ctype.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
// #include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fileacc.h"
#include "fmode.h"
#include "openmode.h"
#include "rtdata.h"
// #include "seterrno.h"
// #include "defwin.h"


_WCRTLINK errno_t __F_NAME(fopen_s,_wfopen_s)( FILE * __restrict * __restrict streamptr,
                                               const CHAR_TYPE * __restrict filename,
                                               const CHAR_TYPE * __restrict mode)
/**************************************************************************************/
{
    errno_t     rc = -1;
    const char *msg;

    /* check for runtime-constraints */
    /* streamptr  not null */
    /* filename   not null */
    /* mode       not null */
    if( __check_constraint_nullptr_msg( msg, streamptr )  &&
        __check_constraint_nullptr_msg( msg, filename ) &&
        __check_constraint_nullptr_msg( msg, mode ) ) {

        /* ua.. and uw.. are treated as a.. and w.. */
        if( *mode == STRING( 'u' ) &&
            ( *(mode + 1) == STRING( 'r' ) ||
              *(mode + 1) == STRING( 'w' ) ||
              *(mode + 1) == STRING( 'a' ) ) ) {
            mode++;                         /* ignore u for now */
        }
        *streamptr = __F_NAME(_fsopen,_wfsopen)( filename, mode, OPENMODE_DENY_COMPAT );

        if( *streamptr != NULL ) {  /* if open ok set rc = 0 */
            rc = 0;
        }
    } else {
        /* Runtime-constraints found, store zero in streamptr */
        if( streamptr != NULL ) {
            *streamptr = NULL;
        }
        /* Now call the handler */
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}
