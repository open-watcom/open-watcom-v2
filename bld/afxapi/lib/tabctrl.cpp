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
* Description:  Implementation of CTabCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CTabCtrl, CWnd )

CTabCtrl::CTabCtrl()
/******************/
{
}

BOOL CTabCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/**********************************************************************************/
{
    return( CWnd::Create( WC_TABCONTROL, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CTabCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                         CWnd *pParentWnd, UINT nID )
/***************************************************/
{
    return( CWnd::CreateEx( dwExStyle, WC_TABCONTROL, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

void CTabCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
/**********************************************************/
{
    UNUSED_ALWAYS( lpDrawItemStruct );
}

BOOL CTabCtrl::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam,
                              LRESULT *pResult )
/**********************************************/
{
    if( message == WM_DRAWITEM ) {
        DrawItem( (LPDRAWITEMSTRUCT)lParam );
        return( TRUE );
    }
    return( CWnd::OnChildNotify( message, wParam, lParam, pResult ) );
}

DWORD CTabCtrl::GetItemState( int nItem, DWORD dwMask ) const
/***********************************************************/
{
    TCITEM tci;
    memset( &tci, 0, sizeof( TCITEM ) );
    tci.mask = TCIF_STATE;
    tci.dwStateMask = dwMask;
    ::SendMessage( m_hWnd, TCM_GETITEM, nItem, (LPARAM)&tci );
    return( tci.dwState );
}

LONG CTabCtrl::InsertItem( int nItem, LPCTSTR lpszItem )
/******************************************************/
{
    TCITEM tci;
    memset( &tci, 0, sizeof( TCITEM ) );
    tci.mask = TCIF_TEXT;
    tci.pszText = (LPTSTR)lpszItem;
    return( ::SendMessage( m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)&tci ) );
}

LONG CTabCtrl::InsertItem( int nItem, LPCTSTR lpszItem, int nImage )
/******************************************************************/
{
    TCITEM tci;
    memset( &tci, 0, sizeof( TCITEM ) );
    tci.mask = TCIF_TEXT | TCIF_IMAGE;
    tci.pszText = (LPTSTR)lpszItem;
    tci.iImage = nImage;
    return( ::SendMessage( m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)&tci ) );
}

LONG CTabCtrl::InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, int nImage,
                           LPARAM lParam )
/****************************************/
{
    TCITEM tci;
    memset( &tci, 0, sizeof( TCITEM ) );
    tci.mask = nMask;
    tci.pszText = (LPTSTR)lpszItem;
    tci.iImage = nImage;
    tci.lParam = lParam;
    return( ::SendMessage( m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)&tci ) );
}

LONG CTabCtrl::InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, int nImage,
                           LPARAM lParam, DWORD dwState, DWORD dwStateMask )
/**************************************************************************/
{
    TCITEM tci;
    memset( &tci, 0, sizeof( TCITEM ) );
    tci.mask = nMask;
    tci.dwState = dwState;
    tci.dwStateMask = dwStateMask;
    tci.pszText = (LPTSTR)lpszItem;
    tci.iImage = nImage;
    tci.lParam = lParam;
    return( ::SendMessage( m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)&tci ) );
}

BOOL CTabCtrl::SetItemState( int nItem, DWORD dwMask, DWORD dwState )
/*******************************************************************/
{
    TCITEM tci;
    memset( &tci, 0, sizeof( TCITEM ) );
    tci.mask = TCIF_STATE;
    tci.dwState = dwState;
    tci.dwStateMask = dwMask;
    return( ::SendMessage( m_hWnd, TCM_SETITEM, nItem, (LPARAM)&tci ) );
}
