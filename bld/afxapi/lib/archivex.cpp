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
* Description:  Implementation of CArchiveException.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CArchiveException, CException )

CArchiveException::CArchiveException( int cause, LPCTSTR lpszArchiveName )
/************************************************************************/
{
    ASSERT( cause >= none && cause <= badSchema );
    m_cause = cause;
    m_strFileName = lpszArchiveName;
}

BOOL CArchiveException::GetErrorMessage( LPTSTR lpszError, UINT nMaxError,
                                         PUINT pnHelpContext )
/************************************************************/
{
    if( pnHelpContext != NULL ) {
        *pnHelpContext = 0;
    }
    if( m_cause < none || m_cause > badSchema ) {
        if( lpszError != NULL && nMaxError > 0 ) {
            lpszError[0] = _T('\0');
        }
        return( FALSE );
    }
    if( lpszError != NULL && nMaxError > 0 ) {
        CString str;
        str.FormatMessage( AFX_IDP_ARCH_NONE + m_cause, (LPCTSTR)m_strFileName );

        int nLength = max( str.GetLength() + 1, nMaxError );
        _tcsncpy( lpszError, str, nLength );
    }
    return( TRUE );
}

#ifdef _DEBUG

void CArchiveException::Dump( CDumpContext &dc ) const
/****************************************************/
{
    CException::Dump( dc );

    dc << "m_cause = " << m_cause << "\n";
    dc << "m_strFileName = " << m_strFileName << "\n";
}

#endif // _DEBUG

void AfxThrowArchiveException( int cause, LPCTSTR lpszArchiveName )
/*****************************************************************/
{
    throw new CArchiveException( cause, lpszArchiveName );
}
