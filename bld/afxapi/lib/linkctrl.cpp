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
* Description:  Implementation of CLinkCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CLinkCtrl, CWnd )

CLinkCtrl::CLinkCtrl()
/********************/
{
}

BOOL CLinkCtrl::Create( LPCTSTR lpszLinkMarkup, DWORD dwStyle, const RECT &rect,
                        CWnd *pParentWnd, UINT nID )
/**************************************************/
{
#ifdef _UNICODE
    return( CWnd::Create( WC_LINK, lpszLinkMarkup, dwStyle, rect, pParentWnd, nID ) );
#else
    UNUSED_ALWAYS( lpszLinkMarkup );
    UNUSED_ALWAYS( dwStyle );
    UNUSED_ALWAYS( rect );
    UNUSED_ALWAYS( pParentWnd );
    UNUSED_ALWAYS( nID );
    return( FALSE );
#endif
}

BOOL CLinkCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/***********************************************************************************/
{
#ifdef _UNICODE
    return( CWnd::Create( WC_LINK, NULL, dwStyle, rect, pParentWnd, nID ) );
#else
    UNUSED_ALWAYS( dwStyle );
    UNUSED_ALWAYS( rect );
    UNUSED_ALWAYS( pParentWnd );
    UNUSED_ALWAYS( nID );
    return( FALSE );
#endif
}

BOOL CLinkCtrl::CreateEx( LPCTSTR lpszLinkMarkup, DWORD dwExStyle, DWORD dwStyle,
                          const RECT &rect, CWnd *pParentWnd, UINT nID )
/**********************************************************************/
{
#ifdef _UNICODE
    return( CWnd::CreateEx( dwExStyle, WC_LINK, lpszLinkMarkup, dwStyle, rect,
                            pParentWnd, nID ) );
#else
    UNUSED_ALWAYS( lpszLinkMarkup );
    UNUSED_ALWAYS( dwExStyle );
    UNUSED_ALWAYS( dwStyle );
    UNUSED_ALWAYS( rect );
    UNUSED_ALWAYS( pParentWnd );
    UNUSED_ALWAYS( nID );
    return( FALSE );
#endif
}

BOOL CLinkCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                          CWnd *pParentWnd, UINT nID )
/****************************************************/
{
#ifdef _UNICODE
    return( CWnd::CreateEx( dwExStyle, WC_LINK, NULL, dwStyle, rect, pParentWnd, nID ) );
#else
    UNUSED_ALWAYS( dwExStyle );
    UNUSED_ALWAYS( dwStyle );
    UNUSED_ALWAYS( rect );
    UNUSED_ALWAYS( pParentWnd );
    UNUSED_ALWAYS( nID );
    return( FALSE );
#endif
}

BOOL CLinkCtrl::GetItemID( int iLink, CString &strID ) const
/**********************************************************/
{
    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_ITEMID;
    li.iLink = iLink;
    if( !::SendMessage( m_hWnd, LM_GETITEM, 0, (LPARAM)&li ) ) {
        return( FALSE );
    }
    strID = li.szID;
    return( TRUE );
}

BOOL CLinkCtrl::GetItemID( int iLink, LPWSTR szID, UINT cchID ) const
/*******************************************************************/
{
    if( szID == NULL || cchID < MAX_LINKID_TEXT ) {
        return( FALSE );
    }

    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_ITEMID;
    li.iLink = iLink;
    if( !::SendMessage( m_hWnd, LM_GETITEM, 0, (LPARAM)&li ) ) {
        return( FALSE );
    }
    wcscpy( szID, li.szID );
    return( TRUE );
}

BOOL CLinkCtrl::GetItemState( int iLink, UINT *pnState, UINT stateMask ) const
/****************************************************************************/
{
    if( pnState == NULL ) {
        return( FALSE );
    }

    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_STATE;
    li.iLink = iLink;
    li.stateMask = stateMask;
    if( !::SendMessage( m_hWnd, LM_GETITEM, 0, (LPARAM)&li ) ) {
        return( FALSE );
    }
    *pnState = li.state;
    return( TRUE );
}

BOOL CLinkCtrl::GetItemUrl( int iLink, CString &strUrl ) const
/************************************************************/
{
    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_URL;
    li.iLink = iLink;
    if( !::SendMessage( m_hWnd, LM_GETITEM, 0, (LPARAM)&li ) ) {
        return( FALSE );
    }
    strUrl = li.szUrl;
    return( TRUE );
}

BOOL CLinkCtrl::GetItemUrl( int iLink, LPWSTR szUrl, UINT cchUrl ) const
/**********************************************************************/
{
    if( szUrl == NULL || cchUrl < L_MAX_URL_LENGTH ) {
        return( FALSE );
    }

    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_URL;
    li.iLink = iLink;
    if( !::SendMessage( m_hWnd, LM_GETITEM, 0, (LPARAM)&li ) ) {
        return( FALSE );
    }
    wcscpy( szUrl, li.szUrl );
    return( TRUE );
}

BOOL CLinkCtrl::SetItemID( int iLink, LPCWSTR szID )
/**************************************************/
{
    if( wcslen( szID ) > MAX_LINKID_TEXT ) {
        return( FALSE );
    }
    
    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_ITEMID;
    li.iLink = iLink;
    wcscpy( li.szID, szID );
    return( ::SendMessage( m_hWnd, LM_SETITEM, 0, (LPARAM)&li ) );
}

BOOL CLinkCtrl::SetItemState( int iLink, UINT state, UINT stateMask )
/*******************************************************************/
{
    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_STATE;
    li.iLink = iLink;
    li.state = state;
    li.stateMask = stateMask;
    return( ::SendMessage( m_hWnd, LM_SETITEM, 0, (LPARAM)&li ) );
}

BOOL CLinkCtrl::SetItemUrl( int iLink, LPCWSTR szUrl )
/****************************************************/
{
    if( wcslen( szUrl ) > L_MAX_URL_LENGTH ) {
        return( FALSE );
    }
    
    LITEM li;
    memset( &li, 0, sizeof( LITEM ) );
    li.mask = LIF_ITEMINDEX | LIF_URL;
    li.iLink = iLink;
    wcscpy( li.szUrl, szUrl );
    return( ::SendMessage( m_hWnd, LM_SETITEM, 0, (LPARAM)&li ) );
}
