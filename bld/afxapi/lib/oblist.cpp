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
* Description:  Implementation of CObList.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_SERIAL( CObList, CObject, 0 )

CObList::CObList()
/****************/
{
    m_pNodeHead = NULL;
    m_pNodeTail = NULL;
    m_nCount = 0;
}

CObList::~CObList()
/*****************/
{
    RemoveAll();
}

void CObList::Serialize( CArchive &ar )
/*************************************/
{
    if( ar.IsStoring() ) {
        ar.WriteCount( m_nCount );
        CNode *pNode = m_pNodeHead;
        for( int i = 0; i < m_nCount; i++ ) {
            ASSERT( pNode != NULL );
            ar << pNode->data;
            pNode = pNode->pNext;
        }
    } else {
        RemoveAll();
        int nCount = ar.ReadCount();
        CObject *pOb;
        for( int i = 0; i < nCount; i++ ) {
            ar >> pOb;
            AddTail( pOb );
        }
    }
}

#ifdef _DEBUG

void CObList::AssertValid() const
/*******************************/
{
    CObject::AssertValid();
    
    ASSERT( m_nCount >= 0 );
}

void CObList::Dump( CDumpContext &dc ) const
/******************************************/
{
    CObject::Dump( dc );

    dc << "m_pNodeHead = " << m_pNodeHead << "\n";
    dc << "m_pNodeTail = " << m_pNodeTail << "\n";
    dc << "m_nCount = " << m_nCount << "\n";
}

#endif // _DEBUG

POSITION CObList::AddHead( CObject *newElement )
/**********************************************/
{
    CNode *pNewNode = new CNode;
    pNewNode->pNext = m_pNodeHead;
    pNewNode->pPrev = NULL;
    pNewNode->data = newElement;
    if( m_pNodeHead != NULL ) {
        m_pNodeHead->pPrev = pNewNode;
    }
    m_pNodeHead = pNewNode;
    if( m_pNodeTail == NULL ) {
        m_pNodeTail = pNewNode;
    }
    m_nCount++;
    return( (POSITION)pNewNode );
}

void CObList::AddHead( CObList *pNewList )
/****************************************/
{
    if( pNewList->m_pNodeHead != NULL ) {
        CNode *pNodeFirst = NULL;
        CNode *pNodeCur = NULL;
        CNode *pOtherNode = pNewList->m_pNodeHead;
        while( pOtherNode != NULL ) {
            if( pNodeCur == NULL ) {
                pNodeCur = new CNode;
                pNodeFirst = pNodeCur;
                pNodeCur->pPrev = NULL;
            } else {
                pNodeCur->pNext = new CNode;
                pNodeCur->pNext->pPrev = pNodeCur;
                pNodeCur = pNodeCur->pNext;
            }
            pNodeCur->pNext = NULL;
            pNodeCur->data = pOtherNode->data;
            pOtherNode = pOtherNode->pNext;
        }
        pNodeCur->pNext = m_pNodeHead;
        if( m_pNodeHead != NULL ) {
            m_pNodeHead->pPrev = pNodeCur;
        }
        m_pNodeHead = pNodeFirst;
        if( m_pNodeTail == NULL ) {
            m_pNodeTail = pNodeCur;
        }
        m_nCount += pNewList->m_nCount;
    }
}

POSITION CObList::AddTail( CObject *newElement )
/**********************************************/
{
    CNode *pNewNode = new CNode;
    pNewNode->pNext = NULL;
    pNewNode->pPrev = m_pNodeTail;
    pNewNode->data = newElement;
    if( m_pNodeTail != NULL ) {
        m_pNodeTail->pNext = pNewNode;
    }
    m_pNodeTail = pNewNode;
    if( m_pNodeHead == NULL ) {
        m_pNodeHead = pNewNode;
    }
    m_nCount++;
    return( (POSITION)pNewNode );
}

void CObList::AddTail( CObList *pNewList )
/****************************************/
{
    if( pNewList->m_pNodeHead != NULL ) {
        CNode *pNodeFirst = NULL;
        CNode *pNodeCur = NULL;
        CNode *pOtherNode = pNewList->m_pNodeHead;
        while( pOtherNode != NULL ) {
            if( pNodeCur == NULL ) {
                pNodeCur = new CNode;
                pNodeFirst = pNodeCur;
                pNodeCur->pPrev = NULL;
            } else {
                pNodeCur->pNext = new CNode;
                pNodeCur->pNext->pPrev = pNodeCur;
                pNodeCur = pNodeCur->pNext;
            }
            pNodeCur->pNext = NULL;
            pNodeCur->data = pOtherNode->data;
            pOtherNode = pOtherNode->pNext;
        }
        pNodeFirst->pPrev = m_pNodeTail;
        if( m_pNodeTail != NULL ) {
            m_pNodeTail->pNext = pNodeFirst;
        }
        m_pNodeTail = pNodeCur;
        if( m_pNodeHead == NULL ) {
            m_pNodeHead = pNodeFirst;
        }
        m_nCount += pNewList->m_nCount;
    }
}

