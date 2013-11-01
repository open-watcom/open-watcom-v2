/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of AfxWndProc.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

LRESULT CALLBACK AfxWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
/**********************************************************************************/
{
    CWnd *pWnd = CWnd::FromHandlePermanent( hWnd );
    if( pWnd == NULL ) {
        return( ::DefWindowProc( hWnd, message, wParam, lParam ) );
    }

    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    pState->m_msgCur.hwnd = hWnd;
    pState->m_msgCur.message = message;
    pState->m_msgCur.wParam = wParam;
    pState->m_msgCur.lParam = lParam;
    pState->m_msgCur.time = ::GetMessageTime();
    DWORD dwPos = ::GetMessagePos();
    pState->m_msgCur.pt.x = LOWORD( dwPos );
    pState->m_msgCur.pt.y = HIWORD( dwPos );

    try {
        return( pWnd->WindowProc( message, wParam, lParam ) );
    } catch( CException *pEx ) {
        CWinThread *pThread = AfxGetThread();
        ASSERT( pThread != NULL );
        return( pThread->ProcessWndProcException( pEx, &pState->m_msgCur ) );
    }
}
