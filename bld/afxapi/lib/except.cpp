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
* Description:  Implementation of CException.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CException, CObject )

CException::CException()
/**********************/
{
    m_bAutoDelete = TRUE;
}

CException::CException( BOOL bAutoDelete )
/****************************************/
{
    m_bAutoDelete = bAutoDelete;
}

BOOL CException::GetErrorMessage( LPTSTR lpszError, UINT nMaxError,
                                  PUINT pnHelpContext )
/*****************************************************/
{
    if( lpszError != NULL && nMaxError > 0 ) {
        lpszError[0] = _T('\0');
    }
    if( pnHelpContext != NULL ) {
        *pnHelpContext = 0;
    }
    return( FALSE );
}

int CException::ReportError( UINT nType, UINT nMessageID )
/********************************************************/
{
    UNUSED_ALWAYS( nType );
    UNUSED_ALWAYS( nMessageID );
    return( 0 );
}

#ifdef _DEBUG

void CException::Dump( CDumpContext &dc ) const
/*********************************************/
{
    CObject::Dump( dc );

    dc << "m_bAutoDelete = " << m_bAutoDelete << "\n";
}

#endif // _DEBUG

void CException::Delete()
/***********************/
{
    if( m_bAutoDelete ) {
        delete this;
    }
}
