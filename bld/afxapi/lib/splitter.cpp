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
* Description:  Implementation of CSplitterWnd.
*
****************************************************************************/


#include "stdafx.h"

// Hit test values
#define HT_NOHIT            0
#define HT_VSPLITBOX        1
#define HT_HSPLITBOX        2
#define HT_VSPLITBAR_FIRST  101
#define HT_VSPLITBAR_LAST   115
#define HT_HSPLITBAR_FIRST  201
#define HT_HSPLITBAR_LAST   215

IMPLEMENT_DYNAMIC( CSplitterWnd, CWnd )

BEGIN_MESSAGE_MAP( CSplitterWnd, CWnd )
    ON_WM_HSCROLL()
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()
    ON_WM_SETCURSOR()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

CSplitterWnd::CSplitterWnd()
/**************************/
{
    m_pDynamicViewClass = NULL;
    m_cxSplitter = 7;
    m_cySplitter = 7;
    m_pColInfo = NULL;
    m_pRowInfo = NULL;
    m_bTracking = FALSE;
    m_htTrack = HT_NOHIT;
}

void CSplitterWnd::ActivateNext( BOOL bPrev )
/*******************************************/
{
    int row;
    int col;
    VERIFY( GetActivePane( &row, &col ) != NULL );
    if( !bPrev ) {
        col++;
        if( col >= m_nCols ) {
            col = 0;
            row++;
            if( row >= m_nRows ) {
                row = 0;
            }
        }
    } else {
        col--;
        if( col < 0 ) {
            col = m_nCols - 1;
            row--;
            if( row < 0 ) {
                row = m_nRows - 1;
            }
        }
    }
    SetActivePane( row, col );
}

BOOL CSplitterWnd::CanActivateNext( BOOL bPrev )
/**********************************************/
{
    UNUSED_ALWAYS( bPrev );
    return( m_nRows > 1 || m_nCols > 1 );
}

BOOL CSplitterWnd::Create( CWnd *pParentWnd, int nMaxRows, int nMaxCols, SIZE sizeMin,
                           CCreateContext *pContext, DWORD dwStyle, UINT nID )
/****************************************************************************/
{
    ASSERT( nMaxRows == 1 || nMaxRows == 2 );
    ASSERT( nMaxCols == 1 || nMaxCols == 2 );
    
    m_nMaxRows = nMaxRows;
    m_nMaxCols = nMaxCols;
    m_nRows = 1;
    m_nCols = 1;

    if( !CWnd::Create( _T("AfxMDIFrame"), NULL, dwStyle & ~(WS_HSCROLL | WS_VSCROLL),
                       CRect( 0, 0, 0, 0 ), pParentWnd, nID ) ) {
        return( FALSE );
    }

    m_pRowInfo = new CRowColInfo[m_nMaxRows];
    for( int i = 0; i < m_nMaxRows; i++ ) {
        m_pRowInfo[i].nMinSize = sizeMin.cy;
        m_pRowInfo[i].nIdealSize = sizeMin.cy;
        m_pRowInfo[i].nCurSize = -1;
    }

    m_pColInfo = new CRowColInfo[m_nMaxCols];
    for( int i = 0; i < m_nMaxCols; i++ ) {
        m_pColInfo[i].nMinSize = sizeMin.cx;
        m_pColInfo[i].nIdealSize = sizeMin.cx;
        m_pColInfo[i].nCurSize = -1;
    }

    SetScrollStyle( dwStyle );

    ASSERT( pContext != NULL );
    ASSERT( pContext->m_pNewViewClass != NULL );
    m_pDynamicViewClass = pContext->m_pNewViewClass;
    if( !CreateView( 0, 0, m_pDynamicViewClass, CSize( 0, 0 ), pContext ) ) {
        DestroyWindow();
        return( FALSE );
    }
    
    return( TRUE );
}

BOOL CSplitterWnd::CreateScrollBarCtrl( DWORD dwStyle, UINT nID )
/***************************************************************/
{
    return( ::CreateWindowEx( 0L, _T("SCROLLBAR"), NULL,
                              WS_CHILD | WS_VISIBLE | dwStyle, 0, 0, 0, 0, m_hWnd,
                              (HMENU)nID, AfxGetInstanceHandle(), NULL ) != NULL );
}

BOOL CSplitterWnd::CreateStatic( CWnd *pParentWnd, int nRows, int nCols,
                                 DWORD dwStyle, UINT nID )
/********************************************************/
{
    m_nMaxRows = nRows;
    m_nMaxCols = nCols;
    m_nRows = nRows;
    m_nCols = nCols;

    if( !CWnd::Create( _T("AfxMDIFrame"), NULL, dwStyle & ~(WS_HSCROLL | WS_VSCROLL),
                       CRect( 0, 0, 0, 0 ), pParentWnd, nID ) ) {
        return( FALSE );
    }

    m_pRowInfo = new CRowColInfo[m_nMaxRows];
    for( int i = 0; i < m_nMaxRows; i++ ) {
        m_pRowInfo[i].nMinSize = 0;
        m_pRowInfo[i].nIdealSize = 0;
        m_pRowInfo[i].nCurSize = -1;
    }

    m_pColInfo = new CRowColInfo[m_nMaxCols];
    for( int i = 0; i < m_nMaxCols; i++ ) {
        m_pColInfo[i].nMinSize = 0;
        m_pColInfo[i].nIdealSize = 0;
        m_pColInfo[i].nCurSize = -1;
    }

    SetScrollStyle( dwStyle );

    return( TRUE );
}

