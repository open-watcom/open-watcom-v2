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
* Description:  Implementation of CPropertyPage.
*
****************************************************************************/


#include "stdafx.h"
#include "dialog.h"

#define PROPSHEETPAGE_STRUCTSIZE_V1 40
#define PROPSHEETPAGE_STRUCTSIZE_V2 48
#define PROPSHEETPAGE_STRUCTSIZE_V3 52
#define PROPSHEETPAGE_STRUCTSIZE_V4 56

UINT CALLBACK _PropertyPageProc( HWND hWnd, UINT message, LPPROPSHEETPAGE pPSP )
/******************************************************************************/
{
    UNUSED_ALWAYS( hWnd );
    if( message == PSPCB_CREATE ) {
        ASSERT( pPSP != NULL );
        CPropertyPage *pPage = (CPropertyPage *)pPSP->lParam;
        ASSERT( pPage != NULL );
        ASSERT( pPage->IsKindOf( RUNTIME_CLASS( CPropertyPage ) ) );
        AfxHookWindowCreate( pPage );
        return( 1 );
    }
    return( 0 );
}

IMPLEMENT_DYNAMIC( CPropertyPage, CDialog )

CPropertyPage::CPropertyPage()
/****************************/
{
    m_psp.dwSize = PROPSHEETPAGE_STRUCTSIZE_V4;
    Construct( (UINT)0 );
}

CPropertyPage::CPropertyPage( UINT nIDTemplate, UINT nIDCaption, DWORD dwSize )
/*****************************************************************************/
{
    m_psp.dwSize = dwSize;
    Construct( nIDTemplate, nIDCaption );
}

CPropertyPage::CPropertyPage( LPCTSTR lpszTemplateName, UINT nIDCaption, DWORD dwSize )
/*************************************************************************************/
{
    m_psp.dwSize = dwSize;
    Construct( lpszTemplateName, nIDCaption );
}

CPropertyPage::CPropertyPage( UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle,
                              UINT nIDHeaderSubTitle, DWORD dwSize )
/******************************************************************/
{
    m_psp.dwSize = dwSize;
    Construct( nIDTemplate, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle );
}

CPropertyPage::CPropertyPage( LPCTSTR lpszTemplateName, UINT nIDCaption,
                              UINT nIDHeaderTitle, UINT nIDHeaderSubTitle, DWORD dwSize )
/***************************************************************************************/
{
    m_psp.dwSize = dwSize;
    Construct( lpszTemplateName, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle );
}

BOOL CPropertyPage::OnApply()
/***************************/
{
    OnOK();
    return( TRUE );
}

BOOL CPropertyPage::OnKillActive()
/********************************/
{
    return( UpdateData() );
}

BOOL CPropertyPage::OnQueryCancel()
/*********************************/
{
    return( TRUE );
}

void CPropertyPage::OnReset()
/***************************/
{
    OnCancel();
}

BOOL CPropertyPage::OnSetActive()
/*******************************/
{
    if( m_bFirstSetActive ) {
        m_bFirstSetActive = FALSE;
    } else {
        UpdateData( FALSE );
    }
    return( TRUE );
}

LRESULT CPropertyPage::OnWizardBack()
/***********************************/
{
    return( 0 );
}

LRESULT CPropertyPage::OnWizardFinish()
/*************************************/
{
    return( 0 );
}

LRESULT CPropertyPage::OnWizardNext()
/***********************************/
{
    return( 0 );
}

void CPropertyPage::OnCancel()
/****************************/
{
}

BOOL CPropertyPage::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT *pResult )
/****************************************************************************/
{
    NMHDR *pNMHDR = (NMHDR *)lParam;
    ASSERT( pNMHDR != NULL );
    ASSERT( pResult != NULL );
    switch( pNMHDR->code ) {
    case PSN_APPLY:
        *pResult = OnApply() ? PSNRET_NOERROR : PSNRET_INVALID;
        return( TRUE );
    case PSN_KILLACTIVE:
        *pResult = !OnKillActive();
        return( TRUE );
    case PSN_QUERYCANCEL:
        *pResult = !OnQueryCancel();
        return( TRUE );
    case PSN_RESET:
        OnReset();
        *pResult = 0L;
        return( TRUE );
    case PSN_SETACTIVE:
        *pResult = OnSetActive() ? 0 : -1;
        return( TRUE );
    case PSN_WIZBACK:
        *pResult = OnWizardBack();
        return( TRUE );
    case PSN_WIZFINISH:
        *pResult = OnWizardFinish();
        return( TRUE );
    case PSN_WIZNEXT:
        *pResult = OnWizardNext();
        return( TRUE );
    }
    return( CDialog::OnNotify( wParam, lParam, pResult ) );
}

void CPropertyPage::OnOK()
/************************/
{
}

#ifdef _DEBUG

