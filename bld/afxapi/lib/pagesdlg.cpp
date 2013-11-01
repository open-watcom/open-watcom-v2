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
* Description:  Implementation of CPageSetupDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "cdhook.h"

UINT_PTR CALLBACK _PagePaintProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
/***************************************************************************************/
{
    CPageSetupDialog *pPSD = (CPageSetupDialog *)CWnd::FromHandlePermanent( hWnd );
    if( pPSD != NULL ) {
        ASSERT( pPSD->IsKindOf( RUNTIME_CLASS( CPageSetupDialog ) ) );
        switch( message ) {
        case WM_PSD_PAGESETUPDLG:
            return( pPSD->PreDrawPage( LOWORD( wParam ), HIWORD( wParam ),
                                       (LPPAGESETUPDLG)lParam ) );
        case WM_PSD_FULLPAGERECT:
        case WM_PSD_MINMARGINRECT:
        case WM_PSD_MARGINRECT:
        case WM_PSD_GREEKTEXTRECT:
        case WM_PSD_ENVSTAMPRECT:
        case WM_PSD_YAFULLPAGERECT:
            return( pPSD->OnDrawPage( CDC::FromHandle( (HDC)wParam ), message,
                                      (LPRECT)lParam ) );
        }
    }
    return( 0L );
}

IMPLEMENT_DYNAMIC( CPageSetupDialog, CCommonDialog )

CPageSetupDialog::CPageSetupDialog( DWORD dwFlags, CWnd *pParentWnd )
    : CCommonDialog( pParentWnd )
/*******************************/
{
    m_psd.lStructSize = sizeof( PAGESETUPDLG );
    m_psd.hwndOwner = pParentWnd->GetSafeHwnd();
    m_psd.hDevMode = NULL;
    m_psd.hDevNames = NULL;
    m_psd.Flags = dwFlags | PSD_ENABLEPAGESETUPHOOK | PSD_ENABLEPAGEPAINTHOOK;
    memset( &m_psd.ptPaperSize, 0, sizeof( POINT ) );
    memset( &m_psd.rtMinMargin, 0, sizeof( RECT ) );
    memset( &m_psd.rtMargin, 0, sizeof( RECT ) );
    m_psd.hInstance = AfxGetResourceHandle();
    m_psd.lCustData = 0L;
    m_psd.lpfnPageSetupHook = AfxCommDlgProc;
    m_psd.lpfnPagePaintHook = _PagePaintProc;
    m_psd.lpPageSetupTemplateName = NULL;
    m_psd.hPageSetupTemplate = NULL;
}

CPageSetupDialog::~CPageSetupDialog()
/***********************************/
{
    if( m_psd.hDevMode != NULL ) {
        ::GlobalFree( m_psd.hDevMode );
    }
    if( m_psd.hDevNames != NULL ) {
        ::GlobalFree( m_psd.hDevNames );
    }
}

UINT CPageSetupDialog::OnDrawPage( CDC *pDC, UINT nMessage, LPRECT lpRect )
/*************************************************************************/
{
    UNUSED_ALWAYS( pDC );
    UNUSED_ALWAYS( nMessage );
    UNUSED_ALWAYS( lpRect );
    return( 0 );
}

UINT CPageSetupDialog::PreDrawPage( WORD wPaper, WORD wFlags, LPPAGESETUPDLG pPSD )
/*********************************************************************************/
{
    UNUSED_ALWAYS( wPaper );
    UNUSED_ALWAYS( wFlags );
    UNUSED_ALWAYS( pPSD );
    return( 0 );
}

INT_PTR CPageSetupDialog::DoModal()
/*********************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pAlternateWndInit == NULL );
    pState->m_pAlternateWndInit = this;
    return( ::PageSetupDlg( &m_psd ) ? IDOK : IDCANCEL );
}

#ifdef _DEBUG

void CPageSetupDialog::AssertValid() const
/****************************************/
{
    CCommonDialog::AssertValid();

    ASSERT( m_psd.lStructSize == sizeof( PAGESETUPDLG ) );
}

void CPageSetupDialog::Dump( CDumpContext &dc ) const
/***************************************************/
{
    CCommonDialog::Dump( dc );

    dc << "m_psd.lStructSize = " << m_psd.lStructSize << "\n";
    dc << "m_psd.hwndOwner = " << m_psd.hwndOwner << "\n";
    dc << "m_psd.hDevMode = " << m_psd.hDevMode << "\n";
    dc << "m_psd.hDevNames = " << m_psd.hDevNames << "\n";
    dc << "m_psd.Flags = ";
    dc.DumpAsHex( m_psd.Flags );
    dc << "\n";
    dc << "m_psd.ptPaperSize = " << m_psd.ptPaperSize << "\n";
    dc << "m_psd.rtMinMargin = " << m_psd.rtMinMargin << "\n";
    dc << "m_psd.rtMargin = " << m_psd.rtMargin << "\n";
    dc << "m_psd.hInstance = " << m_psd.hInstance << "\n";
    dc << "m_psd.lCustData = ";
    dc.DumpAsHex( m_psd.lCustData );
    dc << "\n";
    if( m_psd.lpfnPageSetupHook == AfxCommDlgProc ) {
        dc << "m_psd.lpfnPageSetupHook = AfxCommDlgProc\n";
    } else {
        dc << "m_psd.lpfnPageSetupHook = " << m_psd.lpfnPageSetupHook << "\n";
    }
    dc << "m_psd.lpfnPagePaintHook = " << m_psd.lpfnPagePaintHook << "\n";
    if( IS_INTRESOURCE( m_psd.lpPageSetupTemplateName ) ) {
        dc << "m_psd.lpPageSetupTemplateName = " << (UINT)m_psd.lpPageSetupTemplateName
           << "\n";
    } else {
        dc << "m_psd.lpPageSetupTemplateName = " << m_psd.lpPageSetupTemplateName
           << "\n";
    }
    dc << "m_psd.hPageSetupTemplate = " << m_psd.hPageSetupTemplate << "\n";
}

#endif // _DEBUG

HDC CPageSetupDialog::CreatePrinterDC()
/*************************************/
{
    return( AfxCreateDC( m_psd.hDevNames, m_psd.hDevMode ) );
}

CString CPageSetupDialog::GetDeviceName() const
/*********************************************/
{
    if( m_psd.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_psd.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wDeviceOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}

LPDEVMODE CPageSetupDialog::GetDevMode() const
/********************************************/
{
    if( m_psd.hDevMode == NULL ) {
        return( NULL );
    }
    return( (LPDEVMODE)::GlobalLock( m_psd.hDevMode ) );
}

CString CPageSetupDialog::GetDriverName() const
/*********************************************/
{
    if( m_psd.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_psd.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wDriverOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}

void CPageSetupDialog::GetMargins( LPRECT lpRectMargins, LPRECT lpRectMinMargins ) const
/**************************************************************************************/
{
    if( lpRectMargins != NULL ) {
        memcpy( lpRectMargins, &m_psd.rtMargin, sizeof( RECT ) );
    }
    if( lpRectMinMargins != NULL ) {
        memcpy( lpRectMinMargins, &m_psd.rtMinMargin, sizeof( RECT ) );
    }
}

CString CPageSetupDialog::GetPortName() const
/*******************************************/
{
    if( m_psd.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_psd.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wOutputOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}
