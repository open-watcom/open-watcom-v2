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
* Description:  Implementation of POSIX dirname().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>


/* Since we may need to insert terminating null characters, the pathname
 * storage needs to be writable. If a string literal is passed, it may well
 * not be writable. Hence we use a static buffer as intermediate storage
 * that we can modify. This is explicitly allowed by SUSv3.
 */
static char     path_buf[FILENAME_MAX];

static int  is_path_sep( const char c )
{
#ifdef __UNIX__
    return( c == '/' );
#else
    return( c == '/' || c == '\\' );
#endif
}

static int  is_final_sep_seq( const char *p )
{
    while( *p && is_path_sep( *p ) )
        ++p;

    return( !*p );
}

_WCRTLINK char *dirname( char *path )
{
    if( path == NULL || *path == '\0' ) {
        path_buf[0] = '.';
        path_buf[1] = '\0';
    } else {
        char    *s = path_buf;

        strlcpy( path_buf, path, sizeof( path_buf ) );
#ifndef __UNIX__
        /* Skip optional drive designation */
        if( isalnum( s[0] ) && s[1] == ':' )
            s += 2;
#endif
        if( !is_path_sep( *s++ ) ) {
            /* If path is not absolute, return "."
             */
            --s;
            s[0] = '.';
            s[1] = '\0';
        } else {
            char    *last_sep = s;

            while( *s ) {
                /* If the rest of the path is a sequence of path separators,
                 * don't consider them and quit.
                 */
                if( is_final_sep_seq( s ) )
                    break;

                if( is_path_sep( *s ) )
                    last_sep = s;

                ++s;
            }
            *last_sep = '\0';
        }
    }
    return( path_buf );
}