void CPropertyPage::AssertValid() const
/*************************************/
{
    CDialog::AssertValid();

    ASSERT( m_psp.dwSize == PROPSHEETPAGE_STRUCTSIZE_V1 ||
            m_psp.dwSize == PROPSHEETPAGE_STRUCTSIZE_V2 ||
            m_psp.dwSize == PROPSHEETPAGE_STRUCTSIZE_V3 ||
            m_psp.dwSize == PROPSHEETPAGE_STRUCTSIZE_V4 );
}

void CPropertyPage::Dump( CDumpContext &dc ) const
/************************************************/
{
    CDialog::Dump( dc );

    dc << "m_psp.dwSize = " << m_psp.dwSize << "\n";
    dc << "m_psp.dwFlags = ";
    dc.DumpAsHex( m_psp.dwFlags );
    dc << "\n";
    dc << "m_psp.hInstance = " << m_psp.hInstance << "\n";
    if( m_psp.dwFlags & PSP_DLGINDIRECT ) {
        dc << "m_psp.pResource = " << m_psp.pResource << "\n";
    } else if( IS_INTRESOURCE( m_psp.pszTemplate ) ) {
        dc << "m_psp.pszTemplate = " << (UINT)m_psp.pszTemplate << "\n";
    } else {
        dc << "m_psp.pszTemplate = " << m_psp.pszTemplate << "\n";
    }
    if( m_psp.dwFlags & PSP_USEHICON ) {
        dc << "m_psp.hIcon = " << m_psp.hIcon << "\n";
    } else if( IS_INTRESOURCE( m_psp.pszIcon ) ) {
        dc << "m_psp.pszIcon = " << (UINT)m_psp.pszIcon << "\n";
    } else {
        dc << "m_psp.pszIcon = " << m_psp.pszIcon << "\n";
    }
    dc << "m_psp.pszTitle = " << m_psp.pszTitle << "\n";
    if( m_psp.pfnDlgProc == AfxDlgProc ) {
        dc << "m_psp.pfnDlgProc = AfxDlgProc\n";
    } else {
        dc << "m_psp.pfnDlgProc = " << m_psp.pfnDlgProc << "\n";
    }
    dc << "m_psp.lParam = ";
    dc.DumpAsHex( m_psp.lParam );
    dc << "\n";
    dc << "m_psp.pfnCallback = " << m_psp.pfnCallback << "\n";
    dc << "m_psp.pcRefParent = " << m_psp.pcRefParent << "\n";
}

#endif // _DEBUG

void CPropertyPage::CancelToClose()
/*********************************/
{
    HWND hWndPS = ::GetParent( m_hWnd );
    ::SendMessage( hWndPS, PSM_CANCELTOCLOSE, 0, 0L );
}

void CPropertyPage::Construct( LPCTSTR lpszTemplateName, UINT nIDCaption,
                               UINT nIDHeaderTitle, UINT nIDHeaderSubTitle )
/**************************************************************************/
{
    m_psp.dwFlags = PSP_USECALLBACK;
    m_psp.hInstance = AfxFindResourceHandle( lpszTemplateName, RT_DIALOG );
    m_psp.pszTemplate = lpszTemplateName;
    m_psp.hIcon = NULL;
    if( nIDCaption != 0 ) {
        m_strCaption.LoadString( nIDCaption );
        m_psp.pszTitle = (LPCTSTR)m_strCaption;
        m_psp.dwFlags |= PSP_USETITLE;
    } else {
        m_psp.pszTitle = NULL;
    }
    m_psp.pfnDlgProc = AfxDlgProc;
    m_psp.lParam = (LPARAM)this;
    m_psp.pfnCallback = _PropertyPageProc;
    m_psp.pcRefParent = NULL;
    if( nIDHeaderTitle != 0 ) {
        m_strHeaderTitle.LoadString( nIDHeaderTitle );
        m_psp.pszHeaderTitle = (LPCTSTR)m_strHeaderTitle;
        m_psp.dwFlags |= PSP_USEHEADERTITLE;
    } else {
        m_psp.pszHeaderTitle = NULL;
    }
    if( nIDHeaderSubTitle != 0 ) {
        m_strHeaderSubTitle.LoadString( nIDHeaderSubTitle );
        m_psp.pszHeaderSubTitle = (LPCTSTR)m_strHeaderSubTitle;
        m_psp.dwFlags |= PSP_USEHEADERSUBTITLE;
    } else {
        m_psp.pszHeaderSubTitle = NULL;
    }
    m_bFirstSetActive = TRUE;
}

LRESULT CPropertyPage::QuerySiblings( WPARAM wParam, LPARAM lParam )
/******************************************************************/
{
    HWND hWndPS = ::GetParent( m_hWnd );
    return( ::SendMessage( hWndPS, PSM_QUERYSIBLINGS, wParam, lParam ) );
}

void CPropertyPage::SetModified( BOOL bChanged )
/**********************************************/
{
    HWND hWndPS = ::GetParent( m_hWnd );
    if( bChanged ) {
        ::SendMessage( hWndPS, PSM_CHANGED, (WPARAM)m_hWnd, 0L );
    } else {
        ::SendMessage( hWndPS, PSM_UNCHANGED, (WPARAM)m_hWnd, 0L );
    }
}
