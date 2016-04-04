/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mem.h"
#include "thrdctl.h"
#include "srchmsg.h"
#include "priority.h"
#include "retcode.h"
#include "jdlg.h"

typedef struct {
    DWORD       procid;
    ProcStats   procinfo;
} ThreadCtlInfo;

typedef struct {
    DWORD       procid;
    ThreadNode  *thread;
    DWORD       priority;
    ProcStats   *procinfo;
} ThreadPriorityInfo;

typedef struct {
    RetCodeTypes        type;
    DWORD               id;
    DWORD               rc;
    BOOL                really_kill;
} RetCodeInfo;

msglist ThreadWaitMsgs[] = {
    0,      (char *)(pointer_int)STR_WAIT_4_EXECUTIVE,
    1,      (char *)(pointer_int)STR_WAIT_4_FREE_PAGE,
    2,      (char *)(pointer_int)STR_WAIT_4_PAGE_IN,
    3,      (char *)(pointer_int)STR_WAIT_4_POOL_ALLOC,
    4,      (char *)(pointer_int)STR_WAIT_4_EXEC_DELAY,
    5,      (char *)(pointer_int)STR_WAIT_4_SUSP_COND,
    6,      (char *)(pointer_int)STR_WAIT_4_USER_REQUEST,
    7,      (char *)(pointer_int)STR_WAIT_4_EXECUTIVE,
    8,      (char *)(pointer_int)STR_WAIT_4_FREE_PAGE,
    9,      (char *)(pointer_int)STR_WAIT_4_PAGE_IN,
    10,     (char *)(pointer_int)STR_WAIT_4_POOL_ALLOC,
    11,     (char *)(pointer_int)STR_WAIT_4_EXEC_DELAY,
    12,     (char *)(pointer_int)STR_WAIT_4_SUSP_COND,
    13,     (char *)(pointer_int)STR_WAIT_4_USER_REQUEST,
    14,     (char *)(pointer_int)STR_WAIT_4_EVENT_PR_HIGH,
    15,     (char *)(pointer_int)STR_WAIT_4_EVENT_PR_LOW,
    16,     (char *)(pointer_int)STR_WAIT_4_LPC_RECIEVE,
    17,     (char *)(pointer_int)STR_WAIT_4_LPC_REPLY,
    18,     (char *)(pointer_int)STR_WAIT_4_VIRTUAL_MEMORY,
    19,     (char *)(pointer_int)STR_WAIT_4_PAGE_OUT,
    0,      (char *)(pointer_int)-1
};

msglist ThreadStateMsgs[] = {
    0,      (char *)(pointer_int)STR_INITIALIZED,
    1,      (char *)(pointer_int)STR_READY,
    2,      (char *)(pointer_int)STR_RUNNING,
    3,      (char *)(pointer_int)STR_STANDING_BY,
    4,      (char *)(pointer_int)STR_TERMINATED,
    5,      (char *)(pointer_int)STR_WAITING,
    6,      (char *)(pointer_int)STR_TRANSITION,
    7,      (char *)(pointer_int)STR_UNKNOWN,
    0,      (char *)(pointer_int)-1
};

#define BUF_SIZE        100

/*
 * ParseNumeric
 */
BOOL ParseNumeric( char *buf, BOOL signed_val, DWORD *val ) {

    char        *end;
    char        *last;

    end = buf;
    while( *end ) end++;
    if( end != buf ) {
        end --;
        while( isspace( *end ) ) end--;
        end++;
    }
    if( signed_val ) {
        *val = strtol( buf, &last, 0 );
    } else {
        *val = strtoul( buf, &last, 0 );
    }
    if( end != last || end == buf ) {
        return( FALSE );
    }
    return( TRUE );
}

/*
 * ThreadCtlProc
 */
