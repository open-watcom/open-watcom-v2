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
#ifdef __WIN__
    #include "utils.h"
#endif

/*
 * ReadAFile - read a file into text
 */
vi_rc ReadAFile( linenum afterwhich, char *name )
{
    file        *cfile;
    char        *dir;
    int         len;
    long        bytecnt = 0;
    linenum     lnecnt = 0;
    status_type lastst;
    char        *fn = MemAlloc( FILENAME_MAX );
    vi_rc       rc;

    /*
     * get file name
     */
    rc = ModificationTest();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    len = NextWord1( name, fn );
    if( len <= 0 || IsDirectory( fn ) ) {
        if( len > 0 ) {
            dir = fn;
        } else {
            dir = CurrentDirectory;
        }
        if( EditFlags.ExMode ) {
            return( ERR_INVALID_IN_EX_MODE );
        }
        rc = SelectFileOpen( dir, &fn, "*", FALSE );
        if( rc != ERR_NO_ERR ) {
            MemFree( fn );
            return( rc );
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
            rc = ReadFcbData( cfile );
            lnecnt += cfile->fcbs.tail->end_line - cfile->fcbs.tail->start_line + 1L;
            bytecnt += (long) cfile->fcbs.tail->byte_cnt;
            if( rc != ERR_NO_ERR ) {
                break;
            }
        }
#ifdef __WIN__
        ToggleHourglass( FALSE );
#endif
        UpdateCurrentStatus( lastst );
        if( rc != ERR_NO_ERR && rc != END_OF_FILE ) {
            MemFree( fn );
            return( rc );
        }
        bytecnt += lnecnt;

    }

    /*
     * add lines to current file
     */
    rc = InsertLines( afterwhich, &cfile->fcbs, UndoStack );
    FileFree( cfile );
    if( rc != ERR_NO_ERR ) {
        MemFree( fn );
        return( rc );
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
