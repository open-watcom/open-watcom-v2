/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of AfxInitThread and AfxTermThread.
*
****************************************************************************/


#include "stdafx.h"
#include "threadit.h"

LRESULT CALLBACK _MsgFilterProc( int code, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
    UNUSED_ALWAYS( wParam );
    
    if( code >= 0 ) {
        CWinThread *pThread = AfxGetThread();
        if( pThread != NULL && pThread->ProcessMessageFilter( code, (LPMSG)lParam ) ) {
            return( 1 );
        }
    }
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_hHookOldMsgFilter != NULL );
    return( ::CallNextHookEx( pState->m_hHookOldMsgFilter, code, wParam, lParam ) );
}

void AFXAPI AfxInitThread()
/*************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_hHookOldMsgFilter == NULL );
    pState->m_hHookOldMsgFilter = ::SetWindowsHookEx( WH_MSGFILTER, _MsgFilterProc,
                                                      AfxGetInstanceHandle(),
                                                      ::GetCurrentThreadId() );
}

void AFXAPI AfxTermThread( HINSTANCE hInstTerm )
/**********************************************/
{
    hInstTerm = hInstTerm;
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ::UnhookWindowsHookEx( pState->m_hHookOldMsgFilter );
}
