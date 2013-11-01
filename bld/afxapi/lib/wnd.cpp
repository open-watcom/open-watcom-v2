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
* Description:  Implementation of CWnd and AfxRepositionWindow.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

// WM_CTLCOLOR parameters
struct AFX_CTLCOLOR {
    HWND    hWnd;
    HDC     hDC;
    UINT    nCtlType;
};

// Message handler function pointers
typedef BOOL    (CWnd::*PHANDLER_B_D_V)( CDC * );
typedef BOOL    (CWnd::*PHANDLER_B_B_V)( BOOL );
typedef BOOL    (CWnd::*PHANDLER_B_W_UU)( CWnd *, UINT, UINT );
typedef BOOL    (CWnd::*PHANDLER_B_W_COPYDATASTRUCT)( CWnd *, COPYDATASTRUCT * );
typedef HBRUSH  (CWnd::*PHANDLER_CTLCOLOR)( CDC *, CWnd *, UINT );
typedef HBRUSH  (CWnd::*PHANDLER_CTLCOLOR_REFLECT)( CDC *, UINT );
typedef int     (CWnd::*PHANDLER_I_U_W_U)( UINT, CWnd *, UINT );
typedef int     (CWnd::*PHANDLER_I_W_UU)( CWnd *, UINT, UINT );
typedef int     (CWnd::*PHANDLER_I_V_S)( LPTSTR );
typedef LRESULT (CWnd::*PHANDLER_L_W_L)( WPARAM, LPARAM );
typedef LRESULT (CWnd::*PHANDLER_L_UU_M)( UINT, UINT, CMenu * );
typedef void    (CWnd::*PHANDLER_V_H_V)( HANDLE );
typedef void    (CWnd::*PHANDLER_V_H_H)( HANDLE, HANDLE );
typedef void    (CWnd::*PHANDLER_V_U_V)( UINT );
typedef void    (CWnd::*PHANDLER_V_U_U)( UINT, UINT );
typedef void    (CWnd::*PHANDLER_V_UU_V)( UINT, UINT );
typedef void    (CWnd::*PHANDLER_V_V_II)( int, int );
typedef void    (CWnd::*PHANDLER_V_U_UU)( UINT, UINT, UINT );
typedef void    (CWnd::*PHANDLER_V_U_II)( UINT, int, int );
typedef void    (CWnd::*PHANDLER_V_U_W)( UINT, CWnd * );
typedef void    (CWnd::*PHANDLER_V_W_L)( WPARAM, LPARAM );
typedef void    (CWnd::*PHANDLER_MDIACTIVATE)( BOOL, CWnd *, CWnd * );
typedef void    (CWnd::*PHANDLER_V_D_V)( CDC * );
typedef void    (CWnd::*PHANDLER_V_M_V)( CMenu * );
typedef void    (CWnd::*PHANDLER_V_M_UB)( CMenu *, UINT, BOOL );
typedef void    (CWnd::*PHANDLER_V_W_V)( CWnd * );
typedef void    (CWnd::*PHANDLER_V_V_W)( CWnd * );
typedef void    (CWnd::*PHANDLER_V_W_UU)( CWnd *, UINT, UINT );
typedef void    (CWnd::*PHANDLER_V_W_P)( CWnd *, CPoint );
typedef void    (CWnd::*PHANDLER_V_W_H)( CWnd *, HANDLE );
typedef HCURSOR (CWnd::*PHANDLER_C_V_V)();
typedef void    (CWnd::*PHANDLER_ACTIVATE)( UINT, CWnd *, BOOL );
typedef void    (CWnd::*PHANDLER_SCROLL)( UINT, UINT, CWnd * );
typedef void    (CWnd::*PHANDLER_SCROLL_REFLECT)( UINT, UINT );
typedef void    (CWnd::*PHANDLER_V_V_S)( LPTSTR );
typedef void    (CWnd::*PHANDLER_V_U_CS)( UINT, LPCTSTR );
typedef void    (CWnd::*PHANDLER_OWNERDRAW)( int, LPTSTR );
typedef UINT    (CWnd::*PHANDLER_U_V_V)();
typedef void    (CWnd::*PHANDLER_V_B_NCCALCSIZEPARAMS)( BOOL, NCCALCSIZE_PARAMS * );
typedef void    (CWnd::*PHANDLER_V_UU_M)( UINT, UINT, HMENU );
typedef void    (CWnd::*PHANDLER_V_U_P)( UINT, CPoint );
typedef void    (CWnd::*PHANDLER_SIZING)( UINT, LPRECT );
typedef BOOL    (CWnd::*PHANDLER_MOUSEWHEEL)( UINT, short, CPoint );
typedef void    (CWnd::*PHANDLER_MOUSEHWHEEL)( UINT, short, CPoint );
typedef LRESULT (CWnd::*PHANDLER_L_P)( CPoint );
typedef void    (CWnd::*PHANDLER_V_U_M)( UINT, CMenu * );
typedef UINT    (CWnd::*PHANDLER_U_U_M)( UINT, CMenu * );
typedef UINT    (CWnd::*PHANDLER_U_V_MENUGETOBJECTINFO)( MENUGETOBJECTINFO * );
typedef void    (CWnd::*PHANDLER_V_M_U)( CMenu *, UINT );
typedef void    (CWnd::*PHANDLER_RAWINPUT)( UINT, HRAWINPUT );
typedef void    (CWnd::*PHANDLER_MOUSE_XBUTTON)( UINT, UINT, CPoint );
typedef void    (CWnd::*PHANDLER_MOUSE_NCXBUTTON)( short, UINT, CPoint );
typedef void    (CWnd::*PHANDLER_INPUTDEVICECHANGE)( unsigned short );

// HtmlHelp() function
typedef HWND (WINAPI *PFN_HTMLHELP)( HWND, LPCTSTR, UINT, DWORD_PTR );
static PFN_HTMLHELP _HtmlHelpFnPtr = NULL;

// HtmlHelp() function name
#ifdef _UNICODE
    #define HTMLHELP_NAME   "HtmlHelpW"
#else
    #define HTMLHELP_NAME   "HtmlHelpA"
#endif

// IsAppThemed() function
typedef BOOL (WINAPI *PFN_ISAPPTHEMED)( void );

// Register DRAGLISTMSGSTRING message.
const UINT CWnd::m_nMsgDragList = ::RegisterWindowMessage( DRAGLISTMSGSTRING );

// Special CWnd objects used with SetWindowPos()
const CWnd CWnd::wndBottom( HWND_BOTTOM );
const CWnd CWnd::wndTop( HWND_TOP );
const CWnd CWnd::wndTopMost( HWND_TOPMOST );
const CWnd CWnd::wndNoTopMost( HWND_NOTOPMOST );

IMPLEMENT_DYNCREATE( CWnd, CCmdTarget )

BEGIN_MESSAGE_MAP( CWnd, CCmdTarget )
    ON_WM_CHARTOITEM()
    ON_WM_CTLCOLOR()
    ON_WM_COMPAREITEM()
    ON_WM_DELETEITEM()
    ON_WM_DRAWITEM()
    ON_WM_HSCROLL()
    ON_WM_MEASUREITEM()
    ON_WM_NCDESTROY()
    ON_WM_VKEYTOITEM()
    ON_WM_VSCROLL()
    ON_REGISTERED_MESSAGE( m_nMsgDragList, OnDragList )
    ON_MESSAGE( WM_CTLCOLORBTN, OnNTCtlColor )
    ON_MESSAGE( WM_CTLCOLORDLG, OnNTCtlColor )
    ON_MESSAGE( WM_CTLCOLOREDIT, OnNTCtlColor )
    ON_MESSAGE( WM_CTLCOLORLISTBOX, OnNTCtlColor )
    ON_MESSAGE( WM_CTLCOLORMSGBOX, OnNTCtlColor )
    ON_MESSAGE( WM_CTLCOLORSCROLLBAR, OnNTCtlColor )
    ON_MESSAGE( WM_CTLCOLORSTATIC, OnNTCtlColor )
END_MESSAGE_MAP()

