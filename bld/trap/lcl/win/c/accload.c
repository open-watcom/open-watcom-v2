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
* Description:  Debuggee application loading.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "cpuglob.h"
#include "wdebug.h"
#include "stdwin.h"
#include "getcsip.h"
#include "getsaddr.h"
#include "winerr.h"
#include "int32std.h"
#include "dosextx.h"
#include "dosfile.h"

#define SIG_OFF         0
#define SIG_SIZE        4

#define TINY_ERROR(x)  ((signed long)x < 0)

const unsigned short __based(__segname("_CONST")) win386sig[] = { 0xDEAD,0xBEEF };
const unsigned short __based(__segname("_CONST")) win386sig2[] = { 0xBEEF,0xDEAD };

BOOL WasStarted;

typedef struct {
    WORD        mustbe2;
    WORD        cmdshow;
} word_struct;

typedef struct
{
    WORD        wEnvSeg;
    LPSTR       lpCmdLine;
    word_struct *cmdshow;
    DWORD       reserved;
} lm_parms;

/*
 * AccLoadProg
 *
 * To load a app, we do the following:
 *
 *  Case 1: debugging an existing task
 *      - Find its current CS:IP.
 *      - Plant a breakpoint at the current CS:IP
 *
 *  Case 2: starting a task from scratch
 *      - Look up the start address from the .EXE
 *      - WinExec the app
 *      - Wait for the STARTASK notification for the app
 *      - Plant a breakpoint at its start address
 *
 *  - Wait for the app to hit the breakpoint
 *  - Check if the app is a 32-bit app (look for "DEADBEEF" in code seg).
 *     If it is a 32-bit app:
 *      - Flip the "DEADBEEF" to "BEEFDEAD".  If the extender see's the
 *        "BEEFDEAD", it executes a breakpoint right before it jumps to
 *        the 32-bit code
 *      - Let the app run until a breakpoint is hit
 *      - Trace one instruction. This leaves you at the first instruction
 *        of the 32-bit code
 */
trap_retval ReqProg_load( void )
{
    char                exe_name[_MAX_PATH];
    char                drive[_MAX_DRIVE],directory[_MAX_DIR];
    char                buff[256];
    lm_parms            loadp;
    word_struct         cmdshow;
    char                *parm;
    char                *src;
    char                *dst;
    char                ch;
    unsigned            a,b;
    private_msg         pmsg;
    char                sig[sizeof(DWORD)];
    HTASK               tid;
    DWORD               csip;
    prog_load_req       *acc;
    prog_load_ret       *ret;
    char                *end;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->flags = LD_FLAG_IS_PROT | LD_FLAG_HAVE_RUNTIME_DLLS;
    parm = GetInPtr( sizeof( *acc ) );

    /*
     * reset flags
     */
    OutPos = 0;
    WasStarted = FALSE;
    LoadingDebugee = TRUE;
    Debugging32BitApp = FALSE;
    AddAllCurrentModules();

    /*
     * check for task id
     */
    tid = 0;
    src = parm;
    if( *src == '#' ) {
        src++;
        tid = (HTASK)strtol( src, NULL, 16 );
    } else {
        while( *src != 0 ) {
            if( !isdigit( *src ) ) {
                break;
            }
            src++;
        }
        if( *src == 0 && src != parm ) {
            tid = (HTASK)atoi( parm );
        }
    }
    if( tid != 0 ) {
        csip = GetRealCSIP( tid, &DebugeeModule );
        if( csip == 0 ) {
            tid = 0;
        } else {
            DebugeeTask = tid;
            StopNewTask.loc.segment = FP_SEG( (LPVOID) csip );
            StopNewTask.loc.offset = FP_OFF( (LPVOID) csip );
            ReadMem( StopNewTask.loc.segment, StopNewTask.loc.offset,
                    &StopNewTask.value, 1 );
            ch = '\xcc';
            WriteMem( StopNewTask.loc.segment, StopNewTask.loc.offset, &ch, 1 );
        }
    } else {
        tid = 0;
    }

    /*
     * get the file to execute
     */
    if( tid == 0 ) {
        if( TINY_ERROR( FindFilePath( parm, exe_name, DosExtList ) ) ) {
            exe_name[0] = 0;
        } else {
            _splitpath( exe_name, drive, directory, NULL, NULL );
            a = tolower( drive[0] ) - 'a' + 1;
            _dos_setdrive( a, &b );
            directory[ strlen( directory ) - 1 ] = 0;
            chdir( directory );
        }

        /*
         * get the parm list
         */

        src = parm;
        while( *src != 0 )
            ++src;
        ++src;
        end = GetInPtr( GetTotalSize() - 1 );
        dst = &buff[1];
        for( ;; ) {
            if( src > end )
                break;
            ch = *src;
            if( ch == 0 )
                ch = ' ';
            *dst = ch;
            ++dst;
            ++src;
        }
        if( dst > &buff[1] )
            --dst;
        *dst = '\0';
        buff[0] = dst-buff-1;

        /*
         * get starting point in task
         */
        if( !GetStartAddress( exe_name, &StopNewTask.loc ) ) {
            Out((OUT_ERR,"Could not get starting address"));
            ret->err = WINERR_NOSTART;
            LoadingDebugee = FALSE;
            return( sizeof( *ret ) );
        }
        StopNewTask.segment_number = StopNewTask.loc.segment;
        Out((OUT_LOAD,"Loading %s, cs:ip = %04x:%04lx", exe_name, StopNewTask.loc.segment,
                            StopNewTask.loc.offset ));

        /*
         * load the task
         */
        loadp.cmdshow = &cmdshow;
        loadp.wEnvSeg = 0;
        loadp.lpCmdLine = (LPSTR) buff;
        loadp.cmdshow->mustbe2 = 2;
        loadp.cmdshow->cmdshow = SW_NORMAL;
        loadp.reserved = 0L;
        DebuggerState = LOADING_DEBUGEE;
        DebugeeInstance = LoadModule( exe_name, (LPVOID) &loadp );
        if( (UINT)DebugeeInstance < 32 ) {
            Out((OUT_ERR,"Debugee did not load %d", DebugeeInstance));
            ret->err = WINERR_NOLOAD;
            LoadingDebugee = FALSE;
            return( sizeof( *ret ) );
        }
        DebuggerWaitForMessage( WAITING_FOR_TASK_LOAD, NULL, RESTART_APP );
    }
    AddDebugeeModule();
    pmsg = DebuggerWaitForMessage( WAITING_FOR_BREAKPOINT, DebugeeTask, RESTART_APP );
    if( pmsg == START_BP_HIT ) {

        ret->err = 0;
        ret->task_id = (unsigned_32)DebugeeTask;

        /*
         * look for 32-bit windows application
         */
        ReadMem( IntResult.CS, SIG_OFF, sig, sizeof( DWORD ) );
        if( !StopOnExtender && (!memcmp( sig, win386sig, 4 ) ||
                !memcmp( sig, win386sig2, 4 )) ) {
            Out((OUT_LOAD,"Is Win32App" ));
            Debugging32BitApp = TRUE;
            /*
             * make sure that WDEBUG.386 is installed
             */
            if( !WDebug386 ) {
                ret->err = WINERR_NODEBUG32; /* Can't debug 32 bit app */
                LoadingDebugee = FALSE;
                return( sizeof( *ret ) );
            }
            ret->flags |= LD_FLAG_IS_BIG;
            if( tid == 0 ) {
                WriteMem( IntResult.CS, SIG_OFF, win386sig2, sizeof( DWORD ) );
                pmsg = DebuggerWaitForMessage( GOING_TO_32BIT_START,
                                DebugeeTask, RESTART_APP );
                if( pmsg == FAULT_HIT && IntResult.InterruptNumber == INT_3 ) {
                    IntResult.EIP++;
                    SingleStepMode();
                    pmsg = DebuggerWaitForMessage( GOING_TO_32BIT_START,
                                DebugeeTask, RESTART_APP );
                    if( pmsg != FAULT_HIT || IntResult.InterruptNumber != INT_1 ) {
                        Out((OUT_ERR,"Expected INT_1 not found"));
                        ret->err = WINERR_NOINT1;
                    }
                } else {
                    Out((OUT_ERR,"Expected INT_3 not found"));
                    ret->err = WINERR_NOINT3;
                }
            }
        }
        if( tid != 0 ) {
            ret->flags |= LD_FLAG_IS_STARTED;
            WasStarted = TRUE;
        }
    } else {
        Out((OUT_ERR,"Starting breakpoint not found, pmsg=%d", pmsg ));
        ret->err = WINERR_STARTNOTFOUND;
    }
#if 0
    if( DebugeeTask != NULL ) {
        InitASynchHook();
    }
#endif
    LoadingDebugee = FALSE;
    CurrentModule = 1;
    ret->mod_handle = 0;
    return( sizeof( *ret ) );
}

