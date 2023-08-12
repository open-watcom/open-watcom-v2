/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Handles interfaces to Windows NT debugger functions
*               to handle all debug events (single step, breakpoints etc).
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include <i86.h>
#include "stdnt.h"
#include "globals.h"
#include "madregs.h"
#include "trpld.h"


typedef enum {
    T_OFF,
    T_ON_CURR,
    T_ON_NEXT
}   set_t;

opcode_type place_breakpoint_lin( HANDLE process, FARPROC base )
{
    opcode_type old_opcode;
    DWORD       bytes;

    ReadProcessMemory( process, (LPVOID)base, (LPVOID)&old_opcode, sizeof( old_opcode ), &bytes );
    if( bytes == sizeof( old_opcode ) && old_opcode != BreakOpcode ) {
        WriteProcessMemory( process, (LPVOID)base, (LPVOID)&BreakOpcode, sizeof( BreakOpcode ), &bytes );
        return( old_opcode );
    }
    return( 0 );
}

int remove_breakpoint_lin( HANDLE process, FARPROC base, opcode_type old_opcode )
{
    DWORD       bytes;

    WriteProcessMemory( process, (LPVOID)base, (LPVOID)&old_opcode, sizeof( old_opcode ), &bytes );
    return( bytes != sizeof( old_opcode ) );
}

#if MADARCH & (MADARCH_X86 | MADARCH_X64 | MADARCH_PPC)
static void set_tbit( MYCONTEXT *con, bool on )
{
#if MADARCH & (MADARCH_X86 | MADARCH_X64)
    if( on ) {
        con->EFlags |= INTR_TF;
    } else {
        con->EFlags &= ~INTR_TF;
    }
#elif MADARCH & MADARCH_PPC
    if( on ) {
        con->Msr |= INTR_TF;
    } else {
        con->Msr &= ~INTR_TF;
    }
#endif
}
#endif

static void setATBit( thread_info *ti, set_t set )
/*************************************************
 * control if we are tracing
 */
{
    MYCONTEXT con;

    MyGetThreadContext( ti, &con );
#if MADARCH & MADARCH_X86
    set_tbit( &con, set != T_OFF );
    MySetThreadContext( ti, &con );
#elif MADARCH & MADARCH_X64
    set_tbit( &con, set != T_OFF );
    MySetThreadContext( ti, &con );
#elif MADARCH & MADARCH_AXP
    if( set != T_OFF ) {
        ti->brk_addr = GetIP( &con );
        if( set == T_ON_NEXT ) {
            ti->brk_addr += 4;
        }
        ti->old_opcode = place_breakpoint_lin( ProcessInfo.process_handle, ti->brk_addr );
        if( ti->old_opcode == 0 ) {
            ti->brk_addr = 0;
        }
    } else if( ti->brk_addr != 0 ) {
        remove_breakpoint_lin( ProcessInfo.process_handle, ti->brk_addr, ti->old_opcode );
        ti->brk_addr = 0;
    }
#elif MADARCH & MADARCH_PPC
    set_tbit( &con, set != T_OFF );
    MySetThreadContext( ti, &con );
#else
    #error setATBit not configured
#endif
}

static void setTBitInAllThreads( set_t set )
/*******************************************
 * turn the t-bit on or off in all threads.
 */
{
    thread_info *ti;

    for( ti = ProcessInfo.thread_list; ti != NULL; ti = ti->next ) {
        if( ti->alive ) {
            SuspendThread( ti->thread_handle );
        }
    }
    for( ti = ProcessInfo.thread_list; ti != NULL; ti = ti->next ) {
        if( ti->alive ) {
            setATBit( ti, set );
        }
    }
    for( ti = ProcessInfo.thread_list; ti != NULL; ti = ti->next ) {
        if( ti->alive ) {
            ResumeThread( ti->thread_handle );
        }
    }
}

void InterruptProgram( void )
{
    setTBitInAllThreads( T_ON_CURR );
    /*
     * a trick to make app execute long enough to hit a breakpoint
     */
    PostMessage( HWND_TOPMOST, WM_NULL, 0, 0 );
    PendingProgramInterrupt = true;
}

