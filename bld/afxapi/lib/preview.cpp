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
* Description:  Implementation of CPreviewView.
*
****************************************************************************/


#include "stdafx.h"

// Preview margin size
#define MARGIN_SIZE 8

IMPLEMENT_DYNCREATE( CPreviewView, CScrollView )

BEGIN_MESSAGE_MAP( CPreviewView, CScrollView )
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
    ON_WM_SIZE()
    ON_COMMAND( AFX_ID_PREVIEW_NEXT, OnNextPage )
    ON_COMMAND( AFX_ID_PREVIEW_NUMPAGE, OnNumPageChange )
    ON_COMMAND( AFX_ID_PREVIEW_PREV, OnPrevPage )
    ON_COMMAND( AFX_ID_PREVIEW_CLOSE, OnPreviewClose )
    ON_COMMAND( AFX_ID_PREVIEW_PRINT, OnPreviewPrint )
    ON_COMMAND( AFX_ID_PREVIEW_ZOOMIN, OnZoomIn )
    ON_COMMAND( AFX_ID_PREVIEW_ZOOMOUT, OnZoomOut )
    ON_UPDATE_COMMAND_UI( AFX_ID_PREVIEW_NEXT, OnUpdateNextPage )
    ON_UPDATE_COMMAND_UI( AFX_ID_PREVIEW_NUMPAGE, OnUpdateNumPageChange )
    ON_UPDATE_COMMAND_UI( AFX_ID_PREVIEW_PREV, OnUpdatePrevPage )
    ON_UPDATE_COMMAND_UI( AFX_ID_PREVIEW_ZOOMIN, OnUpdateZoomIn )
    ON_UPDATE_COMMAND_UI( AFX_ID_PREVIEW_ZOOMOUT, OnUpdateZoomOut )
END_MESSAGE_MAP()

CPreviewView::CPreviewView()
/**************************/
{
    m_pOrigView = NULL;
    m_pPrintView = NULL;
    m_pPreviewDC = NULL;
    m_pPreviewState = NULL;
    m_pToolBar = NULL;
    m_pPageInfo = m_pageInfoArray;
    m_nZoomOutPages = 1;
    m_nZoomState = ZOOM_OUT;
    m_nCurrentPage = 1;
    m_nPages = 1;
    m_hMagnifyCursor = NULL;
    m_pPreviewInfo = NULL;
}

CSize CPreviewView::CalcScaleRatio( CSize windowSize, CSize actualSize )
/**********************************************************************/
{
    // The return value of this function contains the numerator and denominator of a
    // ratio stored in the cx and cy fields, respectively, of a CSize object.  It is not
    // an actual size!
    CSize sizeRatio;
    if( ::MulDiv( actualSize.cy, windowSize.cx, actualSize.cx ) <= windowSize.cy ) {
        sizeRatio.cx = windowSize.cx;
        sizeRatio.cy = actualSize.cx;
    } else {
        sizeRatio.cx = windowSize.cy;
        sizeRatio.cy = actualSize.cy;
    }
    return( sizeRatio );
}

void CPreviewView::OnDisplayPageNumber( UINT nPage, UINT nPagesDisplayed )
/************************************************************************/
{
    CString strSubstring;
    CString strFormatted;
    ASSERT( m_pPreviewInfo != NULL );
    if( nPagesDisplayed == 1 ) {
        AfxExtractSubString( strSubstring, m_pPreviewInfo->m_strPageDesc, 0 );
        strFormatted.Format( strSubstring, nPage );
    } else {
        AfxExtractSubString( strSubstring, m_pPreviewInfo->m_strPageDesc, 1 );
        strFormatted.Format( strSubstring, nPage, nPage + nPagesDisplayed - 1 );
    }

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    pFrame->SendMessage( WM_SETMESSAGESTRING, 0, (LPARAM)(LPCTSTR)strFormatted );
}

