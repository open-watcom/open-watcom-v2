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
* Description:  Implementation of CPropertySheet.
*
****************************************************************************/


#include "stdafx.h"

#define PROPSHEETHEADER_STRUCTSIZE_V1   40
#define PROPSHEETHEADER_STRUCTSIZE_V2   52

int CALLBACK _PropertySheetProc( HWND hWnd, UINT message, LPARAM lParam )
/***********************************************************************/
{
    UNUSED_ALWAYS( hWnd );
    if( message == PSCB_PRECREATE ) {
        _AFX_THREAD_STATE *pState = AfxGetThreadState();
        ASSERT( pState != NULL );
        if( pState->m_dwPropStyle != 0 || pState->m_dwPropExStyle != 0 ) {
            DLGTEMPLATE *pTemplate = (DLGTEMPLATE *)lParam;
            ASSERT( pTemplate != NULL );
            DWORD dwOld;
            ::VirtualProtect( pTemplate, sizeof( DLGTEMPLATE ), PAGE_READWRITE, &dwOld );
            if( pTemplate->style & DS_SETFONT ) {
                pTemplate->style = pState->m_dwPropStyle | DS_SETFONT;
            } else {
                pTemplate->style = pState->m_dwPropStyle & ~DS_SETFONT;
            }
            pTemplate->dwExtendedStyle = pState->m_dwPropExStyle;
            ::VirtualProtect( pTemplate, sizeof( DLGTEMPLATE ), dwOld, NULL );
        }
        return( 1 );
    }
    return( 0 );
}

IMPLEMENT_DYNAMIC( CPropertySheet, CWnd )

BEGIN_MESSAGE_MAP( CPropertySheet, CWnd )
    ON_MESSAGE( WM_INITDIALOG, HandleInitDialog )
END_MESSAGE_MAP()

CPropertySheet::CPropertySheet()
/******************************/
{
    Construct( (UINT)0 );
}

CPropertySheet::CPropertySheet( UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage )
/***********************************************************************************/
{
    Construct( nIDCaption, pParentWnd, iSelectPage );
}

CPropertySheet::CPropertySheet( LPCTSTR lpszCaption, CWnd *pParentWnd, UINT iSelectPage )
/***************************************************************************************/
{
    Construct( lpszCaption, pParentWnd, iSelectPage );
}

CPropertySheet::CPropertySheet( UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage,
                                HBITMAP hbmWatermark, HPALETTE hpalWatermark,
                                HBITMAP hbmHeader )
/*************************************************/
{
    Construct( nIDCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark,
               hbmHeader );
}

CPropertySheet::CPropertySheet( LPCTSTR lpszCaption, CWnd *pParentWnd, UINT iSelectPage,
                                HBITMAP hbmWatermark, HPALETTE hpalWatermark,
                                HBITMAP hbmHeader )
/*************************************************/
{
    Construct( lpszCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark,
               hbmHeader );
}

CPropertySheet::~CPropertySheet()
/*******************************/
{
    if( m_psh.ppsp != NULL ) {
        delete [] m_psh.ppsp;
    }
}