static HMENU _FindPopupMenu( HMENU hMenu, UINT nID )
/**************************************************/
{
    int nCount = ::GetMenuItemCount( hMenu );
    for( int i = 0; i < nCount; i++ ) {
        HMENU hSubMenu = ::GetSubMenu( hMenu, i );
        if( hSubMenu != NULL ) {
            hSubMenu = _FindPopupMenu( hSubMenu, nID );
            if( hSubMenu != NULL ) {
                return( hSubMenu );
            }
        } else {
            UINT nCurID = ::GetMenuItemID( hMenu, i );
            if( nCurID == nID ) {
                return( hMenu );
            }
        }
    }
    return( NULL );
}

static UINT _TranslateCtlType( UINT message )
/*******************************************/
{
    switch( message ) {
    case WM_CTLCOLORBTN:
        return( CTLCOLOR_BTN );
    case WM_CTLCOLORDLG:
        return( CTLCOLOR_DLG );
    case WM_CTLCOLOREDIT:
        return( CTLCOLOR_EDIT );
    case WM_CTLCOLORLISTBOX:
        return( CTLCOLOR_LISTBOX );
    case WM_CTLCOLORMSGBOX:
        return( CTLCOLOR_MSGBOX );
    case WM_CTLCOLORSCROLLBAR:
        return( CTLCOLOR_SCROLLBAR );
    case WM_CTLCOLORSTATIC:
        return( CTLCOLOR_STATIC );
    }
    return( 0 );
}

static BOOL _DisableToolTips()
/****************************/
{
    // The tool tips flicker on Windows Vista and later when visual styles are enabled.
    // Lacking a better solution, just disable them on these versions.
    static BOOL bResult = FALSE;
    if( bResult ) {
        return( TRUE );
    }
    
    OSVERSIONINFO osvi;
    ::GetVersionEx( &osvi );
    if( osvi.dwMajorVersion < 6 ) {
        return( FALSE );
    }

    HINSTANCE hInstance = ::LoadLibrary( _T("UXTHEME.DLL") );
    if( hInstance == NULL ) {
        return( FALSE );
    }

    PFN_ISAPPTHEMED pfn = (PFN_ISAPPTHEMED)::GetProcAddress( hInstance, "IsAppThemed" );
    if( pfn != NULL ) {
        bResult = pfn();
    }
    ::FreeLibrary( hInstance );
    return( bResult );
}

CWnd::CWnd()
/**********/
{
    m_hWnd = NULL;
    m_hWndOwner = NULL;
    m_nFlags = 0;
    m_pfnSuper = NULL;
}

CWnd::CWnd( HWND hWnd )
/*********************/
{
    // This constructor is used to initialize the special CWnd objects used with
    // SetWindowPos().  It should never be used to construct a CWnd for an actual window.
    m_hWnd = hWnd;
    m_hWndOwner = NULL;
    m_nFlags = 0;
    m_pfnSuper = NULL;
}

void CWnd::CalcWindowRect( LPRECT lpClientRect, UINT nAdjustType )
/****************************************************************/
{
    ASSERT( m_hWnd != NULL );
    DWORD dwExStyle = ::GetWindowLong( m_hWnd, GWL_EXSTYLE );
    if( nAdjustType == adjustBorder ) {
        dwExStyle &= ~WS_EX_CLIENTEDGE;
    }
    DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
    ::AdjustWindowRectEx( lpClientRect, dwStyle, FALSE, dwExStyle );
}

BOOL CWnd::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
                   const RECT &rect, CWnd *pParentWnd, UINT nID,
                   CCreateContext *pContext )
/*******************************************/
{
    return( CreateEx( 0L, lpszClassName, lpszWindowName, dwStyle, rect.left, rect.top,
                      rect.right - rect.left, rect.bottom - rect.top,
                      pParentWnd->GetSafeHwnd(), (HMENU)nID, pContext ) );
}

BOOL CWnd::CreateEx( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
                     DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                     HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam )
/**********************************************************************/
{
    CREATESTRUCT cs;
    cs.dwExStyle = dwExStyle;
    cs.lpszClass = lpszClassName;
    cs.lpszName = lpszWindowName;
    cs.style = dwStyle;
    cs.x = x;
    cs.y = y;
    cs.cx = nWidth;
    cs.cy = nHeight;
    cs.hwndParent = hWndParent;
    cs.hMenu = nIDorHMenu;
    cs.hInstance = AfxGetInstanceHandle();
    cs.lpCreateParams = lpParam;
    if( !PreCreateWindow( cs ) ) {
        PostNcDestroy();
        return( FALSE );
    }

    AfxHookWindowCreate( this );
    HWND hWnd = ::CreateWindowEx( cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
                                  cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu,
                                  cs.hInstance, cs.lpCreateParams );
    if( !AfxUnhookWindowCreate() ) {
        PostNcDestroy();
        return( FALSE );
    }
    if( hWnd == NULL ) {
        PostNcDestroy();
        return( FALSE );
    }
    ASSERT( m_hWnd == hWnd );
    
    return( TRUE );
}

BOOL CWnd::CreateEx( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
                     DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID,
                     LPVOID lpParam )
/***********************************/
{
    return( CreateEx( dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect.left,
                      rect.top, rect.right - rect.left, rect.bottom - rect.top,
                      pParentWnd->GetSafeHwnd(), (HMENU)nID, lpParam ) );
}

LRESULT CWnd::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
    ASSERT( m_hWnd != NULL );
    if( m_pfnSuper != NULL ) {
        return( ::CallWindowProc( m_pfnSuper, m_hWnd, message, wParam, lParam ) );
    } else {
        return( ::DefWindowProc( m_hWnd, message, wParam, lParam ) );
    }
}


BOOL CWnd::DestroyWindow()
/************************/
{
    return( ::DestroyWindow( m_hWnd ) );
}

void CWnd::DoDataExchange( CDataExchange *pDX )
/*********************************************/
{
    UNUSED_ALWAYS( pDX );
}

CScrollBar *CWnd::GetScrollBarCtrl( int nBar ) const
/**************************************************/
{
    UNUSED_ALWAYS( nBar );
    return( NULL );
}

void CWnd::HtmlHelp( DWORD_PTR dwData, UINT nCmd )
/************************************************/
{
    PrepareForHelp();
    if( _HtmlHelpFnPtr == NULL ) {
        HINSTANCE hInstance = ::LoadLibrary( _T("HHCTRL.OCX") );
        if( hInstance != NULL ) {
            _HtmlHelpFnPtr = (PFN_HTMLHELP)::GetProcAddress( hInstance, HTMLHELP_NAME );
        }
    }
    if( _HtmlHelpFnPtr != NULL ) {
        CWinApp *pApp = AfxGetApp();
        ASSERT( pApp != NULL );
        _HtmlHelpFnPtr( m_hWnd, pApp->m_pszHelpFilePath, nCmd, dwData );
    }
}

BOOL CWnd::IsFrameWnd() const
/***************************/
{
    return( FALSE );
}

BOOL CWnd::OnChildNotify( UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult )
/**************************************************************************************/
{
    return( ReflectChildNotify( message, wParam, lParam, pResult ) );
}