BOOL CSplitterWnd::CreateView( int row, int col, CRuntimeClass *pViewClass,
                               SIZE sizeInit, CCreateContext *pContext )
/**********************************************************************/
{
    ASSERT( pViewClass != NULL );

    CWnd *pWnd = (CWnd *)pViewClass->CreateObject();
    if( pWnd == NULL ) {
        return( FALSE );
    }
    ASSERT( pWnd->IsKindOf( RUNTIME_CLASS( CWnd ) ) );

    BOOL bOwnContext = FALSE;
    if( pContext == NULL ) {
        CView *pPane = (CView *)GetActivePane();
        if( pPane != NULL && pPane->IsKindOf( RUNTIME_CLASS( CView ) ) ) {
            bOwnContext = TRUE;
            pContext = new CCreateContext;
            pContext->m_pNewViewClass = pViewClass;
            pContext->m_pCurrentDoc = pPane->GetDocument();
            ASSERT( pContext->m_pCurrentDoc != NULL );
            pContext->m_pNewDocTemplate = pContext->m_pCurrentDoc->GetDocTemplate();
            pContext->m_pLastView = pPane;
            pContext->m_pCurrentFrame = GetParentFrame();
        }
    }

    m_pRowInfo[row].nIdealSize = sizeInit.cy;
    m_pColInfo[col].nIdealSize = sizeInit.cx;
    
    UINT nID = IdFromRowCol( row, col );
    BOOL bCreated = pWnd->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW,
                                  CRect( 0, 0, sizeInit.cx, sizeInit.cy ), this, nID,
                                  pContext );
    if( bOwnContext ) {
        delete pContext;
        if( bCreated ) {
            pWnd->SendMessage( WM_INITIALUPDATE );
        }
    }
    return( bCreated );
}

void CSplitterWnd::DeleteColumn( int colDelete )
/**********************************************/
{
    ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
    ASSERT( m_nMaxCols == 2 );
    ASSERT( m_nCols == 2 );
    ASSERT( colDelete == 0 || colDelete == 1 );

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    CView *pView = pFrame->GetActiveView();
    if( pView != NULL ) {
        UINT nID = pView->GetDlgCtrlID();
        if( nID >= AFX_IDW_PANE_FIRST && nID <= AFX_IDW_PANE_LAST ) {
            int row = (nID - AFX_IDW_PANE_FIRST) / 16;
            int col = (nID - AFX_IDW_PANE_FIRST) % 16;
            if( col == colDelete ) {
                col = (col + 1) % m_nCols;
                pView = (CView *)GetDlgItem( IdFromRowCol( row, col ) );
                ASSERT( pView != NULL );
                ASSERT( pView->IsKindOf( RUNTIME_CLASS( CView ) ) );
                pFrame->SetActiveView( pView );
            }
        }
    }

    for( int i = 0; i < m_nRows; i++ ) {
        DeleteView( i, colDelete );
    }
    if( m_bHasHScroll ) {
        HWND hWnd = ::GetDlgItem( m_hWnd, AFX_IDW_HSCROLL_FIRST + colDelete );
        ASSERT( hWnd != NULL );
        ::DestroyWindow( hWnd );
    }
    if( colDelete == 0 ) {
        for( int i = 0; i < m_nRows; i++ ) {
            CWnd *pWnd = GetDlgItem( IdFromRowCol( i, 1 ) );
            ASSERT( pWnd != NULL );
            pWnd->SetDlgCtrlID( IdFromRowCol( i, 0 ) );
        }
        CWnd *pWnd = GetDlgItem( AFX_IDW_HSCROLL_FIRST + 1 );
        ASSERT( pWnd != NULL );
        pWnd->SetDlgCtrlID( AFX_IDW_HSCROLL_FIRST );
    }
    m_nCols = 1;
    RecalcLayout();
}

void CSplitterWnd::DeleteRow( int rowDelete )
/*******************************************/
{
    ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
    ASSERT( m_nMaxRows == 2 );
    ASSERT( m_nRows == 2 );
    ASSERT( rowDelete == 0 || rowDelete == 1 );

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    CView *pView = pFrame->GetActiveView();
    if( pView != NULL ) {
        UINT nID = pView->GetDlgCtrlID();
        if( nID >= AFX_IDW_PANE_FIRST && nID <= AFX_IDW_PANE_LAST ) {
            int row = (nID - AFX_IDW_PANE_FIRST) / 16;
            int col = (nID - AFX_IDW_PANE_FIRST) % 16;
            if( row == rowDelete ) {
                row = (row + 1) % m_nRows;
                pView = (CView *)GetDlgItem( IdFromRowCol( row, col ) );
                ASSERT( pView != NULL );
                ASSERT( pView->IsKindOf( RUNTIME_CLASS( CView ) ) );
                pFrame->SetActiveView( pView );
            }
        }
    }
    
    for( int i = 0; i < m_nCols; i++ ) {
        DeleteView( rowDelete, i );
    }
    if( m_bHasVScroll ) {
        HWND hWnd = ::GetDlgItem( m_hWnd, AFX_IDW_VSCROLL_FIRST + rowDelete );
        ASSERT( hWnd != NULL );
        ::DestroyWindow( hWnd );
    }
    if( rowDelete == 0 ) {
        for( int i = 0; i < m_nCols; i++ ) {
            CWnd *pWnd = GetDlgItem( IdFromRowCol( 1, i ) );
            ASSERT( pWnd != NULL );
            pWnd->SetDlgCtrlID( IdFromRowCol( 0, i ) );
        }
        CWnd *pWnd = GetDlgItem( AFX_IDW_VSCROLL_FIRST + 1 );
        ASSERT( pWnd != NULL );
        pWnd->SetDlgCtrlID( AFX_IDW_VSCROLL_FIRST );
    }

    m_nRows = 1;
    RecalcLayout();
}