void CPreviewView::PositionPage( UINT nPage )
/*******************************************/
{
    CSize   windowSize = CalcPageDisplaySize();
    int     nPrinterX = m_dcPrint.GetDeviceCaps( HORZRES );
    int     nPrinterY = m_dcPrint.GetDeviceCaps( VERTRES );
    HDC     hScreenDC = ::GetDC( NULL );
    int     nScreenLogX = ::GetDeviceCaps( hScreenDC, LOGPIXELSX );
    int     nScreenLogY = ::GetDeviceCaps( hScreenDC, LOGPIXELSY );
    ::ReleaseDC( NULL, hScreenDC );

    CSize &actualSize = m_pPageInfo[nPage].sizeUnscaled;
    actualSize.cx = ::MulDiv( nPrinterX, nScreenLogX, m_sizePrinterPPI.cx );
    actualSize.cy = ::MulDiv( nPrinterY, nScreenLogY, m_sizePrinterPPI.cy );
    m_pPageInfo[nPage].sizeZoomOutRatio = CalcScaleRatio( windowSize, actualSize );

    SetScaledSize( nPage );
}

void CPreviewView::OnDraw( CDC *pDC )
/***********************************/
{
    ASSERT( m_pPreviewDC != NULL );
    ASSERT( m_pPreviewInfo != NULL );
    ASSERT( m_pPrintView != NULL );
    UINT nPagesDisplayed = m_nPages;
    for( int i = 0; i < m_nPages; i++ ) {
        PositionPage( i );

        CPoint  ptScrollPos;
        if( m_nZoomState != ZOOM_OUT ) {
            ptScrollPos = -GetDeviceScrollPosition();
        }
        
        int     nSavedDC = pDC->SaveDC();
        pDC->SetMapMode( MM_TEXT );
        pDC->SetWindowOrg( 0, 0 );
        pDC->SetViewportOrg( ptScrollPos );
        pDC->SelectStockObject( BLACK_PEN );
        pDC->SelectStockObject( WHITE_BRUSH );
        pDC->Rectangle( &m_pPageInfo[i].rectScreen );
        pDC->RestoreDC( nSavedDC );

        CPoint ptTopLeft = m_pPageInfo[i].rectScreen.TopLeft();
        ptTopLeft += ptScrollPos;
        m_pPreviewDC->SetOutputDC( pDC->m_hDC );
        m_pPreviewDC->SetScaleRatio( m_pPageInfo[i].sizeScaleRatio.cx,
                                     m_pPageInfo[i].sizeScaleRatio.cy );
        m_pPreviewDC->SetTopLeftOffset( ptTopLeft );
        m_pPreviewDC->ClipToPage();
        m_pPreviewInfo->m_nCurPage = m_nCurrentPage + i;
        m_pPreviewInfo->m_bContinuePrinting = TRUE;
        m_pPrintView->OnPrepareDC( m_pPreviewDC, m_pPreviewInfo );
        if( !m_pPreviewInfo->m_bContinuePrinting ) {
            if( i == 0 ) {
                if( m_nCurrentPage > 0 ) {
                    m_nCurrentPage--;
                    Invalidate();
                }
            } else {
                nPagesDisplayed = i;
            }
            break;
        }
        m_pPreviewInfo->m_rectDraw.left = 0;
        m_pPreviewInfo->m_rectDraw.top = 0;
        m_pPreviewInfo->m_rectDraw.right = m_pPreviewDC->GetDeviceCaps( HORZRES );
        m_pPreviewInfo->m_rectDraw.bottom = m_pPreviewDC->GetDeviceCaps( VERTRES );
        m_pPreviewDC->DPtoLP( &m_pPreviewInfo->m_rectDraw );
        m_pPrintView->OnPrint( m_pPreviewDC, m_pPreviewInfo );
        m_pPreviewDC->ReleaseOutputDC();
    }
    OnDisplayPageNumber( m_nCurrentPage, nPagesDisplayed );
}

CSize CPreviewView::CalcPageDisplaySize()
/***************************************/
{
    CRect rect;
    ::GetClientRect( m_hWnd, &rect );
    rect.OffsetRect( -rect.TopLeft() );
    ASSERT( m_nPages == 1 || m_nPages == 2 );
    if( m_nPages == 2 ) {
        m_nSecondPageOffset = (rect.right - MARGIN_SIZE) / 2;
        rect.right -= 3 * MARGIN_SIZE;
        rect.right /= 2;
    } else {
        rect.right -= 2 * MARGIN_SIZE;
    }
    rect.bottom -= 2 * MARGIN_SIZE;
    return( CSize( rect.right, rect.bottom ) );
}

