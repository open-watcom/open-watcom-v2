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
* Description:  Implementation of CMapStringToString.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_SERIAL( CMapStringToString, CObject, 0 )

CMapStringToString::CMapStringToString()
/**************************************/
{
    m_pHashTable = NULL;
    m_nHashTableSize = 17;
    m_nCount = 0;
}

CMapStringToString::~CMapStringToString()
/***************************************/
{
    CAssoc  *pAssoc;
    CAssoc  *pNextAssoc;
    if( m_pHashTable != NULL ) {
        for( int i = 0; i < m_nHashTableSize; i++ ) {
            pAssoc = m_pHashTable[i];
            while( pAssoc != NULL ) {
                pNextAssoc = pAssoc->pNext;
                delete pAssoc;
                pAssoc = pNextAssoc;
            }
        }
        delete [] m_pHashTable;
    }
}

void CMapStringToString::Serialize( CArchive &ar )
/************************************************/
{
    CObject::Serialize( ar );

    CString key;
    CString value;
    if( ar.IsStoring() ) {
        ar.WriteCount( m_nCount );
        POSITION position = GetStartPosition();
        while( position != NULL ) {
            GetNextAssoc( position, key, value );
            ar << key;
            ar << value;
        }
    } else {
        UINT nCount = ar.ReadCount();
        for( int i = 0; i < nCount; i++ ) {
            ar >> key;
            ar >> value;
            SetAt( key, value );
        }
    }
}

#ifdef _DEBUG

void CMapStringToString::AssertValid() const
/******************************************/
{
    CObject::AssertValid();
    
    ASSERT( m_nCount >= 0 );
}

void CMapStringToString::Dump( CDumpContext &dc ) const
/*****************************************************/
{
    CObject::Dump( dc );

    dc << "m_pHashTable = " << m_pHashTable << "\n";
    dc << "m_nHashTableSize = " << m_nHashTableSize << "\n";
    dc << "m_nCount = " << m_nCount << "\n";
}

#endif // _DEBUG

void CMapStringToString::GetNextAssoc( POSITION &rNextPosition, CString &rKey,
                                       CString &rValue ) const
/************************************************************/
{
    ASSERT( rNextPosition != NULL );
    CAssoc *pAssoc = (CAssoc *)rNextPosition;
    rKey = pAssoc->key;
    rValue = pAssoc->value;
    if( pAssoc->pNext != NULL ) {
        rNextPosition = (POSITION)pAssoc->pNext;
    } else {
        UINT nHashKey = HashKey( pAssoc->key ) % m_nHashTableSize;
        for( int i = nHashKey + 1; i < m_nHashTableSize; i++ ) {
            if( m_pHashTable[i] != NULL ) {
                rNextPosition = (POSITION)m_pHashTable[i];
                return;
            }
        }
        rNextPosition = NULL;
    }
}

POSITION CMapStringToString::GetStartPosition() const
/***************************************************/
{
    if( m_pHashTable == NULL ) {
        return( NULL );
    }
    for( int i = 0; i < m_nHashTableSize; i++ ) {
        if( m_pHashTable[i] != NULL ) {
            return( (POSITION)m_pHashTable[i] );
        }
    }
    return( NULL );
}

UINT CMapStringToString::HashKey( LPCTSTR key ) const
/***************************************************/
{
    ASSERT( key != NULL );
    UINT nHashKey = 0;
    while( *key != _T('\0') ) {
        nHashKey = (nHashKey << 5) + nHashKey + *key;
        key++;
    }
    return( nHashKey );
}

void CMapStringToString::InitHashTable( UINT hashSize, BOOL bAllocNow )
/*********************************************************************/
{
    CAssoc  *pAssoc;
    CAssoc  *pNextAssoc;
    if( m_pHashTable != NULL ) {
        for( int i = 0; i < m_nHashTableSize; i++ ) {
            pAssoc = m_pHashTable[i];
            while( pAssoc != NULL ) {
                pNextAssoc = pAssoc->pNext;
                delete pAssoc;
                pAssoc = pNextAssoc;
            }
        }
        delete [] m_pHashTable;
    }
    m_nHashTableSize = hashSize;
    m_nCount = 0;
    if( bAllocNow ) {
        m_pHashTable = new CAssoc *[hashSize];
        for( int i = 0; i < hashSize; i++ ) {
            m_pHashTable[i] = NULL;
        }
    } else {
        m_pHashTable = NULL;
    }
}

