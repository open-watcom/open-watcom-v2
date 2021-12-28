/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Wrapper around Windows file open/save dialog.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <direct.h>
#include <stdlib.h>
#include "wio.h"
#include "watcom.h"
#include "bool.h"
#include "savelbox.h"
#ifndef NOUSE3D
    #include "ctl3dcvr.h"
#endif
#include "ldstr.h"
#include "uistr.grh"
#include "wclbproc.h"
#include "pathgrp2.h"
#ifdef __NT__
    #undef _WIN32_IE
    #define _WIN32_IE   0x0400
    #include <commctrl.h>
#endif

#include "clibext.h"


/* Window callback functions prototypes */
WINEXPORT UINT_PTR CALLBACK LBSaveOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static bool isListView( HWND list )
{
#ifdef __WINDOWS__
    /* unused parameters */ (void)list;
    return( false );
#else
    char        tmp[20];
    int         len;

    len = GetClassName( list, tmp, sizeof( tmp ) );
    tmp[len] = '\0';
    return( stricmp( tmp, WC_LISTVIEW ) == 0 );
#endif
}

/*
 * writeListBoxContents
 */
static bool writeListBoxContents( void (*headerfn)(FILE *), char *(*linefn)(bool, HWND, int), char *fname, HWND listbox )
{
    int         i;
    int         count;
    FILE        *f;
    char        *str;
    bool        listview;

    f = fopen( fname, "wt" );
    if( f == NULL ) {
        return( false );
    }
    if( headerfn != NULL ) {
        headerfn( f );
    }
    if( linefn != NULL ) {
        listview = isListView( listbox );
#ifdef __NT__
        if( listview ) {
            count = (int)SendMessage( listbox, LVM_GETITEMCOUNT, 0, 0L );
        } else {
#endif
            count = (int)SendMessage( listbox, LB_GETCOUNT, 0, 0L );
            if( count == (int)LB_ERR ) {
                fclose( f );
                return( false );
            }
#ifdef __NT__
        }
#endif
        for( i = 0; i < count; i++ ) {
            str = linefn( listview, listbox, i );
            fprintf( f, "%s\n", str );
        }
    }
    fclose( f );
    return( true );

} /* writeListBoxContents */

#ifndef NOUSE3D

/*
 * LBSaveOFNHookProc - hook used called by common dialog - for 3D controls
 */
UINT_PTR CALLBACK LBSaveOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    wparam = wparam;
    lparam = lparam;
    hwnd = hwnd;
    switch( msg ) {
    case WM_INITDIALOG:
        /*
         * We must call this to subclass the directory listbox even
         * if the app calls Ctl3dAutoSubclass (commdlg bug).
         */
        CvrCtl3dSubclassDlgAll( hwnd );
        return( TRUE );
    }
    return( FALSE );

} /* LBSaveOFNHookProc */

#endif

/*
 * GetSaveFName - let the user select a file name for a save operation
 *                fname must point to a buffer of length at least _MAX_PATH
 */
bool GetSaveFName( HWND mainhwnd, char *fname )
{
    static char         filterList[] = "File (*.*)" \
                                       "\0" \
                                       "*.*" \
                                       "\0\0";
    OPENFILENAME        of;
    int                 rc;

    fname[0] = 0;
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = mainhwnd;
    of.lpstrFilter = (LPSTR)filterList;
    of.lpstrDefExt = "";
    of.nFilterIndex = 1L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = NULL;
    of.Flags = OFN_HIDEREADONLY;
#ifndef NOUSE3D
    of.Flags |= OFN_ENABLEHOOK;
    of.lpfnHook = MakeProcInstance_OFNHOOK( LBSaveOFNHookProc, GET_HINSTANCE( mainhwnd ) );
#endif
    rc = GetSaveFileName( &of );
#ifndef NOUSE3D
    FreeProcInstance_OFNHOOK( of.lpfnHook );
#endif
    return( rc != 0 );

} /* GetSaveFName */

/*
 * GenTmpFileName - generate a unique file name based on tmpname
 */
