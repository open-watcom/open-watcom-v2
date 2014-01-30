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
* Description:  Implementation of CCmdTarget.
*
****************************************************************************/


#include "stdafx.h"

// Command handler function pointers
typedef BOOL (CCmdTarget::*PFN_COMMAND_HANDLER_B)();
typedef BOOL (CCmdTarget::*PFN_COMMAND_HANDLER_EX)( UINT );
typedef void (CCmdTarget::*PFN_COMMAND_HANDLER_RANGE)( UINT );
typedef void (CCmdTarget::*PFN_COMMAND_UI_HANDLER)( CCmdUI * );
typedef void (CCmdTarget::*PFN_COMMAND_UI_HANDLER_RANGE)( CCmdUI *, UINT );
typedef void (CCmdTarget::*PFN_NOTIFY_HANDLER_V)( NMHDR *, LRESULT * );
typedef BOOL (CCmdTarget::*PFN_NOTIFY_HANDLER_B)( NMHDR *, LRESULT * );
typedef BOOL (CCmdTarget::*PFN_NOTIFY_HANDLER_EX)( UINT, NMHDR *, LRESULT * );
typedef void (CCmdTarget::*PFN_NOTIFY_HANDLER_RANGE)( UINT, NMHDR *, LRESULT * );

IMPLEMENT_DYNAMIC( CCmdTarget, CObject )

const AFX_INTERFACEMAP_ENTRY CCmdTarget::_interfaceEntries[] = {
    { NULL, -1 }
};

const AFX_INTERFACEMAP CCmdTarget::interfaceMap =
    { NULL, CCmdTarget::_interfaceEntries };

CCmdTarget::CCmdTarget()
/**********************/
{
    m_dwRef = 1;
}

const AFX_INTERFACEMAP *CCmdTarget::GetInterfaceMap() const
/*********************************************************/
{
    return( &interfaceMap );
}

const AFX_MSGMAP *CCmdTarget::GetMessageMap() const
/*************************************************/
{
    return( GetThisMessageMap() );
}

#ifdef _AFXDLL

const AFX_INTERFACEMAP * PASCAL CCmdTarget::GetThisInterfaceMap()
/***************************************************************/
{
    return( &interfaceMap );
}

#endif // _AFXDLL

const AFX_MSGMAP * PASCAL CCmdTarget::GetThisMessageMap()
/*******************************************************/
{
    static const AFX_MSGMAP_ENTRY _messageEntries[] = {
        { 0, 0, 0, 0, AfxSig_end, NULL }
    };
    static const AFX_MSGMAP messageMap =
        { NULL, _messageEntries };
    return( &messageMap );
}

BOOL CCmdTarget::OnCmdMsg( UINT nID, int nCode, void *pExtra,
                           AFX_CMDHANDLERINFO *pHandlerInfo )
