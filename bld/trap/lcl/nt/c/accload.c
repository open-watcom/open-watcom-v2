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
#include <stdlib.h>
#include <ctype.h>
#include "stdnt.h"
#include "trperr.h"
#include "srvcdbg.h"
#include "doserr.h"

#include "clibext.h"


#ifndef CREATE_SEPARATE_WOW_VDM
#define CREATE_SEPARATE_WOW_VDM     0x00000800  // new for NT 3.5 (daytona)
#endif

/*
 * executeUntilStart - run program until start address hit
 */
static BOOL executeUntilStart( BOOL was_running )
{
    HANDLE      ph;
    opcode_type saved_opcode;
    opcode_type brk_opcode = BRKPOINT;
    LPVOID      base;
    SIZE_T      bytes;
    MYCONTEXT   con;
    thread_info *ti;

    ph = DebugEvent.u.CreateProcessInfo.hProcess;
    if( !was_running ) {
        /*
         * if we are not debugging an already running app, then we
         * plant a breakpoint at the first instruction of our new app
         */
        base = (LPVOID)DebugEvent.u.CreateProcessInfo.lpStartAddress;
        ReadProcessMemory( ph, base, (LPVOID)&saved_opcode, sizeof( saved_opcode ), &bytes );
        WriteProcessMemory( ph, base, (LPVOID)&brk_opcode, sizeof( brk_opcode ), &bytes );
    } else {
        // a trick to make app execute long enough to hit a breakpoint
        PostMessage( HWND_TOPMOST, WM_NULL, 0, 0L );
    }

    for( ;; ) {
        /*
         * if we encounter anything but a break point, then we are in
         * trouble!
         */
        if( DebugExecute( STATE_IGNORE_DEBUG_OUT | STATE_IGNORE_DEAD_THREAD, NULL, FALSE ) & COND_BREAK ) {
            ti = FindThread( DebugEvent.dwThreadId );
            MyGetThreadContext( ti, &con );
            if( was_running ) {
                AdjustIP( &con, sizeof( brk_opcode ) );
                MySetThreadContext( ti, &con );
                return( TRUE );
            }
            if( StopForDLLs ) {
                /*
                 * the user has asked us to stop before any DLL's run
                 * their startup code (";dll"), so we do.
                 */
                WriteProcessMemory( ph, base, (LPVOID)&saved_opcode, sizeof( saved_opcode ), &bytes );
                AdjustIP( &con, sizeof( brk_opcode ) );
                MySetThreadContext( ti, &con );
                return( TRUE );
            }
            if( ( AdjustIP( &con, 0 ) == base ) ) {
                /*
                 * we stopped at the applications starting address,
                 * so we can offically declare that the app has loaded
                 */
                WriteProcessMemory( ph, base, (LPVOID)&saved_opcode, sizeof( saved_opcode ), &bytes );
                return( TRUE );
            }
            /*
             * skip this breakpoint and continue
             */
            AdjustIP( &con, sizeof( brk_opcode ) );
            MySetThreadContext( ti, &con );
        } else {
            return( FALSE );
        }
    }

}

#if defined( MD_x86 )
#ifdef WOW
/*
 * addKERNEL - add the KERNEL module to the library load (WOW)
 */
static void addKERNEL( void )
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
        if( !memicmp( me.szModule, "KERNEL", 6 ) ) {
            memcpy( &im.Module, &me.szModule, sizeof( me.szModule ) );
            memcpy( &im.FileName, &me.szExePath, sizeof( me.szExePath ) );
            AddLib( TRUE, &im );
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
    AddLib( TRUE, &im );
#endif

}

/*
 * addAllWOWModules - add all modules as libraries.  This is invoked if
 *                    WOW was already running, since we will get no
 *                    lib load notifications if it was.
 */
static void addAllWOWModules( void )
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
        AddLib( TRUE, &im );
        me.dwSize = sizeof( MODULEENTRY );
    }

}

/*
 * executeUntilVDMStart - go until we hit our first VDM exception
 */
static BOOL executeUntilVDMStart( void )
{
    int rc;

    for( ;; ) {
        rc = DebugExecute( STATE_WAIT_FOR_VDM_START, NULL, FALSE );
        if( rc == COND_VDM_START ) {
            return( TRUE );
        }
        return( FALSE );
    }

}

/*
 * EnumWOWProcessFunc - callback for each WOW process in the system
 */
