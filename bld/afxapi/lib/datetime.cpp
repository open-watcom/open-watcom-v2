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
* Description:  Implementation of CDateTimeCtrl.
*
****************************************************************************/


#include "stdafx.h"
#include <afxdtctl.h>

IMPLEMENT_DYNAMIC( CDateTimeCtrl, CWnd )

CDateTimeCtrl::CDateTimeCtrl()
/****************************/
{
}

BOOL CDateTimeCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/***************************************************************************************/
{
    return( CWnd::Create( DATETIMEPICK_CLASS, NULL, dwStyle, rect, pParentWnd, nID ) );
}

DWORD CDateTimeCtrl::GetRange( CTime *pMinRange, CTime *pMaxRange ) const
/***********************************************************************/
{
    SYSTEMTIME  st[2];
    DWORD       dwRet = ::SendMessage( m_hWnd, DTM_GETRANGE, 0, (LPARAM)st );
    if( pMinRange != NULL ) {
        *pMinRange = CTime( st[0] ).GetTime();
    }
    if( pMaxRange != NULL ) {
        *pMaxRange = CTime( st[1] ).GetTime();
    }
    return( dwRet );
}

BOOL CDateTimeCtrl::SetRange( const CTime *pMinRange, const CTime *pMaxRange )
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
    return( ::SendMessage( m_hWnd, DTM_SETRANGE, nFlags, (LPARAM)&st ) );
}

BOOL CDateTimeCtrl::SetTime( const CTime *pTimeNew )
/**************************************************/
{
    if( pTimeNew == NULL ) {
        return( ::SendMessage( m_hWnd, DTM_SETSYSTEMTIME, GDT_NONE, 0L ) );
    } else {
        SYSTEMTIME st;
        pTimeNew->GetAsSystemTime( st );
        return( ::SendMessage( m_hWnd, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&st ) );
    }
}

BOOL CDateTimeCtrl::SetTime( LPSYSTEMTIME pTimeNew )
/**************************************************/
{
    if( pTimeNew == NULL ) {
        return( ::SendMessage( m_hWnd, DTM_SETSYSTEMTIME, GDT_NONE, 0L ) );
    } else {
        return( ::SendMessage( m_hWnd, DTM_SETSYSTEMTIME, GDT_VALID,
                               (LPARAM)pTimeNew ) );
    }
}
