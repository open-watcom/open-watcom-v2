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
* Description:  Implementation of AfxCommDlgProc.
*
****************************************************************************/


#include "stdafx.h"
#include "cdhook.h"
#include "dialog.h"

UINT_PTR CALLBACK AfxCommDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
/***************************************************************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    if( pState->m_pAlternateWndInit != NULL ) {
        pState->m_pAlternateWndInit->SubclassWindow( hWnd );
        pState->m_pAlternateWndInit = NULL;
    }
    if( message == WM_INITDIALOG ) {
        hWnd = hWnd;
        wParam = wParam;
        lParam = lParam;
        return( AfxDlgProc( hWnd, message, wParam, lParam ) );
    }
    return( 0L );
}
