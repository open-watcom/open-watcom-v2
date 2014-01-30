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
* Description:  Implementation of CScrollView.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CScrollView, CView )

BEGIN_MESSAGE_MAP( CScrollView, CView )
    ON_WM_HSCROLL()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

const SIZE CScrollView::sizeDefault = { 0, 0 };

void CScrollView::UpdateBars()
/****************************/
{
    CRect rectClient;
    GetClientRect( &rectClient );

    int     cxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );
    int     cyHScroll = ::GetSystemMetrics( SM_CYVSCROLL );
    BOOL    bHadX = GetStyle() & WS_HSCROLL;
    BOOL    bHadY = GetStyle() & WS_VSCROLL;
    if( !bHadY ) {
        rectClient.right -= cxVScroll;
    }
    if( !bHadX ) {
        rectClient.bottom -= cyHScroll;
    }

    BOOL    bHasX = m_totalDev.cx > rectClient.Width();
    BOOL    bHasY = m_totalDev.cy > rectClient.Height();
    int     xOld = GetScrollPos( SB_HORZ );
    int     yOld = GetScrollPos( SB_VERT );
    
    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = m_totalDev.cx;
    si.nPage = rectClient.Width();
    if( !bHasY ) {
        si.nPage += cxVScroll;
    }
    SetScrollInfo( SB_HORZ, &si );
    EnableScrollBarCtrl( SB_HORZ, bHasX );
    si.nMax = m_totalDev.cy;
    si.nPage = rectClient.Height();
    if( !bHasX ) {
        si.nPage += cyHScroll;
    }
    SetScrollInfo( SB_VERT, &si );
    EnableScrollBarCtrl( SB_VERT, bHasY );

    int xNew = bHasX ? GetScrollPos( SB_HORZ ) : 0;
    int yNew = bHasY ? GetScrollPos( SB_VERT ) : 0;
    if( xOld != xNew ) {
        ::ScrollWindow( m_hWnd, xOld - xNew, 0, NULL, NULL );
    }
    if( yOld != yNew ) {
        ::ScrollWindow( m_hWnd, 0, yOld - yNew, NULL, NULL );
    }
}

CScrollView::CScrollView()
/************************/
{
    m_nMapMode = 0;
}

void CScrollView::OnPrepareDC( CDC *pDC, CPrintInfo *pInfo )
/**********************************************************/
{
    if( !pDC->IsPrinting() ) {
        if( m_nMapMode == -1 ) {
            CRect rect;
            GetClientRect( &rect );
            pDC->SetMapMode( MM_ANISOTROPIC );
            pDC->SetViewportExt( rect.Width(), rect.Height() );
            pDC->SetWindowExt( m_totalLog );
        } else {
            CPoint pt = GetDeviceScrollPosition();
            pDC->SetMapMode( m_nMapMode );
            pDC->SetViewportOrg( -pt );
        }
    }

    CView::OnPrepareDC( pDC, pInfo );
}

BOOL CScrollView::OnScroll( UINT nScrollCode, UINT nPos, BOOL bDoScroll )
/***********************************************************************/
{
    CRect rectClient;
    GetClientRect( &rectClient );

    BOOL    bHasX = m_totalDev.cx > rectClient.Width();
    BOOL    bHasY = m_totalDev.cy > rectClient.Height();
    
    int nHorzPos = GetScrollPos( SB_HORZ );
    switch( LOBYTE( nScrollCode ) ) {
    case SB_LEFT:
        nHorzPos = 0;
        break;
    case SB_LINELEFT:
        nHorzPos -= m_lineDev.cx;
        break;
    case SB_LINERIGHT:
        nHorzPos += m_lineDev.cx;
        break;
    case SB_PAGELEFT:
        nHorzPos -= m_pageDev.cx;
        break;
    case SB_PAGERIGHT:
        nHorzPos += m_pageDev.cx;
        break;
    case SB_RIGHT:
        nHorzPos = m_totalDev.cx;
        break;
    case SB_THUMBPOSITION:
        nHorzPos = nPos;
        break;
    case SB_THUMBTRACK:
        nHorzPos = nPos;
        break;
    }
    
    int nVertPos = GetScrollPos( SB_VERT );
    switch( HIBYTE( nScrollCode ) ) {
    case SB_BOTTOM:
        nVertPos = m_totalDev.cy;
        break;
    case SB_LINEDOWN:
        nVertPos += m_lineDev.cy;
        break;
    case SB_LINEUP:
        nVertPos -= m_lineDev.cy;
        break;
    case SB_PAGEDOWN:
        nVertPos += m_pageDev.cy;
        break;
    case SB_PAGEUP:
        nVertPos -= m_pageDev.cy;
        break;
    case SB_THUMBPOSITION:
        nVertPos = nPos;
        break;
    case SB_THUMBTRACK:
        nVertPos = nPos;
        break;
    case SB_TOP:
        nVertPos = 0;
        break;
    }
    if( nVertPos < 0 ) {
        nVertPos = 0;
    }

    return( OnScrollBy( CSize( nHorzPos - GetScrollPos( SB_HORZ ),
                               nVertPos - GetScrollPos( SB_VERT ) ), bDoScroll ) );
}

