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
* Description:  Implementation of CGdiObject.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

IMPLEMENT_DYNAMIC( CGdiObject, CObject )

CGdiObject::CGdiObject()
/**********************/
{
    m_hObject = NULL;
}

CGdiObject::~CGdiObject()
/***********************/
{
    DeleteObject();
}

#ifdef _DEBUG

void CGdiObject::Dump( CDumpContext &dc ) const
/*********************************************/
{
    dc << "m_hObject = " << m_hObject << "\n";
}

#endif // _DEBUG

BOOL CGdiObject::Attach( HGDIOBJ hObject )
/****************************************/
{
    ASSERT( m_hObject == NULL );
    if( hObject == NULL ) {
        return( FALSE );
    }
    m_hObject = hObject;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHGDIOBJ;
    ASSERT( pHandleMap != NULL );
    pHandleMap->SetPermanent( hObject, this );
    return( TRUE );
}

HGDIOBJ CGdiObject::Detach()
/**************************/
{
    if( m_hObject == NULL ) {
        return( NULL );
    }
    HGDIOBJ hObject = m_hObject;
    m_hObject = NULL;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHGDIOBJ;
    ASSERT( pHandleMap != NULL );
    pHandleMap->RemoveHandle( hObject );
    return( hObject );
}

void PASCAL CGdiObject::DeleteTempMap()
/*************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHGDIOBJ;
    ASSERT( pHandleMap != NULL );
    pHandleMap->DeleteTemp();
}

CGdiObject * PASCAL CGdiObject::FromHandle( HGDIOBJ hObject )
/***********************************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHGDIOBJ;
    ASSERT( pHandleMap != NULL );
    return( (CGdiObject *)pHandleMap->FromHandle( hObject ) );
}
