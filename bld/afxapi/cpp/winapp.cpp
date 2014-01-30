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
* Description:  Implementation of CWinApp and AfxMessageBox.
*
****************************************************************************/


#include "stdafx.h"

static const TCHAR _MRUSection[] = _T("Recent File List");
static const TCHAR _MRUEntry[] = _T("File%d");
static const TCHAR _PreviewSection[] = _T("Settings");
static const TCHAR _PreviewEntry[] = _T("PreviewPages");

IMPLEMENT_DYNAMIC( CWinApp, CWinThread )

BEGIN_MESSAGE_MAP( CWinApp, CWinThread )
    ON_COMMAND( ID_APP_EXIT, OnAppExit )
    ON_COMMAND_EX_RANGE( ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile )
    ON_UPDATE_COMMAND_UI( ID_FILE_MRU_FILE1, OnUpdateRecentFileMenu )
END_MESSAGE_MAP()

static HKEY _OpenRegistryKey( LPCTSTR lpszKey, LPCTSTR lpszApp, LPCTSTR lpszSection )
/***********************************************************************************/
{
    HKEY hKeySoftware = NULL;
    HKEY hKeyRegKey = NULL;
    HKEY hKeyApp = NULL;
    HKEY hKeySection = NULL;

    ::RegOpenKeyEx( HKEY_CURRENT_USER, _T("SOFTWARE"), 0L, KEY_ALL_ACCESS,
                    &hKeySoftware );
    if( hKeySoftware != NULL ) {
        ::RegCreateKeyEx( hKeySoftware, lpszKey, 0L, NULL, 0L, KEY_ALL_ACCESS, NULL,
                          &hKeyRegKey, NULL );
        if( hKeyRegKey != NULL ) {
            ::RegCreateKeyEx( hKeyRegKey, lpszApp, 0L, NULL, 0L, KEY_ALL_ACCESS, NULL,
                              &hKeyApp, NULL );
            if( hKeyApp != NULL ) {
                ::RegCreateKeyEx( hKeyApp, lpszSection, 0L, NULL, 0L, KEY_ALL_ACCESS,
                                  NULL, &hKeySection, NULL );
            }
        }
    }
    if( hKeySoftware != NULL ) {
        ::RegCloseKey( hKeySoftware );
    }
    if( hKeyRegKey != NULL ) {
        ::RegCloseKey( hKeyRegKey );
    }
    if( hKeyApp != NULL ) {
        ::RegCloseKey( hKeyApp );
    }
    return( hKeySection );
}

CWinApp::CWinApp( LPCTSTR lpszAppName )
/*************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    ASSERT( pState->m_pCurrentWinThread == NULL );
    pState->m_pCurrentWinThread = this;

    AFX_MODULE_STATE *pModuleState = AfxGetModuleState();
    ASSERT( pModuleState != NULL );
    ASSERT( pModuleState->m_pCurrentWinApp == NULL );
    pModuleState->m_pCurrentWinApp = this;
    
    m_hThread = ::GetCurrentThread();
    m_nThreadID = ::GetCurrentThreadId();
    
    m_hInstance = NULL;
    m_lpCmdLine = NULL;
    m_nCmdShow = 0;
    if( lpszAppName != NULL ) {
        m_pszAppName = _tcsdup( lpszAppName );
    } else {
        m_pszAppName = NULL;
    }
    m_pszRegistryKey = NULL;
    m_pDocManager = NULL;
    m_pszExeName = NULL;
    m_pszHelpFilePath = NULL;
    m_pszProfileName = NULL;
    m_eHelpType = afxWinHelp;
    m_hDevMode = NULL;
    m_hDevNames = NULL;
    m_nWaitCursorCount = 0;
    m_hcurWaitCursorRestore = NULL;
    m_pRecentFileList = NULL;
    m_atomApp = 0;
    m_atomSystemTopic = 0;
    m_nNumPreviewPages = 0;
}

CWinApp::~CWinApp()
/*****************/
{
    // Don't close the thread handle in the CWinThread destructor.
    m_hThread = NULL;
    
    if( m_pszAppName != NULL ) {
        free( (void *)m_pszAppName );
    }
    if( m_pszRegistryKey != NULL ) {
        free( (void *)m_pszRegistryKey );
    }
    if( m_pDocManager != NULL ) {
        delete m_pDocManager;
    }
    if( m_pszExeName != NULL ) {
        free( (void *)m_pszExeName );
    }
    if( m_pszHelpFilePath != NULL ) {
        free( (void *)m_pszHelpFilePath );
    }
    if( m_pszProfileName != NULL ) {
        free( (void *)m_pszProfileName );
    }
    if( m_hDevMode != NULL ) {
        ::GlobalFree( m_hDevMode );
    }
    if( m_hDevNames != NULL ) {
        ::GlobalFree( m_hDevNames );
    }
    if( m_atomApp != 0 ) {
        ::GlobalDeleteAtom( m_atomApp );
    }
    if( m_atomSystemTopic != 0 ) {
        ::GlobalDeleteAtom( m_atomSystemTopic );
    }
}

