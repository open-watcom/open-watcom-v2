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
* Description:  Implementation of CView.
*
****************************************************************************/


#include "stdafx.h"
#include "printing.h"

BOOL CALLBACK _AbortProc( HDC hDC, int iError )
/*********************************************/
{
    UNUSED_ALWAYS( hDC );
    UNUSED_ALWAYS( iError );

    CWinThread *pThread = AfxGetThread();
    ASSERT( pThread != NULL );

    MSG msg;
    while( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
        pThread->PumpMessage();
    }
    return( !_AbortPrinting );
}

IMPLEMENT_DYNAMIC( CView, CWnd )

BEGIN_MESSAGE_MAP( CView, CWnd )
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEACTIVATE()
    ON_WM_PAINT()
    ON_MESSAGE_VOID( WM_INITIALUPDATE, OnInitialUpdate )
    ON_COMMAND_EX( ID_NEXT_PANE, OnNextPaneCmd )
    ON_COMMAND_EX( ID_PREV_PANE, OnNextPaneCmd )
    ON_COMMAND_EX( ID_WINDOW_SPLIT, OnSplitCmd )
    ON_UPDATE_COMMAND_UI( ID_WINDOW_SPLIT, OnUpdateSplitCmd )
    ON_UPDATE_COMMAND_UI( ID_NEXT_PANE, OnUpdateNextPaneMenu )
    ON_UPDATE_COMMAND_UI( ID_PREV_PANE, OnUpdateNextPaneMenu )
END_MESSAGE_MAP()

CView::CView()
/************/
{
    m_pDocument = NULL;
}

CView::~CView()
/*************/
{
    if( m_pDocument != NULL ) {
        m_pDocument->RemoveView( this );
    }
}

BOOL CView::IsSelected( const CObject *pOb ) const
/************************************************/
{
    UNUSED_ALWAYS( pOb );
    return( FALSE );
}

void CView::OnActivateFrame( UINT nState, CFrameWnd *pFrameWnd )
/**************************************************************/
{
    UNUSED_ALWAYS( nState );
    UNUSED_ALWAYS( pFrameWnd );
}

void CView::OnActivateView( BOOL bActivate, CView *pActivateView, CView *pDeactiveView )
/**************************************************************************************/
{
    UNUSED_ALWAYS( pActivateView );
    UNUSED_ALWAYS( pDeactiveView );

    if( bActivate ) {
        SetFocus();
    }
}

void CView::OnBeginPrinting( CDC *pDC, CPrintInfo *pInfo )
/********************************************************/
{
    UNUSED_ALWAYS( pDC );
    UNUSED_ALWAYS( pInfo );
}

void CView::OnEndPrintPreview( CDC *pDC, CPrintInfo *pInfo, POINT point,
                               CPreviewView *pView )
/**************************************************/
{
    UNUSED_ALWAYS( point );

    OnEndPrinting( pDC, pInfo );
    
    ASSERT( pView != NULL );
    CFrameWnd *pFrame = pView->GetParentFrame();
    ASSERT( pFrame != NULL );
    CPrintPreviewState *pState = pView->m_pPreviewState;
    ASSERT( pState != NULL );
    pFrame->SetActiveView( pState->pViewActiveOld );
    pFrame->OnSetPreviewMode( FALSE, pState );
    pView->DestroyWindow();
    pFrame->RecalcLayout();
}

void CView::OnEndPrinting( CDC *pDC, CPrintInfo *pInfo )
/******************************************************/
{
    UNUSED_ALWAYS( pDC );
    UNUSED_ALWAYS( pInfo );
}

void CView::OnInitialUpdate()
/***************************/
{
    OnUpdate( NULL, 0L, NULL );
}

void CView::OnPrepareDC( CDC *pDC, CPrintInfo *pInfo )
/****************************************************/
{
    UNUSED_ALWAYS( pDC );

    if( pInfo != NULL && pInfo->GetMaxPage() == 0xFFFF && pInfo->m_nCurPage != 1 ) {
        pInfo->m_bContinuePrinting = FALSE;
    }
}

BOOL CView::OnPreparePrinting( CPrintInfo *pInfo )
/************************************************/
{
    UNUSED_ALWAYS( pInfo );
    return( FALSE );
}

void CView::OnPrint( CDC *pDC, CPrintInfo *pInfo )
/************************************************/
{
    UNUSED_ALWAYS( pInfo );
    OnDraw( pDC );
}

