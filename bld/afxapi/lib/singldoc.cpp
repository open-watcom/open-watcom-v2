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
* Description:  Implementation of CSingleDocTemplate.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CSingleDocTemplate, CDocTemplate )

CSingleDocTemplate::CSingleDocTemplate( UINT nIDResource, CRuntimeClass *pDocClass,
                                        CRuntimeClass *pFrameClass,
                                        CRuntimeClass *pViewClass )
    : CDocTemplate( nIDResource, pDocClass, pFrameClass, pViewClass )
/*******************************************************************/
{
    m_pOnlyDoc = NULL;
}

void CSingleDocTemplate::AddDocument( CDocument *pDoc )
/*****************************************************/
{
    CDocTemplate::AddDocument( pDoc );

    ASSERT( m_pOnlyDoc == NULL );
    m_pOnlyDoc = pDoc;
}
    
POSITION CSingleDocTemplate::GetFirstDocPosition() const
/******************************************************/
{
    if( m_pOnlyDoc != NULL ) {
        return( (POSITION)1 );
    } else {
        return( NULL );
    }
}

CDocument *CSingleDocTemplate::GetNextDoc( POSITION &rPos ) const
/***************************************************************/
{
    if( rPos == (POSITION)1 ) {
        rPos = NULL;
        return( m_pOnlyDoc );
    } else {
        rPos = NULL;
        return( NULL );
    }
}

CDocument *CSingleDocTemplate::OpenDocumentFile( LPCTSTR lpszPathName,
                                                 BOOL bMakeVisible )
/******************************************************************/
{
    CDocument *pDoc = m_pOnlyDoc;
    if( pDoc != NULL ) {
        if( !pDoc->SaveModified() ) {
            return( NULL );
        }
    } else {
        pDoc = CreateNewDocument();
        ASSERT( pDoc == m_pOnlyDoc );
    }

    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );
    if( pApp->m_pMainWnd == NULL ) {
        pApp->m_pMainWnd = CreateNewFrame( pDoc, NULL );
        ASSERT( pApp->m_pMainWnd != NULL );
    }

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

    InitialUpdateFrame( (CFrameWnd *)pApp->m_pMainWnd, pDoc, bMakeVisible );
    return( pDoc );
}

void CSingleDocTemplate::RemoveDocument( CDocument *pDoc )
/********************************************************/
{
    CDocTemplate::RemoveDocument( pDoc );

    ASSERT( m_pOnlyDoc == pDoc );
    m_pOnlyDoc = NULL;
}

void CSingleDocTemplate::SetDefaultTitle( CDocument *pDocument )
/**************************************************************/
{
    ASSERT( pDocument != NULL );
    CString strTitle;
    if( !GetDocString( strTitle, docName ) || strTitle.IsEmpty() ) {
        strTitle.LoadString( AFX_IDS_UNTITLED );
    }
    pDocument->SetTitle( strTitle );
}

#ifdef _DEBUG

void CSingleDocTemplate::Dump( CDumpContext &dc ) const
/*****************************************************/
{
    CDocTemplate::Dump( dc );

    dc << "m_pOnlyDoc = " << (void *)m_pOnlyDoc << "\n";
}

#endif // _DEBUG
