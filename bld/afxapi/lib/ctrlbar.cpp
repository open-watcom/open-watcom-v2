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
* Description:  Implementation of CControlBar.
*
****************************************************************************/


#include "stdafx.h"

// Identifier of timer to catch when the mouse exits a control bar
#define TIMER_ID    0xE000

// Gripper dimensions
#define GRIPPER_WIDTH   3
#define GRIPPER_BORDER  2

IMPLEMENT_DYNAMIC( CControlBar, CWnd )

BEGIN_MESSAGE_MAP( CControlBar, CWnd )
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONDOWN()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_MESSAGE( WM_HELPHITTEST, OnHelpHitTest )
    ON_MESSAGE( WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI )
    ON_MESSAGE( WM_SIZEPARENT, OnSizeParent )
END_MESSAGE_MAP()

CControlBar::CControlBar()
/************************/
{
    m_bAutoDelete = FALSE;
    m_cxLeftBorder = 0;
    m_cxRightBorder = 0;
    m_cyTopBorder = 0;
    m_cyBottomBorder = 0;
    m_nMRUWidth = 32767;
    m_nCount = 0;
    m_pData = NULL;
    m_dwStyle = 0L;
    m_dwDockStyle = 0L;
    m_pDockSite = NULL;
    m_pDockBar = NULL;
    m_pDockContext = NULL;
}

CSize CControlBar::CalcDynamicLayout( int nLength, DWORD nMode )
/**************************************************************/
{
    UNUSED_ALWAYS( nLength );
    return( CalcFixedLayout( nMode & LM_STRETCH, nMode & LM_HORZ ) );
}

CSize CControlBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
/*************************************************************/
{
    CSize size( 0, 0 );
    if( bStretch ) {
        if( bHorz ) {
            size.cx = 32767;
        } else {
            size.cy = 32767;
        }
    }
    return( size );
}

void CControlBar::CalcInsideRect( CRect &rect, BOOL bHorz ) const
/***************************************************************/
{
    UNUSED_ALWAYS( bHorz );
    
    int cxBorder = ::GetSystemMetrics( SM_CXBORDER );
    int cyBorder = ::GetSystemMetrics( SM_CYBORDER );

    if( m_dwStyle & CBRS_BORDER_LEFT ) {
        rect.left += cxBorder * 2;
    }
    if( m_dwStyle & CBRS_BORDER_TOP ) {
        rect.top += cyBorder * 2;
    }
    if( m_dwStyle & CBRS_BORDER_RIGHT ) {
        rect.right -= cxBorder * 2;
    }
    if( m_dwStyle & CBRS_BORDER_BOTTOM ) {
        rect.bottom -= cyBorder * 2;
    }
    if( (m_dwStyle & CBRS_GRIPPER) &&
        (!IsFloating() || (m_pDockBar->GetBarStyle() & CBRS_FLOAT_MULTI)) ) {
        if( bHorz ) {
            rect.left += GRIPPER_BORDER * 2;
            rect.left += GRIPPER_WIDTH;
        } else {
            rect.top += GRIPPER_BORDER * 2;
            rect.top += GRIPPER_WIDTH;
        }
    }
    rect.left += m_cxLeftBorder;
    rect.top += m_cyTopBorder;
    rect.right -= m_cxRightBorder;
    rect.bottom -= m_cyBottomBorder;
}

void CControlBar::DoPaint( CDC *pDC )
/***********************************/
{
    CRect rect;
    GetClientRect( &rect );
    DrawBorders( pDC, rect );
    DrawGripper( pDC, rect );
}

void CControlBar::DrawBorders( CDC *pDC, CRect &rect )
/****************************************************/
{
    int         cxBorder    = ::GetSystemMetrics( SM_CXBORDER );
    int         cyBorder    = ::GetSystemMetrics( SM_CYBORDER );
    COLORREF    crBtnShadow = ::GetSysColor( COLOR_BTNSHADOW );

    CRect rect2;
    if( m_dwStyle & CBRS_BORDER_LEFT ) {
        rect2.SetRect( rect.left, rect.top, rect.left + cxBorder, rect.bottom );
        pDC->FillSolidRect( &rect2, crBtnShadow );
    }
    if( m_dwStyle & CBRS_BORDER_TOP ) {
        rect2.SetRect( rect.left, rect.top, rect.right, rect.top + cyBorder );
        pDC->FillSolidRect( &rect2, crBtnShadow );
    }
    if( m_dwStyle & CBRS_BORDER_RIGHT ) {
        rect2.SetRect( rect.right - cxBorder, rect.top, rect.right, rect.bottom );
        pDC->FillSolidRect( &rect2, crBtnShadow );
    }
    if( m_dwStyle & CBRS_BORDER_BOTTOM ) {
        rect2.SetRect( rect.left, rect.bottom - cyBorder, rect.right, rect.bottom );
        pDC->FillSolidRect( &rect2, crBtnShadow );
    }
}

