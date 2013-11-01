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
* Description:  Implementation of CMultiDocTemplate.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CMultiDocTemplate, CDocTemplate )

CMultiDocTemplate::CMultiDocTemplate( UINT nIDResource, CRuntimeClass *pDocClass,
                                      CRuntimeClass *pFrameClass,
                                      CRuntimeClass *pViewClass )
    : CDocTemplate( nIDResource, pDocClass, pFrameClass, pViewClass )
/*******************************************************************/
{
    m_nUntitledCount = 0;
}

void CMultiDocTemplate::AddDocument( CDocument *pDoc )
/****************************************************/
{
    CDocTemplate::AddDocument( pDoc );

    ASSERT( m_docList.Find( pDoc ) == NULL );
    m_docList.AddTail( pDoc );
}

POSITION CMultiDocTemplate::GetFirstDocPosition() const
/*****************************************************/
{
    return( m_docList.GetHeadPosition() );
}

CDocument *CMultiDocTemplate::GetNextDoc( POSITION &rPos ) const
/**************************************************************/
{
    CDocument *pDoc = (CDocument *)m_docList.GetNext( rPos );
    ASSERT( pDoc != NULL );
    ASSERT( pDoc->IsKindOf( RUNTIME_CLASS( CDocument ) ) );
    return( pDoc );
}

void CMultiDocTemplate::LoadTemplate()
/************************************/
{
    CDocTemplate::LoadTemplate();

    if( m_hMenuShared == NULL ) {
        HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( m_nIDResource ),
                                                     RT_MENU );
        m_hMenuShared = ::LoadMenu( hInstance, MAKEINTRESOURCE( m_nIDResource ) );
    }
    if( m_hAccelTable == NULL ) {
        HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( m_nIDResource ),
                                                     RT_ACCELERATOR );
        m_hAccelTable = ::LoadAccelerators( hInstance, MAKEINTRESOURCE( m_nIDResource ) );
    }
}

CDocument *CMultiDocTemplate::OpenDocumentFile( LPCTSTR lpszPathName, BOOL bMakeVisible )
/***************************************************************************************/
{
    CDocument *pDoc = CreateNewDocument();
    CFrameWnd *pFrame = CreateNewFrame( pDoc, NULL );
    if( lpszPathName == NULL ) {
        if( !pDoc->OnNewDocument() ) {
            return( NULL );
        }
        SetDefaultTitle( pDoc );
    } else {
        if( !pDoc->OnOpenDocument( lpszPathName ) ) {
            return( NULL );
        }
        pDoc->SetPathName( lpszPathName );
    }
    InitialUpdateFrame( pFrame, pDoc, bMakeVisible );
    return( pDoc );
}

void CMultiDocTemplate::RemoveDocument( CDocument *pDoc )
/*******************************************************/
{
    CDocTemplate::RemoveDocument( pDoc );

    POSITION position = m_docList.Find( pDoc );
    ASSERT( position != NULL );
    m_docList.RemoveAt( position );
}

void CMultiDocTemplate::SetDefaultTitle( CDocument *pDocument )
/*************************************************************/
{
    ASSERT( pDocument != NULL );
    CString strDocName;
    CString strTitle;
    if( !GetDocString( strDocName, docName ) || strDocName.IsEmpty() ) {
        strTitle.LoadString( AFX_IDS_UNTITLED );
    }
    m_nUntitledCount++;
    strTitle.Format( _T("%s%d"), (LPCTSTR)strDocName, m_nUntitledCount );
    pDocument->SetTitle( strTitle );
}

#ifdef _DEBUG

void CMultiDocTemplate::AssertValid() const
/*****************************************/
{
    CDocTemplate::AssertValid();

    ASSERT( m_hMenuShared == NULL || ::IsMenu( m_hMenuShared ) );
}

void CMultiDocTemplate::Dump( CDumpContext &dc ) const
/****************************************************/
{
    CDocTemplate::Dump( dc );

    dc << "m_hMenuShared = " << m_hMenuShared << "\n";
    dc << "m_hAccelTable = " << m_hAccelTable << "\n";
    dc << "m_nUntitledCount = " << m_nUntitledCount << "\n";
}

#endif // _DEBUG
