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
* Description:  Implementation of CString.
*
****************************************************************************/


#include "stdafx.h"
#include <afxtempl.h>

void CString::Grow()
/******************/
{
    if( m_nDataLength >= m_nAllocLength ) {
        int nOldAllocLength = m_nAllocLength;
        m_nAllocLength = m_nDataLength + 1;
        PTSTR pszNew = new TCHAR[m_nAllocLength];
        memcpy( pszNew, m_psz, nOldAllocLength * sizeof( TCHAR ) );
        delete [] m_psz;
        m_psz = pszNew;
    }
}
 
CString::CString()
/****************/
{
    m_nDataLength = 0;
    m_nAllocLength = 1;
    m_psz = new TCHAR[1];
    m_psz[0] = _T('\0');
}

CString::CString( const CString &strSrc )
/***************************************/
{
    m_nDataLength = strSrc.m_nDataLength;
    m_nAllocLength = m_nDataLength + 1;
    m_psz = new TCHAR[m_nAllocLength];
    _tcscpy( m_psz, strSrc.m_psz );
}

CString::CString( const char *pszSrc )
/*************************************/
{
    if( pszSrc == NULL ) {
        m_nDataLength = 0;
        m_nAllocLength = 1;
        m_psz = new TCHAR[1];
        m_psz[0] = _T('\0');
    } else {
        m_nDataLength = strlen( pszSrc );
        m_nAllocLength = m_nDataLength + 1;
        m_psz = new TCHAR[m_nAllocLength];
#ifdef _UNICODE
        ::MultiByteToWideChar( CP_ACP, 0L, pszSrc, -1, m_psz, m_nDataLength );
#else
        strcpy( m_psz, pszSrc );
#endif
        m_psz[m_nAllocLength - 1] = _T('\0');
    }
}

CString::CString( const wchar_t *pszSrc )
/***************************************/
{
    if( pszSrc == NULL ) {
        m_nDataLength = 0;
        m_nAllocLength = 1;
        m_psz = new TCHAR[1];
        m_psz[0] = _T('\0');
    } else {
        m_nDataLength = wcslen( pszSrc );
        m_nAllocLength = m_nDataLength + 1;
        m_psz = new TCHAR[m_nAllocLength];
#ifndef _UNICODE
        ::WideCharToMultiByte( CP_ACP, 0L, pszSrc, -1, m_psz, m_nDataLength,
                               NULL, NULL );
#else
        wcscpy( m_psz, pszSrc );
#endif
        m_psz[m_nAllocLength - 1] = _T('\0');
    }
}

CString::CString( char ch, int nLength )
/**************************************/
{
    ASSERT( nLength >= 0 );
    m_nDataLength = nLength;
    m_nAllocLength = m_nDataLength + 1;
    m_psz = new TCHAR[m_nAllocLength];
    if( m_nDataLength > 0 ) {
#ifdef _UNICODE
        ::MultiByteToWideChar( CP_ACP, 0L, &ch, 1, m_psz, 1 );
#else
        m_psz[0] = ch;
#endif
        for( int i = 1; i < m_nDataLength; i++ ) {
            m_psz[i] = m_psz[0];
        }
    }
    m_psz[m_nAllocLength - 1] = _T('\0');
}

CString::CString( wchar_t ch, int nLength )
/*****************************************/
{
    ASSERT( nLength >= 0 );
    m_nDataLength = nLength;
    m_nAllocLength = m_nDataLength + 1;
    m_psz = new TCHAR[m_nAllocLength];
    if( m_nDataLength > 0 ) {
#ifndef _UNICODE
        ::WideCharToMultiByte( CP_ACP, 0L, &ch, 1, m_psz, 1, NULL, NULL );
#else
        m_psz[0] = ch;
#endif
        for( int i = 1; i < m_nDataLength; i++ ) {
            m_psz[i] = m_psz[0];
        }
    }
    m_psz[m_nAllocLength - 1] = _T('\0');
}

CString::CString( const char *pch, int nLength )
/**********************************************/
{
    ASSERT( pch != NULL );
    m_nDataLength = nLength;
    m_nAllocLength = m_nDataLength + 1;
    m_psz = new TCHAR[m_nAllocLength];
#ifdef _UNICODE
    ::MultiByteToWideChar( CP_ACP, 0L, pch, nLength, m_psz, nLength );
#else
    strncpy( m_psz, pch, nLength );
#endif
    m_psz[m_nAllocLength - 1] = _T('\0');
}

CString::CString( const wchar_t *pch, int nLength )
/*************************************************/
{
    ASSERT( pch != NULL );
    m_nDataLength = nLength;
    m_nAllocLength = m_nDataLength + 1;
    m_psz = new TCHAR[m_nAllocLength];
#ifndef _UNICODE
    ::WideCharToMultiByte( CP_ACP, 0L, pch, nLength, m_psz, nLength, NULL, NULL );
#else
    wcsncpy( m_psz, pch, nLength );
#endif
    m_psz[m_nAllocLength - 1] = _T('\0');
}