/***********************************************************/
{
    const AFX_MSGMAP *pMessageMap = GetMessageMap();
    UINT nMessage = HIWORD( nCode );
    if( nMessage == 0 || nMessage == 0xFFFF ) {
        nMessage = WM_COMMAND;
    } else {
        // Take the low word and cast it to a signed 16-bit integer.
        nCode = (short)LOWORD( nCode );
    }
    for( ;; ) {
        const AFX_MSGMAP_ENTRY *pEntries = pMessageMap->lpEntries;
        int i = 0;
        while( pEntries[i].nSig != AfxSig_end ) {
            if( pEntries[i].nMessage == nMessage && pEntries[i].nCode == nCode &&
                nID >= pEntries[i].nID && nID <= pEntries[i].nLastID ) {
                if( pHandlerInfo != NULL ) {
                    pHandlerInfo->pTarget = this;
                    pHandlerInfo->pmf = pEntries[i].pfn;
                    return( TRUE );
                }
                switch( pEntries[i].nSig ) {
                case AfxSigCmd_v:
                    (this->*pEntries[i].pfn)();
                    return( TRUE );
                case AfxSigCmd_b:
                    if( (this->*(PFN_COMMAND_HANDLER_B)pEntries[i].pfn)() ) {
                        return( TRUE );
                    }
                    break;
                case AfxSigCmd_EX:
                    if( (this->*(PFN_COMMAND_HANDLER_EX)pEntries[i].pfn)( nID ) ) {
                        return( TRUE );
                    }
                    break;
                case AfxSigCmd_RANGE:
                    (this->*(PFN_COMMAND_HANDLER_RANGE)pEntries[i].pfn)( nID );
                    return( TRUE );
                case AfxSigCmdUI:
                    {
                        CCmdUI *pCmdUI = (CCmdUI *)pExtra;
                        (this->*(PFN_COMMAND_UI_HANDLER)pEntries[i].pfn)( pCmdUI );
                        if( pCmdUI->m_bContinueRouting ) {
                            pCmdUI->m_bContinueRouting = FALSE;
                            return( FALSE );
                        }
                        return( TRUE );
                    }
                case AfxSigCmdUI_RANGE:
                    {
                        CCmdUI *pCmdUI = (CCmdUI *)pExtra;
                        (this->*(PFN_COMMAND_UI_HANDLER_RANGE)pEntries[i].pfn)( pCmdUI,
                                                                                nID );
                        if( pCmdUI->m_bContinueRouting ) {
                            pCmdUI->m_bContinueRouting = FALSE;
                            return( FALSE );
                        }
                        return( TRUE );
                    }
                case AfxSigNotify_v:
                    {
                        AFX_NOTIFY *pNotify = (AFX_NOTIFY *)pExtra;
                        ASSERT( pNotify != NULL );
                        (this->*(PFN_NOTIFY_HANDLER_V)pEntries[i].pfn)( pNotify->pNMHDR,
                            pNotify->pResult );
                        return( TRUE );
                    }
                case AfxSigNotify_b:
                    {
                        AFX_NOTIFY *pNotify = (AFX_NOTIFY *)pExtra;
                        ASSERT( pNotify != NULL );
                        if( (this->*(PFN_NOTIFY_HANDLER_B)pEntries[i].pfn)(
                            pNotify->pNMHDR, pNotify->pResult ) ) {
                            return( TRUE );
                        }
                        break;
                    }
                case AfxSigNotify_EX:
                    {
                        AFX_NOTIFY *pNotify = (AFX_NOTIFY *)pExtra;
                        ASSERT( pNotify != NULL );
                        if( (this->*(PFN_NOTIFY_HANDLER_EX)pEntries[i].pfn)( nID,
                            pNotify->pNMHDR, pNotify->pResult ) ) {
                            return( TRUE );
                        }
                        break;
                    }
                case AfxSigNotify_RANGE:
                    {
                        AFX_NOTIFY *pNotify = (AFX_NOTIFY *)pExtra;
                        ASSERT( pNotify != NULL );
                        (this->*(PFN_NOTIFY_HANDLER_RANGE)pEntries[i].pfn)( nID,
                            pNotify->pNMHDR, pNotify->pResult );
                        break;
                    }
                default:
                    return( FALSE );
                }
            }
            i++;
        }
        if( pMessageMap->pfnGetBaseMap == NULL ) {
            break;
        }
        pMessageMap = pMessageMap->pfnGetBaseMap();
    }
    return( FALSE );
}

void CCmdTarget::OnFinalRelease()
/*******************************/
{
    delete this;
}

#ifdef _DEBUG

void CCmdTarget::Dump( CDumpContext &dc ) const
/*********************************************/
{
    CObject::Dump( dc );

    dc << "m_dwRef = " << m_dwRef << "\n";
}

#endif // _DEBUG

DWORD CCmdTarget::InternalQueryInterface( const void *piid, LPVOID *ppvObj )
/*************************************************************************/
{
    const AFX_INTERFACEMAP *pInterfaceMap = GetInterfaceMap();
#ifdef _AFXDLL
    for( ;; ) {
#else
    while( pInterfaceMap != NULL ) {
#endif
        const AFX_INTERFACEMAP_ENTRY *pEntries = pInterfaceMap->pEntry;
        int i = 0;
        while( pEntries[i].piid != NULL ) {
            if( memcmp( pEntries[i].piid, piid, sizeof( IID ) ) == 0 ) {
                *ppvObj = (LPVOID)((BYTE *)this + pEntries[i].nOffset);
                m_dwRef++;
                return( S_OK );
            }
            i++;
        }
#ifdef _AFXDLL
        if( pInterfaceMap->pfnGetBaseMap == NULL ) {
            break;
        }
        pInterfaceMap = pInterfaceMap->pfnGetBaseMap();
#else
        pInterfaceMap = pInterfaceMap->pBaseMap;
#endif
    }
    return( E_NOINTERFACE );
}

DWORD CCmdTarget::InternalAddRef()
/********************************/
{
    m_dwRef++;
    return( m_dwRef );
}

DWORD CCmdTarget::InternalRelease()
/*********************************/
{
    m_dwRef--;
    if( m_dwRef == 0 ) {
        OnFinalRelease();
        return( 0 );
    }
    return( m_dwRef );
}
