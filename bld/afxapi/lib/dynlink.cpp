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
* Description:  Implementation of the entry point for the shared DLL
*               version of the library, as well as CDynLinkLibrary,
*               AfxInitExtensionModule, AfxTermExtensionModule, and
*               AfxFindResourceHandle.
*
****************************************************************************/


#include "stdafx.h"

static AFX_EXTENSION_MODULE _AFXModule;

BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
/***************************************************************************/
{
    UNUSED_ALWAYS( lpReserved );
    if( dwReason == DLL_PROCESS_ATTACH ) {
        // Add the AFX library to the list of modules searched for resources.
        AfxInitExtensionModule( _AFXModule, hInstance );
        new CDynLinkLibrary( _AFXModule, TRUE );
    } else {
        // Delete the CDynLinkLibrary object for the AFX library, as well as any
        // others that are left.
        AfxTermExtensionModule( _AFXModule, TRUE );
    }
    return( TRUE );
}

IMPLEMENT_DYNAMIC( CDynLinkLibrary, CCmdTarget )

CDynLinkLibrary::CDynLinkLibrary( AFX_EXTENSION_MODULE &state, BOOL bSystem )
/***************************************************************************/
{
    m_hModule = state.hModule;
    m_hResource = state.hResource;
    m_bSystem = bSystem;

    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    pState->m_libraryList.AddHead( this );
}

CDynLinkLibrary::CDynLinkLibrary( HINSTANCE hModule, HINSTANCE hResource )
/************************************************************************/
{
    m_hModule = hModule;
    m_hResource = hResource;
    m_bSystem = FALSE;

    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    pState->m_libraryList.AddHead( this );
}

BOOL AFXAPI AfxInitExtensionModule( AFX_EXTENSION_MODULE &state, HMODULE hModule )
/********************************************************************************/
{
    if( !state.bInitialized ) {
        state.bInitialized = TRUE;
        state.hModule = hModule;
        state.hResource = hModule;
    }
    return( TRUE );
}

void AFXAPI AfxTermExtensionModule( AFX_EXTENSION_MODULE &state, BOOL bAll )
/**************************************************************************/
{
    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );

    CDynLinkLibrary *pDLL = pState->m_libraryList.GetHead();
    while( pDLL != NULL ) {
        CDynLinkLibrary *pNextDLL = pState->m_libraryList.GetNext( pDLL );
        if( bAll ) {
            pState->m_libraryList.Remove( pDLL );
        } else if( pDLL->m_hModule == state.hModule ) {
            pState->m_libraryList.Remove( pDLL );
            break;
        }
        pDLL = pNextDLL;
    }
}

HINSTANCE AFXAPI AfxFindResourceHandle( LPCTSTR lpszName, LPCTSTR lpszType )
/**************************************************************************/
{
    // Check the main resource handle.
    HINSTANCE hInstance = AfxGetResourceHandle();
    if( ::FindResource( hInstance, lpszName, lpszType ) != NULL ) {
        return( hInstance );
    }

    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );

    // Check all of the non-system libraries.
    CDynLinkLibrary *pDLL = pState->m_libraryList.GetHead();
    while( pDLL != NULL ) {
        if( !pDLL->m_bSystem ) {
            hInstance = pDLL->m_hResource;
            if( ::FindResource( hInstance, lpszName, lpszType ) != NULL ) {
                return( hInstance );
            }
        }
        pDLL = pState->m_libraryList.GetNext( pDLL );
    }

    // Check all of the system libraries.
    pDLL = pState->m_libraryList.GetHead();
    while( pDLL != NULL ) {
        if( pDLL->m_bSystem ) {
            hInstance = pDLL->m_hResource;
            if( ::FindResource( hInstance, lpszName, lpszType ) != NULL ) {
                return( hInstance );
            }
        }
        pDLL = pState->m_libraryList.GetNext( pDLL );
    }

    // If all else fails, just default to the main resource handle.
    return( AfxGetResourceHandle() );
}
