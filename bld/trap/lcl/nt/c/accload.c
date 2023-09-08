/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "stdnt.h"
#include "globals.h"
#include "trperr.h"
#include "srvcdbg.h"
#include "doserr.h"
#include "ntpath.h"
#include "segmcpu.h"

#include "clibext.h"


static bool executeUntilStart( bool was_running )
/************************************************
 * run program until start address hit
 */
{
    HANDLE      ph;
    opcode_type old_opcode;
    FARPROC     base;
    MYCONTEXT   con;
    thread_info *ti;
    bool        done;

    ph = DebugEvent.u.CreateProcessInfo.hProcess;
    if( was_running ) {
        /*
         * a trick to make app execute long enough to hit a breakpoint
         */
        PostMessage( HWND_TOPMOST, WM_NULL, 0, 0L );
    } else {
        /*
         * if we are not debugging an already running app, then we
         * plant a breakpoint at the first instruction of our new app
         */
        base = (FARPROC)DebugEvent.u.CreateProcessInfo.lpStartAddress;
        old_opcode = place_breakpoint_lin( ph, base );
    }

    done = false;
    do {
        /*
         * if we encounter anything but a break point, then we are in
         * trouble!
         */
        if( (DebugExecute( STATE_IGNORE_DEBUG_OUT | STATE_IGNORE_DEAD_THREAD, NULL, false ) & COND_BREAK) == 0 )
            return( false );
        ti = FindThread( DebugEvent.dwThreadId );
        MyGetThreadContext( ti, &con );
        if( was_running ) {
            done = true;
        } else if( StopForDLLs ) {
            /*
             * the user has asked us to stop before any DLL's run
             * their startup code (";dll"), so we do.
             */
            remove_breakpoint_lin( ph, base, old_opcode );
            done = true;
        } else if( GetIP( &con ) == base ) {
            /*
             * we stopped at the applications starting address,
             * so we can offically declare that the app has loaded
             */
            remove_breakpoint_lin( ph, base, old_opcode );
            break;
        }
        /*
         * skip this breakpoint and continue
         */
        AdjustIP( &con, sizeof( opcode_type ) );
        MySetThreadContext( ti, &con );
    } while( !done );
    return( true );
}

#ifdef WOW
#if MADARCH & MADARCH_X86
static void addKERNEL( void )
/****************************
 * add the KERNEL module to the library load (WOW)
 */
{
  #if 0
    /*
     * there are bugs in the way VDMDBG.DLL implements some of this
     * stuff, so this is currently disabled
     */
    MODULEENTRY                 me;
    thread_info                 *ti;
    IMAGE_NOTE                  im;
    int                         rc;

    ti = FindThread( DebugeeTid );
    me.dwSize = sizeof( MODULEENTRY );
    for( rc = pVDMModuleFirst( ProcessInfo.process_handle, ti->thread_handle, &me, NULL, 0 );
         rc != 0;
         rc = pVDMModuleNext( ProcessInfo.process_handle, ti->thread_handle, &me, NULL, 0 ) )
    {
        if( strnicmp( me.szModule, "KERNEL", 6 ) == 0 ) {
            memcpy( &im.Module, &me.szModule, sizeof( me.szModule ) );
            memcpy( &im.FileName, &me.szExePath, sizeof( me.szExePath ) );
            AddLib16( &im );
            break;
        }
        me.dwSize = sizeof( MODULEENTRY );
    }
  #else
    IMAGE_NOTE                  im;

    /*
     * this is a giant kludge, but it works.  Since KERNEL is already
     * loaded in the WOW , we never get a DLL load notification, so
     * we can't show any symbols.  This fakes up the necessary information
     */
    strcpy( im.Module, "KERNEL" );
    GetSystemDirectory( im.FileName, sizeof( im.FileName ) );
    strcat( im.FileName, "\\KRNL386.EXE" );
    AddLib16( &im );
  #endif
}

static void addAllWOWModules( void )
/***********************************
 * add all modules as libraries.  This is invoked if
 * WOW was already running, since we will get no
 * lib load notifications if it was.
 */
{
    MODULEENTRY         me;
    thread_info         *ti;
    IMAGE_NOTE          im;
    int                 rc;

    ti = FindThread( DebugeeTid );
    me.dwSize = sizeof( MODULEENTRY );
    for( rc = pVDMModuleFirst( ProcessInfo.process_handle, ti->thread_handle, &me, NULL, 0 );
         rc != 0 && strcmp( me.szModule, WOWAppInfo.modname ) != 0;
         rc = pVDMModuleNext( ProcessInfo.process_handle, ti->thread_handle, &me, NULL, 0 ) )
    {
        memcpy( &im.Module, &me.szModule, sizeof( me.szModule ) );
        memcpy( &im.FileName, &me.szExePath, sizeof( me.szExePath ) );
        AddLib16( &im );
        me.dwSize = sizeof( MODULEENTRY );
    }

}

