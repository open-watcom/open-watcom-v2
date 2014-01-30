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
* Description:  Implementation of CStatusBarCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CStatusBarCtrl, CWnd )

CStatusBarCtrl::CStatusBarCtrl()
/******************************/
{
}

BOOL CStatusBarCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd,
                             UINT nID )
/*************************************/
{
    return( CWnd::Create( STATUSCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CStatusBarCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                               CWnd *pParentWnd, UINT nID )
/*********************************************************/
{
    return( CWnd::CreateEx( dwExStyle, STATUSCLASSNAME, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

void CStatusBarCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
/****************************************************************/
{
    UNUSED_ALWAYS( lpDrawItemStruct );
}

BOOL CStatusBarCtrl::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam,
                                    LRESULT *pResult )
/****************************************************/
{
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( pResult );
    
    if( message == WM_DRAWITEM ) {
        DrawItem( (LPDRAWITEMSTRUCT)lParam );
        return( TRUE );
    }
    return( FALSE );
}

BOOL CStatusBarCtrl::GetBorders( int &nHorz, int &nVert, int &nSpacing ) const
/****************************************************************************/
{
    int nBorders[3];
    if( !::SendMessage( m_hWnd, SB_GETBORDERS, 0, (LPARAM)nBorders ) ) {
        return( FALSE );
    }
    nHorz = nBorders[0];
    nVert = nBorders[1];
    nSpacing = nBorders[2];
    return( TRUE );
}

CString CStatusBarCtrl::GetText( int nPane, int *pType ) const
/************************************************************/
{
    DWORD   dwResult = (int)::SendMessage( m_hWnd, SB_GETTEXTLENGTH, nPane, 0L );
    CString str;
    LPTSTR  lpString = str.GetBuffer( LOWORD( dwResult ) + 1 );
    ::SendMessage( m_hWnd, SB_GETTEXT, nPane, (LPARAM)lpString );
    str.ReleaseBuffer();
    if( pType != NULL ) {
        *pType = HIWORD( dwResult );
    }
    return( str );
}

int CStatusBarCtrl::GetText( LPTSTR lpszText, int nPane, int *pType ) const
/*************************************************************************/
{
    DWORD dwResult = (DWORD)::SendMessage( m_hWnd, SB_GETTEXT, nPane, (LPARAM)lpszText );
    if( pType != NULL ) {
        *pType = HIWORD( dwResult );
    }
    return( LOWORD( dwResult ) );
}

int CStatusBarCtrl::GetTextLength( int nPane, int *pType ) const
/**************************************************************/
{
    DWORD dwResult = (DWORD)::SendMessage( m_hWnd, SB_GETTEXTLENGTH, nPane, 0L );
    if( pType != NULL ) {
        *pType = HIWORD( dwResult );
    }
    return( LOWORD( dwResult ) );
}
