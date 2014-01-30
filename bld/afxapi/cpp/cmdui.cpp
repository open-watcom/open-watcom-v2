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
* Description:  Implementation of CCmdUI.
*
****************************************************************************/


#include "stdafx.h"

CCmdUI::CCmdUI()
/**************/
{
    m_nID = -1;
    m_nIndex = -1;
    m_pMenu = NULL;
    m_pSubMenu = NULL;
    m_pOther = NULL;
    m_bContinueRouting = FALSE;
}

void CCmdUI::Enable( BOOL bOn )
/*****************************/
{
    if( m_pMenu != NULL && m_pSubMenu == NULL ) {
        m_pMenu->EnableMenuItem( m_nID, MF_BYCOMMAND | (bOn ? MF_ENABLED : MF_GRAYED) );
    }
    if( m_pOther != NULL ) {
        m_pOther->EnableWindow( bOn );
    }
}

void CCmdUI::SetCheck( int nCheck )
/*********************************/
{
    if( m_pMenu != NULL && m_pSubMenu == NULL ) {
        if( nCheck == 0 ) {
            m_pMenu->CheckMenuItem( m_nID, MF_BYCOMMAND | MF_UNCHECKED );
        } else if( nCheck == 1 ) {
            m_pMenu->CheckMenuItem( m_nID, MF_BYCOMMAND | MF_CHECKED );
        }
    }
    if( m_pOther != NULL ) {
        if( nCheck == 0 ) {
            m_pOther->SendMessage( BM_SETCHECK, BST_UNCHECKED );
        } else if( nCheck == 1 ) {
            m_pOther->SendMessage( BM_SETCHECK, BST_CHECKED );
        } else if( nCheck == 2 ) {
            m_pOther->SendMessage( BM_SETCHECK, BST_INDETERMINATE );
        }
    }
}

void CCmdUI::SetRadio( BOOL bOn )
/*******************************/
{
    SetCheck( bOn ? 1 : 0 );
}

void CCmdUI::SetText( LPCTSTR lpszText )
/**************************************/
{
    if( m_pMenu != NULL && m_pSubMenu == NULL ) {
        UINT nState = m_pMenu->GetMenuState( m_nID, MF_BYCOMMAND );
        m_pMenu->ModifyMenu( m_nID, nState | MF_BYCOMMAND, m_nID, lpszText );
    }
    if( m_pOther != NULL ) {
        m_pOther->SetWindowText( lpszText );
    }
}
