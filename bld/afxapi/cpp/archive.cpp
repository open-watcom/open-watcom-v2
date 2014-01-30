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
* Description:  Implementation of CArchive.
*
****************************************************************************/


#include "stdafx.h"

// Tags used for CObject serialization
#define wNullTag        ((WORD)0x0000)
#define wNewClassTag    ((WORD)0xFFFF)
#define wOldClassTag    ((WORD)0x8000)

CArchive::CArchive( CFile *pFile, UINT nMode, int nBufSize, void *lpBuf )
/***********************************************************************/
{
    m_pFile = pFile;
    m_nMode = nMode;
    m_nBufSize = nBufSize;
    if( lpBuf != NULL ) {
        m_lpBufStart = (BYTE *)lpBuf;
        m_bUserBuf = TRUE;
    } else {
        m_lpBufStart = new BYTE[nBufSize];
        m_bUserBuf = FALSE;
    }
    m_lpBufMax = m_lpBufStart + m_nBufSize;
    if( (nMode & (load | store)) == load ) {
        m_lpBufCur = m_lpBufMax;
        m_pLoadArray = new CPtrArray;
        m_pLoadArray->Add( NULL );
    } else {
        m_lpBufCur = m_lpBufStart;
        m_pStoreMap = new CMapPtrToPtr;
        m_nMapCount = 0;
    }
}

CArchive::~CArchive()
/*******************/
{
    Close();
    if( (m_nMode & (load | store)) == load ) {
        delete m_pLoadArray;
    } else {
        delete m_pStoreMap;
    }
}

void CArchive::Abort()
/********************/
{
    if( m_pFile != NULL ) {
        if( !m_bUserBuf ) {
            delete [] m_lpBufStart;
        }
        m_pFile = NULL;
        m_nBufSize = 0;
        m_lpBufStart = NULL;
        m_lpBufCur = NULL;
        m_lpBufMax = NULL;
    }
}

void CArchive::Close()
/********************/
{
    if( m_pFile != NULL ) {
        Flush();
        if( !m_bUserBuf ) {
            delete [] m_lpBufStart;
        }
        m_pFile = NULL;
        m_nBufSize = 0;
        m_lpBufStart = NULL;
        m_lpBufCur = NULL;
        m_lpBufMax = NULL;
    }
}

void CArchive::Flush()
/********************/
{
    if( m_lpBufCur > m_lpBufStart && (m_nMode & (load | store)) == store ) {
        m_pFile->Write( m_lpBufStart, m_lpBufCur - m_lpBufStart );
        m_lpBufCur = m_lpBufStart;
    }
}

void CArchive::MapObject( const CObject *pOb )
/********************************************/
{
    if( (m_nMode & (store | load)) == load ) {
        m_pLoadArray->Add( (void *)pOb );
    } else {
        m_nMapCount++;
        m_pStoreMap->SetAt( (void *)pOb, (void *)m_nMapCount );
    }
}

UINT CArchive::Read( void *lpBuf, UINT nMax )
/*******************************************/
{
    BYTE    *lpbDst = (BYTE *)lpBuf;
    int     nLeft = nMax;
    int     nRead;
    while( m_lpBufMax - m_lpBufCur < nLeft ) {
        if( m_lpBufCur != m_lpBufMax ) {
            memcpy( lpbDst, m_lpBufCur, m_lpBufMax - m_lpBufCur );
        }
        nRead = m_pFile->Read( m_lpBufStart, m_nBufSize );
        nLeft -= m_lpBufMax - m_lpBufCur;
        lpbDst += m_lpBufMax - m_lpBufCur;
        m_lpBufCur = m_lpBufStart;
        if( nRead < m_nBufSize ) {
            m_lpBufMax = m_lpBufStart + nRead;
        }
    }
    memcpy( lpbDst, m_lpBufCur, nLeft );
    m_lpBufCur += nLeft;
    return( nMax );
}

