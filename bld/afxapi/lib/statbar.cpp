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
* Description:  Implementation of CStatusBar.
*
****************************************************************************/


#include "stdafx.h"
#include "statcui.h"

// This constant gives the additional border space that must be added to a status bar
// pane in order for it to fit a string of a given size.  The number is not documented
// as part of Win32.  However, it is used by Microsoft in MFC, so it is safe to assume
// that it will not change in the future, since such a change would break compatibility
// with pre-existing MFC applications.
#define PANE_BORDER_LENGTH  6

// Status bar pane information
struct STATUSBAR_PANE_INFO {
    UINT    nID;
    int     nWidth;
    CString strText;
    DWORD   dwStyle;
};

IMPLEMENT_DYNAMIC( CStatusBar, CControlBar )

BEGIN_MESSAGE_MAP( CStatusBar, CControlBar )
    ON_WM_NCPAINT()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_MESSAGE( WM_SETTEXT, OnSetText )
END_MESSAGE_MAP()

CStatusBar::CStatusBar()
/**********************/
{
}

CStatusBar::~CStatusBar()
/***********************/
{
    if( m_pData != NULL ) {
        delete [] (STATUSBAR_PANE_INFO *)m_pData;
    }
}

BOOL CStatusBar::Create( CWnd *pParentWnd, DWORD dwStyle, UINT nID )
/******************************************************************/
{
    return( CreateEx( pParentWnd, 0L, dwStyle, nID ) );
}

BOOL CStatusBar::CreateEx( CWnd *pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID )
/***************************************************************************************/
{
    m_dwStyle = dwStyle & CBRS_ALL;
    dwStyle &= ~CBRS_ALL;
    dwStyle |= CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN;
    if( pParentWnd->GetStyle() & WS_THICKFRAME ) {
        dwStyle |= SBARS_SIZEGRIP;
    }
    return( CWnd::CreateEx( 0L, STATUSCLASSNAME, NULL, dwStyle | dwCtrlStyle,
                            CRect( 0, 0, 0, 0 ), pParentWnd, nID ) );
}

CSize CStatusBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
/************************************************************/
{
    UNUSED_ALWAYS( bHorz );

    CSize       size( 0, 0 );
    TEXTMETRIC  tm;
    HDC         hDC = ::GetDC( NULL );
    HFONT       hFont = (HFONT)::SendMessage( m_hWnd, WM_GETFONT, 0, 0L );
    HFONT       hOldFont = (HFONT)::SelectObject( hDC, hFont );
    ::GetTextMetrics( hDC, &tm );
    ::SelectObject( hDC, hOldFont );
    ::ReleaseDC( m_hWnd, hDC );
    size.cy += tm.tmHeight;

    int iBorders[3];
    ::SendMessage( m_hWnd, SB_GETBORDERS, 0, (LPARAM)&iBorders );
    size.cy += iBorders[1] * 2;
    size.cy += ::GetSystemMetrics( SM_CYBORDER ) * 2;
    
    if( bStretch ) {
        size.cx = 32767;
    }
    return( size );
}

void CStatusBar::CalcInsideRect( CRect &rect, BOOL bHorz ) const
/**************************************************************/
{
    CControlBar::CalcInsideRect( rect, bHorz );
    if( GetStyle() & SBARS_SIZEGRIP ) {
        int iBorders[3];
        ::SendMessage( m_hWnd, SB_GETBORDERS, 0, (LPARAM)&iBorders );
        rect.right -= iBorders[0];
        rect.right -= ::GetSystemMetrics( SM_CXVSCROLL );
        rect.right -= ::GetSystemMetrics( SM_CXBORDER ) * 2;
    }
}

void CStatusBar::OnUpdateCmdUI( CFrameWnd *pTarget, BOOL bDisableIfNoHndler )
/***************************************************************************/
{
    UNUSED_ALWAYS( bDisableIfNoHndler );
    
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    CStatusCmdUI        cui;
    cui.m_pOther = this;
    for( int i = 0; i < m_nCount; i++ ) {
        cui.m_nIndex = i;
        cui.m_nID = pPanes[i].nID;
        cui.m_bContinueRouting = FALSE;
        if( cui.m_nID != ID_SEPARATOR ) {
            pTarget->OnCmdMsg( cui.m_nID, CN_UPDATE_COMMAND_UI, &cui, NULL );
        }
    }
}