BOOL CPropertySheet::Create( CWnd *pParentWnd, DWORD dwStyle, DWORD dwExStyle )
/*****************************************************************************/
{
    if( dwStyle == 0xFFFFFFFFL ) {
        dwStyle = WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | DS_CONTEXTHELP |
            WS_VISIBLE;
    }
    if( dwExStyle == 0L ) {
        dwExStyle = WS_EX_DLGMODALFRAME;
    }

    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    pState->m_dwPropStyle = dwStyle;
    pState->m_dwPropExStyle = dwExStyle;

    m_bModeless = TRUE;
    m_psh.dwFlags |= PSH_MODELESS | PSH_PROPSHEETPAGE;
    m_psh.hwndParent = pParentWnd->GetSafeHwnd();
    m_psh.nPages = m_pages.GetCount();
    if( m_psh.ppsp != NULL ) {
        delete [] m_psh.ppsp;
    }
    PROPSHEETPAGE *pPSP = new PROPSHEETPAGE[m_psh.nPages];
    for( int i = 0; i < m_psh.nPages; i++ ) {
        CPropertyPage *pPage = (CPropertyPage *)m_pages.GetAt( i );
        ASSERT( pPage != NULL );
        ASSERT( pPage->IsKindOf( RUNTIME_CLASS( CPropertyPage ) ) );
        memcpy( &pPSP[i], &pPage->m_psp, sizeof( PROPSHEETPAGE ) );
    }
    m_psh.ppsp = pPSP;
    AfxHookWindowCreate( this );
    HWND hWnd = (HWND)::PropertySheet( &m_psh );
    if( !AfxUnhookWindowCreate() ) {
        PostNcDestroy();
        return( NULL );
    }
    if( hWnd == NULL ) {
        PostNcDestroy();
        return( NULL );
    }
    ASSERT( m_hWnd == hWnd );
    
    return( TRUE );
}