void CWinApp::AddToRecentFileList( LPCTSTR lpszPathName )
/*******************************************************/
{
    if( m_pRecentFileList != NULL ) {
        m_pRecentFileList->Add( lpszPathName );
    }
}

int CWinApp::DoMessageBox( LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt )
/*************************************************************************/
{
    UNUSED_ALWAYS( nIDPrompt );
    return( ::MessageBox( m_pMainWnd->GetSafeHwnd(), lpszPrompt, m_pszAppName, nType ) );
}

void CWinApp::DoWaitCursor( int nCode )
/*************************************/
{
    if( nCode > 0 ) {
        if( m_hcurWaitCursorRestore == NULL ) {
            m_hcurWaitCursorRestore = ::GetCursor();
        }
        m_nWaitCursorCount++;
        HCURSOR hWaitCursor = ::LoadCursor( NULL, IDC_WAIT );
        ::SetCursor( hWaitCursor );
    } else if( nCode == 0 && m_nWaitCursorCount > 0 ) {
        HCURSOR hWaitCursor = ::LoadCursor( NULL, IDC_WAIT );
        ::SetCursor( hWaitCursor );
    } else if( nCode < 0 && m_nWaitCursorCount > 0 ) {
        ASSERT( m_hcurWaitCursorRestore != NULL );
        m_nWaitCursorCount--;
        if( m_nWaitCursorCount == 0 ) {
            ::SetCursor( m_hcurWaitCursorRestore );
            m_hcurWaitCursorRestore = NULL;
        }
    }
}

void CWinApp::HtmlHelp( DWORD_PTR dwData, UINT nCmd )
/***************************************************/
{
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->HtmlHelp( dwData, nCmd );
    }
}

BOOL CWinApp::OnDDECommand( LPTSTR lpszCommand )
/**********************************************/
{
    if( m_pDocManager != NULL ) {
        return( m_pDocManager->OnDDECommand( lpszCommand ) );
    } else {
        return( FALSE );
    }
}

CDocument *CWinApp::OpenDocumentFile( LPCTSTR lpszFileName )
/**********************************************************/
{
    if( m_pDocManager != NULL ) {
        return( m_pDocManager->OpenDocumentFile( lpszFileName ) );
    } else {
        return( NULL );
    }
}

BOOL CWinApp::SaveAllModified()
/*****************************/
{
    if( m_pDocManager != NULL ) {
        return( m_pDocManager->SaveAllModified() );
    } else {
        return( TRUE );
    }
}

void CWinApp::WinHelp( DWORD_PTR dwData, UINT nCmd )
/**************************************************/
{
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->WinHelp( dwData, nCmd );
    }
}

void CWinApp::WinHelpInternal( DWORD_PTR dwData, UINT nCmd )
/**********************************************************/
{
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->WinHelpInternal( dwData, nCmd );
    }
}

int CWinApp::ExitInstance()
/*************************/
{
    SaveStdProfileSettings();
    return( CWinThread::ExitInstance() );
}

#ifdef _DEBUG

void CWinApp::AssertValid() const
/*******************************/
{
    CWinThread::AssertValid();

    ASSERT( m_hInstance != NULL );
    ASSERT( m_eHelpType == afxWinHelp || m_eHelpType == afxHTMLHelp );
}

