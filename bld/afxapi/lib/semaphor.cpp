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
* Description:  Implementation of CSemaphore.
*
****************************************************************************/


#include "stdafx.h"
#include <afxmt.h>

IMPLEMENT_DYNAMIC( CSemaphore, CSyncObject )

CSemaphore::CSemaphore( LONG lInitialCount, LONG lMaxCount, LPCTSTR pstrName,
                        LPSECURITY_ATTRIBUTES lpsaAttributes )
    : CSyncObject( pstrName )
/***************************/
{
    m_hObject = ::CreateSemaphore( lpsaAttributes, lInitialCount, lMaxCount, pstrName );
}

BOOL CSemaphore::Unlock()
/***********************/
{
    return( Unlock( 1 ) );
}

BOOL CSemaphore::Unlock( LONG lCount, LPLONG lpPrevCount )
/********************************************************/
{
    return( ::ReleaseSemaphore( m_hObject, lCount, lpPrevCount ) );
}
