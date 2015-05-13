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
* Description:  This function searches for the specified file in the
*               1) current directory or, failing that,
*               2) the paths listed in the specified environment variable
*               until it finds the first occurrence of the file.
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef __UNIX__
    #include <direct.h>
#endif
#include <string.h>
#ifdef __OS2__
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "errorno.h"
#include "_environ.h"
#include "thread.h"

#if defined(__UNIX__)
    #define PATH_SEPARATOR STRING('/')
    #define LIST_SEPARATOR STRING(':')
#else
    #define PATH_SEPARATOR STRING('\\')
    #define LIST_SEPARATOR STRING(';')
#endif


_WCRTLINK void __F_NAME(_searchenv,_wsearchenv)( const CHAR_TYPE *name, const CHAR_TYPE *env_var, CHAR_TYPE *buffer )
{
    CHAR_TYPE   *p, *p2;
    int         prev_errno;
    size_t      len;

#ifdef __WIDECHAR__
    if( _RWD_wenviron == NULL ) {
        __create_wide_environment();
    }
#endif

    prev_errno = _RWD_errno;
    if( __F_NAME(access,_waccess)( name, F_OK ) == 0 ) {
        p = buffer;                                 /* JBS 90/3/30 */
        len = 0;                                    /* JBS 04/1/06 */
        for( ;; ) {
            if( name[0] == PATH_SEPARATOR ) break;
            if( name[0] == STRING( '.' ) ) break;
#ifndef __UNIX__
            if( name[0] == STRING( '/' ) ) break;
            if( name[0] != NULLCHAR && name[1] == STRING( ':' ) ) break;
#endif
            __F_NAME(getcwd,_wgetcwd)( buffer, _MAX_PATH );
            len = __F_NAME(strlen,wcslen)( buffer );
            p = &buffer[ len ];
            if( p[-1] != PATH_SEPARATOR ) {
                if( len < (_MAX_PATH - 1) ) {
                    *p++ = PATH_SEPARATOR;
                    len++;
                }
            }
            break;
        }
        *p = NULLCHAR;
        __F_NAME(strncat,wcsncat)( p, name, (_MAX_PATH - 1) - len );
        return;
    }
    p = __F_NAME(getenv,_wgetenv)( env_var );
    if( p != NULL ) {
        for( ;; ) {
            if( *p == NULLCHAR ) break;
            p2 = buffer;
            len = 0;                                /* JBS 04/1/06 */
            while( *p ) {
                if( *p == LIST_SEPARATOR ) break;
                if( *p != STRING( '"' ) ) {
                    if( len < (_MAX_PATH-1) ) {
                        *p2++ = *p; /* JBS 00/9/29 */
                        len++;
                    }
                }
                p++;
            }
            /* check for zero-length prefix which represents CWD */
            if( p2 != buffer ) {                    /* JBS 90/3/30 */
                if( p2[-1] != PATH_SEPARATOR
#ifndef __UNIX__
                    && p2[-1] != STRING( '/' ) && p2[-1] != STRING( ':' )
#endif
                    ) {
                    if( len < (_MAX_PATH - 1) ) {
                        *p2++ = PATH_SEPARATOR;
                        len++;
                    }
                }
                *p2 = NULLCHAR;
                len += __F_NAME(strlen,wcslen)( name );/* JBS 04/12/23 */
                if( len < _MAX_PATH ) {
                    __F_NAME(strcat,wcscat)( p2, name );
                    /* check to see if file exists */
                    if( __F_NAME(access,_waccess)( buffer, 0 ) == 0 ) {
                        _RWD_errno = prev_errno;
                        return;
                    }
                }
            }
            if( *p == NULLCHAR ) break;
            ++p;
        }
    }
    buffer[0] = NULLCHAR;
}