void CWinApp::Dump( CDumpContext &dc ) const
/******************************************/
{
    CWinThread::Dump( dc );

    dc << "m_hInstance = " << m_hInstance << "\n";
    dc << "m_lpCmdLine = " << m_lpCmdLine << "\n";
    dc << "m_nCmdShow = ";
    switch( m_nCmdShow ) {
    case SW_HIDE:
        dc << "SW_HIDE\n";
        break;
    case SW_SHOWNORMAL:
        dc << "SW_SHOWNORMAL\n";
        break;
    case SW_SHOWMINIMIZED:
        dc << "SW_SHOWMINIMIZED\n";
        break;
    case SW_SHOWMAXIMIZED:
        dc << "SW_SHOWMAXIMIZED\n";
        break;
    case SW_SHOWNOACTIVATE:
        dc << "SW_SHOWNOACTIVATE\n";
        break;
    case SW_SHOW:
        dc << "SW_SHOW\n";
        break;
    case SW_MINIMIZE:
        dc << "SW_MINIMIZE\n";
        break;
    case SW_SHOWMINNOACTIVE:
        dc << "SW_SHOWMINNOACTIVE\n";
        break;
    case SW_SHOWNA:
        dc << "SW_SHOWNA\n";
        break;
    case SW_RESTORE:
        dc << "SW_RESTORE\n";
        break;
    case SW_SHOWDEFAULT:
        dc << "SW_SHOWDEFAULT\n";
        break;
    case SW_FORCEMINIMIZE:
        dc << "SW_FORCEMINIMIZE\n";
        break;
    default:
        dc << m_nCmdShow << "\n";
        break;
    }
    dc << "m_pszAppName = " << m_pszAppName << "\n";
    dc << "m_pszRegistryKey = " << m_pszRegistryKey << "\n";
    dc << "m_pDocManager = " << (void *)m_pDocManager << "\n";
    dc << "m_pszExeName = " << m_pszExeName << "\n";
    dc << "m_pszHelpFilePath = " << m_pszHelpFilePath << "\n";
    dc << "m_pszProfileName = " << m_pszProfileName << "\n";
    dc << "m_eHelpType = ";
    switch( m_eHelpType ) {
    case afxWinHelp:
        dc << "afxWinHelp\n";
        break;
    case afxHTMLHelp:
        dc << "afxHTMLHelp\n";
        break;
    default:
        dc << m_eHelpType << "\n";
        break;
    }
}

#endif // _DEBUG

void CWinApp::AddDocTemplate( CDocTemplate *pTemplate )
/*****************************************************/
{
    if( m_pDocManager == NULL ) {
        m_pDocManager = new CDocManager;
    }
    m_pDocManager->AddDocTemplate( pTemplate );
}

void CWinApp::CloseAllDocuments( BOOL bEndSession )
/*************************************************/
{
    if( m_pDocManager != NULL ) {
        m_pDocManager->CloseAllDocuments( bEndSession );
    }
}

INT_PTR CWinApp::DoPrintDialog( CPrintDialog *pPD )
/*************************************************/
{
    ASSERT( pPD != NULL );
    pPD->m_pd.hDevMode = m_hDevMode;
    pPD->m_pd.hDevNames = m_hDevNames;
    int nResult = pPD->DoModal();
    m_hDevMode = pPD->m_pd.hDevMode;
    m_hDevNames = pPD->m_pd.hDevNames;
    return( nResult );
}

BOOL CWinApp::DoPromptFileName( CString &fileName, UINT nIDSTitle, DWORD lFlags,
                                BOOL bOpenFileDialog, CDocTemplate *pTemplate )
/*****************************************************************************/
{
    if( m_pDocManager == NULL ) {
        return( FALSE );
    }
    return( m_pDocManager->DoPromptFileName( fileName, nIDSTitle, lFlags,
                                             bOpenFileDialog, pTemplate ) );
}

void CWinApp::EnableShellOpen()
/*****************************/
{
    TCHAR   szModuleName[MAX_PATH];
    TCHAR   szShortName[MAX_PATH];
    ::GetModuleFileName( AfxGetInstanceHandle(), szModuleName, MAX_PATH );
    ::GetShortPathName( szModuleName, szShortName, MAX_PATH );

    // Chop off the path and extension.
    LPTSTR lpszFileName = _tcsrchr( szShortName, _T('\\') );
    if( lpszFileName == NULL ) {
        lpszFileName = szShortName;
    } else {
        lpszFileName++;
    }
    LPTSTR lpszExt = _tcsrchr( lpszFileName, _T('.') );
    if( lpszExt != NULL ) {
        *lpszExt = _T('\0');
    }

    m_atomApp = ::GlobalAddAtom( lpszFileName );
    m_atomSystemTopic = ::GlobalAddAtom( _T("system") );
}