BOOL CScrollView::OnScrollBy( CSize sizeScroll, BOOL bDoScroll )
/**************************************************************/
{
    if( !bDoScroll ) {
        return( FALSE );
    }

    int nOldPosX = GetScrollPos( SB_HORZ );
    int nOldPosY = GetScrollPos( SB_VERT );
    SetScrollPos( SB_HORZ, nOldPosX + sizeScroll.cx );
    SetScrollPos( SB_VERT, nOldPosY + sizeScroll.cy );

    int nNewPosX = GetScrollPos( SB_HORZ );
    int nNewPosY = GetScrollPos( SB_VERT );
    ::ScrollWindow( m_hWnd, nOldPosX - nNewPosX, nOldPosY - nNewPosY, NULL, NULL );
    return( TRUE );
}

#ifdef _DEBUG

void CScrollView::AssertValid() const
/***********************************/
{
    CView::AssertValid();

    ASSERT( m_nMapMode != MM_ISOTROPIC );
    ASSERT( m_nMapMode != MM_ANISOTROPIC );
}

void CScrollView::Dump( CDumpContext &dc ) const
/**********************************************/
{
    CView::Dump( dc );

    dc << "m_nMapMode = ";
    switch( m_nMapMode ) {
    case MM_TEXT:
        dc << "MM_TEXT\n";
        break;
    case MM_LOMETRIC:
        dc << "MM_LOMETRIC\n";
        break;
    case MM_HIMETRIC:
        dc << "MM_HIMETRIC\n";
        break;
    case MM_LOENGLISH:
        dc << "MM_LOENGLISH\n";
        break;
    case MM_HIENGLISH:
        dc << "MM_HIENGLISH\n";
        break;
    case MM_TWIPS:
        dc << "MM_TWIPS\n";
        break;
    default:
        dc << m_nMapMode << "\n";
        break;
    }
    dc << "m_totalLog = " << m_totalLog << "\n";
    dc << "m_totalDev = " << m_totalDev << "\n";
    dc << "m_pageDev = " << m_pageDev << "\n";
    dc << "m_lineDev = " << m_lineDev << "\n";
}

#endif // _DEBUG

void CScrollView::CenterOnPoint( CPoint ptCenter )
/************************************************/
{
    CRect rect;
    ::GetClientRect( m_hWnd, &rect );

    DWORD dwStyle = GetStyle();
    if( dwStyle & WS_HSCROLL ) {
        ptCenter.x -= rect.Width() / 2;
        if( ptCenter.x < 0 ) {
            ptCenter.x = 0;
        }
        SetScrollPos( SB_HORZ, ptCenter.x );
    }
    if( dwStyle & WS_VSCROLL ) {
        ptCenter.y -= rect.Height() / 2;
        if( ptCenter.y < 0 ) {
            ptCenter.y = 0;
        }
        SetScrollPos( SB_VERT, ptCenter.y );
    }
}

void CScrollView::CheckScrollBars( BOOL &bHasHorzBar, BOOL &bHasVertBar ) const
/*****************************************************************************/
{
    if( m_nMapMode == -1 ) {
        bHasHorzBar = bHasVertBar = FALSE;
    } else {
        CRect rect;
        ::GetClientRect( m_hWnd, &rect );
        bHasHorzBar = (rect.Width() < m_totalDev.cx);
        bHasVertBar = (rect.Height() < m_totalDev.cy);
    }
}

void CScrollView::FillOutsideRect( CDC *pDC, CBrush *pBrush )
/***********************************************************/
{
    CRect rectClient;
    GetClientRect( &rectClient );

    CRect rect;
    if( m_totalDev.cx < rectClient.Width() ) {
        rect.left = rectClient.left + m_totalDev.cx;
        rect.top = rectClient.top;
        rect.right = rectClient.right;
        rect.bottom = rectClient.bottom;
        pDC->FillRect( &rect, pBrush );
    }
    if( m_totalDev.cy < rectClient.Height() ) {
        rect.left = rectClient.left;
        rect.top = rectClient.top + m_totalDev.cy;
        rect.right = rectClient.right;
        rect.bottom = rectClient.bottom;
        pDC->FillRect( &rect, pBrush );
    }
}