void CPreviewView::DoZoom( UINT nPage, CPoint point )
/***************************************************/
{
    m_nCurrentPage += nPage;
    if( m_nZoomState == ZOOM_OUT ) {
        ::ShowScrollBar( m_hWnd, SB_HORZ, FALSE );
        ::ShowScrollBar( m_hWnd, SB_VERT, FALSE );
    } else {
        PositionPage( 0 );

        CSize &sizeScaleRatio = m_pPageInfo[0].sizeScaleRatio;
        point.x = ::MulDiv( point.x, sizeScaleRatio.cx, sizeScaleRatio.cy );
        point.y = ::MulDiv( point.y, sizeScaleRatio.cx, sizeScaleRatio.cy );
        point += m_pPageInfo[0].rectScreen.TopLeft();
        CenterOnPoint( point );
    }
    Invalidate();
    ::UpdateWindow( m_hWnd );
}

BOOL CPreviewView::FindPageRect( CPoint &point, UINT &nPage )
/***********************************************************/
{
    if( m_nZoomState != ZOOM_OUT ) {
        point += GetDeviceScrollPosition();
    }
    for( int i = 0; i < m_nPages; i++ ) {
        if( m_pPageInfo[i].rectScreen.PtInRect( point ) ) {
            CSize &sizeScaleRatio = m_pPageInfo[i].sizeScaleRatio;
            point.x = ::MulDiv( point.x, sizeScaleRatio.cy, sizeScaleRatio.cx );
            point.y = ::MulDiv( point.y, sizeScaleRatio.cy, sizeScaleRatio.cx );
            nPage = i;
            return( TRUE );
        }
    }
    return( FALSE );
}

void CPreviewView::SetScaledSize( UINT nPage )
/********************************************/
{
    CRect   &rectScreen = m_pPageInfo[nPage].rectScreen;
    CSize   &sizeUnscaled = m_pPageInfo[nPage].sizeUnscaled;
    CSize   &sizeScaleRatio = m_pPageInfo[nPage].sizeScaleRatio;
    CSize   &sizeZoomOutRatio = m_pPageInfo[nPage].sizeZoomOutRatio;
    ASSERT( m_nZoomState == ZOOM_OUT || m_nZoomState == ZOOM_MIDDLE ||
            m_nZoomState == ZOOM_IN );
    switch( m_nZoomState ) {
    case ZOOM_OUT:
        sizeScaleRatio = sizeZoomOutRatio;
        break;
    case ZOOM_MIDDLE:
        // Set the scale ratio halfway between ZOOM_OUT and ZOOM_IN.
        sizeScaleRatio.cx = sizeZoomOutRatio.cx + sizeZoomOutRatio.cy;
        sizeScaleRatio.cy = sizeZoomOutRatio.cy * 2;
        break;
    case ZOOM_IN:
        sizeScaleRatio.cx = 1;
        sizeScaleRatio.cy = 1;
    }

    CSize   sizeScaled;
    sizeScaled.cx = ::MulDiv( sizeUnscaled.cx, sizeScaleRatio.cx, sizeScaleRatio.cy );
    sizeScaled.cy = ::MulDiv( sizeUnscaled.cy, sizeScaleRatio.cx, sizeScaleRatio.cy );

    CSize   sizeDisplay = CalcPageDisplaySize();
    rectScreen.SetRect( MARGIN_SIZE, MARGIN_SIZE, MARGIN_SIZE + sizeScaled.cx,
                        MARGIN_SIZE + sizeScaled.cy );
    if( sizeScaled.cx + 2 * MARGIN_SIZE < sizeDisplay.cx ) {
        rectScreen.OffsetRect( (sizeDisplay.cx - rectScreen.Width()) / 2, 0 );
    }
    if( sizeScaled.cy + 2 * MARGIN_SIZE < sizeDisplay.cy ) {
        rectScreen.OffsetRect( 0, (sizeDisplay.cy - rectScreen.Height()) / 2 );
    }
    if( nPage == 1 ) {
        ASSERT( m_nPages == 2 );
        rectScreen.left += m_nSecondPageOffset;
        rectScreen.right += m_nSecondPageOffset;
    }
    if( m_nZoomState != ZOOM_OUT ) {
        CSize sizeScroll = rectScreen.Size();
        sizeScroll += CSize( 2 * MARGIN_SIZE, 2 * MARGIN_SIZE );
        SetScrollSizes( MM_TEXT, sizeScroll );
    }
}