BOOL CWnd::OnCommand( WPARAM wParam, LPARAM lParam )
/**************************************************/
{
    if( ReflectLastMsg( (HWND)lParam ) ) {
        return( TRUE );
    }
    if( HIWORD( wParam ) == 0 || HIWORD( wParam ) == 1 ) {
        if( this->OnCmdMsg( LOWORD( wParam ), CN_COMMAND, NULL, NULL ) ) {
            return( TRUE );
        }
    } else {
        if( this->OnCmdMsg( LOWORD( wParam ), HIWORD( wParam ), NULL, NULL ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

BOOL CWnd::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT *pResult )
/*******************************************************************/
{
    UNUSED_ALWAYS( wParam );

    NMHDR *pNMHDR = (NMHDR *)lParam;
    ASSERT( pNMHDR != NULL );
    if( ReflectLastMsg( pNMHDR->hwndFrom, pResult ) ) {
        return( TRUE );
    }
    
    AFX_NOTIFY params;
    params.pResult = pResult;
    params.pNMHDR = pNMHDR;
    return( OnCmdMsg( pNMHDR->idFrom, MAKELONG( pNMHDR->code, WM_NOTIFY ),
                      &params, NULL ) );
}

INT_PTR CWnd::OnToolHitTest( CPoint point, TOOLINFO *pTI ) const
/**************************************************************/
{
    HWND hWndChild = ::ChildWindowFromPoint( m_hWnd, point );
    if( hWndChild != NULL && hWndChild != m_hWnd ) {
        UINT nID = ::GetDlgCtrlID( hWndChild );
        if( nID != NULL ) {
            if( pTI != NULL ) {
                pTI->hwnd = m_hWnd;
                pTI->uId = (UINT)hWndChild;
                pTI->uFlags |= TTF_IDISHWND;
                pTI->lpszText = LPSTR_TEXTCALLBACK;
                if( !(::SendMessage( hWndChild, WM_GETDLGCODE, 0, 0L ) & DLGC_BUTTON) ) {
                    pTI->uFlags |= TTF_NOTBUTTON | TTF_CENTERTIP;
                }
            }
            return( nID );
        }
    }
    return( -1 );
}

BOOL CWnd::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult )
/*********************************************************************************/
{
    LRESULT lResult;
    if( pResult == NULL ) {
        pResult = &lResult;
    }
    if( message == WM_COMMAND ) {
        if( OnCommand( wParam, lParam ) ) {
            return( TRUE );
        }
    } else if( message == WM_NOTIFY ) {
        if( OnNotify( wParam, lParam, pResult ) ) {
            return( TRUE );
        }
    } else {
        const AFX_MSGMAP *pMessageMap = GetMessageMap();
        for( ;; ) {
            const AFX_MSGMAP_ENTRY *pEntries = pMessageMap->lpEntries;
            int i = 0;
            while( pEntries[i].nSig != AfxSig_end ) {
                if( pEntries[i].nMessage == 0xC000 ) {
                    if( *(UINT *)pEntries[i].nSig == message ) {
                        *pResult = (this->*(PHANDLER_L_W_L)(AFX_PMSGW)pEntries[i].pfn)(
                            wParam, lParam );
                        return( TRUE );
                    }
                } else if( pEntries[i].nMessage == message ) {
                    switch( pEntries[i].nSig ) {
                    case AfxSig_v_v_v:
                        (this->*pEntries[i].pfn)();
                        break;
                    case AfxSig_b_b_v:
                        *pResult = (this->*(PHANDLER_B_B_V)(AFX_PMSGW)pEntries[i].pfn)(
                            (BOOL)wParam );
                        break;
                    case AfxSig_b_D_v:
                        *pResult = (this->*(PHANDLER_B_D_V)(AFX_PMSGW)pEntries[i].pfn)(
                            CDC::FromHandle( (HDC)wParam ) );
                        break;
                    case AfxSig_b_W_uu:
                        *pResult = (this->*(PHANDLER_B_W_UU)(AFX_PMSGW)pEntries[i].pfn)(
                            CWnd::FromHandle( (HWND)wParam ), LOWORD( lParam ),
                            HIWORD( lParam ) );
                        break;
                    case AfxSig_b_W_COPYDATASTRUCT:
                        *pResult = (this->*(PHANDLER_B_W_COPYDATASTRUCT)(AFX_PMSGW)
                            pEntries[i].pfn)( CWnd::FromHandle( (HWND)wParam ),
                            (COPYDATASTRUCT *)lParam );
                        break;
                    case AfxSig_CTLCOLOR:
                        {
                            AFX_CTLCOLOR *pCtlColor = (AFX_CTLCOLOR *)lParam;
                            ASSERT( pCtlColor != NULL );
                            *pResult = (LRESULT)(this->*(PHANDLER_CTLCOLOR)(AFX_PMSGW)
                                pEntries[i].pfn)( CDC::FromHandle( pCtlColor->hDC ),
                                CWnd::FromHandle( pCtlColor->hWnd ),
                                pCtlColor->nCtlType );
                            break;
                        }
                    case AfxSig_CTLCOLOR_REFLECT:
                        {
                            AFX_CTLCOLOR *pCtlColor = (AFX_CTLCOLOR *)lParam;
                            ASSERT( pCtlColor != NULL );
                            *pResult = (LRESULT)(this->*(PHANDLER_CTLCOLOR_REFLECT)
                                (AFX_PMSGW)pEntries[i].pfn)( CDC::FromHandle(
                                pCtlColor->hDC ), pCtlColor->nCtlType );
                            break;
                        }
                    case AfxSig_i_u_W_u:
                        *pResult = (this->*(PHANDLER_I_U_W_U)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), CWnd::FromHandle( (HWND)lParam ),
                            HIWORD( wParam ) );
                        break;
                    case AfxSig_i_W_uu:
                        *pResult = (this->*(PHANDLER_I_W_UU)(AFX_PMSGW)pEntries[i].pfn)(
                            CWnd::FromHandle( (HWND)wParam ), LOWORD( lParam ),
                            HIWORD( lParam ) );
                        break;
                    case AfxSig_i_v_s:
                        *pResult = (this->*(PHANDLER_I_V_S)(AFX_PMSGW)pEntries[i].pfn)(
                            (LPTSTR)lParam );
                        break;
                    case AfxSig_l_w_l:
                        *pResult = (this->*(PHANDLER_L_W_L)(AFX_PMSGW)pEntries[i].pfn)(
                            wParam, lParam );
                        break;
                    case AfxSig_l_uu_M:
                        *pResult = (this->*(PHANDLER_L_UU_M)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ),
                            CMenu::FromHandle( (HMENU)lParam ) );
                        break;
                    case AfxSig_v_h_v:
                        (this->*(PHANDLER_V_H_V)(AFX_PMSGW)pEntries[i].pfn)(
                            (HANDLE)wParam );
                        break;
                    case AfxSig_v_h_h:
                        (this->*(PHANDLER_V_H_H)(AFX_PMSGW)pEntries[i].pfn)(
                            (HANDLE)wParam, (HANDLE)lParam );
                        break;
                    case AfxSig_v_u_v:
                        (this->*(PHANDLER_V_U_V)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam );
                        break;
                    case AfxSig_v_u_u:
                        (this->*(PHANDLER_V_U_U)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, (UINT)lParam );
                        break;
                    case AfxSig_v_uu_v:
                        (this->*(PHANDLER_V_UU_V)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ) );
                        break;
                    case AfxSig_v_v_ii:
                        (this->*(PHANDLER_V_V_II)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( lParam ), HIWORD( lParam ) );
                        break;
                    case AfxSig_v_u_uu:
                        (this->*(PHANDLER_V_U_UU)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, LOWORD( lParam ), HIWORD( lParam ) );
                        break;
                    case AfxSig_v_u_ii:
                        (this->*(PHANDLER_V_U_II)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, LOWORD( lParam ), HIWORD( lParam ) );
                        break;
                    case AfxSig_v_u_W:
                        (this->*(PHANDLER_V_U_W)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, CWnd::FromHandle( (HWND)lParam ) );
                        break;
                    case AfxSig_v_w_l:
                        (this->*(PHANDLER_V_W_L)(AFX_PMSGW)pEntries[i].pfn)(
                            wParam, lParam );
                        break;
                    case AfxSig_MDIACTIVATE:
                        (this->*(PHANDLER_MDIACTIVATE)(AFX_PMSGW)pEntries[i].pfn)(
                            (HWND)lParam == m_hWnd, CWnd::FromHandle( (HWND)lParam ),
                            CWnd::FromHandle( (HWND) wParam ) );
                        break;
                    case AfxSig_v_D_v:
                        (this->*(PHANDLER_V_D_V)(AFX_PMSGW)pEntries[i].pfn)(
                            CDC::FromHandle( (HDC)wParam ) );
                        break;
                    case AfxSig_v_M_v:
                        (this->*(PHANDLER_V_M_V)(AFX_PMSGW)pEntries[i].pfn)(
                            CMenu::FromHandle( (HMENU)wParam ) );
                        break;
                    case AfxSig_v_M_ub:
                        (this->*(PHANDLER_V_M_UB)(AFX_PMSGW)pEntries[i].pfn)(
                            CMenu::FromHandle( (HMENU)wParam ), LOWORD( lParam ),
                            HIWORD( lParam ) );
                        break;
                    case AfxSig_v_W_v:
                        (this->*(PHANDLER_V_W_V)(AFX_PMSGW)pEntries[i].pfn)(
                            CWnd::FromHandle( (HWND)wParam ) );
                        break;
                    case AfxSig_v_v_W:
                        (this->*(PHANDLER_V_V_W)(AFX_PMSGW)pEntries[i].pfn)(
                            CWnd::FromHandle( (HWND)lParam ) );
                        break;
                    case AfxSig_v_W_uu:
                        (this->*(PHANDLER_V_W_UU)(AFX_PMSGW)pEntries[i].pfn)(
                            CWnd::FromHandle( (HWND)wParam ), LOWORD( lParam ),
                            HIWORD( lParam ) );
                        break;
                    case AfxSig_v_W_p:
                        (this->*(PHANDLER_V_W_P)(AFX_PMSGW)pEntries[i].pfn)(
                            CWnd::FromHandle( (HWND)wParam ), CPoint( lParam ) );
                        break;
                    case AfxSig_v_W_h:
                        (this->*(PHANDLER_V_W_H)(AFX_PMSGW)pEntries[i].pfn)(
                            CWnd::FromHandle( (HWND)wParam ), (HANDLE)lParam );
                        break;
                    case AfxSig_C_v_v:
                        *pResult = (LRESULT)(this->*(PHANDLER_C_V_V)(AFX_PMSGW)
                            pEntries[i].pfn)();
                        break;
                    case AfxSig_ACTIVATE:
                        (this->*(PHANDLER_ACTIVATE)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), CWnd::FromHandle( (HWND)lParam ),
                            HIWORD( wParam ) );
                        break;
                    case AfxSig_SCROLL:
                        (this->*(PHANDLER_SCROLL)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ),
                            CWnd::FromHandle( (HWND)lParam ) );
                        break;
                    case AfxSig_SCROLL_REFLECT:
                        (this->*(PHANDLER_SCROLL_REFLECT)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ) );
                        break;
                    case AfxSig_v_v_s:
                        (this->*(PHANDLER_V_V_S)(AFX_PMSGW)pEntries[i].pfn)(
                            (LPTSTR)lParam );
                        break;
                    case AfxSig_v_u_cs:
                        (this->*(PHANDLER_V_U_CS)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, (LPCTSTR)lParam );
                        break;
                    case AfxSig_OWNERDRAW:
                        (this->*(PHANDLER_OWNERDRAW)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, (LPTSTR)lParam );
                        *pResult = 1;
                        break;
                    case AfxSig_u_v_v:
                        *pResult = (this->*(PHANDLER_U_V_V)(AFX_PMSGW)pEntries[i].pfn)();
                        break;
                    case AfxSig_v_b_NCCALCSIZEPARAMS:
                        (this->*(PHANDLER_V_B_NCCALCSIZEPARAMS)(AFX_PMSGW)
                            pEntries[i].pfn)( (BOOL)wParam,
                            (NCCALCSIZE_PARAMS *)lParam );
                        break;
                    case AfxSig_v_u_p:
                        (this->*(PHANDLER_V_U_P)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, CPoint( lParam ) );
                        break;
                    case AfxSig_v_uu_M:
                        (this->*(PHANDLER_V_UU_M)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ), (HMENU)lParam );
                        break;
                    case AfxSig_SIZING:
                        (this->*(PHANDLER_SIZING)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, (LPRECT)lParam );
                        break;
                    case AfxSig_MOUSEWHEEL:
                        if( !(this->*(PHANDLER_MOUSEWHEEL)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ), CPoint( lParam ) ) ) {
                            return( FALSE );
                        }
                        break;
                    case AfxSig_MOUSEHWHEEL:
                        (this->*(PHANDLER_MOUSEHWHEEL)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ), CPoint( lParam ) );
                        break;
                    case AfxSig_l_p:
                        *pResult = (this->*(PHANDLER_L_P)(AFX_PMSGW)pEntries[i].pfn)(
                            CPoint( lParam ) );
                        break;
                    case AfxSig_v_u_M:
                        (this->*(PHANDLER_V_U_M)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, CMenu::FromHandle( (HMENU)lParam ) );
                        break;
                    case AfxSig_u_u_M:
                        *pResult = (this->*(PHANDLER_U_U_M)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, CMenu::FromHandle( (HMENU)lParam ) );
                        break;
                    case AfxSig_u_v_MENUGETOBJECTINFO:
                        *pResult = (this->*(PHANDLER_U_V_MENUGETOBJECTINFO)(AFX_PMSGW)pEntries[i].pfn)(
                            (MENUGETOBJECTINFO *)lParam );
                        break;
                    case AfxSig_v_M_u:
                        (this->*(PHANDLER_V_M_U)(AFX_PMSGW)pEntries[i].pfn)(
                            CMenu::FromHandle( (HMENU)wParam ), (UINT)lParam );
                        break;
                    case AfxSig_RAWINPUT:
                        (this->*(PHANDLER_RAWINPUT)(AFX_PMSGW)pEntries[i].pfn)(
                            (UINT)wParam, (HRAWINPUT)lParam );
                        break;
                    case AfxSig_MOUSE_XBUTTON:
                        (this->*(PHANDLER_MOUSE_XBUTTON)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ), CPoint( lParam ) );
                        break;
                    case AfxSig_MOUSE_NCXBUTTON:
                        (this->*(PHANDLER_MOUSE_NCXBUTTON)(AFX_PMSGW)pEntries[i].pfn)(
                            LOWORD( wParam ), HIWORD( wParam ), CPoint( lParam ) );
                        break;
                    case AfxSig_INPUTDEVICECHANGE:
                        (this->*(PHANDLER_INPUTDEVICECHANGE)(AFX_PMSGW)pEntries[i].pfn)(
                            (unsigned short)wParam );
                        break;
                    default:
                        return( FALSE );
                    }
                    return( TRUE );
                }
                i++;
            }
            if( pMessageMap->pfnGetBaseMap == NULL ) {
                break;
            }
            pMessageMap = pMessageMap->pfnGetBaseMap();
        }
    }
    return( FALSE );
}

