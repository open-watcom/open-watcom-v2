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
* Description:  Implementation of CStatic.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CStatic, CWnd )

CStatic::CStatic()
/****************/
{
}

BOOL CStatic::Create( LPCTSTR lpszText, DWORD dwStyle, const RECT &rect,
                      CWnd *pParentWnd, UINT nID )
/************************************************/
{
    return( CWnd::Create( _T("STATIC"), lpszText, dwStyle, rect, pParentWnd, nID ) );
}

void CStatic::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
/*********************************************************/
{
    UNUSED_ALWAYS( lpDrawItemStruct );
}

BOOL CStatic::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam,
                             LRESULT *pResult )
/*********************************************/
{
    UNUSED_ALWAYS( wParam );
    UNUSED_ALWAYS( pResult );
    
    if( message == WM_DRAWITEM ) {
        DrawItem( (LPDRAWITEMSTRUCT)lParam );
        return( TRUE );
    }
    return( FALSE );
}
