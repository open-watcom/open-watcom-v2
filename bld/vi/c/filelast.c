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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vi.h"

extern char *helpFiles[1];
extern int  nHelpFiles;

/*
 * UpdateLastFileList - update the list of the last files edited
 */
void UpdateLastFileList( char *fname )
{
    history_data        *h;
    int                 i;
    char                *root;
    char                buff[_MAX_PATH];

    // don't add viw-generated filenames
    if( !strcmp( fname, "no_name" ) || !strncmp( fname, "untitled", 8 ) ) {
        return;
    }

    // don't add help files
    for( i = 0; i < nHelpFiles; i++ ) {
        root = fname + strlen( fname ) - 1;
        while( root != fname && *root != '\\' ) {
            root--;
        }
        if( *root == '\\' ) {
            root++;
        }
        if( !strcmp( root, helpFiles[ i ] ) ) {
            return;
        }
    }


    _fullpath(buff,fname,_MAX_PATH);


    // if name already in list, dont add it.
    h = &LastFilesHist;
    for( i = 0; i < h->max; i++ ) {
        if( h->data[i] ){
            if( !stricmp( buff, h->data[i] ) ) {
                return;
            }
        }
    }

    // add in the next available spot
    AddString2( &(h->data[ h->curr % h->max ] ), buff );
    h->curr += 1;

} /* UpdateLastFileList */
