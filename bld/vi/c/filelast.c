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


#include "vi.h"
#ifndef __WIN__
#include "viuihelp.h"
#endif

#include "clibext.h"


/*
 * UpdateLastFilesList - update the list of the last files edited
 */
void UpdateLastFilesList( char *fname )
{
    history_data        *h;
    int                 i;
    char                buff[FILENAME_MAX];

    // don't add viw-generated filenames
    if( strcmp( fname, "no_name" ) == 0 || strncmp( fname, "untitled", 8 ) == 0 ) {
        return;
    }

#ifndef __WIN__
    // don't add help files
    for( i = 0; i < nHelpFiles; i++ ) {
        const char  *root;

        root = fname + strlen( fname ) - 1;
        while( root != fname && *root != '\\' ) {
            root--;
        }
        if( *root == '\\' ) {
            root++;
        }
        if( strcmp( root, helpFiles[i] ) == 0 ) {
            return;
        }
    }
#endif

    _fullpath( buff, fname, FILENAME_MAX );

    // if name already in list, dont add it.
    h = &EditVars.Hist[HIST_LASTFILES];
    for( i = 0; i < h->max; i++ ) {
        if( h->data[i] != NULL ) {
            if( stricmp( buff, h->data[i] ) == 0 ) {
                return;
            }
        }
    }

    // add in the next available spot
    ReplaceString( &(h->data[h->curr % h->max]), buff );
    h->curr += 1;

} /* UpdateLastFilesList */
