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
* Description:  Implementation of CIPAddressCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CIPAddressCtrl, CWnd )

CIPAddressCtrl::CIPAddressCtrl()
/******************************/
{
}

BOOL CIPAddressCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd,
                             UINT nID )
/*************************************/
{
    return( CWnd::Create( WC_IPADDRESS, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CIPAddressCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                               CWnd *pParentWnd, UINT nID )
/*********************************************************/
{
    return( CWnd::CreateEx( dwExStyle, WC_IPADDRESS, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

int CIPAddressCtrl::GetAddress( BYTE &nField0, BYTE &nField1, BYTE &nField2,
                                BYTE &nField3 )
/*********************************************/
{
    DWORD   dwAddress = 0L;
    int     nResult = ::SendMessage( m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&dwAddress );
    nField0 = (BYTE)FIRST_IPADDRESS( dwAddress );
    nField1 = (BYTE)SECOND_IPADDRESS( dwAddress );
    nField2 = (BYTE)THIRD_IPADDRESS( dwAddress );
    nField3 = (BYTE)FOURTH_IPADDRESS( dwAddress );
    return( nResult );
}