POSITION CWinApp::GetFirstDocTemplatePosition() const
/***************************************************/
{
    if( m_pDocManager != NULL ) {
        return( m_pDocManager->GetFirstDocTemplatePosition() );
    } else {
        return( NULL );
    }
}

CDocTemplate *CWinApp::GetNextDocTemplate( POSITION &pos ) const
/**************************************************************/
{
    if( m_pDocManager != NULL ) {
        return( m_pDocManager->GetNextDocTemplate( pos ) );
    } else {
        pos = NULL;
        return( NULL );
    }
}

BOOL CWinApp::GetPrinterDeviceDefaults( PRINTDLG *pPrintDlg )
/***********************************************************/
{
    UpdatePrinterSelection( FALSE );
    if( m_hDevMode == NULL || m_hDevNames == NULL ) {
        return( FALSE );
    }
    pPrintDlg->hDevMode = m_hDevMode;
    pPrintDlg->hDevNames = m_hDevNames;
    return( TRUE );
}
        
UINT CWinApp::GetProfileInt( LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault )
/*********************************************************************************/
{
    if( m_pszRegistryKey != NULL ) {
        HKEY hKey = _OpenRegistryKey( m_pszRegistryKey, m_pszAppName, lpszSection );
        if( hKey == NULL ) {
            return( nDefault );
        }
        DWORD dwType;
        DWORD dwData;
        DWORD cbData = sizeof( DWORD );
        LONG lRet = ::RegQueryValueEx( hKey, lpszEntry, NULL, &dwType, (LPBYTE)&dwData,
                                       &cbData );
        ::RegCloseKey( hKey );
        if( lRet != ERROR_SUCCESS || dwType != REG_DWORD ) {
            return( nDefault );
        }
        ASSERT( cbData == sizeof( DWORD ) );
        return( (UINT)dwData );
    } else {
        return( ::GetPrivateProfileInt( lpszSection, lpszEntry, nDefault,
                                        m_pszProfileName ) );
    }
}

CString CWinApp::GetProfileString( LPCTSTR lpszSection, LPCTSTR lpszEntry,
                                   LPCTSTR lpszDefault )
/******************************************************/
{
    if( m_pszRegistryKey != NULL ) {
        HKEY hKey = _OpenRegistryKey( m_pszRegistryKey, m_pszAppName, lpszSection );
        if( hKey == NULL ) {
            return( lpszDefault );
        }
        DWORD dwType;
        TCHAR szData[256];
        DWORD cbData = 256 * sizeof( TCHAR );
        LONG lRet = ::RegQueryValueEx( hKey, lpszEntry, NULL, &dwType, (LPBYTE)szData,
                                       &cbData );
        if( lRet != ERROR_SUCCESS || dwType != REG_SZ ) {
            return( lpszDefault );
        }
        return( szData );
    } else {
        TCHAR szData[256];
        ::GetPrivateProfileString( lpszSection, lpszEntry, lpszDefault, szData, 255,
                                   m_pszProfileName );
        return( szData );
    }
}

void CWinApp::LoadStdProfileSettings( UINT nMaxMRU )
/**************************************************/
{
    ASSERT( m_pRecentFileList == NULL );
    if( nMaxMRU > 0 ) {
        m_pRecentFileList = new CRecentFileList( ID_FILE_MRU_FILE1, _MRUSection,
                                                 _MRUEntry, nMaxMRU );
        m_pRecentFileList->ReadList();
    }
    GetProfileInt( _PreviewSection, _PreviewEntry, 0 );
}

void CWinApp::ParseCommandLine( CCommandLineInfo &rCmdInfo )
/**********************************************************/
{
    for( int i = 1; i < __argc; i++ ) {
        const TCHAR *pszParam = __targv[i];
        BOOL        bLast = (i == __argc - 1);
        if( pszParam[0] == _T('/') || pszParam[0] == _T('-') ) {
            rCmdInfo.ParseParam( pszParam + 1, TRUE, bLast );
        } else {
            rCmdInfo.ParseParam( pszParam, FALSE, bLast );
        }
    }
}

