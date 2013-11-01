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
* Description:  Implementation of CBitmapButton.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CBitmapButton, CButton )

CBitmapButton::CBitmapButton()
/****************************/
{
}

void CBitmapButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
/***************************************************************/
{
    // If the bitmaps haven't been loaded yet, just don't draw anything.
    if( m_bitmap.m_hObject == NULL ) {
        return;
    }

    ASSERT( lpDrawItemStruct != NULL );
    CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
    CBitmap *pBitmap = &m_bitmap;
    if( lpDrawItemStruct->itemState & ODS_DISABLED ) {
        if( m_bitmapDisabled.m_hObject != NULL ) {
            pBitmap = &m_bitmapDisabled;
        }
    } else if( lpDrawItemStruct->itemState & ODS_SELECTED ) {
        if( m_bitmapSel.m_hObject != NULL ) {
            pBitmap = &m_bitmapSel;
        }
    } else if( lpDrawItemStruct->itemState & ODS_FOCUS ) {
        if( m_bitmapFocus.m_hObject != NULL ) {
            pBitmap = &m_bitmapFocus;
        }
    }

    BITMAP bm;
    pBitmap->GetBitmap( &bm );

    CDC dcMem;
    dcMem.CreateCompatibleDC( pDC );
    CBitmap *pOldBitmap = dcMem.SelectObject( pBitmap );
    pDC->BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY );
    dcMem.SelectObject( pOldBitmap );
}

BOOL CBitmapButton::AutoLoad( UINT nID, CWnd *pParent )
/*****************************************************/
{
    if( !SubclassDlgItem( nID, pParent ) ) {
        return( FALSE );
    }

    CString strText;
    GetWindowText( strText );
    ASSERT( strText.GetLength() > 0 );

    if( !LoadBitmaps( strText + _T("U"), strText + _T("D"), strText + _T("F"),
                      strText + _T("X") ) ) {
        return( FALSE );
    }

    SizeToContent();
    return( TRUE );
}

BOOL CBitmapButton::LoadBitmaps( LPCTSTR lpszBitmapResource,
                                 LPCTSTR lpszBitmapResourceSel,
                                 LPCTSTR lpszBitmapResourceFocus,
                                 LPCTSTR lpszBitmapResourceDisabled )
/*******************************************************************/
{
    ASSERT( lpszBitmapResource != NULL );
    if( !m_bitmap.LoadBitmap( lpszBitmapResource ) ) {
        return( FALSE );
    }
    if( lpszBitmapResourceSel != NULL ) {
        m_bitmapSel.LoadBitmap( lpszBitmapResourceSel );
    }
    if( lpszBitmapResourceFocus != NULL ) {
        m_bitmapFocus.LoadBitmap( lpszBitmapResourceFocus );
    }
    if( lpszBitmapResourceDisabled != NULL ) {
        m_bitmapDisabled.LoadBitmap( lpszBitmapResourceDisabled );
    }
    return( TRUE );
}

BOOL CBitmapButton::LoadBitmaps( UINT nIDBitmapResource,
                                 UINT nIDBitmapResourceSel,
                                 UINT nIDBitmapResourceFocus,
                                 UINT nIDBitmapResourceDisabled )
/***************************************************************/
{
    ASSERT( nIDBitmapResource != 0 );
    if( !m_bitmap.LoadBitmap( nIDBitmapResource ) ) {
        return( FALSE );
    }
    if( nIDBitmapResourceSel != 0 ) {
        m_bitmapSel.LoadBitmap( nIDBitmapResourceSel );
    }
    if( nIDBitmapResourceFocus != 0 ) {
        m_bitmapFocus.LoadBitmap( nIDBitmapResourceFocus );
    }
    if( nIDBitmapResourceDisabled != 0 ) {
        m_bitmapDisabled.LoadBitmap( nIDBitmapResourceDisabled );
    }
    return( TRUE );
}

void CBitmapButton::SizeToContent()
/*********************************/
{
    ASSERT( m_bitmap.m_hObject != NULL );
    BITMAP bm;
    m_bitmap.GetBitmap( &bm );
    ::SetWindowPos( m_hWnd, NULL, 0, 0, bm.bmWidth, bm.bmHeight,
                    SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER );
}