DWORD_PTR CArchive::ReadCount()
/*****************************/
{
    WORD    wCount;
    DWORD   dwCount;
    Read( &wCount, sizeof( WORD ) );
    if( wCount < 0xFFFF ) {
        return( wCount );
    }
    Read( &dwCount, sizeof( DWORD ) );
    return( dwCount );
}

CObject *CArchive::ReadObject( const CRuntimeClass *pClass )
/**********************************************************/
{
    WORD wTag;
    *this >> wTag;
    if( wTag == wNullTag ) {
        return( NULL );
    } else if( wTag < 0x7FFF && wTag <= m_pLoadArray->GetUpperBound() ) {
        return( (CObject *)m_pLoadArray->GetAt( wTag ) );
    } else if( wTag == 0x7FFF ) {
        DWORD dwTag;
        *this >> dwTag;
        if( dwTag <= m_pLoadArray->GetUpperBound() ) {
            return( (CObject *)m_pLoadArray->GetAt( dwTag ) );
        } else {
            throw new CArchiveException( CArchiveException::badIndex );
        }
    } else if( wTag == wNewClassTag ) {
        WORD wSchema;
        WORD wLength;
        *this >> wSchema;
        *this >> wLength;
        char *lpszClassName = new char[wLength + 1];
        Read( lpszClassName, wLength * sizeof( char ) );
        lpszClassName[wLength] = '\0';
        CObject *pOb = CRuntimeClass::CreateObject( lpszClassName );
        delete [] lpszClassName;
        if( pOb == NULL || (pClass != NULL && !pOb->IsKindOf( pClass )) ) {
            throw new CArchiveException( CArchiveException::badClass );
        }
        m_pLoadArray->Add( pOb );
        UINT wOldSchema = GetObjectSchema();
        SetObjectSchema( wSchema );
        pOb->Serialize( *this );
        SetObjectSchema( wOldSchema );
        return( pOb );
    } else {
        throw new CArchiveException( CArchiveException::badIndex );
    }
}

BOOL CArchive::ReadString( CString &rString )
/*******************************************/
{
    UINT    nCur = 0;
    TCHAR   szBuff[128];
    TCHAR   ch;
    for( ;; ) {
        Read( (void *)&ch, sizeof( TCHAR ) );
        if( ch == _T('\n') ) {
            break;
        } else if( ch == _T('\r') ) {
            Read( (void *)&ch, sizeof( TCHAR ) );
            break;
        }
        szBuff[nCur] = ch;
        nCur++;
        if( nCur == 127 ) {
            szBuff[127] = _T('\0');
            rString += szBuff;
            nCur = 0;
        }
    }
    szBuff[nCur] = _T('\0');
    rString += szBuff;
    return( TRUE );
}

LPTSTR CArchive::ReadString( LPTSTR lpsz, UINT nMax )
/***************************************************/
{
    UINT    nCur = 0;
    TCHAR   ch;
    while( nCur < nMax ) {
        Read( (void *)&ch, sizeof( TCHAR ) );
        if( ch == _T('\n') ) {
            break;
        } else if( ch == _T('\r') ) {
            Read( (void *)&ch, sizeof( TCHAR ) );
            break;
        }
        lpsz[nCur] = ch;
        nCur++;
    }
    lpsz[nCur] = _T('\0');
    return( lpsz );
}

void CArchive::Write( const void *lpBuf, UINT nMax )
/**************************************************/
{
    const BYTE *lpbSrc = (const BYTE *)lpBuf;
    while( m_lpBufMax - m_lpBufCur < nMax ) {
        memcpy( m_lpBufCur, lpbSrc, m_lpBufMax - m_lpBufCur );
        m_pFile->Write( m_lpBufStart, m_nBufSize );
        nMax -= m_lpBufMax - m_lpBufCur;
        lpbSrc += m_lpBufMax - m_lpBufCur;
        m_lpBufCur = m_lpBufStart;
    }
    memcpy( m_lpBufCur, lpbSrc, nMax );
    m_lpBufCur += nMax;
}

