/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of CMiniDockFrameWnd.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNCREATE( CMiniDockFrameWnd, CMiniFrameWnd )

BEGIN_MESSAGE_MAP( CMiniDockFrameWnd, CMiniFrameWnd )
    ON_WM_CLOSE()
    ON_WM_NCLBUTTONDBLCLK()
    ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

CMiniDockFrameWnd::CMiniDockFrameWnd()
    : m_wndDockBar( TRUE )
/************************/
{
}

BOOL CMiniDockFrameWnd::Create( CWnd *pParent, DWORD dwBarStyle )
/***************************************************************/
{
    if( !CMiniFrameWnd::Create( NULL, NULL, WS_CAPTION | WS_SYSMENU | FWS_SNAPTOBARS |
                                MFS_THICKFRAME | MFS_SYNCACTIVE | MFS_MOVEFRAME,
                                rectDefault, pParent ) ) {
        return( FALSE );
    }

    DWORD dwStyle = WS_CHILD | WS_VISIBLE;
    if( dwBarStyle & (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT) ) {
        dwStyle |= CBRS_ALIGN_LEFT;
    } else {
        dwStyle |= CBRS_ALIGN_TOP;
    }
    dwStyle |= dwBarStyle & CBRS_FLOAT_MULTI;
    if( !m_wndDockBar.Create( pParent, dwStyle, AFX_IDW_DOCKBAR_FLOAT ) ) {
        return( FALSE );
    }
    m_wndDockBar.SetParent( this );
    return( TRUE );
}

void CMiniDockFrameWnd::OnClose()
/*******************************/
{
    // Don't call the base class, since doing so will destroy the window.
    m_wndDockBar.ShowAll( FALSE );
}

void CMiniDockFrameWnd::OnNcLButtonDblClk( UINT nHitTest, CPoint point )
/**********************************************************************/
{
    if( nHitTest == HTCAPTION && !(m_wndDockBar.GetBarStyle() & CBRS_FLOAT_MULTI) ) {
        int nCount = m_wndDockBar.m_arrBars.GetCount();
        for( int i = 0; i < nCount; i++ ) {
            CControlBar *pBar = (CControlBar *)m_wndDockBar.m_arrBars.GetAt( i );
            if( pBar != NULL ) {
                ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
                ASSERT( pBar->m_pDockContext != NULL );
                pBar->m_pDockContext->ToggleDocking();
            }
        }
    } else {
        CMiniFrameWnd::OnNcLButtonDblClk( nHitTest, point );
    }
}

void CMiniDockFrameWnd::OnNcLButtonDown( UINT nHitTest, CPoint point )
/********************************************************************/
{
    if( nHitTest == HTCAPTION && !(m_wndDockBar.GetBarStyle() & CBRS_FLOAT_MULTI) ) {
        int nCount = m_wndDockBar.m_arrBars.GetCount();
        for( int i = 0; i < nCount; i++ ) {
            CControlBar *pBar = (CControlBar *)m_wndDockBar.m_arrBars.GetAt( i );
            if( pBar != NULL ) {
                ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
                ASSERT( pBar->m_pDockContext != NULL );
                pBar->m_pDockContext->StartDrag( point );
                break;
            }
        }
    } else {
        CMiniFrameWnd::OnNcLButtonDown( nHitTest, point );
    }
}
