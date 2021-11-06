/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Handling of F1 key (help)
*
****************************************************************************/


#include "guiwind.h"
#include "guixutil.h"
#include "guicontr.h"
#include "guixwind.h"
#include "wclbproc.h"


/* Local Window callback functions prototypes */
#ifdef __OS2_PM__
int                     CALLBACK F1Proc( HAB hab, WPI_QMSG *qmsg, ULONG fs );
#else
WINEXPORT LRESULT       CALLBACK F1Proc( int code, WPARAM dummy, LPARAM msg_param );
#endif

static  unsigned        F1Hooked = 0;
#ifndef __OS2_PM__
static  HHOOK           F1HookHandle = (HHOOK)NULL;
static  HOOKPROC        F1ProcInst;
#else
static  PFN             F1ProcInst;
#endif

// What the hell does this do??!!
static bool scroll_catch_check( WPI_QMSG *qmsg )
{
    (void)qmsg;

    return( false );
}

static gui_window *getFirstGUIParent( HWND hwnd )
{
    HWND        parent;
    gui_window  *wnd;

    wnd = NULL;

    for( ;; ) {
        wnd = GUIGetCtrlWnd( hwnd );
        if( wnd != NULL )
            break;
        //wnd = GUIFindWindowFromHWND( hwnd );
        //if( wnd != NULL )
        //    break;
        parent = _wpi_getparent( hwnd );
        if( parent == HWND_DESKTOP )
            break;
        hwnd = parent;
    }

    return( wnd );
}

#ifdef __OS2_PM__
int CALLBACK F1Proc( HAB hab, WPI_QMSG *qmsg, ULONG fs )
#else
LRESULT CALLBACK F1Proc( int code, WPARAM dummy, LPARAM msg_param )
#endif
{
    WPI_MSG             message;
    WPI_PARAM1          parm1;
    WPI_PARAM2          parm2;
    gui_window          *curr;
    gui_window          *root;
    bool                b;
#ifdef __OS2_PM__
    int                 code;
#else
    WPI_QMSG            *qmsg;
#endif

#ifdef __OS2_PM__
    fs = fs;            // unused
    hab = hab;          // unused
    code = 0;
#else
    qmsg = (WPI_QMSG *)msg_param;
#endif

    message = _wpi_qmsgmessage( qmsg );
    parm1 = _wpi_qmsgparam1( qmsg );
    parm2 = _wpi_qmsgparam2( qmsg );

    if( code >= 0 ) {
        if( scroll_catch_check( qmsg ) ) {
            return( 1 );
        } else if( _wpi_ismsgkeydown( message, parm1 ) && _wpi_getvk( parm1, parm2 ) == VK_F1 ) {
            curr = getFirstGUIParent( qmsg->hwnd );
            if( curr != NULL ) {
                b = true;
                GUIEVENT( curr, GUI_CONTEXTHELP, &b );
                root = GUIGetRootWindow();
                if( root != NULL ) {
                    b = false;
                    GUIEVENT( root, GUI_CONTEXTHELP, &b );
                }
            }
        }
    }

#ifdef __OS2_PM__
    return( false );            // No, we didn't gobble this msg
#else
    return( CallNextHookEx( F1HookHandle, code, dummy, msg_param ) );
#endif
}

void GUIAPI GUIHookF1( void )
{
    if( F1Hooked == 0 ) {
#ifndef __OS2_PM__
        // we use a hook to trap F1 in dialogs that were not
        // created using this instance yet on our behalf. IE COMMDLG stuff

        // we cant use a system wide hook because they only can be
        // used in DLL's
        F1ProcInst = MakeProcInstance_HOOK( F1Proc, GUIMainHInst );
    #if defined( __WINDOWS__ )
        F1HookHandle = SetWindowsHookEx( WH_MSGFILTER, F1ProcInst, GUIMainHInst, GetCurrentTask() );
    #else
        F1HookHandle = SetWindowsHookEx( WH_MSGFILTER, F1ProcInst, GUIMainHInst, GetCurrentThreadId() );
    #endif
#else
        // in OS/2, it has to be an app. specific input filter (OS/2 has
        // bad problems, occassionally, with system input hooks)
        F1ProcInst = (PFN)F1Proc;
        WinSetHook( GUIMainHInst.hab, HMQ_CURRENT, HK_INPUT, F1ProcInst, GUIMainHInst.mod_handle );
#endif
    }
    F1Hooked++;
}

void GUIAPI GUIUnHookF1( void )
{
    if( F1Hooked == 1 ) {
#ifndef __OS2_PM__
        UnhookWindowsHookEx( F1HookHandle );
        FreeProcInstance_HOOK( F1ProcInst );
#else
        WinReleaseHook( GUIMainHInst.hab, HMQ_CURRENT, HK_INPUT, F1ProcInst, GUIMainHInst.mod_handle );
#endif
    }

    if( F1Hooked != 0 ) {
        F1Hooked--;
    }
}
