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
* Description:  Implementation of CFormView.
*
****************************************************************************/


#include "stdafx.h"
#include "dialog.h"

IMPLEMENT_DYNAMIC( CFormView, CScrollView )

BEGIN_MESSAGE_MAP( CFormView, CScrollView )
    ON_WM_CREATE()
END_MESSAGE_MAP()

CFormView::CFormView( LPCTSTR lpszTemplateName )
/**********************************************/
{
    m_lpszTemplateName = lpszTemplateName;
}

CFormView::CFormView( UINT nIDTemplate )
/**************************************/
{
    m_lpszTemplateName = MAKEINTRESOURCE( nIDTemplate );
}

BOOL CFormView::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                        const RECT &rect, CWnd *pParentWnd, UINT nID,
                        CCreateContext *pContext )
/************************************************/
{
    UNUSED_ALWAYS( lpszClassName );
    UNUSED_ALWAYS( lpszWindowName );

    CREATESTRUCT cs;
    memset( &cs, 0, sizeof( CREATESTRUCT ) );
    cs.style = dwStyle;
    if( !PreCreateWindow( cs ) ) {
        PostNcDestroy();
        return( FALSE );
    }

    m_pCreateContext = pContext;
    AfxHookWindowCreate( this );
    HWND hWnd = ::CreateDialogParam( AfxGetInstanceHandle(), m_lpszTemplateName,
                                     pParentWnd->GetSafeHwnd(), AfxDlgProc, 0L );
    AfxUnhookWindowCreate();
    m_pCreateContext = NULL;
    
    if( hWnd == NULL ) {
        PostNcDestroy();
        return( FALSE );
    }
    ASSERT( m_hWnd == hWnd );

    CRect rectDialog;
    ::GetWindowRect( hWnd, &rectDialog );
    SetScrollSizes( MM_TEXT, rectDialog.Size() );

    ::SetWindowLong( hWnd, GWL_ID, nID );
    ::SetWindowPos( hWnd, NULL, rect.left, rect.top, rect.right - rect.left,
                    rect.bottom - rect.top,
                    SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW );
    return( TRUE );
}

void CFormView::OnDraw( CDC *pDC )
/********************************/
{
    // There is no drawing necessary for form views.
    UNUSED_ALWAYS( pDC );
}

void CFormView::OnInitialUpdate()
/*******************************/
{
    CScrollView::OnInitialUpdate();
    UpdateData( FALSE );
}

BOOL CFormView::PreTranslateMessage( MSG *pMsg )
/**********************************************/
{
    if( CScrollView::PreTranslateMessage( pMsg ) ) {
        return( TRUE );
    }
    return( ::IsDialogMessage( m_hWnd, pMsg ) );
}

#ifdef _DEBUG

void CFormView::Dump( CDumpContext &dc ) const
/********************************************/
{
    CScrollView::Dump( dc );

    if( IS_INTRESOURCE( m_lpszTemplateName ) ) {
        dc << "m_lpszTemplateName = " << (UINT)m_lpszTemplateName << "\n";
    } else {
        dc << "m_lpszTemplateName = " << m_lpszTemplateName << "\n";
    }
    dc << "m_pCreateContext = " << m_pCreateContext << "\n";
}

#endif // _DEBUG

int CFormView::OnCreate( LPCREATESTRUCT lpCreateStruct )
/******************************************************/
{
    ASSERT( lpCreateStruct != NULL );
    lpCreateStruct->lpCreateParams = m_pCreateContext;
    return( CScrollView::OnCreate( lpCreateStruct ) );
}
