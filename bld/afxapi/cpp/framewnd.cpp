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
* Description:  Implementation of CFrameWnd.
*
****************************************************************************/


#include "stdafx.h"
#include <dde.h>

// Define _sntprintf if it's not in tchar.h.
#ifndef _sntprintf
    #ifdef _UNICODE
        #define _sntprintf  _snwprintf
    #else
        #define _sntprintf  snprintf
    #endif
#endif

// Number of dock bars
#define NUM_DOCKBARS    4

// Dock bar identifiers and styles
static const DWORD _DockBarInfo[NUM_DOCKBARS][2] = {
    { AFX_IDW_DOCKBAR_TOP,      CBRS_TOP },
    { AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
    { AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT },
    { AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT }
};

// Handle to cursor for use when in help mode
static HCURSOR _HelpCursor = NULL;

IMPLEMENT_DYNCREATE( CFrameWnd, CWnd )

BEGIN_MESSAGE_MAP( CFrameWnd, CWnd )
    ON_WM_ACTIVATE()
    ON_WM_CLOSE()
    ON_WM_CREATE()
    ON_WM_DROPFILES()
    ON_WM_ENDSESSION()
    ON_WM_INITMENUPOPUP()
    ON_WM_MENUSELECT()
    ON_WM_QUERYENDSESSION()
    ON_WM_SETCURSOR()
    ON_WM_SIZE()
    ON_WM_SYSCOMMAND()
    ON_MESSAGE( WM_DDE_INITIATE, OnDDEInitiate )
    ON_MESSAGE( WM_DDE_EXECUTE, OnDDEExecute )
    ON_MESSAGE( WM_DDE_TERMINATE, OnDDETerminate )
    ON_MESSAGE( WM_COMMANDHELP, OnCommandHelp )
    ON_MESSAGE( WM_HELPHITTEST, OnHelpHitTest )
    ON_MESSAGE( WM_POPMESSAGESTRING, OnPopMessageString )
    ON_MESSAGE( WM_SETMESSAGESTRING, OnSetMessageString )
    ON_COMMAND_EX( ID_VIEW_TOOLBAR, OnBarCheck )
    ON_COMMAND_EX( ID_VIEW_STATUS_BAR, OnBarCheck )
    ON_UPDATE_COMMAND_UI( ID_VIEW_TOOLBAR, OnUpdateControlBarMenu )
    ON_UPDATE_COMMAND_UI( ID_VIEW_STATUS_BAR, OnUpdateControlBarMenu )
    ON_UPDATE_COMMAND_UI( ID_INDICATOR_NUM, OnUpdateKeyIndicator )
    ON_UPDATE_COMMAND_UI( ID_INDICATOR_CAPS, OnUpdateKeyIndicator )
    ON_UPDATE_COMMAND_UI( ID_INDICATOR_SCRL, OnUpdateKeyIndicator )
    ON_NOTIFY_EX_RANGE( TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText )
    ON_NOTIFY_EX_RANGE( TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText )
END_MESSAGE_MAP()

const RECT CFrameWnd::rectDefault = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };

CFrameWnd::CFrameWnd()
/********************/
{
    m_bAutoMenuEnable = TRUE;
    m_nWindow = -1;
    m_hMenuDefault = NULL;
    m_hAccelTable = NULL;
    m_bHelpMode = FALSE;
    m_nIDHelp = 0;
    m_nIDTracking = 0;
    m_nIDLastMessage = 0;
    m_pViewActive = NULL;
    m_lpfnCloseProc = NULL;
    m_hMenuAlt = NULL;
    m_bInRecalcLayout = FALSE;
}

void CFrameWnd::ActivateFrame( int nCmdShow )
/*******************************************/
{
    if( nCmdShow == -1 ) {
        nCmdShow = SW_SHOW;
    }
    ::ShowWindow( m_hWnd, nCmdShow );
    ::BringWindowToTop( m_hWnd );
}

BOOL CFrameWnd::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                        const RECT &rect, CWnd *pParentWnd, LPCTSTR lpszMenuName,
                        DWORD dwExStyle, CCreateContext *pContext )
/*****************************************************************/
{
    if( lpszMenuName != NULL ) {
        HINSTANCE hInstance = AfxFindResourceHandle( lpszMenuName, RT_MENU );
        m_hMenuDefault = ::LoadMenu( hInstance, lpszMenuName );
    } else {
        m_hMenuDefault = NULL;
    }
    return( CreateEx( dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect,
                      pParentWnd, (UINT)m_hMenuDefault, pContext ) );
}

CWnd *CFrameWnd::CreateView( CCreateContext *pContext, UINT nID )
/***************************************************************/
{
    if( pContext == NULL ) {
        return( NULL );
    }

    CRuntimeClass *pClass = pContext->m_pNewViewClass;
    if( pClass == NULL ) {
        return( NULL );
    }

    CWnd *pView = (CWnd *)pClass->CreateObject();
    if( pView == NULL ) {
        return( NULL );
    }
    ASSERT( pView->IsKindOf( RUNTIME_CLASS( CWnd ) ) );

    if( !pView->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect( 0, 0, 0, 0 ),
                        this, nID, pContext ) ) {
        return( NULL );
    }
    return( pView );
}

void CFrameWnd::ExitHelpMode()
/****************************/
{
    if( m_bHelpMode ) {
        ::PostMessage( m_hWnd, WM_EXITHELPMODE, 0, 0L );
    }
}

CDocument *CFrameWnd::GetActiveDocument()
/***************************************/
{
    CFrameWnd *pFrame = GetActiveFrame();
    if( pFrame != NULL ) {
        CView *pView = pFrame->GetActiveView();
        if( pView != NULL ) {
            return( pView->GetDocument() );
        }
    }
    return( NULL );
}

CFrameWnd *CFrameWnd::GetActiveFrame()
/************************************/
{
    return( this );
}

