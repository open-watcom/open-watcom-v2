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
* Description:  Implementation of COleDispatchException and
*               AfxThrowOleDispatchException.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( COleDispatchException, CException )

COleDispatchException::COleDispatchException( LPCTSTR lpszDescription, UINT nHelpID,
                                              WORD wCode )
/********************************************************/
{
    m_strDescription = lpszDescription;
    m_dwHelpContext = nHelpID;
    m_wCode = wCode;
}

BOOL COleDispatchException::GetErrorMessage( LPTSTR lpszError, UINT nMaxError,
                                             PUINT pnHelpContext )
/****************************************************************/
{
    if( pnHelpContext != NULL ) {
        *pnHelpContext = m_dwHelpContext;
    }
    if( lpszError != NULL && nMaxError > 0 ) {
        if( nMaxError < m_strDescription.GetLength() ) {
            _tcscpy( lpszError, m_strDescription );
        } else {
            _tcsncpy( lpszError, m_strDescription, nMaxError - 1 );
            lpszError[nMaxError - 1] = _T('\0');
        }
    }
    return( TRUE );
}

void AFXAPI AfxThrowOleDispatchException( WORD wCode, LPCTSTR lpszDescription, UINT nHelpID )
/*******************************************************************************************/
{
    throw new COleDispatchException( lpszDescription, nHelpID, wCode );
}

void AFXAPI AfxThrowOleDispatchException( WORD wCode, UINT nDescriptionID, UINT nHelpID )
/***************************************************************************************/
{
    TCHAR szDescription[1024];
    HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nDescriptionID ), RT_STRING );
    ::LoadString( hInstance, nDescriptionID, szDescription, 1024 );
    throw new COleDispatchException( szDescription, nHelpID, wCode );
}
