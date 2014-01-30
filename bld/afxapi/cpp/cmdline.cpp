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
* Description:  Implementation of CCommandLineInfo.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CCommandLineInfo, CObject )

CCommandLineInfo::CCommandLineInfo()
/**********************************/
{
    m_bShowSplash = TRUE;
    m_bRunAutomated = FALSE;
    m_bRunEmbedded = FALSE;
    m_nShellCommand = FileNew;
}

void CCommandLineInfo::ParseParam( const TCHAR *pszParam, BOOL bFlag, BOOL bLast )
/********************************************************************************/
{
    UNUSED_ALWAYS( bLast );
    if( bFlag ) {
        if( _tcsicmp( pszParam, _T("p") ) == 0 ) {
            m_nShellCommand = FilePrint;
        } else if( _tcsicmp( pszParam, _T("pt") ) == 0 ) {
            m_nShellCommand = FilePrintTo;
        } else if( _tcsicmp( pszParam, _T("dde") ) == 0 ) {
            m_nShellCommand = FileDDE;
        } else if( _tcsicmp( pszParam, _T("automation") ) == 0 ) {
            m_bRunAutomated = TRUE;
        } else if( _tcsicmp( pszParam, _T("embedding") ) == 0) {
            m_bRunEmbedded = TRUE;
        }
    } else {
        if( m_strFileName.IsEmpty() ) {
            m_strFileName = pszParam;
            if( m_nShellCommand == FileNew ) {
                m_nShellCommand = FileOpen;
            }
        } else if( m_nShellCommand == FilePrintTo ) {
            if( m_strPrinterName.IsEmpty() ) {
                m_strPrinterName = pszParam;
            } else if( m_strDriverName.IsEmpty() ) {
                m_strDriverName = pszParam;
            } else if( m_strPortName.IsEmpty() ) {
                m_strPortName = pszParam;
            }
        }
    }
}

#ifdef _DEBUG

void CCommandLineInfo::Dump( CDumpContext &dc ) const
{
    CObject::Dump( dc );

    dc << "m_bShowSplash = " << m_bShowSplash << "\n";
    dc << "m_bRunAutomated = " << m_bRunAutomated << "\n";
    dc << "m_bRunEmbedded = " << m_bRunEmbedded << "\n";
    dc << "m_nShellCommand = ";
    switch( m_nShellCommand ) {
    case FileNew:
        dc << "FileNew\n";
        break;
    case FileOpen:
        dc << "FileOpen\n";
        break;
    case FilePrint:
        dc << "FilePrint\n";
        break;
    case FilePrintTo:
        dc << "FilePrintTo\n";
        break;
    case FileDDE:
        dc << "FileDDE\n";
        break;
    case FileNothing:
        dc << "FileNothing\n";
        break;
    default:
        dc << m_nShellCommand << "\n";
        break;
    }
    dc << "m_strFileName = " << m_strFileName << "\n";
    dc << "m_strPrinterName = " << m_strPrinterName << "\n";
    dc << "m_strDriverName = " << m_strDriverName << "\n";
    dc << "m_strPortName = " << m_strPortName << "\n";
}

#endif // _DEBUG
