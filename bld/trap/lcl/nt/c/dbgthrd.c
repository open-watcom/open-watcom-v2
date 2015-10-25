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
* Description:  Service thread for local debugging with GUI debugger.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "srvcdbg.h"
#include "stdnt.h"
#include "trperr.h"
#include "trpimp.h"
#include "trpimpxx.h"


typedef enum {
    CTL_START,
    CTL_STOP,
    CTL_WAIT,
    CTL_CONTINUE
}   ctl_request;

static struct {
    // control overhead
    ctl_request request;
    HANDLE      requestsem;
    HANDLE      requestdonesem;
    HANDLE      hThread;
    BOOL        on_control_thread;
    BOOL        control_thread_running;
    BOOL        req_done;

    // CTL_*
    BOOL        rc;

    // CTL_START
    DWORD       pid;
    DWORD       flags;
    char        *name;

    // CTL_CONTINUE
    DWORD       how;
    DWORD       err;

}               Shared;


static void CantDoIt( void )
{
    if( PendingProgramInterrupt ) {
        if( MessageBox( 0, TRP_WIN_wanna_kill, TRP_The_WATCOM_Debugger, MB_SYSTEMMODAL + MB_YESNO + MB_ICONQUESTION ) == IDYES ) {
            Terminate();
        }
    } else if( MessageBox( 0, TRP_WIN_wanna_interrupt, TRP_The_WATCOM_Debugger, MB_SYSTEMMODAL + MB_YESNO + MB_ICONQUESTION ) == IDYES ) {
        Interrupt();
    }
}

/*
 * DoContinueDebugEvent
 */
static BOOL DoContinueDebugEvent( DWORD continue_how )
{
    SetLastError( 0 );
    if( !DidWaitForDebugEvent ) {
        return( FALSE );
    }
    return( ContinueDebugEvent( DebugeePid, LastDebugEventTid, continue_how ) );
}

static void StopDebuggee( void );
static void RequestDone( void );
static BOOL StartDebuggee( void );
static BOOL DoWaitForDebugEvent( void );

static bool DoOneControlRequest( void )
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
static int  NumInvalid = 0;


// NB: ProcessQueuedRepains() currently doesn't do anything useful
void ProcessQueuedRepaints( void )
{
    int     i;
    RECT    r;

    for( i = 0; i < NumInvalid; ++i ) {
        GetWindowRect( InvalidHWNDs[i], &r );
        InvalidateRect( InvalidHWNDs[i], &r, FALSE );
    }
    NumInvalid = 0;
}

static void ControlReq( ctl_request req )
{
    MSG     msg;
    HWND    hwnd;
    BOOL    is_dbg_wnd;
    char    buff[10];

    Shared.request = req;
    if( !Shared.control_thread_running ) {
        DoOneControlRequest(); // short circuit multithread stuff
        return;
    }
    ReleaseSemaphore( Shared.requestsem, 1, NULL );
    if( !IsWindow( DebuggerWindow ) ) {
        DebuggerWindow = NULL;
    }
    if( DebuggerWindow == NULL ) {
        WaitForSingleObject( Shared.requestdonesem, INFINITE );
    } else {
        while( !Shared.req_done ) { 
            if ( !GetMessage( &msg, NULL, 0, 0 ) )
                break;    // break on WM_QUIT, when Windows requests this. (If ever)
            hwnd = msg.hwnd;
            is_dbg_wnd = FALSE;
            while( hwnd ) {
                if( hwnd == DebuggerWindow ) {
                    is_dbg_wnd = TRUE;
                    break;
                }
                hwnd = GetParent( hwnd );
            }
            GetClassName( msg.hwnd, buff, sizeof( buff ) - 1 );
            if( !is_dbg_wnd || strcmp( buff, "WTool" ) == 0 ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            } else {
                switch( msg.message ) {
                case WM_KEYDOWN:
                    if( msg.wParam == VK_CANCEL ) {
                        Interrupt();
                    }
                    break;
                case WM_SYSKEYDOWN: // Do not activate menu on F10 single step in GUI debugger
                      if( msg.wParam == VK_F10 && !strcmp( buff, "GUIClass" ) ) {
                        break;
                      }
                      /* Allow someone to press ALT+TAB to get focus! */
                      if( msg.wParam == VK_MENU ){
                        break;
                      }
                      // fall through!
                case WM_COMMAND:
                    CantDoIt();
                    break;
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                    CantDoIt();
                    break;
                case WM_MOUSEMOVE:
                    break;
                case WM_PAINT:
                    // WM_PAINT must be sent to the target window in order
                    // to remove it from the queue 
                    DefWindowProc( msg.hwnd, msg.message, msg.wParam, msg.lParam );
                    break;
                default:
                    DefWindowProc( DebuggerWindow, msg.message, msg.wParam,
                        msg.lParam );
                }
            }
        }
        Shared.req_done = FALSE;    // Reset  
        ReleaseSemaphore( Shared.requestdonesem, 1, NULL );
    }
}

