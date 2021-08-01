/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This function searches for the specified file in the
*               1) current directory or, failing that,
*               2) the paths listed in the specified environment variable
*               until it finds the first occurrence of the file.
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef __UNIX__
    #include <direct.h>
#endif
#include <string.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "rterrno.h"
#include "_environ.h"
#include "thread.h"
#include "pathmac.h"


#if defined(__UNIX__)
    #define LIST_SEPARATOR STRING(':')
#else
    #define LIST_SEPARATOR STRING(';')
#endif


_WCRTLINK void __F_NAME(_searchenv,_wsearchenv)( const CHAR_TYPE *name, const CHAR_TYPE *env_var, CHAR_TYPE *buffer )
{
    CHAR_TYPE   *p, *p2;
    int         prev_errno;
    size_t      len;

    CHECK_WIDE_ENV();

    prev_errno = _RWD_errno;
    if( __F_NAME(access,_waccess)( name, F_OK ) == 0 ) {
        p = buffer;
        len = 0;
        if( !IS_DIR_SEP( name[0] ) && name[0] != STRING( '.' ) ) {
#ifndef __UNIX__
            if( name[0] == NULLCHAR || name[1] != DRV_SEP ) {
#endif
                __F_NAME(getcwd,_wgetcwd)( buffer, _MAX_PATH );
                if( *buffer != NULLCHAR ) {
                    len = __F_NAME(strlen,wcslen)( buffer );
                    p = buffer + len;
                    if( p[-1] != DIR_SEP ) {
                        if( len < ( _MAX_PATH - 1 ) ) {
                            *p++ = DIR_SEP;
                            len++;
                        }
                    }
                }
#ifndef __UNIX__
            }
#endif
        }
        *p = NULLCHAR;
        __F_NAME(strncat,wcsncat)( p, name, ( _MAX_PATH - 1 ) - len );
        return;
    }
    p = __F_NAME(getenv,_wgetenv)( env_var );
    if( p != NULL ) {
        for( ;; ) {
            if( *p == NULLCHAR )
                break;
            p2 = buffer;
            len = 0;
            while( *p != NULLCHAR ) {
                if( *p == LIST_SEPARATOR )
                    break;
                if( *p != STRING( '"' ) ) {
                    if( len < ( _MAX_PATH - 1 ) ) {
                        *p2++ = *p;
                        len++;
                    }
                }
                p++;
            }
            /* check for zero-length prefix which represents CWD */
            if( p2 != buffer ) {
#ifdef __UNIX__
                if( !IS_DIR_SEP( p2[-1] ) ) {
#else
                if( !IS_DIR_SEP( p2[-1] ) && p2[-1] != DRV_SEP ) {
#endif
                    if( len < (_MAX_PATH - 1) ) {
                        *p2++ = DIR_SEP;
                        len++;
                    }
                }
                *p2 = NULLCHAR;
                len += __F_NAME(strlen,wcslen)( name );
                if( len < _MAX_PATH ) {
                    __F_NAME(strcat,wcscat)( p2, name );
                    /* check to see if file exists */
                    if( __F_NAME(access,_waccess)( buffer, 0 ) == 0 ) {
                        _RWD_errno = prev_errno;
                        return;
                    }
                }
            }
            if( *p == NULLCHAR )
                break;
            ++p;
        }
    }
    buffer[0] = NULLCHAR;
}
