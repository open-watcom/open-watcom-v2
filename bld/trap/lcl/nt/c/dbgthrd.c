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
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "srvcdbg.h"
#include "stdnt.h"
#include "trperr.h"

typedef enum {
    CTL_START,
    CTL_STOP,
    CTL_WAIT,
    CTL_CONTINUE,
} ctl_request;

struct {
    // control overhead
    ctl_request         request;
    HANDLE              requestsem;
    HANDLE              requestdonesem;
    HANDLE              hThread;
    BOOL                on_control_thread;
    BOOL                control_thread_running;

    // CTL_*
    BOOL                rc;

    // CTL_START
    DWORD               pid;
    DWORD               flags;
    char                *name;

    // CTL_CONTINUE
    DWORD               how;
    DWORD               err;

} Shared;

#if 0
#define MAX_PAINTS      100
struct {
    RECT        rect;
    HWND        hwnd;
}               paints[MAX_PAINTS];
#endif

static void CantDoIt()
{
    if( PendingProgramInterrupt ) {
        if( MessageBox( NULL, TRP_WIN_wanna_kill, TRP_The_WATCOM_Debugger,
                    MB_SYSTEMMODAL+MB_YESNO+MB_ICONQUESTION ) == IDYES ) {
            Terminate();
        }
    } else if( MessageBox( NULL, TRP_WIN_wanna_interrupt, TRP_The_WATCOM_Debugger,
                    MB_SYSTEMMODAL+MB_YESNO+MB_ICONQUESTION ) == IDYES ) {
        InterruptProgram();
    }
}


/*
 * DoContinueDebugEvent
 */
static BOOL DoContinueDebugEvent( DWORD continue_how )
{
    SetLastError( 0 );
    if( !DidWaitForDebugEvent ) return( FALSE );
    return( ContinueDebugEvent( DebugeePid, LastDebugEventTid, continue_how ) );
} /* DoContinueDebugEvent */

static bool DoOneControlRequest()
{
    Shared.on_control_thread = TRUE;
    if( Shared.request == CTL_STOP ) {
        StopDebuggee();
        RequestDone();
        return( FALSE );
    } else if( Shared.request == CTL_START ) {
        Shared.err = 0;
        if( !StartDebuggee() ) {
            Shared.err = GetLastError();
        }
        RequestDone();
    } else if( Shared.request == CTL_WAIT ) {
        Shared.rc = DoWaitForDebugEvent();
        RequestDone();
    } else if( Shared.request == CTL_CONTINUE ) {
        DoContinueDebugEvent( Shared.how );
        RequestDone();
    }
    return( TRUE );
}

#define MAX_HWNDS 40 // maximum number of hwnds in the debugger
static HWND InvalidHWNDs[MAX_HWNDS];
static int NumInvalid = 0;

static void RecordPaint( HWND hwnd )
{
    int i;

    for( i = 0; i < NumInvalid; ++i ) {
        if( InvalidHWNDs[i] == hwnd ) return;
    }
    if( NumInvalid == MAX_HWNDS ) return;
    InvalidHWNDs[NumInvalid] = hwnd;
    ++NumInvalid;
}

void ProcessQueuedRepaints()
{
    int i;
    RECT r;

    for( i = 0; i < NumInvalid; ++i ) {
        GetWindowRect( InvalidHWNDs[i], &r );
        InvalidateRect( InvalidHWNDs[i], &r, FALSE );
    }
    NumInvalid = 0;
}

