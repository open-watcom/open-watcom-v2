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
* Description:  Implementation of CColorDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "cdhook.h"

// Array used to store custom colors
static COLORREF _CustomColors[16];

// Registered message to set the color
static UINT _SetRGBMessage = ::RegisterWindowMessage( SETRGBSTRING );

IMPLEMENT_DYNAMIC( CColorDialog, CCommonDialog )

CColorDialog::CColorDialog( COLORREF clrInit, DWORD dwFlags, CWnd *pParentWnd )
    : CCommonDialog( pParentWnd )
/*******************************/
{
    m_cc.lStructSize = sizeof( CHOOSECOLOR );
    m_cc.hwndOwner = pParentWnd->GetSafeHwnd();
    m_cc.hInstance = (HWND)AfxGetResourceHandle();
    m_cc.rgbResult = clrInit;
    m_cc.lpCustColors = _CustomColors;
    m_cc.Flags = dwFlags | CC_RGBINIT | CC_ENABLEHOOK;
    m_cc.lpfnHook = AfxCommDlgProc;
    m_cc.lpTemplateName = NULL;
}

BOOL CColorDialog::OnColorOK()
/****************************/
{
    return( FALSE );
}

INT_PTR CColorDialog::DoModal()
/*****************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pAlternateWndInit == NULL );
    pState->m_pAlternateWndInit = this;
    return( ::ChooseColor( &m_cc ) ? IDOK : IDCANCEL );
}

#ifdef _DEBUG

void CColorDialog::AssertValid() const
/************************************/
{
    CCommonDialog::AssertValid();

    ASSERT( m_cc.lStructSize == sizeof( CHOOSECOLOR ) );
}

void CColorDialog::Dump( CDumpContext &dc ) const
/***********************************************/
{
    CCommonDialog::Dump( dc );

    dc << "m_cc.lStructSize = " << m_cc.lStructSize << "\n";
    dc << "m_cc.hwndOwner = " << m_cc.hwndOwner << "\n";
    dc << "m_cc.hInstance = " << m_cc.hInstance << "\n";
    dc << "m_cc.rgbResult = ";
    dc.DumpAsHex( m_cc.rgbResult );
    dc << "\n";
    dc << "m_cc.lpCustColors = " << m_cc.lpCustColors << "\n";
    dc << "m_cc.Flags = ";
    dc.DumpAsHex( m_cc.Flags );
    dc << "\n";
    dc << "m_cc.lCustData = ";
    dc.DumpAsHex( m_cc.lCustData );
    dc << "\n";
    if( m_cc.lpfnHook == AfxCommDlgProc ) {
        dc << "m_cc.lpfnHook = AfxCommDlgProc\n";
    } else {
        dc << "m_cc.lpfnHook = " << m_cc.lpfnHook << "\n";
    }
    if( IS_INTRESOURCE( m_cc.lpTemplateName ) ) {
        dc << "m_cc.lpTemplateName = " << (UINT)m_cc.lpTemplateName << "\n";
    } else {
        dc << "m_cc.lpTemplateName = " << m_cc.lpTemplateName << "\n";
    }
}

#endif // _DEBUG

void CColorDialog::SetCurrentColor( COLORREF clr )
/************************************************/
{
    ::SendMessage( m_hWnd, _SetRGBMessage, 0, clr );
}

COLORREF * CColorDialog::GetSavedCustomColors()
/*********************************************/
{
    return( _CustomColors );
}