void CSplitterWnd::DeleteView( int row, int col )
/***********************************************/
{
    ASSERT( row >= 0 && row < m_nRows );
    ASSERT( col >= 0 && col < m_nCols );

    CWnd *pWnd = GetDlgItem( IdFromRowCol( row, col ) );
    ASSERT( pWnd != NULL );
    pWnd->DestroyWindow();
}

BOOL CSplitterWnd::DoKeyboardSplit()
/**********************************/
{
    int ht = HT_NOHIT;
    if( m_nRows < m_nMaxRows ) {
        ht = HT_VSPLITBOX;
    } else if( m_nRows > 1 ) {
        ht = HT_VSPLITBAR_FIRST;
    } else if( m_nCols < m_nMaxCols ) {
        ht = HT_HSPLITBOX;
    } else if( m_nCols > 1 ) {
        ht = HT_HSPLITBAR_FIRST;
    } else {
        return( FALSE );
    }
    StartTracking( ht );

    CPoint point = m_rectTracker.CenterPoint();
    ::ClientToScreen( m_hWnd, &point );
    ::SetCursorPos( point.x, point.y );
    
    return( TRUE );
}

CWnd *CSplitterWnd::GetActivePane( int *pRow, int *pCol )
/*******************************************************/
{
    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );

    CView *pView = pFrame->GetActiveView();
    if( pView == NULL ) {
        return( NULL );
    }

    int nID = pView->GetDlgCtrlID();
    int row = (nID - AFX_IDW_PANE_FIRST) / 16;
    int col = (nID - AFX_IDW_PANE_FIRST) % 16;
    if( row < 0 || row >= 16 ) {
        // It's not a pane of the splitter.
        return( NULL );
    }
    if( pRow != NULL ) {
        *pRow = row;
    }
    if( pCol != NULL ) {
        *pCol = col;
    }
    return( pView );
}

int CSplitterWnd::HitTest( CPoint pt ) const
/******************************************/
{
    int cxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );
    int cyHScroll = ::GetSystemMetrics( SM_CYHSCROLL );

    CRect rectClient;
    ::GetClientRect( m_hWnd, &rectClient );
    
    if( m_nRows < m_nMaxRows ) {
        CRect rectBox( rectClient.right - cxVScroll, rectClient.top,
                       rectClient.right, rectClient.top + m_cySplitter );
        if( rectBox.PtInRect( pt ) ) {
            return( HT_VSPLITBOX );
        }
    } else if( m_nRows > 1 ) {
        CRect rectBar( rectClient.left, 0, rectClient.right, 0 );
        for( int i = 0; i < m_nRows - 1; i++ ) {
            rectBar.top = rectBar.bottom + m_pRowInfo[i].nCurSize;
            rectBar.bottom = rectBar.top + m_cySplitter;
            if( rectBar.PtInRect( pt ) ) {
                return( HT_VSPLITBAR_FIRST + i );
            }
        }
    }
    
    if( m_nCols < m_nMaxCols ) {
        CRect rectBox( rectClient.left, rectClient.bottom - cyHScroll,
                       rectClient.left + m_cxSplitter, rectClient.bottom );
        if( rectBox.PtInRect( pt ) ) {
            return( HT_HSPLITBOX );
        }
    } else if( m_nCols > 1 ) {
        CRect rectBar( 0, rectClient.top, 0, rectClient.bottom );
        for( int i = 0; i < m_nCols - 1; i++ ) {
            rectBar.left = rectBar.right + m_pColInfo[i].nCurSize;
            rectBar.right = rectBar.left + m_cxSplitter;
            if( rectBar.PtInRect( pt ) ) {
                return( HT_HSPLITBAR_FIRST + i );
            }
        }
    }
    return( HT_NOHIT );
}

void CSplitterWnd::OnDrawSplitter( CDC *pDC, ESplitType nType, const CRect &rect )
/********************************************************************************/
{
    ASSERT( pDC != NULL );
    
    CRect       rect2 = rect;
    COLORREF    clrBtnShadow = ::GetSysColor( COLOR_BTNSHADOW );
    COLORREF    clrBtnHighlight = ::GetSysColor( COLOR_BTNHIGHLIGHT );
    COLORREF    clrWindowFrame = ::GetSysColor( COLOR_WINDOWFRAME );
    COLORREF    clrBtnFace = ::GetSysColor( COLOR_BTNFACE );
    
    if( nType == splitBox ) {
        pDC->Draw3dRect( &rect2, clrBtnFace, clrWindowFrame );
        rect2.DeflateRect( 1, 1 );
        pDC->Draw3dRect( &rect2, clrBtnHighlight, clrBtnShadow );
        rect2.DeflateRect( 1, 1 );
        pDC->FillSolidRect( &rect2, clrBtnFace );
    } else if( nType == splitBar ) {
        pDC->FillSolidRect( &rect2, clrBtnFace );
    }
}

void CSplitterWnd::OnInvertTracker( const CRect &rect )
/*****************************************************/
{
    CClientDC   dc( this );
    CBrush      br( RGB( 192, 192, 192 ) );
    CBrush      *pOldBrush = dc.SelectObject( &br );
    dc.PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT );
    dc.SelectObject( pOldBrush );
}