/*
 * ReqProg_kill
 *
 * If it was a task that we attached to (WasStarted), all we do is
 * forgive the last interrupt, and then exit
 *
 * If it was a task we started, we TerminateApp it, and then wait for
 * the task ended notification.  Note:  Task ended isn't quite good enough,
 * since the module isn't unloaded.  However, you may never get the module
 * unloaded notification, if you are debugging the 2nd, 3rd etc instance
 * of an app, since the module is loaded more than once.  BUT, a NEW command
 * from the debugger ends up restarting the app "too fast" - the module isn't
 * deleted yet, and so it ends up running a second instance, even if you
 * really don't have a first instance.  This is the reason for that half
 * second pause - to allow Windows to get on with the unloading of the module,
 * if it is going to.  Ack.
 */
trap_retval ReqProg_kill( void )
{
    prog_kill_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    Out((OUT_LOAD,"KILL: DebugeeTask=%04x, WasStarted=%d",
        DebugeeTask, WasStarted ));
    if( DebugeeTask != NULL ) {
        IntResult.EFlags &= ~TRACE_BIT;
        if( WasStarted ) {
            Out((OUT_LOAD,"Doing Release Debugee"));
            DebuggerWaitForMessage( RELEASE_DEBUGEE, DebugeeTask, RESTART_APP );
        } else {
            TerminateApp( DebugeeTask, NO_UAE_BOX );
            DebuggerWaitForMessage( KILLING_DEBUGEE, NULL, -1 );
            Out((OUT_LOAD,"Task Terminated(not current)"));
            {
                DWORD   a;
                a = GetTickCount();
                while( GetTickCount() < a + 500 ) {
                    Yield();
                }
            }
        }
#if 0
        FiniASynchHook();
#endif
    }
    ExitSoftMode();
    if( WDebug386 ) {
        if( WasInt32 ) {
            WasInt32 = FALSE;
            DoneWithInterrupt( NULL );
        }
    }
    DebugeeTask = NULL;
    ModuleTop = 0;
    CurrentModule = 1;
    FaultHandlerEntered = FALSE;
    PendingTrap = FALSE;
    SaveStdIn = NIL_HANDLE;
    SaveStdOut = NIL_HANDLE;
    Debugging32BitApp = FALSE;
    return( sizeof( *ret ) );
}
