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
* Description:  Implementation of CFile.
*
****************************************************************************/


#include "stdafx.h"

const HANDLE CFile::hFileNull = INVALID_HANDLE_VALUE;

IMPLEMENT_DYNAMIC( CFile, CObject )

CFile::CFile()
/************/
{
    m_hFile = INVALID_HANDLE_VALUE;
    m_bCloseOnDelete = FALSE;
}

CFile::CFile( HANDLE hFile )
/**************************/
{
    m_hFile = hFile;
    m_bCloseOnDelete = FALSE;
}

CFile::CFile( LPCTSTR lpszFileName, UINT nOpenFlags )
/***************************************************/
{
    CFileException  ex;
    if( !Open( lpszFileName, nOpenFlags, &ex ) ) {
        throw new CFileException( ex.m_cause, ex.m_lOsError, ex.m_strFileName );
    }
}

CFile::~CFile()
/*************/
{
    if( m_bCloseOnDelete ) {
        Close();
    }
}

void CFile::Abort()
/*****************/
{
    if( m_hFile != INVALID_HANDLE_VALUE ) {
        ::CloseHandle( m_hFile );
    }
    m_hFile = INVALID_HANDLE_VALUE;
    m_bCloseOnDelete = FALSE;
    m_strFileName.Empty();
}

void CFile::Close()
/*****************/
{
    CString strFileName = m_strFileName;
    BOOL    bFail = FALSE;
    if( m_hFile != INVALID_HANDLE_VALUE && !::CloseHandle( m_hFile ) ) {
        bFail = TRUE;
    }
    m_hFile = INVALID_HANDLE_VALUE;
    m_bCloseOnDelete = FALSE;
    m_strFileName.Empty();
    if( bFail ) {
        CFileException::ThrowOsError( ::GetLastError(), strFileName );
    }
}

