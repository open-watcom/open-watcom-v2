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
#include <stdlib.h>
#include <string.h>
#include "vi.h"
#ifdef __WIN__
#include "winvi.h"
#include "utils.h"
#endif

/*
 * ReadAFile - read a file into text
 */
int ReadAFile( linenum afterwhich, char *name )
{
    file        *cfile;
    char        *dir;
    int         i;
    long        bytecnt=0;
    linenum     lnecnt=0;
    int         lastst;
    char        *fn = MemAlloc(_MAX_PATH);

    /*
     * get file name
     */
    if( i = ModificationTest() ) {
        return( i );
    }
    if( NextWord1( name, fn ) <=0 || IsDirectory( fn ) ) {
        if( i > 0 ) {
            dir = fn;
        } else {
            dir = CurrentDirectory;
        }
        if( EditFlags.ExMode ) {
            return( ERR_INVALID_IN_EX_MODE );
        }
        i = SelectFileOpen( dir, &fn, "*", FALSE );
        if( i ) {
            MemFree( fn );
            return( i );
        }
        if( fn[0] == 0 ) {
            MemFree( fn );
            return( ERR_NO_ERR );
        }
    }

    /*
     * read directory
     */
    if( fn[0] == '$' ) {
        if( fn[1] == 0 ) {
            fn[1] = '*';
            fn[2] = 0;
        }
        GetSortDir( &fn[1], FALSE );
        cfile = FileAlloc( NULL );
        FormatDirToFile( cfile, FALSE );
    } else {
        cfile = FileAlloc( fn );
        /*
         * read all fcbs
         */
        lastst = UpdateCurrentStatus( CSTATUS_READING );
        #ifdef __WIN__
            ToggleHourglass( TRUE );
        #endif
        while( TRUE ) {
            i = ReadFcbData( cfile );
            lnecnt += cfile->fcb_tail->end_line - cfile->fcb_tail->start_line + 1L;
            bytecnt += (long) cfile->fcb_tail->byte_cnt;
            if( i ) {
                break;
            }
        }
        #ifdef __WIN__
            ToggleHourglass( FALSE );
        #endif
        UpdateCurrentStatus( lastst );
        if( i && i != END_OF_FILE ) {
            MemFree( fn );
            return( i );
        }
        bytecnt += lnecnt;

    }

    /*
     * add lines to current file
     */
    i = InsertLines( afterwhich, cfile->fcb_head, cfile->fcb_tail, UndoStack );
    FileFree( cfile );
    if( i ) {
        MemFree( fn );
        return( i );
    }

    DCDisplayAllLines();

    if( fn[0] == '$' ) {
        Message1( "Directory %s read", &fn[1] );
    } else {
        FileIOMessage( fn, lnecnt, bytecnt );
    }
    MemFree( fn );
    return( ERR_NO_ERR );

} /* ReadAFile */