void CWnd::PostNcDestroy()
/************************/
{
}

BOOL CWnd::PreCreateWindow( CREATESTRUCT &cs )
/********************************************/
{
    if( cs.lpszClass == NULL ) {
        cs.lpszClass = _T("AfxWnd");
    }
    return( TRUE );
}

void CWnd::PreSubclassWindow()
/****************************/
{
}

BOOL CWnd::PreTranslateMessage( MSG *pMsg )
/*****************************************/
{
    AFX_MODULE_STATE *pState = AfxGetModuleState();
    ASSERT( pState != NULL );
    if( pState->m_pfnFilterToolTipMessage != NULL ) {
        pState->m_pfnFilterToolTipMessage( pMsg, this );
    }
    return( FALSE );
}

void CWnd::WinHelp( DWORD_PTR dwData, UINT nCmd )
/***********************************************/
{
    PrepareForHelp();
    
    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );
    ::WinHelp( m_hWnd, pApp->m_pszHelpFilePath, nCmd, dwData );
}

void CWnd::WinHelpInternal( DWORD_PTR dwData, UINT nCmd )
/*******************************************************/
{
    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );
    if( pApp->m_eHelpType == afxHTMLHelp ) {
        switch( nCmd ) {
        case HELP_CONTEXT:
            nCmd = HH_HELP_CONTEXT;
            break;
        case HELP_CONTENTS:
            nCmd = HH_DISPLAY_TOC;
            break;
        case HELP_FINDER:
            nCmd = HH_HELP_FINDER;
            break;
        }
        HtmlHelp( dwData, nCmd );
    } else {
        WinHelp( dwData, nCmd );
    }
}

LRESULT CWnd::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
/********************************************************************/
{
    LRESULT lResult;
    if( OnWndMsg( message, wParam, lParam, &lResult ) ) {
        return( lResult );
    }
    return( DefWindowProc( message, wParam, lParam ) );
}

#ifdef _DEBUG

void CWnd::AssertValid() const
/****************************/
{
    CCmdTarget::AssertValid();

    if( m_hWnd == NULL || m_hWnd == HWND_TOP || m_hWnd == HWND_BOTTOM ||
        m_hWnd == HWND_TOPMOST || m_hWnd == HWND_NOTOPMOST ) {
        return;
    }

    ASSERT( ::IsWindow( m_hWnd ) );
    ASSERT( m_hWndOwner == NULL || ::IsWindow( m_hWnd ) );
}

void CWnd::Dump( CDumpContext &dc ) const
/***************************************/
{
    CCmdTarget::Dump( dc );

    dc << "m_hWnd = " << m_hWnd << "\n";
    
    if( m_hWnd == NULL || m_hWnd == HWND_TOP || m_hWnd == HWND_BOTTOM ||
        m_hWnd == HWND_TOPMOST || m_hWnd == HWND_NOTOPMOST ) {
        return;
    }

    dc << "m_hWndOwner = " << m_hWndOwner << "\n";
    dc << "m_nFlags = " << m_nFlags << "\n";
    dc << "m_pfnSuper = " << m_pfnSuper << "\n";
}