static void ControlReq( ctl_request req )
{
    MSG msg;
    HWND        hwnd;
    BOOL        is_dbg_wnd;
    char        buff[10];
    int         num_paints;

    Shared.request = req;
    if( !Shared.control_thread_running ) {
        DoOneControlRequest(); // short circuit multithread stuff
        return;
    }
    ReleaseSemaphore( Shared.requestsem, 1, NULL );
    if( !IsWindow( DebuggerWindow ) ) DebuggerWindow = NULL;
    if( DebuggerWindow == NULL ) {
        WaitForSingleObject( Shared.requestdonesem, INFINITE );
    } else {
        num_paints = 0;
        while( GetMessage( &msg, NULL, 0, 0 ) ) {
            hwnd = msg.hwnd;
            is_dbg_wnd = FALSE;
            while( hwnd ) {
                if( hwnd == DebuggerWindow ) {
                    is_dbg_wnd = TRUE;
                    break;
                }
                hwnd = GetParent( hwnd );
            }
            GetClassName( msg.hwnd, buff, sizeof( buff )-1 );
            if( !is_dbg_wnd || strcmp( buff, "WTool" ) == 0 ) {
                if( msg.hwnd == NULL && msg.message == WM_QUIT ) break;
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            } else {
                switch( msg.message ) {
                case WM_KEYDOWN:
                    if( msg.wParam == VK_CANCEL ) {
                        InterruptProgram();
                    }
                    break;
                case WM_COMMAND:
                    CantDoIt();
                    break;
                case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    RecordPaint( msg.hwnd );
                    BeginPaint( msg.hwnd, &ps );
                    EndPaint( msg.hwnd, &ps );
                    break;
                }
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                    CantDoIt();
                    break;
                case WM_MOUSEMOVE:
                    break;
                default:
                    DefWindowProc( DebuggerWindow, msg.message, msg.wParam, msg.lParam );
                }
            }
        }
        ProcessQueuedRepaints();
        ReleaseSemaphore( Shared.requestdonesem, 1, NULL );
    }
}

void RequestDone()
{
    if( !Shared.control_thread_running ) return;
    Shared.on_control_thread = FALSE;
    if( !IsWindow( DebuggerWindow ) ) DebuggerWindow = NULL;
    if( DebuggerWindow == NULL ) {
        ReleaseSemaphore( Shared.requestdonesem, 1, NULL );
    } else {
        PostMessage( DebuggerWindow, WM_QUIT, 0, 0 );
        WaitForSingleObject( Shared.requestdonesem, INFINITE );
    }
}

DWORD WINAPI ControlFunc( void *parm )
{
    parm = parm;
    for( ;; ) {
        WaitForSingleObject( Shared.requestsem, INFINITE );
        if( !DoOneControlRequest() ) break;
    }
    return( 0 ); // thread over!
}

#pragma library(advapi32)
BOOL
MyDebugActiveProcess (
    DWORD dwPidToDebug
    )
{
    HANDLE              Token;
    PTOKEN_PRIVILEGES   NewPrivileges;
    BYTE                OldPriv[1024];
    PBYTE               pbOldPriv;
    ULONG               cbNeeded;
    BOOL                b;
    BOOLEAN             fRc;
    LUID                LuidPrivilege;

    //
    // Make sure we have access to adjust and to get the old token privileges
    //
    if (!OpenProcessToken( GetCurrentProcess(),
                           TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                           &Token)) {

        goto done;

    }

    cbNeeded = 0;

    //
    // Initialize the privilege adjustment structure
    //

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &LuidPrivilege );

    NewPrivileges = (PTOKEN_PRIVILEGES)calloc(1,sizeof(TOKEN_PRIVILEGES)+
                                              (1 - ANYSIZE_ARRAY) * sizeof(LUID_AND_ATTRIBUTES));
    if (NewPrivileges == NULL) {
        CloseHandle(Token);
        goto done;
    }

    NewPrivileges->PrivilegeCount = 1;
    NewPrivileges->Privileges[0].Luid = LuidPrivilege;
    NewPrivileges->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //
    // Enable the privilege
    //

    pbOldPriv = OldPriv;
    fRc = AdjustTokenPrivileges( Token,
                                 FALSE,
                                 NewPrivileges,
                                 1024,
                                 (PTOKEN_PRIVILEGES)pbOldPriv,
                                 &cbNeeded );

    if (!fRc) {

        //
        // If the stack was too small to hold the privileges
        // then allocate off the heap
        //
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {

            pbOldPriv = calloc(1,cbNeeded);
            if (pbOldPriv == NULL) {
                CloseHandle(Token);
                goto done;
            }

            fRc = AdjustTokenPrivileges( Token,
                                         FALSE,
                                         NewPrivileges,
                                         cbNeeded,
                                         (PTOKEN_PRIVILEGES)pbOldPriv,
                                         &cbNeeded );
        }
    }

    b = DebugActiveProcess(dwPidToDebug);

    CloseHandle( Token );

    return( b );