bool GenTmpFileName( const char *tmpname, char *buf )
{
    pgroup2     pg;
    char        *ptr;
    size_t      len;
    size_t      fname_len;
    unsigned    i;
    char        id[4];

    _splitpath2( tmpname, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    len = 0;
    strcpy( buf, pg.drive );
    len += strlen( pg.drive );
    strcpy( buf + len, pg.dir );
    len += strlen( pg.dir );
    strcpy( buf + len, pg.fname );
    fname_len = strlen( pg.fname );
    if( fname_len < _MAX_FNAME - 4 ) {
        ptr = buf + len + fname_len;
        len = len + fname_len + 3;
    } else {
        ptr = buf + len + _MAX_FNAME - 4;
        len = len + _MAX_FNAME - 1;
    }
    strcpy( buf + len, pg.ext );
    for( i = 0; i < 1000; i++ ) {
        sprintf( id, "%03d", i );
        memcpy( ptr, id, 3 );
        if( access( buf, F_OK ) == -1 ) {
            break;
        }
    }
    return( i < 1000 );

} /* GenTmpFileName */

/*
 * relToAbs - converts a relative path to an absolute path based on the
 *            current working directory
 *          - assumes that the path given is valid
 */
static void relToAbs( const char *path, char *out )
{
    char        *cwd;
    int         old_drive;
    pgroup2     pg1;
    pgroup2     pg2;
    size_t      len;

    /* remove directory separator on the path end */
    _splitpath2( path, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
    if( pg1.dir[0] != '\0' && ( pg1.dir[0] != '\\' || pg1.dir[1] != '\0' ) ) {
        pg1.dir[strlen( pg1.dir ) - 1] = '\0';
    }

    /* get file current directory on appropriate drive */
    old_drive = _getdrive();
    cwd = getcwd( NULL, 0 );
    _chdrive( toupper( pg1.drive[0] ) - 'A' + 1 );
    chdir( pg1.dir );
    getcwd( out, _MAX_PATH );
    _chdrive( old_drive );
    chdir( cwd );
    free( cwd );

    /*
     * Make sure _splitpath doesn't mistake the last directory spec as a
     * filename.
     */
    len = strlen( out );
    if( len > 0 && out[len - 1] != '\\' )
        out[len++] = '\\';
    out[len++] = 'a';   /* add fake file name for _splitpath2 */
    out[len] = '\0';
    _splitpath2( out, pg2.buffer, &pg2.drive, &pg2.dir, NULL, NULL );

    /* create absolute path for file */
    strupr( pg1.fname );
    strupr( pg1.ext );
    _makepath( out, pg2.drive, pg2.dir, pg1.fname, pg1.ext );

} /* relToAbs */

/*
 * ReportSave
 */
void ReportSave( HWND parent, const char *fname, const char *appname, bool save_ok )
{
    char        ful_fname[_MAX_PATH];
    char        buf[_MAX_PATH + 20];

    if( save_ok ) {
        relToAbs( fname, ful_fname );
        RCsprintf( buf, SLB_DATA_SAVED_TO, ful_fname );
        MessageBox( parent, buf, appname, MB_OK | MB_TASKMODAL );
    } else {
        RCMessageBox( parent, SLB_CANT_SAVE_DATA, appname, MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
    }

} /* ReportSave */

/*
 * SaveListBox - save out a list box
 */
void SaveListBox( int how, void (*headerfn)(FILE *), char *(*linefn)(bool, HWND, int), const char *tmpname,
                  const char *appname, HWND mainhwnd, HWND listbox )
{
    char        fname[_MAX_PATH];
    bool        ok;
    HCURSOR     hourglass;
    HCURSOR     oldcursor;

    if( how == SLB_SAVE_AS ) {
        ok = GetSaveFName( mainhwnd, fname );
    } else {
        ok = GenTmpFileName( tmpname, fname );
        if( !ok ) {
            ReportSave( mainhwnd, fname, appname, ok );
        }
    }
    if( ok ) {
        hourglass = LoadCursor( (HINSTANCE)NULL, IDC_WAIT );
        SetCapture( mainhwnd );
        oldcursor = SetCursor( hourglass );
        ok = writeListBoxContents( headerfn, linefn, fname, listbox );
        SetCursor( oldcursor );
        ReleaseCapture();
        ReportSave( mainhwnd, fname, appname, ok );
    }

} /* SaveListBox */
