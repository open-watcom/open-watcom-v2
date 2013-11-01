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
* Description:  Implementation of CReBar.
*
****************************************************************************/


#include "stdafx.h"

// The structure size for REBARBANDINFO gets hard-coded to avoid problems.
#define REBARBANDINFO_STRUCTSIZE    80

IMPLEMENT_DYNAMIC( CReBar, CControlBar )

BEGIN_MESSAGE_MAP( CReBar, CControlBar )
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_PAINT()
    ON_NOTIFY_REFLECT( RBN_ENDDRAG, OnEndDrag )
    ON_NOTIFY_REFLECT( RBN_HEIGHTCHANGE, OnHeightChange )
END_MESSAGE_MAP()

CReBar::CReBar()
/**************/
{
}

BOOL CReBar::Create( CWnd *pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID )
/*********************************************************************************/
{
    m_dwStyle = dwStyle & CBRS_ALL;
    dwStyle &= ~CBRS_ALL;
    dwStyle |= CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | CCS_NOMOVEY;
    return( CWnd::CreateEx( 0L, REBARCLASSNAME, NULL, dwStyle | dwCtrlStyle,
                            CRect( 0, 0, 0, 0 ), pParentWnd, nID ) );
}

CSize CReBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
/********************************************************/
{
    REBARBANDINFO   rbbi;
    CRect           rectTotal;
    RECT            rectBand;
    int             nCount = (int)::SendMessage( m_hWnd, RB_GETBANDCOUNT, 0, 0L );
    rbbi.cbSize = REBARBANDINFO_STRUCTSIZE;
    rbbi.fMask = RBBIM_STYLE | RBBIM_CHILD;
    rectTotal.SetRectEmpty();
    for( int i = 0; i < nCount; i++ ) {
        ::SendMessage( m_hWnd, RB_GETBANDINFO, i, (LPARAM)&rbbi );
        BOOL bVisible = ::GetWindowLong( rbbi.hwndChild, GWL_STYLE ) & WS_VISIBLE;
        if( bVisible && (rbbi.fStyle & RBBS_HIDDEN) ) {
            ::SendMessage( m_hWnd, RB_SHOWBAND, i, TRUE );
        } else if( !bVisible && !(rbbi.fStyle & RBBS_HIDDEN) ) {
            ::SendMessage( m_hWnd, RB_SHOWBAND, i, FALSE );
        }
    }
    rbbi.fMask = RBBIM_STYLE;
    for( int i = 0; i < nCount; i++ ) {
        ::SendMessage( m_hWnd, RB_GETBANDINFO, i, (LPARAM)&rbbi );
        if( !(rbbi.fStyle & RBBS_HIDDEN) ) {
            ::SendMessage( m_hWnd, RB_GETRECT, i, (LPARAM)&rectBand );
            rectTotal |= rectBand;
        }
    }
    if( bStretch ) {
        if( bHorz ) {
            return( CSize( 32767, rectTotal.Height() ) );
        } else {
            return( CSize( rectTotal.Width(), 32767 ) );
        }
    } else {
        return( rectTotal.Size() );
    }
}

INT_PTR CReBar::OnToolHitTest( CPoint point, TOOLINFO *pTI ) const
/****************************************************************/
{
    HWND hWndChild = ::ChildWindowFromPoint( m_hWnd, point );
    if( hWndChild != NULL && hWndChild != m_hWnd ) {
        CWnd *pWnd = CWnd::FromHandlePermanent( hWndChild );
        if( pWnd != NULL ) {
            ClientToScreen( &point );
            pWnd->ScreenToClient( &point );
            INT_PTR nID = pWnd->OnToolHitTest( point, pTI );
            if( nID >= 0 ) {
                return( nID );
            }
        }
    }
    return( CControlBar::OnToolHitTest( point, pTI ) );
}

void CReBar::OnUpdateCmdUI( CFrameWnd *pTarget, BOOL bDisableIfNoHndler )
/***********************************************************************/
{
    UpdateDialogControls( pTarget, bDisableIfNoHndler );
}