POSITION CObList::Find( CObject *searchValue, POSITION startAfter ) const
/***********************************************************************/
{
    CNode *pNode = (startAfter != NULL) ? (CNode *)startAfter : m_pNodeHead;
    while( pNode != NULL ) {
        if( pNode->data == searchValue ) {
            return( (POSITION)pNode );
        }
        pNode = pNode->pNext;
    }
    return( NULL );
}

POSITION CObList::FindIndex( INT_PTR nIndex ) const
/*************************************************/
{
    ASSERT( nIndex >= 0 );
    CNode *pNode = m_pNodeHead;
    while( nIndex > 0 ) {
        pNode = pNode->pNext;
        if( pNode == NULL ) {
            return( NULL );
        }
        nIndex--;
    }
    return( (POSITION)pNode );
}

POSITION CObList::InsertAfter( POSITION position, CObject *newElement )
/*********************************************************************/
{
    ASSERT( position != NULL );
    CNode *pNode = new CNode;
    pNode->pPrev = (CNode *)position;
    pNode->pNext = pNode->pPrev->pNext;
    pNode->data = newElement;
    pNode->pPrev->pNext = pNode;
    if( pNode->pNext != NULL ) {
        pNode->pNext->pPrev = pNode;
    } else {
        ASSERT( m_pNodeTail == (CNode *)position );
        m_pNodeTail = pNode;
    }
    m_nCount++;
    return( (POSITION)pNode );
}

POSITION CObList::InsertBefore( POSITION position, CObject *newElement )
/**********************************************************************/
{
    ASSERT( position != NULL );
    CNode *pNode = new CNode;
    pNode->pNext = (CNode *)position;
    pNode->pPrev = pNode->pNext->pPrev;
    pNode->data = newElement;
    pNode->pNext->pPrev = pNode;
    if( pNode->pPrev != NULL ) {
        pNode->pPrev->pNext = pNode;
    } else {
        ASSERT( m_pNodeHead == (CNode *)position );
        m_pNodeHead = pNode;
    }
    m_nCount++;
    return( (POSITION)pNode );
}

void CObList::RemoveAll()
/***********************/
{
    CNode *pNode = m_pNodeHead;
    CNode *pNextNode;
    while( pNode != NULL ) {
        pNextNode = pNode->pNext;
        delete pNode;
        pNode = pNextNode;
    }
    m_pNodeHead = NULL;
    m_pNodeTail = NULL;
    m_nCount = 0;
}

void CObList::RemoveAt( POSITION position )
/*****************************************/
{
    CNode *pNode = (CNode *)position;
    if( pNode->pNext != NULL ) {
        pNode->pNext->pPrev = pNode->pPrev;
    } else {
        ASSERT( m_pNodeTail == pNode );
        m_pNodeTail = pNode->pPrev;
    }
    if( pNode->pPrev != NULL ) {
        pNode->pPrev->pNext = pNode->pNext;
    } else {
        ASSERT( m_pNodeHead == pNode );
        m_pNodeHead = pNode->pNext;
    }
    delete pNode;
    m_nCount--;
}

CObject *CObList::RemoveHead()
/****************************/
{
    ASSERT( m_pNodeHead != NULL );
    CNode *pOldHead = m_pNodeHead;
    m_pNodeHead = m_pNodeHead->pNext;
    if( m_pNodeHead == NULL ) {
        ASSERT( m_pNodeTail == pOldHead );
        m_pNodeTail = NULL;
    } else {
        m_pNodeHead->pPrev = NULL;
    }
    CObject *pOldData = pOldHead->data;
    delete pOldHead;
    m_nCount--;
    return( pOldData );
}

CObject *CObList::RemoveTail()
/****************************/
{
    ASSERT( m_pNodeTail != NULL );
    CNode *pOldTail = m_pNodeTail;
    m_pNodeTail = m_pNodeTail->pPrev;
    if( m_pNodeTail == NULL ) {
        ASSERT( m_pNodeHead == pOldTail );
        m_pNodeHead = NULL;
    } else {
        m_pNodeTail->pNext = NULL;
    }
    CObject *pOldData = pOldTail->data;
    delete pOldTail;
    m_nCount--;
    return( pOldData );
}