static void RequestDone( void )
{
    if( !Shared.control_thread_running ) {
        return;
    }
    Shared.on_control_thread = FALSE;
    if( !IsWindow( DebuggerWindow ) ) {
        DebuggerWindow = NULL;
    }
    if( DebuggerWindow == NULL ) {
        ReleaseSemaphore( Shared.requestdonesem, 1, NULL );
    } else {
        Shared.req_done = TRUE; 
        // Notify that something has happened, avoid delay
        PostMessage( DebuggerWindow, WM_NULL, 0, 0 ); 
        WaitForSingleObject( Shared.requestdonesem, INFINITE );
    }
}

static DWORD WINAPI ControlFunc( LPVOID parm )
{
    parm = parm;
    for( ;; ) {
        WaitForSingleObject( Shared.requestsem, INFINITE );
        if( !DoOneControlRequest() ) {
            break;
        }
    }
    return( 0 ); // thread over!
}

#pragma library(advapi32)
static BOOL MyDebugActiveProcess( DWORD dwPidToDebug )
{
    HANDLE              Token;
    TOKEN_PRIVILEGES    NewPrivileges;
    BYTE                OldPriv[1024];
    PBYTE               pbOldPriv;
    ULONG               cbNeeded;
    BOOL                b;
    BOOLEAN             fRc;
    LUID                LuidPrivilege;

    fRc = 0;
    pbOldPriv = NULL;
    //
    // Make sure we have access to adjust and to get the old token privileges
    //
    Token = NULL;
    if( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Token ) ) {

        cbNeeded = 0;
    
        //
        // Initialize the privilege adjustment structure
        //
    
        LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &LuidPrivilege );
    
        NewPrivileges.PrivilegeCount = 1;
        NewPrivileges.Privileges[0].Luid = LuidPrivilege;
        NewPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
        //
        // Enable the privilege
        //
    
        pbOldPriv = OldPriv;
        fRc = AdjustTokenPrivileges( Token, FALSE, &NewPrivileges, 1024, (PTOKEN_PRIVILEGES)pbOldPriv, &cbNeeded );
    
        if( fRc == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER ) {
    
            // 
            // If the stack was too small to hold the privileges
            // then allocate off the heap
            //
            pbOldPriv = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, cbNeeded );
            if( pbOldPriv != NULL ) {
                fRc = AdjustTokenPrivileges( Token, FALSE, &NewPrivileges, cbNeeded, (PTOKEN_PRIVILEGES)pbOldPriv, &cbNeeded );
            }
        }
    }
    if( fRc == 0 && Token != NULL ) {
        /* failed to set privilege, close privilege token handle */
        CloseHandle( Token );
    }
    b = DebugActiveProcess( dwPidToDebug );
    if( fRc != 0 ) {
        /* restore original privileges */
        AdjustTokenPrivileges( Token, FALSE, (PTOKEN_PRIVILEGES)pbOldPriv, 0, NULL, NULL );
        CloseHandle( Token );
    }
    if( pbOldPriv != NULL && pbOldPriv != OldPriv )
        LocalFree( pbOldPriv );
    return( b );
}

static int match( const char *p, const char *criterion )
{
    int result;

    result = strnicmp( p + 1, criterion, strlen( criterion ) ) == 0;
    return( result );
}

