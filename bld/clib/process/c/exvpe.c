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


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <process.h>
#include "msdos.h"
#include "rtdata.h"
#include "seterrno.h"
#include "_process.h"


_WCRTLINK int __F_NAME(execvpe,_wexecvpe)( const CHAR_TYPE *file, const CHAR_TYPE * const *argv, const CHAR_TYPE * const *envp )
{
    register CHAR_TYPE *p;
    register CHAR_TYPE *p2;
    register int retval;
    auto CHAR_TYPE buffer[_MAX_PATH];
    size_t file_len;
    CHAR_TYPE *end;

    retval = __F_NAME(execve,_wexecve)( file, argv, envp );
    if( retval != -1  ||
        (_RWD_errno != ENOENT && _RWD_errno != EINVAL)) return( retval );
    if( *file == STRING( '\\' ) || *file == NULLCHAR || file[1] == STRING( ':' ) )
        return( retval );
    p = __F_NAME(getenv,_wgetenv)( STRING( "PATH" ) );
    if( p == NULL ) return( retval );
    file_len = __F_NAME(strlen,wcslen)( file ) + 1;
    for(;;) {
        if( *p == NULLCHAR ) break;
        end = __F_NAME(strchr,wcschr)( p, STRING( ';' ) );
        if( end == NULL ) {
            end = p + __F_NAME(strlen,wcslen)( p ); /* find null-terminator */
        }
        if( end - p > _MAX_PATH - file_len ) {
            __set_errno( E2BIG );
            __set_doserrno( E_badenv );
            return( -1 );
        }
        memcpy( buffer, p, (end-p)*sizeof(CHAR_TYPE) );
        p2 = buffer + ( end - p );
        if( p2[-1] != STRING( '\\' ) ) {
            *p2++ = STRING( '\\' );
        }
        memcpy( p2, file, file_len*sizeof(CHAR_TYPE) );
        retval = __F_NAME(execve,_wexecve)( buffer, argv, envp );
        if( retval != -1 ) break;
        if(_RWD_errno != ENOENT && _RWD_errno != EINVAL) break;
        if( *end != STRING( ';' ) ) break;
        p = end + 1;
    }
    return( retval );
}
