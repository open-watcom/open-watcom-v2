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
* Description:  Implementation of CStdioFile.
*
****************************************************************************/


#include "stdafx.h"
#include <io.h>
#include <fcntl.h>

IMPLEMENT_DYNAMIC( CStdioFile, CFile )

CStdioFile::CStdioFile()
/**********************/
{
    m_pStream = NULL;
}

CStdioFile::CStdioFile( FILE *pOpenStream )
/*****************************************/
{
    m_pStream = pOpenStream;
}

CStdioFile::CStdioFile( LPCTSTR lpszFileName, UINT nOpenFlags )
        : CFile( lpszFileName, nOpenFlags )
/*****************************************/
{
}

LPTSTR CStdioFile::ReadString( LPTSTR lpsz, UINT nMax )
/*****************************************************/
{
    if( _fgetts( lpsz, nMax, m_pStream ) == NULL && !feof( m_pStream ) ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    return( lpsz );
}

BOOL CStdioFile::ReadString( CString &rString )
/*********************************************/
{
    TCHAR   szBuff[128];
    TCHAR   *pchNL = NULL;
    rString.Empty();
    if( feof( m_pStream ) ) {
        return( FALSE );
    }
    while( pchNL == NULL && !feof( m_pStream ) ) {
        if( _fgetts( szBuff, 128, m_pStream ) == NULL && !feof( m_pStream ) ) {
            CFileException::ThrowErrno( errno, m_strFileName );
        }
        pchNL = _tcschr( szBuff, _T('\n') );
        if( pchNL != NULL ) {
            *pchNL = _T('\0');
        }
        rString += szBuff;
    }
    return( TRUE );
}

void CStdioFile::WriteString( LPCTSTR lpsz )
/******************************************/
{
    if( _fputts( lpsz, m_pStream ) == _TEOF ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
}

void CStdioFile::Abort()
/**********************/
{
    fclose( m_pStream );
    m_hFile = INVALID_HANDLE_VALUE;
    m_bCloseOnDelete = FALSE;
    m_pStream = NULL;
}

void CStdioFile::Close()
/**********************/
{
    int nRet = 0;
    if( m_pStream != NULL ) {
        nRet = fclose( m_pStream );
    }
    m_hFile = INVALID_HANDLE_VALUE;
    m_bCloseOnDelete = FALSE;
    m_pStream = NULL;
    if( nRet != 0 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
}

CFile *CStdioFile::Duplicate() const
/**********************************/
{
    throw new CNotSupportedException;
}

void CStdioFile::Flush()
/**********************/
{
    if( fflush( m_pStream ) != 0 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
}

ULONGLONG CStdioFile::GetLength() const
/*************************************/
{
    long    lOldPos;
    long    lSize;
    if( (lOldPos = ftell( m_pStream )) == -1 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    if( fseek( m_pStream, 0, SEEK_END ) != 0 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    if( (lSize = ftell( m_pStream )) == -1 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    if( fseek( m_pStream, lOldPos, SEEK_SET ) != 0 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    return( lSize );
}

ULONGLONG CStdioFile::GetPosition() const
/***************************************/
{
    long    lPos;
    if( (lPos = ftell( m_pStream )) == -1 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    return( lPos );
}

void CStdioFile::LockRange( ULONGLONG dwPos, ULONGLONG dwCount )
/**************************************************************/
{
    UNUSED_ALWAYS( dwPos );
    UNUSED_ALWAYS( dwCount );
    throw new CNotSupportedException;
}

BOOL CStdioFile::Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException *pError )
/************************************************************************************/
{
    if( !CFile::Open( lpszFileName, nOpenFlags, pError ) ) {
        return( FALSE );
    }

    int nHandle;
    int nAccess;
    switch( nOpenFlags & (modeRead | modeWrite | modeReadWrite) ) {
    case modeRead:
        nAccess = O_RDONLY;
        break;
    case modeWrite:
        nAccess = O_WRONLY;
        break;
    case modeReadWrite:
        nAccess = O_RDWR;
        break;
    }
    if( (nHandle = _open_osfhandle( (long)m_hFile, nAccess )) == -1 ) {
        pError->m_cause = CFileException::ErrnoToException( errno );
        pError->m_lOsError = (LONG)errno;
        pError->m_strFileName = lpszFileName;
        return( FALSE );
    }

    switch( nOpenFlags & (modeRead | modeWrite | modeReadWrite) ) {
    case modeRead:
        m_pStream = _fdopen( nHandle, "r" );
        break;
    case modeWrite:
        if( (nOpenFlags & modeCreate) && !(nOpenFlags & modeNoTruncate) ) {
            m_pStream = _fdopen( nHandle, "w" );
        } else {
            m_pStream = _fdopen( nHandle, "a" );
        }
        break;
    case modeReadWrite:
        m_pStream = _fdopen( nHandle, "r+" );
        break;
    }
    if( m_pStream == NULL ) {
        if( pError != NULL ) {
            pError->m_cause = CFileException::ErrnoToException( errno );
            pError->m_lOsError = (LONG)errno;
            pError->m_strFileName = lpszFileName;
        }
        return( FALSE );
    }
    return( TRUE );        
}

UINT CStdioFile::Read( void *lpBuf, UINT nCount )
/***********************************************/
{
    int nRet = fread( lpBuf, 1, nCount, m_pStream );
    if( ferror( m_pStream ) ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    return( nRet );
}

ULONGLONG CStdioFile::Seek( LONGLONG lOff, UINT nFrom )
/*****************************************************/
{
    int where;
    switch( nFrom ) {
    case begin:
        where = SEEK_SET;
        break;
    case end:
        where = SEEK_END;
        break;
    default:
        where = SEEK_CUR;
        break;
    }
    if( fseek( m_pStream, (long)lOff, where ) != 0 ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
    return( lOff );
}

void CStdioFile::UnlockRange( ULONGLONG dwPos, ULONGLONG dwCount )
/****************************************************************/
{
    UNUSED_ALWAYS( dwPos );
    UNUSED_ALWAYS( dwCount );
    throw new CNotSupportedException;
}

void CStdioFile::Write( const void *lpBuf, UINT nCount )
/******************************************************/
{
    fwrite( lpBuf, 1, nCount, m_pStream );
    if( ferror( m_pStream ) ) {
        CFileException::ThrowErrno( errno, m_strFileName );
    }
}

#ifdef _DEBUG

void CStdioFile::Dump( CDumpContext &dc ) const
/*********************************************/
{
    CFile::Dump( dc );

    dc << "m_pStream = " << m_pStream << "\n";
}

#endif // _DEBUG
