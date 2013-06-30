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
* Description:  Spy hook DLL implementation.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "wi163264.h"
#include "spydll.h"

typedef struct
{
    LPARAM      lParam;
    WPARAM      wParam;
    UINT        wMsg;
    HWND        hWnd;
} callstruct;
typedef callstruct FAR *LPCALLMSG;

SPYDLLENTRY LRESULT CALLBACK CallWndProcFilter( int ncode, WPARAM wparam, LPARAM lparam );
SPYDLLENTRY LRESULT CALLBACK GetMessageFilter( int ncode, WPARAM wparam, LPARAM lparam );

static HHOOK            callHookHandle, getHookHandle;
static BOOL             isFiltering = FALSE;
static HINSTANCE        dllInstance;

#ifndef __NT__
void (FAR *HandleMessage)( LPMSG pmsg );
#else
static HWND             spyHwnd;
static HWND             spyLBHwnd;
#endif

#ifdef __NT__

static void findSpyHwnd( void )
{
    spyHwnd = FindWindow( SPY_CLASS_NAME, NULL );
    spyLBHwnd = (HWND)GET_WNDINFO( spyHwnd );
}

/*
 * HandleMessage - send info back to spy for the NT version ONLY
 */
static void HandleMessage( MSG *data )
{
    COPYDATASTRUCT      info;

    if( !IsWindow( spyHwnd ) ) {
        findSpyHwnd();
    }
    if( data->hwnd != spyHwnd && data->hwnd != spyLBHwnd ) {
        info.cbData = sizeof( MSG );
        info.lpData = data;
        SendMessage( spyHwnd, WM_COPYDATA, 0, (LPARAM)&info );
    }
}

BOOL WINAPI DllMain( HINSTANCE inst, DWORD reason, LPVOID *ptr )
{
    reason = reason;
    ptr = ptr;

    dllInstance = inst;
    if ( reason == DLL_PROCESS_ATTACH ) {
        findSpyHwnd();
    }
    return( TRUE );
}

#else

int WINAPI LibMain( HINSTANCE hmod, WORD dataseg, WORD heap, LPSTR cmdline )
{
    dataseg = dataseg;
    heap = heap;
    cmdline = cmdline;

    dllInstance = hmod;
    return( 1 );
}

int WINAPI WEP( int res )
{
    res = res;

    return( 1 );
}

#endif

/*
 * CallWndProcFilter - hook for calls to wndprocs
 */
LRESULT CALLBACK CallWndProcFilter( int ncode, WPARAM wparam, LPARAM lparam )
{
    MSG         msg;
    LPCALLMSG   pcm;

    if( ncode >= 0 ) {
        pcm = (LPCALLMSG)lparam;
        msg.hwnd = pcm->hWnd;
        msg.lParam = pcm->lParam;
        msg.wParam = pcm->wParam;
        msg.message = pcm->wMsg;
        HandleMessage( &msg );
    }
    return( CallNextHookEx( callHookHandle, ncode, wparam, lparam ) );

} /* CallWndProcFilter */

/*
 * GetMessageFilter - hook for get message
 */
LRESULT CALLBACK GetMessageFilter( int ncode, WPARAM wparam, LPARAM lparam )
{
    if( ncode >= 0 ) {
        HandleMessage( (LPMSG)lparam );
    }
    return( CallNextHookEx( getHookHandle, ncode, wparam, lparam ) );

} /* GetMessageFilter */

/*
 * SetFilter - set up the message filter
 */
void CALLBACK SetFilter( LPVOID hdlmsg )
{
#ifdef __NT__
    hdlmsg = hdlmsg;
#else
    HandleMessage = hdlmsg;
#endif

    if( !isFiltering ) {
        callHookHandle = SetWindowsHookEx( WH_CALLWNDPROC, CallWndProcFilter, dllInstance, /*(HTASK)*/ 0 );
        getHookHandle = SetWindowsHookEx( WH_GETMESSAGE, GetMessageFilter, dllInstance, /*(HTASK)*/ 0 );
        isFiltering = TRUE;
    }

} /* SetFilter */

/*
 * ClearFilter - clear out existing filter
 */
void CALLBACK ClearFilter( void )
{
    if( isFiltering ) {
        UnhookWindowsHookEx( callHookHandle );
        UnhookWindowsHookEx( getHookHandle );
        isFiltering = FALSE;
    }

} /* ClearFilter */
