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
* Description:  Implementation of strerrorlen_s() - safe strerrorlen().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include "widechar.h"
#include <string.h>
#include <wchar.h>
#include "errstr.h"


_WCRTLINK size_t __F_NAME(strerrorlen_s,wcserrorlen_s)( errno_t errnum )
{
    size_t  m = 0;
    char    *msg;

    if( errnum < 0 || errnum >= _sys_nerr ) {
        msg = UNKNOWN_ERROR;
    } else {
        msg = _sys_errlist[ errnum ];
    }

    // count chars up to '\0'  assuming _sys_errlist is singlebyte
    while( *msg++ != '\0' ) {
        ++m;
    }
    return( m );
}
