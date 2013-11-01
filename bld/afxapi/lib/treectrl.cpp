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
* Description:  Implementation of CTreeCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CTreeCtrl, CWnd )

CTreeCtrl::CTreeCtrl()
/********************/
{
}

BOOL CTreeCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/***********************************************************************************/
{
    return( CWnd::Create( WC_TREEVIEW, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CTreeCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                          CWnd *pParentWnd, UINT nID )
/****************************************************/
{
    return( CWnd::CreateEx( dwExStyle, WC_TREEVIEW, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

BOOL CTreeCtrl::GetCheck( HTREEITEM hItem ) const
/***********************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_STATE;
    tvi.hItem = hItem;
    tvi.stateMask = TVIS_STATEIMAGEMASK;
    ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi );
    return( tvi.state & TVIS_STATEIMAGEMASK == INDEXTOSTATEIMAGEMASK( 2 ) );
}

DWORD_PTR CTreeCtrl::GetItemData( HTREEITEM hItem ) const
/*******************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;
    ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi );
    return( tvi.lParam );
}

int CTreeCtrl::GetItemExpandedImageIndex( HTREEITEM hItem ) const
/***************************************************************/
{
    TVITEMEX tvix;
    memset( &tvix, 0, sizeof( TVITEMEX ) );
    tvix.mask = TVIF_HANDLE | TVIF_EXPANDEDIMAGE;
    tvix.hItem = hItem;
    ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvix );
    return( tvix.iExpandedImage );
}

BOOL CTreeCtrl::GetItemImage( HTREEITEM hItem, int &nImage, int &nSelectedImage ) const
/*************************************************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvi.hItem = hItem;
    if( !::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi ) ) {
        return( FALSE );
    }
    nImage = tvi.iImage;
    nSelectedImage = tvi.iSelectedImage;
    return( TRUE );
}

BOOL CTreeCtrl::GetItemPartRect( HTREEITEM hItem, int nPart, LPRECT lpRect ) const
/********************************************************************************/
{
    TVGETITEMPARTRECTINFO tvgipri;
    tvgipri.hti = hItem;
    tvgipri.prc = lpRect;
    tvgipri.partID = (TVITEMPART)nPart;
    return( (BOOL)::SendMessage( m_hWnd, TVM_GETITEMPARTRECT, 0, (LPARAM)&tvgipri ) );
}

UINT CTreeCtrl::GetItemState( HTREEITEM hItem, UINT nStateMask ) const
/********************************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_STATE;
    tvi.hItem = hItem;
    tvi.stateMask = nStateMask;
    ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi );
    return( tvi.state );
}

UINT CTreeCtrl::GetItemStateEx( HTREEITEM hItem ) const
/*****************************************************/
{
    TVITEMEX tvix;
    memset( &tvix, 0, sizeof( TVITEMEX ) );
    tvix.mask = TVIF_HANDLE | TVIF_STATEEX;
    tvix.hItem = hItem;
    ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvix );
    return( tvix.uStateEx );
}

CString CTreeCtrl::GetItemText( HTREEITEM hItem ) const
/*****************************************************/
{
    CString str;
    TVITEM  tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_TEXT;
    tvi.hItem = hItem;
    tvi.pszText = str.GetBuffer( 256 );
    tvi.cchTextMax = 255;
    ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi );
    str.ReleaseBuffer();
    return( str );
}

HTREEITEM CTreeCtrl::HitTest( CPoint pt, UINT *pFlags ) const
/***********************************************************/
{
    TVHITTESTINFO tvhti;
    memset( &tvhti, 0, sizeof( TVHITTESTINFO ) );
    tvhti.pt.x = pt.x;
    tvhti.pt.y = pt.y;
    ::SendMessage( m_hWnd, TVM_HITTEST, 0, (LPARAM)&tvhti );
    if( pFlags != NULL ) {
        *pFlags = tvhti.flags;
    }
    return( tvhti.hItem );
}

HTREEITEM CTreeCtrl::InsertItem( UINT nMask, LPCTSTR lpszItem, int nImage,
                                 int nSelectedImage, UINT nState, UINT nStateMask,
                                 LPARAM lParam, HTREEITEM hParent,
                                 HTREEITEM hInsertAfter )
/*******************************************************/
{
    TVINSERTSTRUCT tvis;
    memset( &tvis, 0, sizeof( TVINSERTSTRUCT ) );
    tvis.hParent = hParent;
    tvis.hInsertAfter = hInsertAfter;
    tvis.item.mask = nMask;
    tvis.item.state = nState;
    tvis.item.stateMask = nStateMask;
    if( lpszItem != NULL ) {
        tvis.item.pszText = (LPTSTR)lpszItem;
        tvis.item.cchTextMax = _tcslen( lpszItem );
    }
    tvis.item.iImage = nImage;
    tvis.item.iSelectedImage = nSelectedImage;
    tvis.item.lParam = lParam;
    return( (HTREEITEM)::SendMessage( m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis ) );
}

HTREEITEM CTreeCtrl::InsertItem( LPCTSTR lpszItem, HTREEITEM hParent,
                                 HTREEITEM hInsertAfter )
/*******************************************************/
{
    TVINSERTSTRUCT tvis;
    memset( &tvis, 0, sizeof( TVINSERTSTRUCT ) );
    tvis.hParent = hParent;
    tvis.hInsertAfter = hInsertAfter;
    tvis.item.mask = TVIF_TEXT;
    if( lpszItem != NULL ) {
        tvis.item.pszText = (LPTSTR)lpszItem;
        tvis.item.cchTextMax = _tcslen( lpszItem );
    }
    return( (HTREEITEM)::SendMessage( m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis ) );
}

HTREEITEM CTreeCtrl::InsertItem( LPCTSTR lpszItem, int nImage, int nSelectedImage,
                                 HTREEITEM hParent, HTREEITEM hInsertAfter )
/**************************************************************************/
{
    TVINSERTSTRUCT tvis;
    memset( &tvis, 0, sizeof( TVINSERTSTRUCT ) );
    tvis.hParent = hParent;
    tvis.hInsertAfter = hInsertAfter;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    if( lpszItem != NULL ) {
        tvis.item.pszText = (LPTSTR)lpszItem;
        tvis.item.cchTextMax = _tcslen( lpszItem );
    }
    tvis.item.iImage = nImage;
    tvis.item.iSelectedImage = nSelectedImage;
    return( (HTREEITEM)::SendMessage( m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis ) );
}

BOOL CTreeCtrl::ItemHasChildren( HTREEITEM hItem ) const
/******************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_CHILDREN;
    tvi.hItem = hItem;
    ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi );
    return( tvi.cChildren != 0 );
}

BOOL CTreeCtrl::SetCheck( HTREEITEM hItem, BOOL fCheck )
/******************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_STATE;
    tvi.hItem = hItem;
    tvi.stateMask = TVIS_STATEIMAGEMASK;
    tvi.state = INDEXTOSTATEIMAGEMASK( fCheck ? 2 : 1 );
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi ) );
}

BOOL CTreeCtrl::SetItem( HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
                         int nSelectedImage, UINT nState, UINT nStateMask,
                         LPARAM lParam )
/**************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = nMask;
    tvi.hItem = hItem;
    tvi.state = nState;
    tvi.stateMask = nStateMask;
    if( lpszItem != NULL ) {
        tvi.pszText = (LPTSTR)lpszItem;
        tvi.cchTextMax = _tcslen( lpszItem );
    }
    tvi.iImage = nImage;
    tvi.iSelectedImage = nSelectedImage;
    tvi.lParam = lParam;
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi ) );
}

BOOL CTreeCtrl::SetItemData( HTREEITEM hItem, DWORD_PTR dwData )
/**************************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;
    tvi.lParam = dwData;
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi ) );
}

BOOL CTreeCtrl::SetItemExpandedImageIndex( HTREEITEM hItem, int iExpandedImage )
/******************************************************************************/
{
    TVITEMEX tvix;
    memset( &tvix, 0, sizeof( TVITEMEX ) );
    tvix.mask = TVIF_HANDLE | TVIF_EXPANDEDIMAGE;
    tvix.hItem = hItem;
    tvix.iExpandedImage = iExpandedImage;
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvix ) );
}

