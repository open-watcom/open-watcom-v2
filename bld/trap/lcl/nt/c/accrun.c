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
* Description:  Handles interfaces to Windows NT debugger functions
*               to handle all debug events (single step, breakpoints etc).
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "madregs.h"
#include "stdnt.h"

typedef enum {
    T_OFF,
    T_ON_CURR,
    T_ON_NEXT
}   set_t;

/*
 * setATBit - control if we are tracing
 */
static void setATBit( thread_info *ti, set_t set )
{
    MYCONTEXT con;

    con.ContextFlags = MYCONTEXT_CONTROL;
    MyGetThreadContext( ti, &con );
#if defined( MD_x86 ) || defined( MD_x64 )
    if( set != T_OFF ) {
        con.EFlags |= TRACE_BIT;
    } else {
        con.EFlags &= ~TRACE_BIT;
    }
    con.ContextFlags = MYCONTEXT_CONTROL;
    MySetThreadContext( ti, &con );
#elif defined( MD_axp )
    {
        DWORD           bytes;
        brkpnt_type     brk = BRK_POINT;

        if( set != T_OFF ) {
            ti->brk_addr = AdjustIP( &con, 0 );
            if( set == T_ON_NEXT )
                ti->brk_addr += 4;
                ReadProcessMemory( ProcessInfo.process_handle,
                    (LPVOID)ti->brk_addr, (LPVOID)&ti->brk_opcode,
                    sizeof( ti->brk_opcode ), (LPDWORD)&bytes );
            if( ti->brk_opcode != brk ) {
                WriteProcessMemory( ProcessInfo.process_handle,
                    (LPVOID)ti->brk_addr, (LPVOID)&brk, sizeof( brk ),
                    (LPDWORD)&bytes );
            } else {
                ti->brk_addr = 0;
            }
        } else if( ti->brk_addr != 0 ) {
            WriteProcessMemory( ProcessInfo.process_handle,
                (LPVOID)ti->brk_addr, (LPVOID)&ti->brk_opcode,
                sizeof( ti->brk_opcode ), (LPDWORD)&bytes );
            ti->brk_addr = 0;
        }
    }
#elif defined( MD_ppc )
    if( set != T_OFF ) {
        con.Msr |= TRACE_BIT;
    } else {
        con.Msr &= ~TRACE_BIT;
    }
    con.ContextFlags = MYCONTEXT_CONTROL;
    MySetThreadContext( ti, &con );
#else
    #error setATBit not configured
#endif
}

/*
 * setTBitsetTBitInAllThreads - turn the t-bit on or off in all threads.
 */
static void setTBitInAllThreads( set_t set )
{
    thread_info *ti;

    ti = ProcessInfo.thread_list;
    while( ti != NULL ) {
        if( ti->alive ) {
            SuspendThread( ti->thread_handle );
        }
        ti = ti->next;
    }

    ti = ProcessInfo.thread_list;
    while( ti != NULL ) {
        if( ti->alive ) {
            setATBit( ti, set );
        }
        ti = ti->next;
    }

    ti = ProcessInfo.thread_list;
    while( ti != NULL ) {
        if( ti->alive ) {
            ResumeThread( ti->thread_handle );
        }
        ti = ti->next;
    }
}

void InterruptProgram( void )
{
    setTBitInAllThreads( T_ON_CURR );
    // a trick to make app execute long enough to hit a breakpoint
    PostMessage( HWND_TOPMOST, WM_NULL, 0, 0 );
    PendingProgramInterrupt = TRUE;
}

