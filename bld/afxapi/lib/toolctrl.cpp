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
* Description:  Implementation of CToolBarCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CToolBarCtrl, CWnd )

CToolBarCtrl::CToolBarCtrl()
/**************************/
{
}

BOOL CToolBarCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/**************************************************************************************/
{
    return( CWnd::Create( TOOLBARCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CToolBarCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                             CWnd *pParentWnd, UINT nID )
/*******************************************************/
{
    return( CWnd::CreateEx( dwExStyle, TOOLBARCLASSNAME, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

int CToolBarCtrl::AddBitmap( int nNumButtons, UINT nBitmapID )
/************************************************************/
{
    TBADDBITMAP tbab;
    tbab.hInst = AfxFindResourceHandle( MAKEINTRESOURCE( nBitmapID ), RT_BITMAP );
    tbab.nID = nBitmapID;
    return( ::SendMessage( m_hWnd, TB_ADDBITMAP, nNumButtons, (LPARAM)&tbab ) );
}

int CToolBarCtrl::AddBitmap( int nNumButtons, CBitmap *pBitmap )
/**************************************************************/
{
    TBADDBITMAP tbab;
    tbab.hInst = NULL;
    tbab.nID = (UINT_PTR)pBitmap->GetSafeHandle();
    return( ::SendMessage( m_hWnd, TB_ADDBITMAP, nNumButtons, (LPARAM)&tbab ) );
}

CString CToolBarCtrl::GetButtonText( int iButton ) const
/******************************************************/
{
    int nLength = (int)::SendMessage( m_hWnd, TB_GETBUTTONTEXT, iButton, 0L );
    if( nLength <= 0 ) {
        return( CString() );
    }

    CString str;
    LPTSTR  lpString = str.GetBuffer( nLength + 1 );
    ::SendMessage( m_hWnd, TB_GETBUTTONTEXT, iButton, (LPARAM)lpString );
    str.ReleaseBuffer();
    return( str );
}

BOOL CToolBarCtrl::GetPadding( int *pnHorzPadding, int *pnVertPadding ) const
/***************************************************************************/
{
    DWORD dwPadding = (DWORD)::SendMessage( m_hWnd, TB_GETPADDING, 0, 0L );
    if( pnHorzPadding != NULL ) {
        *pnHorzPadding = LOWORD( dwPadding );
    }
    if( pnVertPadding != NULL ) {
        *pnVertPadding = HIWORD( dwPadding );
    }
    return( TRUE );
}

void CToolBarCtrl::RestoreState( HKEY hKeyRoot, LPCTSTR lpszSubKey,
                                 LPCTSTR lpszValueName )
/******************************************************/
{
    TBSAVEPARAMS tbsp;
    tbsp.hkr = hKeyRoot;
    tbsp.pszSubKey = lpszSubKey;
    tbsp.pszValueName = lpszValueName;
    ::SendMessage( m_hWnd, TB_SAVERESTORE, FALSE, (LPARAM)&tbsp );
}

void CToolBarCtrl::SaveState( HKEY hKeyRoot, LPCTSTR lpszSubKey, LPCTSTR lpszValueName )
/**************************************************************************************/
{
    TBSAVEPARAMS tbsp;
    tbsp.hkr = hKeyRoot;
    tbsp.pszSubKey = lpszSubKey;
    tbsp.pszValueName = lpszValueName;
    ::SendMessage( m_hWnd, TB_SAVERESTORE, TRUE, (LPARAM)&tbsp );
}
