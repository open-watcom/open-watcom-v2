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
* Description:  Implementation of AfxDlgProc and CDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "dialog.h"

INT_PTR CALLBACK AfxDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
/**********************************************************************************/
{
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( lParam );
    
    if( message == WM_INITDIALOG ) {
        CDialog *pDialog = (CDialog *)CWnd::FromHandlePermanent( hWnd );
        if( pDialog != NULL && pDialog->IsKindOf( RUNTIME_CLASS( CDialog ) ) ) {
            return( pDialog->OnInitDialog() );
        } else {
            return( 1 );
        }
    }
    return( 0 );
}
    
IMPLEMENT_DYNAMIC( CDialog, CWnd )

BEGIN_MESSAGE_MAP( CDialog, CWnd )
    ON_COMMAND( IDOK, OnOK )
    ON_COMMAND( IDCANCEL, OnCancel )
END_MESSAGE_MAP()

CDialog::CDialog()
/****************/
{
    m_lpszTemplateName = NULL;
    m_hDialogTemplate = NULL;
    m_lpDialogTemplate = NULL;
    m_lpDialogInit = NULL;
    m_pParentWnd = NULL;
}

CDialog::CDialog( LPCTSTR lpszTemplateName, CWnd *pParentWnd )
/************************************************************/
{
    m_lpszTemplateName = lpszTemplateName;
    m_hDialogTemplate = NULL;
    m_lpDialogTemplate = NULL;
    m_lpDialogInit = NULL;
    m_pParentWnd = pParentWnd;
}

CDialog::CDialog( UINT nIDTemplate, CWnd *pParentWnd )
/****************************************************/
{
    m_lpszTemplateName = MAKEINTRESOURCE( nIDTemplate );
    m_hDialogTemplate = NULL;
    m_lpDialogTemplate = NULL;
    m_lpDialogInit = NULL;
    m_pParentWnd = pParentWnd;
}

BOOL CDialog::Create( LPCTSTR lpszTemplateName, CWnd *pParentWnd )
/****************************************************************/
{
    ASSERT( lpszTemplateName != NULL );
    if( pParentWnd == NULL ) {
        pParentWnd = AfxGetMainWnd();
    }
    AfxHookWindowCreate( this );
    HINSTANCE hInstance = AfxFindResourceHandle( lpszTemplateName, RT_DIALOG );
    HWND hWnd = ::CreateDialogParam( hInstance, lpszTemplateName,
                                     pParentWnd->GetSafeHwnd(), AfxDlgProc, 0L );
    if( !AfxUnhookWindowCreate() ) {
        PostNcDestroy();
        return( FALSE );
    }
    if( hWnd == NULL ) {
        PostNcDestroy();
        return( FALSE );
    }
    ASSERT( m_hWnd == hWnd );
    return( TRUE );
}

BOOL CDialog::Create( UINT nIDTemplate, CWnd *pParentWnd )
/********************************************************/
{
    return( Create( MAKEINTRESOURCE( nIDTemplate ), pParentWnd ) );
}

BOOL CDialog::CreateIndirect( LPCDLGTEMPLATE lpDialogTemplate, CWnd *pParentWnd,
                              void *pDialogInit )
/***********************************************/
{
    ASSERT( lpDialogTemplate != NULL );
    m_lpDialogInit = pDialogInit;
    if( pParentWnd == NULL ) {
        pParentWnd = AfxGetMainWnd();
    }
    AfxHookWindowCreate( this );
    HWND hWnd = ::CreateDialogIndirectParam( AfxGetResourceHandle(), lpDialogTemplate,
                                             pParentWnd->GetSafeHwnd(), AfxDlgProc, 0L );
    if( !AfxUnhookWindowCreate() ) {
        return( FALSE );
    }
    if( hWnd == NULL ) {
        return( FALSE );
    }
    ASSERT( m_hWnd == hWnd );
    return( TRUE );
}

