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


#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include "restest.h"
#include "getnum.h"
#include "resname.h"

static DWORD    lastNum;
static char     *getNumCaption;

BOOL CALLBACK GetMsgNumDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    char        buf[128];

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        SetDlgItemText( hwnd, GET_NUM_PROMPT, getNumCaption );
        break;
    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            GetDlgItemText( hwnd, INPUT_FIELD, buf, sizeof( buf ) );
            lastNum = strtoul( buf, NULL, 16 );
            EndDialog( hwnd, 0 );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

void DisplayMessageTable( void ) {

    DLGPROC     fp;
    DWORD       lang;
    DWORD       id;
    char        buf[256];
    DWORD       ret;

    /* get language id */
    getNumCaption = "Enter the language id for the message in hex";
    fp = (DLGPROC)MakeProcInstance( GetMsgNumDlgProc, Instance );
    DialogBox( Instance, "GET_NUM_DLG" , NULL, fp );
    lang = lastNum;

    /* get msg number */
    getNumCaption = "Enter the message id for the message in hex";
    DialogBox( Instance, "GET_NUM_DLG" , NULL, fp );
    id = lastNum;
    FreeProcInstance( fp );
    ret = FormatMessage(
                FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
                GetModuleHandle( NULL ),
                id,
                lang,
                buf,
                sizeof( buf ),
                NULL );
    if( ret == 0 ) {
        sprintf( buf, "Language %08lX, Msg: %08lX not loaded.  err code: %ld",
                 lang, id, GetLastError() );
        MessageBox( NULL, buf, "Message Table Error" , MB_OK );
    } else {
        MessageBox( NULL, buf, "Message from Messagetable", MB_OK );
    }
}