void CSplitterWnd::RecalcLayout()
/*******************************/
{
    if( m_pColInfo == NULL || m_pRowInfo == NULL ) {
        return;
    }
    
    HDWP hDWP = ::BeginDeferWindowPos( (m_nRows + 1) * (m_nCols + 1) );

    int cyHScroll = ::GetSystemMetrics( SM_CYHSCROLL );
    int cxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );
    
    CRect rectClient;
    ::GetClientRect( m_hWnd, &rectClient );

    CRect rect = rectClient;
    if( m_bHasHScroll ) {
        rect.bottom -= cyHScroll;
    }
    if( m_bHasVScroll ) {
        rect.right -= cxVScroll;
    }

    // Calculate the row heights and column widths.
    for( int i = 0; i < m_nRows; i++ ) {
        if( m_pRowInfo[i].nCurSize == -1 ) {
            m_pRowInfo[i].nCurSize = m_pRowInfo[i].nIdealSize;
        }
        if( m_pRowInfo[i].nCurSize > rect.Height() || i == m_nRows - 1 ) {
            m_pRowInfo[i].nCurSize = rect.Height();
        }
        rect.top += m_pRowInfo[i].nCurSize + m_cySplitter;
    }
    for( int i = 0; i < m_nCols; i++ ) {
        if( m_pColInfo[i].nCurSize == -1 ) {
            m_pColInfo[i].nCurSize = m_pColInfo[i].nIdealSize;
        }
        if( m_pColInfo[i].nCurSize > rect.Width() || i == m_nCols - 1 ) {
            m_pColInfo[i].nCurSize = rect.Width();
        }
        rect.left += m_pColInfo[i].nCurSize + m_cxSplitter;
    }

    // Move the views into place.
    rect.top = 0;
    for( int i = 0; i < m_nRows; i++ ) {
        rect.left = 0;
        for( int j = 0; j < m_nCols; j++ ) {
            HWND hWndPane = ::GetDlgItem( m_hWnd, IdFromRowCol( i, j ) );
            ASSERT( hWndPane != NULL );

            CRect rectPane( rect.left, rect.top, rect.left + m_pColInfo[j].nCurSize,
                            rect.top + m_pRowInfo[i].nCurSize );
            ::DeferWindowPos( hDWP, hWndPane, NULL, rectPane.left, rectPane.top,
                              rectPane.Width(), rectPane.Height(),
                              SWP_NOACTIVATE | SWP_NOZORDER );
            rect.left += m_pColInfo[j].nCurSize + m_cxSplitter;
        }
        rect.top += m_pRowInfo[i].nCurSize + m_cySplitter;
    }

    // Move the horiztonal scroll bars into place.
    if( m_bHasHScroll ) {
        rect = rectClient;
        for( int i = 0; i < m_nCols; i++ ) {
            CRect rectBar( rect.left, rect.bottom - cyHScroll,
                           rect.left + m_pColInfo[i].nCurSize, rect.bottom );
            if( i == 0 && m_nCols < m_nMaxCols ) {
                // Leave space for the split box.
                rect.left += m_cxSplitter;
                rectBar.left += m_cxSplitter;
            }

            HWND hWndBar = ::GetDlgItem( m_hWnd, AFX_IDW_HSCROLL_FIRST + i );
            ::DeferWindowPos( hDWP, hWndBar, NULL, rectBar.left, rectBar.top,
                              rectBar.Width(), rectBar.Height(),
                              SWP_NOACTIVATE | SWP_NOZORDER );
            rect.left += m_pColInfo[i].nCurSize + m_cxSplitter;
        }
    }

    // Move the vertical scroll bars into place.
    if( m_bHasVScroll ) {
        rect = rectClient;
        for( int i = 0; i < m_nRows; i++ ) {
            CRect rectBar( rect.right - cxVScroll, rect.top, rect.right,
                           rect.top + m_pRowInfo[i].nCurSize );
            if( i == 0 && m_nRows < m_nMaxRows ) {
                // Leave space for the split box.
                rect.top += m_cySplitter;
                rectBar.top += m_cySplitter;
            }

            HWND hWndBar = ::GetDlgItem( m_hWnd, AFX_IDW_VSCROLL_FIRST + i );
            ::DeferWindowPos( hDWP, hWndBar, NULL, rectBar.left, rectBar.top,
                              rectBar.Width(), rectBar.Height(),
                              SWP_NOACTIVATE | SWP_NOZORDER );
            rect.top += m_pRowInfo[i].nCurSize + m_cySplitter;
        }
    }

    // Move the size box into place.
    if( m_bHasHScroll && m_bHasVScroll ) {
        CRect rectBox( rectClient.right - cxVScroll, rectClient.bottom - cyHScroll,
                       rectClient.right, rectClient.bottom );

        HWND hWndBox = ::GetDlgItem( m_hWnd, AFX_IDW_SIZE_BOX );
        ::DeferWindowPos( hDWP, hWndBox, NULL, rectBox.left, rectBox.top,
                          rectBox.Width(), rectBox.Height(),
                          SWP_NOACTIVATE | SWP_NOZORDER );
    }

    ::EndDeferWindowPos( hDWP );
}

void CSplitterWnd::SetSplitCursor( int ht )
/*****************************************/
{
    static HCURSOR hcurHorz = NULL;
    static HCURSOR hcurVert = NULL;
    if( ht == HT_HSPLITBOX || (ht >= HT_HSPLITBAR_FIRST && ht <= HT_HSPLITBAR_LAST) ) {
        if( hcurHorz == NULL ) {
            CWinApp *pApp = AfxGetApp();
            ASSERT( pApp != NULL );
            hcurHorz = pApp->LoadStandardCursor( IDC_SIZEWE );
        }
        ::SetCursor( hcurHorz );
    } else if( ht == HT_VSPLITBOX ||
               (ht >= HT_VSPLITBAR_FIRST && ht <= HT_VSPLITBAR_LAST) ) {
        if( hcurVert == NULL ) {
            CWinApp *pApp = AfxGetApp();
            ASSERT( pApp != NULL );
            hcurVert = pApp->LoadStandardCursor( IDC_SIZENS );
        }
        ::SetCursor( hcurVert );
    }
}

