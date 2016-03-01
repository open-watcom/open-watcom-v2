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


#include <stdlib.h>
#include "bool.h"
#include "drwatcom.h"
#include "notelog.h"
#include "jdlg.h"
#define BUF_SIZE        100

BOOL __export CALLBACK NoteLogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD        cmd;
    int         linecnt;
    int         i;
    HWND        edithwnd;
    char        buf[BUF_SIZE];
    LRESULT     len;
    WORD        *wptr;
    void        (*fn)(char *);

    switch( msg ) {
    case WM_INITDIALOG:
        SetWindowLong( hwnd, DWL_USER, lparam );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case IDOK:
            fn = (void *)GetWindowLong( hwnd, DWL_USER );
            edithwnd = GetDlgItem( hwnd, LOG_TEXT );
            linecnt = (int)SendMessage( edithwnd, EM_GETLINECOUNT, 0, 0L );
            for( i = 0; i < linecnt; i++ ) {
                wptr = (WORD *) buf;
                *wptr = BUF_SIZE - 1;
                len = SendMessage( edithwnd, EM_GETLINE, i, (LPARAM)buf );
                buf[ len ] = '\0';
                if( len > 0 ) {
                    fn( buf );
                }
            }
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case IDCANCEL:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}

void AnotateLog( HWND hwnd, HANDLE Instance, void (*fn)(char *)  ) {

    FARPROC     fp;

    fp = MakeProcInstance( (FARPROC)NoteLogDlgProc, Instance );
    JDialogBoxParam( Instance, "NOTE_LOG", hwnd, (DLGPROC)fp, (LPARAM)fn );
    FreeProcInstance( fp );
}