int CStatusBar::CommandToIndex( UINT nIDFind ) const
/**************************************************/
{
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    for( int i = 0; i < m_nCount; i++ ) {
        if( pPanes[i].nID == nIDFind ) {
            return( i );
        }
    }
    return( -1 );
}

UINT CStatusBar::GetItemID( int nIndex ) const
/********************************************/
{
    ASSERT( nIndex < m_nCount );
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    return( pPanes[nIndex].nID );
}

void CStatusBar::GetPaneInfo( int nIndex, UINT &nID, UINT &nStyle, int &cxWidth ) const
/*************************************************************************************/
{
    ASSERT( nIndex < m_nCount );
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    nID = pPanes[nIndex].nID;
    nStyle = pPanes[nIndex].dwStyle;
    cxWidth = pPanes[nIndex].nWidth;
}

UINT CStatusBar::GetPaneStyle( int nIndex ) const
/***********************************************/
{
    ASSERT( nIndex < m_nCount );
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    return( pPanes[nIndex].dwStyle );
}

CString CStatusBar::GetPaneText( int nIndex ) const
/*************************************************/
{
    ASSERT( nIndex < m_nCount );
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    return( pPanes[nIndex].strText );
}

BOOL CStatusBar::SetIndicators( const UINT *lpIDArray, int nIDCount )
/*******************************************************************/
{
    if( m_pData != NULL ) {
        delete [] (STATUSBAR_PANE_INFO *)m_pData;
    }
    STATUSBAR_PANE_INFO *pPanes = new STATUSBAR_PANE_INFO[nIDCount];
    m_pData = pPanes;
    m_nCount = nIDCount;

    HDC     hDC = ::GetDC( NULL );
    HFONT   hFont = (HFONT)::SendMessage( m_hWnd, WM_GETFONT, 0, 0L );
    HFONT   hOldFont = (HFONT)::SelectObject( hDC, hFont );
    SIZE    size;
    for( int i = 0; i < nIDCount; i++ ) {
        pPanes[i].nID = lpIDArray[i];
        if( lpIDArray[i] == 0 ) {
            pPanes[i].nWidth = ::GetSystemMetrics( SM_CXSCREEN ) / 4;
        } else {
            pPanes[i].strText.LoadString( lpIDArray[i] );
            ::GetTextExtentPoint32( hDC, (LPCTSTR)pPanes[i].strText,
                                    pPanes[i].strText.GetLength(), &size );
            pPanes[i].nWidth = size.cx;
        }
        if( i > 0 ) {
            pPanes[i].dwStyle = SBPS_NORMAL;
        } else {
            pPanes[i].dwStyle = SBPS_STRETCH;
        }
    }
    ::SelectObject( hDC, hOldFont );
    ::ReleaseDC( m_hWnd, hDC );
    UpdateAllPanes( TRUE, TRUE );
    return( TRUE );
}

void CStatusBar::SetPaneInfo( int nIndex, UINT nID, UINT nStyle, int cxWidth )
/****************************************************************************/
{
    ASSERT( nIndex < m_nCount );
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    if( pPanes[nIndex].nID != nID || pPanes[nIndex].dwStyle != nStyle ||
        pPanes[nIndex].nWidth != cxWidth ) {
        pPanes[nIndex].nID = nID;
        pPanes[nIndex].dwStyle = nStyle;
        pPanes[nIndex].nWidth = cxWidth;
        UpdateAllPanes( TRUE, TRUE );
    }
}

void CStatusBar::SetPaneStyle( int nIndex, UINT nStyle )
/******************************************************/
{
    ASSERT( nIndex < m_nCount );
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    if( pPanes[nIndex].dwStyle != nStyle ) {
        pPanes[nIndex].dwStyle = nStyle;
        UpdateAllPanes( FALSE, TRUE );
    }
}

BOOL CStatusBar::SetPaneText( int nIndex, LPCTSTR lpszNewText, BOOL bUpdate )
/***************************************************************************/
{
    ASSERT( nIndex < m_nCount );
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    if( pPanes[nIndex].strText != lpszNewText ) {
        pPanes[nIndex].strText = lpszNewText;
        if( bUpdate ) {
            UpdateAllPanes( FALSE, TRUE );
        }
    }
    return( TRUE );
}