bool Terminate( void )
{
    HANDLE  hp;

    hp = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
    if( hp != NULL ) {
        TerminateProcess( hp, 0 );
        CloseHandle( hp );
        return( true );
    } else {
        return( false );
    }
}

static BOOL WINAPI consoleHandler( DWORD type )
/**********************************************
 * handle console ctrl c
 */
{
    if( type == CTRL_C_EVENT || type == CTRL_BREAK_EVENT ) {
        /*
         * Since NT has no way do an async stop of a process, this is the next
         * best thing.  By turning on the the T-bit in all threads, we force
         * the process to stop the next time any one of its threads stop.
         * If all threads are blocked waiting for an event, then the process
         * will not stop until the event is satisfied.  Most often, the
         * main thread of the process will be blocked awaiting input, and
         * just running the mouse over the window will cause the app to
         * run and then trace-trap
         */
        InterruptProgram();
        return( TRUE );
    } else {
        return( FALSE );
    }
}


static void setTBit( set_t set )
/*******************************
 * control if we are tracing
 */
{
    thread_info *ti;

    ti = FindThread( DebugeeTid );
    setATBit( ti, set );
}

#if MADARCH & (MADARCH_X86 | MADARCH_X64)
static FARPROC  BreakFixed;

static void decIP( MYCONTEXT *con )
{
    con->Eip--;
}

static void remove_bp( HANDLE proc, MYCONTEXT *con, opcode_type old_opcode )
{
    BreakFixed = (FARPROC)con->Eip;
    remove_breakpoint_lin( proc, BreakFixed, old_opcode );
}
#endif

static trap_conditions handleBreakpointEvent( state_type state )
/***************************************************************
 * process an encountered break point
 */
{
    trap_conditions conditions;
#if MADARCH & (MADARCH_X86 | MADARCH_X64)
    thread_info     *ti;
    MYCONTEXT       con;

    /* unused parameters */ (void)state;

    ti = FindThread( DebugeeTid );
    if( ti == NULL ) {
        if( pOpenThread != NULL ) {
            HANDLE  th;

            #define FOREIGN_THREAD_ACCESS   STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE \
                                            | THREAD_GET_CONTEXT \
                                            | THREAD_SET_CONTEXT \
                                            | THREAD_SUSPEND_RESUME
            th = pOpenThread( FOREIGN_THREAD_ACCESS, FALSE, DebugeeTid );
            AddThread( DebugeeTid, th, NULL );
            ti = FindThread( DebugeeTid );
            ti->is_foreign = true;
        }
    }
    conditions = COND_NONE;
    if( ti != NULL ) {
        conditions = COND_BREAK;
        MyGetThreadContext( ti, &con );
        decIP( &con );
        if( ti->is_foreign ) {
            opcode_type old_opcode;

            if( FindBreak( &con, &old_opcode ) ) {
                HANDLE  proc;

                proc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
                remove_bp( proc, &con, old_opcode );
                CloseHandle( proc );
                set_tbit( &con, true );
                conditions = COND_NONE;
            }
        }
        MySetThreadContext( ti, &con );
    }
#elif MADARCH & MADARCH_AXP
    thread_info *ti;
    MYCONTEXT   con;

    conditions = COND_BREAK;
    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
    if( ti->brk_addr != 0 && GetIP( &con ) == ti->brk_addr ) {
        conditions = handleSinglestepEvent( state );
    }
#elif MADARCH & MADARCH_PPC
    /* unused parameters */ (void)state;

    /*
     * nothing special to do
     */
    conditions = COND_BREAK;
#else
    #error handleBreakpointEvent not configured
#endif
    return( conditions );
}

