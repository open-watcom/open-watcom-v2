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


#include <mbstring.h>
#include <string.h>
#include "memory.h"
#include "pathconv.h"


/*
 * Translate  foo/dir1\\dir2" \\"bar"grok  -->  "foo\\dir1\\dir2 \\"bargrok".
 */
char *PathConvert( const char *path, char quote )
/***********************************************/
{
    char *              out;
    char *              p;
    int                 quoteends;      /* quote the whole filename */
    int                 backslash = 0;  /* true if last char was a '\\' */
    int                 inquote = 0;    /* true if inside a quoted string */

    /*** Allocate a buffer for the new string (should be big enough) ***/
    out = AllocMem( 2 * ( strlen(path) + 1 + 2 ) );
    p = out;

    /*** Determine if path contains any bizarre characters ***/
    if( _mbschr( path, ' ' )  !=  NULL      ||
        _mbschr( path, '\t' )  !=  NULL     ||
        _mbschr( path, '"' )  !=  NULL      ||
        _mbschr( path, '\'' )  !=  NULL     ||
        _mbschr( path, '\`' )  !=  NULL     ||
        _mbschr( path, quote )  !=  NULL ) {
        quoteends = 1;
        *p++ = quote;
    } else {
        quoteends = 0;
    }

    /*** Convert the path one character at a time ***/
    while( *path != '\0' ) {
        if( *path == '"' ) {
            if( inquote ) {
                if( backslash ) {
                    *p++ = '"';         /* handle \" within a string */
                    backslash = 0;
                } else {
                    inquote = 0;
                }
            } else {
                inquote = 1;
            }
        } else if( *path == '\\' ) {
            *p++ = '\\';
            if( backslash ) {
                backslash = 0;
            } else {
                backslash = 1;
            }
        } else if( *path == '/' ) {
            if( inquote ) {
                *p++ = '/';
            } else {
                *p++ = '\\';
            }
            backslash = 0;
        } else {
            _mbccpy( p, path );         /* copy an ordinary character */
            p = _mbsinc( p );
            backslash = 0;
        }
        path = _mbsinc( path );
    }
    if( quoteends )  *p++ = quote;
    *p++ = '\0';

    return( out );
}