CWnd *CFrameWnd::GetMessageBar()
/******************************/
{
    return( GetControlBar( AFX_IDW_STATUS_BAR ) );
}

void CFrameWnd::GetMessageString( UINT nID, CString &rString ) const
/******************************************************************/
{
    CString strTemp;
    strTemp.LoadString( nID );
    AfxExtractSubString( rString, strTemp, 0 );
}

BOOL CFrameWnd::LoadFrame( UINT nIDResource, DWORD dwDefaultStyle, CWnd *pParentWnd,
                           CCreateContext *pContext )
/***************************************************/
{
    m_nIDHelp = nIDResource;

    CString str;
    str.LoadString( nIDResource );
    AfxExtractSubString( m_strTitle, str, 0 );

    HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nIDResource ), RT_ICON );
    HICON hIcon = ::LoadIcon( hInstance, MAKEINTRESOURCE( nIDResource ) );
    LPCTSTR lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, NULL,
                                             (HBRUSH)(COLOR_WINDOW + 1), hIcon );
    LoadAccelTable( MAKEINTRESOURCE( nIDResource ) );
    return( Create( lpszClass, m_strTitle, dwDefaultStyle, rectDefault, pParentWnd,
                    MAKEINTRESOURCE( nIDResource ), 0L, pContext ) );
}

BOOL CFrameWnd::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext *pContext )
/*****************************************************************************/
{
    UNUSED_ALWAYS( lpcs );
    
    if( pContext != NULL && pContext->m_pNewViewClass != NULL &&
        CreateView( pContext ) == NULL ) {
        return( FALSE );
    }
    return( TRUE );
}

void CFrameWnd::OnSetPreviewMode( BOOL bPreview, CPrintPreviewState *pState )
/***************************************************************************/
{
    ASSERT( pState != NULL );
    
    DWORD       dwSaveStates = 0L;
    POSITION    position = m_listControlBars.GetHeadPosition();
    while( position != NULL ) {
        CControlBar *pBar = (CControlBar *)m_listControlBars.GetNext( position );
        ASSERT( pBar != NULL );
        ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );

        DWORD dwID = pBar->GetDlgCtrlID();
        if( dwID >= AFX_IDW_CONTROLBAR_FIRST && dwID < AFX_IDW_CONTROLBAR_FIRST + 32 ) {
            DWORD dwThisState = AFX_CONTROLBAR_MASK( dwID );
            if( pBar->IsWindowVisible() ) {
                dwSaveStates |= dwThisState;
            }
            if( !(pBar->IsDockBar() && pBar->GetDlgCtrlID() == AFX_IDW_DOCKBAR_FLOAT) ) {
                ShowControlBar( pBar, pState->dwStates & dwThisState, TRUE );
            }
        }
    }
    pState->dwStates = dwSaveStates;
    
    if( bPreview ) {
        m_lpfnCloseProc = pState->lpfnCloseProc;
        
        pState->hMenu = ::GetMenu( m_hWnd );
        ::SetMenu( m_hWnd, NULL );
        pState->hAccelTable = m_hAccelTable;
        m_hAccelTable = NULL;
        LoadAccelTable( MAKEINTRESOURCE( AFX_IDR_PREVIEW_ACCEL ) );

        HWND hWndPane = ::GetDlgItem( m_hWnd, pState->nIDMainPane );
        if( hWndPane != NULL ) {
            ::ShowWindow( hWndPane, SW_HIDE );
        }
        hWndPane = ::GetDlgItem( m_hWnd, AFX_IDW_PANE_FIRST );
        if( hWndPane != NULL ) {
            ::SetWindowLong( hWndPane, GWL_ID, AFX_IDW_PANE_SAVE );
        }
    } else {
        m_lpfnCloseProc = NULL;
        
        if( pState->hMenu != NULL ) {
            ::SetMenu( m_hWnd, pState->hMenu );
            pState->hMenu = NULL;
        }
        if( pState->hAccelTable != NULL ) {
            m_hAccelTable = pState->hAccelTable;
            pState->hAccelTable = NULL;
        }

        HWND hWndPane = ::GetDlgItem( m_hWnd, AFX_IDW_PANE_SAVE );
        if( hWndPane != NULL ) {
            ::SetWindowLong( hWndPane, GWL_ID, AFX_IDW_PANE_FIRST );
        }
        hWndPane = ::GetDlgItem( m_hWnd, pState->nIDMainPane );
        if( hWndPane != NULL ) {
            ::ShowWindow( hWndPane, SW_SHOW );
        }
    }
}

void CFrameWnd::OnUpdateFrameTitle( BOOL bAddToTitle )
/****************************************************/
{
    if( bAddToTitle ) {
        CDocument *pDoc = GetActiveDocument();
        if( pDoc != NULL ) {
            UpdateFrameTitleForDocument( pDoc->GetTitle() );
        } else {
            UpdateFrameTitleForDocument( NULL );
        }
    } else {
        UpdateFrameTitleForDocument( NULL );
    }
}

void CFrameWnd::OnUpdateFrameMenu( HMENU hMenuAlt )
/*************************************************/
{
    m_hMenuAlt = hMenuAlt;
    if( m_hMenuAlt != NULL ) {
        ::SetMenu( m_hWnd, m_hMenuAlt );
    } else {
        ::SetMenu( m_hWnd, m_hMenuDefault );
    }
}