void ParseServiceStuff( char *name,
    char **pdll_name, char **pservice_name,
    char **pdll_destination, char **pservice_parm )
{
    char        *p;

    ( *pdll_name ) = "";
    ( *pservice_name ) = "";
    ( *pdll_destination ) = "";
    ( *pservice_parm ) = "";
    while( ( p = strrchr( name, LOAD_PROG_CHR_DELIM ) ) != NULL ) {
        if( match( p, LOAD_PROG_STR_DLLNAME ) ) {
            *p = '\0';
            ( *pdll_name ) = p + strlen( LOAD_PROG_STR_DLLNAME ) + 1;
            if( ( *pdll_name )[0] == '"' ) {
                ( *pdll_name )[strlen( *pdll_name ) - 1]= '\0';
                ( *pdll_name )++;
            }
        } else if( match( p, LOAD_PROG_STR_SERVICE ) ) {
            *p = '\0';
            ( *pservice_name ) = p + strlen( LOAD_PROG_STR_SERVICE ) + 1;
            if( ( *pservice_name )[0] == '"' ) {
                ( *pservice_name )[strlen( *pservice_name ) - 1]= '\0';
                ( *pservice_name )++;
            }
        } else if( match( p, LOAD_PROG_STR_SERVICEPARM ) ) {
            *p = '\0';
            ( *pservice_parm ) = p + strlen( LOAD_PROG_STR_SERVICEPARM ) + 1;
            if( ( *pservice_parm )[0] == '"' ) {
                ( *pservice_parm )[strlen( *pservice_parm ) - 1]= '\0';
                ( *pservice_parm )++;
            }
        } else if( match( p, LOAD_PROG_STR_COPYDIR ) ) {
            *p = '\0';
            ( *pdll_destination ) = p + strlen( LOAD_PROG_STR_COPYDIR ) + 1;
            if( ( *pdll_destination )[0] == '"' ) {
                ( *pdll_destination )[strlen( *pdll_destination ) - 1]= '\0';
                ( *pdll_destination )++;
            }
        } else {
            break;
        }
    }
}

typedef long( __stdcall *SELECTPROCESS ) ( char *name );