BOOL CWinApp::ProcessShellCommand( CCommandLineInfo &rCmdInfo )
/*************************************************************/
{
    if( rCmdInfo.m_nShellCommand == CCommandLineInfo::FileNew ) {
        if( !OnCmdMsg( ID_FILE_NEW, 0, NULL, NULL ) ) {
            OnFileNew();
        }
        if( m_pMainWnd == NULL ) {
            return( FALSE );
        }
    } else if( rCmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen ||
               rCmdInfo.m_nShellCommand == CCommandLineInfo::FilePrint ||
               rCmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo ) {
        AfxMessageBox(rCmdInfo.m_strFileName);
        CDocument *pDoc = OpenDocumentFile( rCmdInfo.m_strFileName );
        if( pDoc == NULL ) {
            return( FALSE );
        }
    } else if( rCmdInfo.m_nShellCommand == CCommandLineInfo::FileDDE ) {
        m_nCmdShow = SW_HIDE;
    }
    return( TRUE );
}

void CWinApp::RegisterShellFileTypes( BOOL bCompat )
/**************************************************/
{
    if( m_pDocManager != NULL ) {
        m_pDocManager->RegisterShellFileTypes( bCompat );
    }
}

void CWinApp::SaveStdProfileSettings()
/************************************/
{
    if( m_pRecentFileList != NULL ) {
        m_pRecentFileList->WriteList();
    }
    if( m_nNumPreviewPages != 0 ) {
        WriteProfileInt( _PreviewSection, _PreviewEntry, m_nNumPreviewPages );
    }
}

void CWinApp::SelectPrinter( HANDLE hDevNames, HANDLE hDevMode, BOOL bFreeOld )
/*****************************************************************************/
{
    if( hDevNames != m_hDevNames ) {
        if( m_hDevNames != NULL && bFreeOld ) {
            ::GlobalFree( m_hDevNames );
        }
        m_hDevNames = hDevNames;
    }
    if( hDevMode != m_hDevMode ) {
        if( m_hDevMode != NULL && bFreeOld ) {
            ::GlobalFree( m_hDevMode );
        }
        m_hDevMode = hDevMode;
    }
}

void CWinApp::SetRegistryKey( LPCTSTR lpszRegistryKey )
/*****************************************************/
{
    ASSERT( lpszRegistryKey != NULL );
    if( m_pszRegistryKey != NULL ) {
        free( (void *)m_pszRegistryKey );
    }
    m_pszRegistryKey = _tcsdup( lpszRegistryKey );
}

void CWinApp::SetRegistryKey( UINT nIDRegistryKey )
/*************************************************/
{
    TCHAR       szBuff[256];
    HINSTANCE   hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nIDRegistryKey ),
                                                   RT_STRING );
    ::LoadString( hInstance, nIDRegistryKey, szBuff, 255 );
    SetRegistryKey( szBuff );
}

void CWinApp::UpdatePrinterSelection( BOOL bForceDefaults )
/*********************************************************/
{
    if( m_hDevMode == NULL || m_hDevNames == NULL || bForceDefaults ) {
        if( m_hDevMode != NULL ) {
            ::GlobalFree( m_hDevMode );
            m_hDevMode = NULL;
        }
        if( m_hDevNames != NULL ) {
            ::GlobalFree( m_hDevNames );
            m_hDevNames = NULL;
        }
        CPrintDialog dlg( TRUE );
        if( dlg.GetDefaults() ) {
            m_hDevMode = dlg.m_pd.hDevMode;
            m_hDevNames = dlg.m_pd.hDevNames;

            // Don't delete the new handles in the CPrintDialog destructor.
            dlg.m_pd.hDevMode = NULL;
            dlg.m_pd.hDevNames = NULL;
        }
    }
}

BOOL CWinApp::WriteProfileInt( LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue )
/*********************************************************************************/
{
    if( m_pszRegistryKey != NULL ) {
        HKEY hKey = _OpenRegistryKey( m_pszRegistryKey, m_pszAppName, lpszSection );
        if( hKey == NULL ) {
            return( FALSE );
        }
        LONG lRet = ::RegSetValueEx( hKey, lpszEntry, 0L, REG_DWORD,
                                     (const BYTE *)&nValue, sizeof( DWORD ) );
        ::RegCloseKey( hKey );
        return( lRet == ERROR_SUCCESS );
    } else {
        TCHAR szValue[12];
        _stprintf( szValue, _T("%d"), nValue );
        return( ::WritePrivateProfileString( lpszSection, lpszEntry, szValue,
                                             m_pszProfileName ) );
    }
}

