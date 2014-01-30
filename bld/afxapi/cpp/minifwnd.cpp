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
* Description:  Implementation of CMiniFrameWnd.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNCREATE( CMiniFrameWnd, CFrameWnd )

BEGIN_MESSAGE_MAP( CMiniFrameWnd, CFrameWnd )
    ON_WM_GETMINMAXINFO()
    ON_WM_NCACTIVATE()
    ON_WM_NCHITTEST()
    ON_MESSAGE( WM_FLOATSTATUS, OnFloatStatus )
END_MESSAGE_MAP()

CMiniFrameWnd::CMiniFrameWnd()
/****************************/
{
}

CMiniFrameWnd::~CMiniFrameWnd()
/*****************************/
{
    DestroyWindow();
}

BOOL CMiniFrameWnd::Create( LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
                            const RECT &rect, CWnd *pParentWnd, UINT nID )
/************************************************************************/
{
    return( CreateEx( 0L, lpClassName, lpWindowName, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CMiniFrameWnd::CreateEx( DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName,
                              DWORD dwStyle, const RECT &rect, CWnd *pParentWnd,
                              UINT nID )
/**************************************/
{
    return( CWnd::CreateEx( dwExStyle, lpClassName, lpWindowName, dwStyle, rect,
                            pParentWnd, nID ) );
}

BOOL CMiniFrameWnd::PreCreateWindow( CREATESTRUCT &cs )
/*****************************************************/
{
    if( !CFrameWnd::PreCreateWindow( cs ) ) {
        return( FALSE );
    }
    cs.dwExStyle |= WS_EX_TOOLWINDOW;
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    if( cs.style & (MFS_THICKFRAME | MFS_4THICKFRAME ) ) {
        cs.style |= WS_THICKFRAME;
    }
    return( TRUE );
}

void PASCAL CMiniFrameWnd::CalcBorders( LPRECT lpClientRect, DWORD dwStyle,
                                        DWORD dwExStyle )
/*******************************************************/
{
    dwExStyle |= WS_EX_TOOLWINDOW;
    dwExStyle &= ~WS_EX_CLIENTEDGE;
    ::AdjustWindowRectEx( lpClientRect, dwStyle, FALSE, dwExStyle );
}

void CMiniFrameWnd::OnGetMinMaxInfo( MINMAXINFO *lpMMI )
/******************************************************/
{
    CFrameWnd::OnGetMinMaxInfo( lpMMI );
    if( GetStyle() & MFS_THICKFRAME ) {
        CRect   rectClient;
        CRect   rectWindow;
        ::GetClientRect( m_hWnd, &rectClient );
        ::CopyRect( &rectWindow, &rectClient );
        ::AdjustWindowRectEx( &rectWindow, GetStyle(), FALSE, GetExStyle() );
        ASSERT( lpMMI != NULL );
        lpMMI->ptMinTrackSize.x = rectWindow.Width() - rectClient.Width();
        lpMMI->ptMinTrackSize.y = rectWindow.Height() - rectClient.Height();
    }
}

BOOL CMiniFrameWnd::OnNcActivate( BOOL bActive )
/**********************************************/
{
    if( GetStyle() & MFS_SYNCACTIVE ) {
        return( TRUE );
    }
    return( CFrameWnd::OnNcActivate( bActive ) );
}

LRESULT CMiniFrameWnd::OnNcHitTest( CPoint point )
/************************************************/
{
    LRESULT lHTCode = CFrameWnd::OnNcHitTest( point );
    if( GetStyle() & MFS_BLOCKSYSMENU ) {
        if( lHTCode == HTSYSMENU ) {
            lHTCode = HTCAPTION;
        }
    }
    if( GetStyle() & MFS_MOVEFRAME ) {
        if( (lHTCode >= HTSIZEFIRST && lHTCode <= HTSIZELAST) || lHTCode == HTGROWBOX ) {
            lHTCode = HTCAPTION;
        }
    }
    if( GetStyle() & MFS_4THICKFRAME ) {
        if( lHTCode == HTTOPLEFT || lHTCode == HTTOPRIGHT ) {
            lHTCode = HTTOP;
        } else if( lHTCode == HTBOTTOMLEFT || lHTCode == HTBOTTOMRIGHT ||
                   lHTCode == HTGROWBOX ){
            lHTCode = HTBOTTOM;
        }
    }
    return( lHTCode );
}

LRESULT CMiniFrameWnd::OnFloatStatus( WPARAM wParam, LPARAM lParam )
/******************************************************************/
{
    UNUSED_ALWAYS( lParam );
    
    if( wParam & FS_SHOW ) {
        ASSERT( !(wParam & FS_HIDE) );
        ::ShowWindow( m_hWnd, SW_SHOWNOACTIVATE );
    }
    if( wParam & FS_HIDE ) {
        ASSERT( !(wParam & FS_SHOW) );
        ::ShowWindow( m_hWnd, SW_HIDE );
    }
    if( wParam & FS_ACTIVATE ) {
        ASSERT( !(wParam & FS_DEACTIVATE) );
        if( GetStyle() & MFS_SYNCACTIVE ) {
            ModifyStyle( MFS_SYNCACTIVE, 0L );
            ::SendMessage( m_hWnd, WM_NCACTIVATE, TRUE, 0L );
            ModifyStyle( 0L, MFS_SYNCACTIVE );
        }
    }
    if( wParam & FS_DEACTIVATE ) {
        ASSERT( !(wParam & FS_ACTIVATE) );
        if( GetStyle() & MFS_SYNCACTIVE ) {
            ModifyStyle( MFS_SYNCACTIVE, 0L );
            ::SendMessage( m_hWnd, WM_NCACTIVATE, FALSE, 0L );
            ModifyStyle( 0L, MFS_SYNCACTIVE );
        }
    }
    if( wParam & FS_ENABLE ) {
        ASSERT( !(wParam & FS_DISABLE) );
        ::EnableWindow( m_hWnd, TRUE );
    }
    if( wParam & FS_DISABLE ) {
        ASSERT( !(wParam & FS_ENABLE) );
        ::EnableWindow( m_hWnd, FALSE );
    }
    if( wParam & FS_SYNCACTIVE ) {
        if( GetStyle() & MFS_SYNCACTIVE ) {
            return( 1L );
        }
    }
    return( 0L );
}
