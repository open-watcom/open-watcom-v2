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
* Description:  Implementation of CPrintDialogEx.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CPrintDialogEx, CCommonDialog )

BEGIN_INTERFACE_MAP( CPrintDialogEx, CCommonDialog )
    INTERFACE_PART( CPrintDialogEx, IID_IPrintDialogCallback, PrintDialogCallback )
END_INTERFACE_MAP()

CPrintDialogEx::CPrintDialogEx( DWORD dwFlags, CWnd *pParentWnd )
    : CCommonDialog( pParentWnd )
/*******************************/
{
    dwFlags |= PD_RETURNDC;
    memset( &m_pdex, 0, sizeof( PRINTDLGEX ) );
    m_pdex.lStructSize = sizeof( PRINTDLGEX );
    m_pdex.hwndOwner = pParentWnd->GetSafeHwnd();
    m_pdex.hDevMode = NULL;
    m_pdex.hDevNames = NULL;
    m_pdex.hDC = NULL;
    m_pdex.Flags = dwFlags;
    m_pdex.Flags2 = 0L;
    m_pdex.ExclusionFlags = 0L;
    m_pdex.nPageRanges = 0L;
    m_pdex.nMaxPageRanges = 0L;
    m_pdex.lpPageRanges = NULL;
    m_pdex.nMinPage = 0L;
    m_pdex.nMaxPage = 0L;
    m_pdex.nCopies = 0L;
    m_pdex.hInstance = NULL;
    m_pdex.lpPrintTemplateName = NULL;
    m_pdex.lpCallback = &m_xPrintDialogCallback;
    m_pdex.nPropertyPages = 0L;
    m_pdex.lphPropertyPages = NULL;
    m_pdex.nStartPage = START_PAGE_GENERAL;
    m_pdex.dwResultAction = 0L;
}

LRESULT CPrintDialogEx::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
/*********************************************************************************/
{
    UNUSED_ALWAYS( message );
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( lParam );
    return( 0L );
}

INT_PTR CPrintDialogEx::DoModal()
/*******************************/
{
    return( ::PrintDlgEx( &m_pdex ) );
}

#ifdef _DEBUG

void CPrintDialogEx::AssertValid() const
/**************************************/
{
    CCommonDialog::AssertValid();

    ASSERT( m_pdex.lStructSize == sizeof( PRINTDLGEX ) );
    ASSERT( m_pdex.lpCallback == &m_xPrintDialogCallback );
}

void CPrintDialogEx::Dump( CDumpContext &dc ) const
/*************************************************/
{
    CCommonDialog::Dump( dc );

    dc << "m_pdex.lStructSize = " << m_pdex.lStructSize << "\n";
    dc << "m_pdex.hwndOwner = " << m_pdex.hwndOwner << "\n";
    dc << "m_pdex.hDevMode = " << m_pdex.hDevMode << "\n";
    dc << "m_pdex.hDevNames = " << m_pdex.hDevNames << "\n";
    dc << "m_pdex.hDC = " << m_pdex.hDC << "\n";
    dc << "m_pdex.Flags = ";
    dc.DumpAsHex( m_pdex.Flags );
    dc << "\n";
    dc << "m_pdex.Flags2 = ";
    dc.DumpAsHex( m_pdex.Flags2 );
    dc << "\n";
    dc << "m_pdex.ExclusionFlags = ";
    dc.DumpAsHex( m_pdex.ExclusionFlags );
    dc << "\n";
    dc << "m_pdex.nPageRanges = " << m_pdex.nPageRanges << "\n";
    dc << "m_pdex.nMaxPageRanges = " << m_pdex.nMaxPageRanges << "\n";
    dc << "m_pdex.lpPageRanges = " << m_pdex.lpPageRanges << "\n";
    dc << "m_pdex.nMinPage = " << m_pdex.nMinPage << "\n";
    dc << "m_pdex.nMaxPage = " << m_pdex.nMaxPage << "\n";
    dc << "m_pdex.nCopies = " << m_pdex.nCopies << "\n";
    dc << "m_pdex.hInstance = " << m_pdex.hInstance << "\n";
    if( IS_INTRESOURCE( m_pdex.lpPrintTemplateName ) ) {
        dc << "m_pdex.lpPrintTemplateName = " << (UINT)m_pdex.lpPrintTemplateName
           << "\n";
    } else {
        dc << "m_pdex.lpPrintTemplateName = " << m_pdex.lpPrintTemplateName << "\n";
    }
    if( m_pdex.lpCallback == &m_xPrintDialogCallback ) {
        dc << "m_pdex.lpCallback = &m_xPrintDialogCallback\n";
    } else {
        dc << "m_pdex.lpCallback = " << m_pdex.lpCallback << "\n";
    }
    dc << "m_pdex.nPropertyPages = " << m_pdex.nPropertyPages << "\n";
    dc << "m_pdex.lphPropertyPages = " << m_pdex.lphPropertyPages << "\n";
    dc << "m_pdex.nStartPage = " << m_pdex.nStartPage << "\n";
    dc << "m_pdex.dwResultAction = ";
    switch( m_pdex.dwResultAction ) {
    case PD_RESULT_CANCEL:
        dc << "PD_RESULT_CANCEL\n";
        break;
    case PD_RESULT_PRINT:
        dc << "PD_RESULT_PRINT\n";
        break;
    case PD_RESULT_APPLY:
        dc << "PD_RESULT_APPLY\n";
        break;
    default:
        dc << m_pdex.dwResultAction << "\n";
        break;
    }
}

