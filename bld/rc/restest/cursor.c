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
* Description:  Display a cursor resource. 
*
****************************************************************************/


#include <stdio.h>
#include <dos.h>
#include <windows.h>
#include "restest.h"
#include "resname.h"

static char cursorName[256];

BOOL CALLBACK GetCursorNameDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            GetDlgItemText( hwnd, INPUT_FIELD, cursorName, sizeof( cursorName ) );
            EndDialog( hwnd, 0 );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

void DisplayCursor( HWND hwnd )
{
    FARPROC     fp;
    HCURSOR     oldcur;
    HCURSOR     newcur;
    char        buf[256];

    fp = MakeProcInstance( (FARPROC)GetCursorNameDlgProc, Instance );
    DialogBox( Instance, "GET_RES_NAME_DLG" , NULL, (DLGPROC)fp );
    FreeProcInstance( fp );
    newcur = LoadCursor( Instance, cursorName );
    if( newcur == NULL ) {
        sprintf( buf, "Can't Load Cursor %s", cursorName );
        Error( "cursor", buf );
        return;
    }
    MessageBox( NULL, "The new cursor will be displayed for\n5 seconds after you hit OK", "", MB_OK );
    oldcur = SetCursor( newcur );
    SetCapture( hwnd );
    Sleep( 5 );
    oldcur = SetCursor( oldcur );
    ReleaseCapture();
    DestroyCursor( newcur );
}
