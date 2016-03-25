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
#include "stdwin.h"
#include "dbgrmsg.h"
#include "trpsys.h"

/*
 * hardModeDebugState
 */
static bool hardModeDebugState( void )
{
    if( SystemDebugState == 0 ) {
        return( false );
    }
    if( SystemDebugState == SDS_NOTASKQUEUE ) {
        return( false );
    }
    return( true );

} /* hardModeDebugState */

/*
 * ToDebugger - switch to debugger
 */
BOOL ToDebugger( private_msg pmsg )
{

    SystemDebugState = GetSystemDebugState();
#ifdef DEBUG
    if( SystemDebugState == 0 ) {
        Out(( OUT_SOFT,"ToDebugger: SDS=0" ));
    }
    if( SystemDebugState & SDS_MENU ) {
        Out(( OUT_SOFT,"ToDebugger: SDS=SDS_MENU" ));
    }
    if( SystemDebugState & SDS_SYSMODAL ) {
        Out(( OUT_SOFT,"ToDebugger: SDS=SDS_SYSMODAL" ));
    }
    if( SystemDebugState & SDS_NOTASKQUEUE ) {
        Out(( OUT_SOFT,"ToDebugger: SDS=SDS_NOTASKQUEUE" ));
    }
    if( SystemDebugState & SDS_DIALOG ) {
        Out(( OUT_SOFT,"ToDebugger: SDS=SDS_DIALOG" ));
    }
    if( SystemDebugState & SDS_TASKLOCKED ) {
        Out(( OUT_SOFT,"ToDebugger: SDS=SDS_TASKLOCKED" ));
    }
#endif
    if( SystemDebugState == 0 && DebuggerWindow != NULL ) {
        EnterSoftMode();
    }
    DebuggerState = ACTIVE;
    if( DebuggerWindow != NULL ) {
        Out(( OUT_RUN,"Going to Debugger Now via Window" ));
        PostMessage( DebuggerWindow, WM_NULL, pmsg, MAGIC_COOKIE );
    } else {
        Out(( OUT_RUN,"Going to Debugger Now via Task" ));
        PostAppMessage( DebuggerTask, WM_NULL, pmsg, MAGIC_COOKIE );
    }
    DirectedYield( DebuggerTask );
    return( TRUE );

} /* ToDebugger */

void EnableMainMenu( UINT bit )
{
    HMENU       menu;
    int         i;

    menu = NULL;
    if( DebuggerWindow != NULL ) {
        menu = GetMenu( DebuggerWindow );
    }
    Out((OUT_SOFT,"Enable menu item, DebuggerWindow=%04x, menu=%04x", DebuggerWindow, menu));
    if( menu == NULL ) return;
    for( i = 0; ; ++i ) {
        if( EnableMenuItem( menu, i, MF_BYPOSITION | bit ) == -1 ) break;
    }
}
/*
 * DebuggerWaitForMessage - wait for a message from a debugee
 */
private_msg DebuggerWaitForMessage( debugger_state state, HANDLE task,
                                        WORD dbgeemsg )
{
    MSG         msg;
    HCURSOR     cursor;

    DebuggerState = state;

    /*
     * restart specified task
     */
    if( task != NULL ) {
        AppMessage = dbgeemsg;
        if( SystemDebugState != SDS_NOTASKQUEUE ) {
            Out((OUT_MSG,"Posting message to task=%4.4x",task));
            PostAppMessage( task, WM_NULL, dbgeemsg, MAGIC_COOKIE );
        }
        Out((OUT_RUN,"Directed Yield to=%4.4x",task));
        DirectedYield( task );
    }
    if( state == RELEASE_DEBUGEE ) {
        DebuggerState = ACTIVE;
        return( 0 );
    }

    /*
     * hang out and wait
     */
    EnableMainMenu( MF_DISABLED );
    if( DebuggerWindow != NULL ) {
        cursor = LoadCursor( NULL, IDC_WAIT );
    }
    for( ;; ) {
        GetMessage( &msg, NULL, 0, 0 );
        Out((OUT_RUN,"DEBUGGER msg: hwnd=%04x, msg=%04x, wp=%04x, lp=%08lx", msg.hwnd, msg.message, msg.wParam, msg.lParam ));
        if( /*msg.hwnd == NULL &&*/ msg.message == WM_NULL ) {
            if( msg.lParam != MAGIC_COOKIE ) {
                continue;
            }
            if( DebugeeTask != NULL && ( hardModeDebugState() || msg.wParam == ASYNCH_STOP ) ) {
                Out((OUT_SOFT,"Entering HardMode"));
                HardModeRequired = true;
            } else {
                HardModeRequired = false;
            }
            if( DebuggerWindow != NULL && !InputLocked && msg.wParam != TASK_ENDED && DebugeeTask != NULL ) {
                if( HardModeRequired || ForceHardMode ) {
                    Out((OUT_SOFT,"Locking input to debugger"));
                    if( !LockInput( NULL, DebuggerWindow, TRUE ) ) {
                        Out((OUT_SOFT,"LockInput returned FALSE"));
                    } else {
                        InputLocked = TRUE;
                    }
                }
            }
            break;
        }
        if( DebuggerWindow != NULL && msg.hwnd != NULL ) {
            if( GetWindowTask( msg.hwnd ) == GetWindowTask( DebuggerWindow ) ) {
                switch( msg.message ) {
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                case WM_KEYDOWN:
                    MessageBox( DebuggerWindow, TRP_WIN_no_can_do, TRP_The_WATCOM_Debugger, MB_SYSTEMMODAL|MB_OK|MB_ICONINFORMATION );
                    continue;
                case WM_MOUSEMOVE:
                    if( GetCursor() != cursor ) {
                        SetCursor( cursor );
                    }
                }
            }
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    EnableMainMenu( MF_ENABLED );
    return( msg.wParam );

} /* DebuggerWaitForMessage */