BOOL CSplitterWnd::SplitColumn( int cxBefore )
/********************************************/
{
    ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
    ASSERT( m_nCols == 1 || m_nCols == 2 );
    ASSERT( m_nMaxCols == 1 || m_nMaxCols == 2 );

    // Check that a split can be performed.
    if( m_nCols >= 2 || m_nMaxCols == 1 ||
        cxBefore < 0 || cxBefore < m_pColInfo[0].nMinSize ||
        m_pColInfo[0].nCurSize - (m_cxSplitter + cxBefore) < m_pColInfo[0].nMinSize ) {
        return( FALSE );
    }

    // Move the old scroll bar to the left column and create a new scroll bar for the
    // right column.
    CWnd *pWnd = GetDlgItem( AFX_IDW_HSCROLL_FIRST );
    ASSERT( pWnd != NULL );
    pWnd->SetDlgCtrlID( AFX_IDW_HSCROLL_FIRST + 1 );
    if( !CreateScrollBarCtrl( SBS_HORZ, AFX_IDW_HSCROLL_FIRST ) ) {
        return( FALSE );
    }

    // Place the old views in the right column and create new views for the left column.
    for( int i = 0; i < m_nRows; i++ ) {
        CWnd *pWnd = GetDlgItem( IdFromRowCol( i, 0 ) );
        ASSERT( pWnd != NULL );
        pWnd->SetDlgCtrlID( IdFromRowCol( i, 1 ) );
        if( !CreateView( i, 0, m_pDynamicViewClass, CSize( 0, 0 ), NULL ) ) {
            // If view creation failed, undo everything done up to this point.
            pWnd->SetDlgCtrlID( IdFromRowCol( i, 0 ) );
            for( i--; i >= 0; i-- ) {
                pWnd = GetDlgItem( IdFromRowCol( i, 0 ) );
                ASSERT( pWnd != NULL );
                pWnd->DestroyWindow();
                pWnd = GetDlgItem( IdFromRowCol( i, 1 ) );
                pWnd->SetDlgCtrlID( IdFromRowCol( i, 0 ) );
            }
            return( FALSE );
        }
    }

    // Update the column information for the split.
    m_pColInfo[1].nIdealSize = m_pColInfo[0].nIdealSize;
    m_pColInfo[1].nMinSize = m_pColInfo[0].nMinSize;
    m_pColInfo[1].nCurSize = -1;
    m_pColInfo[0].nCurSize = cxBefore;
    m_nCols = 2;

    // Move everything into place.
    RecalcLayout();
    
    return( TRUE );
}

BOOL CSplitterWnd::SplitRow( int cyBefore )
/*****************************************/
{
    ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
    ASSERT( m_nRows == 1 || m_nRows == 2 );
    ASSERT( m_nMaxRows == 1 || m_nMaxRows == 2 );

    // Check that a split can be performed.
    if( m_nRows >= 2 || m_nMaxRows == 1 ||
        cyBefore < 0 || cyBefore < m_pRowInfo[0].nMinSize ||
        m_pRowInfo[0].nCurSize - (m_cySplitter + cyBefore) < m_pRowInfo[0].nMinSize ) {
        return( FALSE );
    }

    // Move the old scroll bar to the bottom row and create a new scroll bar for the top
    // row.
    CWnd *pWnd = GetDlgItem( AFX_IDW_VSCROLL_FIRST );
    ASSERT( pWnd != NULL );
    pWnd->SetDlgCtrlID( AFX_IDW_VSCROLL_FIRST + 1 );
    if( !CreateScrollBarCtrl( SBS_VERT, AFX_IDW_VSCROLL_FIRST ) ) {
        return( FALSE );
    }

    // Place the old views in the bottom row and create new views for the top row.
    for( int i = 0; i < m_nCols; i++ ) {
        CWnd *pWnd = GetDlgItem( IdFromRowCol( 0, i ) );
        ASSERT( pWnd != NULL );
        pWnd->SetDlgCtrlID( IdFromRowCol( 1, i ) );
        if( !CreateView( 0, i, m_pDynamicViewClass, CSize( 0, 0 ), NULL ) ) {
            // If view creation failed, undo everything done up to this point.
            pWnd->SetDlgCtrlID( IdFromRowCol( 0, i ) );
            for( i--; i >= 0; i-- ) {
                pWnd = GetDlgItem( IdFromRowCol( 0, i ) );
                ASSERT( pWnd != NULL );
                pWnd->DestroyWindow();
                pWnd = GetDlgItem( IdFromRowCol( 1, i ) );
                pWnd->SetDlgCtrlID( IdFromRowCol( 0, i ) );
            }
            return( FALSE );
        }
    }

    // Update the row information for the split.
    m_pRowInfo[1].nIdealSize = m_pRowInfo[0].nIdealSize;
    m_pRowInfo[1].nMinSize = m_pRowInfo[0].nMinSize;
    m_pRowInfo[1].nCurSize = -1;
    m_pRowInfo[0].nCurSize = cyBefore;
    m_nRows = 2;

    // Move everything into place.
    RecalcLayout();
    ::InvalidateRect( m_hWnd, NULL, TRUE );
    ::UpdateWindow( m_hWnd );
    
    return( TRUE );
}