CString::~CString()
/*****************/
{
    delete [] m_psz;
}

BSTR CString::AllocSysString() const
/**********************************/
{
#ifndef _UNICODE
    wchar_t *pszBuff = new wchar_t[m_nDataLength + 1];
    ::MultiByteToWideChar( CP_ACP, 0L, m_psz, -1, pszBuff, m_nDataLength );
    pszBuff[m_nDataLength] = L'\0';
    BSTR bstrResult = ::SysAllocString( pszBuff );
    delete [] pszBuff;
    return( bstrResult );
#else
    return( ::SysAllocString( m_psz ) );
#endif
}

void CString::Append( const CString &strSrc )
/*******************************************/
{
    int nOldLength = m_nDataLength;
    m_nDataLength += strSrc.m_nDataLength;
    Grow();
    _tcscpy( m_psz + nOldLength, strSrc.m_psz );
}

void CString::Append( PCTSTR pszSrc, int nLength )
/************************************************/
{
    ASSERT( pszSrc != NULL );
    int nOldLength = m_nDataLength;
    m_nDataLength += nLength;
    Grow();
    _tcsncpy( m_psz + nOldLength, pszSrc, nLength );
}

void CString::Append( PCTSTR pszSrc )
/***********************************/
{
    if( pszSrc != NULL ) {
        int nOldLength = m_nDataLength;
        m_nDataLength += _tcslen( pszSrc );
        Grow();
        _tcscpy( m_psz + nOldLength, pszSrc );
    }
}

void CString::AppendChar( TCHAR ch )
/**********************************/
{
    m_nDataLength++;
    Grow();
    m_psz[m_nDataLength - 1] = ch;
    m_psz[m_nDataLength] = _T('\0');
}

int CString::Find( PCTSTR pszSub, int iStart ) const
/**************************************************/
{
    ASSERT( pszSub != NULL );
    if( iStart < 0 || iStart > m_nDataLength ) {
        return( -1 );
    }
    TCHAR *p = _tcsstr( m_psz + iStart, pszSub );
    if( p != NULL ) {
        return( p - m_psz );
    } else {
        return( -1 );
    }
}

int CString::Find( TCHAR ch, int iStart ) const
/*********************************************/
{
    if( iStart < 0 || iStart > m_nDataLength ) {
        return( -1 );
    }
    TCHAR *p = _tcschr( m_psz + iStart, ch );
    if( p != NULL ) {
        return( p - m_psz );
    } else {
        return( -1 );
    }
}

void CString::FormatMessageV( PCTSTR pszFormat, va_list *pArgList )
/*****************************************************************/
{
    TCHAR   szBuff[1024];
    ::FormatMessage( FORMAT_MESSAGE_FROM_STRING, pszFormat, 0L, 0L, szBuff, 1024,
                     pArgList );
    SetString( szBuff );
}

void CString::FormatV( PCTSTR pszFormat, va_list args )
/*****************************************************/
{
    TCHAR   szBuff[1024];
    _vstprintf( szBuff, pszFormat, args );
    SetString( szBuff );
}

void CString::FreeExtra()
/***********************/
{
    if( m_nAllocLength > m_nDataLength + 1 ) {
        m_nAllocLength = m_nDataLength + 1;
        PTSTR pszNew = new TCHAR[m_nAllocLength];
        _tcscpy( pszNew, m_psz );
        delete [] m_psz;
        m_psz = pszNew;
    }
}

TCHAR CString::GetAt( int iChar ) const
/*************************************/
{
    if( iChar >= 0 && iChar < m_nDataLength ) {
        return( m_psz[iChar] );
    } else {
        return( _T('\0') );
    }
}

PTSTR CString::GetBuffer( int nMinBufferLength )
/**********************************************/
{
    if( m_nAllocLength < nMinBufferLength ) {
        m_nAllocLength = nMinBufferLength;
        delete [] m_psz;
        m_psz = new TCHAR[m_nAllocLength];
    }
    m_nDataLength = 0;
    return( m_psz );
}

CString CString::Left( int nCount ) const
/***************************************/
{
    if( nCount <= 0 ) {
        return( CString() );
    } else if( nCount >= m_nDataLength ) {
        return( CString( m_psz ) );
    } else {
        return( CString( m_psz, nCount ) );
    }
}

BOOL CString::LoadString( HINSTANCE hInstance, UINT nID )
/*******************************************************/
{
    LPTSTR lpBuffer = GetBuffer( 1024 );
    int nLength = ::LoadString( hInstance, nID, lpBuffer, 1024 );
    ReleaseBuffer( nLength );
    return( nLength != 0 );
}

