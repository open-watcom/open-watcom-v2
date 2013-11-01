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
* Description:  Implementation of CDockContext.
*
****************************************************************************/


#include "stdafx.h"

CDockContext::CDockContext( CControlBar *pBar )
/*********************************************/
{
    m_pBar = pBar;
    m_pDockSite = pBar->m_pDockSite;
    m_dwDockStyle = pBar->m_dwDockStyle;
    m_dwOverDockStyle = 0L;
    m_dwStyle = pBar->m_dwStyle & CBRS_ALIGN_ANY;
    m_bFlip = FALSE;
    m_bForceFrame = FALSE;
    m_pDC = NULL;
    m_bDragging = FALSE;
    m_uMRUDockID = 0;
    m_rectMRUDockPos.SetRectEmpty();
    m_ptMRUFloatPos.x = CW_USEDEFAULT;
    m_ptMRUFloatPos.y = CW_USEDEFAULT;
}

void CDockContext::StartDrag( CPoint pt )
/***************************************/
{
    ASSERT( m_pBar != NULL );
    m_ptLast = pt;
    m_dwStyle = m_pBar->m_dwStyle & CBRS_ALIGN_ANY;
    m_bFlip = ::GetKeyState( VK_SHIFT ) & 0x8000;
    m_bForceFrame = ::GetKeyState( VK_CONTROL ) & 0x8000;
    if( m_pBar->m_dwStyle & CBRS_ORIENT_HORZ ) {
        m_pBar->GetWindowRect( &m_rectDragHorz );
        int xCenter = (m_rectDragHorz.left + m_rectDragHorz.right) / 2;
        int yCenter = (m_rectDragHorz.top + m_rectDragHorz.bottom) / 2;
        int nWidth = m_rectDragHorz.right - m_rectDragHorz.left;
        int nHeight = m_rectDragHorz.bottom - m_rectDragHorz.top;
        m_rectDragVert.left = xCenter - nHeight / 2;
        m_rectDragVert.top = yCenter - nWidth / 2;
        m_rectDragVert.right = xCenter + nHeight / 2;
        m_rectDragVert.bottom = yCenter + nWidth / 2;
    } else {
        m_pBar->GetWindowRect( &m_rectDragVert );
        int xCenter = (m_rectDragVert.left + m_rectDragVert.right) / 2;
        int yCenter = (m_rectDragVert.top + m_rectDragVert.bottom) / 2;
        int nWidth = m_rectDragVert.right - m_rectDragVert.left;
        int nHeight = m_rectDragVert.bottom - m_rectDragVert.top;
        m_rectDragHorz.left = xCenter - nHeight / 2;
        m_rectDragHorz.top = yCenter - nWidth / 2;
        m_rectDragHorz.right = xCenter + nHeight / 2;
        m_rectDragHorz.bottom = yCenter + nWidth / 2;
    }
    m_rectFrameDragHorz = m_rectDragHorz;
    CMiniFrameWnd::CalcBorders( &m_rectFrameDragHorz );
    m_rectFrameDragVert = m_rectDragVert;
    CMiniFrameWnd::CalcBorders( &m_rectFrameDragVert );
    m_pDC = new CClientDC( NULL );
    m_dwOverDockStyle = CanDock();
    m_bDragging = TRUE;
    DrawFocusRect();
    Track();
}

void CDockContext::ToggleDocking()
/********************************/
{
    ASSERT( m_pBar != NULL );
    ASSERT( m_pDockSite != NULL );
    if( m_pBar->IsFloating() ) {
        if( m_uMRUDockID != 0 ) {
            m_pDockSite->DockControlBar( m_pBar, m_uMRUDockID, m_rectMRUDockPos );
        } else {
            m_pDockSite->DockControlBar( m_pBar );
        }
    } else {
        m_pDockSite->FloatControlBar( m_pBar, m_ptMRUFloatPos );
    }
}

DWORD CDockContext::CanDock()
/***************************/
{
    ASSERT( m_pDockSite != NULL );
    DWORD dwResult = 0L;
    dwResult |= m_pDockSite->CanDock( m_rectDragHorz, m_dwDockStyle & CBRS_ORIENT_HORZ );
    dwResult |= m_pDockSite->CanDock( m_rectDragVert, m_dwDockStyle & CBRS_ORIENT_VERT );
    return( dwResult );
}

