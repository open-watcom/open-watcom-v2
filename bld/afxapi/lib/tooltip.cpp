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
* Description:  Implementation of CToolTipCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CToolTipCtrl, CWnd )

CToolTipCtrl::CToolTipCtrl()
/**************************/
{
}

BOOL CToolTipCtrl::Create( CWnd *pParentWnd, DWORD dwStyle )
/**********************************************************/
{
    return( CWnd::Create( TOOLTIPS_CLASS, NULL, dwStyle, CFrameWnd::rectDefault,
                          pParentWnd, 0 ) );
}

BOOL CToolTipCtrl::CreateEx( CWnd *pParentWnd, DWORD dwStyle, DWORD dwStyleEx )
/*****************************************************************************/
{
    return( CWnd::CreateEx( dwStyleEx, TOOLTIPS_CLASS, NULL, dwStyle,
                            CFrameWnd::rectDefault, pParentWnd, 0 ) );
}

BOOL CToolTipCtrl::AddTool( CWnd *pWnd, UINT nIDText, LPCRECT lpRectTool,
                            UINT_PTR nIDTool )
/********************************************/
{
    TOOLINFO ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.hwnd = pWnd->GetSafeHwnd();
    ti.uId = nIDTool;
    if( lpRectTool != NULL ) {
        ::CopyRect( &ti.rect, lpRectTool );
    }
    ti.hinst = AfxFindResourceHandle( MAKEINTRESOURCE( nIDText ), RT_STRING );
    ti.lpszText = (LPTSTR)nIDText;
    return( (BOOL)::SendMessage( m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&ti ) );
}

BOOL CToolTipCtrl::AddTool( CWnd *pWnd, LPCTSTR lpszText, LPCRECT lpRectTool,
                            UINT_PTR nIDTool )
/********************************************/
{
    TOOLINFO ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.hwnd = pWnd->GetSafeHwnd();
    ti.uId = nIDTool;
    if( lpRectTool != NULL ) {
        ::CopyRect( &ti.rect, lpRectTool );
    }
    ti.lpszText = (LPTSTR)lpszText;
    return( (BOOL)::SendMessage( m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&ti ) );
}

void CToolTipCtrl::DelTool( CWnd *pWnd, UINT_PTR nIDTool )
/********************************************************/
{
    TOOLINFO ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.hwnd = pWnd->GetSafeHwnd();
    ti.uId = nIDTool;
    ::SendMessage( m_hWnd, TTM_DELTOOL, 0, (LPARAM)&ti );
}

void CToolTipCtrl::GetText( CString &str, CWnd *pWnd, UINT_PTR nIDTool ) const
/****************************************************************************/
{
    TOOLINFO ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.hwnd = pWnd->GetSafeHwnd();
    ti.uId = nIDTool;
    ti.lpszText = str.GetBuffer( 80 );
    ::SendMessage( m_hWnd, TTM_GETTEXT, 0, (LPARAM)&ti );
    str.ReleaseBuffer();
}

BOOL CToolTipCtrl::GetToolInfo( CToolInfo &ToolInfo, CWnd *pWnd, UINT_PTR nIDTool ) const
/***************************************************************************************/
{
    memset( &ToolInfo, 0, sizeof( CToolInfo ) );
    ToolInfo.cbSize = sizeof( TOOLINFO );
    ToolInfo.hwnd = pWnd->GetSafeHwnd();
    ToolInfo.uId = nIDTool;
    ToolInfo.lpszText = ToolInfo.szText;
    return( (BOOL)::SendMessage( m_hWnd, TTM_GETTOOLINFO, 0, (LPARAM)&ToolInfo ) );
}

BOOL CToolTipCtrl::HitTest( CWnd *pWnd, CPoint pt, LPTOOLINFO lpToolInfo ) const
/******************************************************************************/
{
    TTHITTESTINFO tthti;
    memset( &tthti, 0, sizeof( TTHITTESTINFO ) );
    tthti.hwnd = pWnd->GetSafeHwnd();
    tthti.pt.x = pt.x;
    tthti.pt.y = pt.y;
    tthti.ti.cbSize = sizeof( TOOLINFO );
    if( !::SendMessage( m_hWnd, TTM_HITTEST, 0, (LPARAM)&tthti ) ) {
        return( FALSE );
    }
    ASSERT( lpToolInfo != NULL );
    memcpy( lpToolInfo, &tthti.ti, sizeof( TOOLINFO ) );
    return( TRUE );
}

void CToolTipCtrl::SetToolRect( CWnd *pWnd, UINT_PTR nIDTool, LPCRECT lpRect )
/****************************************************************************/
{
    TOOLINFO ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.hwnd = pWnd->GetSafeHwnd();
    ti.uId = nIDTool;
    ASSERT( lpRect != NULL );
    ::CopyRect( &ti.rect, lpRect );
    ::SendMessage( m_hWnd, TTM_NEWTOOLRECT, 0, (LPARAM)&ti );
}

void CToolTipCtrl::UpdateTipText( LPCTSTR lpszText, CWnd *pWnd, UINT_PTR nIDTool )
/********************************************************************************/
{
    TOOLINFO ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.hwnd = pWnd->GetSafeHwnd();
    ti.uId = nIDTool;
    ti.lpszText = (LPTSTR)lpszText;
    ::SendMessage( m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti );
}

void CToolTipCtrl::UpdateTipText( UINT nIDText, CWnd *pWnd, UINT_PTR nIDTool )
/****************************************************************************/
{
    TOOLINFO ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    ti.hwnd = pWnd->GetSafeHwnd();
    ti.uId = nIDTool;
    ti.hinst = AfxFindResourceHandle( MAKEINTRESOURCE( nIDText ), RT_STRING );
    ti.lpszText = (LPTSTR)nIDText;
    ::SendMessage( m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti );
}
