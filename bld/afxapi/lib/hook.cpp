/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of AfxHookWindowCreate and
*               AfxUnhookWindowCreate.
*
****************************************************************************/


#include "stdafx.h"

LRESULT CALLBACK _CbtProc( int nCode, WPARAM wParam, LPARAM lParam )
/******************************************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    if( nCode == HCBT_CREATEWND && pState->m_pWndInit != NULL ) {
        if( pState->m_pWndInit->SubclassWindow( (HWND)wParam ) ) {
            pState->m_pWndInit = NULL;
            return( 0 );
        }
        LRESULT lResult = ::CallNextHookEx( pState->m_hHookOldCbtFilter, nCode,
                                            wParam, lParam );
        ::UnhookWindowsHookEx( pState->m_hHookOldCbtFilter );
        pState->m_hHookOldCbtFilter = NULL;
        return( lResult );
    }
    return( ::CallNextHookEx( pState->m_hHookOldCbtFilter, nCode, wParam, lParam ) );
}

void AFXAPI AfxHookWindowCreate( CWnd *pWnd )
/*******************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    pState->m_pWndInit = pWnd;
    pState->m_hHookOldCbtFilter = ::SetWindowsHookEx( WH_CBT, _CbtProc, NULL,
                                                      ::GetCurrentThreadId() );
}

BOOL AFXAPI AfxUnhookWindowCreate()
/*********************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    if( pState->m_hHookOldCbtFilter != NULL ) {
        ::UnhookWindowsHookEx( pState->m_hHookOldCbtFilter );
        pState->m_hHookOldCbtFilter = NULL;
    }
    if( pState->m_pWndInit != NULL ) {
        pState->m_pWndInit = NULL;
        return( FALSE );
    }
    return( TRUE );
}