static trap_conditions handleSinglestepEvent( state_type state )
/***************************************************************
 * process a trace or watch point
 */
{
    if( PendingProgramInterrupt ) {
        /*
         * this was really an async stop request, so turn the t-bit
         * off in all the threads
         */
        setTBitInAllThreads( T_OFF );
        PendingProgramInterrupt = false;
        return( COND_USER );
    }

#if MADARCH & MADARCH_X86
    if( state & STATE_WATCH_386 ) {
        if( CheckBreakPoints() ) {
            return( COND_WATCH );
        }
    }
#endif
    if( state & STATE_WATCH ) {
        if( CheckWatchPoints() ) {
            return( COND_WATCH );
        }
    } else {
#if MADARCH & (MADARCH_X86 | MADARCH_X64)
        if( BreakFixed != 0 ) {
            thread_info *ti;

            ti = FindThread( DebugeeTid );
            if( ti == NULL || ti->is_foreign ) {
                HANDLE  proc;

                proc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
                place_breakpoint_lin( proc, BreakFixed );
                CloseHandle( proc );
                BreakFixed = 0;
                return( COND_NONE );
            }
        }
#endif
        return( COND_TRACE );
    }
    return( COND_NONE );
}

#ifdef WOW
static void getImageNote( IMAGE_NOTE *pin )
/******************************************
 * get current image note structure (WOW)
 */
{
    addr48_ptr  addr;

    addr.segment = FlatDS;
    addr.offset = (DWORD)DW3( DebugEvent.u.Exception.ExceptionRecord );
    ReadMemory( &addr, pin, sizeof( *pin ) );
}
#endif

