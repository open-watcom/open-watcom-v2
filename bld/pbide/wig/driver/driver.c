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


#include "windows.h"
#include <stdio.h>
#include <stdlib.h>
#include "driver.h"
#include "pbide.h"

static char     sruName[ _MAX_PATH ];
static char     dllName[ _MAX_PATH ];
static char     parentName[ _MAX_PATH ];

BOOL CALLBACK DriverDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    WatIDEErrInfo       *errinfo;
    char                buffer[256];
    BOOL                rc;

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        SetDlgItemText( hwnd, ID_SRU_NAME, "d:\\dev\\viper\\pb\\wig\\driver\\test\\uo_test.sru" );
        SetDlgItemText( hwnd, ID_DLL_NAME, "d:\\dev\\viper\\pb\\wig\\driver\\test\\uo_test.dll" );
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case ID_DONE:
            EndDialog( hwnd, 0 );
            break;
        case ID_CHECK_DIR:
            GetDlgItemText( hwnd, ID_DLL_NAME, dllName, _MAX_PATH );
            if( WatIDE_DirIsOk( dllName ) ) {
                MessageBox( NULL, "This Directory is OK", "check dir", MB_OK );
            } else {
                MessageBox( NULL,
                        "This Directory is contains files that could cause problems",
                        "check dir", MB_OK );
            }
            break;
        case ID_GEN_CODE:
            GetDlgItemText( hwnd, ID_SRU_NAME, sruName, _MAX_PATH );
            GetDlgItemText( hwnd, ID_PARENT_NAME, parentName, _MAX_PATH );
            if( *parentName == '\0' ) {
                rc = WatIDE_RunWig( sruName, NULL, &errinfo );
            } else {
                rc = WatIDE_RunWig( sruName, parentName, &errinfo );
            }
            if( rc ) {
                    MessageBox( hwnd, "Error Occurred", "", MB_OK );
            } else {
                    MessageBox( hwnd, "Success!", "", MB_OK );
            }
            if( errinfo != NULL ) {
                sprintf( buffer, "%d Errors, %d Warnings", errinfo->errcnt,
                         errinfo->warncnt );
                MessageBox( hwnd, buffer, "", MB_OK );
                MessageBox( hwnd, errinfo->errors, "Error Text", MB_OK );
            }
            break;
        case ID_RUN_IDE:
            GetDlgItemText( hwnd, ID_DLL_NAME, dllName, _MAX_PATH );
            if( WatIDE_RunIDE( dllName ) ) {
                MessageBox( hwnd, "The IDE was not started", "Error", MB_OK );
            } else {
//              MessageBox( hwnd, "The IDE started successfully", "", MB_OK );
            }
            break;
        case ID_CLOSE_IDE:
            if( WatIDE_CloseIDE() ) {
                MessageBox( hwnd, "The IDE was not closed", "Error", MB_OK );
            } else {
                MessageBox( hwnd, "The IDE closed successfully", "", MB_OK );
            }
            break;
        }
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}

int PASCAL WinMain( HANDLE currinst, HANDLE previnst, LPSTR cmdline, int cmdshow)
{
    FARPROC             fp;
    unsigned            ver;

    currinst = currinst;
    previnst = previnst;
    cmdshow = cmdshow;
    cmdline = cmdline;

    ver = WatIDE_GetVersion();
    if( ver != WAT_IDE_DLL_CUR_VER ) {
        MessageBox( NULL, "Wrong DLL version", "Error", MB_OK );
    } else {
        fp = MakeProcInstance( DriverDlgProc, currinst );
        DialogBox( currinst, "DRIVER_DLG", NULL, fp );
        FreeProcInstance( fp );
    }
    return( 0 );
}
