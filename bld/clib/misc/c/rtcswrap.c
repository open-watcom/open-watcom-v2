/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Error handler called when runtime-constraint is violated.
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include <string.h>
#include "seterrno.h"

void _WCNEAR __rtct_fail( const char *fn, const char *reason, void *reserved )
/****************************************************************************/
{
    char    msg[RTCT_MSG_MAX];

    /* Construct the error message the old fashioned unsafe way. We'd
     * rather not get re-entered here, and we have the maximum length
     * of the function name as well the error reason under control.
     */
    strcpy( msg, fn );
    strcat( msg, ", " );
    strcat( msg, reason );
    strcat( msg, ".\n" );

    /* Now call the current handler */
    _RWD_rtcthandler( msg, reserved, EINVAL );
}