#endif // _DEBUG

BOOL CWnd::Attach( HWND hWnd )
/****************************/
{
    ASSERT( m_hWnd == NULL );
    if( hWnd == NULL ) {
        return( FALSE );
    }
    m_hWnd = hWnd;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHWND;
    ASSERT( pHandleMap != NULL );
    pHandleMap->SetPermanent( hWnd, this );
    return( TRUE );
}

void CWnd::CenterWindow( CWnd *pAlternateOwner )
/**********************************************/
{
    CRect   rcParent;
    CRect   rcThis;
    if( pAlternateOwner != NULL ) {
        pAlternateOwner->GetWindowRect( &rcParent );
    } else {
        HWND hwndParent = ::GetParent( m_hWnd );
        if( hwndParent != NULL ) {
            ::GetWindowRect( hwndParent, &rcParent );
        } else {
            ::GetWindowRect( ::GetDesktopWindow(), &rcParent );
        }
    }
    ::GetWindowRect( m_hWnd, &rcThis );
    int x = rcParent.left + (rcParent.Width() - rcThis.Width()) / 2;
    int y = rcParent.top + (rcParent.Height() - rcThis.Height()) / 2;
    ::SetWindowPos( m_hWnd, NULL, x, y, 0, 0,
                    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER );
}

LRESULT CWnd::Default()
/*********************/
{
    const MSG *pMsg = GetCurrentMessage();
    ASSERT( pMsg != NULL );
    ASSERT( pMsg->hwnd == m_hWnd );
    return( DefWindowProc( pMsg->message, pMsg->wParam, pMsg->lParam ) );
}

HWND CWnd::Detach()
/*****************/
{
    if( m_hWnd == NULL ) {
        return( NULL );
    }
    HWND hWnd = m_hWnd;
    m_hWnd = NULL;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHWND;
    ASSERT( pHandleMap != NULL );
    pHandleMap->RemoveHandle( hWnd );
    return( hWnd );
}

BOOL CWnd::EnableScrollBar( int nSBFlags, UINT nArrowFlags )
/**********************************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nSBFlags );
    if( pScrollBar != NULL ) {
        return( pScrollBar->EnableScrollBar( nArrowFlags ) );
    } else {
        return( ::EnableScrollBar( m_hWnd, nSBFlags, nArrowFlags ) );
    }
}

void CWnd::EnableScrollBarCtrl( int nBar, BOOL bEnable )
/******************************************************/
{
    if( nBar == SB_BOTH ) {
        EnableScrollBarCtrl( SB_HORZ, bEnable );
        EnableScrollBarCtrl( SB_VERT, bEnable );
    } else {
        CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
        if( pScrollBar != NULL ) {
            pScrollBar->EnableWindow( bEnable );
        } else {
            ::ShowScrollBar( m_hWnd, nBar, bEnable );
        }
    }
}

BOOL CWnd::EnableToolTips( BOOL bEnable )
/***************************************/
{
    if( bEnable ) {
        if( !_DisableToolTips() ) {
            AFX_MODULE_STATE *pState = AfxGetModuleState();
            ASSERT( pState != NULL );
            pState->m_pfnFilterToolTipMessage = _FilterToolTipMessage;
            m_nFlags |= WF_TOOLTIPS;
        }
    } else {
        m_nFlags &= ~WF_TOOLTIPS;
    }
    return( TRUE );
}

BOOL CWnd::EnableTrackingToolTips( BOOL bEnable )
/***********************************************/
{
    if( bEnable ) {
        if( !_DisableToolTips() ) {
            AFX_MODULE_STATE *pState = AfxGetModuleState();
            ASSERT( pState != NULL );
            pState->m_pfnFilterToolTipMessage = _FilterToolTipMessage;
            m_nFlags |= WF_TRACKINGTOOLTIPS;
        }
    } else {
        m_nFlags &= ~WF_TRACKINGTOOLTIPS;
    }
    return( TRUE );
}

void CWnd::FilterToolTipMessage( MSG *pMsg )
/******************************************/
{
    ASSERT( pMsg != NULL );
    if( pMsg->message != WM_MOUSEMOVE && pMsg->message != WM_NCMOUSEMOVE &&
        pMsg->message != WM_LBUTTONDOWN && pMsg->message != WM_LBUTTONUP &&
        pMsg->message != WM_RBUTTONDOWN && pMsg->message != WM_RBUTTONUP &&
        pMsg->message != WM_MBUTTONDOWN && pMsg->message != WM_MBUTTONUP ) {
        return;
    }

    if( !(m_nFlags & (WF_TOOLTIPS | WF_TRACKINGTOOLTIPS)) ) {
        return;
    }

    if( ::GetKeyState( VK_LBUTTON ) < 0 || ::GetKeyState( VK_MBUTTON ) < 0 ||
        ::GetKeyState( VK_RBUTTON ) < 0 ) {
        CancelToolTips( TRUE );
        return;
    }
    
    HWND hWnd = pMsg->hwnd;
    while( hWnd != NULL && hWnd != m_hWnd ) {
        CWnd *pWnd = CWnd::FromHandlePermanent( hWnd );
        if( pWnd != NULL && pWnd->m_nFlags & (WF_TOOLTIPS | WF_TRACKINGTOOLTIPS) ) {
            return;
        }
        hWnd = ::GetParent( hWnd );
    }
            
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );

    CWnd *pOwner = GetParentOwner();
    ASSERT( pOwner != NULL );
    CToolTipCtrl *&pTip = pState->m_pToolTip;
    if( pTip != NULL && pTip->GetOwner() != pOwner ) {
        pTip->DestroyWindow();
        delete pTip;
        pTip = NULL;
    }

    if( pTip == NULL ) {
        pTip = new CToolTipCtrl;
        if( !pTip->Create( this, TTS_ALWAYSTIP ) ) {
            delete pTip;
            pTip = NULL;
            return;
        }
        pTip->Activate( FALSE );
    }

    CPoint point = pMsg->pt;
    ScreenToClient( &point );

    TOOLINFO    ti;
    memset( &ti, 0, sizeof( TOOLINFO ) );
    ti.cbSize = sizeof( TOOLINFO );
    
    INT_PTR     nHit = OnToolHitTest( point, &ti );
    if( pState->m_pLastHit != this || pState->m_nLastHit != nHit ) {
        if( nHit != -1 ) {
            if( m_nFlags & WF_TRACKINGTOOLTIPS ) {
                ti.uFlags |= TTF_TRACK;
            }
            pTip->SendMessage( TTM_ADDTOOL, 0, (LPARAM)&ti );
            pTip->SetWindowPos( &wndTopMost, 0, 0, 0, 0,
                                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
            pTip->Activate( TRUE );
            if( m_nFlags & WF_TRACKINGTOOLTIPS ) {
                pTip->SendMessage( TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti );
            }
        } else if( pState->m_pLastInfo != NULL ) {
            pTip->Activate( FALSE );
        }
        pTip->RelayEvent( pMsg );
        if( pState->m_pLastInfo != NULL ) {
            pTip->SendMessage( TTM_DELTOOL, 0, (LPARAM)pState->m_pLastInfo );
        }
        pState->m_pLastHit = this;
        pState->m_nLastHit = nHit;
        if( nHit != -1 ) {
            if( pState->m_pLastInfo == NULL ) {
                pState->m_pLastInfo = new TOOLINFO;
            }
            memcpy( pState->m_pLastInfo, &ti, sizeof( TOOLINFO ) );
        } else if( pState->m_pLastInfo != NULL ) {
            delete pState->m_pLastInfo;
            pState->m_pLastInfo = NULL;
        }
    } else if( m_nFlags & WF_TRACKINGTOOLTIPS ) {
        pTip->SendMessage( TTM_TRACKPOSITION, 0, MAKELPARAM( pMsg->pt.x, pMsg->pt.y ) );
    } else {
        pTip->RelayEvent( pMsg );
    }
}

int CWnd::GetCheckedRadioButton( int nIDFirstButton, int nIDLastButton )
/**********************************************************************/
{
    ASSERT( nIDFirstButton <= nIDLastButton );
    for( int i = nIDFirstButton; i <= nIDLastButton; i++ ) {
        if( ::IsDlgButtonChecked( m_hWnd, i ) ) {
            return( i );
        }
    }
    return( 0 );
}

