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
* Description:  Implementation of AFX_MODULE_STATE and AfxGetModuleState.
*
****************************************************************************/


#include "stdafx.h"

// Base module state class
class _AFX_BASE_MODULE_STATE : public AFX_MODULE_STATE {
public:
    _AFX_BASE_MODULE_STATE() : AFX_MODULE_STATE( FALSE ) {}
};

// Instantiate the one and only base module state object.
CProcessLocal< _AFX_BASE_MODULE_STATE > _afxBaseModuleState;

AFX_MODULE_STATE::AFX_MODULE_STATE( BOOL bDLL )
/*********************************************/
{
    m_pCurrentWinApp = NULL;
    m_hCurrentInstanceHandle = NULL;
    m_hCurrentResourceHandle = NULL;
    m_lpszCurrentAppName = NULL;
    m_bDLL = (BYTE)bDLL;
    m_classList.Construct( (int)&((CRuntimeClass *)0)->m_pNextClass );
    m_pfnFilterToolTipMessage = NULL;
#ifdef _AFXDLL
    m_libraryList.Construct( (int)&((CDynLinkLibrary *)0)->m_pNextDLL );
#endif
}

AFX_MODULE_STATE::~AFX_MODULE_STATE()
/***********************************/
{
}

AFX_MODULE_STATE * AFXAPI AfxGetModuleState()
/*******************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    if( pState->m_pModuleState != NULL ) {
        return( pState->m_pModuleState );
    } else {
        return( _afxBaseModuleState );
    }
}
