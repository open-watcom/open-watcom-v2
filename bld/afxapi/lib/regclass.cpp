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
* Description:  Implementation of AfxRegisterClass and AfxRegisterWndClass.
*
****************************************************************************/


#include "stdafx.h"

// Define _sntprintf if it's not in tchar.h.
#ifndef _sntprintf
    #ifdef _UNICODE
        #define _sntprintf  _snwprintf
    #else
        #define _sntprintf  snprintf
    #endif
#endif

BOOL AfxRegisterClass( WNDCLASS *lpWndClass )
/*******************************************/
{
    if( !::RegisterClass( lpWndClass ) ) {
        return( FALSE );
    }

    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    if( pState->m_bDLL ) {
        pState->m_strUnregisterList += lpWndClass->lpszClassName;
        pState->m_strUnregisterList += _T("\n");
    }

    return( TRUE );
}

LPCTSTR AfxRegisterWndClass( UINT nClassStyle, HCURSOR hCursor, HBRUSH hbrBackground,
                             HICON hIcon )
/****************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    if( hCursor == NULL && hbrBackground == NULL && hIcon == NULL ) {
        _sntprintf( pState->m_szTempClassName, _AFX_TEMP_CLASS_NAME_SIZE - 1,
                    _T("Afx:%p:%x"), AfxGetInstanceHandle(), nClassStyle );
    } else {
        _sntprintf( pState->m_szTempClassName, _AFX_TEMP_CLASS_NAME_SIZE - 1,
                    _T("Afx:%p:%x:%p:%p:%p"), AfxGetInstanceHandle(), nClassStyle,
                    hCursor, hbrBackground, hIcon );
    }

    WNDCLASS wc;
    if( ::GetClassInfo( AfxGetInstanceHandle(), pState->m_szTempClassName, &wc ) ) {
        return( pState->m_szTempClassName );
    }

    if( hCursor == NULL ) {
        hCursor = ::LoadCursor( NULL, IDC_ARROW );
    }
    if( hIcon == NULL ) {
        hIcon = ::LoadIcon( NULL, IDI_WINLOGO );
    }
    
    wc.style = nClassStyle;
    wc.lpfnWndProc = ::DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = AfxGetInstanceHandle();
    wc.hIcon = hIcon;
    wc.hCursor = hCursor;
    wc.hbrBackground = hbrBackground;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = pState->m_szTempClassName;
    if( !AfxRegisterClass( &wc ) ) {
        return( NULL );
    }
    return( pState->m_szTempClassName );
}