void CFrameWnd::RecalcLayout( BOOL bNotify )
/******************************************/
{
    UNUSED_ALWAYS( bNotify );
    
    if( !m_bInRecalcLayout ) {
        m_bInRecalcLayout = TRUE;
        if( GetStyle() & FWS_SNAPTOBARS ) {
            CRect rect;
            RepositionBars( 0, 0xFFFF, AFX_IDW_PANE_FIRST, reposQuery, &rect, NULL,
                            FALSE );
            CalcWindowRect( &rect );
            ::SetWindowPos( m_hWnd, NULL, 0, 0, rect.Width(), rect.Height(),
                            SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
        }
        RepositionBars( 0, 0xFFFF, AFX_IDW_PANE_FIRST );
        ::UpdateWindow( m_hWnd );
        m_bInRecalcLayout = FALSE;
    }
}

BOOL CFrameWnd::IsFrameWnd() const
/********************************/
{
    return( TRUE );
}

BOOL CFrameWnd::OnCmdMsg( UINT nID, int nCode, void *pExtra,
                          AFX_CMDHANDLERINFO *pHandlerInfo )
/**********************************************************/
{
    CView *pView = GetActiveView();
    if( pView != NULL && pView->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) {
        return( TRUE );
    }

    if( CWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) {
        return( TRUE );
    }

    CWinApp *pApp = AfxGetApp();
    if( pApp != NULL && pApp->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) {
        return( TRUE );
    }
    return( FALSE );
}

BOOL CFrameWnd::OnCommand( WPARAM wParam, LPARAM lParam )
/*******************************************************/
{
    if( HIWORD( wParam ) == 0 || HIWORD( wParam ) == 1 ) {
        CFrameWnd *pTopLevel = GetTopLevelFrame();
        ASSERT( pTopLevel != NULL );
        if( pTopLevel->m_bHelpMode ) {
            if( !::SendMessage( m_hWnd, WM_COMMANDHELP, 0,
                                HID_BASE_COMMAND + LOWORD( wParam ) ) ) {
                ::SendMessage( m_hWnd, WM_COMMAND, 0, ID_DEFAULT_HELP );
            }
            return( TRUE );
        }
    }
    return( CWnd::OnCommand( wParam, lParam ) );
}

void CFrameWnd::PostNcDestroy()
/*****************************/
{
    delete this;
}

BOOL CFrameWnd::PreCreateWindow( CREATESTRUCT &cs )
/*************************************************/
{
    if( cs.lpszClass == NULL ) {
        cs.lpszClass = _T("AfxFrameOrView");
    }
    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    return( TRUE );
}

BOOL CFrameWnd::PreTranslateMessage( MSG *pMsg )
/**********************************************/
{
    if( m_hAccelTable != NULL &&
        ::TranslateAccelerator( m_hWnd, m_hAccelTable, pMsg ) ) {
        return( TRUE );
    }

    return( CWnd::PreTranslateMessage( pMsg ) );
}

#ifdef _DEBUG

void CFrameWnd::AssertValid() const
/*********************************/
{
    CWnd::AssertValid();

    ASSERT( m_hMenuDefault == NULL || ::IsMenu( m_hMenuDefault ) );
    if( m_pViewActive != NULL ) {
        ASSERT_VALID( m_pViewActive );
        ASSERT( m_pViewActive->IsKindOf( RUNTIME_CLASS( CView ) ) );
    }
    ASSERT( m_hMenuAlt == NULL || ::IsMenu( m_hMenuAlt ) );
}

void CFrameWnd::Dump( CDumpContext &dc ) const
/********************************************/
{
    CWnd::Dump( dc );

    dc << "m_bAutoMenuEnable = " << m_bAutoMenuEnable << "\n";
    dc << "m_nWindow = " << m_nWindow << "\n";
    dc << "m_hMenuDefault = " << m_hMenuDefault << "\n";
    dc << "m_hAccelTable = " << m_hAccelTable << "\n";
    dc << "m_bHelpMode = " << m_bHelpMode << "\n";
    dc << "m_nIDHelp = " << m_nIDHelp << "\n";
    dc << "m_nIDTracking = " << m_nIDTracking << "\n";
    dc << "m_nIDLastMessage = " << m_nIDLastMessage << "\n";
    dc << "m_pViewActive = " << (void *)m_pViewActive << "\n";
    dc << "m_lpfnCloseProc = " << m_lpfnCloseProc << "\n";
    dc << "m_hMenuAlt = " << m_hMenuAlt << "\n";
    dc << "m_strTitle = " << m_strTitle << "\n";
    dc << "m_bInRecalcLayout = " << m_bInRecalcLayout << "\n";
}

#endif // _DEBUG

void CFrameWnd::AddControlBar( CControlBar *pBar )
/************************************************/
{
    ASSERT( pBar != NULL );
    ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
    if( m_listControlBars.Find( pBar ) == NULL ) {
        m_listControlBars.AddTail( pBar );
    }
}

DWORD CFrameWnd::CanDock( CRect rect, DWORD dwDockStyle, CDockBar **ppDockBar )
/*****************************************************************************/
{
    POSITION position = m_listControlBars.GetHeadPosition();
    while( position != NULL ) {
        CControlBar *pControlBar = (CControlBar *)m_listControlBars.GetNext( position );
        ASSERT( pControlBar != NULL );
        if( pControlBar->IsDockBar() && (pControlBar->GetBarStyle() & dwDockStyle) ) {
            CDockBar *pDockBar = (CDockBar *)pControlBar;
            ASSERT( pDockBar->IsKindOf( RUNTIME_CLASS( CDockBar ) ) );

            if( !pDockBar->m_bFloating ||
                (pDockBar->GetBarStyle() & CBRS_FLOAT_MULTI) ) {
                CRect rectBar;
                pDockBar->GetWindowRect( &rectBar );
                
                RECT rectJunk;
                if( rectBar.left == rectBar.right ) {
                    rectBar.right++;
                }
                if( rectBar.top == rectBar.bottom ) {
                    rectBar.bottom++;
                }
                if( ::IntersectRect( &rectJunk, &rect, &rectBar ) ) {
                    if( ppDockBar != NULL ) {
                        *ppDockBar = pDockBar;
                    }
                    return( pDockBar->GetBarStyle() & dwDockStyle );
                }
            }
        }
    }
    return( 0L );
}

BOOL CFrameWnd::CanEnterHelpMode()
/********************************/
{
    if( _HelpCursor == NULL ) {
        _HelpCursor = ::LoadCursor( NULL, IDC_HELP );
        if( _HelpCursor == NULL ) {
            HINSTANCE hInstance = AfxFindResourceHandle(
                MAKEINTRESOURCE( AFX_IDC_CONTEXTHELP ), RT_CURSOR );
            _HelpCursor = ::LoadCursor( hInstance, MAKEINTRESOURCE( AFX_IDC_CONTEXTHELP ) );
        }
    }
    return( _HelpCursor != NULL );
}

CMiniDockFrameWnd *CFrameWnd::CreateFloatingFrame( DWORD dwStyle )
/****************************************************************/
{
    CMiniDockFrameWnd *pFrame = new CMiniDockFrameWnd;
    if( !pFrame->Create( this, dwStyle ) ) {
        delete pFrame;
        return( NULL );
    }
    return( pFrame );
}

void CFrameWnd::DockControlBar( CControlBar *pBar, UINT nDockBarID, LPCRECT lpRect )
/**********************************************************************************/
{
    CDockBar *pDockBar = NULL;
    if( nDockBarID != 0 ) {
         pDockBar = (CDockBar *)GetControlBar( nDockBarID );
         if( pDockBar == NULL ) {
             // We're trying to dock to a dock bar that doesn't exist, so just don't do
             // anything.
             return;
         }
         if( !(pBar->m_dwDockStyle & (pDockBar->m_dwStyle & CBRS_ALIGN_ANY)) ) {
             // We don't support docking to a bar of this style, so just don't do
             // anything.
             return;
         }
         ASSERT( pDockBar->IsKindOf( RUNTIME_CLASS( CDockBar ) ) );
         if( lpRect != NULL ) {
            RECT    rectJunk;
            CRect   rectBar;
            pDockBar->GetWindowRect( &rectBar );
            if( rectBar.left == rectBar.right ) {
                rectBar.left--;
                rectBar.right++;
            }
            if( rectBar.top == rectBar.bottom ) {
                rectBar.top--;
                rectBar.bottom++;
            }
            if( !::IntersectRect( &rectJunk, &rectBar, lpRect ) ) {
                // The rectangle and the dock bar identifier don't coincide, so just
                // don't do anything.
                return;
            }
        }
    } else if( lpRect != NULL ){
        CanDock( *lpRect, pBar->m_dwDockStyle, &pDockBar );
    } else {
        CanDock( CRect( 0, 0, 32767, 32767 ), pBar->m_dwDockStyle, &pDockBar );
    }
    if( pDockBar != NULL ) {
        ASSERT( pDockBar->IsKindOf( RUNTIME_CLASS( CDockBar ) ) );
        pDockBar->DockControlBar( pBar, lpRect );
    }
}

void CFrameWnd::EnableDocking( DWORD dwDockStyle )
/************************************************/
{
    for( int i = 0; i < NUM_DOCKBARS; i++ ) {
        DWORD dwBarDockStyle = _DockBarInfo[i][1];
        if( dwDockStyle & dwBarDockStyle ) {
            CDockBar *pDockBar = (CDockBar *)GetControlBar( _DockBarInfo[i][1] );
            if( pDockBar == NULL ) {
                pDockBar = new CDockBar;
                pDockBar->Create( this, WS_CHILD | WS_VISIBLE | _DockBarInfo[i][1],
                                  _DockBarInfo[i][0] );
            }
        }
    }
}

void CFrameWnd::FloatControlBar( CControlBar *pBar, CPoint pt, DWORD dwStyle )
/****************************************************************************/
{
    ASSERT( pBar != NULL );
    if( pBar->IsFloating() && !(pBar->m_pDockBar->GetBarStyle() & CBRS_FLOAT_MULTI) ) {
        CFrameWnd *pFrame = pBar->GetParentFrame();
        pFrame->SetWindowPos( NULL, pt.x, pt.y, 0, 0,
                              SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER );
    } else {
        CMiniDockFrameWnd *pFrame = CreateFloatingFrame( dwStyle );
        ASSERT( pFrame != NULL );
        CDockBar *pDockBar = (CDockBar *)pFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
        ASSERT( pDockBar != NULL );
        ASSERT( pDockBar->IsKindOf( RUNTIME_CLASS( CDockBar ) ) );
        pDockBar->DockControlBar( pBar );
        if( pt.x != CW_USEDEFAULT && pt.y != CW_USEDEFAULT ) {
            pFrame->SetWindowPos( NULL, pt.x, pt.y, 0, 0,
                                  SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER );
        }
        pFrame->ShowWindow( SW_SHOW );
        pFrame->UpdateWindow();
        RecalcLayout();
    }
}

CControlBar *CFrameWnd::GetControlBar( UINT nID )
/***********************************************/
{
    POSITION position = m_listControlBars.GetHeadPosition();
    while( position != NULL ) {
        CControlBar *pBar = (CControlBar *)m_listControlBars.GetNext( position );
        ASSERT( pBar != NULL );
        ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
        if( pBar->GetDlgCtrlID() == nID ) {
            return( pBar );
        }
    }
    return( NULL );
}

void CFrameWnd::InitialUpdateFrame( CDocument *pDoc, BOOL bMakeVisible )
/**********************************************************************/
{
    UNUSED_ALWAYS( pDoc );

    if( GetActiveView() == NULL ) {
        CView *pView = (CView *)GetDescendantWindow( AFX_IDW_PANE_FIRST );
        if( pView != NULL ) {
            if( pView->IsKindOf( RUNTIME_CLASS( CView ) ) ) {
                SetActiveView( pView );
            } else if( pView->IsKindOf( RUNTIME_CLASS( CSplitterWnd ) ) ) {
                pView = (CView *)pView->GetDescendantWindow( AFX_IDW_PANE_FIRST );
                if( pView->IsKindOf( RUNTIME_CLASS( CView ) ) ) {
                    SetActiveView( pView );
                }
            }
        }
    }
    if( pDoc != NULL ) {
        pDoc->UpdateFrameCounts();
    }
    OnUpdateFrameTitle( TRUE );
    SendMessageToDescendants( WM_INITIALUPDATE );
    if( bMakeVisible ) {
        CWinApp *pApp = AfxGetApp();
        ASSERT( pApp != NULL );
        if( pApp->m_pMainWnd == this ) {
            ActivateFrame( pApp->m_nCmdShow );
            pApp->m_nCmdShow = -1;
        } else {
            ActivateFrame();
        }
    }
}

BOOL CFrameWnd::IsTracking() const
/********************************/
{
    return( m_nIDTracking != 0 && m_nIDTracking != AFX_IDS_IDLEMESSAGE &&
            m_nIDTracking != AFX_IDS_HELPMODEMESSAGE );
}

BOOL CFrameWnd::LoadAccelTable( LPCTSTR lpszResourceName )
/********************************************************/
{
    HINSTANCE hInstance = AfxFindResourceHandle( lpszResourceName, RT_ACCELERATOR );
    ASSERT( m_hAccelTable == NULL );
    m_hAccelTable = ::LoadAccelerators( hInstance, lpszResourceName );
    return( m_hAccelTable != NULL );
}

void CFrameWnd::NotifyFloatingWindows( DWORD dwFlags )
/****************************************************/
{
    CWnd *pTopLevel = GetTopLevelParent();
    if( pTopLevel != this ) {
        if( dwFlags & FS_ACTIVATE ) {
            ASSERT( !(dwFlags & FS_DEACTIVATE) );
            pTopLevel->SendMessage( WM_NCACTIVATE, TRUE );
        }
        if( dwFlags & FS_DEACTIVATE ) {
            ASSERT( !(dwFlags & FS_ACTIVATE) );
            pTopLevel->SendMessage( WM_NCACTIVATE, FALSE );
        }
    }
    
    HWND hWndDesktop = ::GetDesktopWindow();
    HWND hWndChild = ::GetWindow( hWndDesktop, GW_CHILD );
    while( hWndChild != NULL ) {
        HWND hWndOwner = ::GetWindow( hWndChild, GW_OWNER );
        while( hWndOwner != NULL ) {
            if( hWndOwner == m_hWnd ) {
                ::SendMessage( hWndChild, WM_FLOATSTATUS, dwFlags, 0L );
                break;
            }
            hWndOwner = ::GetWindow( hWndOwner, GW_OWNER );
        }
        hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT );
    }
}