done:;
    return( DebugActiveProcess( dwPidToDebug ) );
}


void ParseServiceStuff( char *name,
                        char *(*pdll_name),
                        char *(*pservice_name),
                        char *(*pdll_destination),
                        char *(*pservice_parm) )
{
    char *p;
    (*pdll_name) = ""; (*pservice_name) = ""; (*pdll_destination) = ""; (*pservice_parm) = "";
    while( ( p = strrchr( name, LOAD_PROG_CHR_DELIM ) ) != NULL ) {
        if( strnicmp( p+1, LOAD_PROG_STR_DLLNAME, strlen( LOAD_PROG_STR_DLLNAME ) ) == 0 ) {
            *p = '\0';
            (*pdll_name) = p + strlen( LOAD_PROG_STR_DLLNAME ) + 1;
            if( (*pdll_name)[0] == '"' ) {
                (*pdll_name)[strlen((*pdll_name))-1]='\0';
                (*pdll_name)++;
            }
        } else if( strnicmp( p+1, LOAD_PROG_STR_SERVICE, strlen( LOAD_PROG_STR_SERVICE ) ) == 0 ) {
            *p = '\0';
            (*pservice_name) = p + strlen( LOAD_PROG_STR_SERVICE ) + 1;
            if( (*pservice_name)[0] == '"' ) {
                (*pservice_name)[strlen((*pservice_name))-1]='\0';
                (*pservice_name)++;
            }
        } else if( strnicmp( p+1, LOAD_PROG_STR_SERVICEPARM, strlen( LOAD_PROG_STR_SERVICEPARM ) ) == 0 ) {
            *p = '\0';
            (*pservice_parm) = p + strlen( LOAD_PROG_STR_SERVICEPARM ) + 1;
            if( (*pservice_parm)[0] == '"' ) {
                (*pservice_parm)[strlen((*pservice_parm))-1]='\0';
                (*pservice_parm)++;
            }
        } else if( strnicmp( p+1, LOAD_PROG_STR_COPYDIR, strlen( LOAD_PROG_STR_COPYDIR ) ) == 0 ) {
            *p = '\0';
            (*pdll_destination) = p + strlen( LOAD_PROG_STR_COPYDIR ) + 1;
            if( (*pdll_destination)[0] == '"' ) {
                (*pdll_destination)[strlen((*pdll_destination))-1]='\0';
                (*pdll_destination)++;
            }
        } else {
            break;
        }
    }
}

