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
#include "jdlg.h"
#include "madrtn.h"
#include "malloc.h"
#include "madsys1.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK ExceptionProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

msglist ExceptionMsgs[] = {
    EXCEPTION_ACCESS_VIOLATION,         (char *)(pointer_int)STR_ACCESS_VIOLATION,
    EXCEPTION_BREAKPOINT,               (char *)(pointer_int)STR_NADA,
    EXCEPTION_DATATYPE_MISALIGNMENT,    (char *)(pointer_int)STR_DATA_MISALIGNMENT,
    EXCEPTION_SINGLE_STEP,              (char *)(pointer_int)STR_NADA,
    EXCEPTION_ARRAY_BOUNDS_EXCEEDED,    (char *)(pointer_int)STR_ARRAY_BNDS_EXCEEDED,
    EXCEPTION_FLT_DENORMAL_OPERAND,     (char *)(pointer_int)STR_DENORMAL_FLOAT,
    EXCEPTION_FLT_DIVIDE_BY_ZERO,       (char *)(pointer_int)STR_FLT_DIV_BY_ZERO,
    EXCEPTION_FLT_INEXACT_RESULT,       (char *)(pointer_int)STR_FLT_INEXACT_RESULT,
    EXCEPTION_FLT_INVALID_OPERATION,    (char *)(pointer_int)STR_FLT_INV_OP,
    EXCEPTION_FLT_OVERFLOW,             (char *)(pointer_int)STR_FLT_PT_OVERFLOW,
    EXCEPTION_FLT_STACK_CHECK,          (char *)(pointer_int)STR_FLT_STACK_CHECK,
    EXCEPTION_FLT_UNDERFLOW,            (char *)(pointer_int)STR_FLT_UNDERFLOW,
    EXCEPTION_INT_DIVIDE_BY_ZERO,       (char *)(pointer_int)STR_INT_DIV_BY_ZERO,
    EXCEPTION_INT_OVERFLOW,             (char *)(pointer_int)STR_INT_OVERFLOW,
    EXCEPTION_PRIV_INSTRUCTION,         (char *)(pointer_int)STR_INV_INSTRUCTION,
    STATUS_NONCONTINUABLE_EXCEPTION,    (char *)(pointer_int)STR_NADA,
    0,                                  (char *)(pointer_int)-1
};

#ifdef __NT__

static void setProcessHdl( HANDLE hdl ) {
    ProcessHdl = hdl;
}
#endif

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

ExceptDlgInfo * FaultGetExceptDlgInfo( HWND fault )
{
    return( (ExceptDlgInfo *)GetWindowLong( fault, DWL_USER ) );

}

static void SetIp( HWND hwnd, address *addr ) {
    mad_type_info   host;
    mad_type_info   mti;
    void            *item;
    char            buf[BUF_SIZE];
    unsigned        max;

    max = BUF_SIZE - 1;
    MADTypeInfoForHost( MTK_ADDRESS, sizeof( address ), &host );
    MADTypeInfo( MADTypeDefault( MTK_ADDRESS, MAF_FULL, NULL, addr ), &mti );
    item = alloca( ( mti.b.bits / BITS_PER_BYTE ) + 1);
    MADTypeConvert( &host, addr, &mti, item, 0 );
    MADTypeToString( 16, &mti, item, buf, &max );
    SetDlgItemText( hwnd, INT_CS_IP, buf );
}

static void fillExceptionDlg( HWND hwnd, ExceptDlgInfo *info ) {

    char                buf[BUF_SIZE];
    char                fname[ FNAME_BUFLEN ];
    DWORD               line;
    ProcStats           stats;
    HWND                ctl;
    address             addr;

    GetCurrAddr(&addr,info->regs);
    SetDlgCourierFont( hwnd, INT_TASK_NAME );
    SetDlgCourierFont( hwnd, INT_TASK_PATH );
    SetDlgCourierFont( hwnd, INT_FAULT_TYPE );
    SetDlgCourierFont( hwnd, INT_CS_IP );
    SetDlgCourierFont( hwnd, INT_SOURCE_INFO );
    SetDlgCourierFont( hwnd, INT_SOURCE_INFO2 );

    RCsprintf( buf, STR_EXCEPTION_ENCOUNTERED, AppName );
    SetWindowText( hwnd, buf );

    CopyRCString( STR_PROCESS_NAME, buf, BUF_SIZE );
    SetDlgItemText( hwnd, INT_TASK_NAME_TEXT, buf );

    if( !info->dbinfo->u.Exception.dwFirstChance ) {
        if( ConfigData.exception_action == INT_CHAIN_TO_NEXT ) {
            ConfigData.exception_action = INT_TERMINATE;
        }
        ctl = GetDlgItem( hwnd, INT_CHAIN_TO_NEXT );
        EnableWindow( ctl, FALSE );
    }
    CheckDlgButton( hwnd, ConfigData.exception_action, BST_CHECKED );
    CopyRCString( STR_PROCESS_ID, buf, BUF_SIZE );
    SetDlgItemText( hwnd, INT_TASK_PATH_TEXT, buf );

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
        strcpy( buf, "Fault " );
        MADRegSpecialName( MSR_IP, info->regs, MAF_FULL, buf + 6, BUF_SIZE - 7 );
        SetDlgItemText( hwnd, INT_IP_NAME, buf );
        SetIp( hwnd, &addr );
    }
    if( info->got_dbginfo && GetLineNum( &addr,fname, FNAME_BUFLEN, &line ) ) {
        RCsprintf( buf, STR_LINE_X_OF, line );
        SetDlgItemText( hwnd, INT_SOURCE_INFO, buf );
        SetDlgItemText( hwnd, INT_SOURCE_INFO2, fname );
    } else {
        CopyRCString( STR_N_A, buf, BUF_SIZE );
        SetDlgItemText( hwnd, INT_SOURCE_INFO, buf );
        SetDlgItemText( hwnd, INT_SOURCE_INFO2, "" );
    }
}

