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
* Description:  Implementation of CDocument.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CDocument, CCmdTarget )

BEGIN_MESSAGE_MAP( CDocument, CCmdTarget )
    ON_COMMAND( ID_FILE_CLOSE, OnFileClose )
    ON_COMMAND( ID_FILE_SAVE, OnFileSave )
    ON_COMMAND( ID_FILE_SAVE_AS, OnFileSaveAs )
END_MESSAGE_MAP()

CDocument::CDocument()
/********************/
{
    m_pDocTemplate = NULL;
    m_bModified = FALSE;
    m_bAutoDelete = TRUE;
}

CDocument::~CDocument()
/*********************/
{
    if( m_pDocTemplate != NULL ) {
        m_pDocTemplate->RemoveDocument( this );
    }
}

BOOL CDocument::CanCloseFrame( CFrameWnd *pFrame )
/************************************************/
{
    POSITION position = m_viewList.GetHeadPosition();
    while( position != NULL ) {
        CView *pView = (CView *)m_viewList.GetNext( position );
        if( pView->GetDocument() == this && pView->GetParentFrame() != pFrame ) {
            return( TRUE );
        }
    }
    return( SaveModified() );
}

void CDocument::DeleteContents()
/******************************/
{
}

BOOL CDocument::DoFileSave()
/**************************/
{
    if( m_strPathName.IsEmpty() ) {
        return( DoSave( NULL ) );
    } else {
        return( DoSave( m_strPathName ) );
    }
}

BOOL CDocument::DoSave( LPCTSTR lpszPathName, BOOL bReplace )
/***********************************************************/
{
    CString strPathName;
    if( lpszPathName != NULL ) {
        strPathName = lpszPathName;
    } else {
        CWinApp *pApp = AfxGetApp();
        ASSERT( pApp != NULL );
        if( !pApp->DoPromptFileName( strPathName, AFX_IDS_SAVEFILE, OFN_PATHMUSTEXIST,
                                     FALSE, m_pDocTemplate ) ) {
            return( FALSE );
        }
    }
    if( !OnSaveDocument( strPathName ) ) {
        return( FALSE );
    }
    if( bReplace ) {
        SetPathName( strPathName );
    }
    return( TRUE );
}

CFile *CDocument::GetFile( LPCTSTR lpszFileName, UINT nOpenFlags,
                           CFileException *pError )
/*************************************************/
{
    CFile *pFile = new CFile;
    if( !pFile->Open( lpszFileName, nOpenFlags, pError ) ) {
        delete pFile;
        return( NULL );
    }
    return( pFile );
}

POSITION CDocument::GetFirstViewPosition() const
/**********************************************/
{
    return( m_viewList.GetHeadPosition() );
}

CView *CDocument::GetNextView( POSITION &rPosition ) const
/********************************************************/
{
    return( (CView *)m_viewList.GetNext( rPosition ) );
}

BOOL CDocument::IsModified()
/**************************/
{
    return( m_bModified );
}

void CDocument::OnChangedViewList()
/*********************************/
{
    if( m_bAutoDelete && GetFirstViewPosition() == NULL ) {
        OnCloseDocument();
    } else {
        UpdateFrameCounts();
    }
}

void CDocument::OnCloseDocument()
/*******************************/
{
    POSITION position;
    BOOL bOldAutoDelete = m_bAutoDelete;
    m_bAutoDelete = FALSE;
    while( (position = GetFirstViewPosition()) != NULL ) {
        CView *pView = GetNextView( position );
        ASSERT( pView != NULL );
        CFrameWnd *pFrame = pView->GetParentFrame();
        ASSERT( pFrame != NULL );
        pFrame->DestroyWindow();
    }
    DeleteContents();
    if( bOldAutoDelete ) { 
        delete this;
    }
}

BOOL CDocument::OnNewDocument()
/*****************************/
{
    DeleteContents();
    m_strPathName.Empty();
    m_strTitle.Empty();
    SetModifiedFlag( FALSE );
    return( TRUE );
}

BOOL CDocument::OnOpenDocument( LPCTSTR lpszPathName )
/****************************************************/
{
    CFileException ex;
    CFile *pFile = GetFile( lpszPathName, CFile::modeRead, &ex );
    if( pFile == NULL ) {
        ReportSaveLoadException( lpszPathName, &ex, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC );
        return( FALSE );
    }
    try {
        DeleteContents();
        CArchive ar( pFile, CArchive::load );
        Serialize( ar );
        ar.Close();
        ReleaseFile( pFile, FALSE );
    } catch( CException *pEx ) {
        ReportSaveLoadException( lpszPathName, pEx, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC );
        ReleaseFile( pFile, TRUE );
        return( FALSE );
    }
    SetModifiedFlag( FALSE );
    return( TRUE );
}

BOOL CDocument::OnSaveDocument( LPCTSTR lpszPathName )
/****************************************************/
{
    CFileException ex;
    CFile *pFile = GetFile( lpszPathName, CFile::modeCreate | CFile::modeWrite, &ex );
    if( pFile == NULL ) {
        ReportSaveLoadException( lpszPathName, &ex, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC );
        return( FALSE );
    }
    try {
        CArchive ar( pFile, CArchive::store );
        Serialize( ar );
        ar.Close();
        SetPathName( lpszPathName );
        SetTitle( pFile->GetFileTitle() );
        ReleaseFile( pFile, FALSE );
    } catch( CException *pEx ) {
        ReportSaveLoadException( lpszPathName, pEx, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC );
        ReleaseFile( pFile, TRUE );
        return( FALSE );
    }
    SetModifiedFlag( FALSE );
    return( TRUE );
}