void CFrameWnd::RemoveControlBar( CControlBar *pBar )
/***************************************************/
{
    ASSERT( pBar != NULL );
    ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
    POSITION position = m_listControlBars.Find( pBar );
    if( position != NULL ) {
        m_listControlBars.RemoveAt( position );
    }
}

void CFrameWnd::SetActiveView( CView *pViewNew, BOOL bNotify )
/************************************************************/
{
    if( m_pViewActive != pViewNew ) {
        CView *pViewOld = m_pViewActive;
        if( pViewOld != NULL ) {
            pViewOld->OnActivateView( FALSE, pViewNew, pViewOld );
        }
        m_pViewActive = pViewNew;
        if( bNotify && pViewNew != NULL ) {
            pViewNew->OnActivateView( TRUE, pViewNew, pViewOld );
        }
    }
}

void CFrameWnd::SetMessageText( LPCTSTR lpszText )
/************************************************/
{
    CWnd *pWnd = GetMessageBar();
    if( pWnd != NULL ) {
        pWnd->SetWindowText( lpszText );
    }
}

void CFrameWnd::SetMessageText( UINT nID )
/****************************************/
{
    CString str;
    GetMessageString( nID, str );
    SetMessageText( str );
}

void CFrameWnd::SetTitle( LPCTSTR lpszTitle )
/*******************************************/
{
    m_strTitle = lpszTitle;
    OnUpdateFrameTitle( TRUE );
}