void CDockContext::DrawFocusRect( BOOL bRemoveRect )
/**************************************************/
{
    ASSERT( m_pDC != NULL );
    m_pDC->DrawFocusRect( &m_rectLast );
    if( !bRemoveRect ) {
        if( (m_dwOverDockStyle & CBRS_ORIENT_HORZ) && !m_bForceFrame ) {
            m_pDC->DrawFocusRect( &m_rectDragHorz );
            m_rectLast = m_rectDragHorz;
        } else if( (m_dwOverDockStyle & CBRS_ORIENT_VERT) && !m_bForceFrame ) {
            m_pDC->DrawFocusRect( &m_rectDragVert );
            m_rectLast = m_rectDragVert;
        } else if( ((m_dwStyle & CBRS_ORIENT_HORZ) && !m_bFlip) ||
                   ((m_dwStyle & CBRS_ORIENT_VERT) && m_bFlip) ) {
            m_pDC->DrawFocusRect( &m_rectFrameDragHorz );
            m_rectLast = m_rectFrameDragHorz;
        } else {
            m_pDC->DrawFocusRect( &m_rectFrameDragVert );
            m_rectLast = m_rectFrameDragVert;
        }
    } else {
        m_rectLast.SetRectEmpty();
    }
}

void CDockContext::EndDrag()
/**************************/
{
    DrawFocusRect( TRUE );
    if( m_dwOverDockStyle != 0L && !m_bForceFrame ) {
        CDockBar *pDockBar = GetDockBar( m_dwOverDockStyle );
        ASSERT( pDockBar != NULL );
        if( ((m_dwOverDockStyle & CBRS_ORIENT_HORZ) && !m_bFlip) ||
            ((m_dwOverDockStyle & CBRS_ORIENT_VERT) && m_bFlip) ) {
            pDockBar->DockControlBar( m_pBar, &m_rectDragHorz );
        } else {
            pDockBar->DockControlBar( m_pBar, &m_rectDragVert );
        }
        m_dwOverDockStyle = 0L;
        m_uMRUDockID = pDockBar->GetDlgCtrlID();
        m_pBar->GetWindowRect( &m_rectMRUDockPos );
    } else {
        ASSERT( m_pDockSite != NULL );
        if( ((m_dwStyle & CBRS_ORIENT_HORZ) && !m_bFlip) ||
            ((m_dwStyle & CBRS_ORIENT_VERT) && m_bFlip) ) {
            m_pDockSite->FloatControlBar( m_pBar, m_rectFrameDragHorz.TopLeft(),
                CBRS_ALIGN_TOP | (m_dwDockStyle & CBRS_FLOAT_MULTI) );
            m_ptMRUFloatPos = m_rectFrameDragHorz.TopLeft();
        } else {
            m_pDockSite->FloatControlBar( m_pBar, m_rectFrameDragVert.TopLeft(),
                CBRS_ALIGN_LEFT | (m_dwDockStyle & CBRS_FLOAT_MULTI) );
            m_ptMRUFloatPos = m_rectFrameDragVert.TopLeft();
        }
    }
    delete m_pDC;
    m_pDC = NULL;
    m_bDragging = FALSE;
}

CDockBar *CDockContext::GetDockBar( DWORD dwOverDockStyle )
/*********************************************************/
{
    CDockBar *pDockBar = NULL;
    ASSERT( m_pDockSite != NULL );
    if( dwOverDockStyle & CBRS_ORIENT_HORZ ) {
        m_pDockSite->CanDock( m_rectDragHorz,
                              dwOverDockStyle & CBRS_ORIENT_HORZ,
                              &pDockBar );
    } else {
        m_pDockSite->CanDock( m_rectDragVert,
                              dwOverDockStyle & CBRS_ORIENT_VERT,
                              &pDockBar );
    }
    return( pDockBar );
}

void CDockContext::Move( CPoint pt )
/**********************************/
{
    CSize sizeOffset = pt - m_ptLast;
    m_ptLast = pt;
    if( m_bDragging ) {
        m_rectDragHorz += sizeOffset;
        m_rectDragVert += sizeOffset;
        m_rectFrameDragHorz += sizeOffset;
        m_rectFrameDragVert += sizeOffset;
    }
    DrawFocusRect();
    m_dwOverDockStyle = CanDock();
}

void CDockContext::OnKey( int nChar, BOOL bDown )
/***********************************************/
{
    if( nChar == VK_SHIFT && m_bFlip != bDown ) {
        m_bFlip = bDown;
        DrawFocusRect();
    } else if( nChar == VK_CONTROL && m_bForceFrame != bDown ) {
        m_bForceFrame = bDown;
        DrawFocusRect();
    }
}

BOOL CDockContext::Track()
/************************/
{
    ASSERT( m_pBar != NULL );
    m_pBar->SetCapture();
    while( CWnd::GetCapture() == m_pBar ) {
        MSG msg;
        if( !::GetMessage( &msg, NULL, 0, 0 ) ) {
            ::PostQuitMessage( msg.wParam );
            ::ReleaseCapture();
            break;
        }
        if( msg.message == WM_MOUSEMOVE ) {
            Move( msg.pt );
        } else if( msg.message == WM_LBUTTONUP ) {
            EndDrag();
            ::ReleaseCapture();
            return( TRUE );
        } else if( msg.message == WM_KEYDOWN ) {
            OnKey( msg.wParam, TRUE );
        } else if( msg.message == WM_KEYUP ) {
            OnKey( msg.wParam, FALSE );
        }
    }
    return( FALSE );
}