void CArchive::WriteCount( DWORD_PTR dwCount )
/********************************************/
{
    if( dwCount < 0xFFFF ) {
        WORD wCount = (WORD)dwCount;
        Write( &wCount, sizeof( WORD ) );
    } else {
        WORD wTag = 0xFFFF;
        Write( &wTag, sizeof( WORD ) );
        Write( &dwCount, sizeof( DWORD ) );
    }
}

void CArchive::WriteObject( const CObject *pOb )
/**********************************************/
{
    if( pOb == NULL ) {
        *this << wNullTag;
    } else {
        void *p;
        if( m_pStoreMap->Lookup( (void *)pOb, p ) ) {
            if( (DWORD)p < 0x7FFF ) {
                *this << (WORD)(DWORD)p;
            } else {
                *this << (WORD)0x7FFF;
                *this << (DWORD)p;
            }
        } else {
            m_nMapCount++;
            m_pStoreMap->SetAt( (void *)pOb, (void *)m_nMapCount );
            *this << wNewClassTag;
            pOb->GetRuntimeClass()->Store( *this );

            // Cast to non-const CObject in order to call Serialize.  Since
            // the archive is currently loading, pOb shouldn't be modified.
            ((CObject *)pOb)->Serialize( *this );
        }
    }
}

CArchive &CArchive::operator<<( const CString &str )
/**************************************************/
{
    int nLength = str.GetLength();
#ifdef _UNICODE
    (*this) << (BYTE)0xFF;
    (*this) << (WORD)0xFFFE;
#endif
    if( nLength < 0xFF ) {
        (*this) << (BYTE)nLength;
    } else {
        (*this) << (BYTE)0xFF;
        if( nLength < 0xFFFE ) {
            (*this) << (WORD)nLength;
        } else {
            (*this) << (WORD)0xFFFF;
            (*this) << (DWORD)nLength;
        }
    }
    Write( (const void *)(LPCTSTR)str, sizeof( TCHAR ) * nLength );
    return( *this );
}

CArchive &CArchive::operator>>( CString &str )
/********************************************/
{
    BOOL    bUnicode = FALSE;
    BYTE    byLength;
    WORD    wLength;
    DWORD   dwLength;
    int     nLength;

start:
    (*this) >> byLength;
    if( byLength < 0xFF ) {
        nLength = byLength;
    } else {
        (*this) >> wLength;
        if( wLength == 0xFFFE ) {
            bUnicode = TRUE;
            goto start;
        } else if( wLength < 0xFFFE ) {
            nLength = wLength;
        } else {
            (*this) >> dwLength;
            nLength = dwLength;
        }
    }
    if( bUnicode ) {
        wchar_t *pszBuff = new wchar_t[nLength + 1];
        Read( pszBuff, sizeof( wchar_t ) * nLength );
        pszBuff[nLength] = L'\0';
        str = pszBuff;
        delete [] pszBuff;
    } else {
        char    *pszBuff = new char[nLength + 1];
        Read( pszBuff, sizeof( char ) * nLength );
        pszBuff[nLength] = '\0';
        str = pszBuff;
        delete [] pszBuff;
    }
    return( *this );
}

CArchive & AFXAPI operator<<( CArchive &ar, const RECT &rect )
/************************************************************/
{
    ar << rect.left << rect.top << rect.right << rect.bottom;
    return( ar );
}

CArchive & AFXAPI operator<<( CArchive &ar, POINT point )
/*******************************************************/
{
    ar << point.x << point.y;
    return( ar );
}

CArchive & AFXAPI operator<<( CArchive &ar, SIZE size )
/*****************************************************/
{
    ar << size.cx << size.cy;
    return( ar );
}

CArchive & AFXAPI operator>>( CArchive &ar, RECT &rect )
/******************************************************/
{
    ar >> rect.left >> rect.top >> rect.right >> rect.bottom;
    return( ar );
}

CArchive & AFXAPI operator>>( CArchive &ar, POINT &point )
/********************************************************/
{
    ar >> point.x >> point.y;
    return( ar );
}

CArchive & AFXAPI operator>>( CArchive &ar, SIZE &size )
/******************************************************/
{
    ar >> size.cx >> size.cy;
    return( ar );
}
