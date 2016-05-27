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
#include <malloc.h>
#include "wdebug.h"
#include "stdwin.h"
#include "trpsys.h"

#ifndef WM_PAINTICON
#define WM_PAINTICON    0x26
#endif

typedef struct scwinds {
    HWND                hwnd;
    FARPROC             oldproc;
} scwinds;

static scwinds SCWindows[128];
static WORD     SCCount;
static FARPROC  DefaultProcInstance;
static FARPROC  EnumChildProcInstance;
static FARPROC  EnumTaskProcInstance;

/*
 * SubClassProc - handle all messages for the stopped task
 */
static long SubClassProc( HWND hwnd, unsigned message, WORD wparam, LONG lparam )
{
    PAINTSTRUCT ps;
    HDC         dc;

    lparam = lparam;
    wparam = wparam;

    switch( message ) {
    case WM_SYSCOMMAND:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSCHAR:
    case WM_KEYUP:
    case WM_KEYDOWN:
    case WM_CHAR:
        return( DefWindowProc( hwnd, message, wparam, lparam ) );
    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
        return( TRUE );
    case WM_MOUSEACTIVATE:
        return( MA_NOACTIVATE );
    case WM_PAINTICON:
    case WM_PAINT:
        dc = BeginPaint( hwnd, &ps );
        EndPaint( hwnd, &ps);
        break;
    case WM_TIMER:
        break;
    default:
        Out((OUT_MSG,"Debugee msg: hwnd=%04x, msg=%04x, wp=%04x, lp=%08lx",
                        hwnd, message, wparam, lparam ));
        break;
    }
    return( 0L );

}

long __export FAR PASCAL DefaultProc( HWND hwnd, unsigned message, WORD wparam, LONG lparam )
{
    Out((OUT_MSG,"Default Proc: hwnd=%04x, msg=%04x, wp=%04x, lp=%08lx",
                    hwnd, message, wparam, lparam ));
    return( DefWindowProc( hwnd, message, wparam, lparam ) );
}

/*
 * EnumTaskWindowsFunc:
 *
 * enumerate all task windows, and then sub-class them to point
 * to SubClassProc
 *
 * NOTE: this has some problems, so instead of subclassing, we call
 * SubClassProc directly from the message loop we go into for the debugee.
 *
 */
static BOOL SubClassWindow( HWND hwnd, BOOL do_children )
{
    FARPROC     fp;
    char        buffer[80];

    if( hwnd == NULL ) {
        return( 0 );
    }
    buffer[0]=0;
    GetClassName( hwnd, buffer, sizeof( buffer ) );
    Out((OUT_SOFT,"--- Subclass (%s), id=%04x",buffer,hwnd));
    if( GetWindowWord( hwnd, GWW_HINSTANCE ) == (WORD)GetModuleHandle( "USER") ) {
        Out((OUT_SOFT,"--- Subclass IGNORED (USER)" ));
        return( 1 );
    }
    if( buffer[0] != '#' ) { // don't subclass predefined windows classes
        fp = (FARPROC)SetWindowLong( hwnd, GWL_WNDPROC, (LONG)DefaultProcInstance );
        SCWindows[ SCCount ].hwnd = hwnd;
        SCWindows[ SCCount ].oldproc = fp;
        SCCount++;
        if( do_children ) EnumChildWindows( hwnd, (WNDENUMPROC)EnumChildProcInstance, 0 );
    }
    return( 1 );

}

BOOL __export FAR PASCAL EnumTaskWindowsFunc( HWND hwnd, DWORD lparam )
{
    lparam=lparam;
    return( SubClassWindow( hwnd, TRUE ) );
}

BOOL __export FAR PASCAL EnumChildWindowsFunc( HWND hwnd, DWORD lparam )
{
    lparam=lparam;
    return( SubClassWindow( hwnd, FALSE ) );
}

/*
 * EnterSoftMode:
 *
 * go into soft mode.  We subclass all the task windows, and reply
 * to any outstanding messages.
 *
 * NOTE: we currently do subclassing but we still do the message loop thing.
 *       the subclassing is just to prevent the application's window proc
 *       from being called re-entrantly when the softmode loop calls GetMessage
 */
void EnterSoftMode( void )
{
    MSG         msg;

    if( InSoftMode ) {
        return;
    }
    InSoftMode = true;
    Out((OUT_SOFT,"Entering Soft Mode, task at fault=%04x", TaskAtFault ));

    while( QuerySendMessage( NULL, NULL, NULL, &msg ) ) {
        Out((OUT_SOFT,"--- Replying to a message"));
        ReplyMessage( 1 );
    }

} /* EnterSoftMode */

/*
 * ExitSoftMode:
 *
 * reset all subclassed windows
 */
void ExitSoftMode( void )
{
    int i;

    if( !InSoftMode ) {
        return;
    }
    InSoftMode = false;

    Out((OUT_SOFT,"Exiting Soft Mode" ));

    for( i=SCCount-1;i>=0;--i ) { // go in reverse in case window subclassed 2x
        SetWindowLong( SCWindows[i].hwnd, GWL_WNDPROC, (LONG) SCWindows[i].oldproc );
    }
    SCCount = 0;

} /* ExitSoftMode */


