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
* Description:  'Soft mode' OS/2 PM debugging routines. Warning, highly
*               complex code ahead! Exercise utmost care when modifying.
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#define INCL_PM
#define INCL_DOSMODULEMGR
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_WINSYS
#define INCL_WINHOOKS
#define INCL_WINMESSAGEMGR
#include <os2.h>
#include <os2dbg.h>
#include "pmhook.h"
#include "trpimp.h"
#include "softmode.h"

#ifndef HK_CALLHOOK
  #define HK_CALLHOOK 13
#endif

// Undocumented PM APIs, oh joy!
extern BOOL APIENTRY16 WinLockInput( HWND, USHORT );
extern BOOL APIENTRY16 WinQuerySendMsg( HAB, HMQ, HMQ, PQMSG );
extern BOOL APIENTRY16 WinReplyMsg( HAB, HMQ, HMQ, MRESULT );
extern BOOL APIENTRY16 WinWakeThread( HMQ );
extern HMQ  APIENTRY16 WinQueueFromID( HAB, USHORT, USHORT );
extern BOOL APIENTRY16 WinThreadAssocQueue( HAB, HMQ );


HAB                     HabDebugger;
HWND                    HwndDebugger;

static HWND             HwndDummy;
static BOOL             InHardMode;
static BOOL             NeedHardMode = FALSE;
static HOOKPROC         *PSendMsgHookProc;
static SETHMQPROC       *PSetHmqDebugee;
static HMODULE          HookDLL;
//static HWND           DBActiveWnd;
//static HWND           DBFocusWnd;
//static HWND           AppActiveWnd;
//static HWND           AppFocusWnd;

#define MAX_QUEUES      50
static HMQ              AssumedQueues[MAX_QUEUES];
static int              NumAssumedQueues;

typedef struct {
    HMQ         hmq;
} thread_data;

static  HEV             BeginThreadSem = NULLHANDLE;

#define STACK_SIZE 32768

// If we take over the queue of another process, we may have a serious problem:
// In case the debuggee installed a local queue hook, PM will try to call the
// hook in the context of the debugger, which will almost certainly be fatal.
//

// Following is a brief description of the CallHook hook function arguments and
// return value (taken from OS/2 for PowerPC API Addendum):
//
// HMQ      Current;   /*  Message-queue handle associated with the current thread. */
// HMQ      Created;   /*  Message-queue handle created by the installing thread. */
// PID      Pid;       /*  Process identity of the installing thread. */
// TID      Tid;       /*  Thread identity of the installing thread. */
// SHORT    HookType;  /*  Hook type. */
// PROC     HookProc;  /*  Address of the hook procedure that is about to be called. */
// BOOL     f;         /*  Indicates whether or not the hook HookProc should be called: */

BOOL APIENTRY CallHookProc(HMQ hmqCurrent, HMQ hmqCreated, PID pid, TID tid, SHORT hookType, PFN hookProc)
{
    // Returning TRUE will stop PM from trying to call hooks. We could try to be clever
    // and decide which hooks are safe to call and which are not. For now just don't call any.
    return TRUE;
}

static void APIENTRY SoftModeThread( thread_data *thread )
{
    QMSG        qmsg;
    ULONG       rc;
    RECTL       rcl;
    HPS         ps;

    rc = WinThreadAssocQueue( HabDebugger, thread->hmq );
    rc = WinSetHook( HabDebugger, thread->hmq, HK_CALLHOOK, (PFN)CallHookProc, NULLHANDLE );
    PSetHmqDebugee( thread->hmq, HwndDummy );
    rc = WinSetHook( HabDebugger, NULLHANDLE, HK_SENDMSG, (PFN)PSendMsgHookProc, HookDLL );
    while( WinQuerySendMsg( HabDebugger, NULLHANDLE, thread->hmq, &qmsg ) ) {
        WinReplyMsg( HabDebugger, NULLHANDLE, thread->hmq, (MRESULT)0 );
    }
    while( WinGetMsg( HabDebugger, &qmsg, 0, 0, 0 ) ) { // handle messages for task
        switch( qmsg.msg ) {
            case WM_PAINT:
                // don't do any painting
                ps = WinBeginPaint( qmsg.hwnd, 0, &rcl );
                WinEndPaint( ps );
                break;
            default:
                // have the default window procedure handle the rest
                WinDefWindowProc( qmsg.hwnd, qmsg.msg, qmsg.mp1, qmsg.mp2 );
        }
    }
    WinReleaseHook( HabDebugger, NULLHANDLE, HK_SENDMSG, (PFN)PSendMsgHookProc, HookDLL );
    PSetHmqDebugee( thread->hmq, NULLHANDLE );
    WinReleaseHook( HabDebugger, thread->hmq, HK_CALLHOOK, (PFN)CallHookProc, NULLHANDLE );
    WinThreadAssocQueue( HabDebugger, NULLHANDLE );
    WinPostMsg( HwndDebugger, WM_QUIT, 0, 0 ); // tell debugger we're done
}

// Here's the deal with BeginThreadHelper() and the event sem: each thread
// is given its own thread_data structure. BeginThreadHelper() gets a pointer
// to the data passed in but it has to copy the data to its own (thread
// specific) stack. The event sem must be used to prevent the creating thread
// from trashing the data before the thread is done copying it.
// Note: Currently thread_data only contains a single HMQ field. We could
// just pass it as a thread argument and skip these shenanigans. But hey,
// it's fun and it would be needed anyway if thread_data were extended.
static VOID APIENTRY BeginThreadHelper( ULONG arg )
{
    thread_data tdata;
    thread_data *_arg = (thread_data*)arg;

    tdata = *_arg;
    DosPostEventSem( BeginThreadSem );
    SoftModeThread( &tdata );
    DosExit( EXIT_THREAD, 0 );
}


