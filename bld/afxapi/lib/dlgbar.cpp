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
* Description:  Implementation of CDialogBar.
*
****************************************************************************/


#include "stdafx.h"
#include "dialog.h"

IMPLEMENT_DYNAMIC( CDialogBar, CControlBar )

CDialogBar::CDialogBar()
/**********************/
{
    m_lpszTemplateName = NULL;
}

BOOL CDialogBar::Create( CWnd *pParentWnd, LPCTSTR lpszTemplateName, UINT nStyle,
                         UINT nID )
/*******************************************************************************/
{
    m_dwStyle = nStyle & CBRS_ALL;
    
    CREATESTRUCT cs;
    memset( &cs, 0, sizeof( CREATESTRUCT ) );
    cs.lpszClass = _T("AfxControlBar");
    cs.style = nStyle | WS_CHILD;
    cs.hwndParent = pParentWnd->GetSafeHwnd();
    cs.hMenu = (HMENU)nID;
    cs.hInstance = AfxGetInstanceHandle();
    if( !PreCreateWindow( cs ) ) {
        PostNcDestroy();
        return( FALSE );
    }

    AfxHookWindowCreate( this );
    HWND hWnd = ::CreateDialogParam( AfxGetInstanceHandle(), lpszTemplateName,
                                     pParentWnd->GetSafeHwnd(), AfxDlgProc, 0L );
    AfxUnhookWindowCreate();

    if( hWnd == NULL ) {
        PostNcDestroy();
        return( FALSE );
    }
    ASSERT( m_hWnd == hWnd );

    CRect rect;
    ::GetWindowRect( hWnd, &rect );
    m_sizeDefault = rect.Size();

    ::SetWindowLong( hWnd, GWL_ID, nID );
    ::SetWindowPos( hWnd, NULL, 0, 0, 0, 0,
                    SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW );
    return( TRUE );
}

BOOL CDialogBar::Create( CWnd *pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID )
/**********************************************************************************/
{
    return( Create( pParentWnd, MAKEINTRESOURCE( nIDTemplate ), nStyle, nID ) );
}

CSize CDialogBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
/************************************************************/
{
    if( bStretch ) {
        if( bHorz ) {
            return( CSize( 32767, m_sizeDefault.cy ) );
        } else {
            return( CSize( m_sizeDefault.cx, 32767 ) );
        }
    } else {
        return( m_sizeDefault );
    }
}

void CDialogBar::OnUpdateCmdUI( CFrameWnd *pTarget, BOOL bDisableIfNoHndler )
/***************************************************************************/
{
    UpdateDialogControls( pTarget, bDisableIfNoHndler );
}

#ifdef _DEBUG

void CDialogBar::Dump( CDumpContext &dc ) const
/*********************************************/
{
    CControlBar::Dump( dc );

    dc << "m_sizeDefault = " << m_sizeDefault << "\n";
    if( IS_INTRESOURCE( m_lpszTemplateName ) ) {
        dc << "m_lpszTemplateName = " << (UINT)m_lpszTemplateName << "\n";
    } else {
        dc << "m_lpszTemplateName = " << m_lpszTemplateName << "\n";
    }
}

#endif // _DEBUG
