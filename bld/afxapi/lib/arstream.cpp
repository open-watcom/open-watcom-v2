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
* Description:  Implementation of CArchiveStream.
*
****************************************************************************/


#include "stdafx.h"

CArchiveStream::CArchiveStream( CArchive *pArchive )
/**************************************************/
{
    ASSERT( pArchive != NULL );
    m_pArchive = pArchive;
}

STDMETHODIMP CArchiveStream::QueryInterface( REFIID riid, void **ppvObject )
/**************************************************************************/
{
    if( memcmp( &riid, &IID_IUnknown, sizeof( IID ) ) != 0 &&
        memcmp( &riid, &IID_ISequentialStream, sizeof( IID ) ) != 0 &&
        memcmp( &riid, &IID_IStream, sizeof( IID ) ) != 0 ) {
        return( E_NOINTERFACE );
    }
    if( ppvObject == NULL ) {
        return( E_POINTER );
    }
    *ppvObject = this;
    return( S_OK );
}

STDMETHODIMP_( ULONG ) CArchiveStream::AddRef()
/*********************************************/
{
    // Fake a reference count.  The object needs to be freed using the delete operator.
    return( 1L );
}

STDMETHODIMP_( ULONG ) CArchiveStream::Release()
/**********************************************/
{
    // Fake a reference count.  The object needs to be freed using the delete operator.
    return( 0L );
}

STDMETHODIMP CArchiveStream::Read( void *pv, ULONG cb, ULONG *pcbRead )
/*********************************************************************/
{
    if( pv == NULL ) {
        return( E_POINTER );
    }

    ULONG cbRead;
    ASSERT( m_pArchive != NULL );
    try {
        cbRead = m_pArchive->Read( pv, cb );
    } catch( CException *pEx ) {
        return( E_UNEXPECTED );
    }
    if( pcbRead != NULL ) {
        *pcbRead = cbRead;
    }
    return( S_OK );
}

STDMETHODIMP CArchiveStream::Write( void const *pv, ULONG cb, ULONG *pcbWritten )
/*******************************************************************************/
{
    if( pv == NULL ) {
        return( E_POINTER );
    }

    ASSERT( m_pArchive != NULL );
    try {
        m_pArchive->Write( pv, cb );
    } catch( CException *pEx ) {
        return( E_UNEXPECTED );
    }
    if( pcbWritten != NULL ) {
        *pcbWritten = cb;
    }
    return( S_OK );
}

STDMETHODIMP CArchiveStream::Seek( LARGE_INTEGER dlibMove, DWORD dwOrigin,
                                   ULARGE_INTEGER *plibNewPosition )
/******************************************************************/
{
    UINT nFrom;
    switch( dwOrigin ) {
    case STREAM_SEEK_SET:
        nFrom = CFile::begin;
        break;
    case STREAM_SEEK_CUR:
        nFrom = CFile::current;
        break;
    case STREAM_SEEK_END:
        nFrom = CFile::end;
        break;
    default:
        return( E_INVALIDARG );
    }
    
    ASSERT( m_pArchive != NULL );
    CFile *pFile = m_pArchive->GetFile();
    ASSERT( pFile != NULL );
    ULONGLONG ullNewPosition;
    try {
        ullNewPosition = pFile->Seek( dlibMove.QuadPart, nFrom );
    } catch( CException *pEx ) {
        return( E_UNEXPECTED );
    }
    if( plibNewPosition != NULL ) {
        plibNewPosition->QuadPart = ullNewPosition;
    }
    return( S_OK );
}

STDMETHODIMP CArchiveStream::SetSize( ULARGE_INTEGER libNewSize )
/***************************************************************/
{
    UNUSED_ALWAYS( libNewSize );
    return( E_NOTIMPL );
}

STDMETHODIMP CArchiveStream::CopyTo( IStream *pstm, ULARGE_INTEGER cb,
                                     ULARGE_INTEGER *pcbRead,
                                     ULARGE_INTEGER *pcbWritten )
/***************************************************************/
{
    UNUSED_ALWAYS( pstm );
    UNUSED_ALWAYS( cb );
    UNUSED_ALWAYS( pcbRead );
    UNUSED_ALWAYS( pcbWritten );
    return( E_NOTIMPL );
}

STDMETHODIMP CArchiveStream::Commit( DWORD grfCommitFlags )
/*********************************************************/
{
    UNUSED_ALWAYS( grfCommitFlags );
    return( E_NOTIMPL );
}

STDMETHODIMP CArchiveStream::Revert()
/***********************************/
{
    return( E_NOTIMPL );
}

STDMETHODIMP CArchiveStream::LockRegion( ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                                         DWORD dwLockType )
/*********************************************************/
{
    UNUSED_ALWAYS( libOffset );
    UNUSED_ALWAYS( cb );
    UNUSED_ALWAYS( dwLockType );
    return( E_NOTIMPL );
}

STDMETHODIMP CArchiveStream::UnlockRegion( ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
                                           DWORD dwLockType )
/***********************************************************/
{
    UNUSED_ALWAYS( libOffset );
    UNUSED_ALWAYS( cb );
    UNUSED_ALWAYS( dwLockType );
    return( E_NOTIMPL );
}

STDMETHODIMP CArchiveStream::Stat( STATSTG *pstatstg, DWORD grfStatFlag )
/***********************************************************************/
{
    UNUSED_ALWAYS( pstatstg );
    UNUSED_ALWAYS( grfStatFlag );
    return( E_NOTIMPL );
}

STDMETHODIMP CArchiveStream::Clone( IStream **ppstm )
/***************************************************/
{
    UNUSED_ALWAYS( ppstm );
    return( E_NOTIMPL );
}