static void centerDialog( HWND hwnd ) {

    RECT        area;
    int         screen_width;
    int         screen_hite;
    int         xpos;
    int         ypos;
    WORD        xsize;
    WORD        ysize;

    GetWindowRect( hwnd, &area );
    screen_width = GetSystemMetrics( SM_CXSCREEN );
    screen_hite = GetSystemMetrics( SM_CYSCREEN );
    xsize = area.right - area.left;
    ysize = area.bottom - area.top;
    xpos = ( screen_width - xsize ) / 2;
    ypos = ( screen_hite - ysize ) / 2;
    MoveWindow( hwnd, xpos, ypos, xsize, ysize, TRUE );
}

/*
 * ExceptionProc
 */
BOOL CALLBACK ExceptionProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD                cmd;
    ExceptDlgInfo       *info;
    ProcNode            *procinfo;
    WORD                tmp;
    address             addr;

    info = FaultGetExceptDlgInfo( hwnd );
    switch( msg ) {
    case WM_INITDIALOG:
        /* make sure this dialog always comes up on top of everything else */
        SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE );
        SetWindowPos( hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                        SWP_NOSIZE | SWP_NOMOVE );
        centerDialog( hwnd );
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
        if( info->threadinfo != NULL ) {
            AllocMadRegisters( &(info->regs) );
            LoadMADRegisters( info->regs, info->threadinfo->threadhdl );
            GetCurrAddr( &( info->init_ip ), info->regs );
        }
        if( info->module == NULL ) {
            info->got_dbginfo = FALSE;
        } else {
            if( !LoadDbgInfo( info->module ) ) {
                info->got_dbginfo = FALSE;
            } else {
                info->got_dbginfo = TRUE;
            }
        }
        if( LogData.autolog ) {         //Just create the log and exit
            tmp = ConfigData.exception_action;
            CheckDlgButton( hwnd, INT_TERMINATE, BST_CHECKED );
            SendMessage( hwnd, WM_COMMAND,
                        MAKELONG( INT_ACT_AND_LOG, BN_CLICKED ),
                        (LPARAM)GetDlgItem( hwnd, INT_ACT_AND_LOG ) );
            ConfigData.exception_action = tmp;
        } else {
            fillExceptionDlg( hwnd, info );
        }
        setProcessHdl( info->procinfo->prochdl );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case INT_ACT_AND_LOG:
            MakeLog( info );
            /* fall through */
        case INT_ACT:
            if( IsDlgButtonChecked( hwnd, INT_TERMINATE ) ) {
                ConfigData.exception_action = INT_TERMINATE;
//              hp = OpenProcess( PROCESS_TERMINATE, FALSE,
//                                info->dbinfo->dwProcessId );
                procinfo = FindProcess( info->dbinfo->dwProcessId );
                if( procinfo == NULL ) {
                    RCMessageBox( hwnd, STR_CANT_TERMINATE_APP,
                                  AppName, MB_OK | MB_ICONEXCLAMATION );
                } else {
                    TerminateProcess( procinfo->prochdl, -1 );
//                  CloseHandle( hp );
                    info->rc = DBG_CONTINUE;
                    info->action = INT_TERMINATE;
                    SendMessage( hwnd, WM_CLOSE, 0, 0L );
                }
            } else if( IsDlgButtonChecked( hwnd, INT_CHAIN_TO_NEXT ) ) {
                ConfigData.exception_action = INT_CHAIN_TO_NEXT;
                info->rc = DBG_EXCEPTION_NOT_HANDLED;
                info->action = INT_CHAIN_TO_NEXT;
                SendMessage( hwnd, WM_CLOSE, 0, 0L );
            } else if( IsDlgButtonChecked( hwnd, INT_RESTART ) ) {
                ConfigData.exception_action = INT_RESTART;
                info->rc = DBG_CONTINUE;
                info->action = INT_RESTART;
                SendMessage( hwnd, WM_CLOSE, 0, 0L );
            }
            break;
        case INT_REGISTERS:
            SetDisasmInfo( info->procinfo->prochdl, info->module );
            StatShowSymbols = TRUE;
            if ( DoStatDialog( hwnd ) == 1 ){
                StoreMADRegisters( info->regs, info->threadinfo->threadhdl );
                GetCurrAddr(&addr,info->regs);
                SetIp( hwnd, &addr );
            }
            LoadMADRegisters( info->regs, info->threadinfo->threadhdl );
            break;
        case INT_LOG_OPTIONS:
            SetLogOptions( hwnd );
            break;
        }
        break;
    case WM_CLOSE:
        if( info->rc == 0 ) {
            SendMessage( hwnd, WM_COMMAND, INT_ACT, 0L );
        } else {
            if( info->got_dbginfo ) {
                UnloadDbgInfo( info->module );
            }
            EndDialog( hwnd, info->rc );
        }
        break;
    case WM_DESTROY:
        DeAllocMadRegisters( info->regs );
        MemFree( info );
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

    INT_PTR     ret;

    RefreshCostlyInfo();
    ret = JDialogBoxParam( Instance, "INTERRUPT", MainHwnd, ExceptionProc, (LPARAM)dbinfo );
    return( ret );
}
