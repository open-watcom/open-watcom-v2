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
#include <stdio.h>
#include "drwatcom.h"
#include "srchmsg.h"
#include "mem.h"
#include "intdlg.h"
#include "wdebug.h"


msglist ExceptionMsgs[] = {
    EXCEPTION_ACCESS_VIOLATION,         (char *)STR_ACCESS_VIOLATION,
    EXCEPTION_BREAKPOINT,               (char *)STR_NADA,
    EXCEPTION_DATATYPE_MISALIGNMENT,    (char *)STR_DATA_MISALIGNMENT,
    EXCEPTION_SINGLE_STEP,              (char *)STR_NADA,
    EXCEPTION_ARRAY_BOUNDS_EXCEEDED,    (char *)STR_ARRAY_BNDS_EXCEEDED,
    EXCEPTION_FLT_DENORMAL_OPERAND,     (char *)STR_DENORMAL_FLOAT,
    EXCEPTION_FLT_DIVIDE_BY_ZERO,       (char *)STR_FLT_DIV_BY_ZERO,
    EXCEPTION_FLT_INEXACT_RESULT,       (char *)STR_FLT_INEXACT_RESULT,
    EXCEPTION_FLT_INVALID_OPERATION,    (char *)STR_FLT_INV_OP,
    EXCEPTION_FLT_OVERFLOW,             (char *)STR_FLT_PT_OVERFLOW,
    EXCEPTION_FLT_STACK_CHECK,          (char *)STR_FLT_STACK_CHECK,
    EXCEPTION_FLT_UNDERFLOW,            (char *)STR_FLT_UNDERFLOW,
    EXCEPTION_INT_DIVIDE_BY_ZERO,       (char *)STR_INT_DIV_BY_ZERO,
    EXCEPTION_INT_OVERFLOW,             (char *)STR_INT_OVERFLOW,
    EXCEPTION_PRIV_INSTRUCTION,         (char *)STR_INV_INSTRUCTION,
    STATUS_NONCONTINUABLE_EXCEPTION,    (char *)STR_NADA,
    0,                                  (char *)-1
};

static BOOL     tmpLog;

/*
 * FormatException
 */
void FormatException( char *buf, DWORD code ) {

    char        *str;

    str = SrchMsg( code, ExceptionMsgs, NULL );
    if( str == NULL ) {
        str = buf;
        RCsprintf( buf, STR_UNKNOWN_EXCEPTION_X, code );
    } else {
        strcpy( buf, str );
    }
}

#define FNAME_BUFLEN    50
/*
 * fillExceptionDlg
 */
#define BUF_SIZE        100
static void fillExceptionDlg( HWND hwnd, ExceptDlgInfo *info ) {

    char                buf[BUF_SIZE];
    char                fname[ FNAME_BUFLEN ];
    DWORD               line;
    ProcStats           stats;

    SetDlgCourierFont( hwnd, INT_TASK_NAME );
    SetDlgCourierFont( hwnd, INT_TASK_PATH );
    SetDlgCourierFont( hwnd, INT_FAULT_TYPE );
    SetDlgCourierFont( hwnd, INT_CS_IP );
    SetDlgCourierFont( hwnd, INT_SOURCE_INFO );
    SetDlgCourierFont( hwnd, INT_SOURCE_INFO2 );

    CopyRCString( STR_PROCESS_NAME, buf, BUF_SIZE );
    SetDlgItemText( hwnd, INT_TASK_NAME_TEXT, buf );

    CopyRCString( STR_PROCESS_ID, buf, BUF_SIZE );
    SetDlgItemText( hwnd, INT_TASK_PATH_TEXT, buf );

    CopyRCString( STR_PROCESS_STATUS, buf, BUF_SIZE );
    SetDlgItemText( hwnd, INT_TASK_STATUS, buf );

    RCsprintf( buf, STR_EXCEPTION_ENCOUNTERED, AppName );
    SetWindowText( hwnd, buf );

    while( !GetProcessInfo( info->procinfo->procid, &stats ) ) {
        Sleep( 100 );
        RefreshInfo();
    }
    SetDlgItemText( hwnd, INT_TASK_NAME, stats.name );

    sprintf( buf, "%08lX", info->procinfo->procid );
    SetDlgItemText( hwnd, INT_TASK_PATH, buf );

    FormatException( buf, info->dbinfo->u.Exception.ExceptionRecord.ExceptionCode );
    SetDlgItemText( hwnd, INT_FAULT_TYPE, buf );

    if( info->threadinfo != NULL ) {
        sprintf( buf, "%04X:%08lX", info->context.SegCs, info->context.Eip );
        SetDlgItemText( hwnd, INT_CS_IP, buf );
    }
    if( info->got_dbginfo && GetLineNum( info->module, info->context.Eip,
                                     fname, FNAME_BUFLEN, &line ) ) {
        RCsprintf( buf, STR_LINE_X_OF_FILE_Y, line, fname );
        SetDlgItemText( hwnd, INT_SOURCE_INFO, buf );
        SetDlgItemText( hwnd, INT_SOURCE_INFO2, "" );
    } else {
        CopyRCString( STR_N_A, buf, BUF_SIZE );
        SetDlgItemText( hwnd, INT_SOURCE_INFO, buf );
        SetDlgItemText( hwnd, INT_SOURCE_INFO2, "" );
    }
    CheckDlgButton( hwnd, INT_ADD_TO_LOG, LogData.log_events );
}

