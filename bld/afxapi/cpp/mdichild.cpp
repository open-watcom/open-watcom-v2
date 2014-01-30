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
* Description:  Implementation of CMDIChildWnd.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

// Define _sntprintf if it's not in tchar.h.
#ifndef _sntprintf
    #ifdef _UNICODE
        #define _sntprintf  _snwprintf
    #else
        #define _sntprintf  snprintf
    #endif
#endif

IMPLEMENT_DYNCREATE( CMDIChildWnd, CFrameWnd )

BEGIN_MESSAGE_MAP( CMDIChildWnd, CFrameWnd )
    ON_WM_CREATE()
    ON_WM_MDIACTIVATE()
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

CMDIChildWnd::CMDIChildWnd()
/**************************/
{
    m_hMenuShared = NULL;
}

BOOL CMDIChildWnd::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                           const RECT &rect, CMDIFrameWnd *pParentWnd,
                           CCreateContext *pContext )
/***************************************************/
{
    if( pParentWnd == NULL ) {
        pParentWnd = (CMDIFrameWnd *)AfxGetMainWnd();
    }
    ASSERT( pParentWnd != NULL );
    ASSERT( pParentWnd->IsKindOf( RUNTIME_CLASS( CMDIFrameWnd ) ) );
    
    CREATESTRUCT cs;
    cs.dwExStyle = 0L;
    cs.lpszClass = lpszClassName;
    cs.lpszName = lpszWindowName;
    cs.style = dwStyle;
    cs.x = rect.left;
    cs.y = rect.top;
    cs.cx = rect.right - rect.left;
    cs.cy = rect.bottom - rect.top;
    cs.hwndParent = pParentWnd->m_hWnd;
    cs.hMenu = NULL;
    cs.hInstance = AfxGetInstanceHandle();
    cs.lpCreateParams = pContext;
    if( !PreCreateWindow( cs ) ) {
        PostNcDestroy();
        return( FALSE );
    }

    MDICREATESTRUCT mcs;
    mcs.szClass = cs.lpszClass;
    mcs.szTitle = cs.lpszName;
    mcs.hOwner = cs.hInstance;
    mcs.x = cs.x;
    mcs.y = cs.y;
    mcs.cx = cs.cx;
    mcs.cy = cs.cy;
    mcs.style = cs.style;
    mcs.lParam = (LPARAM)cs.lpCreateParams;
    AfxHookWindowCreate( this );
    HWND hWnd = (HWND)::SendMessage( pParentWnd->m_hWndMDIClient, WM_MDICREATE, 0,
                                     (LPARAM)&mcs );
    if( !AfxUnhookWindowCreate() ) {
        PostNcDestroy();
        return( FALSE );
    }
    if( hWnd == NULL ) {
        PostNcDestroy();
        return( FALSE );
    }
    ASSERT( m_hWnd == hWnd );

    return( TRUE );
}

LRESULT CMDIChildWnd::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
/*******************************************************************************/
{
    return( ::DefMDIChildProc( m_hWnd, message, wParam, lParam ) );
}

BOOL CMDIChildWnd::DestroyWindow()
/********************************/
{
    if( m_hWnd == NULL ) {
        return( FALSE );
    }
    MDIDestroy();
    return( TRUE );
}

CWnd *CMDIChildWnd::GetMessageBar()
/*********************************/
{
    CMDIFrameWnd *pFrame = GetMDIFrame();
    ASSERT( pFrame != NULL );
    return( pFrame->GetMessageBar() );
}

BOOL CMDIChildWnd::LoadFrame( UINT nIDResource, DWORD dwDefaultStyle, CWnd *pParentWnd,
                              CCreateContext *pContext )
/******************************************************/
{
    if( pContext != NULL ) {
        CMultiDocTemplate *pTemplate = (CMultiDocTemplate *)pContext->m_pNewDocTemplate;
        if( pTemplate != NULL ) {
            ASSERT( pTemplate->IsKindOf( RUNTIME_CLASS( CMultiDocTemplate ) ) );
            m_hMenuShared = pTemplate->m_hMenuShared;
            m_hAccelTable = pTemplate->m_hAccelTable;
        }
    }

    HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nIDResource ), RT_ICON );
    HICON hIcon = ::LoadIcon( hInstance, MAKEINTRESOURCE( nIDResource ) );
    LPCTSTR lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, NULL,
                                             (HBRUSH)(COLOR_WINDOW + 1), hIcon );
    m_strTitle.LoadString( nIDResource );
    LoadAccelTable( MAKEINTRESOURCE( nIDResource ) );
    return( Create( lpszClass, m_strTitle, dwDefaultStyle, rectDefault,
                    (CMDIFrameWnd *)pParentWnd, pContext ) );
}