bool Terminate( void )
{
    HANDLE  hp;

    hp = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
    if( hp != NULL ) {
        TerminateProcess( hp, 0 );
        CloseHandle( hp );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

/*
 * consoleHandler - handle console ctrl c
 */
static BOOL WINAPI consoleHandler( DWORD type )
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


/*
 * setTBit - control if we are tracing
 */
static void setTBit( set_t set )
{
    thread_info *ti;

    ti = FindThread( DebugeeTid );
    setATBit( ti, set );
}

/*
 * handleInt3 - process an encountered break point
 */
#if defined( MD_x86 ) || defined( MD_x64 )
static DWORD    BreakFixed;
#endif

static int handleInt3( DWORD state )
{
#if defined( MD_x86 ) || defined( MD_x64 )
    thread_info *ti;
    MYCONTEXT   con;

    state=state; // Unused
    ti = FindThread( DebugeeTid );
    if( ti == NULL ) {
        HANDLE  th;

        if( pOpenThread == NULL ) {
            return( 0 );
        }
        th = pOpenThread( DebugeeTid );
        AddThread( DebugeeTid, th, NULL );
        ti = FindThread( DebugeeTid );
        ti->is_foreign = TRUE;
    }
    if( ti->is_foreign ) {
        HANDLE  proc;
        SIZE_T  written;
        BYTE    ch;

        MyGetThreadContext( ti, &con );
        con.Eip--;
        if( !FindBreak( (WORD)con.SegCs, (DWORD)con.Eip, &ch ) ) {
            MySetThreadContext( ti, &con );
            return( COND_BREAK );
        }
        BreakFixed = con.Eip;
        proc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
        WriteProcessMemory( proc, (LPVOID)con.Eip, &ch, 1, &written );
        con.EFlags |= TRACE_BIT;
        MySetThreadContext( ti, &con );
        CloseHandle( proc );
        return( 0 );
    } else {
        MyGetThreadContext( ti, &con );
        con.Eip--;
        MySetThreadContext( ti, &con );
    }
#elif defined( MD_axp )
    thread_info *ti;
    MYCONTEXT   con;

    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
    if( ti->brk_addr != 0 && AdjustIP( &con, 0 ) == ti->brk_addr ) {
        return( handleInt1( state ) );
    }
#elif defined( MD_ppc )
    /* nothing special to do */
#else
    #error handleInt3 not configured
#endif
    return( COND_BREAK );
}

/*
 * handleInt1 - process a trace or watch point
 */
static int handleInt1( DWORD state )
{
    if( PendingProgramInterrupt ) {
        /*
         * this was really an async stop request, so turn the t-bit
         * off in all the threads
         */
        setTBitInAllThreads( T_OFF );
        PendingProgramInterrupt = FALSE;
        return( COND_USER );
    }

#if defined( MD_x86 )
    if( state & STATE_WATCH_386 ) {
        if( GetDR6() & 0xf ) {
            return( COND_WATCH );
        }
    }
#endif
    if( state & STATE_WATCH ) {
        if( CheckWatchPoints() ) {
            return( COND_WATCH );
        }
    } else {
#if defined( MD_x86 )
        HANDLE      proc;
        DWORD       written;
        brkpnt_type ch;
        thread_info *ti;

        if( BreakFixed == 0 ) {
            return( COND_TRACE );
        }
        ti = FindThread( DebugeeTid );
        if( ti && !ti->is_foreign ) {
            return( COND_TRACE );
        }
        ch = BRK_POINT;
        proc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, DebugeePid );
        WriteProcessMemory( proc, (LPVOID)BreakFixed, &ch, 1, &written );
        CloseHandle( proc );
        BreakFixed = 0;
        return( 0 );
#else
        return( COND_TRACE );
#endif
    }
    return( 0 );
}

#ifdef WOW
#if !defined( MD_x64 )
/*
 * getImageNote - get current image note structure (WOW)
 */
static void getImageNote( IMAGE_NOTE *pin )
{
    ReadMem( FlatDS, (DWORD)DW3( DebugEvent.u.Exception.ExceptionRecord ),
                         pin, sizeof( IMAGE_NOTE ) );
}
#endif
#endif

/*
 * DebugExecute - execute program under debug control
 */
int DebugExecute( DWORD state, int *tsc, bool stop_on_module_load )
{
    DWORD       continue_how;
    DWORD       code;
    DWORD       len;
    msg_list    **owner;
    msg_list    *new;
    int         cond;
    char        *p;
    char        *q;
    bool        rc;
#ifdef WOW
#if !defined( MD_x64 )
    thread_info *ti;
    DWORD       subcode;
    IMAGE_NOTE  imgnote;
#endif
#endif
    int         returnCode;

    if( tsc != NULL ) {
        *tsc = FALSE;
    }
    /*
     * "Slaying" gets set by AccKillProg.  Because a dead WOW app
     * will set the DebugeeEnded flag, and we still need to kill WOW,
     * we ignore the DebugeeEnded setting if we are doing a kill
     */
    if( !Slaying ) {
        if( DebugeeEnded || DebugeePid == 0 ) {
            returnCode = COND_TERMINATE;
            goto done;
        }
    }

    continue_how = DBG_CONTINUE;

    for( ;; ) {
        PendingProgramInterrupt = FALSE;
        if( ( state & STATE_WATCH ) && !( state & STATE_WATCH_386 ) ) {
            setTBit( T_OFF ); /* turn off previous T-bit */
#if defined( MD_axp )
            /*
               We're doing watch points on an Alpha. If we run into a
               control transfer instruction, return a spurious watchpoint
               indication. The debugger will simulate the instruction for
               us. This keeps the trap file from having to figure out
               if a conditional branch is going to happen or not.
            */
            {
                DWORD       bytes;
                DWORD       addr;
                DWORD       opcode;
                MYCONTEXT   con;

                con.ContextFlags = MYCONTEXT_CONTROL;
                MyGetThreadContext( FindThread( DebugeeTid ), &con );
                addr = AdjustIP( &con, 0 );
                ReadProcessMemory( ProcessInfo.process_handle, (LPVOID)addr,
                    (LPVOID)&opcode, sizeof( opcode ), (LPDWORD)&bytes );
                opcode &= 0xfc000000;
                if( opcode == ( 0x1a << 26 ) || opcode >= ( 0x30 << 26 ) ) {
                    returnCode = COND_WATCH;
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
#if !defined( MD_x64 )
        if( IsWin32s && !rc ) {
            returnCode = COND_LIBRARIES;
            goto done;
        }
#endif
        switch( DebugEvent.dwDebugEventCode ) {
        case EXCEPTION_DEBUG_EVENT:
            code = DebugEvent.u.Exception.ExceptionRecord.ExceptionCode;
            switch( code ) {
#ifdef WOW
#if !defined( MD_x64 )
            case STATUS_VDM_EVENT:
                subcode = W1( DebugEvent.u.Exception.ExceptionRecord );
                switch( subcode ) {
                case DBG_TASKSTOP:
                    DebugeeTid = DebugEvent.dwThreadId;
                    getImageNote( &imgnote );
                    RemoveModuleFromLibList( imgnote.Module, imgnote.FileName );
                    if( !stricmp( imgnote.FileName, CurrEXEName ) ) {
                        DebugeeEnded = TRUE;
                        returnCode = COND_TERMINATE;
                        goto done;
                    }
                    break;
                case DBG_DLLSTOP:
                    getImageNote( &imgnote );
                    RemoveModuleFromLibList( imgnote.Module, imgnote.FileName );
                    break;
                case DBG_DLLSTART:
                    getImageNote( &imgnote );
                    AddLib( TRUE, &imgnote );
                    if( !IsWOW && stop_on_module_load ) {
                        returnCode = 0;
                        goto done;
                    }
                    break;
                case DBG_TASKSTART:
                    DebugeeTid = DebugEvent.dwThreadId;
                    ti = FindThread( DebugeeTid );
                    ti->is_wow = TRUE;
                    getImageNote( &imgnote );
                    /*
                     * check and see if we have the 16-bit app that we
                     * started is the one that has finally started
                     */
                    if( !stricmp( imgnote.FileName, CurrEXEName ) &&
                        ( state & STATE_WAIT_FOR_VDM_START ) ) {
                        WOWAppInfo.tid = DebugeeTid;
                        WOWAppInfo.htask = imgnote.hTask;
                        WOWAppInfo.hmodule = imgnote.hModule;
                        strcpy( WOWAppInfo.modname, imgnote.Module );
                        returnCode = COND_VDM_START;
                        goto done;
                    } else {
                        AddLib( TRUE, &imgnote );
                    }
                    break;
                case DBG_SINGLESTEP:
                    DebugeeTid = DebugEvent.dwThreadId;
                    returnCode = handleInt1( state );
                    goto done;
                case DBG_BREAK:
                    DebugeeTid = DebugEvent.dwThreadId;
                    returnCode = handleInt3( state );
                    goto done;
                case DBG_GPFAULT:
                    DebugeeTid = DebugEvent.dwThreadId;
                    LastExceptionCode = STATUS_ACCESS_VIOLATION;
                    returnCode = COND_EXCEPTION;
                    goto done;
                case DBG_ATTACH:
                    /* Sent to let debugger know 16-bit environment is set up.
                     * Only a notification, provides no further data. Must be
                     * handled so that debugger doesn't get confused.
                     */
                    DebugeeTid = DebugEvent.dwThreadId;
                    ti = FindThread( DebugeeTid );
                    ti->is_dos = TRUE;
                    break;
                case DBG_INIT:
                    // I have no idea how to handle this!
                    break;
                default:
                    DebugeeTid = DebugEvent.dwThreadId;
                    LastExceptionCode = STATUS_ACCESS_VIOLATION;
                    returnCode = COND_EXCEPTION;
                    goto done;
                }
                break;
#else
#endif
#endif
            case DBG_CONTROL_C:
                /*
                 * this never seems to happen ever never ever never
                 */
                DebugeeTid = DebugEvent.dwThreadId;
                continue_how = DBG_EXCEPTION_NOT_HANDLED;
                break;
            case STATUS_SINGLE_STEP:
                DebugeeTid = DebugEvent.dwThreadId;
                cond = handleInt1( state );
                if( cond != 0 ) {
                    returnCode = cond;
                    goto done;
                }
                break;
            case STATUS_BREAKPOINT:
                DebugeeTid = DebugEvent.dwThreadId;
                if( state & STATE_WAIT_FOR_VDM_START ) {
                    break;
                }
                cond = handleInt3( state );
                if( cond != 0 ) {
                    returnCode = cond;
                    goto done;
                }
                break;
            default:
                /*
                 * we stop on the second notification of the exception to
                 * give the user's exception handlers a chance to run
                 */
                DebugeeTid = DebugEvent.dwThreadId;
                if( DebugEvent.u.Exception.dwFirstChance &&
                  !( state & STATE_EXPECTING_FAULT ) ) {
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
#if defined( MD_x64 )
                    ultoa( (unsigned long)((pointer_int)a >> 32), buff, 16 );
                    strcat( new->msg, buff );
                    strcat( new->msg, ":0x" );
#endif
#endif
                    ultoa( (unsigned long)(pointer_int)a, buff, 16 );
                    strcat( new->msg, buff );
                    owner = &DebugString;
                    for( ;; ) {
                        if( *owner == NULL )
                            break;
                        owner = &( *owner )->next;
                    }
                    *owner = new;
                    continue_how = DBG_EXCEPTION_NOT_HANDLED;
                    /*
                     *  Carl Young 8-Jun-2004
                     *  Ensure we clear the trap flag so we don't single step
                     *  the exception handler...
                     */
                    setTBit( T_OFF );
                } else {
                    LastExceptionCode = code;
                    returnCode = COND_EXCEPTION;
                    goto done;
                }
                break;
            }
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            DebugeeTid = DebugEvent.dwThreadId;
            if( tsc != NULL ) {
                *tsc = TRUE;
            }
            AddThread( DebugEvent.dwThreadId, DebugEvent.u.CreateThread.hThread, DebugEvent.u.CreateThread.lpStartAddress );
            break;
        case EXIT_THREAD_DEBUG_EVENT:
            DebugeeTid = DebugEvent.dwThreadId;
            ClearDebugRegs();
            if( tsc != NULL ) {
                *tsc = TRUE;
            }
            DeadThread( DebugEvent.dwThreadId );
            break;
        case CREATE_PROCESS_DEBUG_EVENT:        // shouldn't ever get
            DebugeeTid = DebugEvent.dwThreadId;
            break;
        case EXIT_PROCESS_DEBUG_EVENT:
            DebugeeTid = DebugEvent.dwThreadId;
            ClearDebugRegs();
            DebugeeEnded = TRUE;
            DelProcess( FALSE );
            MyContinueDebugEvent( DBG_CONTINUE );
            returnCode = COND_TERMINATE;
            goto done;
        case LOAD_DLL_DEBUG_EVENT:
            AddLib( FALSE, NULL );
            if( !IsWOW && stop_on_module_load ) {
                returnCode = COND_LIBRARIES;
                goto done;
            }
            break;
        case UNLOAD_DLL_DEBUG_EVENT:
            DelLib();
            if( !IsWOW && stop_on_module_load ) {
                returnCode = COND_LIBRARIES;
                goto done;
            }
            break;
        case OUTPUT_DEBUG_STRING_EVENT:
            if( state & ( STATE_IGNORE_DEBUG_OUT | STATE_WAIT_FOR_VDM_START ) )
            {
                break;
            }
            len = DebugEvent.u.DebugString.nDebugStringLength;
            p = LocalAlloc( LMEM_FIXED, len + 1 );
            ReadMem( FlatDS, (ULONG_PTR)DebugEvent.u.DebugString.lpDebugStringData, p, len );
            p[len] = '\0';
            #define GOOFY_NT_MESSAGE "LDR: LdrpMapDll Relocating:"
            if( !strncmp( p, GOOFY_NT_MESSAGE, sizeof( GOOFY_NT_MESSAGE ) - 1 )
                    ) {
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
                    owner = &DebugString;
                    for( ;; ) {
                        if( *owner == NULL ) {
                            break;
                        }
                        owner = &( *owner )->next;
                    }
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
            returnCode = 0;
            goto done;
        default:
            break;
        }
    }
done:
    ;
    if( DebugString != NULL ) {
        returnCode += COND_MESSAGE + ( BreakOnKernelMessage ? COND_STOP : 0 );
    }
    return( returnCode );
}

/*
 * runProg - run threads
 */
static trap_elen runProg( bool single_step )
{
    DWORD       state;
    MYCONTEXT   con;
    BOOL        thread_state_changed;
    thread_info *ti;
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );

    if( DebugeeEnded || DebugeePid == 0 ) {
        ret->conditions = COND_TERMINATE;
        return( sizeof( *ret ) );
    }

    state = 0;

    if( single_step ) {
        /*
           This works on an Alpha (and other machines without a t-bit
           because the MAD will simulate all control transfer instructions.
           We only need to deal with straight line code.
        */
        setTBit( T_ON_NEXT );
    } else {
        setTBit( T_OFF );
        if( WPCount != 0 ) {
            state |= STATE_WATCH;
#if defined( MD_x86 )
            if( SetDebugRegs() ) {
                state |= STATE_WATCH_386;
            }
#endif
        }
    }

    SetConsoleCtrlHandler( consoleHandler, TRUE );
    ret->conditions = DebugExecute( state, &thread_state_changed, TRUE );
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
#if defined( MD_x86 ) || defined( MD_x64 )
    ret->program_counter.offset = con.Eip;
    ret->stack_pointer.offset = con.Esp;
    ret->program_counter.segment = con.SegCs;
    ret->stack_pointer.segment = con.SegSs;
#elif defined( MD_axp )
    ret->program_counter.offset = ( (unsigned_64 *)&con.Fir )->u._32[0];
    ret->stack_pointer.offset = ( (unsigned_64 *)&con.IntSp )->u._32[0];
    ret->program_counter.segment = 0;
    ret->stack_pointer.segment = 0;
#elif defined( MD_ppc )
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

trap_elen ReqProg_go( void )
{
    return( runProg( FALSE ) );
}

trap_elen ReqProg_step( void )
{
    return( runProg( TRUE ) );
}
