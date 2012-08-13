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
* Description:  WRC command line include directory parameter parsing.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "bool.h"
#include "global.h"
#include "rcmem.h"
#include "errors.h"

#if defined(__UNIX__)
# define PATH_SPLIT_S       ":"     /* path seperator in string form        */
#else
# define PATH_SPLIT_S       ";"     /* path seperator in string form        */
#endif


#if 0
static char *fixNewDirs( char *arg ) {
/************************************/
    char        *ret;
    char        *src;
    char        *dst;

    src = arg;
    ret = RcMemMalloc( strlen( src ) + 1 );
    dst = ret;
    while( *src != '\0' ) {
        if( !isspace( *src ) ) {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0';
    RcMemFree( arg );
    return( ret );
}
#endif

static bool scanString( char *buf, const char *str, unsigned len )
/*****************************************************************/
{
    bool        have_quote;

    have_quote = FALSE;
    while( isspace( *str ) ) str++;
    while( *str != '\0' && len > 0 ) {
        if( *str == '\"' ) {
            have_quote = !have_quote;
        } else {
            *buf = *str;
            buf++;
            len--;
        }
        str++;
    }
    *buf = '\0';
    return( have_quote );
}

void AddNewIncludeDirs( const char * arg )
/****************************************/
{
    int     len;
    int     oldlen;

    len = strlen( arg );
    if (len == 0) {
        return;
    }

    if (NewIncludeDirs == NULL) {
        /* + 1 for the '\0' */
        NewIncludeDirs = RcMemMalloc( len + 1 );
        NewIncludeDirs[ 0 ] = '\0';
        oldlen = 0;
    } else {
        /* + 2 for the '\0' and the ';' */
        oldlen = strlen( NewIncludeDirs );
        NewIncludeDirs = RcMemRealloc( NewIncludeDirs, oldlen + len + 2 );
        strcat( NewIncludeDirs + oldlen , PATH_SPLIT_S );
        oldlen ++; //for the semicolon
    }
    if( scanString( NewIncludeDirs + oldlen, arg, len + 1 ) ) {
        RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
    }
//    NewIncludeDirs = fixNewDirs( NewIncludeDirs );
}
