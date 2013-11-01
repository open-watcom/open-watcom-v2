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
* Description:  Implementation of CListBox.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CListBox, CWnd )

CListBox::CListBox()
/******************/
{
}

int CListBox::CharToItem( UINT nKey, UINT nIndex )
/************************************************/
{
    UNUSED_ALWAYS( nKey );
    UNUSED_ALWAYS( nIndex );
    return( -1 );
}

int CListBox::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct )
/******************************************************************/
{
    UNUSED_ALWAYS( lpCompareItemStruct );
    return( 0 );
}

BOOL CListBox::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/**********************************************************************************/
{
    return( CWnd::Create( _T("LISTBOX"), NULL, dwStyle, rect, pParentWnd, nID ) );
}

void CListBox::DeleteItem( LPDELETEITEMSTRUCT lpDeleteItemStruct )
/****************************************************************/
{
    UNUSED_ALWAYS( lpDeleteItemStruct );
}

void CListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
/**********************************************************/
{
    UNUSED_ALWAYS( lpDrawItemStruct );
}

void CListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
/*******************************************************************/
{
    UNUSED_ALWAYS( lpMeasureItemStruct );
}

int CListBox::VKeyToItem( UINT nKey, UINT nIndex )
/************************************************/
{
    UNUSED_ALWAYS( nKey );
    UNUSED_ALWAYS( nIndex );
    return( -1 );
}

BOOL CListBox::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam,
                              LRESULT *pResult )
/**********************************************/
{
    switch( message ) {
    case WM_CHARTOITEM:
        *pResult = CharToItem( LOWORD( wParam ), HIWORD( wParam ) );
        return( TRUE );
    case WM_COMPAREITEM:
        *pResult = CompareItem( (LPCOMPAREITEMSTRUCT)lParam );
        return( TRUE );
    case WM_DELETEITEM:
        DeleteItem( (LPDELETEITEMSTRUCT)lParam );
        return( TRUE );
    case WM_DRAWITEM:
        DrawItem( (LPDRAWITEMSTRUCT)lParam );
        return( TRUE );
    case WM_MEASUREITEM:
        MeasureItem( (LPMEASUREITEMSTRUCT)lParam );
        return( TRUE );
    case WM_VKEYTOITEM:
        *pResult = VKeyToItem( LOWORD( wParam ), HIWORD( wParam ) );
        return( TRUE );
    }
    return( FALSE );
}

void CListBox::GetText( int nIndex, CString &rString ) const
/**********************************************************/
{
    int nLength = (int)::SendMessage( m_hWnd, LB_GETTEXTLEN, nIndex, 0L );
    LPTSTR lpszBuffer = rString.GetBuffer( nLength + 1 );
    ::SendMessage( m_hWnd, LB_GETTEXT, nIndex, (LPARAM)lpszBuffer );
    rString.ReleaseBuffer();
}
