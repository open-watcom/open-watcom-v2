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
* Description:  Implementation of CFileDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "cdhook.h"

#define OPENFILENAME_STRUCTSIZE     76
#define OPENFILENAME_STRUCTSIZE_W2K 88

IMPLEMENT_DYNAMIC( CFileDialog, CCommonDialog )

CFileDialog::CFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
                          DWORD dwFlags, LPCTSTR lpszFilter, CWnd *pParentWnd,
                          DWORD dwSize )
    : CCommonDialog( pParentWnd )
/*******************************/
{
    m_bOpenFileDialog = bOpenFileDialog;
    if( dwSize == 0 ) {
        OSVERSIONINFO osvi;
        osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
        ::GetVersionEx( &osvi );
        if( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 5 ) {
            m_ofn.lStructSize = OPENFILENAME_STRUCTSIZE_W2K;
        } else {
            m_ofn.lStructSize = OPENFILENAME_STRUCTSIZE;
        }
    } else {
        m_ofn.lStructSize = dwSize;
    }
    m_ofn.hwndOwner = pParentWnd->GetSafeHwnd();
    m_ofn.hInstance = AfxGetResourceHandle();
    m_ofn.lpstrFilter = lpszFilter;
    m_ofn.lpstrCustomFilter = NULL;
    m_ofn.nMaxCustFilter = 0;
    m_ofn.nFilterIndex = 0;
    m_ofn.lpstrFile = m_szFileName;
    m_ofn.nMaxFile = MAX_PATH;
    m_ofn.lpstrFileTitle = m_szFileTitle;
    m_ofn.nMaxFileTitle = 64;
    m_ofn.lpstrInitialDir = NULL;
    m_ofn.lpstrTitle = NULL;
    m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLEHOOK;
    m_ofn.nFileOffset = 0;
    m_ofn.nFileExtension = 0;
    m_ofn.lpstrDefExt = lpszDefExt;
    m_ofn.lCustData = 0;
    m_ofn.lpfnHook = AfxCommDlgProc;
    m_ofn.lpTemplateName = NULL;
    if( lpszFileName != NULL ) {
        _tcscpy( m_szFileName, lpszFileName );
    } else {
        m_szFileName[0] = _T('\0');
    }
}

void CFileDialog::OnFileNameChange()
/**********************************/
{
}

BOOL CFileDialog::OnFileNameOK()
/******************************/
{
    return( FALSE );
}

void CFileDialog::OnFolderChange()
/********************************/
{
}

void CFileDialog::OnInitDone()
/****************************/
{
}

void CFileDialog::OnLBSelChangedNotify( UINT nIDBox, UINT iCurSel, UINT nCode )
/*****************************************************************************/
{
    UNUSED_ALWAYS( nIDBox );
    UNUSED_ALWAYS( iCurSel );
    UNUSED_ALWAYS( nCode );
}

UINT CFileDialog::OnShareViolation( LPCTSTR lpszPathName )
/********************************************************/
{
    UNUSED_ALWAYS( lpszPathName );
    return( OFN_SHAREWARN );
}

void CFileDialog::OnTypeChange()
/******************************/
{
}

INT_PTR CFileDialog::DoModal()
/****************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pAlternateWndInit == NULL );
    pState->m_pAlternateWndInit = this;
    
    BOOL bRet;
    if( m_bOpenFileDialog ) {
        bRet = ::GetOpenFileName( &m_ofn );
    } else {
        bRet = ::GetSaveFileName( &m_ofn );
    }
    return( bRet ? IDOK : IDCANCEL );
}

BOOL CFileDialog::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT *pResult )
/**************************************************************************/
{
    OFNOTIFY *pofn = (OFNOTIFY *)lParam;
    ASSERT( pofn != NULL );
    switch( pofn->hdr.code ) {
    case CDN_FILEOK:
        if( OnFileNameOK() ) {
            *pResult = 1L;
            ::SetWindowLong( m_hWnd, DWL_MSGRESULT, 1L );
        } else {
            *pResult = 0;
        }
        return( TRUE );
    case CDN_FOLDERCHANGE:
        OnFolderChange();
        return( TRUE );
    case CDN_INITDONE:
        OnInitDone();
        return( TRUE );
    case CDN_SHAREVIOLATION:
        *pResult = OnShareViolation( pofn->pszFile );
        return( TRUE );
    case CDN_TYPECHANGE:
        OnTypeChange();
        return( TRUE );
    }
    return( CDialog::OnNotify( wParam, lParam, pResult ) );
}

#ifdef _DEBUG

void CFileDialog::AssertValid() const
/***********************************/
{
    CCommonDialog::AssertValid();

    ASSERT( m_ofn.lStructSize == OPENFILENAME_STRUCTSIZE ||
            m_ofn.lStructSize == OPENFILENAME_STRUCTSIZE_W2K );
}

