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
* Description:  Implementation of CFileNewDialog.
*
****************************************************************************/


#include "stdafx.h"
#include "filenew.h"

BEGIN_MESSAGE_MAP( CFileNewDialog, CDialog )
    ON_CONTROL( LBN_DBLCLK, AFX_IDC_LISTBOX, OnDblClkListBox )
END_MESSAGE_MAP()

CFileNewDialog::CFileNewDialog( CDocManager *pDocManager )
    : CDialog( AFX_IDD_NEWTYPEDLG )
/*********************************/
{
    ASSERT( pDocManager != NULL );
    m_pDocManager = pDocManager;
    m_pDocTemplate = NULL;
}

void CFileNewDialog::DoDataExchange( CDataExchange *pDX )
/*******************************************************/
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, AFX_IDC_LISTBOX, m_wndListBox );
}

void CFileNewDialog::OnCancel()
/*****************************/
{
    m_pDocTemplate = NULL;
    CDialog::OnCancel();
}

BOOL CFileNewDialog::OnInitDialog()
/*********************************/
{
    CDialog::OnInitDialog();

    POSITION position = m_pDocManager->GetFirstDocTemplatePosition();
    while( position != NULL ) {
        CDocTemplate *pTemplate = m_pDocManager->GetNextDocTemplate( position );
        ASSERT( pTemplate != NULL );
        CString strFileNewName;
        if( pTemplate->GetDocString( strFileNewName, CDocTemplate::fileNewName ) ) {
            int i = m_wndListBox.AddString( strFileNewName );
            m_wndListBox.SetItemDataPtr( i, pTemplate );
        }
    }
    m_wndListBox.SetCurSel( 0 );
    
    return( TRUE );
}

void CFileNewDialog::OnOK()
/*************************/
{
    int i = m_wndListBox.GetCurSel();
    ASSERT( i >= 0 );
    m_pDocTemplate = (CDocTemplate *)m_wndListBox.GetItemDataPtr( i );
    ASSERT( m_pDocTemplate != NULL );
    ASSERT( m_pDocTemplate != (CDocTemplate *)-1 );
    ASSERT( m_pDocTemplate->IsKindOf( RUNTIME_CLASS( CDocTemplate ) ) );

    CDialog::OnOK();
}

void CFileNewDialog::OnDblClkListBox()
/************************************/
{
    OnOK();
}