void CControlBar::DrawGripper( CDC *pDC, const CRect &rect )
/**********************************************************/
{
    if( (m_dwStyle & CBRS_GRIPPER) &&
        (!IsFloating() || (m_pDockBar->GetBarStyle() & CBRS_FLOAT_MULTI)) ) {
        COLORREF    crBtnHighlight  = ::GetSysColor( COLOR_BTNHIGHLIGHT );
        COLORREF    crBtnShadow     = ::GetSysColor( COLOR_BTNSHADOW );
        CRect       rect2;
        if( m_dwStyle & CBRS_ORIENT_HORZ ) {
            rect2.SetRect( rect.left + GRIPPER_BORDER, rect.top + GRIPPER_BORDER,
                           rect.left + GRIPPER_BORDER + GRIPPER_WIDTH,
                           rect.bottom - GRIPPER_BORDER );
        } else {
            rect2.SetRect( rect.left + GRIPPER_BORDER, rect.top + GRIPPER_BORDER,
                           rect.right - GRIPPER_BORDER,
                           rect.top + GRIPPER_BORDER + GRIPPER_WIDTH );
        }
        pDC->Draw3dRect( &rect2, crBtnHighlight, crBtnShadow );
    }
}

void CControlBar::EnableDocking( DWORD dwDockStyle )
/**************************************************/
{
    m_dwDockStyle = dwDockStyle;
    if( m_pDockContext == NULL ) {
        m_pDockContext = new CDockContext( this );
    }
    if( m_hWndOwner == NULL ) {
        m_hWndOwner = ::GetParent( m_hWnd );
    }
}

BOOL CControlBar::IsDockBar() const
/*********************************/
{
    return( FALSE );
}

void CControlBar::OnBarStyleChange( DWORD dwOldStyle, DWORD dwNewStyle )
/**********************************************************************/
{
    UNUSED_ALWAYS( dwOldStyle );
    UNUSED_ALWAYS( dwNewStyle );
}

BOOL CControlBar::PreTranslateMessage( MSG *pMsg )
/************************************************/
{
    if( CWnd::PreTranslateMessage( pMsg ) ) {
        return( TRUE );
    }

    if( m_dwStyle & CBRS_FLYBY ) {
        if( (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST) ) {
            CPoint point( ::GetMessagePos() );
            ScreenToClient( &point );

            AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
            ASSERT( pState != NULL );
            
            INT_PTR nID = OnToolHitTest( point, NULL );
            if( pState->m_nLastStatus != nID || pState->m_pLastStatus != this ) {
                CWnd    *pOwner = GetOwner();
                ASSERT( pOwner != NULL );
                if( nID > 0 ) {
                    pState->m_nLastStatus = nID;
                    pState->m_pLastStatus = this;
                    pOwner->SendMessage( WM_SETMESSAGESTRING, nID );
                    SetTimer( TIMER_ID, 100, NULL );
                } else {
                    pState->m_nLastStatus = -1;
                    pState->m_pLastStatus = NULL;
                    pOwner->SendMessage( WM_POPMESSAGESTRING, AFX_IDS_IDLEMESSAGE );
                }
            }
        }
    }
    return( FALSE );
}

void CControlBar::PostNcDestroy()
/*******************************/
{
    if( m_bAutoDelete ) {
        delete this;
    }
}

LRESULT CControlBar::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
/***************************************************************************/
{
    if( message == WM_COMMAND || message == WM_NOTIFY || message == WM_DRAWITEM ||
        message == WM_MEASUREITEM || message == WM_DELETEITEM ||
        message == WM_COMPAREITEM || message == WM_VKEYTOITEM ||
        message == WM_CHARTOITEM ) {
        
        LRESULT lResult;
        if( OnWndMsg( message, wParam, lParam, &lResult ) ) {
            return( lResult );
        }
        return( GetOwner()->SendMessage( message, wParam, lParam ) );
    }
    return( CWnd::WindowProc( message, wParam, lParam ) );
}

