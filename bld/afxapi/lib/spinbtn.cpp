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
* Description:  Implementation of CSpinButtonCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CSpinButtonCtrl, CWnd )

CSpinButtonCtrl::CSpinButtonCtrl()
/********************************/
{
}

BOOL CSpinButtonCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd,
                              UINT nID )
/**************************************/
{
    return( CWnd::Create( UPDOWN_CLASS, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CSpinButtonCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                                CWnd *pParentWnd, UINT nID )
/**********************************************************/
{
    return( CWnd::CreateEx( dwExStyle, UPDOWN_CLASS, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

void CSpinButtonCtrl::GetRange( int &lower, int &upper ) const
/************************************************************/
{
    DWORD dwRange = (DWORD)::SendMessage( m_hWnd, UDM_GETRANGE, 0, 0L );
    lower = HIWORD( dwRange );
    upper = LOWORD( dwRange );
}