#endif // _DEBUG

HDC CPrintDialogEx::CreatePrinterDC()
/***********************************/
{
    return( AfxCreateDC( m_pdex.hDevNames, m_pdex.hDevMode ) );
}

BOOL CPrintDialogEx::GetDefaults()
/********************************/
{
    m_pdex.Flags |= PD_RETURNDEFAULT;
    return( SUCCEEDED( ::PrintDlgEx( &m_pdex ) ) );
}

CString CPrintDialogEx::GetDeviceName() const
/*******************************************/
{
    if( m_pdex.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_pdex.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wDeviceOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}

LPDEVMODE CPrintDialogEx::GetDevMode() const
/******************************************/
{
    if( m_pdex.hDevMode == NULL ) {
        return( NULL );
    }
    return( (LPDEVMODE)::GlobalLock( m_pdex.hDevMode ) );
}

CString CPrintDialogEx::GetDriverName() const
/*******************************************/
{
    if( m_pdex.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_pdex.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wDriverOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}

CString CPrintDialogEx::GetPortName() const
/*****************************************/
{
    if( m_pdex.hDevNames == NULL ) {
        return( (LPCTSTR)NULL );
    }
    LPDEVNAMES lpDevNames = (LPDEVNAMES)::GlobalLock( m_pdex.hDevNames );
    CString str( (LPCTSTR)lpDevNames + lpDevNames->wOutputOffset );
    ::GlobalUnlock( lpDevNames );
    return( str );
}

HRESULT CPrintDialogEx::XPrintDialogCallback::QueryInterface( REFIID riid,
                                                              void **ppvObject )
/******************************************************************************/
{
    METHOD_PROLOGUE( CPrintDialogEx, PrintDialogCallback )
    return( pThis->InternalQueryInterface( &riid, ppvObject ) );
}

ULONG CPrintDialogEx::XPrintDialogCallback::AddRef()
/**************************************************/
{
    METHOD_PROLOGUE( CPrintDialogEx, PrintDialogCallback )
    return( pThis->InternalAddRef() );
}

ULONG CPrintDialogEx::XPrintDialogCallback::Release()
/***************************************************/
{
    METHOD_PROLOGUE( CPrintDialogEx, PrintDialogCallback )
    return( pThis->InternalRelease() );
}

HRESULT CPrintDialogEx::XPrintDialogCallback::InitDone()
/******************************************************/
{
    return( S_FALSE );
}

HRESULT CPrintDialogEx::XPrintDialogCallback::SelectionChange()
/*************************************************************/
{
    return( S_FALSE );
}

HRESULT CPrintDialogEx::XPrintDialogCallback::HandleMessage( HWND hDlg, UINT uMsg,
                                                             WPARAM wParam,
                                                             LPARAM lParam,
                                                             LRESULT *pResult )
/*****************************************************************************/
{
    METHOD_PROLOGUE( CPrintDialogEx, PrintDialogCallback )

    if( pThis->m_hWnd == NULL ) {
        pThis->Attach( hDlg );
    }
    ASSERT( pThis->m_hWnd == hDlg );
    
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    pState->m_msgCur.hwnd = hDlg;
    pState->m_msgCur.message = uMsg;
    pState->m_msgCur.wParam = wParam;
    pState->m_msgCur.lParam = lParam;
    pState->m_msgCur.time = ::GetMessageTime();
    DWORD dwPos = ::GetMessagePos();
    pState->m_msgCur.pt.x = LOWORD( dwPos );
    pState->m_msgCur.pt.y = HIWORD( dwPos );
    
    ASSERT( pResult != NULL );
    if( uMsg == WM_INITDIALOG ) {
        *pResult = pThis->OnInitDialog();
        return( S_FALSE );
    }
    
    if( pThis->OnWndMsg( uMsg, wParam, lParam, pResult ) ) {
        if( uMsg == WM_NOTIFY ) {
            ::SetWindowLong( ::GetParent( hDlg ), DWL_MSGRESULT, *pResult );
        }
        return( S_OK );
    } else {
        return( S_FALSE );
    }
}