static BOOL IsTaskWnd( HWND wnd )
{
    if( wnd == NULL ) return( FALSE );
    if( !IsWindow( wnd ) ) return( FALSE );
    if( GetWindowTask( wnd ) != DebugeeTask ) return( FALSE );
    return( TRUE );
}

/*
 * DebugeeWaitForMessage:
 *
 * Wait for a message from the debugger for the debuggee.  Note that
 * the debugee is the one that is executing this code.
 *
 * We go into two possible loops here: either a hard mode loop or a soft
 * mode loop.
 *
 * We go into a soft mode loop, unless:
 *  1) We are asked to be in hard mode
 *  2) We are the full-screen (character mode) debugger
 *  3) The system debug state is non-zero (i.e., we are in one of the states
 *     that require hard mode - no task message queue, a menu is dropped,
 *     a system-modal dialog box is up, a dialog box is up, or the current
 *     task is in a locked state)
 *     *NOTE*  The "go into hard mode if a dialog box is up" requirement
 *             may not be requred.  That state was added to
 *             GetSystemDebugState AFTER I did this.
 *
 * In the hard mode loop, we simply yield to the debugger until the
 * DebuggerState variable changes from "ACTIVE"
 *
 * In the soft-mode loop, we actually do the traditional GetMessage thing.
 * If we get a message that is for the desktop or for a USER task (i.e.,
 * the "switch task" dialog), we translate and dispatch it.   Otherwise,
 * we call SubClassProc to handle the message.
 */

HWND ActiveWnd;
HWND FocusWnd;

restart_opts DebugeeWaitForMessage( void )
{
    MSG         msg;
    HANDLE      huser;
    HWND        hwnddtop;
    HWND        capture;
    HWND        wnd;
    HANDLE      hinst;

    /*
     * give up any capture in context of debugee
     */
    capture = SetCapture(NULL);
    if( capture != NULL ) {
        ReleaseCapture();
        Out((OUT_SOFT,"Capture hwnd=%04x", capture ));
    }

    /*
     * hang out and wait
     */
    huser = GetModuleHandle( "USER");
    hwnddtop = GetDesktopWindow();
    wnd = GetFocus();
    if( IsTaskWnd( wnd ) ) {
        FocusWnd = wnd;
    }
    wnd = GetActiveWindow();
    if( IsTaskWnd( wnd ) ) {
        ActiveWnd = wnd;
    }
    if( HardModeRequired || SystemDebugState == SDS_NOTASKQUEUE || DebuggerWindow == NULL ) {
        Out((OUT_SOFT,"In HardMode Loop! req=%d,SDS=%d,Window=%4.4x",HardModeRequired,SystemDebugState,DebuggerWindow ));
        while( DebuggerState == ACTIVE ) {
            DirectedYield( DebuggerTask );
        }
        if( capture != NULL ) {
            SetCapture( capture );
        }
        return( AppMessage );
    }

    Out((OUT_SOFT,"In SoftMode Loop task=%04x(%04x), act=%04x, foc=%04x, t=%d, DW=%04x", GetCurrentTask(), DebugeeTask, ActiveWnd, FocusWnd, TraceOn, DebuggerWindow ));

    DefaultProcInstance = (FARPROC)MakeProcInstance( (FARPROC)DefaultProc, DebugeeInstance );

    EnumTaskProcInstance = MakeProcInstance( (FARPROC)EnumTaskWindowsFunc, DebugeeInstance );
    EnumChildProcInstance = MakeProcInstance( (FARPROC)EnumChildWindowsFunc, DebugeeInstance );
    EnumTaskWindows( GetCurrentTask(), (WNDENUMPROC)EnumTaskProcInstance, 0 );
    FreeProcInstance( EnumChildProcInstance );
    FreeProcInstance( EnumTaskProcInstance );

    while( 1 ) {
        GetMessage( &msg, NULL, 0, 0 );
        if( msg.hwnd == NULL &&
            msg.message == WM_NULL && msg.lParam == MAGIC_COOKIE ) break;
        if( msg.hwnd != NULL ) {
            hinst = (HINSTANCE)GetWindowWord( msg.hwnd, GWW_HINSTANCE );
        } else {
            hinst = NULL;
        }
        if( msg.hwnd == hwnddtop || hinst == huser ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        } else {
            SubClassProc( msg.hwnd, msg.message, msg.wParam, msg.lParam );
        }
    }
    if( !TraceOn && DebuggerWindow != NULL ) {
        if( IsTaskWnd( FocusWnd ) ) {
            Out((OUT_SOFT,"Focus Window to %4.4x", FocusWnd ));
            SetFocus( FocusWnd );
        }
        if( IsTaskWnd( ActiveWnd ) ) {
            Out((OUT_SOFT,"Active Window to %4.4x", ActiveWnd ));
            SetActiveWindow( ActiveWnd );
        }
    }
    ExitSoftMode();
    FreeProcInstance( DefaultProcInstance );
    if( capture != NULL ) {
        SetCapture( capture );
    }
    Out((OUT_SOFT,"active=%04x, focus=%04x, TraceOn=%d, DW=%04x", ActiveWnd, FocusWnd, TraceOn, DebuggerWindow ));
    PostAppMessage( GetCurrentTask(), WM_NULL, 0, 0L );
    return( msg.wParam );

} /* DebugeeWaitForMessage */
