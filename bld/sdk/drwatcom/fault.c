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
#include <string.h>
#include <dos.h>
#include "wdebug.h"
#include "drwatcom.h"
#include "intdata.h"

static int doLog=TRUE,tmpDoLog;

/*
 * IntDialog - handles input from user when a fault is received
 */
BOOL __export FAR PASCAL IntDialog( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam )
{
    char        buff[256];

    switch( msg ) {
    case WM_INITDIALOG:
        tmpDoLog = TRUE;
        SetDlgCourierFont( hwnd, INT_TASK_NAME );
        SetDlgCourierFont( hwnd, INT_TASK_PATH );
        SetDlgCourierFont( hwnd, INT_FAULT_TYPE );
        SetDlgCourierFont( hwnd, INT_CS_IP );
        SetDlgCourierFont( hwnd, INT_SOURCE_INFO );
        SetDlgCourierFont( hwnd, INT_SOURCE_INFO2 );
        RCsprintf( buff, STR_FAULT_X_ENCOUNTERED, AppName,
                    IntData.InterruptNumber );
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
        CheckDlgButton( hwnd, INT_ADD_TO_LOG, doLog );
        {
            ADDRESS     addr;
            syminfo     si;
            char        *na;

            addr.seg = IntData.CS;
            addr.offset = IntData.EIP;
            if( FindWatSymbol( &addr, &si, TRUE ) == FOUND ) {
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
        return( TRUE );
    case WM_CLOSE:
        tmpDoLog = FALSE;
        PostMessage( hwnd, WM_COMMAND, INT_TERMINATE, 0L );
        return( TRUE );
    case WM_COMMAND:
        switch( wparam ) {
        case IDCANCEL:
            tmpDoLog = FALSE;
            EndDialog( hwnd, KILL_APP );
            return( TRUE );
        case INT_TERMINATE:
            EndDialog( hwnd, KILL_APP );
            return( TRUE );
        case INT_CHAIN_TO_NEXT:
            EndDialog( hwnd, CHAIN );
            return( TRUE );
        case INT_RESTART:
            EndDialog( hwnd, RESTART_APP );
            return( TRUE );
        case INT_LOG_OPTIONS:
            DoLogDialog( hwnd );
            return( TRUE );
#if(0)
        case INT_DUMP:
            DumpTask( hwnd );
            return( TRUE );
#endif
        case INT_ADD_TO_LOG:
            if( HIWORD( lparam ) == BN_CLICKED ) {
                if( !doLog ) {
                    doLog = TRUE;
                } else {
                    doLog = FALSE;
                }
                CheckDlgButton( hwnd, INT_ADD_TO_LOG, doLog );
            }
            break;
        case INT_TASK_STATUS:
            DoStatDialog( hwnd );
            break;
        }
    }
    return( FALSE );

} /* IntDialog */

/*
 * FaultHandler - C handler for a fault
 */
WORD __cdecl FAR FaultHandler( volatile fault_frame ff )
{
    FARPROC     fp;
    WORD        rc;
    char        *fault_str;
    DWORD       faultid;

    if( WDebug386 ) {
        if( IsDebuggerExecuting() ) {
            return( CHAIN );
        }
        WasFault = GetDebugInterruptData( &IntData );
        if( WasFault ) {
            ff.intnumber = IntData.InterruptNumber;
            if( ff.intnumber == WGOD_ASYNCH_STOP_INT ) {
                DoneWithInterrupt( &IntData );
                return( RESTART_APP );
            }
        }
    } else {
        WasFault = FALSE;
    }

    if( (!DumpAny.dump_pending && (ff.intnumber == INT_3) ) ||
        (ff.intnumber == INT_1) ) {
        if( WasFault ) {
            if( ff.intnumber == INT_3 ) {
                IntData.EIP++;
            }
            DoneWithInterrupt( &IntData );
            WasFault = FALSE;
        }
        return( CHAIN );
    }

    /*
     * only one fault at a time
     */
    if( FaultHandlerEntered ) {
        if( WasFault ) {
            WasFault = FALSE;
            DoneWithInterrupt( NULL );
        }
        return( CHAIN );
    }
    FaultHandlerEntered = TRUE;
    ff.ESP = (WORD) ff.ESP;
    ff.EBP = (WORD) ff.EBP;

    /*
     * save state
     */
    if( !WasFault ) {
        SaveState( &IntData, &ff );
    }
    DeadTask = GetCurrentTask();

    MyTaskFindHandle( &DTTaskEntry, DeadTask );
    MyModuleFindHandle( &DTModuleEntry, DTTaskEntry.hModule );
    IsWin32App = CheckIsWin32App( DeadTask );
    MinAddrSpaces = 15;
#if(0)
    if( DumpAny.dump_pending && (ff.intnumber == 3) &&
      (IntData.CS == DumpAny.CS) &&
      (IntData.EIP-1 == DumpAny.EIP) &&
      (DeadTask == DumpAny.taskid) )  {
        WriteMem( DumpAny.CS, DumpAny.EIP, &DumpAny.oldbyte, 1 );
        DumpAny.dump_pending = FALSE;
        DumpTask( NULL );
        SymFileClose();
        IntData.EIP--;
        if( !WasFault ) {
            RestoreState( &IntData, &ff );
        } else {
            WasFault = FALSE;
            DoneWithInterrupt( &IntData );
        }
        FaultHandlerEntered = FALSE;
        return( RESTART_APP );
    }
#endif

    faultid = GetFaultString( ff.intnumber, NULL );
    fault_str = AllocRCString( faultid );
    LBPrintf( ListBox, STR_FAULT_IN_TASK, fault_str,
                DeadTask, DTModuleEntry.szModule );
    FreeRCString( fault_str );
    if( IsWin32App ) {
        LBPrintf( ListBox, STR_ADDRESS_EQ_32, IntData.CS, IntData.EIP );
    } else {
        LBPrintf( ListBox, STR_ADDRESS_EQ_16, (WORD) IntData.EIP );
    }

    LoadDbgInfo( );
    if( LogInfo.flags[LOGFL_AUTOLOG] != '1' ) {
        fp = MakeProcInstance( IntDialog, Instance );
        rc = DialogBox( Instance, "INTERRUPT", NULL, fp );
        FreeProcInstance( fp );
    } else {
        tmpDoLog = TRUE;
        rc = KILL_APP;
    }

    if( rc == RESTART_APP ) {
        if( !WasFault ) {
            RestoreState( &IntData, &ff );
        } else {
            WasFault = FALSE;
            DoneWithInterrupt( &IntData );
        }
        if( tmpDoLog && doLog && LogInfo.flags[LOGFL_LOG_IF_RESTART] == '1' ) {
            MakeLog( TRUE );
        }
    } else {
        if( tmpDoLog && doLog ) {
            MakeLog( TRUE );
        }
    }
    SymFileClose();
    if( WasFault ) {
        WasFault = FALSE;
        DoneWithInterrupt( NULL );
    }
    FaultHandlerEntered = FALSE;

    return( rc );

} /* FaultHandler */