CWnd* CWnd::GetDescendantWindow( int nID, BOOL bOnlyPerm ) const
/**************************************************************/
{
    HWND hWnd = ::GetTopWindow( m_hWnd );
    while( hWnd != NULL ) {
        if( ::GetDlgCtrlID( hWnd ) == nID ) {
            if( bOnlyPerm ) {
                return( CWnd::FromHandlePermanent( hWnd ) );
            } else {
                return( CWnd::FromHandle( hWnd ) );
            }
        }
        CWnd *pWnd = CWnd::FromHandle( hWnd );
        ASSERT( pWnd != NULL );
        pWnd = pWnd->GetDescendantWindow( nID, bOnlyPerm );
        if( pWnd != NULL ) {
            return( pWnd );
        }
        hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );
    }
    return( NULL );
}

CWnd *CWnd::GetOwner() const
/**************************/
{
    HWND hWndOwner = m_hWndOwner;
    if( hWndOwner == NULL ) {
        hWndOwner = ::GetParent( m_hWnd );
    }
    return( CWnd::FromHandle( hWndOwner ) );
}

CFrameWnd *CWnd::GetParentFrame() const
/*************************************/
{
    HWND hWnd = m_hWnd;
    for( ;; ) {
        if( ::GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD ) {
            hWnd = ::GetParent( hWnd );
        } else {
            hWnd = ::GetWindow( hWnd, GW_OWNER );
        }
        if( hWnd == NULL ) {
            return( NULL );
        }
        CWnd *pWnd = CWnd::FromHandlePermanent( hWnd );
        if( pWnd != NULL && pWnd->IsKindOf( RUNTIME_CLASS( CFrameWnd ) ) ) {
            return( (CFrameWnd *)pWnd );
        }
    }
}

CWnd *CWnd::GetParentOwner() const
/********************************/
{
    HWND hWnd = m_hWnd;
    while( ::GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD ) {
        HWND hWndParent = ::GetParent( hWnd );
        if( hWndParent == NULL ) {
            return( CWnd::FromHandle( hWnd ) );
        }
        hWnd = hWndParent;
    }
    return( CWnd::FromHandle( hWnd ) );
}

BOOL CWnd::GetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask )
/*************************************************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
    if( pScrollBar != NULL ) {
        return( pScrollBar->GetScrollInfo( lpScrollInfo, nMask ) );
    } else {
        ASSERT( lpScrollInfo != NULL );
        lpScrollInfo->cbSize = sizeof( SCROLLINFO );
        lpScrollInfo->fMask = nMask;
        return( ::GetScrollInfo( m_hWnd, nBar, lpScrollInfo ) );
    }
}

int CWnd::GetScrollPos( int nBar ) const
/**************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
    if( pScrollBar != NULL ) {
        return( pScrollBar->GetScrollPos() );
    } else {
        return( ::GetScrollPos( m_hWnd, nBar ) );
    }
}

void CWnd::GetScrollRange( int nBar, LPINT lpMinPos, LPINT lpMaxPos ) const
/*************************************************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
    if( pScrollBar != NULL ) {
        pScrollBar->GetScrollRange( lpMinPos, lpMaxPos );
    } else {
        ::GetScrollRange( m_hWnd, nBar, lpMinPos, lpMaxPos );
    }
}

CFrameWnd *CWnd::GetTopLevelFrame() const
/***************************************/
{
    CFrameWnd *pFrame;
    if( IsKindOf( RUNTIME_CLASS( CFrameWnd ) ) ) {
        pFrame = (CFrameWnd *)this;
    } else {
        pFrame = GetParentFrame();
    }
    if( pFrame == NULL ) {
        return( NULL );
    }
    for( ;; ) {
        CFrameWnd *pNext = pFrame->GetParentFrame();
        if( pNext == NULL ) {
            return( pFrame );
        }
        pFrame = pNext;
    }
}

CWnd *CWnd::GetTopLevelOwner() const
/**********************************/
{
    HWND hWnd = m_hWnd;
    if( hWnd == NULL ) {
        return( NULL );
    }
    for( ;; ) {
        HWND hWndOwner = ::GetWindow( hWnd, GW_OWNER );
        if( hWndOwner == NULL ) {
            return( CWnd::FromHandle( hWnd ) );
        }
        hWnd = hWndOwner;
    }
}

CWnd *CWnd::GetTopLevelParent() const
/***********************************/
{
    HWND hWnd = m_hWnd;
    if( hWnd == NULL ) {
        return( NULL );
    }
    HWND hWndParent;
    for( ;; ) {
        if( ::GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD ) {
            hWndParent = ::GetParent( hWnd );
        } else {
            hWndParent = ::GetWindow( hWnd, GW_OWNER );
        }
        if( hWndParent == NULL ) {
            return( CWnd::FromHandle( hWnd ) );
        }
        hWnd = hWndParent;
    }
}

BOOL CWnd::ModifyStyle( DWORD dwRemove, DWORD dwAdd, UINT nFlags )
/****************************************************************/
{
    DWORD dwStyle = ::GetWindowLong( m_hWnd, GWL_STYLE );
    dwStyle &= ~dwRemove;
    dwStyle |= dwAdd;
    if( ::SetWindowLong( m_hWnd, GWL_STYLE, dwStyle ) == 0 ) {
        return( FALSE );
    }
    nFlags |= SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER;
    ::SetWindowPos( m_hWnd, NULL, 0, 0, 0, 0, nFlags );
    return( TRUE );
}

BOOL CWnd::ModifyStyleEx( DWORD dwRemove, DWORD dwAdd, UINT nFlags )
/******************************************************************/
{
    DWORD dwExStyle = ::GetWindowLong( m_hWnd, GWL_EXSTYLE );
    dwExStyle &= ~dwRemove;
    dwExStyle |= dwAdd;
    if( ::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle ) == 0 ) {
        return( FALSE );
    }
    nFlags |= SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER;
    ::SetWindowPos( m_hWnd, NULL, 0, 0, 0, 0, nFlags );
    return( TRUE );
}

void CWnd::PrepareForHelp()
/*************************/
{
    if( IsFrameWnd() ) {
        CFrameWnd *pFrame = (CFrameWnd *)this;
        ASSERT( pFrame->IsKindOf( RUNTIME_CLASS( CFrameWnd ) ) );
        pFrame->ExitHelpMode();
    }
}

BOOL CWnd::ReflectChildNotify( UINT message, WPARAM wParam, LPARAM lParam,
                               LRESULT *pResult )