void CFileDialog::Dump( CDumpContext &dc ) const
/**********************************************/
{
    CCommonDialog::Dump( dc );

    dc << "m_ofn.lStructSize = " << m_ofn.lStructSize << "\n";
    dc << "m_ofn.hwndOwner = " << m_ofn.hwndOwner << "\n";
    dc << "m_ofn.hInstance = " << m_ofn.hInstance << "\n";
    dc << "m_ofn.lpstrFilter = " << m_ofn.lpstrFilter << "\n";
    dc << "m_ofn.lpstrCustomFilter = " << m_ofn.lpstrCustomFilter << "\n";
    dc << "m_ofn.nMaxCustFilter = " << m_ofn.nMaxCustFilter << "\n";
    dc << "m_ofn.nFilterIndex = " << m_ofn.nFilterIndex << "\n";
    dc << "m_ofn.lpstrFile = " << m_ofn.lpstrFile << "\n";
    dc << "m_ofn.nMaxFile = " << m_ofn.nMaxFile << "\n";
    dc << "m_ofn.lpstrFileTitle = " << m_ofn.lpstrFileTitle << "\n";
    dc << "m_ofn.nMaxFileTitle = " << m_ofn.nMaxFileTitle << "\n";
    dc << "m_ofn.lpstrInitialDir = " << m_ofn.lpstrInitialDir << "\n";
    dc << "m_ofn.lpstrTitle = " << m_ofn.lpstrTitle << "\n";
    dc << "m_ofn.Flags = ";
    dc.DumpAsHex( m_ofn.Flags );
    dc << "\n";
    dc << "m_ofn.nFileOffset = " << m_ofn.nFileOffset << "\n";
    dc << "m_ofn.nFileExtension = " << m_ofn.nFileExtension << "\n";
    dc << "m_ofn.lpstrDefExt = " << m_ofn.lpstrDefExt << "\n";
    dc << "m_ofn.lCustData = ";
    dc.DumpAsHex( m_ofn.lCustData );
    dc << "\n";
    if( m_ofn.lpfnHook == AfxCommDlgProc ) {
        dc << "m_ofn.lpfnHook = AfxCommDlgProc\n";
    } else {
        dc << "m_ofn.lpfnHook = " << m_ofn.lpfnHook << "\n";
    }
    if( IS_INTRESOURCE( m_ofn.lpTemplateName ) ) {
        dc << "m_ofn.lpTemplateName = " << (UINT)m_ofn.lpTemplateName << "\n";
    } else {
        dc << "m_ofn.lpTemplateName = " << m_ofn.lpTemplateName << "\n";
    }
    dc << "m_bOpenFileDialog = " << m_bOpenFileDialog << "\n";
}

#endif // _DEBUG

CString CFileDialog::GetFolderPath() const
/****************************************/
{
    TCHAR szBuffer[MAX_PATH];
    ::SendMessage( m_hWnd, CDM_GETFOLDERPATH, MAX_PATH, (LPARAM)szBuffer );
    return( szBuffer );
}

CString CFileDialog::GetNextPathName( POSITION &pos ) const
/*********************************************************/
{
    ASSERT( pos != NULL );
    if( m_ofn.Flags & OFN_ALLOWMULTISELECT ) {
        LPCTSTR lpszFile = (LPCTSTR)pos;
        TCHAR   chSep = (m_ofn.Flags & OFN_EXPLORER ? _T('\0') : _T(' '));
        LPCTSTR lpszNext = _tcschr( lpszFile, chSep );
        if( lpszNext == NULL ) {
            pos = NULL;
        } else {
            lpszNext++;
            if( lpszNext[0] == _T('\0') ) {
                pos = NULL;
            } else {
                pos = (POSITION)lpszNext;
            }
        }
        CString strPath( m_szFileName );
        strPath += _T("\\");
        strPath += lpszFile;
        return( strPath );
    } else {
        pos = NULL;
        return( m_szFileName );
    }
}

POSITION CFileDialog::GetStartPosition() const
/********************************************/
{
    if( m_ofn.Flags & OFN_ALLOWMULTISELECT ) {
        LPCTSTR lpszPath = _tcschr( m_szFileName, _T('\0') );
        ASSERT( lpszPath != NULL );
        lpszPath++;
        ASSERT( lpszPath[0] != _T('\0') );
        return( (POSITION)lpszPath );
    } else {
        return( (POSITION)m_szFileName );
    }
}

void CFileDialog::SetTemplate( LPCTSTR lpWin3ID, LPCTSTR lpWin4ID )
/*****************************************************************/
{
    OSVERSIONINFO osvi;
    ::GetVersionEx( &osvi );
    if( osvi.dwMajorVersion >= 4 ) {
        m_ofn.lpTemplateName = lpWin4ID;
    } else {
        m_ofn.lpTemplateName = lpWin3ID;
    }
    m_ofn.Flags |= OFN_ENABLETEMPLATE;
}
