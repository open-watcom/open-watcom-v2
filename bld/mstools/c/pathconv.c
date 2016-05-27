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
* Description:  String utilities to convert filenames to more usable format.
*               
*
****************************************************************************/


#include <mbstring.h>
#include <string.h>
#include "bool.h"
#include "memory.h"
#include "pathconv.h"


/*
 * Translate  foo/dir1\\dir2" \\"bar"grok  -->  "foo\\dir1\\dir2 \\"bargrok".
 */
char *PathConvert( const char *pathname, char quote )
/***************************************************/
{
    const unsigned char *path = (const unsigned char *)pathname;
    char                *out;
    unsigned char       *p;
    bool                quoteends = false;  /* quote the whole filename */
    bool                backslash = false;  /* true if last char was a '\\' */
    bool                inquote = false;    /* true if inside a quoted string */

    /*** Allocate a buffer for the new string (should be big enough) ***/
    out = AllocMem( 2 * ( strlen( (char *)path ) + 1 + 2 ) );
    p = (unsigned char *)out;

    /*** Determine if path contains any bizarre characters ***/
    if( _mbschr( path, ' ' )  !=  NULL      ||
        _mbschr( path, '\t' )  !=  NULL     ||
        _mbschr( path, '"' )  !=  NULL      ||
        _mbschr( path, '\'' )  !=  NULL     ||
        _mbschr( path, '`' )  !=  NULL      ||
        _mbschr( path, quote )  !=  NULL ) {
        quoteends = true;
        *p++ = quote;
    }

    /*** Convert the path one character at a time ***/
    while( *path != '\0' ) {
        if( *path == '"' ) {
            if( inquote ) {
                if( backslash ) {
                    *p++ = '"';         /* handle \" within a string */
                    backslash = false;
                } else {
                    inquote = false;
                }
            } else {
                inquote = true;
            }
        } else if( *path == '\\' ) {
            *p++ = '\\';
            if( backslash ) {
                backslash = false;
            } else {
                backslash = true;
            }
        } else if( *path == '/' ) {
            if( inquote ) {
                *p++ = '/';
            } else {
                *p++ = '\\';
            }
            backslash = false;
        } else {
            _mbccpy( p, path );         /* copy an ordinary character */
            p = _mbsinc( p );
            backslash = false;
        }
        path = _mbsinc( path );
    }
    if( quoteends )  *p++ = quote;
    *p++ = '\0';

    return( out );
}

/*
 * Translate  foo/dir1\\dir2" \\"bar"grok  -->  foo\\dir1\\dir2 \\"bargrok.
 */
char *PathConvertWithoutQuotes( const char *path )
/************************************************/
{
    char *              out;
    char *              p;
    bool                backslash = false;  /* true if last char was a '\\' */
    bool                inquote = false;    /* true if inside a quoted string */

    /*** Allocate a buffer for the new string (should be big enough) ***/
    out = AllocMem( 2 * ( strlen(path) + 1 + 2 ) );
    p = out;

    /*** Convert the path one character at a time ***/
    while( *path != '\0' ) {
        if( *path == '"' ) {
            if( inquote ) {
                if( backslash ) {
                    *p++ = '"';         /* handle \" within a string */
                    backslash = false;
                } else {
                    inquote = false;
                }
            } else {
                inquote = true;
            }
        } else if( *path == '\\' ) {
            *p++ = '\\';
            if( backslash ) {
                backslash = false;
            } else {
                backslash = true;
            }
        } else if( *path == '/' ) {
            if( inquote ) {
                *p++ = '/';
            } else {
                *p++ = '\\';
            }
            backslash = false;
        } else {
            /* copy an ordinary character */
            _mbccpy( (unsigned char *)p, (unsigned char *)path );     /* copy an ordinary character */
            p = (char *)_mbsinc( (unsigned char *)p );
            backslash = false;
        }
        path = (char *)_mbsinc( (unsigned char *)path );
    }
    *p++ = '\0';

    return( out );
}