void CFrameWnd::ShowControlBar( CControlBar *pBar, BOOL bShow, BOOL bDelay )
/**************************************************************************/
{
    UNUSED_ALWAYS( bDelay );
    
    ASSERT( pBar != NULL );
    if( bShow ) {
        pBar->ShowWindow( SW_SHOW );
    } else {
        pBar->ShowWindow( SW_HIDE );
    }
    if( pBar->IsFloating() ) {
        CDockBar *pDockBar = pBar->m_pDockBar;
        ASSERT( pDockBar != NULL );
        CFrameWnd *pFrame = pDockBar->GetParentFrame();
        ASSERT( pFrame != NULL );
        if( !bShow && pDockBar->GetDockedVisibleCount() == 0 ) {
            pFrame->ShowWindow( SW_HIDE );
        } else if( bShow && pDockBar->GetDockedVisibleCount() == 1 ) {
            pFrame->ShowWindow( SW_SHOW );
            pFrame->UpdateWindow();
        }
    }
    RecalcLayout();
}

void CFrameWnd::UpdateFrameTitleForDocument( LPCTSTR lpszDocName )
/****************************************************************/
{
    if( lpszDocName == NULL ) {
        ::SetWindowText( m_hWnd, m_strTitle );
    } else {
        TCHAR szTitleBarText[256];
        _sntprintf( szTitleBarText, 255, _T("%s - %s"),
                    (LPCTSTR)m_strTitle, lpszDocName );
        ::SetWindowText( m_hWnd, szTitleBarText );
    }
}