void CPreviewView::SetZoomState( UINT nNewState, UINT nPage, CPoint point )
/*************************************************************************/
{
    ASSERT( m_nZoomState == ZOOM_OUT || m_nZoomState == ZOOM_MIDDLE ||
            m_nZoomState == ZOOM_IN );
    m_nZoomState = nNewState;
    DoZoom( nPage, point );
}

BOOL CPreviewView::SetPrintView( CView *pPrintView )
/**************************************************/
{
    ASSERT( m_pPrintView == NULL );
    m_pPrintView = pPrintView;

    ASSERT( m_pPreviewInfo == NULL );
    m_pPreviewInfo = new CPrintInfo;
    m_pPreviewInfo->m_bPreview = TRUE;
    if( !pPrintView->OnPreparePrinting( m_pPreviewInfo ) ) {
        return( FALSE );
    }
    m_nPages = m_pPreviewInfo->m_nNumPreviewPages;
    if( m_nPages == 0 ) {
        m_nPages = 1;
    }
    
    ASSERT( m_pPreviewInfo->m_pPD != NULL );
    ASSERT( m_pPreviewInfo->m_pPD->m_pd.hDC != NULL );
    ASSERT( m_dcPrint.m_hDC == NULL );
    m_dcPrint.Attach( m_pPreviewInfo->m_pPD->m_pd.hDC );

    m_sizePrinterPPI.cx = m_dcPrint.GetDeviceCaps( LOGPIXELSX );
    m_sizePrinterPPI.cy = m_dcPrint.GetDeviceCaps( LOGPIXELSY );
    
    ASSERT( m_pPreviewDC == NULL );
    m_pPreviewDC = new CPreviewDC;
    m_pPreviewDC->SetAttribDC( m_dcPrint.m_hDC );

    m_pPrintView->OnBeginPrinting( m_pPreviewDC, m_pPreviewInfo );
    
    return( TRUE );
}

int CPreviewView::OnCreate( LPCREATESTRUCT lpCreateStruct )
/*********************************************************/
{
    if( CView::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }

    CCreateContext *pContext = (CCreateContext *)lpCreateStruct->lpCreateParams;
    ASSERT( pContext != NULL );
    ASSERT( pContext->m_pLastView != NULL );
    m_pOrigView = pContext->m_pLastView;
    return( 0 );
}

BOOL CPreviewView::OnEraseBkgnd( CDC *pDC )
/*****************************************/
{
    RECT    rect;
    CBrush  br;
    ::GetClientRect( m_hWnd, &rect );
    br.CreateSysColorBrush( COLOR_APPWORKSPACE );
    pDC->FillRect( &rect, &br );
    return( TRUE );
}

void CPreviewView::OnLButtonDown( UINT nFlags, CPoint point )
/***********************************************************/
{
    UNUSED_ALWAYS( nFlags );

    UINT nPage;
    if( FindPageRect( point, nPage ) ) {
        int nNewZoomState;
        ASSERT( m_nZoomState == ZOOM_OUT || m_nZoomState == ZOOM_MIDDLE ||
                m_nZoomState == ZOOM_IN );
        switch( m_nZoomState ) {
        case ZOOM_OUT:
            nNewZoomState = ZOOM_MIDDLE;
            m_nZoomOutPages = m_nPages;
            m_nPages = 1;
            break;
        case ZOOM_MIDDLE:
            nNewZoomState = ZOOM_IN;
            break;
        case ZOOM_IN:
            nNewZoomState = ZOOM_OUT;
            m_nPages = m_nZoomOutPages;
            break;
        }
        SetZoomState( nNewZoomState, nPage, point );
    }
}

BOOL CPreviewView::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT message )
/***********************************************************************/
{
    if( nHitTest == HTCLIENT ) {
        CPoint point( ::GetMessagePos() );
        ::ScreenToClient( m_hWnd, &point );

        UINT nPage;
        if( FindPageRect( point, nPage ) ) {
            if( m_hMagnifyCursor == NULL ) {
                HINSTANCE hInstance = AfxFindResourceHandle(
                    MAKEINTRESOURCE( AFX_IDC_MAGNIFY ), RT_CURSOR );
                m_hMagnifyCursor = ::LoadCursor( hInstance,
                                                 MAKEINTRESOURCE( AFX_IDC_MAGNIFY ) );
            }
            ::SetCursor( m_hMagnifyCursor );
        } else {
            ::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
        }
        return( TRUE );
    }
    return( CScrollView::OnSetCursor( pWnd, nHitTest, message ) );
}

