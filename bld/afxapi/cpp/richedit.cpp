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
* Description:  Implemention of CRichEditCtrl.
*
****************************************************************************/


#include "stdafx.h"
#include "edstate.h"

IMPLEMENT_DYNAMIC( CRichEditCtrl, CWnd )

static BOOL _InitRichEdit()
/*************************/
{
    _AFX_RICHEDIT_STATE *pState = _afxRichEditState.GetData();
    ASSERT( pState != NULL );
    if( pState->m_hInstRichEdit == NULL && pState->m_hInstRichEdit2 == NULL ) {
        pState->m_hInstRichEdit2 = ::LoadLibrary( _T("RICHED20.DLL") );
        if( pState->m_hInstRichEdit2 == NULL ) {
            pState->m_hInstRichEdit = ::LoadLibrary( _T("RICHED32.DLL") );
            if( pState->m_hInstRichEdit == NULL ) {
                return( FALSE );
            }
        }
    }
    return( TRUE );
}
    
CRichEditCtrl::CRichEditCtrl()
/****************************/
{
}

BOOL CRichEditCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/***************************************************************************************/
{
    if( !_InitRichEdit() ) {
        return( FALSE );
    }
    
    if( _afxRichEditState->m_hInstRichEdit2 != NULL ) {
        return( CWnd::Create( RICHEDIT_CLASS, NULL, dwStyle, rect, pParentWnd, nID ) );
    } else {
        return( CWnd::Create( _T("RICHEDIT"), NULL, dwStyle, rect, pParentWnd, nID ) );
    }
}

BOOL CRichEditCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                              CWnd *pParentWnd, UINT nID )
/********************************************************/
{
    if( !_InitRichEdit() ) {
        return( FALSE );
    }

    if( _afxRichEditState->m_hInstRichEdit2 != NULL ) {
        return( CWnd::CreateEx( dwExStyle, RICHEDIT_CLASS, NULL, dwStyle, rect,
                                pParentWnd, nID ) );
    } else {
        return( CWnd::CreateEx( dwExStyle, _T("RICHEDIT"), NULL, dwStyle, rect,
                                pParentWnd, nID ) );
    }
}

CString CRichEditCtrl::GetSelText() const
/***************************************/
{
    long nStartChar;
    long nEndChar;
    ::SendMessage( m_hWnd, EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar );
    if( nStartChar >= nEndChar ) {
        // The beginning should always be before the end.
        ASSERT( nStartChar == nEndChar );
        return( CString() );
    }

    LPSTR lpszBuf = new char[nEndChar - nStartChar + 1];
    memset( lpszBuf, 0, nEndChar - nStartChar + 1 );
    ::SendMessage( m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpszBuf );
    CString str( lpszBuf );
    delete [] lpszBuf;
    return( str );
}

int CRichEditCtrl::GetTextRange( int nFirst, int nLast, CString &refString ) const
/********************************************************************************/
{
    if( nFirst >= nLast && nLast >= 0 ) {
        // The beginning should always be before the end.
        ASSERT( nFirst == nLast );
        refString.Empty();
        return( 0 );
    }

    TEXTRANGE tr;
    tr.chrg.cpMin = nFirst;
    tr.chrg.cpMax = nLast;
    if( nLast < 0 ) {
        // The value -1 is used to indicate the end of the text.
        tr.lpstrText = refString.GetBuffer( GetTextLength() - nFirst + 1 );
    } else {
        tr.lpstrText = refString.GetBuffer( nLast - nFirst + 1 );
    }
    
    int nChars = ::SendMessage( m_hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr );
    refString.ReleaseBuffer( nChars );
    return( nChars );
}
