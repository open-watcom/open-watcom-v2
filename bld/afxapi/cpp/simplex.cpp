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
* Description:  Implementation of CSimpleException.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CSimpleException, CException )

CSimpleException::CSimpleException()
/**********************************/
{
    m_bInitialized = FALSE;
    m_bLoaded = FALSE;
    m_szMessage[0] = _T('\0');
}

CSimpleException::CSimpleException( BOOL bAutoDelete ) : CException( bAutoDelete )
/********************************************************************************/
{
    m_bInitialized = FALSE;
    m_bLoaded = FALSE;
    m_szMessage[0] = _T('\0');
}

BOOL CSimpleException::GetErrorMessage( LPTSTR lpszError, UINT nMaxError,
                                        PUINT pnHelpContext )
/***********************************************************/
{
    if( pnHelpContext != NULL ) {
        *pnHelpContext = 0;
    }
    if( !m_bInitialized ) {
        m_bInitialized = TRUE;
        HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( m_nResourceID ), RT_STRING );
        if( ::LoadString( hInstance, m_nResourceID, m_szMessage, 128 ) > 0 ) {
            m_bLoaded = TRUE;
        }
    }
    if( m_bLoaded ) {
        if( lpszError != NULL ) {
            _tcsncpy( lpszError, m_szMessage, nMaxError );
        }
        return( TRUE );
    } else {
        if( lpszError != NULL && nMaxError > 0 ) {
            lpszError[0] = _T('\0');
        }
        return( FALSE );
    }
}

#ifdef _DEBUG

void CSimpleException::Dump( CDumpContext &dc ) const
/***************************************************/
{
    CException::Dump( dc );

    dc << "m_bInitialized = " << m_bInitialized << "\n";
    dc << "m_bLoaded = " << m_bLoaded << "\n";
    dc << "m_szMessage = " << m_szMessage << "\n";
    dc << "m_nResourceID = " << m_nResourceID << "\n";
}

#endif // _DEBUG
