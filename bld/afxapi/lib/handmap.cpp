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
* Description:  Implementation of CHandleMap.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

CHandleMap::CHandleMap( CRuntimeClass *pClass,
                        void (PASCAL *pfnConstructObject)( CObject *pObject ),
                        void (PASCAL *pfnDestructObject)( CObject *pObject ),
                        size_t nOffset, int nHandles )
/****************************************************/
{
    ASSERT( pClass != NULL );
    ASSERT( pfnConstructObject != NULL );
    ASSERT( pfnDestructObject != NULL );
    ASSERT( nHandles >= 1 );
    m_pClass = pClass;
    m_pfnConstructObject = pfnConstructObject;
    m_pfnDestructObject = pfnDestructObject;
    m_nOffset = nOffset;
    m_nHandles = nHandles;
}

CHandleMap::~CHandleMap()
/***********************/
{
    DeleteTemp();
}

void CHandleMap::DeleteTemp()
/***************************/
{
    HANDLE      h;
    void        *pObject;
    POSITION    position = m_temporaryMap.GetStartPosition();
    while( position != NULL ) {
        m_temporaryMap.GetNextAssoc( position, h, pObject );
        if( pObject != NULL ) {
            HANDLE *pHandles = (HANDLE *)((BYTE *)pObject + m_nOffset);
            for( int i = 0; i < m_nHandles; i++ ) {
                pHandles[i] = NULL;
            }
            m_pfnDestructObject( (CObject *)pObject );
            free( pObject );
        }
    }
    m_temporaryMap.RemoveAll();
}

CObject *CHandleMap::FromHandle( HANDLE h )
/*****************************************/
{
    if( h == NULL ) {
        return( NULL );
    }
    CObject *pObject = LookupPermanent( h );
    if( pObject != NULL ) {
        return( pObject );
    }
    pObject = LookupTemporary( h );
    if( pObject != NULL ) {
        return( pObject );
    }
    pObject = (CObject *)malloc( m_pClass->m_nObjectSize );
    m_pfnConstructObject( pObject );
    HANDLE *pHandles = (HANDLE *)((BYTE *)pObject + m_nOffset);
    for( int i = 0; i < m_nHandles; i++ ) {
        pHandles[i] = h;
    }
    m_temporaryMap.SetAt( h, pObject );
    return( pObject );
}

CObject *CHandleMap::LookupPermanent( HANDLE h )
/**********************************************/
{
    void *pObject;
    if( m_permanentMap.Lookup( h, pObject ) ) {
        return( (CObject *)pObject );
    }
    return( NULL );
}

CObject *CHandleMap::LookupTemporary( HANDLE h )
/**********************************************/
{
    void *pObject;
    if( m_temporaryMap.Lookup( h, pObject ) ) {
        return( (CObject *)pObject );
    }
    return( NULL );
}

void CHandleMap::RemoveHandle( HANDLE h )
/***************************************/
{
    m_permanentMap.RemoveKey( h );
}

void CHandleMap::SetPermanent( HANDLE h, CObject *permOb )
/********************************************************/
{
    m_permanentMap.SetAt( h, permOb );
}
