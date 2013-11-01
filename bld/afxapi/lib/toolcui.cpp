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
* Description:  Implementation of CToolCmdUI.
*
****************************************************************************/


#include "stdafx.h"
#include "toolcui.h"

void CToolCmdUI::Enable( BOOL bOn )
/*********************************/
{
    CToolBar *pToolBar = (CToolBar *)m_pOther;
    ASSERT( pToolBar != NULL );
    ASSERT( pToolBar->IsKindOf( RUNTIME_CLASS( CToolBar ) ) );
    UINT nStyle = pToolBar->GetButtonStyle( m_nIndex );
    if( bOn ) {
        nStyle &= ~TBBS_DISABLED;
    } else {
        nStyle |= TBBS_DISABLED;
    }
    pToolBar->SetButtonStyle( m_nIndex, nStyle );
}

void CToolCmdUI::SetCheck( int nCheck )
/*************************************/
{
    CToolBar *pToolBar = (CToolBar *)m_pOther;
    ASSERT( pToolBar != NULL );
    ASSERT( pToolBar->IsKindOf( RUNTIME_CLASS( CToolBar ) ) );
    UINT nStyle = pToolBar->GetButtonStyle( m_nIndex );
    if( nCheck == 0 ) {
        nStyle &= ~(TBBS_CHECKED | TBBS_INDETERMINATE);
    } else if( nCheck == 1 ) {
        nStyle |= TBBS_CHECKED;
        nStyle &= ~TBBS_INDETERMINATE;
    } else {
        nStyle &= ~TBBS_CHECKED;
        nStyle |= TBBS_INDETERMINATE;
    }
    pToolBar->SetButtonStyle( m_nIndex, nStyle );
}

void CToolCmdUI::SetText( LPCTSTR lpszText )
/******************************************/
{
    UNUSED_ALWAYS( lpszText );
}
