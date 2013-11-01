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
* Description:  Implementation of CFileFind.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CFileFind, CObject )

CFileFind::CFileFind()
/********************/
{
    m_pFoundInfo = NULL;
    m_pNextInfo = NULL;
    m_hContext = NULL;
    m_chDirSeparator = _T('\\');
}

CFileFind::~CFileFind()
/*********************/
{
    Close();
}

void CFileFind::CloseContext()
/****************************/
{
    if( m_hContext != NULL && m_hContext != INVALID_HANDLE_VALUE ) {
        ::FindClose( m_hContext );
        m_hContext = NULL;
    }
}

BOOL CFileFind::FindFile( LPCTSTR pstrName, DWORD dwUnused )
/**********************************************************/
{
    UNUSED_ALWAYS( dwUnused );
    
    TCHAR   szRoot[MAX_PATH];
    DWORD   cchRoot;
    LPTSTR  lpszFilePart;
    if( m_pNextInfo == NULL ) {
        m_pNextInfo = new WIN32_FIND_DATA;
    }
    cchRoot = ::GetFullPathName( pstrName, MAX_PATH, szRoot, &lpszFilePart );
    if( cchRoot == 0 || cchRoot > MAX_PATH ) {
        return( FALSE );
    }
    *lpszFilePart = _T('\0');
    m_hContext = ::FindFirstFile( pstrName, (LPWIN32_FIND_DATA)m_pNextInfo );
    if( m_hContext == INVALID_HANDLE_VALUE ) {
        return( FALSE );
    }
    m_strRoot = szRoot;
    return( TRUE );
}

BOOL CFileFind::FindNextFile()
/****************************/
{
    if( m_pFoundInfo == NULL ) {
        m_pFoundInfo = new WIN32_FIND_DATA;
    }
    void *pTemp = m_pFoundInfo;
    m_pFoundInfo = m_pNextInfo;
    m_pNextInfo = pTemp;
    return( ::FindNextFile( m_hContext, (LPWIN32_FIND_DATA)m_pNextInfo ) );
}

BOOL CFileFind::GetCreationTime( FILETIME *pTimeStamp ) const
/***********************************************************/
{
    memcpy( pTimeStamp, &((LPWIN32_FIND_DATA)m_pFoundInfo)->ftCreationTime,
            sizeof( FILETIME ) );
    return( TRUE );
}

BOOL CFileFind::GetCreationTime( CTime &refTime ) const
/*****************************************************/
{
    refTime = CTime( ((LPWIN32_FIND_DATA)m_pFoundInfo)->ftCreationTime );
    return( TRUE );
}

CString CFileFind::GetFileName() const
/************************************/
{
    TCHAR   szFullPath[MAX_PATH];
    DWORD   cchFullPath;
    LPTSTR  lpszFilePart;
    cchFullPath = ::GetFullPathName( ((LPWIN32_FIND_DATA)m_pFoundInfo)->cFileName,
                                     MAX_PATH, szFullPath, &lpszFilePart );
    if( cchFullPath == 0 || cchFullPath > MAX_PATH ) {
        return( CString() );
    }
    return( lpszFilePart );
}

CString CFileFind::GetFilePath() const
/************************************/
{
    CString strFilePath = m_strRoot;
    if( strFilePath.Right( 1 ).Compare( _T("\\") ) != 0 ) {
        strFilePath += _T("\\");
    }
    strFilePath += ((LPWIN32_FIND_DATA)m_pFoundInfo)->cFileName;
    return( strFilePath );
}

CString CFileFind::GetFileTitle() const
/*************************************/
{
    TCHAR   szFileTitle[MAX_PATH];
    if( ::GetFileTitle( ((LPWIN32_FIND_DATA)m_pFoundInfo)->cFileName,
                        szFileTitle, MAX_PATH ) != 0 ) {
        return( GetFileName() );
    }
    return( szFileTitle );
}

CString CFileFind::GetFileURL() const
/***********************************/
{
    CString strURL( _T("file://") );
    strURL += GetFilePath();
    return( strURL );
}

BOOL CFileFind::GetLastAccessTime( FILETIME *pTimeStamp ) const
/*************************************************************/
{
    memcpy( pTimeStamp, &((LPWIN32_FIND_DATA)m_pFoundInfo)->ftLastAccessTime,
            sizeof( FILETIME ) );
    return( TRUE );
}

BOOL CFileFind::GetLastAccessTime( CTime &refTime ) const
/*******************************************************/
{
    refTime = CTime( ((LPWIN32_FIND_DATA)m_pFoundInfo)->ftLastAccessTime );
    return( TRUE );
}

BOOL CFileFind::GetLastWriteTime( FILETIME *pTimeStamp ) const
/************************************************************/
{
    memcpy( pTimeStamp, &((LPWIN32_FIND_DATA)m_pFoundInfo)->ftLastWriteTime,
            sizeof( FILETIME ) );
    return( TRUE );
}

BOOL CFileFind::GetLastWriteTime( CTime &refTime ) const
/******************************************************/
{
    refTime = CTime( ((LPWIN32_FIND_DATA)m_pFoundInfo)->ftLastWriteTime );
    return( TRUE );
}

CString CFileFind::GetRoot() const
/********************************/
{
    return( m_strRoot );
}

BOOL CFileFind::IsDots() const
/****************************/
{
    CString strFileName = GetFileName();
    return( strFileName == _T(".") || strFileName == _T("..") );
}

BOOL CFileFind::MatchesMask( DWORD dwMask ) const
/***********************************************/
{
    return( (((LPWIN32_FIND_DATA)m_pFoundInfo)->dwFileAttributes & dwMask) == dwMask );
}

#ifdef _DEBUG

void CFileFind::Dump( CDumpContext &dc ) const
/********************************************/
{
    CObject::Dump( dc );

    dc << "m_pFoundInfo = " << m_pFoundInfo << "\n";
    dc << "m_pNextInfo = " << m_pNextInfo << "\n";
    dc << "m_hContext = " << (void *)m_hContext << "\n";
    dc << "m_strRoot = " << m_strRoot << "\n";
    dc << "m_chDirSeparator = " << m_chDirSeparator << "\n";
}

#endif // _DEBUG

void CFileFind::Close()
/*********************/
{
    if( m_pFoundInfo != NULL ) {
        delete (LPWIN32_FIND_DATA)m_pFoundInfo;
    }
    if( m_pNextInfo != NULL ) {
        delete (LPWIN32_FIND_DATA)m_pNextInfo;
    }
    CloseContext();
}

ULONGLONG CFileFind::GetLength() const
/************************************/
{
    LPWIN32_FIND_DATA   lpwfd = (LPWIN32_FIND_DATA)m_pFoundInfo;
    return( ((ULONGLONG)lpwfd->nFileSizeHigh << 32) + lpwfd->nFileSizeLow );
}