static void BeginSoftModeThread( thread_data *arglist )
{
    TID         tid;
    ULONG       ulCount;

    DosResetEventSem( BeginThreadSem , &ulCount );
    DosCreateThread( &tid, BeginThreadHelper, (ULONG)arglist, 0, STACK_SIZE );
    DosWaitEventSem( BeginThreadSem, SEM_INDEFINITE_WAIT );
}


char SetHardMode( char hard )
{
    char        old;

    old = NeedHardMode;
    NeedHardMode = hard;
    return old;
}

BOOL IsPMDebugger( void )
{
    return( HabDebugger != NULLHANDLE );
}

static void CreateDummyWindow( void )
{
    ULONG     flCreate;
    HWND      frame;

    WinRegisterClass( HabDebugger, "Dummy", WinDefWindowProc, CS_SIZEREDRAW, 0 );
    flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX;
    frame = WinCreateStdWindow( HWND_DESKTOP, 0L, &flCreate, "Dummy",
                                "", 0L, NULLHANDLE, 99, &HwndDummy );
    if( frame == NULLHANDLE ) {
        HwndDummy = HwndDebugger;
    }
}

static void GrabThreadQueue( PID pid, TID tid )
{
    thread_data         thread;
    int                 i;

    if( HwndDummy == NULLHANDLE )
        CreateDummyWindow();
    thread.hmq = WinQueueFromID( HabDebugger, pid, tid );
    if( thread.hmq == NULLHANDLE )
        return;
    for( i = 0; i < NumAssumedQueues; ++i ) {
        if( thread.hmq == AssumedQueues[i] )
            return;
    }
    AssumedQueues[NumAssumedQueues] = thread.hmq;
    ++NumAssumedQueues;
    BeginSoftModeThread( &thread );
}

static void ReleaseThreadQueue( PID pid, TID tid )
{
    HMQ                 hmq;
    int                 i;

    hmq = WinQueueFromID( HabDebugger, pid, tid );
    if( hmq == NULLHANDLE )
        return;
    for( i = 0; i < NumAssumedQueues; ++i ) {
        if( hmq == AssumedQueues[i] ) {
            WinPostQueueMsg( hmq, WM_QUIT, 0, 0 ); // break one soft mode loop
            AssumedQueues[i] = NULLHANDLE;
            break;
        }
    }
}

static void ForAllTids( PID pid, void (*rtn)( PID pid, TID tid ) )
{
    TID tid;

    for( tid = 1; tid <= 256; ++tid ) {
        rtn( pid, tid );
    }
}

static void WakeOneThread( PID pid, TID tid )
{
    HMQ         hmq;

    hmq = WinQueueFromID( HabDebugger, pid, tid );
    if( hmq != NULLHANDLE ) {
        WinWakeThread( hmq );
    }
}

VOID WakeThreads( PID pid )
{
    ForAllTids( pid, WakeOneThread );
}

static void EnterSoftMode( PID pid )
{
    if( NumAssumedQueues != 0 )
        return;
    ForAllTids( pid, GrabThreadQueue );
//    AppFocusWnd = WinQueryFocus( HWND_DESKTOP, 0 );
//    AppActiveWnd = WinQueryActiveWindow( HWND_DESKTOP, 0 );
//    if (WinIsWindow(HabDebugger, DBFocusWnd)) WinSetFocus(HWND_DESKTOP, DBFocusWnd);
//    if (WinIsWindow(HabDebugger, DBActiveWnd)) WinSetActiveWindow(HWND_DESKTOP, DBActiveWnd);
}

static void ExitSoftMode( PID pid )
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
//    if (WinIsWindow(HabDebugger, AppFocusWnd)) WinSetFocus(HWND_DESKTOP, AppFocusWnd);
//    if (WinIsWindow(HabDebugger, AppActiveWnd)) WinSetActiveWindow(HWND_DESKTOP, AppActiveWnd);
}

static void EnterHardMode( void )
{
    if( InHardMode )
        return;
    WinLockInput( 0, TRUE );
    InHardMode = TRUE;
}

static void ExitHardMode( void )
{
    if( !InHardMode )
        return;
    WinLockInput( 0, FALSE );
    InHardMode = FALSE;
}

void AssumeQueue( PID pid, TID tid )
{
    tid = tid;
    if( !IsPMDebugger() )
        return;
    if( NeedHardMode == (char) - 1 )
        return;
    if( NeedHardMode ) {
        EnterHardMode();
    } else {
        EnterSoftMode( pid );
    }
}

void ReleaseQueue( PID pid, TID tid )
{
    tid = tid;
    if( !IsPMDebugger() )
        return;
    if( NeedHardMode == (char) - 1 )
        return;
    if( NeedHardMode ) {
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

VOID InitSoftDebug(VOID)
{
    // Create the thread creation event sem and load the queue hook DLL
    DosCreateEventSem( NULL, &BeginThreadSem, 0, FALSE );
    DosLoadModule( NULL, 0, HOOKER, &HookDLL );
    DosQueryProcAddr( HookDLL, 0, "SendMsgHookProc", (PFN*)&PSendMsgHookProc );
    DosQueryProcAddr( HookDLL, 0, "SetHmqDebugee", (PFN*)&PSetHmqDebugee );
}