BOOL CMapStringToString::Lookup( LPCTSTR key, CString &rValue ) const
/*******************************************************************/
{
    if( m_pHashTable == NULL ) {
        return( FALSE );
    }
    UINT    nHashKey = HashKey( key ) % m_nHashTableSize;
    CAssoc  *pAssoc = m_pHashTable[nHashKey];
    while( pAssoc != NULL ) {
        if( pAssoc->key == key ) {
            rValue = pAssoc->value;
            return( TRUE );
        }
        pAssoc = pAssoc->pNext;
    }
    return( FALSE );
}

void CMapStringToString::RemoveAll()
/**********************************/
{
    CAssoc  *pAssoc;
    CAssoc  *pNextAssoc;
    if( m_pHashTable != NULL ) {
        for( int i = 0; i < m_nHashTableSize; i++ ) {
            pAssoc = m_pHashTable[i];
            while( pAssoc != NULL ) {
                pNextAssoc = pAssoc->pNext;
                delete pAssoc;
                pAssoc = pNextAssoc;
            }
            m_pHashTable[i] = NULL;
        }
        m_nCount = 0;
    }
}

BOOL CMapStringToString::RemoveKey( LPCTSTR key )
/***********************************************/
{
    if( m_pHashTable == NULL ) {
        return( FALSE );
    }
    UINT nHashKey = HashKey( key ) % m_nHashTableSize;
    if( m_pHashTable[nHashKey] == NULL ) {
        return( FALSE );
    }
    if( m_pHashTable[nHashKey]->key == key ) {
        CAssoc *pOldAssoc = m_pHashTable[nHashKey];
        m_pHashTable[nHashKey] = pOldAssoc->pNext;
        delete pOldAssoc;
        m_nCount--;
        return( TRUE );
    }
    CAssoc *pAssoc = m_pHashTable[nHashKey];
    while( pAssoc->pNext != NULL ) {
        if( pAssoc->pNext->key == key ) {
            CAssoc *pOldAssoc = pAssoc->pNext;
            pAssoc->pNext = pOldAssoc->pNext;
            delete pOldAssoc;
            m_nCount--;
            return( TRUE );
        }
        pAssoc = pAssoc->pNext;
    }
    return( FALSE );
}

void CMapStringToString::SetAt( LPCTSTR key, LPCTSTR newValue )
/*************************************************************/
{
    if( m_pHashTable == NULL ) {
        m_pHashTable = new CAssoc *[m_nHashTableSize];
        for( int i = 0; i < m_nHashTableSize; i++ ) {
            m_pHashTable[i] = NULL;
        }
    }
    UINT nHashKey = HashKey( key ) % m_nHashTableSize;
    CAssoc *pAssoc = m_pHashTable[nHashKey];
    while( pAssoc != NULL ) {
        if( pAssoc->key == key ) {
            pAssoc->value = newValue;
            return;
        }
        pAssoc = pAssoc->pNext;
    }
    pAssoc = new CAssoc;
    pAssoc->key = key;
    pAssoc->value = newValue;
    pAssoc->pNext = m_pHashTable[nHashKey];
    m_pHashTable[nHashKey] = pAssoc;
    m_nCount++;
}

CString &CMapStringToString::operator[]( LPCTSTR key )
/****************************************************/
{
    if( m_pHashTable == NULL ) {
        m_pHashTable = new CAssoc *[m_nHashTableSize];
        for( int i = 0; i < m_nHashTableSize; i++ ) {
            m_pHashTable[i] = NULL;
        }
    }
    UINT nHashKey = HashKey( key ) % m_nHashTableSize;
    CAssoc *pAssoc = m_pHashTable[nHashKey];
    while( pAssoc != NULL ) {
        if( pAssoc->key == key ) {
            return( pAssoc->value );
        }
        pAssoc = pAssoc->pNext;
    }
    pAssoc = new CAssoc;
    pAssoc->key = key;
    pAssoc->pNext = m_pHashTable[nHashKey];
    m_pHashTable[nHashKey] = pAssoc;
    m_nCount++;
    return( pAssoc->value );
}
