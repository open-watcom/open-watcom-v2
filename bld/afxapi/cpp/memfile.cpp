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
* Description:  Implementation of CMemFile.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CMemFile, CFile )

CMemFile::CMemFile( UINT nGrowBytes )
/***********************************/
{
    m_lpBuffer = NULL;
    m_nBufferSize = 0;
    m_nFileSize = 0;
    m_nGrowBytes = nGrowBytes;
    m_nPosition = 0;
}

CMemFile::CMemFile( BYTE *lpBuffer, UINT nBufferSize, UINT nGrowBytes )
/*********************************************************************/
{
    Attach( lpBuffer, nBufferSize, nGrowBytes );
}

void CMemFile::Attach( BYTE *lpBuffer, UINT nBufferSize, UINT nGrowBytes )
/************************************************************************/
{
    m_lpBuffer = lpBuffer;
    m_nBufferSize = nBufferSize;
    if( nGrowBytes > 0 ) {
        m_nFileSize = 0;
    } else {
        m_nFileSize = nBufferSize;
    }
    m_nGrowBytes = nGrowBytes;
    m_nPosition = 0;
}

BYTE *CMemFile::Detach()
/**********************/
{
    BYTE *lpBuffer = m_lpBuffer;
    m_lpBuffer = NULL;
    m_nBufferSize = 0;
    m_nFileSize = 0;
    m_nPosition = 0;
    return( lpBuffer );
}

AFX_INLINE void CMemFile::Abort()
/*******************************/
{
    Close();
}

void CMemFile::Close()
/********************/
{
    if( m_lpBuffer != NULL ) {
        Free( m_lpBuffer );
    }
    m_lpBuffer = NULL;
    m_nBufferSize = 0;
    m_nFileSize = 0;
    m_nGrowBytes = 0;
    m_nPosition = 0;
}

CFile *CMemFile::Duplicate() const
/********************************/
{
    throw new CNotSupportedException;
}

void CMemFile::Flush()
/********************/
{
}

ULONGLONG CMemFile::GetLength() const
/***********************************/
{
    return( m_nFileSize );
}

ULONGLONG CMemFile::GetPosition() const
/*************************************/
{
    return( m_nPosition );
}

void CMemFile::LockRange( ULONGLONG dwPos, ULONGLONG dwCount )
/************************************************************/
{
    UNUSED_ALWAYS( dwPos );
    UNUSED_ALWAYS( dwCount );
    throw new CNotSupportedException;
}

UINT CMemFile::Read( void *lpBuf, UINT nCount )
/*********************************************/
{
    ASSERT( m_nPosition <= m_nFileSize );
    if( nCount == 0 || m_nPosition == m_nFileSize ) {
        return( 0 );
    } 
    if( m_nPosition + nCount >= m_nFileSize ) {
        nCount = m_nFileSize - m_nPosition;
        Memcpy( (BYTE *)lpBuf, m_lpBuffer + m_nPosition, nCount );
    } else {
        Memcpy( (BYTE *)lpBuf, m_lpBuffer + m_nPosition, nCount );
    }
    m_nPosition += nCount;
    return( nCount );
}

ULONGLONG CMemFile::Seek( LONGLONG lOff, UINT nFrom )
/***************************************************/
{
    switch( nFrom ) {
    case begin:
        if( lOff < 0 ) {
            throw new CFileException( CFileException::badSeek );
        }
        m_nPosition = (SIZE_T)lOff;
        break;
    case end:
        if( lOff > 0 ) {
            throw new CFileException( CFileException::badSeek );
        }
        m_nPosition = m_nFileSize - (SIZE_T)lOff;
        break;
    default:
        if( m_nPosition + lOff < 0 ) {
            throw new CFileException( CFileException::badSeek );
        }
        m_nPosition += (SIZE_T)lOff;
        break;
    }
    GrowFile( m_nPosition );
    return( m_nPosition );
}

void CMemFile::SetLength( ULONGLONG dwNewLen )
/********************************************/
{
    GrowFile( (SIZE_T)dwNewLen );
    m_nFileSize = (SIZE_T)dwNewLen;
}

void CMemFile::UnlockRange( ULONGLONG dwPos, ULONGLONG dwCount )
/**************************************************************/
{
    UNUSED_ALWAYS( dwPos );
    UNUSED_ALWAYS( dwCount );
    throw new CNotSupportedException;
}

void CMemFile::Write( const void *lpBuf, UINT nCount )
/****************************************************/
{
    if( m_nPosition + nCount >= m_nFileSize ) {
        m_nFileSize = m_nPosition + nCount;
        GrowFile( m_nFileSize );
    }
    Memcpy( m_lpBuffer + m_nPosition, (const BYTE *)lpBuf, nCount );
    m_nPosition += nCount;
}

#ifdef _DEBUG

void CMemFile::Dump( CDumpContext &dc ) const
/*******************************************/
{
    CFile::Dump( dc );

    dc << "m_lpBuffer = " << m_lpBuffer << "\n";
    dc << "m_nBufferSize = " << m_nBufferSize << "\n";
    dc << "m_nFileSize = " << m_nFileSize << "\n";
    dc << "m_nGrowBytes = " << m_nGrowBytes << "\n";
    dc << "m_nPosition = " << m_nPosition << "\n";
}

#endif // _DEBUG

BYTE *CMemFile::Alloc( SIZE_T nBytes )
/************************************/
{
    return( (BYTE *)malloc( nBytes ) );
}

void CMemFile::Free( BYTE *lpMem )
/********************************/
{
    free( lpMem );
}

void CMemFile::GrowFile( SIZE_T dwNewLen )
/****************************************/
{
    if( m_nBufferSize < dwNewLen ) {
        if( m_nGrowBytes == 0 ) {
            throw new CMemoryException;
        }
        while( m_nBufferSize < dwNewLen ) {
            m_nBufferSize += m_nGrowBytes;
        }
        if( m_lpBuffer == NULL ) {
            m_lpBuffer = Alloc( m_nBufferSize );
        } else {
            m_lpBuffer = Realloc( m_lpBuffer, m_nBufferSize );
        }
        if( m_lpBuffer == NULL ) {
            throw new CMemoryException;
        }
    }
}

BYTE *CMemFile::Memcpy( BYTE *lpMemTarget, const BYTE *lpMemSource, SIZE_T nBytes )
/*********************************************************************************/
{
    memcpy( lpMemTarget, lpMemSource, nBytes );
    return( lpMemTarget );
}

BYTE *CMemFile::Realloc( BYTE *lpMem, SIZE_T nBytes )
/***************************************************/
{
    return( (BYTE *)realloc( lpMem, nBytes ) );
}
