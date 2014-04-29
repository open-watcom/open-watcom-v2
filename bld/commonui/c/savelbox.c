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
* Description:  Wrapper around Windows file open/save dialog.
*
****************************************************************************/


#include "precomp.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <ctype.h>
#include <direct.h>
#include <stdlib.h>
#include "watcom.h"
#include "savelbox.h"
#ifndef NOUSE3D
    #include "ctl3dcvr.h"
#endif
#include "ldstr.h"
#include "uistr.gh"
#include "wprocmap.h"

/*
 * writeListBoxContents
 */
static BOOL writeListBoxContents( void (*writefn)( FILE * ), char *fname, HWND listbox )
{
    WORD        i;
    LRESULT     count;
    FILE        *f;
    char        str[256];

    f = fopen( fname, "w" );
    if( f == NULL ) {
        return( FALSE );
    }
    if( writefn != NULL ) {
        writefn( f );
    }
    count = SendMessage( listbox, LB_GETCOUNT, 0, 0L );
    if( count == LB_ERR ) {
        fclose( f );
        return( FALSE );
    }
    for( i = 0; i < count; i++ ) {
        SendMessage( listbox, LB_GETTEXT, i, (LPARAM)str );
        fprintf( f, "%s\n", str );
    }
    fclose( f );
    return( TRUE );

} /* writeListBoxContents */

#ifndef NOUSE3D

/*
 * LBSaveHook - hook used called by common dialog - for 3D controls
 */
WINEXPORT UINT_PTR CALLBACK LBSaveHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
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
        CvrCtl3dSubclassDlg( hwnd, CTL3D_ALL );
        return( TRUE );
    }
    return( FALSE );

} /* LBSaveHook */

#endif

/*
 * GetSaveFName - let the user select a file name for a save operation
 *                fname must point to a buffer of length at least _MAX_PATH
 */
BOOL GetSaveFName( HWND mainhwnd, char *fname )
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
    of.lpfnHook = (LPOFNHOOKPROC)MakeOpenFileHookProcInstance( LBSaveHook, GET_HINSTANCE( mainhwnd ) );
#endif
    rc = GetSaveFileName( &of );
#ifndef NOUSE3D
    FreeProcInstance( (FARPROC)of.lpfnHook );
#endif
    return( rc );

} /* GetSaveFName */

/*
 * GenTmpFileName - generate a unique file name based on tmpname
 */
BOOL GenTmpFileName( char *tmpname, char *buf )
{
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];
    char        *ptr;
    size_t      len;
    size_t      fname_len;
    unsigned    i;
    char        id[4];

    _splitpath( tmpname, drive, dir, fname, ext );
    len = 0;
    strcpy( buf, drive );
    len += strlen( drive );
    strcpy( buf + len, dir );
    len += strlen( dir );
    strcpy( buf + len, fname );
    fname_len = strlen( fname );
    if( fname_len < _MAX_FNAME - 4 ) {
        ptr = buf + len + fname_len;
        len = len + fname_len + 3;
    } else {
        ptr = buf + len + _MAX_FNAME - 4;
        len = len + _MAX_FNAME - 1;
    }
    strcpy( buf + len, ext );
    for( i = 0;; ) {
        sprintf( id, "%03d", i );
        memcpy( ptr, id, 3 );
        if( access( buf, F_OK ) == -1 ) {
            break;
        }
        i++;
        if( i > 999 ) {
            return( FALSE );
        }
    }
    return( TRUE );

} /* GenTmpFileName */

/*
 * relToAbs - converts a relative path to an absolute path based on the
 *            current working directory
 *          - assumes that the path given is valid
 */
static void relToAbs( char *path, char *out )
{
    char        *cwd;
    unsigned    old_dir;
    unsigned    tot;
    char        dir[_MAX_DIR];
    char        drive[_MAX_DRIVE];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];
    char        *ptr;

    cwd = getcwd( NULL, 0 );
    _dos_getdrive( &old_dir );
    _splitpath( path, drive, dir, fname, ext );
    if( strcmp( dir, "\\" ) != 0 ) {
        if( *dir != '\0' ) {
            ptr = dir;
            while( *ptr ) {
                ptr++;
            }
            ptr--;
            *ptr = '\0';
        }
    }
    _dos_setdrive( toupper( drive[0] ) - 'A' + 1, &tot );
    chdir( dir );
    getcwd( out, _MAX_PATH );
    _dos_setdrive( old_dir, &tot );
    chdir( cwd );
    free( cwd );
    ptr = out;
    /*
     * Make sure _splitpath doesn't mistake the last directory spec as a
     * filename.
     */
    while( *ptr ) {
        ptr++;
    }
    if( *(ptr - 1) != '\\' ) {
        ptr[0] = '\\';
        ptr++;
    }
    strcpy( ptr, "a" );
    strupr( fname );
    strupr( ext );
    _splitpath( out, drive, dir, NULL, NULL );
    _makepath( out, drive, dir, fname, ext );

} /* relToAbs */

/*
 * ReportSave
 */
void ReportSave( HWND parent, char *fname, char *appname, BOOL save_ok )
{
    char        ful_fname[_MAX_PATH];
    char        buf[_MAX_PATH + 20];

    if( save_ok ) {
        relToAbs( fname, ful_fname );
        RCsprintf( buf, SLB_DATA_SAVED_TO, ful_fname );
        MessageBox( parent, buf, appname, MB_OK | MB_TASKMODAL );
    } else {
        RCMessageBox( parent, SLB_CANT_SAVE_DATA, appname,
                      MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION );
    }

} /* ReportSave */

/*
 * SaveListBox - save out a list box
 */
void SaveListBox( int how, void (*writefn)( FILE * ), char *tmpname,
                  char *appname, HWND mainhwnd, HWND listbox )
{
    char        fname[_MAX_PATH];
    BOOL        ret;
    HCURSOR     hourglass;
    HCURSOR     oldcursor;

    if( how == SLB_SAVE_AS ) {
        ret = GetSaveFName( mainhwnd, fname );
    } else {
        ret = GenTmpFileName( tmpname, fname );
        if( !ret ) {
            ReportSave( mainhwnd, fname, appname, ret );
        }
    }
    if( ret ) {
        hourglass = LoadCursor( NULL, IDC_WAIT );
        SetCapture( mainhwnd );
        oldcursor = SetCursor( hourglass );
        ret = writeListBoxContents( writefn, fname, listbox );
        SetCursor( oldcursor );
        ReleaseCapture();
        ReportSave( mainhwnd, fname, appname, ret );
    }

} /* SaveListBox */
