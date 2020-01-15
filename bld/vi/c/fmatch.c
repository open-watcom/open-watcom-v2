/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Wildcard file matching.
*
****************************************************************************/


#include "vi.h"
#include "walloca.h"
#include "rxsupp.h"


static regexp  *cRx = NULL;

/*
 * FileMatch - check if a file matches a wild card
 */
bool FileMatch( const char *name )
{
    return( RegExec( cRx, name, true ) );

} /* FileMatch */

/*
 * FileMatchInit - start file matching
 */
vi_rc FileMatchInit( const char *wild )
{
    char        *tomatch;
    int         i, j, len;

    RegExpAttrSave( 1, "." );

    /*
     * compute required size
     */
    j = 4;
    len = strlen( wild );
    for( i = 0; i < len; i++ ) {
        if( wild[i] == '?' ) {
            j += 2;
        } else if( wild[i] == '*' ) {
            j += 3;
        } else {
            j++;
        }
    }
    tomatch = alloca( j );

    /*
     * build match string
     */
    j = 0;
    tomatch[j++] = '^';
#ifdef __UNIX__
    tomatch[j++] = '@';     // case sensitive compare
#else
    tomatch[j++] = '~';     // case insensitive compare
#endif
    len = strlen( wild );
    for( i = 0; i < len; i++ ) {
        if( wild[i] == '?' ) {
            tomatch[j++] = '\\';
            tomatch[j++] = '.';
            continue;
        }
        if( wild[i] == '*' ) {
            tomatch[j++] = '\\';
            tomatch[j++] = '.';
        }
        tomatch[j++] = wild[i];
    }
    tomatch[j++] = '$';
    tomatch[j] = '\0';

    cRx = RegComp( tomatch );
    return( RegExpError );

} /* FileMatchInit */

/*
 * FileMatchFini - done with file matching
 */
void FileMatchFini( void )
{
    RegExpAttrRestore();
    if( cRx != NULL ) {
        MemFree( cRx );
        cRx = NULL;
    }

} /* FileMatchFini */
