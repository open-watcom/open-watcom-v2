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
* Description:  File selection dialog (open/save).
*
****************************************************************************/


#include "vi.h"
#include <commdlg.h>
#include <dlgs.h>
#include <cderr.h>
#include <assert.h>
#include "wprocmap.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT UINT_PTR CALLBACK OpenHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

typedef UINT (CALLBACK *OPENHOOKTYPE)( HWND, UINT, WPARAM, LPARAM );

static char *filterList = "C/C++ Files (*.c;*.h;*.cpp;*.hpp;*.cxx;*.hxx;*.inl)\0*.c;*.h;*.cpp;*.hpp;*.cxx;*.hxx;*.inl\0"
                          "C Files (*.c;*.h)\0*.c;*.h\0"
                          "C++ Files (*.cpp;*.hpp;*.cxx;*.hxx;*.inl)\0*.cpp;*.hpp;*.cxx;*.hxx;*.inl\0"
                          "Fortran Files (*.for;*.fi;*.f;*.inc)\0*.for;*.fi;*.f;*.inc\0"
                          "Java(Script) Files (*.java;*.js)\0*.java;*.js\0"
                          "SQL Files (*.sql)\0*.sql\0"
                          "Batch Files (*.bat;*.cmd)\0*.bat;*.cmd\0"
                          "Basic (*.bas;*.frm;*.cls)\0*.bas;*.frm;*.cls\0"
                          "Perl Files (*.pl;*.cgi)\0*.pl;*.cgi\0"
                          "HTML Files (*.htm;*.html;*.xhtml)\0*.htm;*.html;*.xhtml\0"
                          "WML Files (*.wml)\0*.wml\0"
                          "GML Files (*.gml)\0*.gml\0"
                          "DBTest (*.tst)\0*.tst\0"
                          "Makefiles (makefile;*.mk;*.mif;*.mak)\0makefile;*.mk;*.mif;*.mak\0"
                          "Assembly Files (*.asm;*.inc)\0*.asm;*.inc\0"
                          "Resource Files (*.rc;*.rh;*.dlg)\0*.rc;*.rh;*.dlg\0"
                          "AWK Files (*.awk)\0*.awk\0"
                          "All Files (*.*)\0*.*\0"
                          "\0";
static char *FileNameList;

WINEXPORT UINT_PTR CALLBACK OpenHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int                         len;
#ifdef __WINDOWS_386__
    static OPENFILENAME __far   *of;
#else
    static OPENFILENAME         *of;
#endif

    wparam = wparam;
    lparam = lparam;
    hwnd = hwnd;

    switch( msg ) {
    case WM_INITDIALOG:
        /* danger - not sure that lparam is guaranteed to be the of. struct */
#ifdef __WINDOWS_386__
        of = (OPENFILENAME __far *)MK_FP32( (void *)lparam );
#else
        of = (OPENFILENAME *)lparam;
#endif
        // return( FALSE );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            len = SendDlgItemMessage( hwnd, edt1, WM_GETTEXTLENGTH, 0, 0 );
            if( len >= of->nMaxFile ) {
                FileNameList = MemAlloc( len + 1 );
                len = SendDlgItemMessage( hwnd, edt1, WM_GETTEXT, len + 1, (LPARAM)FileNameList );
            }
        }
        break;
    }
    return( FALSE );
}

/*
 * SelectFileOpen - use common dialog file open to pick a file to edit
 */
vi_rc SelectFileOpen( const char *dir, char **result, const char *mask, bool want_all_dirs )
{
    OPENFILENAME        of;
    bool                rc;
    static long         filemask = 1;
    bool                is_chicago = false;

#if defined( __NT__ ) && !defined( _WIN64 )
    /* added to get around chicago crashing in the fileopen dlg */
    /* -------------------------------------------------------- */
    if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 ) {
        is_chicago = true;
    }
    /* -------------------------------------------------------- */
#endif

    mask = mask;
    want_all_dirs = want_all_dirs;
    *result[0] = 0;
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = root_window_id;
    of.lpstrFilter = (LPSTR) filterList;
    of.lpstrDefExt = NULL;
    of.nFilterIndex = filemask;
    of.lpstrFile = *result;
    of.nMaxFile = FILENAME_MAX;
    of.lpstrTitle = NULL;
    of.lpstrInitialDir = dir;
    if( is_chicago ) {
        of.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    } else {
        of.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLEHOOK;
        of.lpfnHook = (LPOFNHOOKPROC)MakeOpenFileHookProcInstance( OpenHook, InstanceHandle );
    }
    rc = GetOpenFileName( &of ) != 0;
    filemask = of.nFilterIndex;
    if( !is_chicago ) {
        (void)FreeProcInstance( (FARPROC)of.lpfnHook );
    }
    if( !rc && CommDlgExtendedError() == FNERR_BUFFERTOOSMALL ) {
        if( !is_chicago ) {
            MemFree( (char*)(of.lpstrFile) );
            *result = FileNameList;
        }
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
vi_rc SelectFileSave( char *result )
{
    OPENFILENAME        of;
    int                 doit;
    bool                is_chicago = false;

    assert( CurrentFile != NULL );

    strcpy( result, CurrentFile->name );
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = root_window_id;
    of.lpstrFilter = (LPSTR) filterList;
    of.lpstrDefExt = NULL;
    of.nFilterIndex = 1L;
    of.lpstrFile = result;
    of.nMaxFile = FILENAME_MAX;
    of.lpstrTitle = NULL;
    of.lpstrInitialDir = CurrentFile->home;
#if defined( __NT__ ) && !defined( _WIN64 )
    if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 )
        is_chicago = true;
#endif
    if( is_chicago ) {
        of.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_EXPLORER;
    } else {
        of.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_ENABLEHOOK;
        of.lpfnHook = (LPOFNHOOKPROC)MakeOpenFileHookProcInstance( OpenHook, InstanceHandle );
    }
    doit = GetSaveFileName( &of );
    if( !is_chicago ) {
        (void)FreeProcInstance( (FARPROC)of.lpfnHook );
    }
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
    char        *path = MemAlloc( FILENAME_MAX );
    char        *ptr;
    vi_rc       rc;

    CloseStartupDialog();
    path[0] = 0;
    rc = SelectFileOpen( "", &path, NULL, false );
    ShowStartupDialog();
    if( rc == ERR_NO_ERR && path[0] != 0 ) {
        ptr = DupString( path );
    } else {
        ptr = NULL;
    }
    MemFree( path );
    return( ptr );

} /* GetInitialFileName */

vi_rc SelectLineInFile( selflinedata *sfd )
{
    sfd = sfd;
    return( ERR_NO_ERR );
}
