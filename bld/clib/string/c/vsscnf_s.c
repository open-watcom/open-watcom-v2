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
* Description:  Implementation of vsscanf_s() - safe formatted string input.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include "saferlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include "scanf.h"


static INTCHAR_TYPE cget_string( PTR_SCNF_SPECS specs )
{
    INTCHAR_TYPE    c;

    if( (c = *(UCHAR_TYPE *)(specs->ptr)) != NULLCHAR ) {
        ++(specs->ptr);
    } else {
        c = INTCHAR_EOF;
        specs->eoinp = 1;
    }
    return( c );
}


static void uncget_string( INTCHAR_TYPE c, PTR_SCNF_SPECS specs )
{
    --specs->ptr;
}


_WCRTLINK int __F_NAME(vsscanf_s,vswscanf_s)( const CHAR_TYPE *s, const CHAR_TYPE *format, va_list arg )
{
    SCNF_SPECS      specs;
    const char      *msg;
    int             rc;

    /* Basic check for null pointers to see if we can continue */
    if( __check_constraint_nullptr_msg( msg, s )
     && __check_constraint_nullptr_msg( msg, format ) ) {

        specs.ptr        = (CHAR_TYPE *)s;
        specs.cget_rtn   = cget_string;
        specs.uncget_rtn = uncget_string;
        msg = NULL;
        rc = __F_NAME(__scnf_s,__wscnf_s)( (PTR_SCNF_SPECS)&specs, format, &msg, arg );
        if( msg == NULL ) {
            /* no rt-constraint violation inside worker routine */
            return( rc );
        }
    }
    __rtct_fail( __func__, msg, NULL );
    return( EOF );
}
