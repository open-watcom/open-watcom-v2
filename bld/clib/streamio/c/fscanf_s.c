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
* Description:  Implementation of fscanf_s() - safe formatted stream input.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include "saferlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include "rtdata.h"
#include "scanf.h"
#include "fileacc.h"
#include "orient.h"


static int cget_file( PTR_SCNF_SPECS specs )
{
    int     c;

    if( (c = __F_NAME(getc,getwc)( (FILE *)specs->ptr )) == __F_NAME(EOF,WEOF) ) {
        specs->eoinp = 1;
    }
    return( c );
}


static void uncget_file( int c, PTR_SCNF_SPECS specs )
{
    __F_NAME(ungetc,ungetwc)( c, (FILE *)specs->ptr );
}


_WCRTLINK int __F_NAME(fscanf_s,fwscanf_s)( FILE * __restrict stream,
                                 const CHAR_TYPE * __restrict format, ... )
{
    va_list         arg;
    SCNF_SPECS      specs;
    const char      *msg;
    int             rc;

    /* Basic check for null pointers to see if we can continue */
    if( __check_constraint_nullptr_msg( msg, stream )
     && __check_constraint_nullptr_msg( msg, format ) ) {

        _AccessFile( stream );

        /*** Deal with stream orientation ***/
        ORIENT_STREAM(stream,0);

        specs.ptr        = (CHAR_TYPE *)stream;
        specs.cget_rtn   = cget_file;
        specs.uncget_rtn = uncget_file;
        msg = NULL;

        va_start( arg, format );
        rc = __F_NAME(__scnf_s,__wscnf_s)( (PTR_SCNF_SPECS)&specs, format, &msg, arg );
        va_end( arg );

        _ReleaseFile( stream );

        if( msg == NULL ) {
            /* no rt-constraint violation inside worker routine */
            return( rc );
        }
    }
    __rtct_fail( __func__, msg, NULL );
    return( EOF );
}
