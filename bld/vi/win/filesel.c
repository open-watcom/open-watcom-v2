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


#include <string.h>
//#define INCLUDE_COMMDLG_H
#include "winvi.h"
#include <commdlg.h>
#include <dlgs.h>
#include <cderr.h>

static char *filterList = "C Files (*.c;*.h)\0*.c;*.h\0"
                          "C++ Files (*.cpp;*.hpp)\0*.cpp;*.hpp\0"
                          "Fortran Files (*.for;*.fi)\0*.for;*.fi\0"
                          "Asm Files (*.asm)\0*.asm\0"
                          "All Files (*.*)\0*.*\0"
                          "\0";
static char *FileNameList;

BOOL WINEXP OpenHook( HWND hwnd, int msg, UINT wparam, LONG lparam )
{
    int len;
    static OPENFILENAME *of;

    wparam = wparam;
    lparam = lparam;
    hwnd = hwnd;

    switch( msg ) {
    case WM_INITDIALOG:
        /* danger - not sure that lparam is guaranteed to be the of. struct */
        of = (OPENFILENAME *)lparam;
        // return( FALSE );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            len = SendDlgItemMessage( hwnd, edt1, WM_GETTEXTLENGTH, 0, 0 );
            if( len >= of->nMaxFile ) {
                FileNameList = MemAlloc( len + 1 );
                len = SendDlgItemMessage( hwnd, edt1, WM_GETTEXT, len+1, (LPARAM)FileNameList );
            }
        }
        break;
    }
    return( FALSE );
}

/*
 * SelectFileOpen - use common dialog file open to pick a file to edit
 */
int SelectFileOpen( char *dir, char **result, char *mask, bool want_all_dirs  )
{
    OPENFILENAME        of;
    BOOL                rc;
    static long         filemask = 1;

    #ifdef __NT__
    /* added to get around chicago crashing in the fileopen dlg */
    /* -------------------------------------------------------- */
    DWORD ver;
    bool is_chicago = FALSE;

    ver = GetVersion();
    if( !( ver < 0x80000000 ) && !( LOBYTE( LOWORD( ver ) ) < 4 ) ) {
        is_chicago = TRUE;
    }
    /* -------------------------------------------------------- */
    #endif

    mask = mask;
    want_all_dirs = want_all_dirs;
    *result[ 0 ] = 0;
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = Root;
    of.lpstrFilter = (LPSTR) filterList;
    of.lpstrDefExt = NULL;
    of.nFilterIndex = filemask;
    of.lpstrFile = *result;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = NULL;
    of.lpstrInitialDir = dir;
    #ifdef __NT__
    if( is_chicago ) {
        of.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    } else {
    #endif
        of.Flags = OFN_PATHMUSTEXIST | OFN_ENABLEHOOK |
                   OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
        of.lpfnHook = (LPVOID) MakeProcInstance( (LPVOID) OpenHook,
                                                 InstanceHandle );
    #ifdef __NT__
    }
    #endif
    rc = GetOpenFileName( &of );
    filemask = of.nFilterIndex;
    #ifdef __NT__
    if( is_chicago ) {
    #endif
    #ifndef __NT__
        FreeProcInstance( (LPVOID) of.lpfnHook );
    #endif
    #ifdef __NT__
    }
    #endif
    if( rc == FALSE && CommDlgExtendedError() == FNERR_BUFFERTOOSMALL ) {
        #ifdef __NT__
        if( !is_chicago ) {
        #endif
            MemFree( (char*)(of.lpstrFile) );
            *result = FileNameList;
        #ifdef __NT__
        }
        #endif
        #if 0
        MyBeep();
        Message1( "Please open files in smaller groups" );
        #endif
    }
    UpdateCurrentDirectory();
    return( ERR_NO_ERR );

} /* SelectFileOpen */

/*
 * SelectFileSave - use common dialog file open to pickname to save under
 */
int SelectFileSave( char *result )
{
    OPENFILENAME        of;
    int doit;

    assert( CurrentFile != NULL );

    strcpy( result, CurrentFile->name );
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = Root;
    of.lpstrFilter = (LPSTR) filterList;
    of.lpstrDefExt = NULL;
    of.nFilterIndex = 1L;
    of.lpstrFile = result;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = NULL;
    of.lpstrInitialDir = CurrentFile->home;
    of.Flags = OFN_PATHMUSTEXIST | OFN_ENABLEHOOK | OFN_OVERWRITEPROMPT |
               OFN_HIDEREADONLY | OFN_NOREADONLYRETURN;
    of.lpfnHook = (LPVOID) MakeProcInstance( (LPVOID) OpenHook, InstanceHandle );
    doit = GetSaveFileName( &of );
    #ifndef __NT__
    FreeProcInstance( (LPVOID) of.lpfnHook );
    #endif

    if( doit != 0 ) {
        UpdateCurrentDirectory();
        return( ERR_NO_ERR );
    } else {
        return( ERR_SAVE_CANCELED );
    }

} /* SelectFileSave */

/*
 * GetInitialFileName - prompt the user for an initial file
 */
char *GetInitialFileName( void )
{
    char        *path = MemAlloc( _MAX_PATH );
    char        *ptr;
    int         rc;

    CloseStartupDialog();
    path[0] = 0;
    rc = SelectFileOpen( "", &path, NULL, FALSE );
    ShowStartupDialog();
    if( rc == ERR_NO_ERR && path[0] != 0 ) {
        AddString( &ptr, path );
    } else {
        ptr = NULL;
    }
    MemFree( path );
    return( ptr );

} /* GetInitialFileName */

int SelectLineInFile( selflinedata *sfd )
{
    sfd = sfd;
    return( 1 );
}