static BOOL WINAPI EnumWOWProcessFunc( DWORD pid, DWORD attrib, LPARAM lparam )
/******************************************************************************
 * EnumWOWProcessFunc - callback for each WOW process in the system
 */
{
    if( attrib & WOW_SYSTEM ) {
        *(DWORD *)lparam = pid;
        return( FALSE );
    }
    return( TRUE );

}
#endif  /* MADARCH & MADARCH_X86 */
#endif  /* WOW */

static size_t MergeArgvArray( const char *src, char *dst, size_t len )
{
    char    ch;
    char    *start = dst;

    while( len-- > 0 ) {
        ch = *src++;
        if( ch == '\0' ) {
            if( len == 0 )
                break;
            ch = ' ';
        }
        *dst++ = ch;
    }
    *dst = '\0';
    return( dst - start );
}

trap_retval TRAP_CORE( Prog_load )( void )
/*****************************************
 * create a new process for debugging
 */
{
    char            *parm;
    char            *src;
    char            *dst;
    char            *endsrc;
    char            exe_name[PATH_MAX];
    MYCONTEXT       con;
    thread_info     *ti;
    HANDLE          handle;
    prog_load_req   *acc;
    prog_load_ret   *ret;
    header_info     hi;
    WORD            stack;
    DWORD           pid;
    DWORD           pid_started;
    DWORD           cr_flags;
    char            *buff;
    size_t          nBuffRequired;
    char            *dll_name;
    char            *service_name;
    char            *dll_destination;
    char            *service_parm;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    parm = GetInPtr( sizeof( *acc ) );
    /*
     * reset status variables
     */
    LastExceptionCode = -1;
    DebugString = NULL;
    DebugeeEnded = false;
    RemoveAllThreads();
    FreeLibList();
    DidWaitForDebugEvent = false;
    DebugeePid = 0;
    DebugeeTid = 0;
    SupportingExactBreakpoints = false;
    /*
     * check if pid is specified
     */
    ParseServiceStuff( parm, &dll_name, &service_name, &dll_destination, &service_parm );
    pid = 0;
    src = parm;
    /*
     *  Just to be really safe!
     */
    nBuffRequired = GetTotalSizeIn() + PATH_MAX + 16;
    buff = LocalAlloc( LMEM_FIXED, nBuffRequired );
    if( buff == NULL ) {
        ret->err = ERROR_NOT_ENOUGH_MEMORY;
        return( sizeof( *ret ) );
    }

    if( *src == '#' ) {
        src++;
        pid = strtoul( src, &endsrc, 16 );
        if( pid == 0 ) {
            pid = -1;
        }
        strcpy( buff, endsrc );
    } else {
        while( isdigit( *src ) ) {
            src++;
        }
        if( *src == '\0' && src != parm ) {
            pid = atoi( parm );
        }
    }
    /*
     * get program to debug.  If the user has specified a pid, then
     * skip directly to doing a DebugActiveProcess
     */
    handle = INVALID_HANDLE_VALUE;
#if MADARCH & MADARCH_X64
    IsWOW64 = false;
#elif defined( WOW )
    IsWOW = false;
    IsDOS = false;
#endif
    if( pid == 0 ) {
        if( FindFilePath( DIG_FILETYPE_EXE, parm, exe_name ) == 0 ) {
            ret->err = ENOENT;
            goto error_exit;
        }
        /*
         * Get type of application
         */
        handle = CreateFile( exe_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
        if( handle == INVALID_HANDLE_VALUE ) {
            goto error_exit;
        }
        GetFullPathName( exe_name, MAX_PATH, CurrEXEName, NULL );
        /*
         * get the parm list
         */
        if( strchr( CurrEXEName, ' ' ) != NULL ) {
            dst = StrCopyDst( "\"", buff );
            dst = StrCopyDst( CurrEXEName, dst );
            dst = StrCopyDst( "\"", dst );
        } else {
            dst = StrCopyDst( CurrEXEName, buff );
        }
        *dst++ = ' ';
        src = parm;
        while( *src++ != '\0' )
            {}
        /*
         * parm layout
         * <--parameters-->0<--program_name-->0<--arguments-->0
         */
        MergeArgvArray( src, dst, GetTotalSizeIn() - sizeof( *acc ) - ( src - parm ) );

        cr_flags = DEBUG_ONLY_THIS_PROCESS;

        if( !GetEXEHeader( handle, &hi, &stack ) ) {
            goto error_exit;
        }
        if( hi.signature == EXESIGN_PE ) {
            DebugeeSubsystem = PE( hi.u.pehdr, subsystem );
#if MADARCH & MADARCH_X64
            if( !IS_PE64( hi.u.pehdr ) ) {
                IsWOW64 = true;
            }
#endif
            if( DebugeeSubsystem == SS_WINDOWS_CHAR ) {
                cr_flags |= CREATE_NEW_CONSOLE;
            }
#if MADARCH & MADARCH_X64
#elif defined( WOW )
        } else if( hi.signature == EXESIGN_NE ) {
            IsWOW = true;
            /*
             * find out the pid of WOW, if it is already running.
             */
            pVDMEnumProcessWOW( EnumWOWProcessFunc, (LPARAM)&pid );
            if( pid != 0 ) {
                WORD    version;

                version = LOWORD( GetVersion() );
                if( LOBYTE( version ) == 3 && HIBYTE( version ) < 50 ) {
                    int kill = MessageBox( NULL, TRP_NT_wow_warning, TRP_The_WATCOM_Debugger, MB_APPLMODAL + MB_YESNO );
                    if( kill == IDYES ) {
                        DWORD axs = PROCESS_TERMINATE+STANDARD_RIGHTS_REQUIRED;
                        HANDLE hprocess = OpenProcess( axs, FALSE, pid );

                        if( hprocess != 0 && TerminateProcess( hprocess, 0 ) ) {
                            CloseHandle( hprocess );
                            pid = 0;
                        }
                    }
                } else {
                    cr_flags |= CREATE_SEPARATE_WOW_VDM;
                    pid = 0; // always start a new VDM.
                }
            }
            if( pid != 0 ) {
                goto error_exit;
            }
        } else {
            IsDOS = true;
#endif
        }
        CloseHandle( handle );
        handle = INVALID_HANDLE_VALUE;
    }
    /*
     * start the debugee
     */
    pid_started = pid;
    if( *dll_name ) {
        strcat( buff, LOAD_PROG_STR_DELIM );
        strcat( buff, LOAD_PROG_STR_DLLNAME );
        strcat( buff, dll_name );
    }
    if( *service_name ) {
        strcat( buff, LOAD_PROG_STR_DELIM );
        strcat( buff, LOAD_PROG_STR_SERVICE );
        strcat( buff, service_name );
    }
    if( *dll_destination ) {
        strcat( buff, LOAD_PROG_STR_DELIM );
        strcat( buff, LOAD_PROG_STR_COPYDIR );
        strcat( buff, dll_destination );
    }
    if( *service_parm ) {
        strcat( buff, LOAD_PROG_STR_DELIM );
        strcat( buff, LOAD_PROG_STR_SERVICEPARM );
        strcat( buff, service_parm );
    }
    ret->err = StartControlThread( buff, &pid_started, cr_flags );
    if( ret->err != 0 ) {
        goto error_exit;
    }
    /*
     * CREATE_PROCESS_DEBUG_EVENT will always be the first debug event.
     * If it is not, then something is horribly wrong.
     */
    if( !MyWaitForDebugEvent() )
        goto error_exit;
    if( ( DebugEvent.dwDebugEventCode != CREATE_PROCESS_DEBUG_EVENT ) || ( DebugEvent.dwProcessId != pid_started ) ) {
        goto error_exit;
    }
    ProcessInfo.pid = DebugEvent.dwProcessId;
    ProcessInfo.process_handle = DebugEvent.u.CreateProcessInfo.hProcess;
    ProcessInfo.base_addr = DebugEvent.u.CreateProcessInfo.lpBaseOfImage;
#ifdef WOW
#if MADARCH & MADARCH_X86
    if( IsWOW || IsDOS ) {
        AddProcess16( &hi );
    } else {
#endif
#endif
        AddProcess( &hi );
#ifdef WOW
#if MADARCH & MADARCH_X86
    }
#endif
#endif
    AddThread( DebugEvent.dwThreadId, DebugEvent.u.CreateProcessInfo.hThread, (FARPROC)DebugEvent.u.CreateProcessInfo.lpStartAddress );
    DebugeePid = DebugEvent.dwProcessId;
    DebugeeTid = DebugEvent.dwThreadId;
    LastDebugEventTid = DebugEvent.dwThreadId;

#ifdef WOW
#if MADARCH & MADARCH_X86
    if( IsWOW ) {
        bool    vdm_start;

        ret->flags = LD_FLAG_IS_PROT;
        ret->err = 0;
        ret->task_id = DebugeePid;
        /*
         * we use our own CS and DS as the Flat CS and DS, for lack
         * of anything better
         */
        FlatDS = GetDS();
        FlatCS = GetCS();
        DebugExecute( STATE_WAIT_FOR_VDM_START, &vdm_start, false );
        if( !vdm_start ) {
            goto error_exit;
        }
        if( pid ) {
            addAllWOWModules();
        } else {
            addKERNEL();
        }
        /*
         * we save the starting CS:IP of the WOW app, since we will use
         * it to force execution of code later
         */
        ti = FindThread( DebugeeTid );
        MyGetThreadContext( ti, &con );
        WOWAppInfo.addr.segment = (WORD)con.SegCs;
        WOWAppInfo.addr.offset = (WORD)con.Eip;
        con.SegSs = con.SegDs; // Wow lies about the stack segment.  Reset it
        con.Esp = stack;
        MySetThreadContext( ti, &con );
    } else if( IsDOS ) {
        bool    vdm_start;
        /*
         * TODO! Clean up this code
         */
        ret->flags = 0; //LD_FLAG_IS_PROT;
        ret->err = 0;
        ret->task_id = DebugeePid;
        /*
         * we use our own CS and DS as the Flat CS and DS, for lack
         * of anything better
         */
        FlatDS = GetDS();
        FlatCS = GetCS();
        DebugExecute( STATE_WAIT_FOR_VDM_START, &vdm_start, false );
        if( !vdm_start ) {
            goto error_exit;
        }
#if 0
        if( pid ) {
            addAllWOWModules();
        } else {
            addKERNEL();
        }
#endif
        /*
         * we save the starting CS:IP of the WOW app, since we will use
         * it to force execution of code later
         */
        ti = FindThread( DebugeeTid );
        MyGetThreadContext( ti, &con );
        WOWAppInfo.addr.segment = (WORD)con.SegCs;
        WOWAppInfo.addr.offset = (WORD)con.Eip;
        con.SegSs = con.SegDs; // Wow lies about the stack segment.  Reset it
        con.Esp = stack;
        MySetThreadContext( ti, &con );
    } else {
#endif
#endif
        FARPROC base;

        if( pid == 0 ) {
            base = (FARPROC)DebugEvent.u.CreateProcessInfo.lpStartAddress;
        } else {
            base = 0;
        }

        ret->flags = LD_FLAG_IS_PROT;
        ret->err = 0;
        ret->task_id = DebugeePid;
        if( executeUntilStart( pid != 0 ) ) {
            FARPROC old;
            /*
             * make the application load our DLL, so that we can have it
             * run code out of it.  One small note: this will not work right
             * if the app does not load our DLL at the same address the
             * debugger loaded it at!!!
             */
            ti = FindThread( DebugeeTid );
            MyGetThreadContext( ti, &con );
            old = GetIP( &con );
            if( base != 0 ) {
                SetIP( &con, base );
            }
            MySetThreadContext( ti, &con );
            SetIP( &con, old );
            MySetThreadContext( ti, &con );
        }
        ti = FindThread( DebugeeTid );
        MyGetThreadContext( ti, &con );
#if MADARCH & MADARCH_X86
        FlatCS = con.SegCs;
        FlatDS = con.SegDs;
#endif
        ret->flags |= LD_FLAG_IS_BIG;
#ifdef WOW
#if MADARCH & MADARCH_X86
    }
#endif
#endif
    ret->flags |= LD_FLAG_HAVE_RUNTIME_DLLS;
    if( pid != 0 ) {
        ret->flags |= LD_FLAG_IS_STARTED;
    }
    ret->mod_handle = 0;
    if( buff != NULL ) {
        LocalFree( buff );
    }
    return( sizeof( *ret ) );

error_exit:
    if( ret->err == 0 )
        ret->err = GetLastError();
    if( handle != INVALID_HANDLE_VALUE )
        CloseHandle( handle );
    if( buff != NULL ) {
        LocalFree( buff );
    }
    return( sizeof( *ret ) );

}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret   *ret;

    DelProcess( true );
    StopControlThread();
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}
