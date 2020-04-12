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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <windows.h>
#include "restest.h"
#include "resname.h"
#include "wclbproc.h"


static char dialogName[256];

INT_PTR CALLBACK GetDialogNameDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            GetDlgItemText( hwnd, INPUT_FIELD, dialogName, sizeof( dialogName ) );
            EndDialog( hwnd, 0 );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}


INT_PTR CALLBACK DispDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;
    wparam = wparam;
    switch( msg ) {
    case WM_COMMAND:
    {
#if(0)
        char    buf[256];

        sprintf( buf, "ID: %d", LOWORD( wparam ) );
        MessageBox( hwnd, buf, "Control Msg", MB_OK );
#endif
        break;
    }

    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

void DisplayDialog( void )
{
    DLGPROC     dlgproc;
    int         ret;
    char        buf[256];

    dlgproc = MakeProcInstance_DLG( GetDialogNameDlgProc, Instance );
    DialogBox( Instance, "GET_RES_NAME_DLG" , NULL, dlgproc );
    FreeProcInstance_DLG( dlgproc );
    dlgproc = MakeProcInstance_DLG( DispDialogDlgProc, Instance );
    ret = DialogBox( Instance, dialogName , NULL, dlgproc );
    FreeProcInstance_DLG( dlgproc );
    if( ret == -1 ) {
        sprintf( buf, "Can't Load Dialog %s", dialogName );
        Error( "dialog", buf );
    }
}
