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
* Description:  Implementation of CWinThread, AfxBeginThread, and
*               AfxEndThread.
*
****************************************************************************/


#include "stdafx.h"
#include "threadit.h"

// Thread message handler function pointer
typedef void (CWinThread::*PHANDLER_THREAD)( WPARAM, LPARAM );

DWORD WINAPI _WinThreadProc( LPVOID lpParameter )
/***********************************************/
{
    CWinThread *pThread = (CWinThread *)lpParameter;
    ASSERT( pThread != NULL );
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pCurrentWinThread == NULL );
    pState->m_pCurrentWinThread = pThread;
    AfxInitThread();
    int nExitCode;
    if( pThread->m_pfnThreadProc != NULL ) {
        nExitCode = pThread->m_pfnThreadProc( pThread->m_pThreadParams );
    } else {
        if( pThread->InitInstance() ) {
            nExitCode = pThread->Run();
        } else {
            nExitCode = -1;
        }
    }
    if( pThread->m_bAutoDelete ) {
        delete pThread;
    }
    AfxTermThread( NULL );
    return( nExitCode );
}

void CWinThread::DispatchThreadMessage( MSG *pMsg )
/*************************************************/
{
    const AFX_MSGMAP *pMessageMap = GetMessageMap();
    for( ;; ) {
        const AFX_MSGMAP_ENTRY *pEntries = pMessageMap->lpEntries;
        int i = 0;
        while( pEntries[i].nSig != AfxSig_end ) {
            if( pEntries[i].nMessage == 0xC000 ) {
                if( *(UINT *)pEntries[i].nSig == pMsg->message ) {
                    (this->*(PHANDLER_THREAD)(AFX_PMSGT)pEntries[i].pfn)(
                        pMsg->wParam, pMsg->lParam );
                    return;
                }
            } else if( pEntries[i].nMessage == pMsg->message ) {
                if( pEntries[i].nSig == AfxSig_v_w_l ) {
                    (this->*(PHANDLER_THREAD)(AFX_PMSGT)pEntries[i].pfn)(
                        pMsg->wParam, pMsg->lParam );
                }
                return;
            }
            i++;
        }
        if( pMessageMap->pfnGetBaseMap == NULL ) {
            break;
        }
        pMessageMap = pMessageMap->pfnGetBaseMap();
    }
}

IMPLEMENT_DYNAMIC( CWinThread, CCmdTarget )

CWinThread::CWinThread()
/**********************/
{
    m_pMainWnd = NULL;
    m_pActiveWnd = NULL;
    m_bAutoDelete = TRUE;
    m_hThread = NULL;
    m_nThreadID = 0L;
    m_pThreadParams = NULL;
    m_pfnThreadProc = NULL;
}

CWinThread::~CWinThread()
/***********************/
{
    if( m_hThread != NULL ) {
        ::CloseHandle( m_hThread );
    }
}

int CWinThread::ExitInstance()
/****************************/
{
    return( 0 );
}

CWnd *CWinThread::GetMainWnd()
/****************************/
{
    return( m_pMainWnd );
}

BOOL CWinThread::InitInstance()
/*****************************/
{
    return( TRUE );
}

BOOL CWinThread::IsIdleMessage( MSG *pMsg )
/*****************************************/
{
    if( pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE ) {
        return( TRUE );
    }
    return( FALSE );
}

BOOL CWinThread::OnIdle( LONG lCount )
/************************************/
{
    UNUSED_ALWAYS( lCount );
    
    CWnd::DeleteTempMap();
    CMenu::DeleteTempMap();
    CDC::DeleteTempMap();
    CGdiObject::DeleteTempMap();
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->SendMessageToDescendants( WM_IDLEUPDATECMDUI, TRUE, 0L, TRUE, TRUE );
    }
    return( FALSE );
}

BOOL CWinThread::PreTranslateMessage( MSG *pMsg )
/***********************************************/
{
    HWND hMainWnd = m_pMainWnd->GetSafeHwnd();
    HWND hWnd = pMsg->hwnd;
    if( hWnd == NULL ) {
        DispatchThreadMessage( pMsg );
        return( TRUE );
    }
    while( hWnd != NULL && hWnd != hMainWnd ) {
        CWnd *pWnd = CWnd::FromHandlePermanent( hWnd );
        if( pWnd != NULL && pWnd->PreTranslateMessage( pMsg ) ) {
            return( TRUE );
        }
        hWnd = ::GetParent( hWnd );
    }
    if( m_pMainWnd != NULL ) {
        return( m_pMainWnd->PreTranslateMessage( pMsg ) );
    }
    return( FALSE );
}

BOOL CWinThread::ProcessMessageFilter( int code, LPMSG lpMsg )
/************************************************************/
{
    UNUSED_ALWAYS( code );
    UNUSED_ALWAYS( lpMsg );
    return( FALSE );
}

