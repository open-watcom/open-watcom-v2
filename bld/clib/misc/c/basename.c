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
* Description:  Implementation of POSIX basename().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <libgen.h>


/* Since we may need to delete trailing path separators, the pathname
 * storage must be writable. If a string literal is passed, it may well
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

/* Note: This would lend itself to regular expressions. Assuming '/' as
 * the path separator, the following regexp would do most of the work:
 *
 *  \([^\/]+\)\/*$
 */

_WCRTLINK char *basename( char *path )
{
    char    *last_part = path_buf;

    if( path == NULL || *path == '\0' ) {
        path_buf[0] = '.';
        path_buf[1] = '\0';
    } else {
        char    *s = path_buf;

        strlcpy( path_buf, path, sizeof( path_buf ) );
        if( is_final_sep_seq( path_buf ) ) {
            /* If entire path consists of path separators, return
             * a string consisting of just one.
             */
            path_buf[1] = '\0';
        } else {
            while( *s ) {
                if( is_path_sep( *s++ ) )
                    last_part = s;
                /* If the rest of the path is a sequence of path separators,
                 * delete them by inserting a null terminator.
                 */
                if( is_final_sep_seq( s ) )
                    *s = '\0';
            }
        }
    }
    return( last_part );
}
