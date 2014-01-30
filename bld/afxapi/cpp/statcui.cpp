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
* Description:  Implementation of CStatusCmdUI.
*
****************************************************************************/


#include "stdafx.h"
#include "statcui.h"

void CStatusCmdUI::Enable( BOOL bOn )
/***********************************/
{
    CStatusBar *pStatusBar = (CStatusBar *)m_pOther;
    ASSERT( pStatusBar != NULL );
    ASSERT( pStatusBar->IsKindOf( RUNTIME_CLASS( CStatusBar ) ) );
    UINT nStyle = pStatusBar->GetPaneStyle( m_nIndex );
    if( bOn ) {
        nStyle &= ~SBPS_DISABLED;
    } else {
        nStyle |= SBPS_DISABLED;
    }
    pStatusBar->SetPaneStyle( m_nIndex, nStyle );
}

void CStatusCmdUI::SetCheck( int nCheck )
/***************************************/
{
    CStatusBar *pStatusBar = (CStatusBar *)m_pOther;
    ASSERT( pStatusBar != NULL );
    ASSERT( pStatusBar->IsKindOf( RUNTIME_CLASS( CStatusBar ) ) );
    UINT nStyle = pStatusBar->GetPaneStyle( m_nIndex );
    if( nCheck == 0 ) {
        nStyle &= ~SBPS_POPOUT;
    } else {
        nStyle |= SBPS_POPOUT;
    }
    pStatusBar->SetPaneStyle( m_nIndex, nStyle );
}

void CStatusCmdUI::SetText( LPCTSTR lpszText )
/******************************************/
{
    CStatusBar *pStatusBar = (CStatusBar *)m_pOther;
    ASSERT( pStatusBar != NULL );
    ASSERT( pStatusBar->IsKindOf( RUNTIME_CLASS( CStatusBar ) ) );
    pStatusBar->SetPaneText( m_nIndex, lpszText );
}