/***********************************************/
{
    if( message == WM_COMMAND ) {
        if( OnCmdMsg( 0, MAKELONG( HIWORD( wParam ), WM_COMMAND + WM_REFLECT_BASE ),
                      NULL, NULL ) ) {
            return( TRUE );
        }
    } else if( message == WM_NOTIFY ) {
        NMHDR *pNMHDR = (NMHDR *)lParam;
        ASSERT( pNMHDR != NULL );
        AFX_NOTIFY params;
        params.pResult = pResult;
        params.pNMHDR = pNMHDR;
        if( OnCmdMsg( 0, MAKELONG( pNMHDR->code, WM_NOTIFY + WM_REFLECT_BASE ),
                      &params, NULL ) ) {
            return( TRUE );
        }
    } else if( message == WM_CTLCOLORBTN || message == WM_CTLCOLORDLG ||
               message == WM_CTLCOLORLISTBOX || message == WM_CTLCOLORMSGBOX ||
               message == WM_CTLCOLORSCROLLBAR || message == WM_CTLCOLORSTATIC ) {
        AFX_CTLCOLOR acc;
        acc.hDC = (HDC)wParam;
        acc.nCtlType = _TranslateCtlType( message );
        if( OnWndMsg( WM_CTLCOLOR + WM_REFLECT_BASE, 0, (LPARAM)&acc, pResult ) ) {
            return( TRUE );
        }
    } else {
        if( OnWndMsg( message + WM_REFLECT_BASE, wParam, lParam, pResult ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

void CWnd::RepositionBars( UINT nIDFirst, UINT nIDLast, UINT nIDLeftOver, UINT nFlag,
                           LPRECT lpRectParam, LPCRECT lpRectClient, BOOL bStretch )
/**********************************************************************************/
{
    AFX_SIZEPARENTPARAMS    spp;
    if( nFlag == reposQuery ) {
        spp.hDWP = NULL;
    } else {
        spp.hDWP = ::BeginDeferWindowPos( 8 );
    }
    if( lpRectClient == NULL ) {
        ::GetClientRect( m_hWnd, &spp.rect );
    } else {
        ::CopyRect( &spp.rect, lpRectClient );
    }
    spp.sizeTotal.cx = 0;
    spp.sizeTotal.cy = 0;
    spp.bStretch = bStretch;

    HWND hWnd = ::GetTopWindow( m_hWnd );
    HWND hWndLeftOver = NULL;
    while( hWnd != NULL ) {
        UINT nID = ::GetDlgCtrlID( hWnd );
        if( nID == nIDLeftOver ) {
            hWndLeftOver = hWnd;
        } else if( nID >= nIDFirst && nID <= nIDLast ) {
            if( CWnd::FromHandlePermanent( hWnd ) != NULL ) {
                ::SendMessage( hWnd, WM_SIZEPARENT, 0, (LPARAM)&spp );
            }
        }
        hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );
    }

    if( nFlag == reposQuery ) {
        ASSERT( lpRectParam != NULL );
        if( bStretch ) {
            ::CopyRect( lpRectParam, &spp.rect );
        } else {
            lpRectParam->left = 0;
            lpRectParam->top = 0;
            lpRectParam->right = spp.sizeTotal.cx;
            lpRectParam->bottom = spp.sizeTotal.cy;
        }
    } else {
        if( hWndLeftOver != NULL ) {
            if( nFlag == reposExtra ) {
                ASSERT( lpRectParam != NULL );
                spp.rect.left += lpRectParam->left;
                spp.rect.top += lpRectParam->top;
                spp.rect.right -= lpRectParam->right;
                spp.rect.bottom -= lpRectParam->bottom;
            }
            AfxRepositionWindow( &spp, hWndLeftOver, &spp.rect );
        }
        ::EndDeferWindowPos( spp.hDWP );
    }
}

BOOL CWnd::SendChildNotifyLastMsg( LRESULT *pResult )
/***************************************************/
{
    const MSG *pMsg = CWnd::GetCurrentMessage();
    return( OnChildNotify( pMsg->message, pMsg->wParam, pMsg->lParam, pResult ) );
}

void CWnd::SendMessageToDescendants( UINT message, WPARAM wParam, LPARAM lParam,
                                     BOOL bDeep, BOOL bOnlyPerm )
/***************************************************************/
{
    HWND hWnd = ::GetTopWindow( m_hWnd );
    while( hWnd != NULL ) {
        if( !bOnlyPerm || CWnd::FromHandlePermanent( hWnd ) != NULL ) {
            ::SendMessage( hWnd, message, wParam, lParam );
        }
        if( bDeep ) {
            CWnd *pWnd = CWnd::FromHandle( hWnd );
            ASSERT( pWnd != NULL );
            pWnd->SendMessageToDescendants( message, wParam, lParam, bDeep, bOnlyPerm );
        }
        hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );
    }
}

BOOL CWnd::SetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw )
/***************************************************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
    if( pScrollBar != NULL ) {
        return( pScrollBar->SetScrollInfo( lpScrollInfo, bRedraw ) );
    } else {
        ASSERT( lpScrollInfo != NULL );
        lpScrollInfo->cbSize = sizeof( SCROLLINFO );
        return( ::SetScrollInfo( m_hWnd, nBar, lpScrollInfo, bRedraw ) );
    }
}

int CWnd::SetScrollPos( int nBar, int nPos, BOOL bRedraw )
/********************************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
    if( pScrollBar != NULL ) {
        return( pScrollBar->SetScrollPos( nPos, bRedraw ) );
    } else {
        return( ::SetScrollPos( m_hWnd, nBar, nPos, bRedraw ) );
    }
}

void CWnd::SetScrollRange( int nBar, int nMinPos, int nMaxPos, BOOL bRedraw )
/***************************************************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
    if( pScrollBar != NULL ) {
        pScrollBar->SetScrollRange( nMinPos, nMaxPos, bRedraw );
    } else {
        ::SetScrollRange( m_hWnd, nBar, nMinPos, nMaxPos, bRedraw );
    }
}

void CWnd::ShowScrollBar( UINT nBar, BOOL bShow )
/***********************************************/
{
    CScrollBar *pScrollBar = GetScrollBarCtrl( nBar );
    if( pScrollBar != NULL ) {
        pScrollBar->ShowScrollBar( bShow );
    } else {
        ::ShowScrollBar( m_hWnd, nBar, bShow );
    }
}

BOOL CWnd::SubclassWindow( HWND hWnd )
/************************************/
{
    if( !Attach( hWnd ) ) {
        return( FALSE );
    }
    PreSubclassWindow();
    m_pfnSuper = (WNDPROC)::GetWindowLong( hWnd, GWL_WNDPROC );
    ASSERT( m_pfnSuper != NULL );
    ::SetWindowLong( hWnd, GWL_WNDPROC, (DWORD)(WNDPROC)AfxWndProc );
    return( TRUE );
}

HWND CWnd::UnsubclassWindow()
/***************************/
{
    if( m_hWnd == NULL || m_pfnSuper == NULL ) {
        return( NULL );
    }
    ::SetWindowLong( m_hWnd, GWL_WNDPROC, (DWORD)m_pfnSuper );
    m_pfnSuper = NULL;
    return( Detach() );
}

BOOL CWnd::UpdateData( BOOL bSaveAndValidate )
/********************************************/
{
    CDataExchange dx( this, bSaveAndValidate );
    try {
        DoDataExchange( &dx );
    } catch( CUserException *pUserEx ) {
        return( FALSE );
    }
    return( TRUE );
}

void CWnd::UpdateDialogControls( CCmdTarget *pTarget, BOOL bDisableIfNoHndler )
/*****************************************************************************/
{
    CCmdUI              cui;
    AFX_CMDHANDLERINFO  info;
    HWND                hWndCtrl = ::GetTopWindow( m_hWnd );
    while( hWndCtrl != NULL ) {
        cui.m_nID = ::GetDlgCtrlID( hWndCtrl );
        cui.m_pOther = CWnd::FromHandle( hWndCtrl );
        if( !OnCmdMsg( cui.m_nID, CN_UPDATE_COMMAND_UI, &cui, NULL ) &&
            !cui.m_pOther->OnCmdMsg( 0, MAKELONG( (WORD)CN_UPDATE_COMMAND_UI,
                                                  WM_COMMAND + WM_REFLECT_BASE ),
                                     &cui, NULL ) &&
            !pTarget->OnCmdMsg( cui.m_nID, CN_UPDATE_COMMAND_UI, &cui, NULL ) ) {
            if( bDisableIfNoHndler &&
                (cui.m_pOther->SendMessage( WM_GETDLGCODE ) & DLGC_BUTTON) &&
                !pTarget->OnCmdMsg( cui.m_nID, CN_COMMAND, NULL, &info ) ) {
                UINT nType = cui.m_pOther->GetStyle() & BS_TYPEMASK;
                if( nType != BS_AUTOCHECKBOX && nType != BS_AUTO3STATE &&
                    nType != BS_GROUPBOX && nType != BS_AUTORADIOBUTTON ) {
                    ::EnableWindow( hWndCtrl, FALSE );
                }
            }
        }
        hWndCtrl = ::GetWindow( hWndCtrl, GW_HWNDNEXT );
    }
}

void PASCAL CWnd::CancelToolTips( BOOL bKeys )
/********************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    if( pState->m_pToolTip != NULL ) {
        pState->m_pToolTip->Activate( FALSE );
    }
    if( bKeys && ::GetKeyState( VK_LBUTTON ) >= 0 && pState->m_pLastStatus != NULL ) {
        CWnd *pWnd = AfxGetMainWnd();
        if( pWnd != NULL ) {
            pWnd->SendMessage( WM_POPMESSAGESTRING, (WPARAM)-1 );
        }
    }
}

void PASCAL CWnd::DeleteTempMap()
/*******************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHWND;
    ASSERT( pHandleMap != NULL );
    pHandleMap->DeleteTemp();
}

CWnd * PASCAL CWnd::FromHandle( HWND hWnd )
/*****************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHWND;
    ASSERT( pHandleMap != NULL );
    return( (CWnd *)pHandleMap->FromHandle( hWnd ) );
}

CWnd * PASCAL CWnd::FromHandlePermanent( HWND hWnd )
/**************************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHWND;
    ASSERT( pHandleMap != NULL );
    return( (CWnd *)pHandleMap->LookupPermanent( hWnd ) );
}

const MSG * PASCAL CWnd::GetCurrentMessage()
/******************************************/
{
    _AFX_THREAD_STATE *pState = AfxGetThreadState();
    ASSERT( pState != NULL );
    return( &pState->m_msgCur );
}

BOOL PASCAL CWnd::ReflectLastMsg( HWND hWndChild, LRESULT *pResult )
/******************************************************************/
{
    CWnd *pWnd = CWnd::FromHandlePermanent( hWndChild );
    if( pWnd == NULL ) {
        return( FALSE );
    }
    return( pWnd->SendChildNotifyLastMsg( pResult ) );
}

void PASCAL CWnd::_FilterToolTipMessage( MSG *pMsg, CWnd *pWnd )
/**************************************************************/
{
    ASSERT( pWnd != NULL );
    pWnd->FilterToolTipMessage( pMsg );
}

int CWnd::OnCharToItem( UINT nChar, CListBox *pListBox, UINT nIndex )
/*******************************************************************/
{
    UNUSED_ALWAYS( nChar );
    UNUSED_ALWAYS( nIndex );
    
    LRESULT lResult;
    if( pListBox->SendChildNotifyLastMsg( &lResult ) ) {
        return( (int)lResult );
    }
    return( (int)Default() );
}

int CWnd::OnCompareItem( int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct )
/****************************************************************************/
{
    UNUSED_ALWAYS( nIDCtl );

    LRESULT lResult;
    if( ReflectLastMsg( lpCompareItemStruct->hwndItem, &lResult ) ) {
        return( (int)lResult );
    }
    return( (int)Default() );
}

HBRUSH CWnd::OnCtlColor( CDC *pDC, CWnd *pWnd, UINT nCtlType )
/************************************************************/
{
    UNUSED_ALWAYS( pDC );
    UNUSED_ALWAYS( nCtlType );

    LRESULT lResult;
    if( pWnd->SendChildNotifyLastMsg( &lResult ) ) {
        return( (HBRUSH)lResult );
    }
    return( (HBRUSH)Default() );
}

void CWnd::OnDeleteItem( int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct )
/**************************************************************************/
{
    UNUSED_ALWAYS( nIDCtl );

    if( !ReflectLastMsg( lpDeleteItemStruct->hwndItem ) ) {
        Default();
    }
}

void CWnd::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )
/********************************************************************/
{
    UNUSED_ALWAYS( nIDCtl );

    if( lpDrawItemStruct->CtlType == ODT_MENU ) {
        CMenu *pMenu = CMenu::FromHandlePermanent( (HMENU)lpDrawItemStruct->hwndItem );
        if( pMenu != NULL ) {
            pMenu->DrawItem( lpDrawItemStruct );
        } else {
            Default();
        }
    } else if( !ReflectLastMsg( lpDrawItemStruct->hwndItem ) ) {
        Default();
    }
}

