/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <dlgs.h>
#include <cderr.h>
#include <assert.h>
#include "wclbproc.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT UINT_PTR CALLBACK OpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static char filterList[] = {
    #define VI_LANG_FILTER
    #define pick(enum,enumrc,name,namej,fname,desc,filter) desc " (" filter ")\0" filter "\0"
    #include "langdef.h"
    #undef pick
};

static char *FileNameList;

WINEXPORT UINT_PTR CALLBACK OpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    unsigned                    len;
#ifdef __WINDOWS_386__
    static OPENFILENAME __far   *of;
#else
    static OPENFILENAME         *of;
#endif

    (void)wparam; (void)lparam; (void)hwnd;

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
                len = SendDlgItemMessage( hwnd, edt1, WM_GETTEXT, len + 1, (LPARAM)(LPSTR)FileNameList );
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
#if defined( __NT__ ) && !defined( _WIN64 )
    bool                is_chicago;
#endif

    /* unused parameters */ (void)mask; (void)want_all_dirs;

#if defined( __NT__ ) && !defined( _WIN64 )
    /* added to get around chicago crashing in the fileopen dlg */
    /* -------------------------------------------------------- */
    is_chicago = ( (GetVersion() & 0x800000FF) >= 0x80000004 );
    /* -------------------------------------------------------- */
#endif

    *result[0] = '\0';
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = root_window_id;
    of.lpstrFilter = (LPSTR)filterList;
    of.lpstrDefExt = NULL;
    of.nFilterIndex = filemask;
    of.lpstrFile = *result;
    of.nMaxFile = FILENAME_MAX;
    of.lpstrTitle = NULL;
    of.lpstrInitialDir = dir;
#if defined( __NT__ ) && !defined( _WIN64 )
    if( is_chicago ) {
        of.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    } else {
#endif
        of.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLEHOOK;
        of.lpfnHook = MakeProcInstance_OFNHOOK( OpenOFNHookProc, InstanceHandle );
#if defined( __NT__ ) && !defined( _WIN64 )
    }
#endif
    rc = GetOpenFileName( &of ) != 0;
#if defined( __NT__ ) && !defined( _WIN64 )
    if( !is_chicago ) {
#endif
        FreeProcInstance_OFNHOOK( of.lpfnHook );
#if defined( __NT__ ) && !defined( _WIN64 )
    }
#endif
    filemask = of.nFilterIndex;
    if( !rc && CommDlgExtendedError() == FNERR_BUFFERTOOSMALL ) {
#if defined( __NT__ ) && !defined( _WIN64 )
        if( !is_chicago ) {
#endif
            MemFree( (char*)(of.lpstrFile) );
            *result = FileNameList;
#if defined( __NT__ ) && !defined( _WIN64 )
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
vi_rc SelectFileSave( char *result )
{
    OPENFILENAME        of;
    int                 doit;
#if defined( __NT__ ) && !defined( _WIN64 )
    bool                is_chicago;
#endif

    assert( CurrentFile != NULL );

#if defined( __NT__ ) && !defined( _WIN64 )
    /* added to get around chicago crashing in the fileopen dlg */
    /* -------------------------------------------------------- */
    is_chicago = ( (GetVersion() & 0x800000FF) >= 0x80000004 );
    /* -------------------------------------------------------- */
#endif
    strcpy( result, CurrentFile->name );
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = root_window_id;
    of.lpstrFilter = (LPSTR)filterList;
    of.lpstrDefExt = NULL;
    of.nFilterIndex = 1L;
    of.lpstrFile = result;
    of.nMaxFile = FILENAME_MAX;
    of.lpstrTitle = NULL;
    of.lpstrInitialDir = CurrentFile->home;
#if defined( __NT__ ) && !defined( _WIN64 )
    if( is_chicago ) {
        of.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_EXPLORER;
    } else {
#endif
        of.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_ENABLEHOOK;
        of.lpfnHook = MakeProcInstance_OFNHOOK( OpenOFNHookProc, InstanceHandle );
#if defined( __NT__ ) && !defined( _WIN64 )
    }
#endif
    doit = GetSaveFileName( &of );
#if defined( __NT__ ) && !defined( _WIN64 )
    if( !is_chicago ) {
#endif
        FreeProcInstance_OFNHOOK( of.lpfnHook );
#if defined( __NT__ ) && !defined( _WIN64 )
    }
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
    char        *path = MemAlloc( FILENAME_MAX );
    char        *ptr;
    vi_rc       rc;

    path[0] = '\0';
    rc = SelectFileOpen( "", &path, NULL, false );
    if( rc == ERR_NO_ERR && path[0] != '\0' ) {
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
