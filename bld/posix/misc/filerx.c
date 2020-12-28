/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Regexp and plain file matching routines.
*
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#if defined( __QNX__ )
#include <sys/dir.h>
#else
#include <direct.h>
#endif
#include "watcom.h"
#include "regexp.h"
#include "misc.h"
#include "fnutils.h"
#include "filerx.h"
#include "pathgrp2.h"

#include "clibext.h"


static char *rxErrorStrings[] = {
    #define pick(e,t)       t,
        REGEXPR_ERRORS()
    #undef pick
};

/* FileMatch - check if a file matches a wild card */
int FileMatch( void *crx, const char *name )
{
    int i;

    i = RegExec( crx, name, true );
    if( i ) {
        return( true );
    }
    return( false );
}

/* FileNameWild - determine if a file name has a wild card */
bool FileNameWild( const char *wild, bool isrx )
{
    size_t      len, i;
    int         ch;

    len = strlen( wild );
    for( i = 0; i < len; i++ ) {
        ch = wild[i];
        if( !isrx ) {
            if( ch == '*' || ch == '?' ) {
                return( true );
            }
        } else {
            if( ch == '[' || ch == ']' || ch == '*' || ch == '+' ||
                ch == '?' || ch == '(' || ch == ')' ) {
                return( true );
            }
        }
    }
    return( false );
}

/* FileMatchInit - start file matching */
char *FileMatchInit( void **crx, const char *wild )
{
    char        *tomatch;
    size_t      j;
    regexp      *rx;
    size_t      i, len;

    /*
        Calculate size of regular expression.  We calculate it because it
        can be as large as 3*_MAX_PATH... which is a lot to allocate off
        the stack all the time.
    */
    MagicString = ".";
    MagicFlag = false;
    j = 0;
    len = strlen( wild );
    for( i = 0; i < len; i++ ) { /* this loop is closely related to the next */
        if( wild[i] == '?' ) {
            j += 2;     /* for "\." */
        } else if( wild[i] == '*' ) {
            j += 3;     /* for "\.*" */
        } else {
            ++j;
        }
    }
    tomatch = MemAlloc( j + 3 );  /* for "^" "$" and null char */
    tomatch[0] = '^';
    j = 1;
    for( i = 0; i < len; i++ ) {
        if( wild[i] == '#' ) {
            tomatch[j++] = '|';
        } else if( wild[i] == '?' ) {
            tomatch[j++] = '\\';
            tomatch[j++] = '.';
        } else if( wild[i] == '*' ) {
            tomatch[j++] = '\\';
            tomatch[j++] = '.';
            tomatch[j++] = '*';
        } else {
            tomatch[j++] = wild[i];
        }
    }
    tomatch[j++] = '$';
    tomatch[j] = 0;

    rx = RegComp( tomatch );
    if( RegExpError == ERR_NO_ERR ) {
        *crx = rx;
    } else {
        MemFree( rx );
        *crx = NULL;
    }
    MemFree( tomatch );
    return( rxErrorStrings[RegExpError] );
}

/* FileMatchFini - done with file matching */
void FileMatchFini( void *crx )
{
    MemFree( crx );
}

char *FileMatchDirAll( const char *filename, char *npath, char *wild )
{
    size_t      i, j, len;
    char        ch;

    len = strlen( filename );
    for( i = len; i > 0; --i ) {
        ch = filename[i - 1];
        if( ch == '/' || ch == '\\' || ch == ':' ) {
            break;
        }
    }
    for( j = 0; j < i; j++ ) {
        npath[j] = filename[j];
    }
    npath[i] = 0;
    for( j = i; j <= len; j++ ) {
        wild[j - i] = filename[j];
    }

    strcat( npath, "*.*" );
    return( npath );
}

/* FileMatchNoRx - match file name vs a wild card, the old dos way */
int FileMatchNoRx( const char *name, const char *wild )
{
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];
    pgroup2     pg;
    unsigned    j, elen, flen;
    size_t      i, k, len;

    len = strlen( wild );

    elen = flen = 0;
    for( i = 0; i < len; i++ ) {
        if( wild[i] == '.' ) {
            i++;
            break;
        }
        if( wild[i] == '*' ) {
            for( j = flen; j < _MAX_FNAME - 1; j++ ) {
                fname[j] = '?';
            }
            flen = _MAX_FNAME - 1;
        }
        if( flen < _MAX_FNAME - 1 ) {
            fname[flen] = wild[i];
            flen++;
        }
    }
    fname[flen] = 0;

    for( k = i; k < len; k++ ) {
        if( wild[k] == '*' ) {
            for( j = elen; j < _MAX_EXT - 1; j++ ) {
                ext[j] = '?';
            }
            elen = _MAX_EXT - 1;
            break;
        }
        ext[elen] = wild[k];
        elen++;
    }
    ext[elen] = 0;

    _splitpath2( name, pg.buffer, NULL, NULL, &pg.fname, &pg.ext );
    if( pg.ext[0] == '.' ) {
        pg.ext++;
    }

    len = strlen( pg.fname );
    if( len < flen )
        len = flen;
    for( i = 0; i < len; i++ ) {
        if( FNameCharCmp( pg.fname[i], fname[i] ) != 0 && fname[i] != '?' ) {
            return( false );
        }
    }

    len = strlen( pg.ext );
    if( len < elen )
        len = elen;
    for( i = 0; i < len; i++ ) {
        if( FNameCharCmp( pg.ext[i], ext[i] ) != 0 && ext[i] != '?' ) {
            return( false );
        }
    }
    return( true );
}
