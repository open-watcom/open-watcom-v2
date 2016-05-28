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
* Description:  Implementation of AfxWinInit.
*
****************************************************************************/


#include "stdafx.h"

BOOL AFXAPI AfxWinInit( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,
                        int nCmdShow )
/*************************************************************************************/
{
    UNUSED_ALWAYS( hPrevInstance );

    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    pState->m_hCurrentInstanceHandle = hInstance;
    pState->m_hCurrentResourceHandle = hInstance;

    WNDCLASS wc;
    wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ::DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = ::LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _T("AfxWnd" );
    AfxRegisterClass( &wc );
    wc.hIcon = ::LoadIcon( NULL, IDI_WINLOGO );
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("AfxFrameOrView");
    AfxRegisterClass( &wc );
    wc.style = CS_DBLCLKS;
    wc.hbrBackground = NULL;
    wc.lpszClassName = _T("AfxMDIFrame");
    AfxRegisterClass( &wc );
    wc.style = 0;
    wc.hIcon = NULL;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = _T("AfxControlBar");
    AfxRegisterClass( &wc );
    
    CWinApp *pApp = AfxGetApp();
    if( pApp != NULL ) {
        pApp->m_hInstance = hInstance;
        pApp->m_lpCmdLine = lpCmdLine;
        pApp->m_nCmdShow = nCmdShow;

        TCHAR szBuff[MAX_PATH + 1];
        ::GetModuleFileName( hInstance, szBuff, MAX_PATH );
        TCHAR *pszFile = _tcsrchr( szBuff, _T('\\') );
        if( pszFile != NULL ) {
            pszFile++;
        } else {
            pszFile = szBuff;
        }
        TCHAR *pszExt = _tcsrchr( pszFile, _T('.') );
        if( pszExt != NULL ) {
            *pszExt = _T('\0');
        }
        pApp->m_pszExeName = _tcsdup( pszFile );
        if( pApp->m_eHelpType == afxHTMLHelp ) {
            _tcscat( pszFile, _T(".CHM") );
        } else {
            _tcscat( pszFile, _T(".HLP") );
        }
        pApp->m_pszHelpFilePath = _tcsdup( pszFile );
        if( pApp->m_pszAppName == NULL ) {
            TCHAR szAppName[256];
            if( ::LoadString( hInstance, AFX_IDS_APP_TITLE, szAppName, 256 ) > 0 ) {
                pApp->m_pszAppName = _tcsdup( szAppName );
            } else {
                pApp->m_pszAppName = _tcsdup( pApp->m_pszExeName );
            }
        }
        pState->m_lpszCurrentAppName = pApp->m_pszAppName;
    }
    
    return( TRUE );
}