BOOL CString::LoadString( UINT nID )
/**********************************/
{
    HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nID ), RT_STRING );
    return( LoadString( hInstance, nID ) );
}
    
CString CString::Mid( int iStart, int iCount ) const
/**************************************************/
{
    if( iStart >= 0 && iStart < m_nDataLength && iCount > 0 ) {
        if( iCount < m_nDataLength - iStart ) {
            return( CString( m_psz + iStart, iCount ) );
        } else {
            return( CString( m_psz + iStart ) );
        }
    } else {
        return( CString() );
    }
}

CString CString::Mid( int iStart ) const
/**************************************/
{
    if( iStart >= 0 && iStart < m_nDataLength ) {
        return( CString( m_psz + iStart ) );
    } else {
        return( CString() );
    }
}

void CString::Preallocate( int nLength )
/**************************************/
{
    if( m_nAllocLength != nLength ) {
        m_nAllocLength = nLength;
        delete [] m_psz;
        m_psz = new TCHAR[m_nAllocLength];
    }
    m_nDataLength = 0;
}

void CString::ReleaseBuffer( int nNewLength )
/*******************************************/
{
    if( nNewLength >= 0 ) {
        if( nNewLength < m_nAllocLength ) {
            m_nDataLength = nNewLength;
        }
    } else {
        m_nDataLength = _tcslen( m_psz );
    }
}

void CString::ReleaseBufferSetLength( int nNewLength )
/****************************************************/
{
    if( nNewLength < m_nAllocLength ) {
        m_nDataLength = nNewLength;
    }
}

int CString::ReverseFind( TCHAR ch ) const
/****************************************/
{
    TCHAR *p = _tcsrchr( m_psz, ch );
    if( p != NULL ) {
        return( p - m_psz );
    } else {
        return( -1 );
    }
}

CString CString::Right( int nCount ) const
/****************************************/
{
    if( nCount >= 0 && nCount < m_nDataLength ) {
        return( CString( m_psz + m_nDataLength - nCount ) );
    } else {
        return( CString( m_psz ) );
    }
}

void CString::SetAt( int iChar, TCHAR ch )
/****************************************/
{
    if( iChar >= 0 && iChar < m_nDataLength ) {
        m_psz[iChar] = ch;
    }
}

void CString::SetString( PCTSTR pszSrc, int nLength )
/***************************************************/
{
    ASSERT( pszSrc != NULL );
    m_nDataLength = nLength;
    Grow();
    _tcsncpy( m_psz, pszSrc, nLength );
    m_psz[m_nDataLength] = _T('\0');
}

void CString::SetString( PCTSTR pszSrc )
/**************************************/
{
    if( pszSrc == NULL ) {
        Empty();
    } else {
        m_nDataLength = _tcslen( pszSrc );
        Grow();
        _tcscpy( m_psz, pszSrc );
    }
}

void CString::Truncate( int nNewLength )
/**************************************/
{
    if( nNewLength < m_nDataLength ) {
        m_nDataLength = nNewLength;
        m_psz[m_nDataLength] = _T('\0');
    }
}

CString &CString::operator+=( PCSTR pszSrc )
/******************************************/
{
    if( pszSrc != NULL ) {
#ifdef _UNICODE
        int     nLength = strlen( pszSrc );
        wchar_t *pszBuff = new wchar_t[nLength + 1];
        ::MultiByteToWideChar( CP_ACP, 0L, pszSrc, -1, pszBuff, nLength + 1 );
        Append( pszBuff );
        delete [] pszBuff;
#else
        Append( pszSrc );
#endif
    }
    return( *this );
}

CString &CString::operator+=( PCWSTR pszSrc )
/*******************************************/
{
    if( pszSrc != NULL ) {
#ifndef _UNICODE
        int     nLength = wcslen( pszSrc );
        char    *pszBuff = new char[nLength + 1];
        ::WideCharToMultiByte( CP_ACP, 0L, pszSrc, -1, pszBuff, nLength + 1, NULL, NULL );
        Append( pszBuff );
        delete [] pszBuff;
#else
        Append( pszSrc );
#endif
    }
    return( *this );
}

CString &CString::operator+=( char ch )
/*************************************/
{
#ifdef _UNICODE
    wchar_t ch2;
    ::MultiByteToWideChar( CP_ACP, 0L, &ch, 1, &ch2, 1 );
    AppendChar( ch2 );
#else
    AppendChar( ch );
#endif
    return( *this );
}

CString &CString::operator+=( wchar_t ch )
/****************************************/
{
#ifndef _UNICODE
    char ch2;
    ::WideCharToMultiByte( CP_ACP, 0L, &ch, 1, &ch2, 1, NULL, NULL );
    AppendChar( ch2 );
#else
    AppendChar( ch );
#endif
    return( *this );
}

