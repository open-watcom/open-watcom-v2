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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//#define DEBUG

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#define INCLUDE_TOOL_H
#include "segmcpu.h"
#include "wdebug.h"
#include "stdwin.h"
#include "trperr.h"
#include "winctrl.h"
#include "trpimp.h"
#include "trpld.h"
#include "trpsys.h"
#include "initfini.h"
#include "di386cli.h"
#include "wclbtool.h"
#ifdef DEBUG
    #include "dpmi.h"
#endif


extern WORD FAR PASCAL AllocCSToDSAlias( WORD );

static LPFNINTHCALLBACK     fault_fn;
static LPFNNOTIFYCALLBACK   notify_fn;

#ifdef DEBUG

#define MONO

/*
 * Debugging output code for AT
 */
static unsigned             DbgFlags = OUT_ALL;
static int                  _cnt;
#ifdef MONO
static int                  _line = 0;
#endif

static char *GetScreenPointer( void )
{
#if 0
    static short    sel;
    long            rc;

    if( sel == 0 ) {
        rc = _DPMISegmentToDescriptor( 0xB000 );
        if( rc >= 0 ) {
            sel = rc;
        }
    }
    return( _MK_FP( sel, 0 ) );
#else
    extern char _B000H[];
    return( _MK_FP( _B000H, 0 ) );
#endif
}

void MyClearScreen( void )
{
#ifdef MONO
    int i;

    char *scrn = GetScreenPointer();

    for( i = 0; i < ( 80 * 25 ); i++ ) {
        scrn[i * 2] = ' ';
        scrn[i * 2 + 1] = 7;
    }
#endif
}

void MyOut( unsigned f, char *str, ... )
{
    va_list     args;
    char        res[128];
    int         len,i;
    char        *scr;
    char        *scrn;

    if( (f & DbgFlags) == 0 )
        return;
    sprintf( res, "%03d) ", ++_cnt );
    va_start( args, str );
    vsprintf( &res[5], str, args );
    va_end( args );
#ifdef MONO
    len = strlen( res );

    scrn = GetScreenPointer();

    scr = &scrn[_line * 80 * 2];

    for( i = 0; i < len; i++ ) {
        scr[i * 2] = res[i];
        scr[i * 2 + 1] = 7;
    }
    for( i = len; i < 80; i++ ) {
        scr[i * 2] = ' ';
        scr[i * 2 + 1] = 7;
    }
    _line++;
    if( _line > 24 )
        _line = 0;

    scr = &scrn[_line * 80 * 2];
    for( i = 0; i < 80; i++ ) {
        scr[i * 2] = ' ';
        scr[i * 2 + 1] = 7;
    }
#else
    MessageBox( NULL, res, "FOO", MB_SYSTEMMODAL | MB_OK );
#endif
}

#endif

void SetInputLock( bool lock_status )
{
    if( DebuggerWindow != NULL ) {
        if( InputLocked != lock_status ) {
            Out((OUT_SOFT,( lock_status ) ? "Locking input to debugger" : "Unlocking input from debugger"));
            if( !LockInput( NULL, DebuggerWindow, lock_status ) ) {
                Out((OUT_SOFT,"LockInput returned FALSE"));
            } else {
                InputLocked = lock_status;
            }
        }
    }
}

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
    DebuggerState = ACTIVE;
    if( CheckWin386Debug() == WGOD_VERSION ) {
        WDebug386 = true;
        UseHotKey( 1 );
    }
    fault_fn = MakeProcInstance_INTH( IntHandler, Instance );
    if( !InterruptRegister( NULL, fault_fn ) ) {
        return( TRP_WIN_Failed_to_get_interrupt_hook );
    }
    notify_fn = MakeProcInstance_NOTIFY( NotifyHandler, Instance );
    if( !NotifyRegister( NULL, notify_fn, NF_NORMAL | NF_RIP ) ) {
        return( TRP_WIN_Failed_to_get_notify_hook );
    }
    Out(( OUT_INIT,"ds=%04x, faultInstance=%Fp, notifyInstance=%Fp,Instance=%04x",
        _FP_SEG( &fault_fn ), fault_fn, notify_fn, Instance ));
    if( WDebug386 ) {
        if( Start386Debug() ) {
            DebuggerIsExecuting( 1 );
            Out((OUT_INIT,"Hooked Interrupts"));
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
    if( fault_fn != NULL ) {
        FreeProcInstance_INTH( fault_fn );
    }
    NotifyUnRegister( NULL );
    if( notify_fn != NULL ) {
        FreeProcInstance_NOTIFY( notify_fn );
    }
    if( WDebug386 ) {
        ResetDebugInterrupts32();
        DebuggerIsExecuting( -1 );
        Out((OUT_INIT,"Debug interrupts reset"));
        UseHotKey( 0 );
        Done386Debug();
    }
//    FreeProcInstance( SubClassProcInstance );
    FiniDebugHook();
    WasInt32 = false;
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
trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version        ver;

    /* unused parameters */ (void)remote;

    DebuggerTask = GetCurrentTask();

    ClearScreen();
    Out(( OUT_INIT,"TrapInit entered, debugger task=%04x", DebuggerTask ));

    BreakOpcode = BRKPOINT;

#ifdef DEBUG
    if( *parms == '[' ) {
        unsigned    bit;
        char        c;
        ++parms;
        while( (c = *parms) != '\0' ) {
            ++parms;
            if( c == ']' )
                break;
            switch( c ) {
            case 'a':
                bit = OUT_ALL;
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
            if( *parms == '~' ) {
                DbgFlags &= ~bit;
                ++parms;
            } else {
                DbgFlags |= bit;
            }
        }
    }
#endif
    if( parms[0] == 'c' && parms[1] == 'g' && parms[2] == 'e' ) {
        DebugDebugeeOnly = true;
    }
    if( parms[0] == '3' && parms[1] == '2' ) {
        StopOnExtender = true;
    }

    err[0] = 0;
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;

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
void TRAPENTRY_FUNC( InfoFunction )( HWND hwnd )
{

    DebuggerWindow = hwnd;
    Out(( OUT_INIT,"DebuggerWindow = %04x", DebuggerWindow ));
    if( hwnd == NULL ) {
        SetInputLock( false );
    }
}

/*
 * GetHwndFunc - inform trap file of gui debugger being used
 */
HWND TRAPENTRY_FUNC( GetHwndFunc )( void )
{
    return( DebuggerWindow );
}

/*
 * set input hook routine
 */
void TRAPENTRY_FUNC( InputHook )( event_hook_fn *ptr )
{
    HookRtn = ptr;
}

/*
 * HardModeCheck - obsolete
 */
bool TRAPENTRY_FUNC( HardModeCheck )( void )
{
    return( HardModeRequired );
}

/*
 * SetHardMode - force hard mode
 */
void TRAPENTRY_FUNC( SetHardMode )( bool force )
{
    ForceHardMode = force;
}

/*
 * UnLockInput - unlock input from the debugger
 */
void TRAPENTRY_FUNC( UnLockInput )( void )
{
    SetInputLock( false );
}