void CDocument::PreCloseFrame( CFrameWnd *pFrame )
/************************************************/
{
    UNUSED_ALWAYS( pFrame );
}

void CDocument::ReleaseFile( CFile *pFile, BOOL bAbort )
/******************************************************/
{
    if( bAbort ) {
        pFile->Abort();
    } else {
        pFile->Close();
    }
    delete pFile;
}

void CDocument::ReportSaveLoadException( LPCTSTR lpszPathName, CException *e,
                                         BOOL bSaving, UINT nIDPDefault )
/***********************************************************************/
{
    UNUSED_ALWAYS( lpszPathName );
    UNUSED_ALWAYS( bSaving );
    
    TCHAR szMessage[256];
    if( e->GetErrorMessage( szMessage, 256 ) ) {
        AfxMessageBox( szMessage );
    } else {
        AfxMessageBox( nIDPDefault );
    }
}

BOOL CDocument::SaveModified()
/****************************/
{
    if( !IsModified() ) {
        return( TRUE );
    }

    CString strMessage;
    strMessage.Format( AFX_IDP_ASK_TO_SAVE, (LPCTSTR)GetTitle() );
    int nRet = AfxMessageBox( strMessage, MB_YESNOCANCEL );
    if( nRet == IDYES ) {
        return( DoFileSave() );
    } else if( nRet == IDNO ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

void CDocument::SetModifiedFlag( BOOL bModified )
/***********************************************/
{
    m_bModified = bModified;
}

void CDocument::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU )
/*****************************************************************/
{
    TCHAR szFullPath[MAX_PATH];
    if( ::GetFullPathName( lpszPathName, MAX_PATH, szFullPath, NULL ) == 0 ) {
        throw new CFileException( CFileException::badPath );
    }
    m_strPathName = szFullPath;

    TCHAR szFileTitle[MAX_PATH];
    if( ::GetFileTitle( lpszPathName, szFileTitle, MAX_PATH ) != 0 ) {
        SetTitle( lpszPathName );
    } else {
        SetTitle( szFileTitle );
    }

    if( bAddToMRU ) {
        CWinApp *pApp = AfxGetApp();
        ASSERT( pApp != NULL );
        pApp->AddToRecentFileList( szFullPath );
    }
}

void CDocument::SetTitle( LPCTSTR lpszTitle )
/*******************************************/
{
    m_strTitle = lpszTitle;
    UpdateFrameCounts();
}

void CDocument::UpdateFrameCounts()
/*********************************/
{
    POSITION position = GetFirstViewPosition();
    while( position != NULL ) {
        CView *pView = GetNextView( position );
        ASSERT_VALID( pView );
        CFrameWnd *pFrame = pView->GetParentFrame();
        if( pFrame != NULL ) {
            pFrame->m_nWindow = -1;
        }
    }

    int nCount = 0;
    position = GetFirstViewPosition();
    while( position != NULL ) {
        CView *pView = GetNextView( position );
        ASSERT_VALID( pView );
        CFrameWnd *pFrame = pView->GetParentFrame();
        if( pFrame != NULL && pFrame->m_nWindow < 0 ) {
            nCount++;
            pFrame->m_nWindow = nCount;
        }
    }

    position = GetFirstViewPosition();
    while( position != NULL ) {
        CView *pView = GetNextView( position );
        ASSERT_VALID( pView );
        CFrameWnd *pFrame = pView->GetParentFrame();
        if( pFrame != NULL ) {
            if( nCount == 1 ) {
                pFrame->m_nWindow = 0;
            }
            pFrame->OnUpdateFrameTitle( TRUE );
        }
    }
}

#ifdef _DEBUG

void CDocument::AssertValid() const
/*********************************/
{
    CCmdTarget::AssertValid();

    POSITION position = GetFirstViewPosition();
    while( position != NULL ) {
        CView *pView = GetNextView( position );
        ASSERT_VALID( pView );
    }
}

void CDocument::Dump( CDumpContext &dc ) const
/********************************************/
{
    CCmdTarget::Dump( dc );

    dc << "m_strTitle = " << m_strTitle << "\n";
    dc << "m_strPathName = " << m_strPathName << "\n";
    dc << "m_pDocTemplate = " << (void *)m_pDocTemplate << "\n";
    dc << "m_bModified = " << m_bModified << "\n";
    dc << "m_bAutoDelete = " << m_bAutoDelete << "\n";
}

#endif // _DEBUG

void CDocument::AddView( CView *pView )
/*************************************/
{
    m_viewList.AddTail( pView );
    pView->m_pDocument = this;
    OnChangedViewList();
}

void CDocument::RemoveView( CView *pView )
/****************************************/
{
    POSITION position = m_viewList.Find( pView );
    if( position != NULL ) {
        m_viewList.RemoveAt( position );
        OnChangedViewList();
    }
}

void CDocument::UpdateAllViews( CView *pSender, LPARAM lHint, CObject *pHint )
/****************************************************************************/
{
    POSITION position = m_viewList.GetHeadPosition();
    while( position != NULL ) {
        CView *pView = (CView *)m_viewList.GetNext( position );
        pView->OnUpdate( pSender, lHint, pHint );
    }
}

void CDocument::OnFileClose()
/***************************/
{
    if( SaveModified() ) {
        OnCloseDocument();
    }
}

void CDocument::OnFileSave()
/**************************/
{
    DoFileSave();
}

void CDocument::OnFileSaveAs()
/****************************/
{
    DoSave( NULL );
}
