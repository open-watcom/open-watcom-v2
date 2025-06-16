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


#ifndef __UNIX__
    #include <mbstring.h>
#endif
#include <string.h>
#include "bool.h"
#include "memory.h"
#include "pathconv.h"


/*
 * Translate  foo/dir1\\dir2" \\"bar"grok  -->  "foo\\dir1\\dir2 \\"bargrok".
 */
char *PathConvert( const char *path, char quote )
/***************************************************/
{
    char                *p;
    char                *out;
    bool                quoteends = false;  /* quote the whole filename */
    bool                backslash = false;  /* true if last char was a '\\' */
    bool                inquote = false;    /* true if inside a quoted string */

    /*** Allocate a buffer for the new string (should be big enough) ***/
    out = AllocMem( 2 * ( strlen( path ) + 1 + 2 ) );
    p = out;

    /*** Determine if path contains any bizarre characters ***/
#ifdef __UNIX__
    if( strchr( path, ' ' ) !=  NULL
      || strchr( path, '\t' ) !=  NULL
      || strchr( path, '"' ) !=  NULL
      || strchr( path, '\'' ) !=  NULL
      || strchr( path, '`' ) !=  NULL
      || strchr( path, quote ) !=  NULL )
#else
    if( _mbschr( (const unsigned char *)path, ' ' ) !=  NULL
      || _mbschr( (const unsigned char *)path, '\t' ) !=  NULL
      || _mbschr( (const unsigned char *)path, '"' ) !=  NULL
      || _mbschr( (const unsigned char *)path, '\'' ) !=  NULL
      || _mbschr( (const unsigned char *)path, '`' ) !=  NULL
      || _mbschr( (const unsigned char *)path, quote ) !=  NULL )
#endif
    {
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
            /*
             * copy an ordinary character
             */
#ifdef __UNIX__
            *p++ = *path;
#else
            _mbccpy( (unsigned char *)p, (unsigned char *)path );
            p = (char *)_mbsinc( (unsigned char *)p );
#endif
            backslash = false;
        }
#ifdef __UNIX__
        path++;
#else
        path = (char *)_mbsinc( (unsigned char *)path );
#endif
    }
    if( quoteends )
        *p++ = quote;
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
    out = AllocMem( 2 * ( strlen( path ) + 1 + 2 ) );
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
            /*
             * copy an ordinary character
             */
#ifdef __UNIX__
            *p++ = *path;
#else
            _mbccpy( (unsigned char *)p, (unsigned char *)path );
            p = (char *)_mbsinc( (unsigned char *)p );
#endif
            backslash = false;
        }
#ifdef __UNIX__
        path++;
#else
        path = (char *)_mbsinc( (unsigned char *)path );
#endif
    }
    *p++ = '\0';

    return( out );
}
