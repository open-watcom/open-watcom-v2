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
* Description:  Implementation of CHotKeyCtrl.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CHotKeyCtrl, CWnd )

CHotKeyCtrl::CHotKeyCtrl()
/************************/
{
}

BOOL CHotKeyCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID )
/*************************************************************************************/
{
    return( CWnd::Create( HOTKEY_CLASS, NULL, dwStyle, rect, pParentWnd, nID ) );
}

BOOL CHotKeyCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT &rect,
                            CWnd *pParentWnd, UINT nID )
/******************************************************/
{
    return( CWnd::CreateEx( dwExStyle, HOTKEY_CLASS, NULL, dwStyle, rect, pParentWnd,
                            nID ) );
}

void CHotKeyCtrl::GetHotKey( WORD &wVirtualKeyCode, WORD &wModifiers ) const
/**************************************************************************/
{
    DWORD dwHotKey = (DWORD)::SendMessage( m_hWnd, HKM_GETHOTKEY, 0, 0L );
    wVirtualKeyCode = LOBYTE( LOWORD( dwHotKey ) );
    wModifiers = HIBYTE( LOWORD( dwHotKey ) );
}

CString CHotKeyCtrl::GetHotKeyName() const
/****************************************/
{
    WORD wVirtualKeyCode;
    WORD wModifiers;
    GetHotKey( wVirtualKeyCode, wModifiers );

    CString str;
    if( wModifiers & HOTKEYF_CONTROL ) {
        str += GetKeyName( VK_CONTROL, FALSE );
        str += _T(" + ");
    }
    if( wModifiers & HOTKEYF_ALT ) {
        str += GetKeyName( VK_MENU, FALSE );
        str += _T(" + ");
    }
    if( wModifiers & HOTKEYF_SHIFT ) {
        str += GetKeyName( VK_SHIFT, FALSE );
        str += _T(" + ");
    }
    str += GetKeyName( wVirtualKeyCode, wModifiers & HOTKEYF_EXT );
    return( str );
}

CString CHotKeyCtrl::GetKeyName( UINT vk, BOOL fExtended )
/********************************************************/
{
    LPARAM lParam = ::MapVirtualKey( vk, MAPVK_VK_TO_VSC ) << 16;
    if( fExtended ) {
        lParam |= 0x01000000L;
    }

    CString str;
    LPTSTR  lpString = str.GetBuffer( 64 );
    int     nResult = ::GetKeyNameText( lParam, lpString, 64 );
    str.ReleaseBuffer( nResult );
    return( str );
}
