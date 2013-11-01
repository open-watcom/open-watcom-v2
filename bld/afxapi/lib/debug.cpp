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
* Description:  Implementation of AfxAssertFailedLine, AfxAssertValidObject,
*               and AfxTrace.
*
****************************************************************************/


#include "stdafx.h"

#ifdef _DEBUG

BOOL AFXAPI AfxAssertFailedLine( LPCSTR lpszFileName, int nLine )
/***************************************************************/
{
    CString strMessage( _T("Assertion failed!\n\nFile: ") );
    strMessage += lpszFileName;
    strMessage.AppendFormat( _T("\nLine: %d\n\n"), nLine );
    strMessage += _T("Select Abort to terminate the program.\n");
    strMessage += _T("Select Retry to debug.\n");
    strMessage += _T("Select Ignore to continue execution.");
    int nResult = ::MessageBox( NULL, strMessage, NULL, MB_ABORTRETRYIGNORE );
    if( nResult == IDABORT ) {
        AfxAbort();
    } else if( nResult == IDRETRY ) {
        return( TRUE );
    }
    return( FALSE );
}

void AFXAPI AfxAssertValidObject( CObject *pObject, LPCSTR lpszFileName, int nLine )
/**********************************************************************************/
{
    if( pObject == NULL ) {
        if( AfxAssertFailedLine( lpszFileName, nLine ) ) {
            AfxDebugBreak();
        }
        return;
    }
    if( ::IsBadReadPtr( pObject, sizeof( CObject ) ) ||
        ::IsBadWritePtr( pObject, sizeof( CObject ) ) ) {
        if( AfxAssertFailedLine( lpszFileName, nLine ) ) {
            AfxDebugBreak();
        }
        return;
    }
    CRuntimeClass *pClass = pObject->GetRuntimeClass();
    if( pClass == NULL ) {
        if( AfxAssertFailedLine( lpszFileName, nLine ) ) {
            AfxDebugBreak();
        }
        return;
    }
    int nObjectSize = pClass->m_nObjectSize;
    if( ::IsBadReadPtr( pObject, nObjectSize ) ||
        ::IsBadWritePtr( pObject, nObjectSize ) ) {
        if( AfxAssertFailedLine( lpszFileName, nLine ) ) {
            AfxDebugBreak();
        }
        return;
    }
    pObject->AssertValid();
}

#endif // _DEBUG

void __cdecl AfxTrace( LPCTSTR lpszFormat, ... )
/**********************************************/
{
#ifdef _DEBUG
    va_list args;
    TCHAR   szBuff[1024];
    va_start( args, lpszFormat );
    _vstprintf( szBuff, lpszFormat, args );
    ::OutputDebugString( szBuff );
    va_end( args );
#else
    lpszFormat = lpszFormat;
#endif
}
