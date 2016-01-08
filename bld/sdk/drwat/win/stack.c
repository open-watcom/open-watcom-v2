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
#include <stdio.h>
#include <dos.h>
#include "bool.h"
#include "wdebug.h"
#include "drwatcom.h"
#include "jdlg.h"

typedef struct x {
ADDRESS dispaddr;
ADDRESS faddr;
STACKTRACEENTRY ste;
} stdata;

stdata  std;

/*
 * STDialog - show a stack frame
 */
BOOL __export FAR PASCAL STDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    int         i;

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        {
            char        buff[256];
            MODULEENTRY me;
            ADDRESS     addr;
            syminfo     si;

            if( MyModuleFindHandle( &me, std.ste.hModule ) ) {
                RCsprintf( buff, STR_STACK_TRACE, me.szModule );
                SetWindowText( hwnd, buff );
            }
            SetDlgCourierFont( hwnd, ST_CSIP );
            SetDlgCourierFont( hwnd, ST_SSBP );
            sprintf( buff, "CS:IP = %04x:%04x", std.ste.wCS, std.ste.wIP  );
            SetDlgItemText( hwnd, ST_CSIP, buff );
            sprintf( buff, "SS:BP = %04x:%04x", std.ste.wSS, std.ste.wBP  );
            SetDlgItemText( hwnd, ST_SSBP, buff );

            addr.seg = std.ste.wCS;
            addr.offset = std.ste.wIP;
            if( FindWatSymbol( &addr, &si, TRUE ) == FOUND ) {
                RCsprintf( buff, STR_SRC_INFO_FMT, si.linenum, si.filename );
            } else {
                RCsprintf( buff, STR_N_A );
            }
            SetDlgMonoFont( hwnd, ST_SOURCE_INFO );
            SetDlgItemText( hwnd, ST_SOURCE_INFO, buff );
        }

        /*
         * display some code
         */
        std.dispaddr.seg = std.ste.wCS;
        std.dispaddr.offset = std.ste.wIP;
        std.faddr = std.dispaddr;
        InstructionBackup( 3, &std.dispaddr );
        for( i=ST_DISASM1;i<=ST_DISASM8;i++ ) {
            SetDlgCourierFont( hwnd, i );
        }
        DisplayAsmLines( hwnd, &std.dispaddr, &std.faddr, ST_DISASM1,
                          ST_DISASM8, ST_SCROLL );
        return( TRUE );
        break;

    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, ST_CANCEL, 0L );
        return( TRUE );

    case WM_COMMAND:
        switch( wparam ) {
        case ST_NEXT:
        case ST_PREVIOUS:
        case ST_CANCEL:
            EndDialog( hwnd, wparam );
            break;
        }
        return( TRUE );

    case WM_VSCROLL:
        ScrollAsmDisplay( hwnd, wparam, &std.dispaddr, &std.faddr,
                                ST_DISASM1, ST_DISASM8, ST_SCROLL );
        return( TRUE );
        break;
    }
    return( FALSE );

} /* STDialog */

/*
 * StartStackTraceDialog - do the stack trace dialog
 */
void StartStackTraceDialog( HWND hwnd )
{
    FARPROC     fp;
    BOOL        first_try;
    INT_PTR     rc;
    int         currframe=0;
    int         oldcurrframe=0;
    int         i;

//    rc = (INT_PTR)ST_NEXT;
    rc = ST_NEXT;
    first_try = TRUE;
    while( 1 ) {
        std.ste.hTask = DeadTask;
        if( !MyStackTraceCSIPFirst( &std.ste, IntData.SS, IntData.CS,
                    (WORD) IntData.EIP, (WORD) IntData.EBP ) ) {
            RCMessageBox( hwnd, STR_NO_STACK_FRAMES_FOUND, AppName, MB_OK );
            return;
        }
        for( i=0;i<=currframe;i++ ) {
            if( !MyStackTraceNext( &std.ste ) || !IsValidSelector( std.ste.wCS ) ) {
                if( first_try ) {
                    RCMessageBox( hwnd, STR_NO_STACK_FRAMES_FOUND,
                                  AppName, MB_OK );
                    return;
                } else {
                    RCMessageBox( hwnd, STR_NO_MORE_STACK_FRAMES,
                                  AppName, MB_OK );
                    currframe = oldcurrframe;
                    continue;
                }
            }
            first_try = FALSE;
        }
        fp = MakeProcInstance( (FARPROC)STDialog, Instance );
        rc = JDialogBox( Instance, "STACKTRACE", hwnd, (DLGPROC)fp );
        FreeProcInstance( fp );
        oldcurrframe = currframe;
        if( rc == ST_NEXT ) {
            currframe++;
        } else if( rc == ST_PREVIOUS ) {
            currframe--;
            if( currframe < 0 ) {
                currframe = 0;
                RCMessageBox( hwnd, STR_NO_MORE_STACK_FRAMES,
                              AppName, MB_OK );
            }
        } else {
            break;
        }
    }

} /* StartStackTraceDialog */