void CMDIChildWnd::OnUpdateFrameMenu( BOOL bActive, CWnd *pActivateWnd, HMENU hMenuAlt )
/**************************************************************************************/
{
    CMDIFrameWnd *pFrame = GetMDIFrame();
    ASSERT( pFrame != NULL );

    if( hMenuAlt == NULL ) {
        hMenuAlt = m_hMenuShared;
    }
    
    if( bActive ) {
        ::SendMessage( pFrame->m_hWndMDIClient, WM_MDISETMENU, (WPARAM)hMenuAlt,
                       (LPARAM)pFrame->GetWindowMenuPopup( hMenuAlt ) );
    } else if( pActivateWnd == NULL ) {
        ::SendMessage( pFrame->m_hWndMDIClient, WM_MDISETMENU,
                       (WPARAM)pFrame->m_hMenuDefault, NULL );
    }
}

void CMDIChildWnd::OnUpdateFrameTitle( BOOL bAddToTitle )
/*******************************************************/
{
    UNUSED_ALWAYS( bAddToTitle );

    CDocument *pDoc = GetActiveDocument();
    if( pDoc != NULL ) {
        if( m_nWindow == 0 ) {
            ::SetWindowText( m_hWnd, pDoc->GetTitle() );
        } else {
            TCHAR szTitleBarText[256];
            _sntprintf( szTitleBarText, 255, _T("%s : %d"),
                        (LPCTSTR)pDoc->GetTitle(), m_nWindow );
            ::SetWindowText( m_hWnd, szTitleBarText );
        }
    } else {
        ::SetWindowText( m_hWnd, NULL );
    }
}

#ifdef _DEBUG

void CMDIChildWnd::AssertValid() const
/************************************/
{
    CFrameWnd::AssertValid();

    ASSERT( m_hMenuShared == NULL || ::IsMenu( m_hMenuShared ) );
}

void CMDIChildWnd::Dump( CDumpContext &dc ) const
/***********************************************/
{
    CFrameWnd::Dump( dc );

    dc << "m_hMenuShared = " << m_hMenuShared << "\n";
}

#endif // _DEBUG

CMDIFrameWnd *CMDIChildWnd::GetMDIFrame()
/***************************************/
{
    HWND hWndMDIClient = ::GetParent( m_hWnd );
    ASSERT( hWndMDIClient != NULL );
    HWND hWnd = ::GetParent( hWndMDIClient );
    ASSERT( hWnd != NULL );
    CMDIFrameWnd *pFrame = (CMDIFrameWnd *)CWnd::FromHandle( hWnd );
    ASSERT( pFrame->IsKindOf( RUNTIME_CLASS( CMDIFrameWnd ) ) );
    return( pFrame );
}

void CMDIChildWnd::SetHandles( HMENU hMenu, HACCEL hAccel )
/*********************************************************/
{
    m_hMenuShared = hMenu;
    m_hAccelTable = hAccel;
}

int CMDIChildWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
/*********************************************************/
{
    // Don't call CFrameWnd::OnCreate!  It doesn't handle the MDICREATESTRUCT passed
    // to MDI child windows.
    if( CWnd::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }

    MDICREATESTRUCT *lpmcs = (MDICREATESTRUCT *)lpCreateStruct->lpCreateParams;
    ASSERT( lpmcs != NULL );
    if( !OnCreateClient( lpCreateStruct, (CCreateContext *)lpmcs->lParam ) ) {
        return( -1 );
    }
    RecalcLayout();
    return( 0 );
}

void CMDIChildWnd::OnMDIActivate( BOOL bActivate, CWnd *pActivateWnd,
                                  CWnd *pDeactivateWnd )
/******************************************************/
{
    UNUSED_ALWAYS( pDeactivateWnd );

    CView *pView = GetActiveView();
    if( pView != NULL ) {
        if( !bActivate ) {
            pView->OnActivateView( FALSE, pView, pView );
        } else {
            pView->OnActivateView( TRUE, pView, pView );
        }
    }

    OnUpdateFrameMenu( bActivate, pActivateWnd, NULL );
    CMDIFrameWnd *pFrame = GetMDIFrame();
    ASSERT( pFrame != NULL );
    pFrame->DrawMenuBar();
}
