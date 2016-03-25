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
#include <env.h>
#define INCL_PM
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#define INCL_WINSYS
#define INCL_WINHOOKS
#define INCL_WINMESSAGEMGR
#include <os2.h>
#include <os2dbg.h>
#include <i86.h>
#include "pmhook.h"//
#include "trpimp.h"

extern BOOL APIENTRY WinLockInput( HWND, USHORT );
extern BOOL APIENTRY WinQuerySendMsg( HAB, HMQ, HMQ, PQMSG );
extern BOOL APIENTRY WinReplyMsg( HAB, HMQ, HMQ, MRESULT );
extern BOOL APIENTRY WinWakeThread( HMQ );
extern HMQ  APIENTRY WinQueueFromID( HAB, PID, TID );
extern BOOL APIENTRY WinThreadAssocQueue( HAB, HMQ );


HAB                     HabDebugger;
HWND                    HwndDebugger;

static HWND             HwndDummy;
static bool             InHardMode;
static char             NeedHardMode = 0;
static HOOKPROC         *PSendMsgHookProc;
static SETHMQPROC       *PSetHmqDebugee;
static HMODULE          HookDLL;
//static HWND           DBActiveWnd;
//static HWND           DBFocusWnd;
//static HWND           AppActiveWnd;
//static HWND           AppFocusWnd;

#define MAX_QUEUES      50
static HMQ              AssumedQueues[ MAX_QUEUES ];
static int              NumAssumedQueues;

typedef struct {
    HMQ         hmq;
} thread_data;

static  unsigned long   BeginThreadSem = { 0 };
static  thread_data     *BeginThreadArg;

#define STACK_SIZE 10000

void APIENTRY SoftModeThread( thread_data *thread )
{
    QMSG        qmsg;
    ULONG       rc;
    RECTL       rcl;
    HPS         ps;

    rc = WinThreadAssocQueue( HabDebugger, thread->hmq );
    PSetHmqDebugee( thread->hmq, HwndDummy );
    rc = WinSetHook( HabDebugger, NULL, HK_SENDMSG, (PFN)PSendMsgHookProc, HookDLL );
    while( WinQuerySendMsg( HabDebugger, NULL, thread->hmq, &qmsg ) ) {
        WinReplyMsg( HabDebugger, NULL, thread->hmq, (MRESULT) 0 );
    }
    while( WinGetMsg( HabDebugger, &qmsg, 0, 0, 0 ) ) { // handle messages for task
        switch( qmsg.msg ) {
        case WM_PAINT:
            ps = WinBeginPaint( qmsg.hwnd, 0, &rcl );
            WinEndPaint( ps );
            break;
        default:
            WinDefWindowProc( qmsg.hwnd, qmsg.msg, qmsg.mp1, qmsg.mp2 );
        }
    }
    WinReleaseHook( HabDebugger, NULL, HK_SENDMSG, (PFN)PSendMsgHookProc, HookDLL );
    PSetHmqDebugee( thread->hmq, NULL );
    WinThreadAssocQueue( HabDebugger, NULL );
    WinPostMsg( HwndDebugger, WM_QUIT, 0, 0 ); // tell debugger we're done
}

static void BeginThreadHelper( void )
{
    thread_data *_arg;

    _arg = BeginThreadArg;
    DosSemClear( &BeginThreadSem );
    SoftModeThread( _arg );
    DosExit( EXIT_THREAD, 0 );
}


void BeginSoftModeThread( thread_data *arglist )
{
    TID         tid;
    SEL         sel;
    byte        *stack;

    DosSemRequest( &BeginThreadSem, -1L );
    DosAllocSeg( STACK_SIZE + sizeof( thread_data ), (PSEL)&sel, 0 );
    stack = MK_FP( sel, 0 );
    BeginThreadArg = (thread_data*)stack;
    stack += sizeof( thread_data );
    *BeginThreadArg = *arglist;
    DosCreateThread( (PFNTHREAD)BeginThreadHelper, &tid, stack + STACK_SIZE );
}


char SetHardMode( char hard )
{
    char        old;

    old = NeedHardMode;
    NeedHardMode = hard;
    return( old );
}

BOOL IsPMDebugger( void )
{
    return( HabDebugger != NULL );
}

void CreateDummyWindow( void )
{
    ULONG flCreate;
    HWND        frame;

    WinRegisterClass( HabDebugger, "Dummy", WinDefWindowProc, CS_SIZEREDRAW, 0 );
    flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX;
    frame = WinCreateStdWindow( HWND_DESKTOP, 0L, &flCreate, "Dummy",
                                    "", 0L, 0, 99, &HwndDummy );
    if( frame == NULL ) {
        HwndDummy = HwndDebugger;
    }
}