trap_conditions DebugExecute( state_type state, bool *retflag, bool stop_on_module_load )
/****************************************************************************************
 * execute program under debug control
 */
{
    DWORD           continue_how;
    DWORD           code;
    DWORD           len;
    msg_list        **owner;
    msg_list        *new;
    char            *p;
    char            *q;
    bool            rc;
#ifdef WOW
    thread_info     *ti;
    DWORD           subcode;
    IMAGE_NOTE      imgnote;
#endif
    trap_conditions conditions;

    if( retflag != NULL ) {
        *retflag = false;
    }
    /*
     * "Slaying" gets set by AccKillProg.  Because a dead WOW app
     * will set the DebugeeEnded flag, and we still need to kill WOW,
     * we ignore the DebugeeEnded setting if we are doing a kill
     */
    if( !Slaying ) {
        if( DebugeeEnded || DebugeePid == 0 ) {
            conditions = COND_TERMINATE;
            goto done;
        }
    }

    continue_how = DBG_CONTINUE;

    for( ;; ) {
        PendingProgramInterrupt = false;
        if( (state & STATE_WATCH) && (state & STATE_WATCH_386) == 0 ) {
            setTBit( T_OFF ); /* turn off previous T-bit */
#if MADARCH & MADARCH_AXP
            /*
             * We're doing watch points on an Alpha. If we run into a
             * control transfer instruction, return a spurious watchpoint
             * indication. The debugger will simulate the instruction for
             * us. This keeps the trap file from having to figure out
             * if a conditional branch is going to happen or not.
             */
            {
                DWORD       bytes;
                DWORD       addr;
                DWORD       opcode;
                MYCONTEXT   con;

                MyGetThreadContext( FindThread( DebugeeTid ), &con );
                addr = GetIP( &con );
                ReadProcessMemory( ProcessInfo.process_handle, (LPVOID)addr,
                    (LPVOID)&opcode, sizeof( opcode ), &bytes );
                opcode &= 0xfc000000;
                if( opcode == ( 0x1a << 26 ) || opcode >= ( 0x30 << 26 ) ) {
                    conditions = COND_WATCH;
                    goto done;
                }
            }
#endif
            setTBit( T_ON_NEXT );
        }
        MyContinueDebugEvent( continue_how );
        continue_how = DBG_CONTINUE;
        rc = MyWaitForDebugEvent();
        LastDebugEventTid = DebugEvent.dwThreadId;
#if MADARCH & MADARCH_X64
#else
        if( IsWin32s && !rc ) {
            conditions = COND_LIBRARIES;
            goto done;
        }
#endif
        switch( DebugEvent.dwDebugEventCode ) {
        case EXCEPTION_DEBUG_EVENT:
            code = DebugEvent.u.Exception.ExceptionRecord.ExceptionCode;
            switch( code ) {
#ifdef WOW
            case STATUS_VDM_EVENT:
                subcode = W1( DebugEvent.u.Exception.ExceptionRecord );
                switch( subcode ) {
                case DBG_TASKSTOP:
                    DebugeeTid = DebugEvent.dwThreadId;
                    getImageNote( &imgnote );
                    RemoveModuleFromLibList( imgnote.Module, imgnote.FileName );
                    if( !stricmp( imgnote.FileName, CurrEXEName ) ) {
                        DebugeeEnded = true;
                        conditions = COND_TERMINATE;
                        goto done;
                    }
                    break;
                case DBG_DLLSTOP:
                    getImageNote( &imgnote );
                    RemoveModuleFromLibList( imgnote.Module, imgnote.FileName );
                    break;
                case DBG_DLLSTART:
                    getImageNote( &imgnote );
                    AddLib16( &imgnote );
                    if( !IsWOW && stop_on_module_load ) {
                        conditions = COND_NONE;
                        goto done;
                    }
                    break;
                case DBG_TASKSTART:
                    DebugeeTid = DebugEvent.dwThreadId;
                    ti = FindThread( DebugeeTid );
                    ti->is_wow = true;
                    getImageNote( &imgnote );
                    /*
                     * check and see if we have the 16-bit app that we
                     * started is the one that has finally started
                     */
                    if( stricmp( imgnote.FileName, CurrEXEName ) == 0 && (state & STATE_WAIT_FOR_VDM_START) ) {
                        WOWAppInfo.tid = DebugeeTid;
                        WOWAppInfo.htask = imgnote.hTask;
                        WOWAppInfo.hmodule = imgnote.hModule;
                        strcpy( WOWAppInfo.modname, imgnote.Module );
                        if( retflag != NULL ) {
                            *retflag = true;
                        }
                        conditions = COND_NONE;
                        goto done;
                    } else {
                        AddLib16( &imgnote );
                    }
                    break;
                case DBG_SINGLESTEP:
                    DebugeeTid = DebugEvent.dwThreadId;
                    conditions = handleSinglestepEvent( state );
                    goto done;
                case DBG_BREAK:
                    DebugeeTid = DebugEvent.dwThreadId;
                    conditions = handleBreakpointEvent( state );
                    goto done;
                case DBG_GPFAULT:
                    DebugeeTid = DebugEvent.dwThreadId;
                    LastExceptionCode = STATUS_ACCESS_VIOLATION;
                    conditions = COND_EXCEPTION;
                    goto done;
                case DBG_ATTACH:
                    /*
                     * Sent to let debugger know 16-bit environment is set up.
                     * Only a notification, provides no further data. Must be
                     * handled so that debugger doesn't get confused.
                     */
                    DebugeeTid = DebugEvent.dwThreadId;
                    ti = FindThread( DebugeeTid );
                    ti->is_dos = true;
                    break;
                case DBG_INIT:
                    /*
                     * I have no idea how to handle this!
                     */
                    break;
                default:
                    DebugeeTid = DebugEvent.dwThreadId;
                    LastExceptionCode = STATUS_ACCESS_VIOLATION;
                    conditions = COND_EXCEPTION;
                    goto done;
                }
                break;
#endif  /* WOW */
            case DBG_CONTROL_C:
                /*
                 * this never seems to happen ever never ever never
                 */
                DebugeeTid = DebugEvent.dwThreadId;
                continue_how = DBG_EXCEPTION_NOT_HANDLED;
                break;
            case STATUS_SINGLE_STEP:
                DebugeeTid = DebugEvent.dwThreadId;
                conditions = handleSinglestepEvent( state );
                if( conditions != COND_NONE )
                    goto done;
                break;
            case STATUS_BREAKPOINT:
                DebugeeTid = DebugEvent.dwThreadId;
                if( state & STATE_WAIT_FOR_VDM_START )
                    break;
                conditions = handleBreakpointEvent( state );
                if( conditions != COND_NONE ) {
                    goto done;
                }
                break;
            default:
                /*
                 * we stop on the second notification of the exception to
                 * give the user's exception handlers a chance to run
                 */
                DebugeeTid = DebugEvent.dwThreadId;
                if( DebugEvent.u.Exception.dwFirstChance == 0 || (state & STATE_EXPECTING_FAULT) != 0 ) {
                    LastExceptionCode = code;
                    conditions = COND_EXCEPTION;
                    goto done;
                }
                {
                    char    buff[20];
                    void    *a;

                    new = LocalAlloc( LMEM_FIXED, 80 );
                    new->next = NULL;
                    strcpy( new->msg, "First chance exception: 0x" );
                    ultoa( code, buff, 16 );
                    strcat( new->msg, buff );
                    strcat( new->msg, " at 0x" );
                    a = DebugEvent.u.Exception.ExceptionRecord.ExceptionAddress;
#if 0
#if MADARCH & MADARCH_X64
                    ultoa( (unsigned long)((pointer_uint)a >> 32), buff, 16 );
                    strcat( new->msg, buff );
                    strcat( new->msg, ":0x" );
#endif
#endif
                    ultoa( (unsigned long)(pointer_uint)a, buff, 16 );
                    strcat( new->msg, buff );
                    for( owner = &DebugString; *owner != NULL; owner = &(*owner)->next )
                        {}
                    *owner = new;
                    continue_how = DBG_EXCEPTION_NOT_HANDLED;
                    /*
                     *  Carl Young 8-Jun-2004
                     *  Ensure we clear the trap flag so we don't single step
                     *  the exception handler...
                     */
                    setTBit( T_OFF );
                }
                break;
            }
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            DebugeeTid = DebugEvent.dwThreadId;
            AddThread( DebugEvent.dwThreadId, DebugEvent.u.CreateThread.hThread, (FARPROC)DebugEvent.u.CreateThread.lpStartAddress );
            if( retflag == NULL || (state & STATE_WAIT_FOR_VDM_START) )
                break;
            *retflag = true;
            break;
        case EXIT_THREAD_DEBUG_EVENT:
            DebugeeTid = DebugEvent.dwThreadId;
            ClearDebugRegs();
            DeadThread( DebugEvent.dwThreadId );
            if( retflag == NULL || (state & STATE_WAIT_FOR_VDM_START) )
                break;
            *retflag = true;
            break;
        case CREATE_PROCESS_DEBUG_EVENT:        /* shouldn't ever get */
            DebugeeTid = DebugEvent.dwThreadId;
            break;
        case EXIT_PROCESS_DEBUG_EVENT:
            DebugeeTid = DebugEvent.dwThreadId;
            ClearDebugRegs();
            DebugeeEnded = true;
            DelProcess( false );
            MyContinueDebugEvent( DBG_CONTINUE );
            conditions = COND_TERMINATE;
            goto done;
        case LOAD_DLL_DEBUG_EVENT:
            AddLib();
#if MADARCH & MADARCH_X64
            if( !IsWOW64 && stop_on_module_load ) {
#elif defined( WOW )
            if( !IsWOW && stop_on_module_load ) {
#else
            if( stop_on_module_load ) {
#endif
                conditions = COND_LIBRARIES;
                goto done;
            }
            break;
        case UNLOAD_DLL_DEBUG_EVENT:
            DelLib();
#if MADARCH & MADARCH_X64
            if( !IsWOW64 && stop_on_module_load ) {
#elif defined( WOW )
            if( !IsWOW && stop_on_module_load ) {
#else
            if( stop_on_module_load ) {
#endif
                conditions = COND_LIBRARIES;
                goto done;
            }
            break;
        case OUTPUT_DEBUG_STRING_EVENT:
            if( state & (STATE_IGNORE_DEBUG_OUT | STATE_WAIT_FOR_VDM_START) ) {
                break;
            }
            len = DebugEvent.u.DebugString.nDebugStringLength;
            p = LocalAlloc( LMEM_FIXED, len + 1 );
            {
                addr48_ptr  addr;

                addr.segment = FlatDS;
                addr.offset = (ULONG_PTR)DebugEvent.u.DebugString.lpDebugStringData;
                ReadMemory( &addr, p, len );
            }
            p[len] = '\0';
            #define GOOFY_NT_MESSAGE "LDR: LdrpMapDll Relocating:"
            if( strncmp( p, GOOFY_NT_MESSAGE, sizeof( GOOFY_NT_MESSAGE ) - 1 ) == 0 ) {
                LocalFree( p );
                break;
            }
            q = p;
            for( ;; ) {
                if( q[0] == '\0' || q[0] == '\r' ) {
                    new = LocalAlloc( LMEM_FIXED, sizeof( *new ) + q - p + 1 );
                    new->next = NULL;
                    memcpy( new->msg, p, q - p );
                    new->msg[q - p] = '\0';
                    for( owner = &DebugString; *owner != NULL; owner = &(*owner)->next )
                        {}
                    *owner = new;
                    if( q[0] == '\0' ) {
                        break;
                    }
                    ++q;
                    if( q[0] == '\n' ) {
                        ++q;
                    }
                    p = q;
                } else {
                    ++q;
                }
            }
            LocalFree( p );
            conditions = COND_NONE;
            goto done;
        default:
            break;
        }
    }
done:
    if( DebugString != NULL ) {
        conditions = conditions | COND_MESSAGE | ( BreakOnKernelMessage ? COND_STOP : COND_NONE );
    }
    return( conditions );
}

static trap_elen runProg( bool single_step )
/*******************************************
 * run threads
 */
{
    state_type  state;
    MYCONTEXT   con;
    bool        thread_state_changed;
    thread_info *ti;
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );

    if( DebugeeEnded || DebugeePid == 0 ) {
        ret->conditions = COND_TERMINATE;
        return( sizeof( *ret ) );
    }

    state = STATE_NONE;

    if( single_step ) {
        /*
         * This works on an Alpha (and other machines without a t-bit
         * because the MAD will simulate all control transfer instructions.
         * We only need to deal with straight line code.
         */
        setTBit( T_ON_NEXT );
    } else {
        setTBit( T_OFF );
        if( IsWatch() ) {
            state |= STATE_WATCH;
#if MADARCH & MADARCH_X86
            if( SetDebugRegs() ) {
                state |= STATE_WATCH_386;
            }
#endif
        }
    }

    SetConsoleCtrlHandler( consoleHandler, TRUE );
    ret->conditions = DebugExecute( state, &thread_state_changed, true );
    SetConsoleCtrlHandler( consoleHandler, FALSE );

    if( state & STATE_WATCH_386 ) {
        ClearDebugRegs();
    } else if( state & STATE_WATCH ) {
        setTBit( T_OFF );
    }
    if( single_step ) {
        setTBit( T_OFF );
    }

    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
#if MADARCH & MADARCH_X86
    ret->program_counter.offset = con.Eip;
    ret->stack_pointer.offset = con.Esp;
    ret->program_counter.segment = con.SegCs;
    ret->stack_pointer.segment = con.SegSs;
#elif MADARCH & MADARCH_X64
    ret->program_counter.offset = con.Eip;
    ret->stack_pointer.offset = con.Esp;
    ret->program_counter.segment = con.SegCs;
    ret->stack_pointer.segment = con.SegSs;
#elif MADARCH & MADARCH_AXP
    ret->program_counter.offset = ( (unsigned_64 *)&con.Fir )->u._32[0];
    ret->stack_pointer.offset = ( (unsigned_64 *)&con.IntSp )->u._32[0];
    ret->program_counter.segment = 0;
    ret->stack_pointer.segment = 0;
#elif MADARCH & MADARCH_PPC
    ret->program_counter.offset = con.Iar;
    ret->stack_pointer.offset = con.Gpr2;
    ret->program_counter.segment = 0;
    ret->stack_pointer.segment = 0;
#else
    #error runProg not configured
#endif
    if( ModuleTop > CurrentModule ) {
        ret->conditions |= COND_LIBRARIES;
    }
    ret->conditions |= COND_CONFIG | COND_THREAD;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
{
    return( runProg( false ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( runProg( true ) );
}