void CFrameWnd::OnActivate( UINT nState, CWnd *pWndOther, BOOL bMinimized )
/*************************************************************************/
{
    CWnd::OnActivate( nState, pWndOther, bMinimized );

    CFrameWnd *pTopLevel = GetTopLevelFrame();
    if( nState == WA_INACTIVE && pWndOther != pTopLevel &&
        (pWndOther == NULL || pWndOther->GetTopLevelFrame() != pTopLevel) ) {
        NotifyFloatingWindows( FS_DEACTIVATE );
    } else {
        NotifyFloatingWindows( FS_ACTIVATE );
    }
    
    CView *pView = GetActiveView();
    if( pView != NULL ) {
        pView->OnActivateFrame( nState, this );
    }
}

void CFrameWnd::OnClose()
/***********************/
{
    if( m_lpfnCloseProc != NULL ) {
        m_lpfnCloseProc( this );
    } else {
        CWinApp *pApp = AfxGetApp();
        if( pApp != NULL && pApp->m_pMainWnd == this ) {
            if( !pApp->SaveAllModified() ) {
                return;
            }
            pApp->CloseAllDocuments( FALSE );
            if( pApp->m_pMainWnd == NULL ) {
                // If the main window has been destroyed by CloseAllDocuments()
                // (e.g. in an SDI application), just return since trying to execute the
                // rest of OnClose() will cause a crash due to an invalid this pointer.
                return;
            }
        }
        
        CDocument *pDoc = GetActiveDocument();
        if( pDoc != NULL ) {
            if( !pDoc->CanCloseFrame( this ) ) {
                return;
            }
            
            BOOL        bOnlyFrame = TRUE;
            POSITION    position = pDoc->GetFirstViewPosition();
            while( position != NULL ) {
                CView *pView = pDoc->GetNextView( position );
                ASSERT( pView != NULL );
                if( pView->GetParentFrame() != this ) {
                    bOnlyFrame = FALSE;
                    break;
                }
            }
            if( bOnlyFrame ) {
                // OnCloseDocument destroys the frame, so just return after calling it.
                pDoc->OnCloseDocument();
                return;
            }
            pDoc->PreCloseFrame( this );
        }
        DestroyWindow();
    }
}

int CFrameWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
/******************************************************/
{
    if( CWnd::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }
    if( !OnCreateClient( lpCreateStruct,
                         (CCreateContext *)lpCreateStruct->lpCreateParams ) ) {
        return( -1 );
    }
    RecalcLayout();

    // Post (don't send) the message to set the status bar text so that it will be
    // processed after creation is complete.
    ::PostMessage( m_hWnd, WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0L );
    
    return( 0 );
}

void CFrameWnd::OnDropFiles( HDROP hDropInfo )
/********************************************/
{
    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );

    ::SetActiveWindow( m_hWnd );
    UINT nCount = ::DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );
    for( int i = 0; i < nCount; i++ ) {
        TCHAR szFileName[MAX_PATH];
        ::DragQueryFile( hDropInfo, i, szFileName, MAX_PATH );
        pApp->OpenDocumentFile( szFileName );
    }
    ::DragFinish( hDropInfo );
}

void CFrameWnd::OnEndSession( BOOL bEnding )
/******************************************/
{
    if( bEnding ) {
        CWinApp *pApp = AfxGetApp();
        if( pApp != NULL && pApp->m_pMainWnd == this ) {
            pApp->CloseAllDocuments( TRUE );
            pApp->ExitInstance();
        }
    }
}

void CFrameWnd::OnInitMenuPopup( CMenu *pPopupMenu, UINT nIndex, BOOL bSysMenu )
/******************************************************************************/
{
    CWnd::OnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );

    AFX_CMDHANDLERINFO  chi;
    UINT                nCount;
    UINT                nID;
    CCmdUI              cui;

    cui.m_pMenu = pPopupMenu;
    if( !bSysMenu ) {
        ASSERT( pPopupMenu != NULL );
        nCount = pPopupMenu->GetMenuItemCount();
        for( int i = 0; i < nCount; i++ ) {
            nID = pPopupMenu->GetMenuItemID( i );
            if( nID != -1 ) {
                cui.m_nIndex = i;
                cui.m_nID = nID;
                cui.m_pSubMenu = NULL;
                cui.m_bContinueRouting = FALSE;
                if( !OnCmdMsg( nID, CN_UPDATE_COMMAND_UI, &cui, NULL ) ) {
                    if( m_bAutoMenuEnable && nID < 0xF000 &&
                        !OnCmdMsg( nID, CN_COMMAND, NULL, &chi ) ) {
                        pPopupMenu->EnableMenuItem( nID, MF_BYCOMMAND | MF_GRAYED );
                    }
                }
            }
        }
    }
}

void CFrameWnd::OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu )
/***********************************************************************/
{
    CWnd::OnMenuSelect( nItemID, nFlags, hSysMenu );

    if( nFlags == 0xFFFF && hSysMenu == NULL ) {
        CFrameWnd *pFrame = GetTopLevelFrame();
        ASSERT( pFrame != NULL );
        if( pFrame->m_bHelpMode ) {
            SetMessageText( AFX_IDS_HELPMODEMESSAGE );
        } else {
            SetMessageText( AFX_IDS_IDLEMESSAGE );
        }
    } else {
        if( nFlags & MF_POPUP ) {
            SetMessageText( _T("") );
        } else if( nItemID >= AFX_IDM_FIRST_MDICHILD ) {
            SetMessageText( AFX_IDS_MDICHILD );
        } else if( nItemID >= 0xF000 && nItemID < 0xF1F0) {
            SetMessageText( ID_COMMAND_FROM_SC( nItemID ) );
        } else {
            SetMessageText( nItemID );
        }
    }
}

BOOL CFrameWnd::OnQueryEndSession()
/*********************************/
{
    CWinApp *pApp = AfxGetApp();
    if( pApp == NULL || pApp->m_pMainWnd != this ) {
        return( TRUE );
    }
    return( pApp->SaveAllModified() );
}