BOOL CTreeCtrl::SetItemImage( HTREEITEM hItem, int nImage, int nSelectedImage )
/*****************************************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvi.hItem = hItem;
    tvi.iImage = nImage;
    tvi.iSelectedImage = nSelectedImage;
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi ) );
}

BOOL CTreeCtrl::SetItemState( HTREEITEM hItem, UINT nState, UINT nStateMask )
/***************************************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_STATE;
    tvi.hItem = hItem;
    tvi.stateMask = nStateMask;
    tvi.state = nState;
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi ) );
}

BOOL CTreeCtrl::SetItemStateEx( HTREEITEM hItem, UINT nStateEx )
/**************************************************************/
{
    TVITEMEX tvix;
    memset( &tvix, 0, sizeof( TVITEMEX ) );
    tvix.mask = TVIF_HANDLE | TVIF_STATEEX;
    tvix.hItem = hItem;
    tvix.uStateEx = nStateEx;
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvix ) );
}

BOOL CTreeCtrl::SetItemText( HTREEITEM hItem, LPCTSTR lpszItem )
/**************************************************************/
{
    TVITEM tvi;
    memset( &tvi, 0, sizeof( TVITEM ) );
    tvi.mask = TVIF_HANDLE | TVIF_TEXT;
    tvi.hItem = hItem;
    if( lpszItem != NULL ) {
        tvi.pszText = (LPTSTR)lpszItem;
        tvi.cchTextMax = _tcslen( lpszItem );
    }
    return( (BOOL)::SendMessage( m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi ) );
}