BOOL CView::OnScroll( UINT nScrollCode, UINT nPos, BOOL bDoScroll )
/*****************************************************************/
{
    UNUSED_ALWAYS( nScrollCode );
    UNUSED_ALWAYS( nPos );
    UNUSED_ALWAYS( bDoScroll );
    return( FALSE );
}

BOOL CView::OnScrollBy( CSize sizeScroll, BOOL bDoScroll )
/********************************************************/
{
    UNUSED_ALWAYS( sizeScroll );
    UNUSED_ALWAYS( bDoScroll );
    return( FALSE );
}

void CView::OnUpdate( CView *pSender, LPARAM lHint, CObject *pHint )
/******************************************************************/
{
    UNUSED_ALWAYS( pSender );
    UNUSED_ALWAYS( lHint );
    UNUSED_ALWAYS( pHint );
    Invalidate();
}

CScrollBar *CView::GetScrollBarCtrl( int nBar ) const
/***************************************************/
{
    ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
    if( (nBar == SB_HORZ && !(GetStyle() & WS_HSCROLL)) ||
        (nBar == SB_VERT && !(GetStyle() & WS_VSCROLL)) ) {
        CWnd *pSplitter = GetParent();
        ASSERT( pSplitter != NULL );
        if( pSplitter->IsKindOf( RUNTIME_CLASS( CSplitterWnd ) ) ) {
            UINT nID = ::GetDlgCtrlID( m_hWnd );
            if( nBar == SB_HORZ ) {
                nID = AFX_IDW_HSCROLL_FIRST + (nID - AFX_IDW_PANE_FIRST) % 16;
            } else {
                nID = AFX_IDW_VSCROLL_FIRST + (nID - AFX_IDW_PANE_FIRST) / 16;
            }
            return( (CScrollBar *)pSplitter->GetDlgItem( nID ) );
        }
    }
    return( NULL );
}

BOOL CView::OnCmdMsg( UINT nID, int nCode, void *pExtra,
                      AFX_CMDHANDLERINFO *pHandlerInfo )
/******************************************************/
{
    if( CWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) {
        return( TRUE );
    }

    CDocument *pDoc = GetDocument();
    if( pDoc != NULL && pDoc->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) {
        return( TRUE );
    }
    return( FALSE );
}

BOOL CView::PreCreateWindow( CREATESTRUCT &cs )
/*********************************************/
{
    if( !CWnd::PreCreateWindow( cs ) ) {
        return( FALSE );
    }
    if( cs.lpszClass == NULL ) {
        cs.lpszClass = _T("AfxFrameOrView");
    }
    if( cs.style & WS_BORDER ) {
        cs.style &= ~WS_BORDER;
        cs.dwExStyle |= WS_EX_CLIENTEDGE;
    }
    return( TRUE );
}

void CView::PostNcDestroy()
/*************************/
{
    delete this;
}

#ifdef _DEBUG

void CView::Dump( CDumpContext &dc ) const
/****************************************/
{
    CWnd::Dump( dc );

    dc << "m_pDocument = " << (void *)m_pDocument << "\n";
}

#endif // _DEBUG

BOOL CView::DoPreparePrinting( CPrintInfo *pInfo )
/************************************************/
{
    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );
    ASSERT( pInfo != NULL );
    if( pInfo->m_bPreview || pInfo->m_bDirect ) {
        if( !pApp->GetPrinterDeviceDefaults( (PRINTDLG *)&pInfo->m_pPD->m_pd ) ) {
            return( FALSE );
        }
    } else {
        if( pApp->DoPrintDialog( pInfo->m_pPD ) != IDOK ) {
            return( FALSE );
        }
    }
    if( pInfo->m_pPD->m_pd.hDC == NULL ) {
        pInfo->m_pPD->m_pd.hDC = pInfo->m_pPD->CreatePrinterDC();
        if( pInfo->m_pPD->m_pd.hDC == NULL ) {
            return( FALSE );
        }
    }
    pInfo->m_nNumPreviewPages = pApp->m_nNumPreviewPages;
    pInfo->m_strPageDesc.LoadString( AFX_IDS_PREVIEWPAGEDESC );
    return( TRUE );
}

BOOL CView::DoPrintPreview( UINT nIDResource, CView *pPrintView,
                            CRuntimeClass *pPreviewViewClass,
                            CPrintPreviewState *pState )
