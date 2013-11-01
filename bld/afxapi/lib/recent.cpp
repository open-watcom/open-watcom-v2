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
* Description:  Implementation of CRecentFileList.
*
****************************************************************************/


#include "stdafx.h"

CRecentFileList::CRecentFileList( UINT nStart, LPCTSTR lpszSection,
                                  LPCTSTR lpszEntryFormat, int nSize,
                                  int nMaxDispLength )
/****************************************************/
{
    ASSERT( nSize > 0 );
    m_nSize = nSize;
    m_arrNames = new CString[nSize];
    m_strSectionName = lpszSection;
    m_strEntryName = lpszEntryFormat;
    m_nStart = nStart;
    m_nMaxDisplayLength = nMaxDispLength;
}

CRecentFileList::~CRecentFileList()
/*********************************/
{
    ASSERT( m_arrNames != NULL );
    delete [] m_arrNames;
}

void CRecentFileList::Add( LPCTSTR lpszPathName )
/***********************************************/
{
    ASSERT( lpszPathName != NULL );
    for( int i = 0; i < m_nSize; i++ ) {
        if( m_arrNames[i].CompareNoCase( lpszPathName ) == 0 ) {
            return;
        }
    }
    for( int i = m_nSize - 1; i > 0; i-- ) {
        m_arrNames[i] = m_arrNames[i - 1];
    }
    m_arrNames[0] = lpszPathName;
}

BOOL CRecentFileList::GetDisplayName( CString &strName, int nIndex, LPCTSTR lpszCurDir,
                                      int nCurDir, BOOL bAtLeastName ) const
/**************************************************************************/
{
    ASSERT( nIndex < m_nSize );
    if( m_arrNames[nIndex].IsEmpty() ) {
        return( FALSE );
    }
    if( lpszCurDir != NULL ) {
        if( _tcsnicmp( m_arrNames[nIndex], lpszCurDir, nCurDir ) == 0 &&
            _tcschr( (LPCTSTR)m_arrNames[nIndex] + nCurDir + 1, _T('\\') ) == NULL ) {
            LPTSTR lpszBuffer = strName.GetBuffer( MAX_PATH );
            ::GetFileTitle( m_arrNames[nIndex], lpszBuffer, MAX_PATH );
            strName.ReleaseBuffer();
            return( TRUE );
        }
    }
    if( m_arrNames[nIndex].GetLength() <= m_nMaxDisplayLength ) {
        strName = m_arrNames[nIndex];
        return( TRUE );
    }
    if( bAtLeastName ) {
        LPTSTR lpszBuffer = strName.GetBuffer( MAX_PATH );
        ::GetFileTitle( m_arrNames[nIndex], lpszBuffer, MAX_PATH );
        strName.ReleaseBuffer();
        return( TRUE );
    }

    LPTSTR lpszBuffer = strName.GetBuffer( m_nMaxDisplayLength + 1 );
    memset( lpszBuffer, 0, (m_nMaxDisplayLength + 1) * sizeof( TCHAR ) );
    _tcsncpy( lpszBuffer, m_arrNames[nIndex], m_nMaxDisplayLength - 3 );
    _tcscat( lpszBuffer, _T("...") );
    strName.ReleaseBuffer();
    return( TRUE );
}

void CRecentFileList::ReadList()
/******************************/
{
    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );

    CString strKey;
    for( int i = 0; i < m_nSize; i++ ) {
        strKey.Format( m_strEntryName, i );
        m_arrNames[i] = pApp->GetProfileString( m_strSectionName, strKey );
    }
}

void CRecentFileList::Remove( int nIndex )
/****************************************/
{
    ASSERT( nIndex >= 0 && nIndex < m_nSize );
    for( int i = nIndex; i < m_nSize; i++ ) {
        m_arrNames[i] = m_arrNames[i + 1];
    }
    m_arrNames[m_nSize].Empty();
}

void CRecentFileList::UpdateMenu( CCmdUI *pCmdUI )
/************************************************/
{
    ASSERT( pCmdUI != NULL );

    CMenu *pMenu = pCmdUI->m_pMenu;
    if( pMenu != NULL ) {
        if( m_strOriginal.IsEmpty() ) {
            pMenu->GetMenuString( m_nStart, m_strOriginal, MF_BYCOMMAND );
        }
        for( int i = 0; i < m_nSize; i++ ) {
            pMenu->RemoveMenu( m_nStart + i, MF_BYCOMMAND );
        }
        if( m_arrNames[0].IsEmpty() ) {
            pMenu->InsertMenu( pCmdUI->m_nIndex, MF_BYPOSITION, m_nStart,
                               m_strOriginal );
            pCmdUI->Enable( FALSE );
        } else {
            CString strDisplayName;
            TCHAR   szCurDir[MAX_PATH];
            CString strCaption;
            int     nCurDir = ::GetCurrentDirectory( MAX_PATH, szCurDir );
            for( int i = 0; i < m_nSize && !m_arrNames[i].IsEmpty(); i++ ) {
                if( i + 1 < 10 ) {
                    strCaption.Format( _T("&%d "), i + 1 );
                } else if( i + 1 == 10 ) {
                    strCaption = _T("1&0 ");
                } else {
                    strCaption.Format( _T("%d "), i + 1 );
                }
                GetDisplayName( strDisplayName, i, szCurDir, nCurDir );
                for( int j = 0; j < strDisplayName.GetLength(); j++ ) {
                    if( strDisplayName[j] == _T('&') ) {
                        strCaption += _T("&&");
                    } else {
                        strCaption.AppendChar( strDisplayName[j] );
                    }
                }
                pMenu->InsertMenu( pCmdUI->m_nIndex + i, MF_BYPOSITION, m_nStart + i,
                                   strCaption );
            }
        }
    }
}

void CRecentFileList::WriteList()
/*******************************/
{
    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );

    CString strKey;
    for( int i = 0; i < m_nSize; i++ ) {
        strKey.Format( m_strEntryName, i );
        pApp->WriteProfileString( m_strSectionName, strKey, m_arrNames[i] );
    }
}