void CSplitterWnd::StartTracking( int ht )
/****************************************/
{
    if( m_bTracking || ht == HT_NOHIT ) {
        return;
    }
    
    int cxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );
    int cyHScroll = ::GetSystemMetrics( SM_CYHSCROLL );

    CRect rectClient;
    ::GetClientRect( m_hWnd, &rectClient );
    
    if( ht == HT_VSPLITBOX ) {
        m_rectLimit = rectClient;
        if( m_bHasHScroll ) {
            m_rectLimit.bottom -= cyHScroll;
        }
        m_rectTracker = rectClient;
        m_rectTracker.bottom = m_rectTracker.top + m_cySplitter;
    } else if( ht >= HT_VSPLITBAR_FIRST && ht <= HT_VSPLITBAR_LAST ) {
        int n = ht - HT_VSPLITBAR_FIRST;
        m_rectTracker.SetRect( rectClient.left, 0, rectClient.right, 0 );
        for( int i = 0; i <= n; i++ ) {
            m_rectTracker.top = m_rectTracker.bottom + m_pRowInfo[i].nCurSize;
            m_rectTracker.bottom = m_rectTracker.top + m_cySplitter;
        }
        m_rectLimit = m_rectTracker;
        m_rectLimit.top -= m_pRowInfo[n].nCurSize;
        m_rectLimit.bottom += m_pRowInfo[n + 1].nCurSize;
    } else if( ht == HT_HSPLITBOX ) {
        m_rectLimit = rectClient;
        if( m_bHasVScroll ) {
            m_rectLimit.right - cxVScroll;
        }
        m_rectTracker = rectClient;
        m_rectTracker.right = m_rectTracker.left + m_cxSplitter;
    } else if( ht >= HT_HSPLITBAR_FIRST && ht <= HT_HSPLITBAR_LAST ) {
        int n = ht - HT_HSPLITBAR_FIRST;
        m_rectTracker.SetRect( 0, rectClient.top, 0, rectClient.bottom );
        for( int i = 0; i <= n; i++ ) {
            m_rectTracker.left = m_rectTracker.right + m_pColInfo[i].nCurSize;
            m_rectTracker.right = m_rectTracker.left + m_cxSplitter;
        }
        m_rectLimit = m_rectTracker;
        m_rectLimit.left -= m_pColInfo[n].nCurSize;
        m_rectLimit.right += m_pColInfo[n + 1].nCurSize;
    }

    m_htTrack = ht;
    m_bTracking = TRUE;
    ::SetCapture( m_hWnd );
    ::SetFocus( m_hWnd );
    ::RedrawWindow( m_hWnd, NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW );
    OnInvertTracker( m_rectTracker );
    SetSplitCursor( ht );
}

void CSplitterWnd::StopTracking( BOOL bAccept )
/*********************************************/
{
    if( !m_bTracking ) {
        return;
    }

    m_bTracking = FALSE;
    OnInvertTracker( m_rectTracker );
    ::ReleaseCapture();
    if( bAccept ) {
        if( m_htTrack == HT_VSPLITBOX ) {
            SplitRow( m_rectTracker.top - m_rectLimit.top );
        } else if( m_htTrack == HT_HSPLITBOX ) {
            SplitColumn( m_rectTracker.left - m_rectLimit.left );
        } else if( m_htTrack >= HT_VSPLITBAR_FIRST && m_htTrack <= HT_VSPLITBAR_LAST ) {
            int i = m_htTrack - HT_VSPLITBAR_FIRST;
            ASSERT( i < m_nRows );
            m_pRowInfo[i].nCurSize = m_rectTracker.top - m_rectLimit.top;
            m_pRowInfo[i + 1].nCurSize = m_rectLimit.bottom - m_rectTracker.bottom;
            if( !(GetStyle() & SPLS_DYNAMIC_SPLIT) ) {
                if( m_pRowInfo[i].nCurSize < m_pRowInfo[i].nMinSize ) {
                    m_pRowInfo[i].nCurSize = m_pRowInfo[i].nMinSize;
                }
                if( m_pRowInfo[i + 1].nCurSize < m_pRowInfo[i + 1].nMinSize ) {
                    m_pRowInfo[i + 1].nCurSize = m_pRowInfo[i + 1].nMinSize;
                }
                RecalcLayout();
            } else {
                if( m_pRowInfo[i].nCurSize < m_pRowInfo[i].nMinSize ) {
                    DeleteRow( i );
                } else if( m_pRowInfo[i + 1].nCurSize < m_pRowInfo[i + 1].nMinSize ) {
                    DeleteRow( i + 1 );
                } else {
                    RecalcLayout();
                }
            }
        } else if( m_htTrack >= HT_HSPLITBAR_FIRST && m_htTrack <= HT_HSPLITBAR_LAST ) {
            int i = m_htTrack - HT_HSPLITBAR_FIRST;
            ASSERT( i < m_nCols );
            m_pColInfo[i].nCurSize = m_rectTracker.left - m_rectLimit.left;
            m_pColInfo[i + 1].nCurSize = m_rectLimit.right - m_rectTracker.right;
            if( !(GetStyle() & SPLS_DYNAMIC_SPLIT) ) {
                if( m_pColInfo[i].nCurSize < m_pColInfo[i].nMinSize ) {
                    m_pColInfo[i].nCurSize = m_pColInfo[i].nMinSize;
                }
                if( m_pColInfo[i + 1].nCurSize < m_pColInfo[i + 1].nMinSize ) {
                    m_pColInfo[i + 1].nCurSize = m_pColInfo[i + 1].nMinSize;
                }
                RecalcLayout();
            } else {
                if( m_pColInfo[i].nCurSize < m_pColInfo[i].nMinSize ) {
                    DeleteColumn( i );
                } else if( m_pColInfo[i + 1].nCurSize < m_pColInfo[i].nMinSize ) {
                    DeleteColumn( i + 1 );
                } else {
                    RecalcLayout();
                }
            }
        }
        ::UpdateWindow( m_hWnd );
        m_htTrack = HT_NOHIT;
    }
}

