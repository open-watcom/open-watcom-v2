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
#include <process.h>
#include "drwatcom.h"
#include "menu.h"

typedef struct {
    HWND        hwnd;
    DWORD       flags;
    char        *title;
    char        *text;
}MsgBoxInfo;

/*
 * MsgBoxMain
 */
void MsgBoxMain( void *_info ) {
    MsgBoxInfo          *info = _info;

    MessageBox( info->hwnd, info->text, info->title, info->flags );
    MemFree( info->text );
    MemFree( info->title );
    MemFree( info );
}

/*
 * DebugThdMsgBox - create a thread to put up a message box
 *                  so the debugger thread is never frozen waiting for
 *                  someone to hit OK
 */
void DebugThdMsgBox( HWND hwnd, char *text, char *title, DWORD flags ) {

    MsgBoxInfo          *info;

    info = MemAlloc( sizeof( MsgBoxInfo ) );
    info->hwnd = hwnd;
    info->flags = flags;
    info->text = MemAlloc( strlen( text ) + 1 );
    strcpy( info->text, text );
    info->title = MemAlloc( strlen( title ) + 1 );
    strcpy( info->title, title );
    _beginthread( MsgBoxMain, 0, info );
}

/*
 * sendDebugEvent
 */
static void sendDebugEvent( CommunicationBuffer *data ) {
    SendMessage( MainHwnd, DR_DEBUG_EVENT, 0, (DWORD)data );
}

/*
 * getProcessName
 */
static BOOL getProcessName( char *name ) {

    OPENFILENAME        of;
    BOOL                ret;
    char                filter[100];
    char                *ptr;

    name[0] = '\0';
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = MainHwnd;
    CopyRCString( STR_EXECUTABLE_FILTER, filter, sizeof( filter ) );
    ptr = filter + strlen( filter ) + 1;
    strcpy( ptr, "*.exe" );
    ptr += strlen( ptr ) + 1;
    *ptr = '\0';
    of.lpstrFilter = filter;
    of.lpstrDefExt = "exe";
    of.nFilterIndex = 1L;
    of.lpstrFile = name;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = AllocRCString( STR_LOAD_PROCESS );
    of.Flags = OFN_HIDEREADONLY;
    ret = GetOpenFileName( &of );
    FreeRCString( (char *)of.lpstrTitle );
    return( ret );
}

/*
 * addRunningProcess -
 */
static BOOL addRunningProcess( DWORD pid ) {

    BOOL        rc;
    DWORD       process_status;
    HANDLE      prochdl;

    if( pid == GetCurrentProcessId() ) {
        return( TRUE );
    } else {
        SetDebugErrorLevel( SLE_WARNING );
        Sleep( 500 );
        prochdl = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, pid );
        if( prochdl == NULL ) {
//          MessageBox( NULL, "OpenProcess failed", "", MB_OK );
            return( TRUE );
        }
        rc = GetExitCodeProcess( prochdl, &process_status );
        CloseHandle( prochdl );
        if( !rc ) {
//          MessageBox( NULL, "get status failed", "", MB_OK );
            return( TRUE );
        }
        if( process_status != STILL_ACTIVE ) {
//          MessageBox( NULL, "Process was not still active", "", MB_OK );
            return( TRUE );
        }
        if( !DebugActiveProcess( pid ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

/*
 * DebuggerMain - thread that acts as the debugger for attatched processes
 */
void DebuggerMain( void *_info ) {

    ProcAttatchInfo            *info = _info;
    STARTUPINFO                 startup;
    PROCESS_INFORMATION         procinfo;
    CommunicationBuffer         data;
    BOOL                        error;
    BOOL                        ret;
    char                        buf[256];

    error = FALSE;

    /* start the new process or attatch to it */
    if( info->type == MENU_NEW_TASK ) {
        if( info->info.path == NULL ) {
            info->info.path = MemAlloc( _MAX_PATH );
            if( !getProcessName( info->info.path ) ) {
                MemFree( info->info.path );
                return;
            }
        }
        memset( &startup, 0, sizeof( STARTUPINFO ) );
        startup.cb = sizeof( STARTUPINFO );
        ret = CreateProcess( NULL,              /* application path */
                       info->info.path,         /* command line */
                       NULL,                    /* process security
                                                   attributes */
                       NULL,                    /* main thread security
                                                   attributes */
                       FALSE,                   /* inherits parent handles */
                       DEBUG_PROCESS |          /* create parameters */
                       NORMAL_PRIORITY_CLASS,
                       NULL,                    /* environment block */
                       NULL,                    /* current directory */
                       &startup,                /* other startup info */
                       &procinfo );             /* structure to get process
                                                   info */
        if( !ret ) {
            if( info->errhdler == NULL ) {
                RCsprintf( buf, STR_CANT_CREATE_PROCESS, info->info.path );
                MessageBox( NULL, buf, AppName, MB_OK | MB_ICONEXCLAMATION
                                                | MB_SETFOREGROUND );
            }
            error = TRUE;
        }
    } else if( info->type == MENU_ADD_RUNNING ) {
        error = addRunningProcess( info->info.pid );
    } else {
        error = TRUE;
    }
    if( error && info->errhdler != NULL ) {
        info->errhdler( info );
    }
    if( !error ) {
        for( ;; ) {
            if( !WaitForDebugEvent( &data.dbginfo, INFINITE ) ) {
#ifdef DEBUG
                sprintf( cmdline, "bad event %ld", GetLastError() );
                DebugThdMsgBox( NULL, cmdline, "", MB_OK );
#endif
            }
            sendDebugEvent( &data );
            ContinueDebugEvent( data.dbginfo.dwProcessId,
                                data.dbginfo.dwThreadId,
                                data.action );
            if( data.dbginfo.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT ) {
                break;
            }
        }
    }
    if( info->type == MENU_NEW_TASK && info->info.path != NULL ) {
        MemFree( info->info.path );
    }
    MemFree( info );
}

/*
 * CallProcCtl
 */
void CallProcCtl( DWORD event, void *info, void (*hdler)(void *) )
{
    ProcAttatchInfo     *threadinfo;

    threadinfo = MemAlloc( sizeof( ProcAttatchInfo ) );
    threadinfo->type = event;
    switch( event ) {
    case MENU_ADD_RUNNING:
        threadinfo->info.pid = *(DWORD *)info;
        break;
    case MENU_NEW_TASK:
        threadinfo->info.path = info;
        break;
    }
    threadinfo->errhdler = hdler;
    _beginthread( DebuggerMain, 0, threadinfo );
}
