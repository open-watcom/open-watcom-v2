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
* Description:  Implementation of CMDIFrameWnd.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNCREATE( CMDIFrameWnd, CFrameWnd )

BEGIN_MESSAGE_MAP( CMDIFrameWnd, CFrameWnd )
    ON_COMMAND_EX( ID_WINDOW_TILE_HORZ, OnMDIWindowCmd )
    ON_COMMAND_EX( ID_WINDOW_TILE_VERT, OnMDIWindowCmd )
    ON_COMMAND_EX( ID_WINDOW_CASCADE, OnMDIWindowCmd )
    ON_COMMAND_EX( ID_WINDOW_ARRANGE, OnMDIWindowCmd )
    ON_COMMAND( ID_WINDOW_NEW, OnWindowNew )
    ON_UPDATE_COMMAND_UI( ID_WINDOW_NEW, OnUpdateMDIWindowCmd )
    ON_UPDATE_COMMAND_UI( ID_WINDOW_TILE_HORZ, OnUpdateMDIWindowCmd )
    ON_UPDATE_COMMAND_UI( ID_WINDOW_TILE_VERT, OnUpdateMDIWindowCmd )
    ON_UPDATE_COMMAND_UI( ID_WINDOW_CASCADE, OnUpdateMDIWindowCmd )
    ON_UPDATE_COMMAND_UI( ID_WINDOW_ARRANGE, OnUpdateMDIWindowCmd )
END_MESSAGE_MAP()

CMDIFrameWnd::CMDIFrameWnd()
/**************************/
{
    m_hWndMDIClient = NULL;
}

BOOL CMDIFrameWnd::CreateClient( LPCREATESTRUCT lpCreateStruct, CMenu *pWindowMenu )
/**********************************************************************************/
{
    UNUSED_ALWAYS( lpCreateStruct );
    
    CLIENTCREATESTRUCT ccs;
    ccs.hWindowMenu = pWindowMenu->GetSafeHmenu();
    ccs.idFirstChild = AFX_IDM_FIRST_MDICHILD;

    m_hWndMDIClient = ::CreateWindowEx( WS_EX_CLIENTEDGE, _T("MDICLIENT"), NULL,
                                        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN |
                                        WS_CLIPSIBLINGS | MDIS_ALLCHILDSTYLES,
                                        0, 0, 0, 0, m_hWnd, (HMENU)AFX_IDW_PANE_FIRST,
                                        AfxGetInstanceHandle(), &ccs );
    return( m_hWndMDIClient != NULL );
}

HMENU CMDIFrameWnd::GetWindowMenuPopup( HMENU hMenuBar )
/******************************************************/
{
    int nCount = ::GetMenuItemCount( hMenuBar );
    for( int i = nCount - 1; i >= 0; i-- ) {
        HMENU hSubMenu = ::GetSubMenu( hMenuBar, i );
        int nSubCount = ::GetMenuItemCount( hSubMenu );
        for( int j = 0; j < nSubCount; j++ ) {
            int nID = ::GetMenuItemID( hSubMenu, j );
            if( nID >= AFX_IDM_WINDOW_FIRST && nID <= AFX_IDM_WINDOW_LAST ) {
                return( hSubMenu );
            }
        }
    }
    return( NULL );
}

LRESULT CMDIFrameWnd::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
/*******************************************************************************/
{
    return( ::DefFrameProc( m_hWnd, m_hWndMDIClient, message, wParam, lParam ) );
}

CFrameWnd *CMDIFrameWnd::GetActiveFrame()
/***************************************/
{
    CMDIChildWnd *pChild = MDIGetActive();
    if( pChild != NULL ) {
        return( pChild );
    } else {
        return( this );
    }
}

BOOL CMDIFrameWnd::OnCmdMsg( UINT nID, int nCode, void *pExtra,
                             AFX_CMDHANDLERINFO *pHandlerInfo )
/*************************************************************/
{
    CMDIChildWnd *pChild = MDIGetActive();
    if( pChild != NULL && pChild->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) ) {
        return( TRUE );
    }
    return( CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) );
}

