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
* Description:  Implementation of getcwd() for Linux. 
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include "seterrno.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "liballoc.h"
#include "linuxsys.h"

_WCRTLINK CHAR_TYPE *__F_NAME(getcwd,_wgetcwd)( CHAR_TYPE *buf, size_t size )
{
    unsigned char   path[ _MAX_PATH ];
    syscall_res     res;
    UCHAR_TYPE      *out;
    unsigned char   *in;
    CHAR_TYPE       ch;

    res = sys_call2( SYS_getcwd, (u_long)path, _MAX_PATH );
    if( __syscall_iserror( res ) ) {
        lib_set_errno( __syscall_errno( res ) );
        return( NULL );
    }
    if( buf == NULL ) {
        buf = lib_malloc( max( size, __syscall_val( size_t, res ) ) * CHARSIZE );
        if( buf == NULL ) {
            lib_set_errno( ENOMEM );
            return( NULL );
        }
    } else {
        if( __syscall_val( size_t, res ) > size ) {
            lib_set_errno( ERANGE );
            return( NULL );
        }
    }
    in = path;
    out = (UCHAR_TYPE *)buf;
    do {
        ch = *(in++);
        *(out++) = ch;
    } while( ch );
    return( buf );
}
