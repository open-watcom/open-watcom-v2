/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of CSimpleList.
*
****************************************************************************/


#include "stdafx.h"

void CSimpleList::AddHead( void *p )
/**********************************/
{
    ASSERT( p != NULL );
    *(void **)((BYTE *)p + m_nNextOffset) = m_pHead;
    m_pHead = p;
}

BOOL CSimpleList::Remove( void *p )
/*********************************/
{
    ASSERT( p != NULL );
    if( m_pHead == p ) {
        m_pHead = GetNext( m_pHead );
        return( TRUE );
    }
    
    void    *pNode = m_pHead;
    void    *pNext;
    void    *pNewNext;
    while( pNode != NULL ) {
        pNext = GetNext( pNode );
        if( pNext == p ) {
            pNewNext = GetNext( pNext );
            *(void **)((BYTE *)p + m_nNextOffset) = pNewNext;
            return( TRUE );
        }
        pNode = pNext;
    }
    return( FALSE );
}
