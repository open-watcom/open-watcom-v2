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
* Description:  Implementation of spawnvpe() and _wspawnvpe().
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <process.h>
#if defined( __OS2__ )
#include <wos2.h>
#endif
#include "rtdata.h"
#include "msdos.h"
#include "errorno.h"
#include "thread.h"

#pragma on(check_stack);

_WCRTLINK int __F_NAME(spawnvpe,_wspawnvpe)( int mode, const CHAR_TYPE *file, const CHAR_TYPE * const *argv, const CHAR_TYPE * const *envp )
{
    CHAR_TYPE       *p;
    CHAR_TYPE       *p2;
    int             retval;
    CHAR_TYPE       buffer[_MAX_PATH];
    size_t          file_len;
    CHAR_TYPE       *end;

    retval = __F_NAME(spawnve,_wspawnve)( mode, file, argv, envp );
    if( retval != -1  || (_RWD_errno != ENOENT && _RWD_errno != EINVAL) )
        return( retval );
    if( *file == STRING( '\\' ) || *file == NULLCHAR || file[1] == STRING( ':' ) )
        return( retval );
    p = __F_NAME(getenv,_wgetenv)( STRING( "PATH" ) );
    if( p == NULL )
        return( retval );
    file_len = __F_NAME(strlen,wcslen)( file ) + 1;
    for( ;; ) {
        if( *p == NULLCHAR )
            break;
        end = __F_NAME(strchr,wcschr)( p, STRING( ';' ) );
        if( end == NULL ) {
            end = p + __F_NAME(strlen,wcslen)( p ); /* find null-terminator */
        }
        if( end - p > _MAX_PATH - file_len ) {
            _RWD_errno = E2BIG;
            _RWD_doserrno = E_badenv;
            return( -1 );
        }
        memcpy( buffer, p, (end - p) * sizeof( CHAR_TYPE ) );
        p2 = buffer + (end - p);
        if( p2[-1] != STRING( '\\' ) ) {
            *p2++ = STRING( '\\' );
        }
        memcpy( p2, file, file_len * sizeof( CHAR_TYPE ) );
        retval = __F_NAME(spawnve,_wspawnve)( mode, buffer, argv, envp );
        if( retval != -1 )
            break;
        if(_RWD_errno != ENOENT && _RWD_errno != EINVAL)
            break;
        if( *end != STRING( ';' ) )
            break;
        p = end + 1;
    }
    return( retval );
}