void CWnd::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
/*********************************************************************/
{
    UNUSED_ALWAYS( nSBCode );
    UNUSED_ALWAYS( nPos );

    if( pScrollBar == NULL || !pScrollBar->SendChildNotifyLastMsg() ) {
        Default();
    }
}

void CWnd::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct )
/*****************************************************************************/
{
    UNUSED_ALWAYS( lpMeasureItemStruct );

    if( lpMeasureItemStruct->CtlType == ODT_MENU ) {
        _AFX_THREAD_STATE *pState = AfxGetThreadState();
        ASSERT( pState != NULL );
        HMENU hMenu = NULL;
        if( pState->m_hTrackingWindow == m_hWnd ) {
            hMenu = pState->m_hTrackingMenu;
        }
        if( hMenu == NULL ) {
            hMenu = ::GetMenu( m_hWnd );
        }
        hMenu = _FindPopupMenu( hMenu, lpMeasureItemStruct->itemID );
        CMenu *pMenu = CMenu::FromHandlePermanent( hMenu );
        if( pMenu != NULL ) {
            pMenu->MeasureItem( lpMeasureItemStruct );
        } else {
            Default();
        }
    } else {
        CWnd *pWnd = GetDlgItem( nIDCtl );
        if( pWnd == NULL || !pWnd->SendChildNotifyLastMsg() ) {
            Default();
        }
    }
}

void CWnd::OnNcDestroy()
/**********************/
{
    CWinThread *pThread = AfxGetThread();
    if( pThread != NULL && pThread->m_pMainWnd == this ) {
        pThread->m_pMainWnd = NULL;
        ::PostQuitMessage( 0 );
    }
    
    Default();
    UnsubclassWindow();
    PostNcDestroy();
}

void CWnd::OnParentNotify( UINT message, LPARAM lParam )
/******************************************************/
{
    HWND hWnd;
    if( message == WM_CREATE || message == WM_DESTROY ) {
        hWnd = (HWND)lParam;
    } else {
        hWnd = ::ChildWindowFromPoint( m_hWnd, CPoint( LOWORD( lParam ),
                                                       HIWORD( lParam ) ) );
    }
    if( !ReflectLastMsg( hWnd ) ) {
        Default();
    }
}

int CWnd::OnVKeyToItem( UINT nChar, CListBox *pListBox, UINT nIndex )
/*******************************************************************/
{
    UNUSED_ALWAYS( nChar );
    UNUSED_ALWAYS( nIndex );
    
    LRESULT lResult;
    if( pListBox->SendChildNotifyLastMsg( &lResult ) ) {
        return( (int)lResult );
    }
    return( (int)Default() );
}

void CWnd::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar )
/*********************************************************************/
{
    UNUSED_ALWAYS( nSBCode );
    UNUSED_ALWAYS( nPos );

    if( pScrollBar == NULL || !pScrollBar->SendChildNotifyLastMsg() ) {
        Default();
    }
}

LRESULT CWnd::OnDragList( WPARAM wParam, LPARAM lParam )
/******************************************************/
{
    UNUSED_ALWAYS( wParam );
    
    LRESULT         lResult;
    DRAGLISTINFO    *pInfo = (DRAGLISTINFO *)lParam;
    ASSERT( pInfo != NULL );
    if( ReflectLastMsg( pInfo->hWnd, &lResult ) ) {
        return( lResult );
    }
    return( Default() );
}

LRESULT CWnd::OnNTCtlColor( WPARAM wParam, LPARAM lParam )
/********************************************************/
{
    AFX_CTLCOLOR acc;
    acc.hWnd = (HWND)lParam;
    acc.hDC = (HDC)wParam;
    acc.nCtlType = _TranslateCtlType( GetCurrentMessage()->message );
    return( WindowProc( WM_CTLCOLOR, 0, (LPARAM)&acc ) );
}

void CWnd::OnHelp()
/*****************/
{
    if( !::SendMessage( m_hWnd, WM_COMMANDHELP, 0, 0L ) ) {
        ::SendMessage( m_hWnd, WM_COMMAND, ID_DEFAULT_HELP, 0L );
    }
}

void CWnd::OnHelpFinder()
/***********************/
{
    WinHelpInternal( 0, HELP_FINDER );
}

void CWnd::OnHelpIndex()
/**********************/
{
    WinHelpInternal( 0, HELP_INDEX );
}

void CWnd::OnHelpUsing()
/**********************/
{
    WinHelpInternal( 0, HELP_HELPONHELP );
}

void AFXAPI AfxRepositionWindow( AFX_SIZEPARENTPARAMS *lpLayout, HWND hWnd,
                                 LPCRECT lpRect )
/***********************************************/
{
    ASSERT( lpLayout != NULL );
    ASSERT( lpRect != NULL );
    if( lpLayout->hDWP != NULL ) {
        ::DeferWindowPos( lpLayout->hDWP, hWnd, NULL, lpRect->left, lpRect->top,
                          lpRect->right - lpRect->left, lpRect->bottom - lpRect->top,
                          SWP_NOACTIVATE | SWP_NOZORDER );
    } else {
        ::SetWindowPos( hWnd, NULL, lpRect->left, lpRect->top,
                        lpRect->right - lpRect->left, lpRect->bottom - lpRect->top,
                        SWP_NOACTIVATE | SWP_NOZORDER );
    }
}