BOOL CWinApp::WriteProfileString( LPCTSTR lpszSection, LPCTSTR lpszEntry,
                                  LPCTSTR lpszValue )
/***************************************************/
{
    if( m_pszRegistryKey != NULL ) {
        HKEY hKey = _OpenRegistryKey( m_pszRegistryKey, m_pszAppName, lpszSection );
        if( hKey == NULL ) {
            return( FALSE );
        }
        LONG lRet = ::RegSetValueEx( hKey, lpszEntry, 0L, REG_SZ,
                                     (const BYTE *)lpszValue,
                                     (_tcslen( lpszValue ) + 1) * sizeof( TCHAR ) );
        ::RegCloseKey( hKey );
        return( lRet == ERROR_SUCCESS );
    } else {
        return( ::WritePrivateProfileString( lpszSection, lpszEntry, lpszValue,
                                             m_pszProfileName ) );
    }
}

void CWinApp::OnContextHelp()
/***************************/
{
    if( m_pMainWnd != NULL && m_pMainWnd->IsFrameWnd() ) {
        ((CFrameWnd *)m_pMainWnd)->OnContextHelp();
    }
}

void CWinApp::OnFileNew()
/***********************/
{
    if( m_pDocManager != NULL ) {
        m_pDocManager->OnFileNew();
    }
}

void CWinApp::OnFileOpen()
/************************/
{
    if( m_pDocManager != NULL ) {
        m_pDocManager->OnFileOpen();
    }
}

void CWinApp::OnFilePrintSetup()
/******************************/
{
    CPrintDialog dlg( TRUE );
    DoPrintDialog( &dlg );
}

void CWinApp::OnHelp()
/********************/
{
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->OnHelp();
    }
}

void CWinApp::OnHelpFinder()
/**************************/
{
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->OnHelpFinder();
    }
}

void CWinApp::OnHelpIndex()
/*************************/
{
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->OnHelpIndex();
    }
}

void CWinApp::OnHelpUsing()
/*************************/
{
    if( m_pMainWnd != NULL ) {
        m_pMainWnd->OnHelpUsing();
    }
}

void CWinApp::OnAppExit()
/***********************/
{
    ASSERT( m_pMainWnd != NULL );
    m_pMainWnd->SendMessage( WM_CLOSE );
}

BOOL CWinApp::OnOpenRecentFile( UINT nID )
/****************************************/
{
    ASSERT( m_pRecentFileList != NULL );
    OpenDocumentFile( (*m_pRecentFileList)[nID - ID_FILE_MRU_FILE1] );
    return( TRUE );
}

void CWinApp::OnUpdateRecentFileMenu( CCmdUI *pCmdUI )
/****************************************************/
{
    if( m_pRecentFileList != NULL ) {
        m_pRecentFileList->UpdateMenu( pCmdUI );
    } else {
        pCmdUI->Enable( FALSE );
    }
}

int AFXAPI AfxMessageBox( LPCTSTR lpszText, UINT nType, UINT nIDHelp )
/********************************************************************/
{
    CWinApp *pApp = AfxGetApp();
    if( pApp != NULL ) {
        return( pApp->DoMessageBox( lpszText, nType, nIDHelp ) );
    } else {
        return( ::MessageBox( NULL, lpszText, NULL, nType ) );
    }
}

int AFXAPI AfxMessageBox( UINT nIDPrompt, UINT nType, UINT nIDHelp )
/******************************************************************/
{
    TCHAR       szBuff[256];
    HINSTANCE   hInstance = AfxFindResourceHandle( MAKEINTRESOURCE( nIDPrompt ), RT_STRING );
    ::LoadString( hInstance, nIDPrompt, szBuff, 255 );
    CWinApp *pApp = AfxGetApp();
    if( pApp != NULL ) {
        return( pApp->DoMessageBox( szBuff, nType, nIDHelp ) );
    } else {
        return( ::MessageBox( NULL, szBuff, NULL, nType ) );
    }
}