BOOL CALLBACK RetCodeDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    RetCodeInfo         *info;
    WORD                cmd;
    char                buf[BUF_SIZE];
    char                *title;
    msg_id              info_str_id;

    info = (RetCodeInfo *)GetWindowLong( hwnd, DWL_USER );
    switch( msg ) {
    case WM_INITDIALOG:
        info = (RetCodeInfo *)lparam;
        info->really_kill = FALSE;
        SetWindowLong( hwnd, DWL_USER, lparam );
        if( info->type == RETCD_THREAD ) {
            title = AllocRCString( STR_THREAD_RETURN_CODE );
            info_str_id = STR_THREAD_X;
        } else {
            title = AllocRCString( STR_PROCESS_RETURN_CODE );
            info_str_id = STR_PROCESS_X;
        }
        SetWindowText( hwnd, title );
        FreeRCString( title );
        RCsprintf( buf, info_str_id, info->type, info->id );
        SetDlgItemText( hwnd, RET_PROC_INFO, buf );
        SetDlgItemText( hwnd, RET_VALUE, "-1" );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case IDOK:
            GetDlgItemText( hwnd, RET_VALUE, buf, BUF_SIZE );
            if( !ParseNumeric( buf, TRUE, &info->rc ) ) {
                RCMessageBox( hwnd, STR_INVALID_RETURN_CODE,
                            AppName, MB_OK | MB_ICONEXCLAMATION );
                break;
            }
            info->really_kill = TRUE;
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

/*
 * GetRetCode
 */
BOOL GetRetCode( HWND parent, RetCodeTypes type, DWORD id, DWORD *rc ) {

    RetCodeInfo         info;

    info.type = type;
    info.id = id;
    info.rc = 0;
    JDialogBoxParam( Instance, "RET_CODE_DLG", parent, RetCodeDlgProc, (LPARAM)&info );
    *rc = info.rc;
    return( info.really_kill );
}

/*
 * enableChoices
 */
static void enableChoices( HWND hwnd, BOOL enable ) {
    EnableWindow( GetDlgItem( hwnd, THREAD_KILL ), enable );
//    EnableWindow( GetDlgItem( hwnd, THREAD_SET_PRIORITY ), enable );
    EnableWindow( GetDlgItem( hwnd, THREAD_SUSPEND ), enable );
    EnableWindow( GetDlgItem( hwnd, THREAD_RESUME ), enable );
    if( !enable ) {
        SetDlgItemText( hwnd, THREAD_TID, "" );
        SetDlgItemText( hwnd, THREAD_SUSPEND_CNT, "" );
        SetDlgItemText( hwnd, THREAD_PRIORITY, "" );
    }
}

#if(0)
/*
 * ThreadCtlProc
 */
BOOL CALLBACK ThreadPriorityProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD                cmd;
    ThreadPriorityInfo  *info;
    char                buf[100];
    BOOL                ret;

    info = (ThreadPriorityInfo *)GetWindowLong( hwnd, DWL_USER );
    switch( msg ) {
    case WM_INITDIALOG:
        info = (ThreadPriorityInfo *)lparam;
        sprintf( buf, "pid = %08lX (%s)", info->procid,
                 info->procinfo->name );
        SetDlgItemText( hwnd, PRIORITY_INFO, buf );
        sprintf( buf, "tid = %08lX", info->thread->threadid );
        SetDlgItemText( hwnd, PRIORITY_PATH, buf );
        switch( info->priority ) {
        case THREAD_PRIORITY_IDLE:
            CheckDlgButton( hwnd, PRIORITY_IDLE, BST_CHECKED );
            break;
        case THREAD_PRIORITY_LOWEST:
            CheckDlgButton( hwnd, PRIORITY_LOWEST, BST_CHECKED );
            break;
        case THREAD_PRIORITY_BELOW_NORMAL:
            CheckDlgButton( hwnd, PRIORITY_BELOW_NORMAL, BST_CHECKED );
            break;
        case THREAD_PRIORITY_NORMAL:
            CheckDlgButton( hwnd, PRIORITY_NORMAL, BST_CHECKED );
            break;
        case THREAD_PRIORITY_ABOVE_NORMAL:
            CheckDlgButton( hwnd, PRIORITY_ABOVE_NORMAL, BST_CHECKED );
            break;
        case THREAD_PRIORITY_HIGHEST:
            CheckDlgButton( hwnd, PRIORITY_HIGHEST, BST_CHECKED );
            break;
        case THREAD_PRIORITY_TIME_CRITICAL:
            CheckDlgButton( hwnd, PRIORITY_TIME_CRITICAL, BST_CHECKED );
            break;
        }
        SetWindowLong( hwnd, DWL_USER, lparam );
        break;
    case WM_COMMAND:
         cmd = LOWORD( wparam );
         switch( cmd ) {
         case IDOK:
             if( IsDlgButtonChecked( hwnd, PRIORITY_IDLE ) ) {
                ret = SetThreadPriority( info->thread->threadhdl,
                                         THREAD_PRIORITY_IDLE );
             } else if( IsDlgButtonChecked( hwnd, PRIORITY_LOWEST ) ) {
                ret = SetThreadPriority( info->thread->threadhdl,
                                         THREAD_PRIORITY_LOWEST );
             } else if( IsDlgButtonChecked( hwnd, PRIORITY_BELOW_NORMAL ) ) {
                ret = SetThreadPriority( info->thread->threadhdl,
                                         THREAD_PRIORITY_BELOW_NORMAL );
             } else if( IsDlgButtonChecked( hwnd, PRIORITY_NORMAL ) ) {
                ret = SetThreadPriority( info->thread->threadhdl,
                                         THREAD_PRIORITY_NORMAL );
             } else if( IsDlgButtonChecked( hwnd, PRIORITY_ABOVE_NORMAL ) ) {
                ret = SetThreadPriority( info->thread->threadhdl,
                                         THREAD_PRIORITY_ABOVE_NORMAL );
             } else if( IsDlgButtonChecked( hwnd, PRIORITY_HIGHEST ) ) {
                ret = SetThreadPriority( info->thread->threadhdl,
                                         THREAD_PRIORITY_HIGHEST );
             } else if( IsDlgButtonChecked( hwnd, PRIORITY_TIME_CRITICAL ) ) {
                ret = SetThreadPriority( info->thread->threadhdl,
                                         THREAD_PRIORITY_TIME_CRITICAL );
             }
             if( !ret ) {
                 RCMessageBox( hwnd, STR_CANT_SET_THREAD_PRI, AppName,
                             MB_OK | MB_ICONEXCLAMATION );
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
#endif

static DWORD getThreadId( char *str ) {
    DWORD       ret;

    sscanf( str, "%*s %*s %X", &ret );
    return( ret );
}

static void fillThreadCtl( HWND hwnd, ProcStats *info, char *buf ) {

    HWND        lb;
    int         index;
    char        save[100];
    ThreadList  thdinfo;
    ThreadPlace place;
    BOOL        rc;

    lb = GetDlgItem( hwnd, THREAD_LIST );
    index = (int)SendMessage( lb, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        strcpy( save, "bbbbbbbbbbbb" ); /* just some text that shouldn't
                                           match anything in the listbox */
    } else {
        SendMessage( lb, LB_GETTEXT, index, (LPARAM)save );
    }
    SendMessage( lb, LB_RESETCONTENT, 0, 0L );
    rc = GetNextThread( &thdinfo, &place, info->pid, TRUE );
    while( rc ) {
        sprintf( buf, "tid = %08lX", thdinfo.tid );
        SendMessage( lb, LB_ADDSTRING, 0, (LPARAM)buf );
        rc = GetNextThread( &thdinfo, &place, info->pid, FALSE );
    }
    index = SendMessage( lb, LB_FINDSTRING, -1, (LPARAM)save );
    if( index == LB_ERR ) {
        enableChoices( hwnd, FALSE );
    } else {
        SendMessage( lb, LB_SETCURSEL, index, 0L );
    }
}

static void fillThreadInfo( HWND hwnd, ProcStats *info ) {

    HWND        lb;
    int         index;
    char        buf[100];
    DWORD       threadid;
    ThreadStats thdinfo;
#ifndef CHICAGO
    char        *str1;
#endif

    lb = GetDlgItem( hwnd, THREAD_LIST );
    index = (int)SendMessage( lb, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        SetDlgItemText( hwnd, THREAD_TID, "" );
        SetDlgItemText( hwnd, THREAD_SUSPEND_CNT, "" );
        SetDlgItemText( hwnd, THREAD_PRIORITY, "" );
    } else {
        enableChoices( hwnd, TRUE );
        SendMessage( lb, LB_GETTEXT, index, (LPARAM)buf );
        threadid = getThreadId( buf );
        sprintf( buf, "tid = %08lX", threadid );
        SetDlgItemText( hwnd, THREAD_TID, buf );

        if( GetThreadInfo( info->pid, threadid, &thdinfo ) ) {
#ifndef CHICAGO
            if( thdinfo.state == 5 ) {  /* the thread is in a wait state */
                str1 = SrchMsg( thdinfo.wait_reason, ThreadWaitMsgs, NULL );
                if( str1 == NULL ) {
                    str1 = AllocRCString( STR_WAIT_4_UNKNOWN );
                    RCsprintf( buf, STR_STATE, str1 );
                    FreeRCString( str1 );
                } else {
                    RCsprintf( buf, STR_STATE, str1 );
                }
            } else {
                str1 = SrchMsg( thdinfo.state, ThreadStateMsgs, NULL );
                if( str1 == NULL ) {
                    str1 = AllocRCString( STR_BRACED_UNKNOWN );
                    RCsprintf( buf, STR_STATE, str1 );
                    FreeRCString( str1 );
                } else {
                    RCsprintf( buf, STR_STATE, str1 );
                }
            }
            SetDlgItemText( hwnd, THREAD_SUSPEND_CNT, buf );
#endif

            /* get priority */
            RCsprintf( buf, STR_PRIORITY_X, thdinfo.cur_pri,
                        thdinfo.base_pri );
            SetDlgItemText( hwnd, THREAD_PRIORITY, buf );
        }
    }
}

/*
 * ThreadCtlProc
 */
BOOL CALLBACK ThreadCtlProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD                cmd;
    ThreadCtlInfo       *info;
    int                 index;
    char                buf[200];
    DWORD               threadid;
    ThreadNode          *thread;
    ProcNode            *process;
    DWORD               susp_cnt;
    DWORD               rc;
    char                *action;

    info = (ThreadCtlInfo *)GetWindowLong( hwnd, DWL_USER );
    switch( msg ) {
    case WM_INITDIALOG:
        info = MemAlloc( sizeof( ThreadCtlInfo ) );
        if( !GetProcessInfo( lparam, &info->procinfo ) ) {
             RCsprintf( buf, STR_CANT_GET_PROC_INFO, info->procid );
             MessageBox( hwnd, buf, AppName, MB_OK | MB_ICONEXCLAMATION );
             SendMessage( hwnd, WM_CLOSE, 0, 0 );
        }
        info->procid = lparam;
        ThreadDlg = hwnd;
        SetWindowLong( hwnd, DWL_USER, (DWORD)info );
        fillThreadCtl( hwnd, &info->procinfo, buf );
        RCsprintf( buf, STR_THREAD_4_PROC_X, lparam );
        SetDlgItemText( hwnd, THREAD_PROC_NAME, buf );
        sprintf( buf, "(%s)", info->procinfo.name );
        SetDlgItemText( hwnd, THREAD_PROC_PATH, buf );
        SendDlgItemMessage( hwnd, THREAD_LIST, LB_SETCURSEL, 0, 0L );
        index = (int)SendDlgItemMessage( hwnd, THREAD_LIST, LB_GETCURSEL, 0, 0L );
        if( index != LB_ERR ) {
            enableChoices( hwnd, TRUE );
        }
        fillThreadInfo( hwnd, &info->procinfo );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        if( cmd == THREAD_SUSPEND || cmd == THREAD_RESUME ||
            cmd == THREAD_KILL || cmd == THREAD_SET_PRIORITY ) {
            index = (int)SendDlgItemMessage( hwnd, THREAD_LIST, LB_GETCURSEL, 0, 0L );
            if( index == LB_ERR ) {
                RCMessageBox( hwnd, STR_NO_SELECTED_THREAD, AppName,
                            MB_OK | MB_ICONEXCLAMATION );
                break;
            }
            SendDlgItemMessage( hwnd, THREAD_LIST, LB_GETTEXT, index, (LPARAM)buf );
            threadid = getThreadId( buf );
            process = FindProcess( info->procinfo.pid );
            thread = FindThread( process, threadid );
        }
        switch( cmd ) {
        case IDOK:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case THREAD_REFRESH:
            RefreshInfo();
            if( GetProcessInfo( info->procid, &info->procinfo ) ) {
                fillThreadCtl( hwnd, &info->procinfo, buf );
                fillThreadInfo( hwnd, &info->procinfo );
            } else {
                action = AllocRCString( STR_REFRESH );
                RCMessageBox( hwnd, STR_CANT_REFRESH_THRD, action,
                              MB_OK | MB_ICONEXCLAMATION );
                FreeRCString( action );
            }
            break;
        case THREAD_SUSPEND:
            action = AllocRCString( STR_THREAD_SUSPEND );
            if( thread == NULL ) {
                RCsprintf( buf, STR_CANT_GET_HDL_4_THD_X, threadid );
                MessageBox( hwnd, buf, action, MB_OK | MB_ICONEXCLAMATION );
            } else {
                susp_cnt = SuspendThread( thread->threadhdl );
                if( susp_cnt == -1 ) {
                    RCsprintf( buf, STR_CANT_SUSPEND_THRD_X, threadid );
                    MessageBox( hwnd, buf, action, MB_ICONQUESTION | MB_OK );
                } else if( susp_cnt > 0 ) {
                    RCsprintf( buf, STR_THREAD_ALREADY_SUSP,
                               threadid, susp_cnt );
                    index = MessageBox( hwnd, buf, action,
                                        MB_ICONQUESTION | MB_YESNO );
                    if( index == IDNO ) {
                        ResumeThread( thread->threadhdl );
                    }
                }
                SendMessage( hwnd, WM_COMMAND, THREAD_REFRESH, 0L );
            }
            FreeRCString( action );
            break;
        case THREAD_RESUME:
            action = AllocRCString( STR_RESUME );
            if( thread == NULL ) {
                RCsprintf( buf, STR_THREAD_NOT_RESUMED , threadid );
                MessageBox( hwnd, buf, action, MB_OK | MB_ICONEXCLAMATION );
            } else {
                susp_cnt = ResumeThread( thread->threadhdl );
                if( susp_cnt == -1 ) {
                    RCsprintf( buf, STR_CANT_RESUME_THRD_X, threadid );
                    MessageBox( hwnd, buf, action,
                                MB_ICONEXCLAMATION | MB_OK );
                } else if( susp_cnt == 0 ) {
                    RCsprintf( buf, STR_THRD_IS_NOT_SUSP, threadid );
                    MessageBox( hwnd, buf, action,
                                MB_ICONEXCLAMATION | MB_OK );
                } else if( susp_cnt > 1 ) {
                    RCsprintf( buf, STR_SUSP_COUNT_DECREMENTED,
                                threadid, susp_cnt );
                    MessageBox( hwnd, buf, action,
                                MB_ICONEXCLAMATION | MB_OK );
                }
                SendMessage( hwnd, WM_COMMAND, THREAD_REFRESH, 0L );
            }
            FreeRCString( action );
            break;
        case THREAD_KILL:
            action = AllocRCString( STR_KILL );
            if( thread == NULL ) {
                RCsprintf( buf, STR_THRD_NOT_TERMINATED, threadid );
                MessageBox( hwnd, buf, action, MB_OK | MB_ICONEXCLAMATION );
            } else if( GetRetCode( hwnd, RETCD_THREAD, thread->threadid, &rc ) ) {
                if( !TerminateThread( thread->threadhdl, rc ) ) {
                    RCsprintf( buf, STR_CANT_KILL_THRD_X, threadid );
                    MessageBox( hwnd, buf, action,
                                MB_OK | MB_ICONEXCLAMATION );
                }
                SendMessage( hwnd, WM_COMMAND, THREAD_REFRESH, 0L );
            }
            FreeRCString( action );
            break;
        case THREAD_SET_PRIORITY:
//          {
//              ThreadPriorityInfo      prinfo;
//
//              if( thread == NULL ) {
//                  sprintf( buf, "Unable to get a handle for thread %08X.\n",
//                           threadid );
//                  MessageBox( hwnd, buf, "Set Priority",
//                              MB_OK | MB_ICONEXCLAMATION );
//              } else {
//                  prinfo.procid = info->procid;
//                  prinfo.thread = thread;
//                  prinfo.priority = GetThreadPriority( thread->threadhdl );
//                  prinfo.procinfo = &info->procinfo;
//                  DialogBoxParam( Instance, "THREAD_PRIORITY_DLG", hwnd,
//                                  ThreadPriorityProc, (DWORD)&prinfo );
//                  fillThreadInfo( hwnd, &info->procinfo );
//              }
//          }
//          break;
        case THREAD_LIST:
            if( HIWORD( wparam ) == LBN_SELCHANGE ) {
                fillThreadInfo( hwnd, &info->procinfo );
            }
            break;
        }
        break;
    case DR_TASK_LIST_CHANGE:
        /* make sure this process still exists */
//here  if( FindProcess( info->procid ) == NULL ) {
//here      SendDlgItemMessage( hwnd, THREAD_LIST, LB_RESETCONTENT, 0, 0L );
//here      enableChoices( hwnd, FALSE );
//here      info->proc = NULL;
//here  } else {
//here      fillThreadCtl( hwnd, info->proc, buf );
//here  }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    case WM_DESTROY:
        MemFree( info );
        ThreadDlg = NULL;
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}