CPoint CScrollView::GetDeviceScrollPosition() const
/*************************************************/
{
    return( CPoint( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ) ) );
}

void CScrollView::GetDeviceScrollSizes( int &nMapMode, SIZE &sizeTotal, SIZE &sizePage,
                                        SIZE &sizeLine ) const
/************************************************************/
{
    nMapMode = m_nMapMode;
    sizeTotal = m_totalDev;
    sizePage = m_pageDev;
    sizeLine = m_lineDev;
}

CPoint CScrollView::GetScrollPosition() const
/*******************************************/
{
    CPoint pt = GetDeviceScrollPosition();
    CWindowDC dc( NULL );
    dc.SetMapMode( m_nMapMode );
    dc.DPtoLP( &pt );
    return( pt );
}

CSize CScrollView::GetTotalSize() const
/*************************************/
{
    return( m_totalLog );
}

void CScrollView::ScrollToPosition( POINT pt )
/********************************************/
{
    CWindowDC dc( NULL );
    dc.SetMapMode( m_nMapMode );
    dc.LPtoDP( &pt );

    int xOld = GetScrollPos( SB_HORZ );
    int yOld = GetScrollPos( SB_VERT );
    SetScrollPos( SB_HORZ, pt.x );
    SetScrollPos( SB_VERT, pt.y );

    int xNew = GetScrollPos( SB_HORZ );
    int yNew = GetScrollPos( SB_VERT );
    ::ScrollWindow( m_hWnd, xOld - xNew, yOld - yNew, NULL, NULL );
}

void CScrollView::SetScaleToFitSize( SIZE sizeTotal )
/***************************************************/
{
    m_nMapMode = -1;
    m_totalLog.cx = sizeTotal.cx;
    m_totalLog.cy = sizeTotal.cy;
    EnableScrollBarCtrl( SB_BOTH, FALSE );
}

void CScrollView::SetScrollSizes( int nMapMode, SIZE sizeTotal, const SIZE &sizePage,
                                  const SIZE &sizeLine )
/******************************************************/
{
    ASSERT( nMapMode > 0 );
    ASSERT( nMapMode != MM_ISOTROPIC );
    ASSERT( nMapMode != MM_ANISOTROPIC );
    m_nMapMode = nMapMode;
    m_totalLog = sizeTotal;
    m_totalDev = sizeTotal;
    m_pageDev = sizePage;
    m_lineDev = sizeLine;

    CWindowDC dc( NULL );
    dc.SetMapMode( nMapMode );
    dc.LPtoDP( (POINT *)&m_totalDev );
    dc.LPtoDP( (POINT *)&m_pageDev );
    dc.LPtoDP( (POINT *)&m_lineDev );

    if( m_totalDev.cy < 0 ) {
        m_totalDev.cy = -m_totalDev.cy;
    }
    if( m_pageDev.cy < 0 ) {
        m_pageDev.cy = -m_pageDev.cy;
    }
    if( m_lineDev.cy < 0 ) {
        m_lineDev.cy = -m_lineDev.cy;
    }
    
    if( m_pageDev.cx == 0 ) {
        m_pageDev.cx = m_totalDev.cx / 10;
    }
    if( m_pageDev.cy == 0 ) {
        m_pageDev.cy = m_totalDev.cy / 10;
    }
    if( m_lineDev.cx == 0 ) {
        m_lineDev.cx = m_totalDev.cx / 100;
    }
    if( m_lineDev.cy == 0 ) {
        m_lineDev.cy = m_totalDev.cy / 100;
    }

    UpdateBars();
}

void CScrollView::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
/****************************************************************************/
{
    if( pScrollBar == NULL || !pScrollBar->SendChildNotifyLastMsg() ) {
        if( pScrollBar == GetScrollBarCtrl( SB_HORZ ) ) {
            OnScroll( MAKEWORD( nSBCode, 0xFF ), nPos );
        } else {
            Default();
        }
    }
}

void CScrollView::OnSize( UINT nType, int cx, int cy )
/****************************************************/
{
    CView::OnSize( nType, cx, cy );
    UpdateBars();
}

void CScrollView::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
/****************************************************************************/
{
    if( pScrollBar == NULL || !pScrollBar->SendChildNotifyLastMsg() ) {
        if( pScrollBar == GetScrollBarCtrl( SB_VERT ) ) {
            OnScroll( MAKEWORD( 0xFF, nSBCode ), nPos );
        } else {
            Default();
        }
    }
}
