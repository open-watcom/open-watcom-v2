/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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


#include "spy.h"
#include "spydll.h"
#include "dllmain.h"


#ifdef __WINDOWS__
typedef struct
{
    LPARAM  lParam;
    WPARAM  wParam;
    UINT    message;
    HWND    hwnd;
} CWPSTRUCT;
typedef CWPSTRUCT FAR *LPCWPSTRUCT;
#endif

SPYDLLENTRY LRESULT CALLBACK CallWndProcFilter( int ncode, WPARAM wparam, LPARAM lparam );
SPYDLLENTRY LRESULT CALLBACK GetMessageFilter( int ncode, WPARAM wparam, LPARAM lparam );

static HHOOK            callHookHandle;
static HHOOK            getHookHandle;
static bool             isFiltering = false;
static HINSTANCE        dllInstance;
#ifdef __WINDOWS__
static message_func     *dll_HandleMessage;
#endif

#ifdef __NT__
static HWND             spyHwnd;
static HWND             spyLBHwnd;

static void findSpyHwnd( void )
{
    spyHwnd = FindWindow( SPY_CLASS_NAME, NULL );
    spyLBHwnd = (HWND)GET_WNDINFO( spyHwnd );
}

/*
 * dll_HandleMessage - send info back to spy for the NT version ONLY
 */
static void dll_HandleMessage( LPMSG data )
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

BOOL WINAPI DllMain( HINSTANCE inst, DWORD reason, LPVOID ptr )
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

/*
 * SetFilterProc - set up the message filter procedure
 */
SPYDLLENTRY void CALLBACK SetFilterProc( message_func *hdlmsg )
{
    dll_HandleMessage = hdlmsg;
}

int WINAPI LibMain( HINSTANCE hmod, WORD dataseg, WORD heap, LPSTR cmdline )
{
    /* unused parameters */ (void)dataseg; (void)heap; (void)cmdline;

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
    LPCWPSTRUCT pcm;

    if( ncode >= 0 ) {
        pcm = (LPCWPSTRUCT)lparam;
        msg.hwnd = pcm->hwnd;
        msg.lParam = pcm->lParam;
        msg.wParam = pcm->wParam;
        msg.message = pcm->message;
        dll_HandleMessage( &msg );
    }
    return( CallNextHookEx( callHookHandle, ncode, wparam, lparam ) );

} /* CallWndProcFilter */

/*
 * GetMessageFilter - hook for get message
 */
LRESULT CALLBACK GetMessageFilter( int ncode, WPARAM wparam, LPARAM lparam )
{
    if( ncode >= 0 ) {
        dll_HandleMessage( (LPMSG)lparam );
    }
    return( CallNextHookEx( getHookHandle, ncode, wparam, lparam ) );

} /* GetMessageFilter */

/*
 * SetFilter - set up the message filter
 */
SPYDLLENTRY void CALLBACK SetFilter( void )
{
    if( !isFiltering ) {
        callHookHandle = SetWindowsHookEx( WH_CALLWNDPROC, CallWndProcFilter, dllInstance, /*(HTASK)*/ 0 );
        getHookHandle = SetWindowsHookEx( WH_GETMESSAGE, GetMessageFilter, dllInstance, /*(HTASK)*/ 0 );
        isFiltering = true;
    }

} /* SetFilter */

/*
 * ClearFilter - clear out existing filter
 */
SPYDLLENTRY void CALLBACK ClearFilter( void )
{
    if( isFiltering ) {
        UnhookWindowsHookEx( callHookHandle );
        UnhookWindowsHookEx( getHookHandle );
        isFiltering = false;
    }

} /* ClearFilter */
