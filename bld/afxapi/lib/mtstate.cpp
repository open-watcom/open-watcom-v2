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
* Description:  Implementation of AFX_MODULE_THREAD_STATE and
*               AfxGetModuleThreadState.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

void PASCAL _ConstructWnd( CObject *pObject )
/*******************************************/
{
    new( pObject ) CWnd;
}

void PASCAL _DestructWnd( CObject *pObject )
/******************************************/
{
    CWnd *pWnd = (CWnd *)pObject;
    pWnd->~CWnd();
}

void PASCAL _ConstructMenu( CObject *pObject )
/********************************************/
{
    new( pObject ) CMenu;
}

void PASCAL _DestructMenu( CObject *pObject )
/*******************************************/
{
    CMenu *pMenu = (CMenu *)pObject;
    pMenu->~CMenu();
}

void PASCAL _ConstructDC( CObject *pObject )
/******************************************/
{
    new( pObject ) CDC;
}

void PASCAL _DestructDC( CObject *pObject )
/*****************************************/
{
    CDC *pDC = (CDC *)pObject;
    pDC->~CDC();
}

void PASCAL _ConstructGdiObject( CObject *pObject )
/*************************************************/
{
    new( pObject ) CGdiObject;
}

void PASCAL _DestructGdiObject( CObject *pObject )
/************************************************/
{
    CGdiObject *pGdiObject = (CGdiObject *)pObject;
    pGdiObject->~CGdiObject();
}

void PASCAL _ConstructImageList( CObject *pObject )
/*************************************************/
{
    new( pObject ) CImageList;
}

void PASCAL _DestructImageList( CObject *pObject )
/************************************************/
{
    CImageList *pImageList = (CImageList *)pObject;
    pImageList->~CImageList();
}

AFX_MODULE_THREAD_STATE::AFX_MODULE_THREAD_STATE()
/************************************************/
{
    m_pCurrentWinThread = NULL;
    m_pmapHWND = new CHandleMap( RUNTIME_CLASS( CWnd ), _ConstructWnd, _DestructWnd,
                                 offsetof( CWnd, m_hWnd ) );
    m_pmapHMENU = new CHandleMap( RUNTIME_CLASS( CMenu ), _ConstructMenu, _DestructMenu,
                                  offsetof( CMenu, m_hMenu ) );
    m_pmapHDC = new CHandleMap( RUNTIME_CLASS( CDC ), _ConstructDC, _DestructDC,
                                offsetof( CDC, m_hDC ), 2 );
    m_pmapHGDIOBJ = new CHandleMap( RUNTIME_CLASS( CGdiObject ), _ConstructGdiObject,
                                    _DestructGdiObject,
                                    offsetof( CGdiObject, m_hObject ) );
    m_pmapHIMAGELIST = new CHandleMap( RUNTIME_CLASS( CImageList ), _ConstructImageList,
                                       _DestructImageList,
                                       offsetof( CImageList, m_hImageList ) );
    m_pToolTip = NULL;
    m_pLastHit = NULL;
    m_nLastHit = -1;
    m_pLastInfo = NULL;
    m_nLastStatus = -1;
    m_pLastStatus = NULL;
}

AFX_MODULE_THREAD_STATE::~AFX_MODULE_THREAD_STATE()
/*************************************************/
{
    delete m_pmapHWND;
    delete m_pmapHMENU;
    delete m_pmapHDC;
    delete m_pmapHGDIOBJ;
    delete m_pmapHIMAGELIST;
}

AFX_MODULE_THREAD_STATE * AFXAPI AfxGetModuleThreadState()
/********************************************************/
{
    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    return( pState->m_thread );
}