typedef long (__stdcall * SELECTPROCESS)( char *name );
BOOL StartDebuggee()
{
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;
    BOOL                rc;
    DWORD               oldErrorMode = SetErrorMode( 0 );
    HMODULE             mod;
    SELECTPROCESS       select;
    char                *dll_name, *service_name, *dll_destination, *service_parm;
    SC_HANDLE           service_manager;
    SC_HANDLE           service;
    SERVICE_STATUS      status;
    int                 i;
    char                buff[_MAX_PATH];
    char                fname[_MAX_FNAME];
    char                ext[_MAX_EXT];

    ParseServiceStuff( Shared.name, &dll_name, &service_name, &dll_destination, &service_parm );
    service = NULL;
    service_manager = NULL;
    if( service_name[0] ) {
        service_manager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
        if( service_manager == NULL ) {
            AddMessagePrefix( "Unable to open service manager", 0 );
            rc = FALSE;
            goto failed;
        }
    }
    if( service_manager != NULL ) {
        ENUM_SERVICE_STATUS *eenum = NULL;
        DWORD bytesNeeded, servicesReturned, resumeHandle = 0;
        EnumServicesStatus( service_manager, SERVICE_WIN32+SERVICE_DRIVER,
                            SERVICE_ACTIVE+SERVICE_INACTIVE,
                            NULL, 0, &bytesNeeded, &servicesReturned,
                            &resumeHandle );
        if( servicesReturned == 0 ) {
            eenum = calloc( 1, bytesNeeded );
            EnumServicesStatus( service_manager, SERVICE_WIN32+SERVICE_DRIVER,
                                SERVICE_ACTIVE+SERVICE_INACTIVE,
                                eenum, bytesNeeded, &bytesNeeded, &servicesReturned,
                                &resumeHandle );
            for( i = 0; i < servicesReturned; ++i ) {
                strlwr( eenum[i].lpServiceName );
                strlwr( eenum[i].lpDisplayName );
            }
            strlwr( service_name );
            for( i = 0; i < servicesReturned; ++i ) {
                if( strcmp( eenum[i].lpServiceName, service_name ) == 0 ) {
                    service_name = eenum[i].lpServiceName;
                    goto done;
                }
            }

            for( i = 0; i < servicesReturned; ++i ) {
                if( strcmp( eenum[i].lpDisplayName, service_name ) == 0 ) {
                    service_name = eenum[i].lpServiceName;
                    goto done;
                }
            }

            for( i = 0; i < servicesReturned; ++i ) {
                if( strstr( eenum[i].lpServiceName, service_name ) != 0 ) {
                    service_name = eenum[i].lpServiceName;
                    goto done;
                }
            }

            for( i = 0; i < servicesReturned; ++i ) {
                if( strstr( eenum[i].lpDisplayName, service_name ) != 0 ) {
                    service_name = eenum[i].lpServiceName;
                    goto done;
                }
            }


        }
done:;
        service = OpenService( service_manager, service_name, SERVICE_ALL_ACCESS );
        if( service == NULL ) {
            AddMessagePrefix( "Unable to open the specified service", 0 );
            rc = FALSE;
            goto failed;
        }
        free( eenum );
    }

    if( service != NULL ) {
        if( ControlService( service, SERVICE_CONTROL_STOP, &status ) ) {
            i = 0;
            for( ;; ) {
                if( i == 40 ) {
                    AddMessagePrefix( "Unable to stop the specified service", 0 );
                    Shared.err = ERROR_SERVICE_REQUEST_TIMEOUT;
                    goto failed;
                }
                if( !QueryServiceStatus( service, &status ) ) {
                    AddMessagePrefix( "Unable to stop the specified service", 0 );
                    rc = FALSE;
                    goto failed;
                }
                if( status.dwCurrentState == SERVICE_STOPPED ) break;
                Sleep( 500 );
                ++i;
            }
        }
    }

    if( dll_name[0] && dll_destination[0] ) {
        char *end;
        WIN32_FIND_DATA dat;
        strcpy( buff, dll_destination );
        end = buff + strlen( buff ) - 1;
        if( *end != '\\' && *end != '/' ) {
            strcat( buff, "\\" );
        }
        strcat( buff, "." );
        if( FindFirstFile( buff, &dat ) != INVALID_HANDLE_VALUE && ( dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
            _splitpath( dll_name, NULL, NULL, fname, ext );
            _makepath( buff, NULL, dll_destination, fname, ext );
            dll_destination = buff;
        }
        if( !CopyFile( dll_name, dll_destination, FALSE ) ) {
            AddMessagePrefix( "Unable to copy '", 3000 );
            strcat( MsgPrefix, dll_name );
            strcat( MsgPrefix, "' to '" );
            strcat( MsgPrefix, dll_destination );
            strcat( MsgPrefix, "'" );
            rc = FALSE;
            goto failed;
        }
    }

    if( service != NULL ) {
        char const *parm[2];
        parm[0] = service_parm;
        parm[1] = NULL;
        StartService( service, 0, parm );
        i = 0;
        for( ;; ) {
            if( i == 40 ) {
                AddMessagePrefix( "Unable to start the specified service", 0 );
                Shared.err = ERROR_SERVICE_REQUEST_TIMEOUT;
                goto failed;
                break;
            }
            if( !QueryServiceStatus( service, &status ) ) {
                AddMessagePrefix( "Unable to start the specified service", 0 );
                rc = FALSE;
                goto failed;
                break;
            }
            if( status.dwCurrentState == SERVICE_RUNNING ) break;
            Sleep( 500 );
            ++i;
        }
        CloseServiceHandle( service );
        Sleep( 1000 ); // just in case it's slow!
    }
    if( service_manager != NULL ) {
        CloseServiceHandle( service_manager );
    }

    if( Shared.pid == -1 ) {
        mod = LoadLibrary( "PView.dll" );
        if( mod != NULL ) {
            select = (SELECTPROCESS)GetProcAddress( mod, "_SelectProcess@4" );
            if( select != NULL ) {
                if( Shared.name == NULL || Shared.name[0] == '\0' ) {
                    Shared.pid = (*select)( "" );
                } else {
                    i = 0;
                    for( ;; ) {
                        if( i == 10 ) {
                            Shared.pid = (*select)( "" );
                            break;
                        }
                        Shared.pid = (*select)( Shared.name );
                        if( Shared.pid != NULL && Shared.pid != -1 ) {
                            break;
                        }
                        Sleep( 500 );
                        ++i;
                    }
                }
            }
        }
        CloseHandle( mod );
    }
    if( Shared.pid != NULL && Shared.pid != -1 ) {
        rc = MyDebugActiveProcess( Shared.pid );
        if( IsWOW ) {
            /*
             * WOW was already running, so we start up wowdeb (this
             * is used in debugging WOW, provided with NT), and
             * then we do a "CreateProcess" on the WOW app.  Since WOW
             * is already running, this doesn't really create a process,
             * but instead has the existing WOW start the 16-bit task.
             */
            memset( &sinfo, 0, sizeof( sinfo ) );
            sinfo.cb = sizeof( sinfo );
            sinfo.wShowWindow = SW_NORMAL;
            rc = CreateProcess( NULL,           /* application name */
                                "wowdeb.exe",   /* command line */
                                NULL,           /* process attributes */
                                NULL,           /* thread attributes */
                                FALSE,          /* inherit handles */
                                0,              /* creation flags */
                                NULL,           /* environment block */
                                NULL,           /* starting directory */
                                &sinfo,         /* startup info */
                                &pinfo          /* process info */
                                );
            rc = CreateProcess( NULL,           /* application name */
                                Shared.name,           /* command line */
                                NULL,           /* process attributes */
                                NULL,           /* thread attributes */
                                FALSE,          /* inherit handles */
                                0,              /* creation flags */
                                NULL,           /* environment block */
                                NULL,           /* starting directory */
                                &sinfo,         /* startup info */
                                &pinfo          /* process info */
                                );
        }
        pinfo.dwProcessId = Shared.pid;
    } else {
        memset( &sinfo, 0, sizeof( sinfo ) );
        sinfo.cb = sizeof( sinfo );
        sinfo.wShowWindow = SW_NORMAL;
        rc = CreateProcess( NULL,               /* application name */
                            Shared.name,               /* command line */
                            NULL,               /* process attributes */
                            NULL,               /* thread attributes */
                            FALSE,              /* inherit handles */
                            Shared.flags,           /* creation flags */
                            NULL,               /* environment block */
                            NULL,               /* starting directory */
                            &sinfo,             /* startup info */
                            &pinfo              /* process info */
                            );
    }
failed:;
    SetErrorMode( oldErrorMode );
    Shared.pid = pinfo.dwProcessId;
    return( rc );
}

BOOL DoWaitForDebugEvent( void )
{
    BOOL        done;
    DWORD       code;
    BOOL        rc;

    done = FALSE;

    UseVDMStuff = FALSE;
    while( !done ) {
        SetLastError( 0 );
        if( WaitForDebugEvent( &DebugEvent, INFINITE ) ) {
            rc = TRUE;
            DidWaitForDebugEvent = TRUE;

            if( DebugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT ) {
                code = DebugEvent.u.Exception.ExceptionRecord.ExceptionCode;
    #ifdef WOW
                if( code == STATUS_VDM_EVENT ) {
                    BOOL    vdmrc;

                    vdmrc = pVDMProcessException( &DebugEvent );
                    if( vdmrc ) {
                        UseVDMStuff = TRUE;
                        done = TRUE;
                    } else {
                        SetDebugeeTid();
                        DoContinueDebugEvent( DBG_CONTINUE );
                    }
                /*
                 * sometimes, we seem to get crap back, so the thing to do
                 * is to ignore it.  When all else fails, punt.
                 */
                } else
    #endif
                       {
                    switch( code ) {
                    case STATUS_DATATYPE_MISALIGNMENT:
                    case STATUS_BREAKPOINT:
                    case STATUS_SINGLE_STEP:
                    case STATUS_ACCESS_VIOLATION:
                    case STATUS_IN_PAGE_ERROR:
                    case STATUS_NO_MEMORY:
                    case STATUS_ILLEGAL_INSTRUCTION:
                    case STATUS_NONCONTINUABLE_EXCEPTION:
                    case STATUS_INVALID_DISPOSITION:
                    case STATUS_ARRAY_BOUNDS_EXCEEDED:
                    case STATUS_FLOAT_DENORMAL_OPERAND:
                    case STATUS_FLOAT_DIVIDE_BY_ZERO:
                    case STATUS_FLOAT_INVALID_OPERATION:
                    case STATUS_FLOAT_OVERFLOW:
                    case STATUS_FLOAT_STACK_CHECK:
                    case STATUS_FLOAT_UNDERFLOW:
                    case STATUS_INTEGER_DIVIDE_BY_ZERO:
                    case STATUS_INTEGER_OVERFLOW:
                    case STATUS_PRIVILEGED_INSTRUCTION:
                    case STATUS_STACK_OVERFLOW:
                    case STATUS_CONTROL_C_EXIT:
                        done = TRUE;
                        break;
                    default:
                        if( ( code & ERROR_SEVERITY_ERROR ) == ERROR_SEVERITY_ERROR ) {
                            done = TRUE;
                            break;
                        }
                        SetDebugeeTid();
                        DoContinueDebugEvent( DBG_EXCEPTION_NOT_HANDLED );
                        break;
                    }
                }
            } else {
                done = TRUE;
            }
        } else {
            rc = FALSE;
            break;
        }
    }
    return( rc );
} /* DoWaitForDebugEvent */


StopDebuggee()
{

    if( DebugeePid != NULL && (IsWOW || !DebugeeEnded) ) {
        /*
         * we must process debug events until the process is actually
         * terminated
         */
        Slaying = TRUE;
        if( IsWin32s ) {
            DoContinueDebugEvent( DBG_TERMINATE_PROCESS );
            DoWaitForDebugEvent();
            DoContinueDebugEvent( DBG_CONTINUE );
        } else {
            HANDLE      hp;
            hp = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
            if( hp != NULL ) {
                TerminateProcess( hp, 0 );
                CloseHandle( hp );
                while( !(DebugExecute( 0, NULL, FALSE ) & COND_TERMINATE) ) {}
                /*
                 * we must continue the final debug event for everything to
                 * be truly clean and wonderful
                 */
                DoContinueDebugEvent( DBG_CONTINUE );
            }
        }
        Slaying = FALSE;
    }
    DebugeePid = NULL;
}

// end of seperate thread

DWORD StartControlThread( char *name, DWORD *pid, DWORD cr_flags )
{

    Shared.pid = *pid;
    Shared.flags = cr_flags;
    Shared.name = name;
    Shared.control_thread_running = FALSE;
    if( !IsWin32s ) {
        DWORD       tid;

        Shared.requestsem = CreateSemaphore( NULL, 0, 1, NULL );
        Shared.requestdonesem = CreateSemaphore( NULL, 0, 1, NULL );
        Shared.hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ControlFunc, NULL, 0, &tid );
        if (Shared.hThread == NULL) {
            MessageBox( NULL, "Error creating thread!", TRP_The_WATCOM_Debugger, MB_APPLMODAL+MB_OK );
        }
        Shared.control_thread_running = TRUE;
    }
    ControlReq( CTL_START );
    *pid = Shared.pid;
    return( Shared.err );
}

/*
 * MyWaitForDebugEvent - wait for a debug event.  Only return meaningful
 *                       VDM debug events
 */
BOOL MyWaitForDebugEvent( void )
{
    if( Shared.on_control_thread ) {
        return( DoWaitForDebugEvent() );
    }
    ControlReq( CTL_WAIT );
    return( Shared.rc );
}

void MyContinueDebugEvent( int continue_how )
{
    if( Shared.on_control_thread ) {
        DoContinueDebugEvent( continue_how );
        return;
    }
    Shared.how = continue_how;
    ControlReq( CTL_CONTINUE );
}

void StopControlThread()
{
    ControlReq( CTL_STOP );
    if( Shared.control_thread_running ) {
        WaitForSingleObject( Shared.hThread, INFINITE );
        CloseHandle( Shared.requestsem );
        CloseHandle( Shared.requestdonesem );
        Shared.control_thread_running = FALSE;
    }
}