DWORD CSplitterWnd::GetScrollStyle() const
/****************************************/
{
    DWORD dwStyle = 0L;
    if( m_bHasHScroll ) {
        dwStyle |= WS_HSCROLL;
    }
    if( m_bHasVScroll ) {
        dwStyle |= WS_VSCROLL;
    }
    return( dwStyle );
}

BOOL CSplitterWnd::IsChildPane( CWnd *pWnd, int *pRow, int *pCol )
/****************************************************************/
{
    if( pWnd == NULL || pWnd->GetParent() != this ) {
        return( FALSE );
    }

    UINT nID = pWnd->GetDlgCtrlID();
    if( nID < AFX_IDW_PANE_FIRST || nID > AFX_IDW_PANE_LAST ) {
        return( FALSE );
    }

    if( pRow != NULL ) {
        *pRow = (nID - AFX_IDW_PANE_FIRST) / 16;
    }
    if( pCol != NULL ) {
        *pCol = (nID - AFX_IDW_PANE_FIRST) % 16;
    }
    return( TRUE );
}

void CSplitterWnd::SetActivePane( int row, int col, CWnd *pWnd )
/**************************************************************/
{
    if( pWnd == NULL ) {
        pWnd = GetDlgItem( IdFromRowCol( row, col ) );
    }
    if( pWnd != NULL && pWnd->IsKindOf( RUNTIME_CLASS( CView ) ) ) {
        CFrameWnd *pFrame = GetParentFrame();
        ASSERT( pFrame != NULL );
        pFrame->SetActiveView( (CView *)pWnd );
    }
}

void CSplitterWnd::SetScrollStyle( DWORD dwStyle )
/************************************************/
{
    m_bHasHScroll = (dwStyle & WS_HSCROLL);
    m_bHasVScroll = (dwStyle & WS_VSCROLL);

    for( int i = 0; i < m_nCols; i++ ) {
        CWnd *pWnd = GetDlgItem( AFX_IDW_HSCROLL_FIRST + i );
        if( pWnd == NULL ) {
            if( !CreateScrollBarCtrl( SBS_HORZ, AFX_IDW_HSCROLL_FIRST + i ) ) {
                throw new CResourceException;
            }
            pWnd = GetDlgItem( AFX_IDW_HSCROLL_FIRST + i );
        }
        ASSERT( pWnd != NULL );
        pWnd->ShowWindow( m_bHasHScroll ? SW_SHOW : SW_HIDE );
    }

    for( int i = 0; i < m_nRows; i++ ) {
        CWnd *pWnd = GetDlgItem( AFX_IDW_VSCROLL_FIRST + i );
        if( pWnd == NULL ) {
            if( !CreateScrollBarCtrl( SBS_VERT, AFX_IDW_VSCROLL_FIRST + i ) ) {
                throw new CResourceException;
            }
            pWnd = GetDlgItem( AFX_IDW_VSCROLL_FIRST + i );
        }
        ASSERT( pWnd != NULL );
        pWnd->ShowWindow( m_bHasVScroll ? SW_SHOW : SW_HIDE );
    }

    CWnd *pWnd = GetDlgItem( AFX_IDW_SIZE_BOX );
    if( m_bHasHScroll && m_bHasVScroll ) {
        if( pWnd == NULL ) {
            if( !CreateScrollBarCtrl( SBS_SIZEBOX | WS_DISABLED, AFX_IDW_SIZE_BOX ) ) {
                throw new CResourceException;
            }
        }
    } else {
        if( pWnd != NULL ) {
            pWnd->DestroyWindow();
        }
    }
}

void CSplitterWnd::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
/*****************************************************************************/
{
    ASSERT( pScrollBar != NULL );
    
    UINT    nID = pScrollBar->GetDlgCtrlID();
    int     col = nID - AFX_IDW_HSCROLL_FIRST;
    UINT    nOldPos = pScrollBar->GetScrollPos();

    for( int i = 0; i < m_nRows; i++ ) {
        CWnd *pPane = GetDlgItem( IdFromRowCol( i, col ) );
        ASSERT( pPane != NULL );
        pPane->SendMessage( WM_HSCROLL, MAKEWPARAM( nSBCode, nPos ),
                            (LPARAM)pScrollBar->m_hWnd );
        if( i != m_nRows - 1 ) {
            pScrollBar->SetScrollPos( nOldPos );
        }
    }
}

void CSplitterWnd::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
/*******************************************************************/
{
    CWnd::OnKeyDown( nChar, nRepCnt, nFlags );

    if( !m_bTracking ) {
        return;
    }

    if( nChar == VK_ESCAPE || nChar == VK_RETURN ) {
        StopTracking( nChar == VK_RETURN );
    } else if( m_htTrack == HT_VSPLITBOX ||
               (m_htTrack >= HT_VSPLITBAR_FIRST && m_htTrack <= HT_VSPLITBAR_LAST) ) {
        if( nChar == VK_UP || nChar == VK_DOWN ) {
            CPoint point;
            ::GetCursorPos( &point );
            if( nChar == VK_UP ) {
                point.y--;
            } else {
                point.y++;
            }
            ::SetCursorPos( point.x, point.y );
        }
    } else if( m_htTrack == HT_HSPLITBOX ||
               (m_htTrack >= HT_HSPLITBAR_FIRST && m_htTrack <= HT_HSPLITBAR_LAST) ) {
        if( nChar == VK_LEFT || nChar == VK_RIGHT ) {
            CPoint point;
            ::GetCursorPos( &point );
            if( nChar == VK_LEFT ) {
                point.x--;
            } else {
                point.x++;
            }
            ::SetCursorPos( point.x, point.y );
        }
    }
}

