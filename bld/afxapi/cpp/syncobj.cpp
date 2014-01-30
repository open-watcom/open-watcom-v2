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
* Description:  Implementation of CSyncObject.
*
****************************************************************************/


#include "stdafx.h"
#include <afxmt.h>

IMPLEMENT_DYNAMIC( CSyncObject, CObject )

CSyncObject::CSyncObject( LPCTSTR pstrName )
/******************************************/
{
    UNUSED_ALWAYS( pstrName );
    m_hObject = NULL;
}

CSyncObject::~CSyncObject()
/*************************/
{
    if( m_hObject != NULL ) {
        ::CloseHandle( m_hObject );
    }
}

BOOL CSyncObject::Lock( DWORD dwTimeout )
/***************************************/
{
    DWORD dwRet = ::WaitForSingleObject( m_hObject, dwTimeout );
    if( dwRet != WAIT_OBJECT_0 && dwRet != WAIT_ABANDONED ) {
        return( FALSE );
    }
    return( TRUE );
}

BOOL CSyncObject::Unlock( LONG lCount, LPLONG lpPrevCount )
/*********************************************************/
{
    UNUSED_ALWAYS( lCount );
    UNUSED_ALWAYS( lpPrevCount );
    return( TRUE );
}

#ifdef _DEBUG

void CSyncObject::Dump( CDumpContext &dc ) const
/**********************************************/
{
    CObject::Dump( dc );

    dc << "m_hObject = " << m_hObject << "\n";
}

#endif // _DEBUG
