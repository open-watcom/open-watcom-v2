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
* Description:  Implementation of CFindReplaceDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "cdhook.h"

IMPLEMENT_DYNAMIC( CFindReplaceDialog, CCommonDialog )

CFindReplaceDialog::CFindReplaceDialog()
    : CCommonDialog( NULL )
/*************************/
{
    memset( &m_fr, 0, sizeof( FINDREPLACE ) );
    m_fr.lStructSize = sizeof( FINDREPLACE );
    m_fr.hwndOwner = NULL;
    m_fr.hInstance = AfxGetResourceHandle();
    m_fr.Flags = FR_ENABLEHOOK;
    m_fr.lpstrFindWhat = m_szFindWhat;
    m_fr.lpstrReplaceWith = m_szReplaceWith;
    m_fr.wFindWhatLen = 128;
    m_fr.wReplaceWithLen = 128;
    m_fr.lCustData = 0L;
    m_fr.lpfnHook = AfxCommDlgProc;
    m_fr.lpTemplateName = NULL;

    m_szFindWhat[0] = _T('\0');
    m_szReplaceWith[0] = _T('\0');
}

BOOL CFindReplaceDialog::Create( BOOL bFindDialogOnly, LPCTSTR lpszFindWhat,
                                 LPCTSTR lpszReplaceWith, DWORD dwFlags,
                                 CWnd *pParentWnd )
/*************************************************/
{
    if( lpszFindWhat != NULL ) {
        _tcscpy( m_fr.lpstrFindWhat, lpszFindWhat );
    }
    if( lpszReplaceWith != NULL ) {
        _tcscpy( m_fr.lpstrReplaceWith, lpszReplaceWith );
    }
    m_fr.Flags |= dwFlags;
    m_fr.hwndOwner = pParentWnd->GetSafeHwnd();

    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pAlternateWndInit == NULL );
    pState->m_pAlternateWndInit = this;

    HWND hWnd = NULL;
    if( bFindDialogOnly ) {
        hWnd = ::FindText( &m_fr );
    } else {
        hWnd = ::ReplaceText( &m_fr );
    }
    if( hWnd == NULL ) {
        PostNcDestroy();
        return( FALSE );
    }
    ASSERT( m_hWnd == hWnd );
    return( TRUE );
}

void CFindReplaceDialog::PostNcDestroy()
/**************************************/
{
    delete this;
}

#ifdef _DEBUG

void CFindReplaceDialog::AssertValid() const
/******************************************/
{
    CCommonDialog::AssertValid();

    ASSERT( m_fr.lStructSize == sizeof( FINDREPLACE ) );
}

void CFindReplaceDialog::Dump( CDumpContext &dc ) const
/*****************************************************/
{
    CCommonDialog::Dump( dc );

    dc << "m_fr.lStructSize = " << m_fr.lStructSize << "\n";
    dc << "m_fr.hwndOwner = " << m_fr.hwndOwner << "\n";
    dc << "m_fr.hInstance = " << m_fr.hInstance << "\n";
    dc << "m_fr.Flags = ";
    dc.DumpAsHex( m_fr.Flags );
    dc << "\n";
    dc << "m_fr.lpstrFindWhat = " << m_fr.lpstrFindWhat << "\n";
    dc << "m_fr.lpstrReplaceWith = " << m_fr.lpstrReplaceWith << "\n";
    dc << "m_fr.wFindWhatLen = " << m_fr.wFindWhatLen << "\n";
    dc << "m_fr.wReplaceWithLen = " << m_fr.wReplaceWithLen << "\n";
    dc << "m_fr.lCustData = ";
    dc.DumpAsHex( m_fr.lCustData );
    dc << "\n";
    if( m_fr.lpfnHook == AfxCommDlgProc ) {
        dc << "m_fr.lpfnHook = AfxCommDlgProc\n";
    } else {
        dc << "m_fr.lpfnHook = " << m_fr.lpfnHook << "\n";
    }
    if( IS_INTRESOURCE( m_fr.lpTemplateName ) ) {
        dc << "m_fr.lpTemplateName = " << (UINT)m_fr.lpTemplateName << "\n";
    } else {
        dc << "m_fr.lpTemplateName = " << m_fr.lpTemplateName << "\n";
    }
}

#endif // _DEBUG

CFindReplaceDialog * PASCAL CFindReplaceDialog::GetNotifier( LPARAM lParam )
/**************************************************************************/
{
    return( (CFindReplaceDialog *)((BYTE *)lParam -
        offsetof( CFindReplaceDialog, m_fr )) );
}