/******************************************************/
{
    UNUSED_ALWAYS( nIDResource );

    ASSERT( pPreviewViewClass != NULL );
    CPreviewView *pPreviewView = (CPreviewView *)pPreviewViewClass->CreateObject();
    if( pPreviewView == NULL ) {
        return( FALSE );
    }
    ASSERT( pPreviewView->IsKindOf( RUNTIME_CLASS( CPreviewView ) ) );
    pPreviewView->m_pOrigView = this;
    pPreviewView->m_pPreviewState = pState;
    if( !pPreviewView->SetPrintView( pPrintView ) ) {
        delete pPreviewView;
        return( FALSE );
    }
    
    CFrameWnd *pFrame = GetTopLevelFrame();
    ASSERT( pFrame != NULL );
    pFrame->OnSetPreviewMode( TRUE, pState );

    CCreateContext context;
    context.m_pNewViewClass = NULL;
    context.m_pCurrentDoc = m_pDocument;
    context.m_pNewDocTemplate = NULL;
    context.m_pLastView = this;
    context.m_pCurrentFrame = NULL;
    if( !pPreviewView->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect( 0, 0, 0, 0 ),
                               pFrame, AFX_IDW_PANE_FIRST, &context ) ) {
        delete pPreviewView;
        pFrame->OnSetPreviewMode( FALSE, pState );
        return( FALSE );
    }
    pState->pViewActiveOld = pFrame->GetActiveView();
    pFrame->SetActiveView( pPreviewView );

    pPreviewView->m_pToolBar = new CDialogBar;
    if( !pPreviewView->m_pToolBar->Create( pFrame, nIDResource,
                                           WS_CHILD | WS_VISIBLE | CBRS_TOP,
                                           AFX_IDW_PREVIEW_BAR ) ) {
        delete pPreviewView->m_pToolBar;
        delete pPreviewView;
        pFrame->OnSetPreviewMode( FALSE, pState );
        return( FALSE );
    }
    pFrame->RecalcLayout();
    return( TRUE );
}

int CView::OnCreate( LPCREATESTRUCT lpCreateStruct )
/**************************************************/
{
    if( CWnd::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }

    CCreateContext *pContext = (CCreateContext *)lpCreateStruct->lpCreateParams;
    if( pContext != NULL && pContext->m_pCurrentDoc != NULL ) {
        pContext->m_pCurrentDoc->AddView( this );
    }
    return( 0 );
}

void CView::OnDestroy()
/********************/
{
    CFrameWnd *pFrame = GetParentFrame();
    if( pFrame != NULL && pFrame->GetActiveView() == this ) {
        pFrame->SetActiveView( NULL );
    }
    CWnd::OnDestroy();
}

BOOL CView::OnEraseBkgnd( CDC *pDC )
/**********************************/
{
    RECT    rect;
    CBrush  br;
    ::GetClientRect( m_hWnd, &rect );
    br.CreateSysColorBrush( COLOR_WINDOW );
    pDC->FillRect( &rect, &br );
    return( TRUE );
}

int CView::OnMouseActivate( CWnd *pDesktopWnd, UINT nHitTest, UINT message )
/**************************************************************************/
{
    int nRet = CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
    if( nRet == MA_NOACTIVATE || nRet == MA_NOACTIVATEANDEAT ) {
        return( nRet );
    }

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    if( pFrame->GetActiveView() != this ) {
        pFrame->SetActiveView( this );
    }
    
    return( nRet );
}

void CView::OnPaint()
/*******************/
{
    CPaintDC dc( this );
    OnPrepareDC( &dc );
    OnDraw( &dc );
}

BOOL CView::OnNextPaneCmd( UINT nID )
/***********************************/
{
    CSplitterWnd *pSplitter = (CSplitterWnd *)GetParent();
    if( pSplitter == NULL || !pSplitter->IsKindOf( RUNTIME_CLASS( CSplitterWnd ) ) ) {
        return( FALSE );
    }
    pSplitter->ActivateNext( nID == ID_PREV_PANE );
    return( TRUE );
}

BOOL CView::OnSplitCmd( UINT nID )
/********************************/
{
    UNUSED_ALWAYS( nID );

    CSplitterWnd *pSplitter = (CSplitterWnd *)GetParent();
    if( pSplitter == NULL || !pSplitter->IsKindOf( RUNTIME_CLASS( CSplitterWnd ) ) ) {
        return( FALSE );
    }
    pSplitter->DoKeyboardSplit();
    return( TRUE );
}