void CPreviewView::OnSize( UINT nType, int cx, int cy )
/*****************************************************/
{
    if( m_nZoomState == ZOOM_OUT ) {
        // Don't update scroll bars when zoomed out.
        CView::OnSize( nType, cx, cy );
    } else {
        CScrollView::OnSize( nType, cx, cy );
    }
}

void CPreviewView::OnNextPage()
/*****************************/
{
    m_nCurrentPage++;
    Invalidate();
    ::UpdateWindow( m_hWnd );
}

void CPreviewView::OnNumPageChange()
/**********************************/
{
    ASSERT( m_nPages == 1 || m_nPages == 2 );
    if( m_nPages == 2 ) {
        m_nPages = 1;
    } else {
        m_nPages = 2;
    }
    Invalidate();
    ::UpdateWindow( m_hWnd );

    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );
    pApp->m_nNumPreviewPages = m_nPages;
}

void CPreviewView::OnPrevPage()
/*****************************/
{
    ASSERT( m_nCurrentPage > 0 );
    m_nCurrentPage--;
    Invalidate();
    ::UpdateWindow( m_hWnd );
}

void CPreviewView::OnPreviewClose()
/*********************************/
{
    ASSERT( m_pOrigView != NULL );
    m_pOrigView->OnEndPrintPreview( m_pPreviewDC, m_pPreviewInfo, CPoint(), this );
}

void CPreviewView::OnPreviewPrint()
/*********************************/
{
    ASSERT( m_pOrigView != NULL );
    CFrameWnd *pFrame = m_pOrigView->GetTopLevelFrame();
    ASSERT( pFrame != NULL );
    OnPreviewClose();
    pFrame->SendMessage( WM_COMMAND, ID_FILE_PRINT );
}

void CPreviewView::OnZoomIn()
/***************************/
{
    int nNewZoomState;
    ASSERT( m_nZoomState == ZOOM_OUT || m_nZoomState == ZOOM_MIDDLE );
    if( m_nZoomState == ZOOM_OUT ) {
        nNewZoomState = ZOOM_MIDDLE;
        m_nZoomOutPages = m_nPages;
        m_nPages = 1;
    } else if( m_nZoomState == ZOOM_MIDDLE ) {
        nNewZoomState = ZOOM_IN;
    }
    SetZoomState( nNewZoomState, m_nCurrentPage, CPoint() );
}

void CPreviewView::OnZoomOut()
/****************************/
{
    int nNewZoomState;
    ASSERT( m_nZoomState == ZOOM_IN || m_nZoomState == ZOOM_MIDDLE );
    if( m_nZoomState == ZOOM_IN ) {
        nNewZoomState = ZOOM_MIDDLE;
    } else if( m_nZoomState == ZOOM_MIDDLE ) {
        nNewZoomState = ZOOM_OUT;
        m_nPages = m_nZoomOutPages;
    }
    SetZoomState( nNewZoomState, m_nCurrentPage, CPoint() );
}

void CPreviewView::OnUpdateNextPage( CCmdUI *pCmdUI )
/***************************************************/
{
    pCmdUI->Enable( m_nCurrentPage + m_nPages <= m_pPreviewInfo->GetMaxPage() );
}

void CPreviewView::OnUpdateNumPageChange( CCmdUI *pCmdUI )
/********************************************************/
{
    CString strText;
    ASSERT( m_nPages == 1 || m_nPages == 2 );
    if( m_nPages == 2 ) {
        strText.LoadString( AFX_IDS_ONEPAGE );
    } else {
        strText.LoadString( AFX_IDS_TWOPAGE );
    }
    pCmdUI->SetText( strText );
    pCmdUI->Enable( m_nZoomState == ZOOM_OUT );
}

void CPreviewView::OnUpdatePrevPage( CCmdUI *pCmdUI )
/***************************************************/
{
    pCmdUI->Enable( m_nCurrentPage > m_nPages );
}

void CPreviewView::OnUpdateZoomIn( CCmdUI *pCmdUI )
/*************************************************/
{
    pCmdUI->Enable( m_nZoomState == ZOOM_OUT || m_nZoomState == ZOOM_MIDDLE );
}

void CPreviewView::OnUpdateZoomOut( CCmdUI *pCmdUI )
/**************************************************/
{
    pCmdUI->Enable( m_nZoomState == ZOOM_IN || m_nZoomState == ZOOM_MIDDLE );
}
