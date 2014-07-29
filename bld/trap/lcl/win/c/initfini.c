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
#include <dos.h>
#include "cpuglob.h"
#include "wdebug.h"
#include "stdwin.h"
#include "trperr.h"
#include "winctrl.h"

extern  WORD FAR PASCAL AllocCSToDSAlias( WORD );

static FARPROC  faultInstance;
static FARPROC  notifyInstance;
static HANDLE   wint32;

/*
 * InitDebugging:
 *
 * - check for WDEBUG.386
 * - register an interrupt handler (for handling 16-bit faults)
 * - register a notify handler (for receiving all system notifications)
 * - if we have WDEBUG.386, then we load WINT32.DLL, get all its entry
 *   points, and then tell it we want to handle 32-bit faults
 * - we then get a data segment alias for our code segment, so that we
 *   can write stuff into our code segment (see FAULT.C)
 *
 */
char *InitDebugging( void )
{

    DebuggerState=ACTIVE;
    StartWDebug386();
    faultInstance = MakeProcInstance( (FARPROC)IntHandler, Instance );
    if( !InterruptRegister( NULL, faultInstance ) ) {
        return( TRP_WIN_Failed_to_get_interrupt_hook );
    }
    notifyInstance = MakeProcInstance( (FARPROC)NotifyHandler, Instance );
    if( !NotifyRegister( NULL, (LPFNNOTIFYCALLBACK)notifyInstance,
                         NF_NORMAL | NF_RIP ) ) {
        return( TRP_WIN_Failed_to_get_notify_hook );
    }
    Out(( OUT_INIT,"ds=%04x, faultInstance=%Fp, notifyInstance=%Fp,Instance=%04x",
        FP_SEG( &faultInstance ),
        faultInstance, notifyInstance, Instance ));
    if( WDebug386 ) {
        wint32 = LoadLibrary( "wint32.dll" );
        if( (UINT)wint32 < 32 ) {
            WDebug386 = FALSE;
        } else {
            DoneWithInterrupt = (LPVOID) GetProcAddress( wint32, "DoneWithInterrupt" );
            GetDebugInterruptData = (LPVOID) GetProcAddress( wint32, "GetDebugInterruptData" );
            ResetDebugInterrupts32 = (LPVOID) GetProcAddress( wint32, "ResetDebugInterrupts32" );
            SetDebugInterrupts32 = (LPVOID) GetProcAddress( wint32, "SetDebugInterrupts32" );
            DebuggerIsExecuting = (LPVOID) GetProcAddress( wint32, "DebuggerIsExecuting" );

            if( !SetDebugInterrupts32() ) {
                WDebug386 = FALSE;
                FreeLibrary( wint32 );
            } else {
                DebuggerIsExecuting( 1 );
                Out((OUT_INIT,"Hooked Interrupts"));
            }
        }
    }
//    SubClassProcInstance = MakeProcInstance( (FARPROC)SubClassProc, Instance );

    InitDebugHook();
    CSAlias = AllocCSToDSAlias( GetCS() );
    return( "" );

} /* InitDebugging */

/*
 * FinishDebugging - undo all the stuff done by InitDebugging
 */
void FinishDebugging( void )
{

    InterruptUnRegister( NULL );
    if( faultInstance != NULL ) {
        FreeProcInstance( faultInstance );
    }
    NotifyUnRegister( NULL );
    if( notifyInstance != NULL ) {
        FreeProcInstance( notifyInstance );
    }
    if( WDebug386 ) {
        ResetDebugInterrupts32();
        DebuggerIsExecuting( -1 );
        Out((OUT_INIT,"Debug interrupts reset"));
        FreeLibrary( wint32 );
    }
    KillWDebug386();
//    FreeProcInstance( SubClassProcInstance );
    FiniDebugHook();
    WasInt32 = FALSE;
    if( CSAlias != NULL ) {
        FreeSelector( CSAlias );
    }
    CSAlias = 0;
    DisableHookEvents();
    HookRtn = NULL;

} /* FinishDebugging */


/*
 * TrapInit - debugger initialization entry point
 */
trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
{
    trap_version        ver;

    remote = remote;

    DebuggerTask = GetCurrentTask();

    ClearScreen();
    Out(( OUT_INIT,"TrapInit entered, debugger task=%04x", DebuggerTask ));

#ifdef DEBUG
    if( *parm == '[' ) {
        unsigned                bit;
        extern unsigned DbgFlags;
        ++parm;
        while( *parm && *parm != ']' ) {
            switch( *parm ) {
            case 'a':
                bit = -1;
                break;
            case 'b':
                bit = OUT_BREAK;
                break;
            case 'e':
                bit = OUT_ERR;
                break;
            case 'h':
                bit = OUT_HOOK;
                break;
            case 'i':
                bit = OUT_INIT;
                break;
            case 'l':
                bit = OUT_LOAD;
                break;
            case 'p':
                bit = OUT_MAP;
                break;
            case 'm':
                bit = OUT_MSG;
                break;
            case 'q':
                bit = OUT_REQ;
                break;
            case 'r':
                bit = OUT_RUN;
                break;
            case 's':
                bit = OUT_SOFT;
                break;
            case 't':
                bit = OUT_TEMP;
                break;
            }
            ++parm;
            if( *parm == '~' ) {
                DbgFlags &= ~bit;
                ++parm;
            } else {
                DbgFlags |= bit;
            }
        }
        if( *parm ) ++parm;
    }
#endif
    if( parm[0] == 'c' && parm[1] == 'g' && parm[2] == 'e' ) {
        DebugDebugeeOnly = TRUE;
    }
    if( parm[0] == '3' && parm[1] == '2' ) {
        StopOnExtender = TRUE;
    }

    err[0] = 0;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;

    return( ver );
}

/*
 * TrapFini - debugger finish entry point
 */
void TRAPENTRY TrapFini( void )
{
    Out(( OUT_INIT,"TrapFini entered" ));
}

/*
 * InfoFunction - inform trap file of gui debugger being used
 */
void TRAPENTRY InfoFunction( HWND hwnd )
{

    DebuggerWindow = hwnd;
    Out(( OUT_INIT,"DebuggerWindow = %04x", DebuggerWindow ));
    if( hwnd == NULL ) {
        UnLockInput();
    }
}

/*
 * GetHwndFunc - inform trap file of gui debugger being used
 */
HWND TRAPENTRY GetHwndFunc( void )
{
    return( DebuggerWindow );
}

/*
 * set input hook routine
 */
void TRAPENTRY InputHook( LPVOID ptr )
{
    HookRtn = ptr;
}

/*
 * HardModeCheck - obsolete
 */
BOOL TRAPENTRY HardModeCheck( void )
{
    return( HardModeRequired );
}

/*
 * SetHardMode - force hard mode
 */
void TRAPENTRY SetHardMode( char force )
{
    ForceHardMode = force;
}

/*
 * UnLockInput - unlock input from the debugger
 */
void TRAPENTRY UnLockInput( void )
{
    if( DebuggerWindow != NULL && InputLocked ) {
        Out((OUT_SOFT,"Unlocking input from debugger"));
        if( !LockInput( NULL, DebuggerWindow, FALSE ) ) {
            Out((OUT_SOFT,"LockInput returned FALSE"));
        } else {
            InputLocked = FALSE;
        }
    }
}
