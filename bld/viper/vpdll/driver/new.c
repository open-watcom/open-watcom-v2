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


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "new.h"
#include "vpdll.h"

#define MSG_LEN         60
#define BUFLEN          150

static FunctionPtrs     Ptrs;
static HANDLE           Dll_Hdl;

BOOL CALLBACK DllNewDlgProc( HWND hwndDlg, UINT msg, UINT wParam, DWORD lParam )
{
    WORD                cmd;
    char                buf[BUFLEN];
    UINT                num;

    switch( msg ){
    case WM_COMMAND:
        cmd = LOWORD( wParam );
        switch( cmd ) {
        case ID_BEGIN_LIST:
            if( Ptrs.VPDLL_BeginFileList() ) {
                MessageBox( hwndDlg, "BeginFileList failed", "", MB_OK );
            }
            break;
        case ID_END_LIST:
            if( Ptrs.VPDLL_EndFileList() ) {
                MessageBox( hwndDlg, "EndFileList failed", "", MB_OK );
            }
            break;
        case ID_ADD:
            num = GetDlgItemText( hwndDlg, ID_EDIT, buf, BUFLEN );
            if( Ptrs.VPDLL_AddFile( buf ) ) {
                MessageBox( hwndDlg, "AddFile failed", "", MB_OK );
            }
            break;
        case ID_RETURN:
            Ptrs.VPDLL_ReturnToIDE( );
            break;
        case ID_DONE:
            EndDialog( hwndDlg, 0 );
            Ptrs.VPDLL_VPDone( );
            break;
        }
    break;
    default:
        return( 0 );
    }
    return( TRUE );
}

BOOL PASCAL __export CBFunction( DWORD msg, DWORD parm1, DWORD parm2 )
{
    char        buf[512];
    int         rc;

    switch( msg ){
    case VPDLL_GEN_CODE:
        if( parm1 ) {
            rc = MessageBox( NULL, "VPDLL_GEN_CODE (query)\nGenerate an error?", "CALLBACK MESSAGE", MB_YESNO );
        } else {
            rc = MessageBox( NULL, "VPDLL_GEN_CODE (don't query)\nGenerate an error?", "CALLBACK MESSAGE", MB_YESNO );
        }
        break;
    case VPDLL_NEW_PROJECT:
        sprintf( buf, "VPDLL_NEW_PROJECT '%s'\nGenerate an error?", (char *) parm1 );
        rc = MessageBox( NULL, buf, "CALLBACK MESSAGE", MB_YESNO );
        break;
    case VPDLL_SHUT_DOWN:
        rc = MessageBox( NULL, "VPDLL_SHUT_DOWN", "CALLBACK MESSAGE", MB_OK);
        return( FALSE );
        break;
    case VPDLL_TO_FRONT:
        rc = MessageBox( NULL, "VPDLL_TO_FRONT", "CALLBACK MESSAGE", MB_OK );
        return( FALSE );
        break;
    default:
        MessageBox( NULL, "invalid callback message", "ERROR", MB_OK );
        return( FALSE );
    }
    return( rc == IDYES );
}

void GetFunctions( void )
{
    Ptrs.VPDLL_GetVersion = (void*)GetProcAddress( Dll_Hdl,
                                                     "VPDLL_GetVersion" );
    Ptrs.VPDLL_SetCallBack = (void*)GetProcAddress( Dll_Hdl,
                                                     "VPDLL_Init" );
    Ptrs.VPDLL_ReturnToIDE = (void*)GetProcAddress( Dll_Hdl,
                                                     "VPDLL_ReturnToIde" );
    Ptrs.VPDLL_AddFile = (void*)GetProcAddress( Dll_Hdl,
                                                     "VPDLL_AddFile" );
    Ptrs.VPDLL_VPDone = (void*)GetProcAddress( Dll_Hdl,
                                                     "VPDLL_VPDone" );
    Ptrs.VPDLL_BeginFileList = (void*)GetProcAddress( Dll_Hdl,
                                                     "VPDLL_BeginFileList" );
    Ptrs.VPDLL_EndFileList = (void*)GetProcAddress( Dll_Hdl,
                                                     "VPDLL_EndFileList" );
}

int PASCAL WinMain( HANDLE currinst, HANDLE previnst, LPSTR cmdline, int cmdshow)
{
    char                str[MSG_LEN];
    char                *target;
    FARPROC             fp;

//    if( *cmdline == '\0' ) cmdline = "d:\\dev\\viper\\vpdll\\dll\\vpdll.dll";
    cmdline = "d:\\dev\\viper\\vpdll\\dll\\vpdll.dll";
    target = cmdline;
    while( isspace( *target ) ) target++;
    while( !isspace( *target ) && *target != '\0' ) target ++;
    if( *target != '\0' ) {
        *target = '\0';
        target ++;
    }
    while( isspace( *target ) ) target ++;


    Dll_Hdl = LoadLibrary( cmdline );
    if( Dll_Hdl < 32 ) {
        sprintf( str, "Unable to load '%s'.", cmdline );
        MessageBox( NULL, str, "new", MB_OK );
        return( 0 );
    }
    GetFunctions( );
    if( Ptrs.VPDLL_GetVersion() != VPDLL_VERSION ) {
        MessageBox( NULL, "DLL is not current Version", "Error", MB_OK );
        return( 0 );
    }

    fp = MakeProcInstance( (FARPROC)CBFunction, currinst );
    if( Ptrs.VPDLL_SetCallBack( fp ) ) {
        MessageBox( NULL, "SetCallBack Failed", "", MB_OK );
        return( 0 );
    }
    DialogBox( currinst, "NEW_DLG", NULL, DllNewDlgProc );
    FreeLibrary( Dll_Hdl );
    FreeProcInstance( fp );
    return( 0 );
}