void CView::OnUpdateNextPaneMenu( CCmdUI *pCmdUI )
/************************************************/
{
    ASSERT( pCmdUI != NULL );

    CSplitterWnd *pSplitter = (CSplitterWnd *)GetParent();
    pCmdUI->Enable( pSplitter != NULL &&
                    pSplitter->IsKindOf( RUNTIME_CLASS( CSplitterWnd ) ) &&
                    pSplitter->CanActivateNext( pCmdUI->m_nID == ID_PREV_PANE ) );
}

void CView::OnUpdateSplitCmd( CCmdUI *pCmdUI )
/********************************************/
{
    ASSERT( pCmdUI != NULL );

    CSplitterWnd *pSplitter = (CSplitterWnd *)GetParent();
    pCmdUI->Enable( pSplitter != NULL &&
                    pSplitter->IsKindOf( RUNTIME_CLASS( CSplitterWnd ) ) &&
                    !pSplitter->IsTracking() );
}

void CView::OnFilePrint()
/***********************/
{
    CPrintInfo info;
    if( LOWORD( CWnd::GetCurrentMessage()->wParam ) == ID_FILE_PRINT_DIRECT ) {
        info.m_bDirect = TRUE;
    }
    if( OnPreparePrinting( &info ) ) {
        CDocument *pDoc = GetDocument();
        ASSERT( pDoc != NULL );

        _AbortPrinting = FALSE;
        CPrintingDialog dlg;
        dlg.SetDlgItemText( AFX_IDC_PRINT_DOCNAME, pDoc->GetTitle() );
        dlg.SetDlgItemText( AFX_IDC_PRINT_PRINTERNAME, info.m_pPD->GetDeviceName() );
        dlg.SetDlgItemText( AFX_IDC_PRINT_PORTNAME, info.m_pPD->GetPortName() );
        dlg.ShowWindow( SW_SHOW );
        dlg.UpdateWindow();
        
        int nStartPage = info.GetFromPage();
        int nEndPage = info.GetToPage();
        if( nStartPage < info.GetMinPage() ) {
            nStartPage = info.GetMinPage();
        }
        if( nStartPage > info.GetMaxPage() ) {
            nStartPage = info.GetMaxPage();
        }
        if( nEndPage > info.GetMinPage() ) {
            nEndPage = info.GetMinPage();
        }
        if( nEndPage < info.GetMaxPage() ) {
            nEndPage = info.GetMaxPage();
        }
        if( nStartPage > nEndPage ) {
            int nTemp = nStartPage;
            nStartPage = nEndPage;
            nEndPage = nTemp;
        }

        CDC *pDC = CDC::FromHandle( info.m_pPD->m_pd.hDC );
        pDC->m_bPrinting = TRUE;
        pDC->SetAbortProc( _AbortProc );
        OnBeginPrinting( pDC, &info );

        DOCINFO di;
        di.cbSize = sizeof( DOCINFO );
        di.lpszDocName = pDoc->GetTitle();
        di.lpszOutput = NULL;
        di.lpszDatatype = NULL;
        di.fwType = 0L;
        if( pDC->StartDoc( &di ) > 0 ) {
            info.m_rectDraw.left = 0;
            info.m_rectDraw.top = 0;
            info.m_rectDraw.right = pDC->GetDeviceCaps( HORZRES );
            info.m_rectDraw.bottom = pDC->GetDeviceCaps( VERTRES );
            
            BOOL bFailed = FALSE;
            for( info.m_nCurPage = nStartPage; info.m_nCurPage <= nEndPage;
                 info.m_nCurPage++ ) {
                OnPrepareDC( pDC, &info );
                if( !info.m_bContinuePrinting ) {
                    break;
                }
                dlg.SetDlgItemInt( AFX_IDC_PRINT_PAGENUM, info.m_nCurPage );
                if( pDC->StartPage() < 0 ) {
                    bFailed = TRUE;
                    break;
                }
                OnPrepareDC( pDC, &info );
                OnPrint( pDC, &info );
                pDC->EndPage();
            }
            if( bFailed ) {
                pDC->AbortDoc();
            } else {
                pDC->EndDoc();
            }
        }
        
        OnEndPrinting( pDC, &info );
        dlg.DestroyWindow();
    }
}

void CView::OnFilePrintPreview()
/******************************/
{
    CPrintPreviewState *pState = new CPrintPreviewState;
    if( !DoPrintPreview( AFX_IDD_PREVIEW_TOOLBAR, this, RUNTIME_CLASS( CPreviewView ),
                         pState ) ) {
        delete pState;
    }
}