CFile *CFile::Duplicate() const
/*****************************/
{
    HANDLE  hFile;
    CFile   *pFile;
    if( !::DuplicateHandle( ::GetCurrentProcess(), m_hFile, ::GetCurrentProcess(),
                            &hFile, 0L, FALSE, DUPLICATE_SAME_ACCESS ) ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
    pFile = new CFile;
    pFile->m_hFile = hFile;
    pFile->m_bCloseOnDelete = m_bCloseOnDelete;
    return( pFile );
}

void CFile::Flush()
/*****************/
{
    if( !::FlushFileBuffers( m_hFile ) ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
}

CString CFile::GetFileName() const
/********************************/
{
    TCHAR   szFullPath[MAX_PATH];
    DWORD   cchFullPath;
    LPTSTR  lpFilePart;

    if( m_strFileName.IsEmpty() ) {
        return( m_strFileName );
    }
    cchFullPath = ::GetFullPathName( m_strFileName, MAX_PATH, szFullPath, &lpFilePart );
    if( cchFullPath == 0 || cchFullPath >= MAX_PATH ) {
        return( m_strFileName );
    }
    return( lpFilePart );
}

CString CFile::GetFileTitle() const
/*********************************/
{
    TCHAR   szFileTitle[MAX_PATH];
    if( ::GetFileTitle( m_strFileName, szFileTitle, MAX_PATH ) != 0 ) {
        return( GetFileName() );
    }
    return( szFileTitle );
}

ULONGLONG CFile::GetLength() const
/********************************/
{
    DWORD   dwFileSizeLow;
    DWORD   dwFileSizeHigh;
    if( (dwFileSizeLow = ::GetFileSize( m_hFile,
                                        &dwFileSizeHigh )) == INVALID_FILE_SIZE ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
    return( ((ULONGLONG)dwFileSizeHigh << 32) | dwFileSizeLow );
}

ULONGLONG CFile::GetPosition() const
/**********************************/
{
    DWORD   dwLowOff;
    DWORD   dwHighOff = 0L;
    if( (dwLowOff = ::SetFilePointer( m_hFile, 0L, (PLONG)&dwHighOff,
                                      FILE_CURRENT )) == INVALID_SET_FILE_POINTER ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
    return( ((ULONGLONG)dwHighOff << 32) | dwLowOff );
}

BOOL CFile::GetStatus( CFileStatus &rStatus ) const
/*************************************************/
{
    FILETIME    ftCreation;
    FILETIME    ftAccess;
    FILETIME    ftWrite;
    DWORD       dwAttributes;
    DWORD       dwFileSizeLow;
    DWORD       dwFileSizeHigh;
    TCHAR       szFullName[MAX_PATH];
    if( !::GetFileTime( m_hFile, &ftCreation, &ftAccess, &ftWrite ) ) {
        return( FALSE );
    }
    dwAttributes = ::GetFileAttributes( m_strFileName );
    if( dwAttributes  == INVALID_FILE_ATTRIBUTES ) {
        return( FALSE );
    }
    dwFileSizeLow = ::GetFileSize( m_hFile, &dwFileSizeHigh );
    if( dwFileSizeLow == INVALID_FILE_SIZE ) {
        return( FALSE );
    }
    if( !::GetFullPathName( m_strFileName, MAX_PATH, szFullName, NULL ) ) {
        return( FALSE );
    }
    rStatus.m_ctime = CTime( ftCreation );
    rStatus.m_mtime = CTime( ftWrite );
    rStatus.m_atime = CTime( ftAccess );
    rStatus.m_size = ((ULONGLONG)dwFileSizeHigh << 32) + dwFileSizeLow;
    rStatus.m_attribute = 0;
    if( dwAttributes & FILE_ATTRIBUTE_READONLY ) {
        rStatus.m_attribute |= readOnly;
    }
    if( dwAttributes & FILE_ATTRIBUTE_HIDDEN ) {
        rStatus.m_attribute |= hidden;
    }
    if( dwAttributes & FILE_ATTRIBUTE_SYSTEM ) {
        rStatus.m_attribute |= system;
    }
    if( dwAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
        rStatus.m_attribute |= directory;
    }
    if( dwAttributes & FILE_ATTRIBUTE_ARCHIVE ) {
        rStatus.m_attribute |= archive;
    }
    _tcscpy( rStatus.m_szFullName, szFullName );
    return( TRUE );
}
    
void CFile::LockRange( ULONGLONG dwPos, ULONGLONG dwCount )
/*********************************************************/
{
    if( !::LockFile( m_hFile, (DWORD)dwPos, (DWORD)(dwPos >> 32), (DWORD)dwCount,
                     (DWORD)(dwCount >> 32) ) ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
}

BOOL CFile::Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException *pError )
/*******************************************************************************/
{
    DWORD               dwDesiredAccess = 0;
    DWORD               dwShareMode = 0;
    DWORD               dwCreationDisposition = 0;
    DWORD               dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    SECURITY_ATTRIBUTES sa = { sizeof( SECURITY_ATTRIBUTES ), NULL, TRUE };
    HANDLE              hFile;
    TCHAR               szFullPath[MAX_PATH];
    DWORD               cchFullPath;

    switch( nOpenFlags & (modeRead | modeWrite | modeReadWrite) ) {
    case modeRead:
        dwDesiredAccess = GENERIC_READ;
        break;
    case modeWrite:
        dwDesiredAccess = GENERIC_WRITE;
        break;
    case modeReadWrite:
        dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
        break;
    }
    switch( nOpenFlags & (shareExclusive | shareDenyRead | shareDenyWrite |
                          shareDenyNone) ) {
    case shareExclusive:
        dwShareMode = 0;
        break;
    case shareDenyRead:
        dwShareMode = FILE_SHARE_WRITE;
        break;
    case shareDenyWrite:
        dwShareMode = FILE_SHARE_READ;
        break;
    case shareDenyNone:
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        break;
    }
    switch( nOpenFlags & (modeCreate | modeNoTruncate) ) {
    case modeCreate:
        dwCreationDisposition = CREATE_ALWAYS;
        break;
    case modeCreate | modeNoTruncate:
        dwCreationDisposition = OPEN_ALWAYS;
        break;
    default:
        dwCreationDisposition = OPEN_EXISTING;
        break;
    }
    if( nOpenFlags & modeNoInherit ) {
        sa.bInheritHandle = FALSE;
    }
    if( nOpenFlags & osNoBuffer ) {
        dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;
    }
    if( nOpenFlags & osWriteThrough ) {
        dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
    }
    if( nOpenFlags & osRandomAccess ) {
        dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
    }
    if( nOpenFlags & osSequentialScan ) {
        dwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
    }
    cchFullPath = ::GetFullPathName( lpszFileName, MAX_PATH, szFullPath, NULL );
    if( cchFullPath == 0 ) {
        if( pError != NULL ) {
            pError->m_cause = CFileException::OsErrorToException( ::GetLastError() );
            pError->m_lOsError = ::GetLastError();
            pError->m_strFileName = lpszFileName;
        }
        return( FALSE );
    } else if( cchFullPath >= MAX_PATH ) {
        return( FALSE );
    }
    m_bCloseOnDelete = FALSE;
    m_hFile = INVALID_HANDLE_VALUE;
    m_strFileName = szFullPath;
    if( (hFile = ::CreateFile( lpszFileName, dwDesiredAccess, dwShareMode, &sa,
                                 dwCreationDisposition, dwFlagsAndAttributes,
                                 NULL )) == INVALID_HANDLE_VALUE ) {
        if( pError != NULL ) {
            pError->m_cause = CFileException::OsErrorToException( ::GetLastError() );
            pError->m_lOsError = ::GetLastError();
            pError->m_strFileName = lpszFileName;
        }
        return( FALSE );
    }
    m_bCloseOnDelete = TRUE;
    m_hFile = hFile;
    return( TRUE );
}

UINT CFile::Read( void *lpBuf, UINT nCount )
/******************************************/
{
    DWORD   dwBytesRead;
    if( !::ReadFile( m_hFile, lpBuf, nCount, &dwBytesRead, NULL ) ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
    return( (UINT)dwBytesRead );
}

ULONGLONG CFile::Seek( LONGLONG lOff, UINT nFrom )
/************************************************/
{
    DWORD   dwMoveMethod;
    DWORD   dwHighOff = (DWORD)(lOff >> 32);
    DWORD   dwLowOff;
    switch( nFrom ) {
    case begin:
        dwMoveMethod = FILE_BEGIN;
        break;
    case end:
        dwMoveMethod = FILE_END;
        break;
    default:
        dwMoveMethod = FILE_CURRENT;
        break;
    }
    if( (dwLowOff = ::SetFilePointer( m_hFile, (DWORD)lOff, (PLONG)&dwHighOff,
                                      dwMoveMethod )) == INVALID_SET_FILE_POINTER ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
    return( ((ULONGLONG)dwHighOff << 32) | dwLowOff );
}

void CFile::SetLength( ULONGLONG dwNewLen )
/*****************************************/
{
    Seek( dwNewLen, begin );
    if( !::SetEndOfFile( m_hFile ) ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
}

void CFile::UnlockRange( ULONGLONG dwPos, ULONGLONG dwCount )
/***********************************************************/
{
    if( !::UnlockFile( m_hFile, (DWORD)dwPos, (DWORD)(dwPos >> 32), (DWORD)dwCount,
                       (DWORD)(dwCount >> 32) ) ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
}

void CFile::Write( const void *lpBuf, UINT nCount )
/*************************************************/
{
    DWORD   dwBytesWritten;
    if( !::WriteFile( m_hFile, lpBuf, nCount, &dwBytesWritten, NULL ) ) {
        CFileException::ThrowOsError( ::GetLastError(), m_strFileName );
    }
}

#ifdef _DEBUG

void CFile::Dump( CDumpContext &dc ) const
/****************************************/
{
    CObject::Dump( dc );

    dc << "m_hFile = " << (void *)m_hFile << "\n";
    dc << "m_bCloseOnDelete = " << m_bCloseOnDelete << "\n";
    dc << "m_strFileName = " << m_strFileName << "\n";
}

#endif // _DEBUG

BOOL PASCAL CFile::GetStatus( LPCTSTR lpszFileName, CFileStatus &rStatus )
/************************************************************************/
{
    WIN32_FIND_DATA wfd;
    HANDLE          hFileFind;
    hFileFind = ::FindFirstFile( lpszFileName, &wfd );
    if( hFileFind == INVALID_HANDLE_VALUE ) {
        return( FALSE );
    }
    ::FindClose( hFileFind );
    rStatus.m_ctime = CTime( wfd.ftCreationTime );
    rStatus.m_mtime = CTime( wfd.ftLastWriteTime );
    rStatus.m_atime = CTime( wfd.ftLastAccessTime );
    rStatus.m_size = ((ULONGLONG)wfd.nFileSizeHigh << 32) + wfd.nFileSizeLow;
    rStatus.m_attribute = 0;
    if( wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY ) {
        rStatus.m_attribute |= readOnly;
    }
    if( wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) {
        rStatus.m_attribute |= hidden;
    }
    if( wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) {
        rStatus.m_attribute |= system;
    }
    if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
        rStatus.m_attribute |= directory;
    }
    if( wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE ) {
        rStatus.m_attribute |= archive;
    }
    _tcscpy( rStatus.m_szFullName, wfd.cFileName );
    return( TRUE );
}

void PASCAL CFile::Remove( LPCTSTR lpszFileName )
/***********************************************/
{
    if( !::DeleteFile( lpszFileName ) ) {
        CFileException::ThrowOsError( ::GetLastError(), lpszFileName );
    }
}

void PASCAL CFile::Rename( LPCTSTR lpszOldName, LPCTSTR lpszNewName )
/*******************************************************************/
{
    if( !::MoveFile( lpszOldName, lpszNewName ) ) {
        CFileException::ThrowOsError( ::GetLastError(), lpszOldName );
    }
}

void PASCAL CFile::SetStatus( LPCTSTR lpszFileName, const CFileStatus &rStatus )
/******************************************************************************/
{
    SYSTEMTIME  stCreation;
    SYSTEMTIME  stLastWrite;
    SYSTEMTIME  stLastAccess;
    FILETIME    ftLocalCreation;
    FILETIME    ftLocalLastWrite;
    FILETIME    ftLocalLastAccess;
    FILETIME    ftCreation;
    FILETIME    ftLastWrite;
    FILETIME    ftLastAccess;
    DWORD       dwOldAttributes;
    DWORD       dwNewAttributes;
    HANDLE      hFile;
    dwOldAttributes = ::GetFileAttributes( lpszFileName );
    if( dwOldAttributes == INVALID_FILE_ATTRIBUTES ) {
        CFileException::ThrowOsError( ::GetLastError(), lpszFileName );
    }
    if( dwOldAttributes & FILE_ATTRIBUTE_READONLY ) {
        dwOldAttributes &= ~FILE_ATTRIBUTE_READONLY;
        if( !::SetFileAttributes( lpszFileName, dwOldAttributes ) ) {
            CFileException::ThrowOsError( ::GetLastError(), lpszFileName );
        }
    }
    hFile = ::CreateFile( lpszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
                          NULL, OPEN_EXISTING, 0L, NULL );
    if( hFile == INVALID_HANDLE_VALUE ) {
        CFileException::ThrowOsError( ::GetLastError(), lpszFileName );
    }
    rStatus.m_ctime.GetAsSystemTime( stCreation );
    rStatus.m_mtime.GetAsSystemTime( stLastWrite );
    rStatus.m_atime.GetAsSystemTime( stLastAccess );
    ::SystemTimeToFileTime( &stCreation, &ftLocalCreation );
    ::SystemTimeToFileTime( &stLastWrite, &ftLocalLastWrite );
    ::SystemTimeToFileTime( &stLastAccess, &ftLocalLastAccess );
    ::LocalFileTimeToFileTime( &ftLocalCreation, &ftCreation );
    ::LocalFileTimeToFileTime( &ftLocalLastWrite, &ftLastWrite );
    ::LocalFileTimeToFileTime( &ftLocalLastAccess, &ftLastAccess );
    if( !::SetFileTime( hFile, &ftCreation, &ftLastAccess, &ftLastWrite ) ) {
        CFileException::ThrowOsError( ::GetLastError(), lpszFileName );
    }
    if( !::CloseHandle( hFile ) ) {
        CFileException::ThrowOsError( ::GetLastError(), lpszFileName );
    }
    dwNewAttributes = 0L;
    if( rStatus.m_attribute & readOnly ) {
        dwNewAttributes |= FILE_ATTRIBUTE_READONLY;
    }
    if( rStatus.m_attribute & hidden ) {
        dwNewAttributes |= FILE_ATTRIBUTE_HIDDEN;
    }
    if( rStatus.m_attribute & system ) {
        dwNewAttributes |= FILE_ATTRIBUTE_SYSTEM;
    }
    if( rStatus.m_attribute & archive ) {
        dwNewAttributes |= FILE_ATTRIBUTE_ARCHIVE;
    }
    if( dwNewAttributes != dwOldAttributes ) {
        if( !::SetFileAttributes( lpszFileName, dwNewAttributes ) ) {
            CFileException::ThrowOsError( ::GetLastError(), lpszFileName );
        }
    }
}
