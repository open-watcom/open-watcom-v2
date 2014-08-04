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
* Description:  Implementation of CFileException and AfxThrowFileException.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CFileException, CException )

CFileException::CFileException( int cause, LONG lOsError, LPCTSTR lpszArchiveName )
/*********************************************************************************/
{
    ASSERT( cause >= none && cause <= endOfFile );
    m_cause = cause;
    m_lOsError = lOsError;
    m_strFileName = lpszArchiveName;
}

BOOL CFileException::GetErrorMessage( LPTSTR lpszError, UINT nMaxError,
                                      PUINT pnHelpContext )
/*********************************************************/
{
    if( pnHelpContext != NULL ) {
        *pnHelpContext = 0;
    }
    if( m_cause < none || m_cause > endOfFile ) {
        if( lpszError != NULL && nMaxError > 0 ) {
            lpszError[0] = _T('\0');
        }
        return( FALSE );
    }
    if( lpszError != NULL && nMaxError > 0 ) {
        CString str;
        str.FormatMessage( AFX_IDP_FILE_NONE + m_cause, (LPCTSTR)m_strFileName );

        int nLength = str.GetLength() + 1;
        if( nLength < nMaxError )
            nLength = nMaxError;
        _tcsncpy( lpszError, str, nLength );
    }
    return( TRUE );
}

#ifdef _DEBUG

void CFileException::Dump( CDumpContext &dc ) const
/*************************************************/
{
    CException::Dump( dc );

    dc << "m_cause = " << m_cause << "\n";
    dc << "m_lOsError = " << m_lOsError << "\n";
    dc << "m_strFileName = " << m_strFileName << "\n";
}

#endif // _DEBUG

int PASCAL CFileException::ErrnoToException( int nErrno )
/*******************************************************/
{
    switch( nErrno ) {
    case ENFILE:
    case ENOENT:
        return( fileNotFound );
    case EMFILE:
        return( tooManyOpenFiles );
    case EACCES:
    case EPERM:
        return( accessDenied );
    case EBADF:
        return( invalidFile );
    case EINVAL:
    case EIO:
        return( hardIO );
    case EDEADLOCK:
        return( sharingViolation );
    case ENOSPC:
        return( diskFull );
    default:
        return( genericException );
    }
}

int PASCAL CFileException::OsErrorToException( LONG lOsError )
/************************************************************/
{
    switch( lOsError ) {
    case NO_ERROR:
        return( none );
    case ERROR_DISK_CHANGE:
    case ERROR_FILE_NOT_FOUND:
    case ERROR_INVALID_HANDLE:
    case ERROR_NO_MORE_FILES:
        return( fileNotFound );
    case ERROR_ALREADY_ASSIGNED:
    case ERROR_BAD_NET_NAME:
    case ERROR_BAD_NETPATH:
    case ERROR_BAD_PATHNAME:
    case ERROR_BUFFER_OVERFLOW:
    case ERROR_DEV_NOT_EXIST:
    case ERROR_DIR_NOT_ROOT:
    case ERROR_DIRECTORY:
    case ERROR_DUP_NAME:
    case ERROR_FILENAME_EXCED_RANGE:
    case ERROR_INVALID_DRIVE:
    case ERROR_INVALID_LEVEL:
    case ERROR_INVALID_NAME:
    case ERROR_LABEL_TOO_LONG:
    case ERROR_META_EXPANSION_TOO_LONG:
    case ERROR_NO_VOLUME_LABEL:
    case ERROR_NOT_SAME_DEVICE:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_SHARING_PAUSED:
    case ERROR_WRONG_DISK:
        return( badPath );
    case ERROR_NO_MORE_SEARCH_HANDLES:
    case ERROR_SHARING_BUFFER_EXCEEDED:
    case ERROR_TOO_MANY_NAMES:
    case ERROR_TOO_MANY_OPEN_FILES:
        return( tooManyOpenFiles );
    case ERROR_ACCESS_DENIED:
    case ERROR_ALREADY_EXISTS:
    case ERROR_BAD_NET_RESP:
    case ERROR_BUSY:
    case ERROR_CANNOT_MAKE:
    case ERROR_FILE_EXISTS:
    case ERROR_INVALID_ACCESS:
    case ERROR_INVALID_PASSWORD:
    case ERROR_NETNAME_DELETED:
    case ERROR_NETWORK_ACCESS_DENIED:
    case ERROR_NETWORK_BUSY:
    case ERROR_REQ_NOT_ACCEP:
    case ERROR_SWAPERROR:
    case ERROR_WRITE_FAULT:
    case ERROR_WRITE_PROTECT:
        return( accessDenied );
    case ERROR_BAD_DEV_TYPE:
    case ERROR_BAD_EXE_FORMAT:
    case ERROR_BAD_REM_ADAP:
    case ERROR_BAD_FORMAT:
    case ERROR_INVALID_EXE_SIGNATURE:
    case ERROR_INVALID_ORDINAL:
    case ERROR_INVALID_TARGET_HANDLE:
    case ERROR_NOT_DOS_DISK:
        return( invalidFile );
    case ERROR_CURRENT_DIRECTORY:
    case ERROR_DIR_NOT_EMPTY:
        return( removeCurrentDir );
    case ERROR_NO_SPOOL_SPACE:
        return( directoryFull );
    case ERROR_BAD_LENGTH:
    case ERROR_NEGATIVE_SEEK:
    case ERROR_READ_FAULT:
    case ERROR_SECTOR_NOT_FOUND:
    case ERROR_SEEK:
    case ERROR_SEEK_ON_DEVICE:
        return( badSeek );
    case ERROR_ADAP_HDW_ERR:
    case ERROR_BAD_COMMAND:
    case ERROR_BAD_UNIT:
    case ERROR_CRC:
    case ERROR_INVALID_CATEGORY:
    case ERROR_IO_INCOMPLETE:
    case ERROR_IO_PENDING:
    case ERROR_NET_WRITE_FAULT:
    case ERROR_NOT_READY:
    case ERROR_OPERATION_ABORTED:
    case ERROR_UNEXP_NET_ERR:
        return( hardIO );
    case ERROR_SHARING_VIOLATION:
        return( sharingViolation );
    case ERROR_DRIVE_LOCKED:
    case ERROR_LOCK_FAILED:
    case ERROR_LOCK_VIOLATION:
        return( lockViolation );
    case ERROR_DISK_FULL:
    case ERROR_HANDLE_DISK_FULL:
        return( diskFull );
    case ERROR_HANDLE_EOF:
        return( endOfFile );
    default:
        return( genericException );
    }
}

void PASCAL CFileException::ThrowErrno( int nErrno, LPCTSTR lpszFileName )
/************************************************************************/
{
    throw new CFileException( ErrnoToException( nErrno ), nErrno, lpszFileName );
}

void PASCAL CFileException::ThrowOsError( LONG lOsError, LPCTSTR lpszFileName )
/*****************************************************************************/
{
    throw new CFileException( OsErrorToException( lOsError ), lOsError, lpszFileName );
}

void AFXAPI AfxThrowFileException( int cause, LONG lOsError, LPCTSTR lpszFileName )
/*********************************************************************************/
{
    throw new CFileException( cause, lOsError, lpszFileName );
}