BOOL CMDIFrameWnd::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext *pContext )
/********************************************************************************/
{
    UNUSED_ALWAYS( pContext );
    
    CMenu *pMenu = GetMenu();
    CMenu *pSubMenu = NULL;
    if( pMenu != NULL ) {
        UINT nCount = pMenu->GetMenuItemCount();
        pSubMenu = pMenu->GetSubMenu( nCount - 2 );
    }

    return( CreateClient( lpcs, pSubMenu ) );
}

BOOL CMDIFrameWnd::PreCreateWindow( CREATESTRUCT &cs )
/****************************************************/
{
    if( cs.lpszClass == NULL ) {
        cs.lpszClass = _T("AfxMDIFrame");
    }
    return( TRUE );
}

BOOL CMDIFrameWnd::PreTranslateMessage( MSG *pMsg )
/*************************************************/
{
    if( CFrameWnd::PreTranslateMessage( pMsg ) ) {
        return( TRUE );
    }

    return( ::TranslateMDISysAccel( m_hWndMDIClient, pMsg ) );
}

#ifdef _DEBUG

void CMDIFrameWnd::AssertValid() const
/************************************/
{
    CFrameWnd::AssertValid();

    ASSERT( m_hWndMDIClient == NULL || ::IsWindow( m_hWndMDIClient ) );
}

void CMDIFrameWnd::Dump( CDumpContext &dc ) const
/***********************************************/
{
    CFrameWnd::Dump( dc );

    dc << "m_hWndMDIClient = " << m_hWndMDIClient << "\n";
}

#endif // _DEBUG

CMDIChildWnd *CMDIFrameWnd::CreateNewChild( CRuntimeClass *pClass, UINT nResource,
                                            HMENU hMenu, HACCEL hAccel )
/**********************************************************************/
{
    ASSERT( pClass != NULL );
    CMDIChildWnd *pChild = (CMDIChildWnd *)pClass->CreateObject();
    ASSERT( pChild->IsKindOf( RUNTIME_CLASS( CMDIChildWnd ) ) );
    if( !pChild->LoadFrame( nResource ) ) {
        delete pChild;
        return( NULL );
    }
    pChild->SetHandles( hMenu, hAccel );
    return( pChild );
}

BOOL CMDIFrameWnd::OnMDIWindowCmd( UINT nID )
/*******************************************/
{
    switch( nID ) {
    case ID_WINDOW_TILE_HORZ:
        ::SendMessage( m_hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0L );
        return( TRUE );
    case ID_WINDOW_TILE_VERT:
        ::SendMessage( m_hWndMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0L );
        return( TRUE );
    case ID_WINDOW_CASCADE:
        ::SendMessage( m_hWndMDIClient, WM_MDICASCADE, 0, 0L );
        return( TRUE );
    case ID_WINDOW_ARRANGE:
        ::SendMessage( m_hWndMDIClient, WM_MDIICONARRANGE, 0, 0L );
        return( TRUE );
    default:
        return( FALSE );
    }
}

void CMDIFrameWnd::OnWindowNew()
/******************************/
{
    CMDIChildWnd *pChild = MDIGetActive();
    if( pChild != NULL ) {
        CDocument *pDoc = pChild->GetActiveDocument();
        ASSERT( pDoc != NULL );
        CDocTemplate *pTemplate = pDoc->GetDocTemplate();
        ASSERT( pTemplate != NULL );
        CFrameWnd *pFrame = pTemplate->CreateNewFrame( pDoc, pChild );
        if( pFrame != NULL ) {
            pTemplate->InitialUpdateFrame( pFrame, pDoc );
        }
    }
}

void CMDIFrameWnd::OnUpdateMDIWindowCmd( CCmdUI *pCmdUI )
/*******************************************************/
{
    HWND hWndActive = (HWND)::SendMessage( m_hWndMDIClient, WM_MDIGETACTIVE, 0, 0L );
    pCmdUI->Enable( hWndActive != NULL );
}
