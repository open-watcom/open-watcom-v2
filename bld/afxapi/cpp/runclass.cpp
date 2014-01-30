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
* Description:  Implementation of CRuntimeClass, AfxClassInit, and
*               AfxDoForAllClasses.
*
****************************************************************************/


#include "stdafx.h"

CObject *CRuntimeClass::CreateObject()
/************************************/
{
    if( m_pfnCreateObject == NULL ) {
        return( NULL );
    }
    return( m_pfnCreateObject() );
}

BOOL CRuntimeClass::IsDerivedFrom( const CRuntimeClass *pBaseClass )
/******************************************************************/
{
    if( pBaseClass == this ) {
        return( TRUE );
    } else {
        CRuntimeClass *pNextClass;
#ifdef _AFXDLL
        ASSERT( m_pfnGetBaseClass != NULL );
        pNextClass = m_pfnGetBaseClass();
#else
        pNextClass = m_pBaseClass;
#endif
        if( pNextClass == NULL ) {
            return( FALSE );
        } else {
            return( pNextClass->IsDerivedFrom( pBaseClass ) );
        }
    }
}

void CRuntimeClass::Store( CArchive &ar ) const
/*********************************************/
{
    int nLength = strlen( m_lpszClassName );
    ar << (WORD)m_wSchema;
    ar << (WORD)nLength;
    ar.Write( m_lpszClassName, nLength * sizeof( char ) );
}

CObject * PASCAL CRuntimeClass::CreateObject( LPCSTR lpszClassName )
/******************************************************************/
{
    CRuntimeClass *pClass = CRuntimeClass::FromName( lpszClassName );
    if( pClass == NULL ) {
        return( NULL );
    }
    return( pClass->CreateObject() );
}

CObject * PASCAL CRuntimeClass::CreateObject( LPCWSTR lpszClassName )
/*******************************************************************/
{
    CRuntimeClass *pClass = CRuntimeClass::FromName( lpszClassName );
    if( pClass == NULL ) {
        return( NULL );
    }
    return( pClass->CreateObject() );
}

CRuntimeClass * PASCAL CRuntimeClass::FromName( LPCSTR lpszClassName )
/********************************************************************/
{
    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    CRuntimeClass *pClass = pState->m_classList.GetHead();
    while( pClass != NULL ) {
        if( strcmp( pClass->m_lpszClassName, lpszClassName ) == 0 ) {
            return( pClass );
        }
        pClass = pState->m_classList.GetNext( pClass );
    }
    return( NULL );
}

CRuntimeClass * PASCAL CRuntimeClass::FromName( LPCWSTR lpszClassName )
/*********************************************************************/
{
    // Since class names are always stored as ANSI, names specified as Unicode must
    // always be converted.
    int     nLength = wcslen( lpszClassName );
    char    *lpszTemp = new char[nLength + 1];
    ::WideCharToMultiByte( CP_ACP, 0L, lpszClassName, -1, lpszTemp, nLength + 1,
                           NULL, NULL );
    CRuntimeClass *pClass = CRuntimeClass::FromName( lpszTemp );
    delete [] lpszTemp;
    return( pClass );
}

void AFXAPI AfxClassInit( CRuntimeClass *pNewClass )
/**************************************************/
{
    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    pState->m_classList.AddHead( pNewClass );
}

#ifdef _DEBUG

void AFXAPI AfxDoForAllClasses( void (*pfn)( CRuntimeClass *pClass, void *pContext ),
                                void *pContext )
/***********************************************************************************/
{
    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    CRuntimeClass *pClass = pState->m_classList.GetHead();
    while( pClass != NULL ) {
        pfn( pClass, pContext );
        pClass = pState->m_classList.GetNext( pClass );
    }
}

#endif // _DEBUG