static BOOL StartDebuggee( void )
{
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;
    BOOL                rc;
    DWORD               oldErrorMode = SetErrorMode( 0 );
    HMODULE             mod;
    SELECTPROCESS       select;
    char                *dll_name;
    char                *service_name;
    char                *dll_destination;
    char                *service_parm;
    SC_HANDLE           service_manager;
    SC_HANDLE           service;
    SERVICE_STATUS      status;
    DWORD               i;
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
        DWORD               bytesNeeded;
        DWORD               servicesReturned;
        DWORD               resumeHandle = 0;

        EnumServicesStatus( service_manager, SERVICE_WIN32 + SERVICE_DRIVER, SERVICE_ACTIVE + SERVICE_INACTIVE, NULL, 0, &bytesNeeded, &servicesReturned, &resumeHandle );
        if( servicesReturned == 0 ) {
            eenum = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, bytesNeeded );
            if( eenum == NULL ) {
                rc = FALSE;
                goto failed;
            }
            EnumServicesStatus( service_manager, SERVICE_WIN32 + SERVICE_DRIVER, SERVICE_ACTIVE + SERVICE_INACTIVE, eenum, bytesNeeded, &bytesNeeded, &servicesReturned, &resumeHandle );
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
    done:
        service = OpenService( service_manager, service_name, SERVICE_ALL_ACCESS );
        if( service == NULL ) {
            AddMessagePrefix( "Unable to open the specified service", 0 );
            rc = FALSE;
            goto failed;
        }
        LocalFree( eenum );
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
                if( status.dwCurrentState == SERVICE_STOPPED )
                    break;
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
        if( FindFirstFile( buff, &dat ) != INVALID_HANDLE_VALUE ) {
           if( dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                _splitpath( dll_name, NULL, NULL, fname, ext );
                _makepath( buff, NULL, dll_destination, fname, ext );
                dll_destination = buff;
            }
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
            }
            if( !QueryServiceStatus( service, &status ) ) {
                AddMessagePrefix( "Unable to start the specified service", 0 );
                rc = FALSE;
                goto failed;
            }
            if( status.dwCurrentState == SERVICE_RUNNING )
                break;
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
            select = ( SELECTPROCESS )GetProcAddress( mod, "_SelectProcess@4" );
            if( select != NULL ) {
                if( Shared.name == NULL || Shared.name[0] == '\0' ) {
                    Shared.pid = ( *select ) ( "" );
                } else {
                    i = 0;
                    for( ;; ) {
                        if( i == 10 ) {
                            Shared.pid = ( *select ) ( "" );
                            break;
                        }
                        Shared.pid = ( *select ) ( Shared.name );
                        if( Shared.pid != 0 && Shared.pid != -1 ) {
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
    if( Shared.pid != 0 && Shared.pid != -1 ) {
        rc = MyDebugActiveProcess( Shared.pid );
#if !defined( MD_x64 )
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
                                TRUE,           /* inherit handles */
                                0,              /* creation flags */
                                NULL,           /* environment block */
                                NULL,           /* starting directory */
                                &sinfo,         /* startup info */
                                &pinfo          /* process info */
                                );
            rc = CreateProcess( NULL,           /* application name */
                                Shared.name,    /* command line */
                                NULL,           /* process attributes */
                                NULL,           /* thread attributes */
                                TRUE,           /* inherit handles */
                                0,              /* creation flags */
                                NULL,           /* environment block */
                                NULL,           /* starting directory */
                                &sinfo,         /* startup info */
                                &pinfo          /* process info */
                                );
        }
#endif
        pinfo.dwProcessId = Shared.pid;
    } else {
        memset( &sinfo, 0, sizeof( sinfo ) );
        sinfo.cb = sizeof( sinfo );
        sinfo.wShowWindow = SW_NORMAL;
        rc = CreateProcess( NULL,               /* application name */
                            Shared.name,        /* command line */
                            NULL,               /* process attributes */
                            NULL,               /* thread attributes */
                            TRUE,              /* inherit handles */
                            Shared.flags,       /* creation flags */
                            NULL,               /* environment block */
                            NULL,               /* starting directory */
                            &sinfo,             /* startup info */
                            &pinfo              /* process info */
                            );
    }
failed:
    SetErrorMode( oldErrorMode );
    Shared.pid = pinfo.dwProcessId;
    return( rc );
}

static BOOL DoWaitForDebugEvent( void )
{
    BOOL    done;
    DWORD   code;
    BOOL    rc;

    done = FALSE;

#if !defined( MD_x64 )
    UseVDMStuff = FALSE;
#endif
    while( !done ) {
        SetLastError( 0 );
        if( WaitForDebugEvent( &DebugEvent, INFINITE ) ) {
            rc = TRUE;
            DidWaitForDebugEvent = TRUE;

            if( DebugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT ) {
                code = DebugEvent.u.Exception.ExceptionRecord.ExceptionCode;
#ifdef WOW
#if !defined( MD_x64 )
                if( code == STATUS_VDM_EVENT ) {
                    BOOL    vdmrc;

                    vdmrc = pVDMProcessException( &DebugEvent );
                    if( vdmrc ) {
                        UseVDMStuff = TRUE;
                        done = TRUE;
                    } else {
                        LastDebugEventTid = DebugEvent.dwThreadId;
                        DoContinueDebugEvent( DBG_CONTINUE );
                    }
                    /*
                     * sometimes, we seem to get crap back, so the thing to do
                     * is to ignore it.  When all else fails, punt.
                     */
                } else
#endif
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
                        if( ( code & ERROR_SEVERITY_ERROR ) ==
                                ERROR_SEVERITY_ERROR ) {
                            done = TRUE;
                            break;
                        }
                        LastDebugEventTid = DebugEvent.dwThreadId;
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
}

static void StopDebuggee( void )
{
    if( DebugeePid && ( IsWOW || !DebugeeEnded ) ) {
        /*
         * we must process debug events until the process is actually
         * terminated
         */
        Slaying = TRUE;
#if !defined( MD_x64 )
        if( IsWin32s ) {
            DoContinueDebugEvent( DBG_TERMINATE_PROCESS );
            DoWaitForDebugEvent();
            DoContinueDebugEvent( DBG_CONTINUE );
        } else {
#else
        {
#endif
            HANDLE  hp;

            hp = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
            if( hp != NULL ) {
                TerminateProcess( hp, 0 );
                CloseHandle( hp );
                while( !( DebugExecute( 0, NULL, FALSE ) & COND_TERMINATE ) ) {
                }
                /*
                 * we must continue the final debug event for everything to
                 * be truly clean and wonderful
                 */
                DoContinueDebugEvent( DBG_CONTINUE );
            }
        }
        Slaying = FALSE;
    }
    DebugeePid = 0;
}

// end of seperate thread

DWORD StartControlThread( char *name, DWORD *pid, DWORD cr_flags )
{

    Shared.pid = *pid;
    Shared.flags = cr_flags;
    Shared.name = name;
    Shared.control_thread_running = FALSE;
#if !defined( MD_x64 )
    if( !IsWin32s ) {
#else
    {
#endif
        DWORD       tid;

        Shared.requestsem = CreateSemaphore( NULL, 0, 1, NULL );
        Shared.requestdonesem = CreateSemaphore( NULL, 0, 1, NULL );
        Shared.hThread = CreateThread( NULL, 0, ControlFunc, NULL, 0, &tid );
        if( Shared.hThread == NULL ) {
            MessageBox( NULL, "Error creating thread!", TRP_The_WATCOM_Debugger, MB_APPLMODAL + MB_OK );
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

void StopControlThread( void )
{
    ControlReq( CTL_STOP );
    if( Shared.control_thread_running ) {
        WaitForSingleObject( Shared.hThread, INFINITE );
        CloseHandle( Shared.requestsem );
        CloseHandle( Shared.requestdonesem );
        Shared.control_thread_running = FALSE;
    }
}
