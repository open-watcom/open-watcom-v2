/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of CTime.
*
****************************************************************************/


#include "stdafx.h"

CTime::CTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST )
/**************************************************************************************/
{
    struct tm crtTime = { nSec, nMin, nHour, nDay, nMonth, nYear - 1900, -1, -1, nDST };
    m_time = mktime( &crtTime );
}

CTime::CTime( WORD wDosDate, WORD wDosTime, int nDST )
/****************************************************/
{
    FILETIME    ft;
    FILETIME    ftLocal;
    SYSTEMTIME  st;
    ::DosDateTimeToFileTime( wDosDate, wDosTime, &ft );
    ::FileTimeToLocalFileTime( &ft, &ftLocal );
    ::FileTimeToSystemTime( &ftLocal, &st );
    struct tm crtTime = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1,
                          st.wYear - 1900, -1, -1, nDST };
    m_time = mktime( &crtTime );
}

CTime::CTime( const SYSTEMTIME &st, int nDST )
/********************************************/
{
    struct tm crtTime = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1,
                          st.wYear - 1900, -1, -1, nDST };
    m_time = mktime( &crtTime );
}

CTime::CTime( const FILETIME &ft, int nDST )
/******************************************/
{
    FILETIME    ftLocal;
    SYSTEMTIME  st;
    ::FileTimeToLocalFileTime( &ft, &ftLocal );
    ::FileTimeToSystemTime( &ftLocal, &st );
    struct tm crtTime = { st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth - 1,
                          st.wYear - 1900, -1, -1, nDST };
    m_time = mktime( &crtTime );
}

CString CTime::Format( LPCTSTR lpszFormat )
/*****************************************/
{
    struct tm   crtTime;
    TCHAR       szBuff[256];
    _localtime( &m_time, &crtTime );
    _tcsftime( szBuff, 256, lpszFormat, &crtTime );
    return( szBuff );
}

CString CTime::FormatGmt( LPCTSTR lpszFormat )
/********************************************/
{
    struct tm   crtTime;
    TCHAR       szBuff[256];
    _gmtime( &m_time, &crtTime );
    _tcsftime( szBuff, 256, lpszFormat, &crtTime );
    return( szBuff );
}

BOOL CTime::GetAsSystemTime( SYSTEMTIME &st ) const
/*************************************************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    st.wYear = crtTime.tm_year + 1900;
    st.wMonth = crtTime.tm_mon + 1;
    st.wDayOfWeek = crtTime.tm_wday;
    st.wDay = crtTime.tm_mday;
    st.wHour = crtTime.tm_hour;
    st.wMinute = crtTime.tm_min;
    st.wSecond = crtTime.tm_sec;
    st.wMilliseconds = 0;
    return( TRUE );
}

int CTime::GetDay() const
/***********************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    return( crtTime.tm_mday );
}

int CTime::GetDayOfWeek() const
/*****************************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    return( crtTime.tm_wday );
}

struct tm *CTime::GetGmtTm( struct tm *ptm ) const
/************************************************/
{
    if( ptm == NULL ) {
        throw( new CInvalidArgException );
    }
    _gmtime( &m_time, ptm );
    return( ptm );
}

int CTime::GetHour() const
/************************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    return( crtTime.tm_hour );
}

struct tm *CTime::GetLocalTm( struct tm *ptm ) const
/**************************************************/
{
    if( ptm == NULL ) {
        throw( new CInvalidArgException );
    }
    _localtime( &m_time, ptm );
    return( ptm );
}

int CTime::GetMinute() const
/**************************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    return( crtTime.tm_min );
}

int CTime::GetMonth() const
/*************************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    return( crtTime.tm_mon );
}

int CTime::GetSecond() const
/**************************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    return( crtTime.tm_sec );
}

int CTime::GetYear() const
/************************/
{
    struct tm   crtTime;
    _localtime( &m_time, &crtTime );
    return( crtTime.tm_year );
}
