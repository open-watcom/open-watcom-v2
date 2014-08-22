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
* Description:  Expand file name to a list.
*
****************************************************************************/


#include "vi.h"
#include "posix.h"
#include "clibext.h"

/*
 * ExpandFileNames - take a file name, and expand it out to a list of dos
 *                   file names
 */
int ExpandFileNames( char *p, char ***argv )
{
    int         argc, i;
    char        drive[_MAX_DRIVE], directory[_MAX_DIR], name[_MAX_FNAME];
    char        extin[_MAX_EXT], pathin[FILENAME_MAX];
    char        *start, *new;
    bool        wildcard;
    vi_rc       rc;

    argc = 0;
    wildcard = false;
    start = p;
    *argv = NULL;

    /*
     * check if there is anything to expand
     */
    for( ;; ) {
        if( *p == '\0' ) {
            break;
        }
        if( *p == '?'  ||  *p == '*' || *p == '|' || *p == '(' ||
                *p == '[' ) {
            wildcard = true;
            break;
        }
        p++;
    }

    if( !wildcard ) {
        // don't change to lowercase any more
        //FileLower( start );
        return( 0 );
    }

    /*
     * get all matches
     */
    rc = GetSortDir( start, false );
    if( rc != ERR_NO_ERR ) {
        return( 0 );
    }
    _splitpath( start, drive, directory, name, extin );

    /*
     * run through matches
     */
    for( i = 0; i < DirFileCount; i++ ) {
        if( DirFiles[i]->attr & (_A_VOLID + _A_SUBDIR) ) {
            continue;
        }
        _splitpath( DirFiles[i]->name, NULL, NULL, name, extin );
        _makepath( pathin, drive, directory, name, extin );
        *argv = MemReAlloc( *argv, (argc + 1) * sizeof( char * ) );
        new = MemAlloc( strlen( pathin ) + 1 );
        strcpy( new, pathin );
        (*argv)[argc++] = new;
    }

    return( argc );

} /* ExpandFileNames */