#ifdef _DEBUG

void CControlBar::Dump( CDumpContext &dc ) const
/**********************************************/
{
    CWnd::Dump( dc );

    dc << "m_bAutoDelete = " << m_bAutoDelete << "\n";
    dc << "m_cxLeftBorder = " << m_cxLeftBorder << "\n";
    dc << "m_cxRightBorder = " << m_cxRightBorder << "\n";
    dc << "m_cyTopBorder = " << m_cyTopBorder << "\n";
    dc << "m_cyBottomBorder = " << m_cyBottomBorder << "\n";
    dc << "m_nMRUWidth = " << m_nMRUWidth << "\n";
    dc << "m_nCount = " << m_nCount << "\n";
    dc << "m_pData = " << m_pData << "\n";
    dc << "m_dwStyle = ";
    dc.DumpAsHex( m_dwStyle );
    dc << "\n";
    dc << "m_dwDockStyle = ";
    dc.DumpAsHex( m_dwDockStyle );
    dc << "\n";
    dc << "m_pDockSite = " << (void *)m_pDockSite << "\n";
    dc << "m_pDockBar = " << (void *)m_pDockBar << "\n";
    dc << "m_pDockContext = " << (void *)m_pDockContext << "\n";
}

#endif // _DEBUG

void CControlBar::EraseNonClient()
/********************************/
{
    CWindowDC   dc( this );
    CRect       rectClient;
    CRect       rectWindow;
    GetClientRect( &rectClient );
    ClientToScreen( &rectClient );
    GetWindowRect( &rectWindow );
    rectClient.OffsetRect( -rectWindow.left, -rectWindow.top );
    rectWindow.OffsetRect( -rectWindow.left, -rectWindow.top );
    dc.ExcludeClipRect( &rectClient );
    DrawBorders( &dc, rectWindow );
    DrawGripper( &dc, rectWindow );
}

BOOL CControlBar::IsFloating() const
/**********************************/
{
    return( m_pDockBar != NULL && m_pDockBar->m_bFloating );
}

void CControlBar::SetBarStyle( DWORD dwStyle )
/********************************************/
{
    DWORD dwOldStyle = m_dwStyle;
    m_dwStyle = dwStyle;
    EnableToolTips( dwStyle & CBRS_TOOLTIPS );
    OnBarStyleChange( dwOldStyle, dwStyle );
}

void CControlBar::SetBorders( int cxLeft, int cyTop, int cxRight, int cyBottom )
/******************************************************************************/
{
    m_cxLeftBorder = cxLeft;
    m_cyTopBorder = cyTop;
    m_cxRightBorder = cxRight;
    m_cyBottomBorder = cyBottom;
}

int CControlBar::OnCreate( LPCREATESTRUCT lpCreateStruct )
/********************************************************/
{
    if( CWnd::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }

    CFrameWnd *pFrame = GetParentFrame();
    if( pFrame != NULL ) {
        pFrame->AddControlBar( this );
        m_pDockSite = pFrame;
    }

    if( m_dwStyle & CBRS_TOOLTIPS ) {
        EnableToolTips();
    }
    
    return( 0 );
}

void CControlBar::OnDestroy()
/***************************/
{
    if( m_pDockSite != NULL ) {
        m_pDockSite->RemoveControlBar( this );
    }
    CWnd::OnDestroy();
}

void CControlBar::OnLButtonDown( UINT nFlags, CPoint point )
/**********************************************************/
{
    CWnd::OnLButtonDown( nFlags, point );
    if( m_pDockContext != NULL && OnToolHitTest( point, NULL ) < 0 ) {
        ClientToScreen( &point );
        m_pDockContext->StartDrag( point );
    }
}

void CControlBar::OnLButtonDblClk( UINT nFlags, CPoint point )
/************************************************************/
{
    CWnd::OnLButtonDblClk( nFlags, point );
    if( m_pDockContext != NULL && OnToolHitTest( point, NULL ) < 0 ) {
        ClientToScreen( &point );
        m_pDockContext->ToggleDocking();
    }
}