BOOL CDialog::CreateIndirect( HGLOBAL hDialogTemplate, CWnd *pParentWnd )
/***********************************************************************/
{
    ASSERT( hDialogTemplate != NULL );
    LPCDLGTEMPLATE lpDialogTemplate = (LPCDLGTEMPLATE)::LockResource( hDialogTemplate );
    if( lpDialogTemplate == NULL ) {
        return( FALSE );
    }
    return( CreateIndirect( lpDialogTemplate, pParentWnd, NULL ) );
}

INT_PTR CDialog::DoModal()
/************************/
{
    if( m_lpszTemplateName != NULL ) {
        HINSTANCE hInstance = AfxFindResourceHandle( m_lpszTemplateName, RT_DIALOG );

        AfxHookWindowCreate( this );
        INT_PTR nResult = ::DialogBoxParam( hInstance, m_lpszTemplateName,
                                            m_pParentWnd->GetSafeHwnd(),
                                            AfxDlgProc, 0L );
        AfxUnhookWindowCreate();
        return( nResult );
    } else {
        if( m_lpDialogTemplate == NULL && m_hDialogTemplate != NULL ) {
            m_lpDialogTemplate = (LPCDLGTEMPLATE)::LockResource( m_hDialogTemplate );
        }
        if( m_lpDialogTemplate == NULL ) {
            return( -1 );
        }
        AfxHookWindowCreate( this );
        INT_PTR nResult = ::DialogBoxIndirectParam( AfxGetResourceHandle(),
                                                    m_lpDialogTemplate,
                                                    m_pParentWnd->GetSafeHwnd(),
                                                    AfxDlgProc, 0L );
        AfxUnhookWindowCreate();
        return( nResult );
    }
}

void CDialog::OnCancel()
/**********************/
{
    EndDialog( IDCANCEL );
}

BOOL CDialog::OnInitDialog()
/**************************/
{
    UpdateData( FALSE );
    return( TRUE );
}

void CDialog::OnOK()
/******************/
{
    if( UpdateData() ) {
        EndDialog( IDOK );
    }
}

BOOL CDialog::PreTranslateMessage( MSG *pMsg )
/********************************************/
{
    if( CWnd::PreTranslateMessage( pMsg ) ) {
        return( TRUE );
    }
    return( ::IsDialogMessage( m_hWnd, pMsg ) );
}

#ifdef _DEBUG

void CDialog::Dump( CDumpContext &dc ) const
/******************************************/
{
    CWnd::Dump( dc );

    dc << "m_nIDHelp = " << m_nIDHelp << "\n";
    if( IS_INTRESOURCE( m_lpszTemplateName ) ) {
        dc << "m_lpszTemplateName = " << (UINT)m_lpszTemplateName << "\n";
    } else {
        dc << "m_lpszTemplateName = " << m_lpszTemplateName << "\n";
    }
    dc << "m_hDialogTemplate = " << m_hDialogTemplate << "\n";
    dc << "m_lpDialogTemplate = " << m_lpDialogTemplate << "\n";
    dc << "m_lpDialogInit = " << m_lpDialogInit << "\n";
    dc << "m_pParentWnd = " << (void *)m_pParentWnd << "\n";
}

#endif // _DEBUG

BOOL CDialog::InitModalIndirect( LPCDLGTEMPLATE lpDialogTemplate, CWnd *pParentWnd,
                                 void *lpDialogInit )
/***************************************************/
{
    ASSERT( lpDialogTemplate != NULL );
    m_lpszTemplateName = NULL;
    m_hDialogTemplate = NULL;
    m_lpDialogTemplate = lpDialogTemplate;
    m_lpDialogInit = lpDialogInit;
    m_pParentWnd = pParentWnd;
    return( TRUE );
}

BOOL CDialog::InitModalIndirect( HGLOBAL hDialogTemplate, CWnd *pParentWnd )
/**************************************************************************/
{
    ASSERT( hDialogTemplate != NULL );
    m_lpszTemplateName = NULL;
    m_hDialogTemplate = hDialogTemplate;
    m_lpDialogTemplate = NULL;
    m_lpDialogInit = NULL;
    m_pParentWnd = pParentWnd;
    return( TRUE );
}