static BOOL WINAPI EnumWOWProcessFunc( DWORD pid, DWORD attrib, LPARAM lparam )
{
    if( attrib & WOW_SYSTEM ) {
        *( DWORD * ) lparam = pid;
        return( FALSE );
    }
    return( TRUE );

}
#else
static BOOL WINAPI EnumWOWProcessFunc( DWORD pid, DWORD attrib, LPARAM lparam )
{
    (void)pid, (void)attrib; // Unused
    *( DWORD *)lparam = 0;
    return( FALSE );
}
#endif
#endif

/*
 * AccLoadProg - create a new process for debugging
 */
trap_retval ReqProg_load( void )
{
    char            *parm;
    char            *src;
    char            *dst;
    char            *endsrc;
    char            exe_name[PATH_MAX];
    char            ch;
    BOOL            rc;
    int             len;
    MYCONTEXT       con;
    thread_info     *ti;
    HANDLE          handle;
    prog_load_req   *acc;
    prog_load_ret   *ret;
    header_info     hi;
    WORD            stack;
    WORD            version;
    DWORD           pid;
    DWORD           pid_started;
    DWORD           cr_flags;
    char            *buff = NULL;
    size_t          nBuffRequired = 0;
    char            *dll_name;
    char            *service_name;
    char            *dll_destination;
    char            *service_parm;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    parm = GetInPtr( sizeof( *acc ) );

    /*
     * reset status variables
     */
    LastExceptionCode = -1;
    DebugString = NULL;
    DebugeeEnded = FALSE;
    RemoveAllThreads();
    FreeLibList();
    DidWaitForDebugEvent = FALSE;
    DebugeePid = 0;
    DebugeeTid = 0;
    SupportingExactBreakpoints = 0;

    /*
     * check if pid is specified
     */
    ParseServiceStuff( parm, &dll_name, &service_name, &dll_destination, &service_parm );
    pid = 0;
    src = parm;

    /*
    //  Just to be really safe!
    */
    nBuffRequired = GetTotalSize() + PATH_MAX + 16;
    if( NULL == ( buff = malloc( nBuffRequired ) ) ) {
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
        if( *src == 0 && src != parm ) {
            pid = atoi( parm );
        }
    }

    /*
     * get program to debug.  If the user has specified a pid, then
     * skip directly to doing a DebugActiveProcess
     */
    IsWOW = FALSE;
#if !defined( MD_x64 )
    IsDOS = FALSE;
#endif
    if( pid == 0 ) {
        if( FindFilePath( parm, exe_name, ExtensionList ) != 0 ) {
            ret->err = ERROR_FILE_NOT_FOUND;
            goto error_exit;
        }

        /*
         * Get type of application
         */
        handle = CreateFile( (LPTSTR)exe_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
        if( handle == INVALID_HANDLE_VALUE ) {
            ret->err = GetLastError();
            goto error_exit;
        }
        GetFullPathName( exe_name, MAX_PATH, CurrEXEName, NULL );

        /*
         * get the parm list
         */
        if( strchr( CurrEXEName, ' ' ) != NULL ) {
            strcpy( buff, "\"" );
            strcat( buff, CurrEXEName );
            strcat( buff, "\"" );
        } else {
            strcpy( buff, CurrEXEName );
        }
        dst = &buff[strlen( buff )];
        src = parm;
        while( *src != 0 ) {
            ++src;
        }
        // parm layout
        // <--parameters-->0<--program_name-->0<--arguments-->0
        //
        for( len = GetTotalSize() - sizeof( *acc ) - (src - parm) - 1; len > 0; --len ) {
            ch = *src;
            if( ch == 0 ) {
                ch = ' ';
            }
            *dst = ch;
            ++dst;
            ++src;
        }
        *dst = 0;

        cr_flags = DEBUG_ONLY_THIS_PROCESS;

        if( !GetEXEHeader( handle, &hi, &stack ) ) {
            ret->err = GetLastError();
            CloseHandle( handle );
            goto error_exit;
        }
        if( hi.sig == EXE_PE ) {
            if( IS_PE64( hi.u.peh ) ) {
                DebugeeSubsystem = PE64( hi.u.peh ).subsystem;
            } else {
                DebugeeSubsystem = PE32( hi.u.peh ).subsystem;
#if defined( MD_x64 )
                IsWOW = TRUE;
#endif
            }
            if( DebugeeSubsystem == SS_WINDOWS_CHAR ) {
                cr_flags |= CREATE_NEW_CONSOLE;
            }
#if !defined( MD_x64 )
        } else if( hi.sig == EXE_NE ) {
            IsWOW = TRUE;
            /*
             * find out the pid of WOW, if it is already running.
             */
            pVDMEnumProcessWOW( EnumWOWProcessFunc, (LPARAM)&pid );
            if( pid != 0 ) {
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
                ret->err = GetLastError();
                CloseHandle( handle );
                goto error_exit;
            }
        } else {
            IsDOS = TRUE;
#endif
        }
        CloseHandle( handle );
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
    rc = MyWaitForDebugEvent();
    if( !rc || ( DebugEvent.dwDebugEventCode != CREATE_PROCESS_DEBUG_EVENT ) || ( DebugEvent.dwProcessId != pid_started ) ) {
        ret->err = GetLastError();
        goto error_exit;
    }
    ProcessInfo.pid = DebugEvent.dwProcessId;
    ProcessInfo.process_handle = DebugEvent.u.CreateProcessInfo.hProcess;
    ProcessInfo.base_addr = DebugEvent.u.CreateProcessInfo.lpBaseOfImage;
    AddProcess( &hi );
    AddThread( DebugEvent.dwThreadId, DebugEvent.u.CreateProcessInfo.hThread, DebugEvent.u.CreateProcessInfo.lpStartAddress );
    DebugeePid = DebugEvent.dwProcessId;
    DebugeeTid = DebugEvent.dwThreadId;
    LastDebugEventTid = DebugEvent.dwThreadId;

#if defined( MD_x86 )
#ifdef WOW
    if( IsWOW ) {
        ret->flags = LD_FLAG_IS_PROT;
        ret->err = 0;
        ret->task_id = DebugeePid;
        /*
         * we use our own CS and DS as the Flat CS and DS, for lack
         * of anything better
         */
        FlatDS = GetDS();
        FlatCS = GetCS();
        if( !executeUntilVDMStart() ) {
            ret->err = GetLastError();
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
        WOWAppInfo.segment = ( WORD ) con.SegCs;
        WOWAppInfo.offset = ( WORD ) con.Eip;
        con.SegSs = con.SegDs; // Wow lies about the stack segment.  Reset it
        con.Esp = stack;
        MySetThreadContext( ti, &con );
    } else if( IsDOS ) {
        // TODO! Clean up this code
        ret->flags = 0; //LD_FLAG_IS_PROT;
        ret->err = 0;
        ret->task_id = DebugeePid;
        /*
         * we use our own CS and DS as the Flat CS and DS, for lack
         * of anything better
         */
        FlatDS = GetDS();
        FlatCS = GetCS();
        if( !executeUntilVDMStart() ) {
            ret->err = GetLastError();
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
        WOWAppInfo.segment = ( WORD )con.SegCs;
        WOWAppInfo.offset = ( WORD )con.Eip;
        con.SegSs = con.SegDs; // Wow lies about the stack segment.  Reset it
        con.Esp = stack;
        MySetThreadContext( ti, &con );
    } else {
#else
    {
#endif
#else
    {
#endif
        LPVOID base;

        if( pid == 0 ) {
            base = (LPVOID)DebugEvent.u.CreateProcessInfo.lpStartAddress;
        } else {
            base = 0;
        }

        ret->flags = LD_FLAG_IS_PROT;
        ret->err = 0;
        ret->task_id = DebugeePid;
        if( executeUntilStart( pid != 0 ) ) {
            LPVOID old;
            /*
             * make the application load our DLL, so that we can have it
             * run code out of it.  One small note: this will not work right
             * if the app does not load our DLL at the same address the
             * debugger loaded it at!!!
             */

            ti = FindThread( DebugeeTid );
            MyGetThreadContext( ti, &con );
            old = (LPVOID)AdjustIP( &con, 0 );
            if( base != 0 ) {
                SetIP( &con, base );
            }
            MySetThreadContext( ti, &con );
            SetIP( &con, old );
            MySetThreadContext( ti, &con );
        }
        ti = FindThread( DebugeeTid );
        MyGetThreadContext( ti, &con );
#if defined( MD_x86 )
        FlatCS = con.SegCs;
        FlatDS = con.SegDs;
#endif
        ret->flags |= LD_FLAG_IS_BIG;
    }
    ret->flags |= LD_FLAG_HAVE_RUNTIME_DLLS;
    if( pid != 0 ) {
        ret->flags |= LD_FLAG_IS_STARTED;
    }
    ret->mod_handle = 0;

error_exit:
    if( buff ) {
        free( buff );
        buff = NULL;
    }
    return( sizeof( *ret ) );

}

trap_retval ReqProg_kill( void )
{
    prog_kill_ret   *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    DelProcess( TRUE );
    StopControlThread();
    return( sizeof( *ret ) );
}
