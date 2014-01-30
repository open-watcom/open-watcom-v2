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
* Description:  Implementation of COleException and AfxThrowOleException.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( COleException, CException )

COleException::COleException()
/****************************/
{
    m_sc = S_OK;
}

BOOL COleException::GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext )
/******************************************************************************************/
{
    if( pnHelpContext != NULL ) {
        *pnHelpContext = 0;
    }
    if( lpszError == NULL || nMaxError == 0 ) {
        return( TRUE );
    }
    return( ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, m_sc, LANG_SYSTEM_DEFAULT,
                             lpszError, nMaxError, NULL ) != 0 );
}

SCODE PASCAL COleException::Process( CException *pAnyException )
/**************************************************************/
{
    ASSERT( pAnyException != NULL );
    if( pAnyException->IsKindOf( RUNTIME_CLASS( COleException ) ) ) {
        return( ((COleException *)pAnyException)->m_sc );
    } else if( pAnyException->IsKindOf( RUNTIME_CLASS( CMemoryException ) ) ) {
        return( E_OUTOFMEMORY );
    } else if( pAnyException->IsKindOf( RUNTIME_CLASS( CNotSupportedException ) ) ) {
        return( E_NOTIMPL );
    } else {
        return( E_UNEXPECTED );
    }
}