CString &CString::operator=( PCSTR pszSrc )
/*****************************************/
{
    if( pszSrc == NULL ) {
        Empty();
    } else {
#ifdef _UNICODE
        int     nLength = strlen( pszSrc );
        wchar_t *pszBuff = new wchar_t[nLength + 1];
        ::MultiByteToWideChar( CP_ACP, 0L, pszSrc, -1, pszBuff, nLength + 1 );
        SetString( pszBuff );
        delete [] pszBuff;
#else
        SetString( pszSrc );
#endif
    }
    return( *this );
}

CString &CString::operator=( PCWSTR pszSrc )
/******************************************/
{
    if( pszSrc == NULL ) {
        Empty();
    } else {
#ifndef _UNICODE
        int     nLength = wcslen( pszSrc );
        char    *pszBuff = new char[nLength + 1];
        ::WideCharToMultiByte( CP_ACP, 0L, pszSrc, -1, pszBuff, nLength + 1, NULL, NULL );
        SetString( pszBuff );
        delete [] pszBuff;
#else
        SetString( pszSrc );
#endif
    }
    return( *this );
}

CString &CString::operator=( char ch )
/************************************/
{
    TCHAR szSrc[2];
#ifdef _UNICODE
    ::MultiByteToWideChar( CP_ACP, 0L, &ch, 1, szSrc, 1 );
#else
    szSrc[0] = ch;
#endif
    szSrc[1] = _T('\0');
    SetString( szSrc );
    return( *this );
}

CString &CString::operator=( wchar_t ch )
/***************************************/
{
    TCHAR szSrc[2];
#ifndef _UNICODE
    ::WideCharToMultiByte( CP_ACP, 0L, &ch, 1, szSrc, 1, NULL, NULL );
#else
    szSrc[0] = ch;
#endif
    szSrc[1] = _T('\0');
    SetString( szSrc );
    return( *this );
}

void __cdecl CString::AppendFormat( UINT nFormatID, ... )
/*******************************************************/
{
    va_list     args;
    TCHAR       szFormat[1024];
    TCHAR       szBuff[1024];
    HINSTANCE   hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nFormatID ), RT_STRING );
    va_start( args, nFormatID );
    ::LoadString( hInstance, nFormatID, szFormat, 1024 );
    _vstprintf( szBuff, szFormat, args );
    Append( szBuff );
    va_end( args );
}

void __cdecl CString::AppendFormat( PCTSTR pszFormat, ... )
/*********************************************************/
{
    va_list args;
    TCHAR   szBuff[1024];
    va_start( args, pszFormat );
    _vstprintf( szBuff, pszFormat, args );
    Append( szBuff );
    va_end( args );
}

void __cdecl CString::Format( UINT nFormatID, ... )
/*************************************************/
{
    va_list     args;
    TCHAR       szFormat[1024];
    HINSTANCE   hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nFormatID ), RT_STRING );
    va_start( args, nFormatID );
    ::LoadString( hInstance, nFormatID, szFormat, 1024 );
    FormatV( szFormat, args );
    va_end( args );
}

void __cdecl CString::Format( PCTSTR pszFormat, ... )
/***************************************************/
{
    va_list args;
    va_start( args, pszFormat );
    FormatV( pszFormat, args );
    va_end( args );
}

void __cdecl CString::FormatMessage( UINT nFormatID, ... )
/********************************************************/
{
    va_list     args;
    TCHAR       szFormat[1024];
    HINSTANCE   hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nFormatID ), RT_STRING );
    va_start( args, nFormatID );
    ::LoadString( hInstance, nFormatID, szFormat, 1024 );
    FormatMessageV( szFormat, &args );
    va_end( args );
}

void __cdecl CString::FormatMessage( PCTSTR pszFormat, ... )
/**********************************************************/
{
    va_list args;
    va_start( args, pszFormat );
    FormatMessageV( pszFormat, &args );
    va_end( args );
}

template<>
UINT AFXAPI HashKey( LPCTSTR key )
/********************************/
{
    if( key == NULL ) {
        return( 0 );
    }

    UINT nHashKey = 0;
    while( *key != _T('\0') ) {
        nHashKey = (nHashKey << 5) + nHashKey + *key;
        key++;
    }
    return( nHashKey );
}

template<>
void AFXAPI SerializeElements( CArchive &ar, CString *pElements, INT_PTR nCount )
/*******************************************************************************/
{
    ASSERT( pElements != NULL );
    ASSERT( nCount >= 0 );
    for( int i = 0; i < nCount; i++ ) {
        if( ar.IsStoring() ) {
            ar << pElements[i];
        } else {
            ar >> pElements[i];
        }
    }
}
