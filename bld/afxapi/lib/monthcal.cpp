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
* Description:  Implementation of CMonthCalCtrl.
*
****************************************************************************/


#include "stdafx.h"
#include <afxdtctl.h>

IMPLEMENT_DYNAMIC( CMonthCalCtrl, CWnd )

CMonthCalCtrl::CMonthCalCtrl()
/****************************/
{
}

BOOL CMonthCalCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/***************************************************************************************/
{
    return( CWnd::Create( MONTHCAL_CLASS, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CMonthCalCtrl::Create( DWORD dwStyle, const POINT &pt, CWnd *pParentWnd, UINT nID )
/**************************************************************************************/
{
    if( !CWnd::Create( MONTHCAL_CLASS, NULL, dwStyle & ~WS_VISIBLE,
                       CRect( pt.x, pt.y, 0, 0 ), pParentWnd, nID ) ) {
        return( FALSE );
    }

    if( !SizeMinReq() ) {
        DestroyWindow();
        return( FALSE );
    }

    if( dwStyle & WS_VISIBLE ) {
        ::ShowWindow( m_hWnd, SW_SHOWNA );
    }
    return( TRUE );
}

int CMonthCalCtrl::GetMonthRange( CTime &refMinRange, CTime &refMaxRange,
                                  DWORD dwFlags ) const
/*****************************************************/
{
    SYSTEMTIME  st[2];
    int         nRet = (int)::SendMessage( m_hWnd, MCM_GETMONTHRANGE, (WPARAM)dwFlags,
                                           (LPARAM)st );
    refMinRange = CTime( st[0] ).GetTime();
    refMaxRange = CTime( st[1] ).GetTime();
    return( nRet );
}

int CMonthCalCtrl::GetMonthRange( LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange,
                                  DWORD dwFlags ) const
/*****************************************************/
{
    SYSTEMTIME  st[2];
    int         nRet = (int)::SendMessage( m_hWnd, MCM_GETMONTHRANGE, (WPARAM)dwFlags,
                                           (LPARAM)st );
    if( pMinRange != NULL ) {
        memcpy( pMinRange, &st[0], sizeof( SYSTEMTIME ) );
    }
    if( pMaxRange != NULL ) {
        memcpy( pMaxRange, &st[1], sizeof( SYSTEMTIME ) );
    }
    return( nRet );
}

DWORD CMonthCalCtrl::GetRange( CTime *pMinRange, CTime *pMaxRange ) const
/***********************************************************************/
{
    SYSTEMTIME  st[2];
    DWORD       dwRet = ::SendMessage( m_hWnd, MCM_GETRANGE, 0, (LPARAM)st );
    if( pMinRange != NULL ) {
        *pMinRange = CTime( st[0] ).GetTime();
    }
    if( pMaxRange != NULL ) {
        *pMaxRange = CTime( st[1] ).GetTime();
    }
    return( dwRet );
}

DWORD CMonthCalCtrl::GetRange( LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange ) const
/***********************************************************************************/
{
    SYSTEMTIME  st[2];
    DWORD       dwRet = ::SendMessage( m_hWnd, MCM_GETRANGE, 0, (LPARAM)st );
    if( pMinRange != NULL ) {
        memcpy( pMinRange, &st[0], sizeof( SYSTEMTIME ) );
    }
    if( pMaxRange != NULL ) {
        memcpy( pMaxRange, &st[1], sizeof( SYSTEMTIME ) );
    }
    return( dwRet );
}

BOOL CMonthCalCtrl::GetSelRange( CTime &refMinRange, CTime &refMaxRange ) const
/*****************************************************************************/
{
    SYSTEMTIME  st[2];
    if( !::SendMessage( m_hWnd, MCM_GETSELRANGE, 0, (LPARAM)st ) ) {
        return( FALSE );
    }
    refMinRange = CTime( st[0] ).GetTime();
    refMaxRange = CTime( st[1] ).GetTime();
    return( TRUE );
}

BOOL CMonthCalCtrl::GetSelRange( LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange ) const
/*************************************************************************************/
{
    SYSTEMTIME  st[2];
    if( !::SendMessage( m_hWnd, MCM_GETSELRANGE, 0, (LPARAM)st ) ) {
        return( FALSE );
    }
    if( pMinRange != NULL ) {
        memcpy( pMinRange, &st[0], sizeof( SYSTEMTIME ) );
    }
    if( pMaxRange != NULL ) {
        memcpy( pMaxRange, &st[1], sizeof( SYSTEMTIME ) );
    }
    return( TRUE );
}

BOOL CMonthCalCtrl::SetRange( const CTime *pMinRange, const CTime *pMaxRange )
/****************************************************************************/
{
    SYSTEMTIME  st[2];
    UINT        nFlags = 0;
    if( pMinRange != NULL ) {
        nFlags |= GDTR_MIN;
        pMinRange->GetAsSystemTime( st[0] );
    }
    if( pMaxRange != NULL ) {
        nFlags |= GDTR_MAX;
        pMaxRange->GetAsSystemTime( st[1] );
    }
    return( ::SendMessage( m_hWnd, MCM_SETRANGE, nFlags, (LPARAM)st ) );
}

BOOL CMonthCalCtrl::SetRange( LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange )
/****************************************************************************/
{
    SYSTEMTIME  st[2];
    UINT        nFlags = 0;
    if( pMinRange != NULL ) {
        nFlags |= GDTR_MIN;
        memcpy( &st[0], pMinRange, sizeof( SYSTEMTIME ) );
    }
    if( pMaxRange != NULL ) {
        nFlags |= GDTR_MAX;
        memcpy( &st[1], pMaxRange, sizeof( SYSTEMTIME ) );
    }
    return( ::SendMessage( m_hWnd, MCM_SETRANGE, nFlags, (LPARAM)st ) );
}

BOOL CMonthCalCtrl::SizeMinReq( BOOL bRepaint )
/*********************************************/
{
    RECT rect;
    if( !::SendMessage( m_hWnd, MCM_GETMINREQRECT, 0, (LPARAM)&rect ) ) {
        return( FALSE );
    }
    if( bRepaint ) {
        ::SetWindowPos( m_hWnd, NULL, 0, 0, rect.right, rect.bottom,
                        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
    } else {
        ::SetWindowPos( m_hWnd, NULL, 0, 0, rect.right, rect.bottom,
                        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW );
    }
    return( TRUE );
}
