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
* Description:  Implemention of _AFX_EDIT_STATE and _AFX_RICHEDIT_STATE.
*
****************************************************************************/


#include "stdafx.h"
#include "edstate.h"

_AFX_EDIT_STATE::_AFX_EDIT_STATE()
/********************************/
{
    pFindReplaceDlg = NULL;
    bFindOnly = TRUE;
    bCase = FALSE;
    bNext = TRUE;
    bWord = FALSE;
}

_AFX_EDIT_STATE::~_AFX_EDIT_STATE()
/*********************************/
{
}

_AFX_RICHEDIT_STATE::_AFX_RICHEDIT_STATE()
/****************************************/
{
    m_hInstRichEdit = NULL;
    m_hInstRichEdit2 = NULL;
}

_AFX_RICHEDIT_STATE::~_AFX_RICHEDIT_STATE()
/*****************************************/
{
    if( m_hInstRichEdit != NULL ) {
        ::FreeLibrary( m_hInstRichEdit );
    }
    if( m_hInstRichEdit2 != NULL ) {
        ::FreeLibrary( m_hInstRichEdit2 );
    }
}

// Instantiate the one and only rich edit state object.
CProcessLocal< _AFX_RICHEDIT_STATE > _afxRichEditState;
