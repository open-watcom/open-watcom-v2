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
* Description:  Implementation of CListCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CListCtrl, CWnd )

CListCtrl::CListCtrl()
/********************/
{
}

BOOL CListCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/***********************************************************************************/
{
    return( CWnd::Create( WC_LISTVIEW, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CListCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                          CWnd *pParentWnd, UINT nID )
/****************************************************/
{
    return( CWnd::CreateEx( dwExStyle, WC_LISTVIEW, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

void CListCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
/***********************************************************/
{
    UNUSED_ALWAYS( lpDrawItemStruct );
}

BOOL CListCtrl::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam,
                               LRESULT *pResult )
/***********************************************/
{
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( pResult );
    
    if( message == WM_DRAWITEM ) {
        DrawItem( (LPDRAWITEMSTRUCT)lParam );
        return( TRUE );
    }
    return( FALSE );
}

CString CListCtrl::GetEmptyText() const
/*************************************/
{
    WCHAR szBuffer[256];
    memset( szBuffer, 0, 256 * sizeof( WCHAR ) );
    ::SendMessage( m_hWnd, LVM_GETEMPTYTEXT, 255, (LPARAM)szBuffer );
    return( szBuffer );
}

DWORD_PTR CListCtrl::GetItemData( int nItem ) const
/*************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.mask = LVIF_PARAM;
    lvi.iItem = nItem;
    ::SendMessage( m_hWnd, LVM_GETITEM, 0, (LPARAM)&lvi );
    return( lvi.lParam );
}

int CListCtrl::GetItemText( int nItem, int nSubItem, LPTSTR lpszText, int nLen ) const
/************************************************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.iSubItem = nSubItem;
    lvi.pszText = lpszText;
    lvi.cchTextMax = nLen;
    return( (int)::SendMessage( m_hWnd, LVM_GETITEMTEXT, nItem, (LPARAM)&lvi ) );
}

CString CListCtrl::GetItemText( int nItem, int nSubItem ) const
/*************************************************************/
{
    CString str;
    LVITEM  lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.iSubItem = nSubItem;
    lvi.pszText = str.GetBuffer( 256 );
    lvi.cchTextMax = 255;
    int nResult = (int)::SendMessage( m_hWnd, LVM_GETITEMTEXT, nItem, (LPARAM)&lvi );
    str.ReleaseBuffer( nResult );
    return( str );
}

int CListCtrl::HitTest( CPoint pt, UINT *pFlags ) const
/*****************************************************/
{
    LVHITTESTINFO lvhti;
    memset( &lvhti, 0, sizeof( LVHITTESTINFO ) );
    lvhti.pt.x = pt.x;
    lvhti.pt.y = pt.y;
    int nResult = (int)::SendMessage( m_hWnd, LVM_HITTEST, 0, (LPARAM)&lvhti );
    if( pFlags != NULL ) {
        *pFlags = lvhti.flags;
    }
    return( nResult );
}

int CListCtrl::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat,
                             int nWidth, int nSubItem )
/*****************************************************/
{
    LVCOLUMN lvc;
    memset( &lvc, 0, sizeof( LVCOLUMN ) );
    lvc.mask = LVCF_FMT;
    lvc.fmt = nFormat;
    if( nWidth >= 0 ) {
        lvc.mask |= LVCF_WIDTH;
        lvc.cx = nWidth;
    }
    if( lpszColumnHeading != NULL ) {
        lvc.mask |= LVCF_TEXT;
        lvc.pszText = (LPTSTR)lpszColumnHeading;
        lvc.cchTextMax = _tcslen( lpszColumnHeading );
    }
    if( nSubItem >= 0 ) {
        lvc.mask |= LVCF_SUBITEM;
        lvc.iSubItem = nSubItem;
    }
    return( (int)::SendMessage( m_hWnd, LVM_INSERTCOLUMN, nCol, (LPARAM)&lvc ) );
}

int CListCtrl::InsertItem( int nItem, LPCTSTR lpszItem )
/******************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.mask = LVIF_TEXT;
    lvi.iItem = nItem;
    if( lpszItem != NULL ) {
        lvi.pszText = (LPTSTR)lpszItem;
        lvi.cchTextMax = _tcslen( lpszItem );
    }
    return( (int)::SendMessage( m_hWnd, LVM_INSERTITEM, 0, (LPARAM)&lvi ) );
}

int CListCtrl::InsertItem( int nItem, LPCTSTR lpszItem, int nImage )
/******************************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.mask = LVIF_TEXT | LVIF_IMAGE;
    lvi.iItem = nItem;
    if( lpszItem != NULL ) {
        lvi.pszText = (LPTSTR)lpszItem;
        lvi.cchTextMax = _tcslen( lpszItem );
    }
    lvi.iImage = nImage;
    return( (int)::SendMessage( m_hWnd, LVM_INSERTITEM, 0, (LPARAM)&lvi ) );
}

int CListCtrl::InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
                           UINT nStateMask, int nImage, LPARAM lParam )
/*********************************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.mask = nMask;
    lvi.iItem = nItem;
    lvi.state = nState;
    lvi.stateMask = nStateMask;
    if( lpszItem != NULL ) {
        lvi.pszText = (LPTSTR)lpszItem;
        lvi.cchTextMax = _tcslen( lpszItem );
    }
    lvi.iImage = nImage;
    lvi.lParam = lParam;
    return( (int)::SendMessage( m_hWnd, LVM_INSERTITEM, 0, (LPARAM)&lvi ) );
}

BOOL CListCtrl::SetBkImage( HBITMAP hbm, BOOL fTile, int xOffsetPercent,
                            int yOffsetPercent )
/**********************************************/
{
    LVBKIMAGE lvbi;
    memset( &lvbi, 0, sizeof( LVBKIMAGE ) );
    lvbi.ulFlags = LVBKIF_SOURCE_HBITMAP;
    if( fTile ) {
        lvbi.ulFlags |= LVBKIF_STYLE_TILE;
    }
    lvbi.hbm = hbm;
    lvbi.xOffsetPercent = xOffsetPercent;
    lvbi.yOffsetPercent = yOffsetPercent;
    return( ::SendMessage( m_hWnd, LVM_SETBKIMAGE, 0, (LPARAM)&lvbi ) );
}

BOOL CListCtrl::SetBkImage( LPTSTR pszUrl, BOOL fTile, int xOffsetPercent,
                            int yOffsetPercent )
/**********************************************/
{
    LVBKIMAGE lvbi;
    memset( &lvbi, 0, sizeof( LVBKIMAGE ) );
    lvbi.ulFlags = LVBKIF_SOURCE_URL;
    if( fTile ) {
        lvbi.ulFlags |= LVBKIF_STYLE_TILE;
    }
    if( pszUrl != NULL ) {
        lvbi.pszImage = pszUrl;
        lvbi.cchImageMax = _tcslen( pszUrl );
    }
    lvbi.xOffsetPercent = xOffsetPercent;
    lvbi.yOffsetPercent = yOffsetPercent;
    return( ::SendMessage( m_hWnd, LVM_SETBKIMAGE, 0, (LPARAM)&lvbi ) );
}

BOOL CListCtrl::SetCheck( int nItem, BOOL fCheck )
/************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.state = INDEXTOSTATEIMAGEMASK( fCheck ? 2 : 1 );
    lvi.stateMask = LVIS_STATEIMAGEMASK;
    return( ::SendMessage( m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)&lvi ) );
}

BOOL CListCtrl::SetItem( int nItem, int nSubItem, UINT nMask, LPCTSTR pszItem,
                         int nImage, UINT nState, UINT nStateMask, LPARAM lParam )
/********************************************************************************/
{
    LVITEM lvi;
    memcpy( &lvi, 0, sizeof( LVITEM ) );
    lvi.mask = nMask;
    lvi.iItem = nItem;
    lvi.iSubItem = nSubItem;
    lvi.state = nState;
    lvi.stateMask = nStateMask;
    if( pszItem != NULL ) {
        lvi.pszText = (LPTSTR)pszItem;
        lvi.cchTextMax = _tcslen( pszItem );
    }
    lvi.iImage = nImage;
    lvi.lParam = lParam;
    return( ::SendMessage( m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi ) );
}

BOOL CListCtrl::SetItem( int nItem, int nSubItem, UINT nMask, LPCTSTR pszItem,
                         int nImage, UINT nState, UINT nStateMask, LPARAM lParam,
                         int nIndent )
/************************************/
{
    LVITEM lvi;
    memcpy( &lvi, 0, sizeof( LVITEM ) );
    lvi.mask = nMask;
    lvi.iItem = nItem;
    lvi.iSubItem = nSubItem;
    lvi.state = nState;
    lvi.stateMask = nStateMask;
    if( pszItem != NULL ) {
        lvi.pszText = (LPTSTR)pszItem;
        lvi.cchTextMax = _tcslen( pszItem );
    }
    lvi.iImage = nImage;
    lvi.lParam = lParam;
    lvi.iIndent = nIndent;
    return( ::SendMessage( m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi ) );
}

BOOL CListCtrl::SetItemData( int nItem, DWORD_PTR dwData )
/********************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.mask = LVIF_PARAM;
    lvi.iItem = nItem;
    lvi.lParam = dwData;
    return( ::SendMessage( m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi ) );
}

BOOL CListCtrl::SetItemIndexState( PLVITEMINDEX pItemIndex, DWORD dwState, DWORD dwMask )
/***************************************************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.state = dwState;
    lvi.stateMask = dwMask;
    return( ::SendMessage( m_hWnd, LVM_SETITEMINDEXSTATE, (WPARAM)pItemIndex,
                           (LPARAM)&lvi ) );
}

BOOL CListCtrl::SetItemState( int nItem, UINT nState, UINT nMask )
/****************************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.state = nState;
    lvi.stateMask = nMask;
    return( ::SendMessage( m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)&lvi ) );
}

BOOL CListCtrl::SetItemText( int nItem, int nSubItem, LPCTSTR lpszText )
/**********************************************************************/
{
    LVITEM lvi;
    memset( &lvi, 0, sizeof( LVITEM ) );
    lvi.iSubItem = nSubItem;
    lvi.pszText = (LPTSTR)lpszText;
    return( ::SendMessage( m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi ) );
}
