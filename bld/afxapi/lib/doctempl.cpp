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
* Description:  Implementation of CDocTemplate.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CDocTemplate, CCmdTarget )

CDocTemplate::CDocTemplate( UINT nIDResource, CRuntimeClass *pDocClass,
                            CRuntimeClass *pFrameClass, CRuntimeClass *pViewClass )
/*********************************************************************************/
{
    ASSERT( pDocClass != NULL );
    ASSERT( pFrameClass != NULL );
    ASSERT( pViewClass != NULL );
    m_nIDResource = nIDResource;
    m_pDocClass = pDocClass;
    m_pFrameClass = pFrameClass;
    m_pViewClass = pViewClass;
}

void CDocTemplate::AddDocument( CDocument *pDoc )
/***********************************************/
{
    ASSERT( pDoc != NULL );
    ASSERT( pDoc->m_pDocTemplate == NULL );
    pDoc->m_pDocTemplate = this;
}

void CDocTemplate::CloseAllDocuments( BOOL bEndSession )
/******************************************************/
{
    bEndSession = bEndSession;

    POSITION position = GetFirstDocPosition();
    while( position != NULL ) {
        CDocument *pDoc = GetNextDoc( position );
        ASSERT( pDoc != NULL );
        pDoc->OnCloseDocument();
    }
}

CDocument *CDocTemplate::CreateNewDocument()
/******************************************/
{
    ASSERT( m_pDocClass != NULL );
    CDocument *pDoc = (CDocument *)m_pDocClass->CreateObject();
    if( pDoc == NULL ) {
        return( NULL );
    }
    ASSERT( pDoc->IsKindOf( RUNTIME_CLASS( CDocument ) ) );
    AddDocument( pDoc );
    return( pDoc );
}

CFrameWnd *CDocTemplate::CreateNewFrame( CDocument *pDoc, CFrameWnd *pOther )
/***************************************************************************/
{
    ASSERT( m_pFrameClass != NULL );
    CFrameWnd *pFrame = (CFrameWnd *)m_pFrameClass->CreateObject();
    if( pFrame == NULL ) {
        return( NULL );
    }
    ASSERT( pFrame->IsKindOf( RUNTIME_CLASS( CFrameWnd ) ) );

    CCreateContext cc;
    cc.m_pNewViewClass = m_pViewClass;
    cc.m_pCurrentDoc = pDoc;
    cc.m_pNewDocTemplate = this;
    cc.m_pLastView = NULL;
    cc.m_pCurrentFrame = pOther;
    if( !pFrame->LoadFrame( m_nIDResource, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
                            NULL, &cc ) ) {
        return( NULL );
    }
    return( pFrame );
}

BOOL CDocTemplate::GetDocString( CString &rString,
                                 enum CDocTemplate::DocStringIndex index ) const
/******************************************************************************/
{
    return( AfxExtractSubString( rString, m_strDocStrings, index ) );
}

void CDocTemplate::InitialUpdateFrame( CFrameWnd *pFrame, CDocument *pDoc,
                                       BOOL bMakeVisible )
/********************************************************/
{
    ASSERT( pFrame != NULL );
    pFrame->InitialUpdateFrame( pDoc, bMakeVisible );
}

void CDocTemplate::LoadTemplate()
/*******************************/
{
    if( m_strDocStrings.IsEmpty() ) {
        m_strDocStrings.LoadString( m_nIDResource );
    }
}

CDocTemplate::Confidence CDocTemplate::MatchDocType( LPCTSTR lpszPathName,
                                                     CDocument *&rpDocMatch )
/***************************************************************************/
{
    POSITION position = GetFirstDocPosition();
    while( position != NULL ) {
        CDocument *pDoc = GetNextDoc( position );
        ASSERT( pDoc != NULL );
        if( pDoc->GetPathName() == lpszPathName ) {
            rpDocMatch = pDoc;
            return( yesAlreadyOpen );
        }
    }

    CString strExt;
    if( !GetDocString( strExt, filterExt ) || strExt.IsEmpty() ) {
        return( yesAttemptForeign );
    }

    LPCTSTR lpszPathExt = _tcsrchr( lpszPathName, _T('.') );
    if( lpszPathExt != NULL && strExt.CompareNoCase( lpszPathExt ) == 0 ) {
        return( yesAttemptNative );
    }
    return( yesAttemptForeign );
}

void CDocTemplate::RemoveDocument( CDocument *pDoc )
/**************************************************/
{
    ASSERT( pDoc != NULL );
    ASSERT( pDoc->m_pDocTemplate == this );
    pDoc->m_pDocTemplate = NULL;
}

BOOL CDocTemplate::SaveAllModified()
/**********************************/
{
    POSITION position = GetFirstDocPosition();
    while( position != NULL ) {
        CDocument *pDoc = GetNextDoc( position );
        ASSERT( pDoc != NULL );
        if( !pDoc->SaveModified() ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

#ifdef _DEBUG

void CDocTemplate::AssertValid() const
/************************************/
{
    CCmdTarget::AssertValid();

    ASSERT( m_pDocClass != NULL );
    ASSERT( m_pFrameClass != NULL );
    ASSERT( m_pViewClass != NULL );

    POSITION position = GetFirstDocPosition();
    while( position != NULL ) {
        CDocument *pDoc = GetNextDoc( position );
        ASSERT_VALID( pDoc );
    }
}

void CDocTemplate::Dump( CDumpContext &dc ) const
/***********************************************/
{
    CCmdTarget::Dump( dc );

    dc << "m_nIDResource = " << m_nIDResource << "\n";
    dc << "m_pDocClass = " << m_pDocClass->m_lpszClassName << "\n";
    dc << "m_pFrameClass = " << m_pFrameClass->m_lpszClassName << "\n";
    dc << "m_pViewClass = " << m_pViewClass->m_lpszClassName << "\n";
    dc << "m_strDocStrings = " << m_strDocStrings << "\n";
}

#endif // _DEBUG
