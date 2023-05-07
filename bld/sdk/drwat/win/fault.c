/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "drwatcom.h"
#include <dos.h>
#include "wclbproc.h"
#include "wdebug.h"
#include "intdata.h"
#include "jdlg.h"


/* Local Window callback functions prototypes */
WINEXPORT INT_PTR CALLBACK IntDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static BOOL doLog;

/*
 * IntDialogDlgProc - handles input from user when a fault is received
 */
INT_PTR CALLBACK IntDialogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char        buff[256];
    WORD        tmp;
    bool        ret;

    lparam = lparam;

    ret = false;

    switch( msg ) {
    case WM_INITDIALOG:
        doLog = FALSE;
        CheckDlgButton( hwnd, ExceptionAction, BST_CHECKED );
        CopyRCString( STR_TERMINATE_TASK, buff, sizeof( buff ) );
        SetDlgItemText( hwnd, INT_TERMINATE, buff );
        SetDlgCourierFont( hwnd, INT_TASK_NAME );
        SetDlgCourierFont( hwnd, INT_TASK_PATH );
        SetDlgCourierFont( hwnd, INT_FAULT_TYPE );
        SetDlgCourierFont( hwnd, INT_CS_IP );
        SetDlgCourierFont( hwnd, INT_SOURCE_INFO );
        SetDlgCourierFont( hwnd, INT_SOURCE_INFO2 );
        RCsprintf( buff, STR_FAULT_X_ENCOUNTERED, AppName, IntData.InterruptNumber );
        SetWindowText( hwnd, buff );
        SetDlgItemText( hwnd, INT_TASK_NAME, DTTaskEntry.szModule );
        SetDlgItemText( hwnd, INT_TASK_PATH, DTModuleEntry.szExePath );
        GetFaultString( IntData.InterruptNumber, buff );
        SetDlgItemText( hwnd, INT_FAULT_TYPE, buff );
        if( IsWin32App ) {
            sprintf( buff, "%04x:%08lx", IntData.CS, IntData.EIP );
        } else {
            sprintf( buff, "%04x:%04x", IntData.CS, (WORD) IntData.EIP );
        }
        SetDlgItemText( hwnd, INT_CS_IP, buff );
        {
            ADDRESS     addr;
            syminfo     si;
            char        *na;

            addr.seg = IntData.CS;
            addr.offset = IntData.EIP;
            if( FindWatSymbol( &addr, &si, true ) ) {
                if( si.linenum > 0 ) {
                    RCsprintf( buff, STR_LINE, si.linenum );
                    SetDlgItemText( hwnd, INT_SOURCE_INFO2, buff );
                } else {
                    SetDlgItemText( hwnd, INT_SOURCE_INFO2, "" );
                }
                SetDlgItemText( hwnd, INT_SOURCE_INFO, si.filename );
            } else {
                na = AllocRCString( STR_N_A );
                SetDlgItemText( hwnd, INT_SOURCE_INFO, na );
                FreeRCString( na );
            }
        }
        ret = true;
        break;
    case WM_CLOSE:
        tmp = ExceptionAction;
        CheckDlgButton( hwnd, INT_TERMINATE, BST_CHECKED );
        SendMessage( hwnd, WM_COMMAND, INT_ACT, 0L );
        ExceptionAction = tmp;
        ret = true;
        break;
    case WM_COMMAND:
        switch( wparam ) {
        case INT_ACT_AND_LOG:
            doLog = TRUE;
            /* fall through */
        case INT_ACT:
            if( IsDlgButtonChecked( hwnd, INT_TERMINATE ) ) {
                ExceptionAction = INT_TERMINATE;
                EndDialog( hwnd, KILL_APP );
                ret = true;
            } else if( IsDlgButtonChecked( hwnd, INT_CHAIN_TO_NEXT ) ) {
                ExceptionAction = INT_CHAIN_TO_NEXT;
                EndDialog( hwnd, CHAIN );
                ret = true;
            } else if( IsDlgButtonChecked( hwnd, INT_RESTART ) ) {
                ExceptionAction = INT_RESTART;
                EndDialog( hwnd, RESTART_APP );
                ret = true;
            }
            break;
        case INT_LOG_OPTIONS:
            DoLogDialog( hwnd );
            ret = true;
            break;
        case INT_REGISTERS:
            DoStatDialog( hwnd );
            break;
        }
    }
    return( ret );

} /* IntDialogDlgProc */

