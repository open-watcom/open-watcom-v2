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
* Description:  Implementation of CPrintDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "cdhook.h"

IMPLEMENT_DYNAMIC( CPrintDialog, CCommonDialog )

CPrintDialog::CPrintDialog( BOOL bPrintSetupOnly, DWORD dwFlags, CWnd *pParentWnd )
    : CCommonDialog( pParentWnd )
/*******************************/
{
    if( bPrintSetupOnly ) {
        dwFlags |= PD_PRINTSETUP;
    } else {
        dwFlags |= PD_RETURNDC;
    }
    m_pd.lStructSize = sizeof( PRINTDLG );
    m_pd.hwndOwner = pParentWnd->GetSafeHwnd();
    m_pd.hDevMode = NULL;
    m_pd.hDevNames = NULL;
    m_pd.hDC = NULL;
    m_pd.Flags = dwFlags | PD_ENABLEPRINTHOOK | PD_ENABLESETUPHOOK;
    m_pd.nFromPage = 0;
    m_pd.nToPage = 0;
    m_pd.nMinPage = 0;
    m_pd.nMaxPage = 0;
    m_pd.nCopies = 0;
    m_pd.hInstance = AfxGetResourceHandle();
    m_pd.lCustData = 0L;
    m_pd.lpfnPrintHook = AfxCommDlgProc;
    m_pd.lpfnSetupHook = AfxCommDlgProc;
    m_pd.lpPrintTemplateName = NULL;
    m_pd.lpSetupTemplateName = NULL;
    m_pd.hPrintTemplate = NULL;
    m_pd.hSetupTemplate = NULL;
}

INT_PTR CPrintDialog::DoModal()
/*****************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pAlternateWndInit == NULL );
    pState->m_pAlternateWndInit = this;
    return( ::PrintDlg( &m_pd ) ? IDOK : IDCANCEL );
}

#ifdef _DEBUG

void CPrintDialog::AssertValid() const
/************************************/
{
    CCommonDialog::AssertValid();

    ASSERT( m_pd.lStructSize == sizeof( PRINTDLG ) );
}

void CPrintDialog::Dump( CDumpContext &dc ) const
/***********************************************/
{
    CCommonDialog::Dump( dc );

    dc << "m_pd.lStructSize = " << m_pd.lStructSize << "\n";
    dc << "m_pd.hwndOwner = " << m_pd.hwndOwner << "\n";
    dc << "m_pd.hDevMode = " << m_pd.hDevMode << "\n";
    dc << "m_pd.hDevNames = " << m_pd.hDevNames << "\n";
    dc << "m_pd.hDC = " << m_pd.hDC << "\n";
    dc << "m_pd.Flags = ";
    dc.DumpAsHex( m_pd.Flags );
    dc << "\n";
    dc << "m_pd.nFromPage = " << m_pd.nFromPage << "\n";
    dc << "m_pd.nToPage = " << m_pd.nToPage << "\n";
    dc << "m_pd.nMinPage = " << m_pd.nMinPage << "\n";
    dc << "m_pd.nMaxPage = " << m_pd.nMaxPage << "\n";
    dc << "m_pd.nCopies = " << m_pd.nCopies << "\n";
    dc << "m_pd.hInstance = " << m_pd.hInstance << "\n";
    dc << "m_pd.lCustData = ";
    dc.DumpAsHex( m_pd.lCustData );
    dc << "\n";
    if( m_pd.lpfnPrintHook == AfxCommDlgProc ) {
        dc << "m_pd.lpfnPrintHook = AfxCommDlgProc\n";
    } else {
        dc << "m_pd.lpfnPrintHook = " << m_pd.lpfnPrintHook << "\n";
    }
    if( m_pd.lpfnSetupHook == AfxCommDlgProc ) {
        dc << "m_pd.lpfnSetupHook = AfxCommDlgProc\n";
    } else {
        dc << "m_pd.lpfnSetupHook = " << m_pd.lpfnSetupHook << "\n";
    }
    if( IS_INTRESOURCE( m_pd.lpPrintTemplateName ) ) {
        dc << "m_pd.lpPrintTemplateName = " << (UINT)m_pd.lpPrintTemplateName << "\n";
    } else {
        dc << "m_pd.lpPrintTemplateName = " << m_pd.lpPrintTemplateName << "\n";
    }
    if( IS_INTRESOURCE( m_pd.lpSetupTemplateName ) ) {
        dc << "m_pd.lpSetupTemplateName = " << (UINT)m_pd.lpSetupTemplateName << "\n";
    } else {
        dc << "m_pd.lpPrintTemplateName = " << m_pd.lpSetupTemplateName << "\n";
    }
    dc << "m_pd.hPrintTemplate = " << m_pd.hPrintTemplate << "\n";
    dc << "m_pd.hSetupTemplate = " << m_pd.hSetupTemplate << "\n";
}

#endif // _DEBUG

HDC CPrintDialog::CreatePrinterDC()
/*********************************/
{
    return( AfxCreateDC( m_pd.hDevNames, m_pd.hDevMode ) );
}

BOOL CPrintDialog::GetDefaults()
/******************************/
{
    m_pd.Flags |= PD_RETURNDEFAULT;
    return( ::PrintDlg( &m_pd ) );
}

CString CPrintDialog::GetDeviceName() const
/*****************************************/
{
    if( m_pd.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_pd.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wDeviceOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}

LPDEVMODE CPrintDialog::GetDevMode() const
/****************************************/
{
    if( m_pd.hDevMode == NULL ) {
        return( NULL );
    }
    return( (LPDEVMODE)::GlobalLock( m_pd.hDevMode ) );
}

CString CPrintDialog::GetDriverName() const
/*****************************************/
{
    if( m_pd.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_pd.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wDriverOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}

CString CPrintDialog::GetPortName() const
/***************************************/
{
    if( m_pd.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_pd.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wOutputOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}