void CSplitterWnd::OnLButtonDown( UINT nFlags, CPoint point )
/***********************************************************/
{
    CWnd::OnLButtonDown( nFlags, point );

    StartTracking( HitTest( point ) );
}

void CSplitterWnd::OnLButtonUp( UINT nFlags, CPoint point )
/*********************************************************/
{
    CWnd::OnLButtonUp( nFlags, point );

    if( m_bTracking ) {
        StopTracking( TRUE );
    }
}

void CSplitterWnd::OnMouseMove( UINT nFlags, CPoint point )
/*********************************************************/
{
    CWnd::OnMouseMove( nFlags, point );

    if( m_bTracking ) {
        if( m_htTrack == HT_VSPLITBOX ||
            (m_htTrack >= HT_VSPLITBAR_FIRST && m_htTrack <= HT_VSPLITBAR_LAST) ) {
            int yOld = (m_rectTracker.top + m_rectTracker.bottom) / 2;
            if( point.y < m_rectLimit.top ) {
                point.y = m_rectLimit.top;
            } else if( point.y > m_rectLimit.bottom ) {
                point.y = m_rectLimit.bottom;
            }
            OnInvertTracker( m_rectTracker );
            m_rectTracker.OffsetRect( 0, point.y - yOld );
            OnInvertTracker( m_rectTracker );
        } else if( m_htTrack == HT_HSPLITBOX ||
                   (m_htTrack >= HT_HSPLITBAR_FIRST && m_htTrack <= HT_HSPLITBAR_LAST )) {
            int xOld = (m_rectTracker.left + m_rectTracker.right) / 2;
            if( point.x < m_rectLimit.left ) {
                point.x = m_rectLimit.left;
            } else if( point.x > m_rectLimit.right ) {
                point.x = m_rectLimit.right;
            }
            OnInvertTracker( m_rectTracker );
            m_rectTracker.OffsetRect( point.x - xOld, 0 );
            OnInvertTracker( m_rectTracker );
        }
    }
}

void CSplitterWnd::OnPaint()
/**************************/
{
    CRect rectClient;
    GetClientRect( &rectClient );

    int cxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );
    int cyHScroll = ::GetSystemMetrics( SM_CYHSCROLL );
    
    CPaintDC dc( this );
    if( m_bHasHScroll && m_nCols < m_nMaxCols ) {
        CRect rectBox( rectClient.left, rectClient.bottom - cyHScroll,
                       rectClient.left + m_cxSplitter, rectClient.bottom );
        OnDrawSplitter( &dc, splitBox, &rectBox );
    }
    if( m_bHasVScroll && m_nRows < m_nMaxRows ) {
        CRect rectBox( rectClient.right - cxVScroll, rectClient.top,
                       rectClient.right, rectClient.top + m_cySplitter );
        OnDrawSplitter( &dc, splitBox, &rectBox );
    }

    int nStart = rectClient.top;
    for( int i = 0; i < m_nRows - 1; i++ ) {
        nStart += m_pRowInfo[i].nCurSize;
        CRect rectBar( rectClient.left, nStart, rectClient.right,
                       nStart + m_cySplitter );
        OnDrawSplitter( &dc, splitBar, &rectBar );
        nStart = rectBar.bottom;
    }

    nStart = rectClient.left;
    for( int i = 0; i < m_nCols - 1; i++ ) {
        nStart += m_pColInfo[i].nCurSize;
        CRect rectBar( nStart, rectClient.top, nStart + m_cxSplitter,
                       rectClient.bottom );
        OnDrawSplitter( &dc, splitBar, &rectBar );
        nStart = rectBar.bottom;
    }
}

BOOL CSplitterWnd::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT message )
/***********************************************************************/
{
    int cxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );
    int cyHScroll = ::GetSystemMetrics( SM_CYHSCROLL );

    CPoint point( ::GetMessagePos() );
    ::ScreenToClient( m_hWnd, &point );
    
    int ht = HitTest( point );
    if( ht != HT_NOHIT ) {
        SetSplitCursor( ht );
        return( TRUE );
    }

    return( CWnd::OnSetCursor( pWnd, nHitTest, message ) );
}

void CSplitterWnd::OnSize( UINT nType, int cx, int cy )
/*****************************************************/
{
    CWnd::OnSize( nType, cx, cy );

    RecalcLayout();
}

void CSplitterWnd::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
/*****************************************************************************/
{
    ASSERT( pScrollBar != NULL );

    UINT    nID = pScrollBar->GetDlgCtrlID();
    int     row = nID - AFX_IDW_VSCROLL_FIRST;
    UINT    nOldPos = pScrollBar->GetScrollPos();

    for( int i = 0; i < m_nCols; i++ ) {
        CWnd *pPane = GetDlgItem( IdFromRowCol( row, i ) );
        ASSERT( pPane != NULL );
        pPane->SendMessage( WM_VSCROLL, MAKEWPARAM( nSBCode, nPos ),
                            (LPARAM)pScrollBar->m_hWnd );
        if( i != m_nCols - 1 ) {
            pScrollBar->SetScrollPos( nOldPos );
        }
    }
}
