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
* Description:  Implementation of CMultiLock.
*
****************************************************************************/


#include "stdafx.h"
#include <afxmt.h>

CMultiLock::CMultiLock( CSyncObject *ppObjects[], DWORD dwCount, BOOL bInitialLock )
/**********************************************************************************/
{
    ASSERT( ppObjects != NULL );
    ASSERT( dwCount != 0 );
    m_ppObjectArray = ppObjects;
    if( dwCount < 8 ) {
        m_pHandleArray = m_hPreallocated;
        m_bLockedArray = m_bPreallocated;
    } else {
        m_pHandleArray = new HANDLE[dwCount];
        m_bLockedArray = new BOOL[dwCount];
    }
    for( int i = 0; i < dwCount; i++ ) {
        ASSERT( m_ppObjectArray[i] != NULL );
        m_pHandleArray[i] = m_ppObjectArray[i]->m_hObject;
        m_bLockedArray[i] = FALSE;
    }
    m_dwCount = dwCount;
    if( bInitialLock ) {
        Lock();
    }
}

CMultiLock::~CMultiLock()
/***********************/
{
    Unlock();
    if( m_pHandleArray != m_hPreallocated ) {
        delete [] m_pHandleArray;
        delete [] m_bLockedArray;
    }
}

DWORD CMultiLock::Lock( DWORD dwTimeout, BOOL bWaitForAll, DWORD dwWakeMask )
/***************************************************************************/
{
    DWORD dwRet;
    if( dwWakeMask != 0 ) {
        dwRet = ::MsgWaitForMultipleObjects( m_dwCount, m_pHandleArray, bWaitForAll,
                                            dwTimeout, dwWakeMask );
    } else {
        dwRet = ::WaitForMultipleObjects( m_dwCount, m_pHandleArray, bWaitForAll,
                                         dwTimeout );
    }
    if( dwRet < WAIT_OBJECT_0 + m_dwCount ) {
        if( bWaitForAll ) {
            for( int i = 0; i < m_dwCount; i++ ) {
                m_bLockedArray[i] = TRUE;
            }
        } else {
            m_bLockedArray[dwRet - WAIT_OBJECT_0] = TRUE;
        }
    }
    return( dwRet );
}

BOOL CMultiLock::Unlock()
/***********************/
{
    for( int i = 0; i < m_dwCount; i++ ) {
        if( m_bLockedArray[i] ) {
            m_bLockedArray[i] = !m_ppObjectArray[i]->Unlock();
        }
    }
    return( TRUE );
}

BOOL CMultiLock::Unlock( LONG lCount, LPLONG lpPrevCount )
/********************************************************/
{
    BOOL bRet = FALSE;
    for( int i = 0; i < m_dwCount; i++ ) {
        if( m_bLockedArray[i] &&
            m_ppObjectArray[i]->IsKindOf( RUNTIME_CLASS( CSemaphore ) ) ) {
            m_bLockedArray[i] = !m_ppObjectArray[i]->Unlock( lCount, lpPrevCount );
            bRet |= !m_bLockedArray[i];
        }
    }
    return( bRet );
}