LRESULT CWinThread::ProcessWndProcException( CException *e, const MSG *pMsg )
/***************************************************************************/
{
    UNUSED_ALWAYS( e );
    
    if( pMsg->message == WM_CREATE ) {
        return( -1 );
    } else if( pMsg->message == WM_PAINT ) {
        ::ValidateRect( pMsg->hwnd, NULL );
    }
    return( 0 );
}

BOOL CWinThread::PumpMessage()
/****************************/
{
    MSG msg;
    if( !::GetMessage( &msg, NULL, 0, 0 ) ) {
        return( FALSE );
    }
    if( !PreTranslateMessage( &msg ) ) {
        ::TranslateMessage( &msg );
        ::DispatchMessage( &msg );
    }
    return( TRUE );
}

int CWinThread::Run()
/*******************/
{
    MSG     msg;
    LONG    lCount = 0L;
    for( ;; ) {
        while( !::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
            if( OnIdle( lCount ) ) {
                lCount++;
            } else {
                ::WaitMessage();
            }
        }
        if( !PumpMessage() ) {
            break;
        }
        if( IsIdleMessage( &msg ) ) {
            if( OnIdle( lCount ) ) {
                lCount++;
            } else {
                ::WaitMessage();
            }
        } else {
            lCount = 0L;
        }
    }
    return( ExitInstance() );
}

#ifdef _DEBUG

void CWinThread::Dump( CDumpContext &dc ) const
/*********************************************/
{
    CCmdTarget::Dump( dc );

    dc << "m_pMainWnd = " << (void *)m_pMainWnd << "\n";
    dc << "m_pActiveWnd = " << (void *)m_pActiveWnd << "\n";
    dc << "m_bAutoDelete = " << m_bAutoDelete << "\n";
    dc << "m_hThread = " << m_hThread << "\n";
    dc << "m_nThreadID = " << m_nThreadID << "\n";
    dc << "m_pThreadParams = " << m_pThreadParams << "\n";
    dc << "m_pfnThreadProc = " << m_pfnThreadProc << "\n";
}

#endif // _DEBUG

BOOL CWinThread::CreateThread( DWORD dwCreateFlags, UINT nStackSize,
                               LPSECURITY_ATTRIBUTES lpSecurityAttrs )
/********************************************************************/
{
    if( m_hThread != NULL ) {
        return( FALSE );
    }
    m_hThread = ::CreateThread( lpSecurityAttrs, nStackSize, _WinThreadProc, this,
                                dwCreateFlags, &m_nThreadID );
    return( m_hThread != NULL );
}

CWinThread * AFXAPI AfxBeginThread( AFX_THREADPROC pfnThreadProc, LPVOID pParam,
                                    int nPriority, UINT nStackSize, DWORD dwCreateFlags,
                                    LPSECURITY_ATTRIBUTES lpSecurityAttrs )
/*************************************************************************/
{
    CWinThread *pThread = new CWinThread;
    pThread->m_pfnThreadProc = pfnThreadProc;
    pThread->m_pThreadParams = pParam;
    pThread->m_hThread = ::CreateThread( lpSecurityAttrs, nStackSize, _WinThreadProc,
                                         pThread, dwCreateFlags | CREATE_SUSPENDED,
                                         &pThread->m_nThreadID );
    if( pThread->m_hThread == NULL ) {
        delete pThread;
        return( NULL );
    }
    ::SetThreadPriority( pThread->m_hThread, nPriority );
    if( !(dwCreateFlags & CREATE_SUSPENDED) ) {
        ::ResumeThread( pThread->m_hThread );
    }
    return( pThread );
}

CWinThread * AFXAPI AfxBeginThread( CRuntimeClass *pThreadClass, int nPriority,
                                    UINT nStackSize, DWORD dwCreateFlags,
                                    LPSECURITY_ATTRIBUTES lpSecurityAttrs )
/*************************************************************************/
{
    CWinThread *pThread = (CWinThread *)pThreadClass->CreateObject();
    if( pThread == NULL ) {
        return( NULL );
    }
    pThread->m_hThread = ::CreateThread( lpSecurityAttrs, nStackSize, _WinThreadProc,
                                         pThread, dwCreateFlags | CREATE_SUSPENDED,
                                         &pThread->m_nThreadID );
    if( pThread->m_hThread == NULL ) {
        delete pThread;
        return( NULL );
    }
    ::SetThreadPriority( pThread->m_hThread, nPriority );
    if( !(dwCreateFlags & CREATE_SUSPENDED) ) {
        ::ResumeThread( pThread->m_hThread );
    }
    return( pThread );
}

void AFXAPI AfxEndThread( UINT nExitCode, BOOL bDelete )
/******************************************************/
{
    if( bDelete ) {
        CWinThread *pThread = AfxGetThread();
        if( pThread->m_bAutoDelete ) {
            delete pThread;
        }
    }
    ::ExitThread( nExitCode );
}
