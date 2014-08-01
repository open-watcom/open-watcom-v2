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
* Description:  Regexp and plain file matching routines.
*
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#if defined( __QNX__ )
#include <dir.h>
#else
#include <direct.h>
#endif
#include <malloc.h>

#include "watcom.h"
#define STANDALONE
#include "regexp.h"
#include "misc.h"
#include "fnutils.h"
#include "clibext.h"
#include "filerx.h"

static char *rxErrorStrings[] = {
    NULL,
    "Internal err: Regexp foulup",
    "Internal err: Regexp corrupted pointer",
    "Internal err: Regexp memory corruption",
    "Trailing \\\\",
    "?+* follows nothing",
    "Unmatched []",
    "invalid [] range",
    "nested *?+",
    "*+ operand could be empty",
    "Unmatched ()",
    "Too many ()",
    "NULL argument"
    "Invalid case toggle"
};

/* FileMatch - check if a file matches a wild card */
int FileMatch( void *crx, char *name )
{
    int i;

    i = RegExec( crx, name, TRUE );
    if( i ) {
        return( TRUE );
    }
    return( FALSE );
}

/* FileNameWild - determine if a file name has a wild card */
int FileNameWild( char *wild, int isrx )
{
    size_t      len, i;
    int         ch;

    len = strlen( wild );
    for( i = 0; i < len; i++ ) {
        ch = wild[i];
        if( !isrx ) {
            if( ch == '*' || ch == '?' ) {
                return( TRUE );
            }
        } else {
            if( ch == '[' || ch == ']' || ch == '*' || ch == '+' ||
                ch == '?' || ch == '(' || ch == ')' ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

/* FileMatchInit - start file matching */
char *FileMatchInit( void **crx, char *wild )
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
    MagicFlag = FALSE;
    j = 0;
    len = strlen( wild );
    for( i = 0; i < len; i++ ) { /* this loop is closely related to the next */
        if( wild[i] == '?' ) {
            j += 2;     /* for "\." */
        } 
        else if( wild[i] == '*' ) {
            j += 3;     /* for "\.*" */
        } else {
            ++j;
        }
    }
    tomatch = alloca( j + 3 );  /* for "^" "$" and null char */
    if( tomatch == NULL ) {
        Die( "Out of memory!\n" );
    }
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
    if( RegExpError ) {
        MemFree( rx );
        *crx = NULL;
    } else {
        *crx = rx;
    }
    return( rxErrorStrings[RegExpError] );
}

/* FileMatchFini - done with file matching */
void FileMatchFini( void *crx )
{
    MemFree( crx );
}

/* OpenDirAll */
DIR *OpenDirAll( char *filename, char *wild )
{
    size_t      i, j, len;
    char        npath[_MAX_PATH];
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

    return( opendir( npath ) );
}

/* FileMatchNoRx - match file name vs a wild card, the old dos way */
int FileMatchNoRx( char *name, char *wild )
{
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];
    char        sp_buf[_MAX_PATH2];
    char        *cfname;
    char        *cext;
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

    memset( sp_buf, 0, sizeof( sp_buf ) );
    _splitpath2( name, sp_buf, NULL, NULL, &cfname, &cext );
    if( cext[0] == '.' ) {
        ++cext;
    }

    len = strlen( cfname );
    len = max( len, flen );
    for( i = 0; i < len; i++ ) {
        if( FNameCharCmp( cfname[i], fname[i] ) != 0 && fname[i] != '?' ) {
            return( FALSE );
        }
    }

    len = strlen( cext );
    len = max( len, elen );
    for( i = 0; i < len; i++ ) {
        if( FNameCharCmp( cext[i], ext[i] ) != 0 && ext[i] != '?' ) {
            return( FALSE );
        }
    }
    return( TRUE );
}