/*
 * FaultHandler - C handler for a fault
 */
appl_action __cdecl __far FaultHandler( fault_frame ff )
{
    DLGPROC     dlgproc;
    appl_action appl_act;
    char        *fault_str;
    msg_id      faultid;

    WasFault32 = false;
    if( WDebug386 ) {
        if( IsDebuggerExecuting() ) {
            return( CHAIN );
        }
        WasFault32 = (bool)GetDebugInterruptData( &IntData );
        if( WasFault32 ) {
            ff.intf.intnumber = IntData.InterruptNumber;
            if( ff.intf.intnumber == WGOD_ASYNCH_STOP_INT ) {
                DoneWithInterrupt( &IntData );
                return( RESTART_APP );
            }
        }
    }

    if( (!DumpAny.dump_pending && (ff.intf.intnumber == INT_3) ) ||
        (ff.intf.intnumber == INT_1) ) {
        if( WasFault32 ) {
            if( ff.intf.intnumber == INT_3 ) {
                IntData.EIP++;
            }
            DoneWithInterrupt( &IntData );
            WasFault32 = false;
        }
        return( CHAIN );
    }

    /*
     * only one fault at a time
     */
    if( FaultHandlerEntered ) {
        if( WasFault32 ) {
            WasFault32 = false;
            DoneWithInterrupt( NULL );
        }
        return( CHAIN );
    }
    FaultHandlerEntered = true;
    ff.ESP = (WORD)ff.ESP;
    ff.EBP = (WORD)ff.EBP;

    /*
     * save state
     */
    if( !WasFault32 ) {
        SaveState( &IntData, &ff );
    }
    DeadTask = GetCurrentTask();

    MyTaskFindHandle( &DTTaskEntry, DeadTask );
    MyModuleFindHandle( &DTModuleEntry, DTTaskEntry.hModule );
    IsWin32App = CheckIsWin32App( DeadTask );
    MinAddrSpaces = 15;
    faultid = GetFaultString( ff.intf.intnumber, NULL );
    fault_str = AllocRCString( faultid );
    LBPrintf( ListBox, STR_FAULT_IN_TASK, fault_str,
                (WORD)DeadTask, DTModuleEntry.szModule );
    FreeRCString( fault_str );
    if( IsWin32App ) {
        LBPrintf( ListBox, STR_ADDRESS_EQ_32, IntData.CS, IntData.EIP );
    } else {
        LBPrintf( ListBox, STR_ADDRESS_EQ_16, IntData.CS, (WORD)IntData.EIP );
    }

    LoadDbgInfo();
    if( LogInfo.flags[LOGFL_AUTOLOG] != '1' ) {
        dlgproc = MakeProcInstance_DLG( IntDialogDlgProc, Instance );
        appl_act = JDialogBox( Instance, "INTERRUPT", NULL, dlgproc );
        FreeProcInstance_DLG( dlgproc );
    } else {
        appl_act = KILL_APP;
    }

    if( appl_act == RESTART_APP ) {
        if( !WasFault32 ) {
            RestoreState( &IntData, &ff );
        } else {
            WasFault32 = false;
            DoneWithInterrupt( &IntData );
        }
        if( doLog ) {
            MakeLog( true );
        }
    } else {
        if( doLog ) {
            MakeLog( true );
        }
    }
    UnloadDbgInfo();
    if( WasFault32 ) {
        WasFault32 = false;
        DoneWithInterrupt( NULL );
    }
    FaultHandlerEntered = false;

    return( appl_act );

} /* FaultHandler */