BOOL CFrameWnd::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT message )
/********************************************************************/
{
    CFrameWnd *pTopLevel = GetTopLevelFrame();
    ASSERT( pTopLevel != NULL );
    if( pTopLevel->m_bHelpMode ) {
        ::SetCursor( _HelpCursor );
        return( TRUE );
    } else {
        return( CWnd::OnSetCursor( pWnd, nHitTest, message ) );
    }
}

void CFrameWnd::OnSize( UINT nType, int cx, int cy )
/**************************************************/
{
    CWnd::OnSize( nType, cx, cy );
    RecalcLayout();
}

void CFrameWnd::OnSysCommand( UINT nID, LPARAM lParam )
/*****************************************************/
{
    CFrameWnd *pFrame = GetTopLevelFrame();
    ASSERT( pFrame != NULL );
    if( pFrame->m_bHelpMode &&
        (nID == SC_SIZE || nID == SC_MOVE || nID == SC_MINIMIZE || nID == SC_MAXIMIZE ||
         nID == SC_NEXTWINDOW || nID == SC_PREVWINDOW || nID == SC_CLOSE ||
         nID == SC_RESTORE || nID == SC_TASKLIST) ) {
        if( !::SendMessage( m_hWnd, WM_COMMANDHELP, 0,
                            HID_BASE_COMMAND + ID_COMMAND_FROM_SC( nID ) ) ) {
            ::SendMessage( m_hWnd, WM_COMMAND, ID_DEFAULT_HELP, 0L );
        }
    } else {
        CWnd::OnSysCommand( nID, lParam );
    }
}

LRESULT CFrameWnd::OnDDEInitiate( WPARAM wParam, LPARAM lParam )
/**************************************************************/
{
    HWND    hWndClient = (HWND)wParam;
    ATOM    atomApp = (ATOM)LOWORD( lParam );
    ATOM    atomTopic = (ATOM)HIWORD( lParam );

    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );

    if( pApp->m_atomApp != 0 && pApp->m_atomSystemTopic != 0 &&
        atomApp == pApp->m_atomApp && atomTopic == pApp->m_atomSystemTopic ) {
        ::SendMessage( hWndClient, WM_DDE_ACK, (WPARAM)m_hWnd, lParam );
    }
        
    return( 0L );
}

LRESULT CFrameWnd::OnDDETerminate( WPARAM wParam, LPARAM lParam )
/***************************************************************/
{
    UNUSED_ALWAYS( lParam );

    HWND hWndClient = (HWND)wParam;
    ::PostMessage( hWndClient, WM_DDE_TERMINATE, (WPARAM)m_hWnd, 0L );
    return( 0L );
}

LRESULT CFrameWnd::OnDDEExecute( WPARAM wParam, LPARAM lParam )
/*************************************************************/
{
    HWND    hWndClient = (HWND)wParam;
    HGLOBAL hMem = (HGLOBAL)lParam;
    ::PostMessage( hWndClient, WM_DDE_ACK, (WPARAM)m_hWnd, MAKELPARAM( 0x8000, hMem ) );

    CString str;
    LPVOID lpMem = ::GlobalLock( hMem );
#ifdef _UNICODE
    if( !::IsWindowUnicode( hWndClient ) ) {
        str = (LPCSTR)lpMem;
    } else {
#endif
        str = (LPCTSTR)lpMem;
#ifdef _UNICODE
    }
#endif
    ::GlobalUnlock( hMem );

    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );
    
    LPTSTR lpszCommand = str.GetBuffer();
    pApp->OnDDECommand( lpszCommand );
    str.ReleaseBuffer();
    
    return( 0L );
}

LRESULT CFrameWnd::OnCommandHelp( WPARAM wParam, LPARAM lParam )
/**************************************************************/
{
    UNUSED_ALWAYS( wParam );
    
    if( lParam == 0L ) {
        if( IsTracking() ) {
            lParam = HID_BASE_COMMAND + m_nIDTracking;
        } else if( m_nIDHelp != 0 ) {
            lParam = HID_BASE_RESOURCE + m_nIDHelp;
        } else {
            return( FALSE );
        }
    }
    
    CWinApp *pApp = AfxGetApp();
    if( pApp != NULL ) {
        pApp->WinHelpInternal( lParam );
    }
    return( TRUE );
}

LRESULT CFrameWnd::OnHelpHitTest( WPARAM wParam, LPARAM lParam )
/**************************************************************/
{
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( lParam );

    if( m_nIDHelp == 0 ) {
        return( 0L );
    }
    return( HID_BASE_RESOURCE + m_nIDHelp );
}

LRESULT CFrameWnd::OnPopMessageString( WPARAM wParam, LPARAM lParam )
/*******************************************************************/
{
    return( ::SendMessage( m_hWnd, WM_SETMESSAGESTRING, wParam, lParam ) );
}

LRESULT CFrameWnd::OnSetMessageString( WPARAM wParam, LPARAM lParam )
/*******************************************************************/
{
    UINT nIDOld = m_nIDLastMessage;
    if( wParam != 0 ) {
        SetMessageText( (UINT)wParam );
    } else {
        SetMessageText( (LPCTSTR)lParam );
    }
    m_nIDTracking = (UINT)wParam;
    m_nIDLastMessage = (UINT)wParam;
    return( nIDOld );
}

void CFrameWnd::OnUpdateKeyIndicator( CCmdUI *pCmdUI )
/****************************************************/
{
    SHORT   wKeyState = 0x0000;
    if( pCmdUI->m_nID == ID_INDICATOR_NUM ) {
        wKeyState = ::GetKeyState( VK_NUMLOCK );
    } else if( pCmdUI->m_nID == ID_INDICATOR_CAPS ) {
        wKeyState = ::GetKeyState( VK_CAPITAL );
    } else if( pCmdUI->m_nID == ID_INDICATOR_SCRL ) {
        wKeyState = ::GetKeyState( VK_SCROLL );
    }
    pCmdUI->Enable( wKeyState & 0x0001 );
}

