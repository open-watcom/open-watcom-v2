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
* Description:  Implementation of CTimeSpan.
*
****************************************************************************/


#include "stdafx.h"

CString CTimeSpan::Format( LPCTSTR lpszFormat )
/*********************************************/
{
    CString str;
    TCHAR   szBuff[12];
    while( *lpszFormat != _T('\0') ) {
        if( *lpszFormat == _T('%') ) {
            lpszFormat++;
            switch( *lpszFormat ) {
            case _T('D'):
                _stprintf( szBuff, _T("%d"), GetDays() );
                break;
            case _T('H'):
                _stprintf( szBuff, _T("%d"), GetHours() );
                break;
            case _T('M'):
                _stprintf( szBuff, _T("%d"), GetMinutes() );
                break;
            case _T('S'):
                _stprintf( szBuff, _T("%d"), GetSeconds() );
                break;
            case _T('%'):
                szBuff[0] = _T('%');
                szBuff[1] = _T('\0');
                break;
            default:
                szBuff[0] = _T('\0');
                break;
            }
            str += szBuff;
        } else {
            str.AppendChar( *lpszFormat );
        }
        lpszFormat++;
    }
    return( str );
}