void GrabThreadQueue( PID pid, TID tid )
{
    thread_data         thread;
    int                 i;

    if( HwndDummy == NULL ) CreateDummyWindow();
    thread.hmq = WinQueueFromID( HabDebugger, pid, tid );
    if( thread.hmq == NULL ) return;
    for( i = 0; i < NumAssumedQueues; ++i ) {
        if( thread.hmq == AssumedQueues[i] ) return;
    }
    AssumedQueues[ NumAssumedQueues ] = thread.hmq;
    ++NumAssumedQueues;
    BeginSoftModeThread( &thread );
}

void ReleaseThreadQueue( PID pid, TID tid )
{
    HMQ                 hmq;
    int                 i;

    pid=pid;tid=tid;
    hmq = WinQueueFromID( HabDebugger, pid, tid );
    if( hmq == NULL ) return;
    for( i = 0; i < NumAssumedQueues; ++i ) {
        if( hmq == AssumedQueues[i] ) {
            WinPostQueueMsg( hmq, WM_QUIT, 0, 0 ); // break one soft mode loop
            AssumedQueues[ i ] = NULL;
            break;
        }
    }
}

void ForAllTids( PID pid, void (*rtn)( PID pid, TID tid ) )
{
    TID tid;

    for( tid = 1; tid <= 256; ++tid ) {
        rtn( pid, tid );
    }
}

void WakeOneThread( PID pid, TID tid )
{
    HMQ         hmq;

    hmq = WinQueueFromID( HabDebugger, pid, tid );
    if( hmq != NULL ) {
        WinWakeThread( hmq );
    }
}

VOID WakeThreads( PID pid )
{
    ForAllTids( pid, WakeOneThread );
}

void EnterSoftMode( PID pid )
{
    if( NumAssumedQueues != 0 ) return;
    ForAllTids( pid, GrabThreadQueue );
//    AppFocusWnd = WinQueryFocus( HWND_DESKTOP, 0 );
//    AppActiveWnd = WinQueryActiveWindow( HWND_DESKTOP, 0 );
//    if( WinIsWindow( HabDebugger, DBFocusWnd ) ) WinSetFocus( HWND_DESKTOP, DBFocusWnd );
//    if( WinIsWindow( HabDebugger, DBActiveWnd ) ) WinSetActiveWindow( HWND_DESKTOP, DBActiveWnd );
}

void ExitSoftMode( PID pid )
{
    int         i;
    QMSG        qmsg;

    ForAllTids( pid, ReleaseThreadQueue );
    for( i = 0; i < NumAssumedQueues; ++i ) { // wait for NumAssumedQueues WM_QUIT messages
        while( WinGetMsg( HabDebugger, &qmsg, 0L, 0, 0 ) ) {
            WinDispatchMsg( HabDebugger, &qmsg );
        }
    }
    NumAssumedQueues = 0;
//    DBFocusWnd = WinQueryFocus( HWND_DESKTOP, 0 );
//    DBActiveWnd = WinQueryActiveWindow( HWND_DESKTOP, 0 );
//    if( WinIsWindow( HabDebugger, AppFocusWnd ) ) WinSetFocus( HWND_DESKTOP, AppFocusWnd );
//    if( WinIsWindow( HabDebugger, AppActiveWnd ) ) WinSetActiveWindow( HWND_DESKTOP, AppActiveWnd );
}

void EnterHardMode( void )
{
    if( InHardMode ) return;
    WinLockInput( 0, TRUE );
    InHardMode = TRUE;
}

void ExitHardMode( void )
{
    if( !InHardMode )
        return;
    WinLockInput( 0, FALSE );
    InHardMode = FALSE;
}

void AssumeQueue( PID pid, TID tid )
{
    tid=tid;
    if( !IsPMDebugger() )
        return;
    if( NeedHardMode == (char)-1 )
        return;
    if( NeedHardMode != 0 ) {
        EnterHardMode();
    } else {
        EnterSoftMode( pid );
    }
}

void ReleaseQueue( PID pid, TID tid )
{
    tid=tid;
    if( !IsPMDebugger() )
        return;
    if( NeedHardMode == (char)-1 )
        return;
    if( NeedHardMode != 0 ) {
        ExitHardMode();
    } else {
        ExitSoftMode( pid );
    }
}

void TellSoftModeHandles( HAB hab, HWND hwnd )
{
    HabDebugger = hab;
    HwndDebugger = hwnd;
}

VOID InitSoftDebug( VOID )
{
    DosLoadModule( NULL, 0, HOOKER, &HookDLL );
    DosGetProcAddr( HookDLL, "SENDMSGHOOKPROC", (PFN*)&PSendMsgHookProc );
    DosGetProcAddr( HookDLL, "SETHMQDEBUGEE", (PFN*)&PSetHmqDebugee );
}
