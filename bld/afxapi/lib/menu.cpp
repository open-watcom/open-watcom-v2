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
* Description:  Implementation of CMenu.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

IMPLEMENT_DYNAMIC( CMenu, CObject )

CMenu::CMenu()
/************/
{
    m_hMenu = NULL;
}

CMenu::~CMenu()
/*************/
{
    DestroyMenu();
}

void CMenu::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
/*******************************************************/
{
    UNUSED_ALWAYS( lpDrawItemStruct );
}

void CMenu::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
/****************************************************************/
{
    UNUSED_ALWAYS( lpMeasureItemStruct );
}

#ifdef _DEBUG

void CMenu::AssertValid() const
/*****************************/
{
    CObject::AssertValid();

    ASSERT( m_hMenu == NULL || ::IsMenu( m_hMenu ) );
}

void CMenu::Dump( CDumpContext &dc ) const
/****************************************/
{
    CObject::Dump( dc );

    dc << "m_hMenu = " << m_hMenu << "\n";
}

#endif // _DEBUG

BOOL CMenu::Attach( HMENU hMenu )
/*******************************/
{
    ASSERT( m_hMenu == NULL );
    if( hMenu == NULL ) {
        return( FALSE );
    }
    m_hMenu = hMenu;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHMENU;
    ASSERT( pHandleMap != NULL );
    pHandleMap->SetPermanent( hMenu, this );
    return( TRUE );
}

HMENU CMenu::Detach()
/*******************/
{
    if( m_hMenu == NULL ) {
        return( NULL );
    }
    HMENU hMenu = m_hMenu;
    m_hMenu = NULL;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHMENU;
    ASSERT( pHandleMap != NULL );
    pHandleMap->RemoveHandle( hMenu );
    return( hMenu );
}

BOOL CMenu::TrackPopupMenu( UINT nFlags, int x, int y, CWnd *pWnd, LPCRECT lpRect )
/*********************************************************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    pState->m_hTrackingWindow = pWnd->GetSafeHwnd();
    pState->m_hTrackingMenu = m_hMenu;
    BOOL bRet = ::TrackPopupMenu( m_hMenu, nFlags, x, y, 0, pWnd->GetSafeHwnd(),
                                  lpRect );
    pState->m_hTrackingWindow = NULL;
    pState->m_hTrackingMenu = NULL;
    return( bRet );
}

BOOL CMenu::TrackPopupMenuEx( UINT fuFlags, int x, int y, CWnd *pWnd, LPTPMPARAMS lptpm )
/***************************************************************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    pState->m_hTrackingWindow = pWnd->GetSafeHwnd();
    pState->m_hTrackingMenu = m_hMenu;
    BOOL bRet = ::TrackPopupMenuEx( m_hMenu, fuFlags, x, y, pWnd->GetSafeHwnd(), lptpm );
    pState->m_hTrackingWindow = NULL;
    pState->m_hTrackingMenu = NULL;
    return( bRet );
}

void PASCAL CMenu::DeleteTempMap()
/********************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHMENU;
    ASSERT( pHandleMap != NULL );
    pHandleMap->DeleteTemp();
}

CMenu * PASCAL CMenu::FromHandle( HMENU hMenu )
/*********************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHMENU;
    ASSERT( pHandleMap != NULL );
    return( (CMenu *)pHandleMap->FromHandle( hMenu ) );
}

CMenu * PASCAL CMenu::FromHandlePermanent( HMENU hMenu )
/******************************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHMENU;
    ASSERT( pHandleMap != NULL );
    return( (CMenu *)pHandleMap->LookupPermanent( hMenu ) );
}