void CStatusBar::UpdateAllPanes( BOOL bUpdateRects, BOOL bUpdateText )
/********************************************************************/
{
    STATUSBAR_PANE_INFO *pPanes = (STATUSBAR_PANE_INFO *)m_pData;
    if( bUpdateRects ) {
        int iBorders[3];
        ::SendMessage( m_hWnd, SB_GETBORDERS, 0, (LPARAM)iBorders );
        
        CRect rect;
        ::GetWindowRect( m_hWnd, &rect );
        CalcInsideRect( rect, TRUE );
        
        UINT    *pParts = new UINT[m_nCount];
        UINT    nLeftOver = rect.right - rect.left;
        UINT    nStretchPanes = 0;
        for( int i = 0; i < m_nCount; i++ ) {
            if( pPanes[i].dwStyle & SBPS_STRETCH ) {
                nStretchPanes++;
            } else {
                nLeftOver -= pPanes[i].nWidth;
                nLeftOver -= iBorders[2] + PANE_BORDER_LENGTH;
            }
        }
        for( int i = 0; i < m_nCount; i++ ) {
            if( i == 0 ) {
                pParts[i] = 0;
            } else {
                pParts[i] = pParts[i - 1];
            }
            if( pPanes[i].dwStyle & SBPS_STRETCH ) {
                ASSERT( nStretchPanes > 0 );
                pParts[i] += nLeftOver / nStretchPanes;
            } else {
                pParts[i] += pPanes[i].nWidth;
                pParts[i] += iBorders[2] + PANE_BORDER_LENGTH;
            }
        }
        ::SendMessage( m_hWnd, SB_SETPARTS, m_nCount, (LPARAM)pParts );
    }
    if( bUpdateText ) {
        for( int i = 0; i < m_nCount; i++ ) {
            if( pPanes[i].dwStyle & SBPS_DISABLED ) {
                ::SendMessage( m_hWnd, SB_SETTEXT, i | pPanes[i].dwStyle, (LPARAM)NULL );
            } else {
                ::SendMessage( m_hWnd, SB_SETTEXT, i | pPanes[i].dwStyle,
                               (LPARAM)(LPCTSTR)pPanes[i].strText );
            }
        }
    }
}

void CStatusBar::OnNcPaint()
/**************************/
{
    EraseNonClient();
}

void CStatusBar::OnPaint()
/************************/
{
    // Bypass CControlBar::OnPaint and have the status bar control do the painting.
    // The borders and gripper are painted in OnNcPaint.
    CWnd::OnPaint();
}

void CStatusBar::OnSize( UINT nType, int cx, int cy )
/***************************************************/
{
    CControlBar::OnSize( nType, cx, cy );
    UpdateAllPanes( TRUE, FALSE );
}

LRESULT CStatusBar::OnGetText( WPARAM wParam, LPARAM lParam )
/***********************************************************/
{
    ASSERT( (LPCTSTR)lParam != NULL );

    int n = CommandToIndex( ID_SEPARATOR );
    if( n < 0 ) {
        ((LPTSTR)lParam)[0] = _T('\0');
        return( 0L );
    }

    CString strText = GetPaneText( n );
    int     nLength = strText.GetLength();
    if( nLength >= (int)wParam ) {
        nLength = (int)wParam - 1;
    }
    _tcsncpy( (LPTSTR)lParam, (LPCTSTR)strText, nLength );
    ((LPTSTR)lParam)[0] = _T('\0');
    return( nLength );
}

LRESULT CStatusBar::OnGetTextLength( WPARAM wParam, LPARAM lParam )
/*****************************************************************/
{
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( lParam );

    int n = CommandToIndex( ID_SEPARATOR );
    if( n < 0 ) {
        return( 0L );
    }
    return( GetPaneText( n ).GetLength() );
}

LRESULT CStatusBar::OnSetText( WPARAM wParam, LPARAM lParam )
/***********************************************************/
{
    UNUSED_ALWAYS( wParam );

    int n = CommandToIndex( ID_SEPARATOR );
    if( n < 0 ) {
        return( FALSE );
    }
    SetPaneText( n, (LPCTSTR)lParam );
    return( TRUE );
}