INT_PTR CPropertySheet::DoModal()
/*******************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    pState->m_dwPropStyle = 0L;
    pState->m_dwPropExStyle = 0L;

    m_bModeless = FALSE;
    m_psh.dwFlags &= ~PSH_MODELESS;
    m_psh.dwFlags |= PSH_PROPSHEETPAGE;
    m_psh.nPages = m_pages.GetCount();
    if( m_psh.ppsp != NULL ) {
        delete [] m_psh.ppsp;
    }
    PROPSHEETPAGE *pPSP = new PROPSHEETPAGE[m_psh.nPages];
    for( int i = 0; i < m_psh.nPages; i++ ) {
        CPropertyPage *pPage = (CPropertyPage *)m_pages.GetAt( i );
        ASSERT( pPage != NULL );
        ASSERT( pPage->IsKindOf( RUNTIME_CLASS( CPropertyPage ) ) );
        memcpy( &pPSP[i], &pPage->m_psp, sizeof( PROPSHEETPAGE ) );
    }
    m_psh.ppsp = pPSP;
    AfxHookWindowCreate( this );
    INT_PTR nResult = ::PropertySheet( &m_psh );
    AfxUnhookWindowCreate();
    return( nResult );
}

BOOL CPropertySheet::OnInitDialog()
/*********************************/
{
    if( !m_bStacked ) {
        HWND hWndTab = ::GetDlgItem( m_hWnd, AFX_IDC_TAB_CONTROL );
        DWORD dwStyle = ::GetWindowLong( hWndTab, GWL_STYLE );
        dwStyle &= ~TCS_MULTILINE;
        ::SetWindowLong( hWndTab, GWL_STYLE, dwStyle );
        ::SetWindowPos( hWndTab, NULL, 0, 0, 0, 0,
                        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
    }
    return( Default() );
}

BOOL CPropertySheet::PreTranslateMessage( MSG *pMsg )
/***************************************************/
{
    if( (HWND)::SendMessage( m_hWnd, PSM_GETCURRENTPAGEHWND, 0, 0L ) == NULL ) {
        ::DestroyWindow( m_hWnd );
        return( TRUE );
    }
    if( ::SendMessage( m_hWnd, PSM_ISDIALOGMESSAGE, 0, (LPARAM)pMsg ) ) {
        return( TRUE );
    }
    return( CWnd::PreTranslateMessage( pMsg ) );
}

#ifdef _DEBUG

void CPropertySheet::AssertValid() const
/**************************************/
{
    CWnd::AssertValid();

    ASSERT( m_psh.dwSize == PROPSHEETHEADER_STRUCTSIZE_V1 ||
            m_psh.dwSize == PROPSHEETHEADER_STRUCTSIZE_V2 );
}

void CPropertySheet::Dump( CDumpContext &dc ) const
/*************************************************/
{
    CWnd::Dump( dc );

    dc << "m_psh.dwSize = " << m_psh.dwSize << "\n";
    dc << "m_psh.dwFlags = ";
    dc.DumpAsHex( m_psh.dwFlags );
    dc << "\n";
    dc << "m_psh.hwndParent = " << m_psh.hwndParent << "\n";
    dc << "m_psh.hInstance = " << m_psh.hInstance << "\n";
    if( m_psh.dwFlags & PSH_USEHICON ) {
        dc << "m_psh.hIcon = " << m_psh.hIcon << "\n";
    } else if( IS_INTRESOURCE( m_psh.pszIcon ) ) {
        dc << "m_psh.pszIcon = " << (UINT)m_psh.pszIcon << "\n";
    } else {
        dc << "m_psh.pszIcon = " << m_psh.pszIcon << "\n";
    }
    dc << "m_psh.pszCaption = " << m_psh.pszCaption << "\n";
    dc << "m_psh.nPages = " << m_psh.nPages << "\n";
    if( !(m_psh.dwFlags & PSH_USEPSTARTPAGE) ) {
        dc << "m_psh.nStartPage = " << m_psh.nStartPage << "\n";
    } else if( IS_INTRESOURCE( m_psh.pStartPage ) ) {
        dc << "m_psh.pStartPage = " << (UINT)m_psh.pStartPage << "\n";
    } else {
        dc << "m_psh.pStartPage = " << m_psh.pStartPage << "\n";
    }
    if( m_psh.dwFlags & PSH_PROPSHEETPAGE ) {
        dc << "m_psh.ppsp = " << m_psh.ppsp << "\n";
    } else {
        dc << "m_psh.phpage = " << m_psh.phpage << "\n";
    }
    dc << "m_psh.pfnCallback = " << m_psh.pfnCallback << "\n";
    if( m_psh.dwFlags & PSH_USEHBMWATERMARK ) {
        dc << "m_psh.hbmWatermark = " << m_psh.hbmWatermark << "\n";
    } else if( IS_INTRESOURCE( m_psh.pszbmWatermark ) ) {
        dc << "m_psh.pszbmWatermark = " << (UINT)m_psh.pszbmWatermark << "\n";
    } else {
        dc << "m_psh.pszbmWatermark = " << m_psh.pszbmWatermark << "\n";
    }
    dc << "m_psh.hplWatermark = " << m_psh.hplWatermark << "\n";
    if( m_psh.dwFlags & PSH_USEHBMHEADER ) {
        dc << "m_psh.hbmHeader = " << m_psh.hbmHeader << "\n";
    } else if( IS_INTRESOURCE( m_psh.pszbmHeader ) ) {
        dc << "m_psh.pszbmHeader = " << (UINT)m_psh.pszbmHeader << "\n";
    } else {
        dc << "m_psh.pszbmHeader = " << m_psh.pszbmHeader << "\n";
    }
}

#endif // _DEBUG

void CPropertySheet::AddPage( CPropertyPage *pPage )
/**************************************************/
{
    m_pages.Add( pPage );
    if( m_hWnd != NULL ) {
        HPROPSHEETPAGE hPSP = ::CreatePropertySheetPage( &pPage->m_psp );
        ::SendMessage( m_hWnd, PSM_ADDPAGE, 0, (LPARAM)hPSP );
    }
}

void CPropertySheet::Construct( UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage,
                                HBITMAP hbmWatermark, HPALETTE hpalWatermark,
                                HBITMAP hbmHeader )
/*************************************************/
{
    if( nIDCaption != 0 ) {
        m_strCaption.LoadString( nIDCaption );
        Construct( m_strCaption, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader );
    } else {
        Construct( (LPCTSTR)NULL, pParentWnd, iSelectPage, hbmWatermark, hpalWatermark, hbmHeader );
    }
}

void CPropertySheet::Construct( LPCTSTR pszCaption, CWnd *pParentWnd, UINT iSelectPage,
                                HBITMAP hbmWatermark, HPALETTE hpalWatermark,
                                HBITMAP hbmHeader )
/*************************************************/
{
    m_psh.dwSize = PROPSHEETHEADER_STRUCTSIZE_V2;
    m_psh.dwFlags = PSH_USECALLBACK;
    m_psh.hwndParent = pParentWnd->GetSafeHwnd();
    m_psh.hInstance = AfxGetResourceHandle();
    m_psh.hIcon = NULL;
    m_psh.pszCaption = pszCaption;
    if( pszCaption != NULL ) {
        m_psh.dwFlags |= PSH_PROPTITLE;
    }
    m_psh.nPages = 0;
    m_psh.nStartPage = iSelectPage;
    m_psh.ppsp = NULL;
    m_psh.pfnCallback = _PropertySheetProc;
    if( hbmWatermark != NULL ) {
        m_psh.hbmWatermark = hbmWatermark;
        m_psh.dwFlags |= PSH_WATERMARK | PSH_USEHBMWATERMARK;
    } else {
        m_psh.hbmWatermark = NULL;
    }
    if( hpalWatermark != NULL ) {
        m_psh.hplWatermark = hpalWatermark;
        m_psh.dwFlags |= PSH_USEHPLWATERMARK;
    } else {
        m_psh.hplWatermark = NULL;
    }
    if( hbmHeader != NULL ) {
        m_psh.hbmHeader = hbmHeader;
        m_psh.dwFlags |= PSH_USEHBMWATERMARK;
    } else {
        m_psh.hbmHeader = NULL;
    }
    m_bStacked = TRUE;
}

int CPropertySheet::GetActiveIndex() const
/****************************************/
{
    HWND hWndTab = (HWND)::SendMessage( m_hWnd, PSM_GETTABCONTROL, 0, 0L );
    return( (int)::SendMessage( hWndTab, TCM_GETCURSEL, 0, 0L ) );
}

CPropertyPage *CPropertySheet::GetActivePage() const
/**************************************************/
{
    int nIndex = GetActiveIndex();
    ASSERT( nIndex >= 0 && nIndex < m_psh.nPages );
    CPropertyPage *pPage = (CPropertyPage *)m_pages.GetAt( nIndex );
    ASSERT( pPage != NULL );
    ASSERT( pPage->IsKindOf( RUNTIME_CLASS( CPropertyPage ) ) );
    return( pPage );
}

int CPropertySheet::GetPageIndex( CPropertyPage *pPage )
/******************************************************/
{
    ASSERT( pPage != NULL );
    int nCount = m_pages.GetSize();
    for( int i = 0; i < nCount; i++ ) {
        if( m_pages.GetAt( i ) == pPage ) {
            return( i );
        }
    }
    return( -1 );
}

void CPropertySheet::RemovePage( CPropertyPage *pPage )
/*****************************************************/
{
    int nIndex = GetPageIndex( pPage );
    ASSERT( nIndex >= 0 );
    RemovePage( nIndex );
}

void CPropertySheet::RemovePage( int nPage )
/******************************************/
{
    m_pages.RemoveAt( nPage );
    if( m_hWnd != NULL ) {
        ::SendMessage( m_hWnd, PSM_REMOVEPAGE, nPage, 0L );
    }
}

BOOL CPropertySheet::SetActivePage( CPropertyPage *pPage )
/********************************************************/
{
    int nIndex = GetPageIndex( pPage );
    ASSERT( nIndex >= 0 );
    return( SetActivePage( nIndex ) );
}

BOOL CPropertySheet::SetActivePage( int nPage )
/*********************************************/
{
    HWND hWndTab = (HWND)::SendMessage( m_hWnd, PSM_GETTABCONTROL, 0, 0L );
    ::SendMessage( hWndTab, TCM_SETCURSEL, nPage, 0L );
    return( TRUE );
}

LRESULT CPropertySheet::HandleInitDialog( WPARAM wParam, LPARAM lParam )
/**********************************************************************/
{
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( lParam );
    return( OnInitDialog() );
}
