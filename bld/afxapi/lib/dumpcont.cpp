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
* Description:  Implementation of CDumpContext.
*
****************************************************************************/


#include "stdafx.h"

// Instantiate the afxDump object, which is declared in afx.h
#ifdef _DEBUG
CDumpContext    afxDump;
#endif

void CDumpContext::OutputString( LPCTSTR lpsz )
/*********************************************/
{
    if( m_pFile != NULL ) {
        m_pFile->Write( lpsz, sizeof( TCHAR ) * _tcslen( lpsz ) );
    } else {
        ::OutputDebugString( lpsz );
    }
}

CDumpContext::CDumpContext( CFile *pFile )
/****************************************/
{
    m_pFile = pFile;
    m_nDepth = 0;
}

CDumpContext &CDumpContext::DumpAsHex( BYTE b )
/*********************************************/
{
    TCHAR   szBuff[5];
    _stprintf( szBuff, _T("0x%02hhx"), b );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::DumpAsHex( DWORD dw )
/***********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%08lx"), dw );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::DumpAsHex( int n )
/********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%08lx"), n );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::DumpAsHex( LONG l )
/*********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%08lx"), l );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::DumpAsHex( LONGLONG n )
/*************************************************/
{
    TCHAR   szBuff[19];
    _stprintf( szBuff, _T("0x%016I64x"), n );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::DumpAsHex( UINT u )
/*********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%08lx"), u );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::DumpAsHex( ULONGLONG n )
/**************************************************/
{
    TCHAR   szBuff[19];
    _stprintf( szBuff, _T("0x%016I64x"), n );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::DumpAsHex( WORD w )
/*********************************************/
{
    TCHAR   szBuff[7];
    _stprintf( szBuff, _T("0x%04hx"), w );
    OutputString( szBuff );
    return( *this );
}

void CDumpContext::Flush()
/************************/
{
    if( m_pFile != NULL ) {
        m_pFile->Flush();
    }
}

void CDumpContext::HexDump( LPCTSTR lpszLine, BYTE *pby, int nBytes, int nWidth )
/*******************************************************************************/
{
    int nColumn = 0;
    TCHAR szBuff[3];
    for( int i = 0; i < nBytes; i++ ) {
        if( nColumn == 0 ) {
            OutputString( lpszLine );
        }
        _stprintf( szBuff, _T(" %02hx"), pby[i] );
        OutputString( szBuff );
        nColumn++;
        if( nColumn >= nWidth ) {
            OutputString( _T("\r\n") );
            nColumn = 0;
        }
    }
    if( nColumn != 0 ) {
        OutputString( _T("\r\n") );
    }
}

CDumpContext &CDumpContext::operator<<( const CObject *pOb )
/**********************************************************/
{
    if( pOb != NULL ) {
        pOb->Dump( *this );
    }
    return( *this );
}

CDumpContext &CDumpContext::operator<<( const CObject &ob )
/*********************************************************/
{
    ob.Dump( *this );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( LPCSTR lpsz )
/***************************************************/
{
    int         nLength = strlen( lpsz );
#ifdef _UNICODE
    wchar_t     *pszBuff1 = new wchar_t[nLength + 1];
    ::MultiByteToWideChar( CP_ACP, 0L, lpsz, -1, pszBuff1, nLength + 1 );
#else
    const char  *pszBuff1 = lpsz;
#endif

    // Convert LF to CR+LF.
    TCHAR   *pszBuff2 = new TCHAR[2 * nLength + 1];
    int     nCurPos = 0;
    for( int i = 0; i < nLength; i++ ) {
        if( pszBuff1[i] == _T('\n') ) {
            pszBuff2[nCurPos] = _T('\r');
            nCurPos++;
        }
        pszBuff2[nCurPos] = pszBuff1[i];
        nCurPos++;
    }
    pszBuff2[nCurPos] = _T('\0');

    OutputString( pszBuff2 );
    delete [] pszBuff2;
#ifdef _UNICODE
    delete [] pszBuff1;
#endif
    return( *this );
}

CDumpContext &CDumpContext::operator<<( LPCWSTR lpsz )
/****************************************************/
{
    int             nLength = wcslen( lpsz );
#ifndef _UNICODE
    char            *pszBuff1 = new char[nLength + 1];
    ::WideCharToMultiByte( CP_ACP, 0L, lpsz, -1, pszBuff1, nLength + 1, NULL, NULL );
#else
    const wchar_t   *pszBuff1 = lpsz;
#endif

    // Convert LF to CR+LF.
    TCHAR   *pszBuff2 = new TCHAR[2 * nLength + 1];
    int     nCurPos = 0;
    for( int i = 0; i < nLength; i++ ) {
        if( pszBuff1[i] == _T('\n') ) {
            pszBuff2[nCurPos] = _T('\r');
            nCurPos++;
        }
        pszBuff2[nCurPos] = pszBuff1[i];
        nCurPos++;
    }
    pszBuff2[nCurPos] = _T('\0');

    OutputString( pszBuff2 );
    delete [] pszBuff2;
#ifndef _UNICODE
    delete [] pszBuff1;
#endif
    return( *this );
}

CDumpContext &CDumpContext::operator<<( const void *lp )
/******************************************************/
{
    TCHAR szBuff[11];
    _stprintf( szBuff, _T("0x%8lx"), (LONG)lp );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( BYTE by )
/***********************************************/
{
    TCHAR szBuff[5];
    _stprintf( szBuff, _T("%hhu"), by );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( WORD w )
/**********************************************/
{
    TCHAR   szBuff[7];
    _stprintf( szBuff, _T("%hu"), w );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( DWORD dw )
/************************************************/
{
    TCHAR   szBuff[12];
    _stprintf( szBuff, _T("%lu"), dw );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( int n )
/*********************************************/
{
    TCHAR szBuff[12];
    _stprintf( szBuff, _T("%ld"), n );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( double d )
/************************************************/
{
    TCHAR szBuff[32];
    _stprintf( szBuff, _T("%g"), d );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( float f )
/***********************************************/
{
    TCHAR   szBuff[32];
    _stprintf( szBuff, _T("%hg"), f );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( LONG l )
/**********************************************/
{
    TCHAR   szBuff[12];
    _stprintf( szBuff, _T("%ld"), l );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( UINT u )
/**********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("%lu"), u );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( LONGLONG n )
/**************************************************/
{
    TCHAR   szBuff[22];
    _stprintf( szBuff, _T("%I64d"), n );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( ULONGLONG n )
/***************************************************/
{
    TCHAR   szBuff[21];
    _stprintf( szBuff, _T("%I64u"), n );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( HWND h )
/**********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%0lx"), (LONG)h );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( HDC h )
/*********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%0lx"), (LONG)h );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( HMENU h )
/***********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%0lx"), (LONG)h );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( HACCEL h )
/************************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%0lx"), (LONG)h );
    OutputString( szBuff );
    return( *this );
}

CDumpContext &CDumpContext::operator<<( HFONT h )
/***********************************************/
{
    TCHAR   szBuff[11];
    _stprintf( szBuff, _T("0x%0lx"), (LONG)h );
    OutputString( szBuff );
    return( *this );
}

#ifdef _DEBUG

CDumpContext & AFXAPI operator<<( CDumpContext &dc, const RECT &rect )
/********************************************************************/
{
    dc << "(" << rect.left << ", " << rect.top << ", ";
    dc << rect.right << ", " << rect.bottom << ")";
    return( dc );
}

CDumpContext & AFXAPI operator<<( CDumpContext &dc, POINT point )
/***************************************************************/
{
    dc << "(" << point.x << ", " << point.y << ")";
    return( dc );
}

CDumpContext & AFXAPI operator<<( CDumpContext &dc, SIZE size )
/*************************************************************/
{
    dc << "(" << size.cx << ", " << size.cy << ")";
    return( dc );
}

#endif // _DEBUG