BOOL CReBar::AddBar( CWnd *pBar, LPCTSTR lpszText, CBitmap *pbmp, DWORD dwStyle )
/*******************************************************************************/
{
    ASSERT( pBar != NULL );
    
    CSize sizeChild;
    if( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) ) {
        sizeChild = ((CControlBar *)pBar)->CalcFixedLayout( FALSE, TRUE );
    } else {
        CRect rectChild;
        pBar->GetWindowRect( &rectChild );
        sizeChild = rectChild.Size();
    }
    
    REBARBANDINFO rbbi;
    rbbi.cbSize = REBARBANDINFO_STRUCTSIZE;
    rbbi.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE;
    rbbi.fStyle = dwStyle;
    if( lpszText != NULL ) {
        rbbi.fMask |= RBBIM_TEXT;
        rbbi.lpText = (LPTSTR)lpszText;
        rbbi.cch = _tcslen( lpszText );
    }
    rbbi.hwndChild = pBar->m_hWnd;
    rbbi.cxMinChild = sizeChild.cx;
    rbbi.cyMinChild = sizeChild.cy;
    if( pbmp != NULL ) {
        rbbi.fMask |= RBBIM_BACKGROUND;
        rbbi.hbmBack = (HBITMAP)pbmp->m_hObject;
    }
    if( !::SendMessage( m_hWnd, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbbi ) ) {
        return( FALSE );
    }

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    pFrame->RecalcLayout();
    return( TRUE );
}

BOOL CReBar::AddBar( CWnd *pBar, COLORREF clrFore, COLORREF clrBack, LPCTSTR lpszText,
                     DWORD dwStyle )
/**********************************/
{
    ASSERT( pBar != NULL );

    CSize sizeChild;
    if( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) ) {
        sizeChild = ((CControlBar *)pBar)->CalcFixedLayout( FALSE, TRUE );
    } else {
        CRect rectChild;
        pBar->GetWindowRect( &rectChild );
        sizeChild = rectChild.Size();
    }

    REBARBANDINFO rbbi;
    rbbi.cbSize = REBARBANDINFO_STRUCTSIZE;
    rbbi.fMask = RBBIM_STYLE | RBBIM_COLORS | RBBIM_CHILD | RBBIM_CHILDSIZE;
    rbbi.fStyle = dwStyle;
    rbbi.clrFore = clrFore;
    rbbi.clrBack = clrBack;
    if( lpszText != NULL ) {
        rbbi.fMask |= RBBIM_TEXT;
        rbbi.lpText = (LPTSTR)lpszText;
        rbbi.cch = _tcslen( lpszText );
    }
    rbbi.hwndChild = pBar->m_hWnd;
    rbbi.cxMinChild = sizeChild.cx;
    rbbi.cyMinChild = sizeChild.cy;
    if( !::SendMessage( m_hWnd, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbbi ) ) {
        return( FALSE );
    }

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    pFrame->RecalcLayout();
    return( TRUE );
}

void CReBar::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp )
/**************************************************************************/
{
    UNUSED_ALWAYS( bCalcValidRects );

    ASSERT( lpncsp != NULL );
    CRect rect;
    CalcInsideRect( rect, m_dwStyle & CBRS_ORIENT_HORZ );
    lpncsp->rgrc[0].left += rect.left;
    lpncsp->rgrc[0].top += rect.top;
    lpncsp->rgrc[0].right += rect.right;
    lpncsp->rgrc[0].bottom += rect.bottom;
}

void CReBar::OnNcPaint()
/**********************/
{
    EraseNonClient();
}

void CReBar::OnPaint()
/********************/
{
    // Bypass CControlBar::OnPaint and have the rebar control do the painting.
    CWnd::OnPaint();
}

void CReBar::OnEndDrag( NMHDR *pNMHDR, LRESULT *pResult )
/*******************************************************/
{
    UNUSED_ALWAYS( pNMHDR );

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    pFrame->RecalcLayout();
    *pResult = 0L;
}

void CReBar::OnHeightChange( NMHDR *pNMHDR, LRESULT *pResult )
/************************************************************/
{
    UNUSED_ALWAYS( pNMHDR );

    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    pFrame->RecalcLayout();
    *pResult = 0L;
}
