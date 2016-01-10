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
#include "filelist.h"
#include "wprocmap.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT BOOL CALLBACK FileListProc( HWND dlg, UINT msg, WPARAM w, LPARAM l );

static info *findInfo( char *file_name )
{
    info    *i;

    for( i = InfoHead; i != NULL; i = i->next ) {
        if( !strcmp( file_name, i->CurrentFile->name ) ) {
            break;
        }
    }
    return( i );
}

static bool applyToSelectedList( HWND list_box, bool (*func)( info * ) )
{
    int     count, i;
    info    *info;
    char    *name;

    count = (int)SendMessage( list_box, LB_GETCOUNT, 0, 0L );
    for( i = 0; i < count; i++ ) {
        if( SendMessage( list_box, LB_GETSEL, i, 0L ) ) {
            name = MemAlloc( SendMessage( list_box, LB_GETTEXTLEN, i, 0L ) + 1 );
            SendMessage( list_box, LB_GETTEXT, i, (LPARAM)name );
            info = findInfo( name );
            MemFree( name );
            if( func( info ) ) {
                break;
            }
        }
    }
    return( true );
}

static bool doGoto( info *i )
{
    BringUpFile( i, true );
    return( true );
}

static bool doClose( info *i )
{
    vi_rc   rc;

    BringUpFile( i, true );
    rc = CurFileExitOptionSaveChanges();
    if( rc > ERR_NO_ERR || InfoHead == NULL ) {
        return( true );
    }
    return( false );
}

static int fillBox( HWND list_box )
{
    info    *i;
    int     count;

    count = 0;
    SendMessage( list_box, LB_RESETCONTENT, 0, 0L );
    for( i = InfoHead; i != NULL; i = i->next ) {
        SendMessage( list_box, LB_ADDSTRING, 0, (LPARAM)i->CurrentFile->name );
        count++;
    }
    SendMessage( list_box, LB_SETSEL, TRUE, 0L );
    return( count );
}

WINEXPORT BOOL CALLBACK FileListProc( HWND dlg, UINT msg, WPARAM wparam, LPARAM lparam )
{
    HWND    list_box;
    bool    (*func)( info * );

#ifdef __NT__
    lparam = lparam;
#endif
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( dlg );
        /* fill the list box with the names of the open files */
        fillBox( GetDlgItem( dlg, ID_FILE_LIST ) );
        return( TRUE );
    case WM_COMMAND:
        list_box = GetDlgItem( dlg, ID_FILE_LIST );
        func = NULL;
        switch( LOWORD( wparam ) ) {
        case ID_GOTO:
            func = doGoto;
            break;
        case ID_CLOSE:
            func = doClose;
            break;
        case IDOK:
        case IDCANCEL:
            EndDialog( dlg, ERR_NO_ERR );
            break;
        case ID_FILE_LIST:
            if( GET_WM_COMMAND_CMD( wparam, lparam ) == LBN_DBLCLK ) {
                func = doGoto;
            }
            break;
        default:
            return( FALSE );
        }
        if( func != NULL ) {
            if( applyToSelectedList( list_box, func ) ) {
                EndDialog( dlg, ERR_NO_ERR );
            }
        }
        return( TRUE );
    }
    return( FALSE );
}

vi_rc EditFileFromList( void )
{
    FARPROC     proc;
    vi_rc       rc;

    proc = MakeDlgProcInstance( FileListProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "FILELIST", Root, (DLGPROC)proc );
    FreeProcInstance( proc );
    return( rc );
}
