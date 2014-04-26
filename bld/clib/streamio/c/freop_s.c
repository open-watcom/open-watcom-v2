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
* Description:  Implementation of freopen_s() - safe version of freopen().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include "saferlib.h"
#include <wchar.h>
#include <stdio.h>
#include <ctype.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#include "fileacc.h"
#include "fmode.h"
#include "openmode.h"
#include "rtdata.h"


_WCRTLINK errno_t __F_NAME(freopen_s,_wfreopen_s)(
                          FILE * __restrict * __restrict newstreamptr,
                          const CHAR_TYPE * __restrict filename,
                          const CHAR_TYPE * __restrict mode,
                          FILE * __restrict stream )
/********************************************************************/
{
    errno_t     rc = -1;
    const char *msg;

    /* check for runtime-constraints */
    /* newstreamptr  not null */
    /* mode          not null */
    /* stream        not null */
    if( __check_constraint_nullptr_msg( msg, newstreamptr )  &&
        __check_constraint_nullptr_msg( msg, mode ) &&
        __check_constraint_nullptr_msg( msg, stream ) ) {

        /* ua.. and uw.. are treated as a.. and w.. */
        if( (*mode == __F_NAME('u',L'u')) &&
            ( (*(mode + 1) == __F_NAME('r',L'r')) ||
              (*(mode + 1) == __F_NAME('w',L'w')) ||
              (*(mode + 1) == __F_NAME('a',L'a'))) ) {
            mode++;                         /* ignore u for now */
        }
        *newstreamptr = __F_NAME(freopen,_wfreopen)( filename, mode, stream );

        if( *newstreamptr != NULL ) {  /* if reopen ok set rc = 0 */
            rc = 0;
        }
    } else {
        /* Runtime-constraints found, store zero in newstreamptr */
        if( newstreamptr != NULL ) {
            *newstreamptr = NULL;
        }
        /* Now call the handler */
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}