void CControlBar::OnPaint()
/*************************/
{
    CPaintDC dc( this );
    DoPaint( &dc );
}

void CControlBar::OnTimer( UINT_PTR nIDEvent )
/********************************************/
{
    if( nIDEvent == TIMER_ID ) {
        CPoint point( ::GetMessagePos() );
        ::ScreenToClient( m_hWnd, &point );
        if( OnToolHitTest( point, NULL ) <= 0 ) {
            CWnd *pOwner = GetOwner();
            ASSERT( pOwner != NULL );
            pOwner->SendMessage( WM_POPMESSAGESTRING, AFX_IDS_IDLEMESSAGE );
            KillTimer( TIMER_ID );
        }
    }
}

LRESULT CControlBar::OnHelpHitTest( WPARAM wParam, LPARAM lParam )
/****************************************************************/
{
    UNUSED_ALWAYS( wParam );
    
    CPoint point( lParam );
    INT_PTR nID = OnToolHitTest( point, NULL );
    if( nID <= 0 ) {
        return( 0L );
    }
    return( nID );
}

LRESULT CControlBar::OnIdleUpdateCmdUI( WPARAM wParam, LPARAM lParam )
/********************************************************************/
{
    UNUSED_ALWAYS( lParam );

    CFrameWnd *pFrame = GetParentFrame();
    if( pFrame != NULL ) {
        OnUpdateCmdUI( pFrame, (BOOL)wParam );
    }
    return( 0L );
}

LRESULT CControlBar::OnSizeParent( WPARAM wParam, LPARAM lParam )
/***************************************************************/
{
    UNUSED_ALWAYS( wParam );

    if( GetStyle() & WS_VISIBLE ) {
        AFX_SIZEPARENTPARAMS *pSPP = (AFX_SIZEPARENTPARAMS *)lParam;
        ASSERT( pSPP != NULL );

        DWORD nMode = 0L;
        if( m_dwStyle & CBRS_ORIENT_HORZ ) {
            nMode |= LM_HORZ;
        }
        if( pSPP->bStretch ) {
            nMode |= LM_STRETCH;
        }
        CSize size = CalcDynamicLayout( -1, nMode );
        CRect rect;
        if( m_dwStyle & CBRS_ALIGN_LEFT ) {
            rect.left = pSPP->rect.left;
            rect.top = pSPP->rect.top;
            rect.right = pSPP->rect.left + size.cx;
            rect.bottom = min( pSPP->rect.top + size.cy, pSPP->rect.bottom );
            pSPP->rect.left += size.cx;
            pSPP->sizeTotal.cx += size.cx;
            pSPP->sizeTotal.cy = max( pSPP->sizeTotal.cy, size.cy );
        } else if( m_dwStyle & CBRS_ALIGN_RIGHT ) {
            rect.left = pSPP->rect.right - size.cx;
            rect.top = pSPP->rect.top;
            rect.right = pSPP->rect.right;
            rect.bottom = min( pSPP->rect.top + size.cy, pSPP->rect.bottom );
            pSPP->rect.right -= size.cx;
            pSPP->sizeTotal.cx += size.cx;
            pSPP->sizeTotal.cy = max( pSPP->sizeTotal.cy, size.cy );
        } else if( m_dwStyle & CBRS_ALIGN_BOTTOM ) {
            rect.left = pSPP->rect.left;
            rect.top = pSPP->rect.bottom - size.cy;
            rect.right = min( pSPP->rect.left + size.cx, pSPP->rect.right );
            rect.bottom = pSPP->rect.bottom;
            pSPP->rect.bottom -= size.cy;
            pSPP->sizeTotal.cx = max( pSPP->sizeTotal.cx, size.cx );
            pSPP->sizeTotal.cy += size.cy;
        } else {
            rect.left = pSPP->rect.left;
            rect.top = pSPP->rect.top;
            rect.right = min( pSPP->rect.left + size.cx, pSPP->rect.right );
            rect.bottom = pSPP->rect.top + size.cy;
            pSPP->rect.top += size.cy;
            pSPP->sizeTotal.cx = max( pSPP->sizeTotal.cx, size.cx );
            pSPP->sizeTotal.cy += size.cy;
        }
        AfxRepositionWindow( pSPP, m_hWnd, &rect );
    }
    return( 0L );
}