BOOL CFrameWnd::OnToolTipText( UINT nID, NMHDR *pNMHDR, LRESULT *pResult )
/************************************************************************/
{
    UNUSED_ALWAYS( nID );
    UNUSED_ALWAYS( pResult );

    int nToolID;
    if( ((NMTTDISPINFO *)pNMHDR)->uFlags & TTF_IDISHWND ) {
        nToolID = ::GetDlgCtrlID( pNMHDR->hwndFrom );
    } else {
        nToolID = pNMHDR->idFrom;
    }

    CString strPrompt;
    CString strTip;
    strPrompt.LoadString( nToolID );
    AfxExtractSubString( strTip, strPrompt, 1 );
    ASSERT( strTip.GetLength() < 80 );
    
    // On Windows XP and later, Windows sends TTN_NEEDTEXTW when a common control
    // manifest is used, even if the application is using ANSI.  Therefore, it is
    // necessary to handle both TTN_NEEDTEXTA and TTN_NEEDTEXTW in the ANSI version.
#ifdef _UNICODE
    NMTTDISPINFOW *pInfo = (NMTTDISPINFOW *)pNMHDR;
    wcscpy( pInfo->szText, strTip );
#else
    if( pNMHDR->code == TTN_NEEDTEXTA ) {
        NMTTDISPINFOA *pInfo = (NMTTDISPINFOA *)pNMHDR;
        strcpy( pInfo->szText, strTip );
    } else {
        NMTTDISPINFOW *pInfo = (NMTTDISPINFOW *)pNMHDR;
        ::MultiByteToWideChar( CP_ACP, 0L, strTip, -1, pInfo->szText, 80 );
    }
#endif
    
    return( TRUE );
}
BOOL CFrameWnd::OnBarCheck( UINT nID )
/************************************/
{
    CControlBar *pBar = GetControlBar( nID );
    if( pBar == NULL ) {
        return( FALSE );
    }
    ShowControlBar( pBar, !pBar->IsWindowVisible(), FALSE );
    return( TRUE );
}

void CFrameWnd::OnContextHelp()
/*****************************/
{
    if( !m_bHelpMode && CanEnterHelpMode() ) {
        m_bHelpMode = TRUE;
        ::SetCursor( _HelpCursor );
        
        UINT nIDOld = (UINT)::SendMessage( m_hWnd, WM_SETMESSAGESTRING,
                                           AFX_IDS_HELPMODEMESSAGE, 0L );
        if( nIDOld == 0 ) {
            nIDOld = AFX_IDS_IDLEMESSAGE;
        }
        
        CWinApp *pApp = AfxGetApp();
        ASSERT( pApp != NULL );
        
        MSG         msg;
        LONG        lCount = 0L;
        DWORD_PTR   dwHelpId = 0L;
        for( ;; ) {
            while( !::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
                if( pApp->OnIdle( lCount ) ) {
                    lCount++;
                } else {
                    ::WaitMessage();
                }
            }
            ::GetMessage( &msg, NULL, 0, 0 );
            if( msg.message == WM_EXITHELPMODE ) {
                break;
            } else if( msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE ) {
                break;
            } else if( (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST) ||
                       msg.message == WM_SYSCOMMAND ) {
                if( !pApp->PreTranslateMessage( &msg ) ) {
                    ::TranslateMessage( &msg );
                    if( msg.message != WM_SYSKEYDOWN && msg.message != WM_SYSKEYUP &&
                        msg.message != WM_SYSCHAR && msg.message != WM_SYSDEADCHAR &&
                        msg.message != WM_SYSCOMMAND ) {
                        continue;
                    }
                }
            } else if( msg.message == WM_LBUTTONDOWN ||
                       msg.message == WM_NCLBUTTONDOWN ) {
                int nHitTest = (int)::SendMessage( msg.hwnd, WM_NCHITTEST, 0,
                                                   MAKELPARAM( msg.pt.x, msg.pt.y ) );
                if( nHitTest == HTCLIENT ) {
                    HWND hWnd = msg.hwnd;
                    while( hWnd != NULL && dwHelpId == 0L ) {
                        CPoint point( msg.pt );
                        ::ScreenToClient( hWnd, &point );
                        dwHelpId = (DWORD_PTR)::SendMessage( hWnd, WM_HELPHITTEST, 0,
                                                             MAKELPARAM( point.x,
                                                                         point.y ) );
                        hWnd = ::GetParent( hWnd );
                    }
                    break;
                } else if( nHitTest >= 0 && nHitTest != HTMENU &&
                           nHitTest != HTSYSMENU ) {
                    dwHelpId = HID_BASE_NCAREAS + nHitTest;
                    break;
                }
            }
            ::DispatchMessage( &msg );
        }
        m_bHelpMode = FALSE;
        ::SendMessage( m_hWnd, WM_SETMESSAGESTRING, nIDOld, 0L );

        POINT point;
        ::GetCursorPos( &point );

        HWND hWndCursor = ::WindowFromPoint( point );
        if( hWndCursor != NULL ) {
            int nHitTest = (int)::SendMessage( hWndCursor, WM_NCHITTEST, 0,
                                                 MAKELPARAM( point.x, point.y ) );
            ::SendMessage( hWndCursor, WM_SETCURSOR, (WPARAM)hWndCursor,
                           MAKELPARAM( nHitTest, WM_MOUSEMOVE ) );
        } else {
            HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW );
            ::SetCursor( hCursor );
        }

        if( dwHelpId != 0L ) {
            pApp->WinHelpInternal( dwHelpId );
        }
    }
}

void CFrameWnd::OnUpdateControlBarMenu( CCmdUI *pCmdUI )
/******************************************************/
{
    CControlBar *pBar = GetControlBar( pCmdUI->m_nID );
    if( pBar != NULL ) {
        pCmdUI->SetCheck( pBar->IsWindowVisible() ? 1 : 0 );
    } else {
        pCmdUI->ContinueRouting();
    }
}