/*
 * ExceptionProc
 */
BOOL CALLBACK ExceptionProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    WORD                cmd;
    ExceptDlgInfo       *info;
    HANDLE              hp;

    info = (ExceptDlgInfo *)GetWindowLong( hwnd, DWL_USER );
    switch( msg ) {
    case WM_INITDIALOG:
        /* make sure this dialog always comes up on top of everything else */
        SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE );
        SetWindowPos( hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE );
        info = MemAlloc( sizeof( ExceptDlgInfo ) );
        info->dbinfo = (DEBUG_EVENT *)lparam;
        info->rc = 0;
        info->action = 0;
        SetWindowLong( hwnd, DWL_USER, (DWORD)info );
        info->procinfo = FindProcess( info->dbinfo->dwProcessId );
        info->threadinfo = FindThread( info->procinfo,
                                       info->dbinfo->dwThreadId );
        info->module = ModuleFromAddr( info->procinfo,
               info->dbinfo->u.Exception.ExceptionRecord.ExceptionAddress );
        if( info->module == NULL ) {
            info->got_dbginfo = FALSE;
        } else {
            if( !LoadDbgInfo( info->module ) ) {
                info->got_dbginfo = FALSE;
            } else {
                info->got_dbginfo = TRUE;
            }
        }
        if( info->threadinfo != NULL ) {
            info->context.ContextFlags = CONTEXT_FULL;
            GetThreadContext( info->threadinfo->threadhdl, &info->context );
        }
        fillExceptionDlg( hwnd, info );
        tmpLog = TRUE;
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case IDCANCEL:
            tmpLog = FALSE;
            /* fall through */
        case INT_TERMINATE:
            hp = OpenProcess( PROCESS_TERMINATE, FALSE,
                              info->dbinfo->dwProcessId );
            if( hp == NULL ) {
                RCMessageBox( hwnd, STR_CANT_TERMINATE_APP,
                              AppName, MB_OK | MB_ICONEXCLAMATION );
            } else {
                TerminateProcess( hp, -1 );
                CloseHandle( hp );
                info->rc = DBG_CONTINUE;
                info->action = INT_TERMINATE;
                SendMessage( hwnd, WM_CLOSE, 0, 0L );
            }
            break;
        case INT_ADD_TO_LOG:
            LogData.log_events = !LogData.log_events;
            CheckDlgButton( hwnd, INT_ADD_TO_LOG, LogData.log_events );
            break;
        case INT_RESTART:
            info->rc = DBG_CONTINUE;
            info->action = INT_RESTART;
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case INT_CHAIN_TO_NEXT:
            info->rc = DBG_EXCEPTION_NOT_HANDLED;
            info->action = INT_CHAIN_TO_NEXT;
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case INT_TASK_STATUS:
            IntData.EAX = info->context.Eax;
            IntData.EBX = info->context.Ebx;
            IntData.ECX = info->context.Ecx;
            IntData.EDX = info->context.Edx;
            IntData.EDI = info->context.Edi;
            IntData.ESI = info->context.Esi;
            IntData.EFlags = info->context.EFlags;
            IntData.EBP = info->context.Ebp;
            IntData.EIP = info->context.Eip;
            IntData.ESP = info->context.Esp;
            IntData.SS = info->context.SegSs;
            IntData.CS = info->context.SegCs;
            IntData.DS = info->context.SegDs;
            IntData.ES = info->context.SegEs;
            IntData.FS = info->context.SegFs;
            IntData.GS = info->context.SegGs;
            SetDisasmInfo( info->procinfo->prochdl, info->module );
            StatShowSymbols = TRUE;
            SetProcessInfo( info->procinfo->prochdl, info->procinfo->procid );
            DoStatDialog( hwnd );
            info->context.Eax = IntData.EAX;
            info->context.Ebx = IntData.EBX;
            info->context.Ecx = IntData.ECX;
            info->context.Edx = IntData.EDX;
            info->context.Edi = IntData.EDI;
            info->context.Esi = IntData.ESI;
            info->context.EFlags = IntData.EFlags;
            info->context.Ebp = IntData.EBP;
            info->context.Eip = IntData.EIP;
            info->context.Esp = IntData.ESP;
            info->context.SegSs = IntData.SS;
            info->context.SegCs = IntData.CS;
            info->context.SegDs = IntData.DS;
            info->context.SegEs = IntData.ES;
            info->context.SegFs = IntData.FS;
            info->context.SegGs = IntData.GS;
            SetThreadContext( info->threadinfo->threadhdl, &info->context );
            break;
        case INT_LOG_OPTIONS:
            SetLogOptions( hwnd );
            break;
        }
        break;
    case WM_CLOSE:
        if( info->rc == 0 ) {
            SendMessage( hwnd, WM_COMMAND, INT_TERMINATE, 0L );
        } else {
            if( LogData.log_events && tmpLog ) {
                MakeLog( info );
            }
            if( info->got_dbginfo ) {
                UnloadDbgInfo( info->module );
            }
            EndDialog( hwnd, info->rc );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

/*
 * HandleException
 */
int HandleException( DEBUG_EVENT *dbinfo ) {

    int         ret;

    RefreshCostlyInfo();
    ret = DialogBoxParam( Instance, "INTERRUPT", MainHwnd, ExceptionProc,
                    (DWORD)dbinfo );
    return( ret );
}
