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
* Description:  Implementation of CToolBar.
*
****************************************************************************/


#include "stdafx.h"
#include "toolcui.h"

// System color data
struct SYSCOLORDATA {
    RGBQUAD rgbq;
    DWORD   dwColorIndex;
};

// Size of system color map
#define NUM_SYSCOLORS 4

// System color map
static SYSCOLORDATA _Colors[NUM_SYSCOLORS] = {
    { { 0x00, 0x00, 0x00, 0x00 }, COLOR_BTNTEXT },
    { { 0x80, 0x80, 0x80, 0x00 }, COLOR_BTNSHADOW },
    { { 0xC0, 0xC0, 0xC0, 0x00 }, COLOR_BTNFACE },
    { { 0xFF, 0xFF, 0xFF, 0x00 }, COLOR_BTNHIGHLIGHT }
};

// Toolbar resource header
struct TOOLBAR_HEADER {
    WORD    nVersion;
    WORD    nWidth;
    WORD    nHeight;
    WORD    nCount;
};

IMPLEMENT_DYNAMIC( CToolBar, CControlBar )

BEGIN_MESSAGE_MAP( CToolBar, CControlBar )
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_PAINT()
    ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()

CToolBar::CToolBar()
/******************/
{
    m_hRsrcImageWell = NULL;
    m_hInstImageWell = NULL;
    m_hbmImageWell = NULL;
    m_sizeButton.cx = 23;
    m_sizeButton.cy = 22;
    m_sizeImage.cx = 16;
    m_sizeImage.cy = 15;
}

CToolBar::~CToolBar()
/*******************/
{
    if( m_hbmImageWell != NULL ) {
        ::DeleteObject( m_hbmImageWell );
    }
}

BOOL CToolBar::Create( CWnd *pParentWnd, DWORD dwStyle, UINT nID )
/****************************************************************/
{
    return( CreateEx( pParentWnd, 0L, dwStyle, CRect( 0, 0, 0, 0 ), nID ) );
}

BOOL CToolBar::CreateEx( CWnd *pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle,
                         CRect rcBorders, UINT nID )
/**************************************************/
{
    m_dwStyle = dwStyle & CBRS_ALL;
    dwStyle &= ~CBRS_ALL;
    dwStyle |= CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | TBSTYLE_WRAPABLE;
    m_cxLeftBorder = rcBorders.left;
    m_cxRightBorder = rcBorders.right;
    m_cyTopBorder = rcBorders.top;
    m_cyBottomBorder = rcBorders.bottom;
    if( !CWnd::CreateEx( 0L, TOOLBARCLASSNAME, NULL, dwStyle | dwCtrlStyle,
                         CRect( 0, 0, 0, 0 ), pParentWnd, nID ) ) {
        return( FALSE );
    }
    SetSizes( m_sizeButton, m_sizeImage );
    return( TRUE );
}

void CToolBar::GetItemRect( int nIndex, LPRECT lpRect ) const
/***********************************************************/
{
    ::SendMessage( m_hWnd, TB_GETITEMRECT, nIndex, (LPARAM)lpRect );
}

CSize CToolBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
/**********************************************************/
{
    CSize sizeButton( ::SendMessage( m_hWnd, TB_GETBUTTONSIZE, 0, 0L ) );
    CSize size( 0, 0 );
    if( bHorz ) {
        size.cy = sizeButton.cy;
    } else {
        size.cx = sizeButton.cx;
    }
    if( bStretch ) {
        if( bHorz ) {
            size.cx = 32767;
        } else {
            size.cy = 32767;
        }
    } else {
        TBBUTTON    tbb;
        int         nCount = (int)::SendMessage( m_hWnd, TB_BUTTONCOUNT, 0, 0L );
        for( int i = 0; i < nCount; i++ ) {
            ::SendMessage( m_hWnd, TB_GETBUTTON, i, (LPARAM)&tbb );
            if( tbb.fsStyle & TBSTYLE_SEP ) {
                if( bHorz ) {
                    size.cx += tbb.iBitmap;
                } else {
                    size.cy += tbb.iBitmap;
                }
            } else {
                if( bHorz ) {
                    size.cx += sizeButton.cx;
                } else {
                    size.cy += sizeButton.cy;
                }
            }
        }
    }

    CRect rect;
    rect.SetRectEmpty();
    CalcInsideRect( rect, bHorz );
    size.cx -= rect.Width();
    size.cy -= rect.Height();
    
    return( size );
}

INT_PTR CToolBar::OnToolHitTest( CPoint point, TOOLINFO *pTI ) const
/******************************************************************/
{
    int nCount = (int)::SendMessage( m_hWnd, TB_BUTTONCOUNT, 0, 0L );
    for( int i = 0; i < nCount; i++ ) {
        RECT rc;
        ::SendMessage( m_hWnd, TB_GETITEMRECT, i, (LPARAM)&rc );
        if( ::PtInRect( &rc, point ) ) {
            TBBUTTON tbb;
            ::SendMessage( m_hWnd, TB_GETBUTTON, i, (LPARAM)&tbb );
            if( tbb.fsStyle & TBSTYLE_SEP ) {
                return( -1 );
            } else {
                if( pTI != NULL ) {
                    pTI->hwnd = m_hWnd;
                    pTI->uId = tbb.idCommand;
                    ::CopyRect( &pTI->rect, &rc );
                    pTI->lpszText = LPSTR_TEXTCALLBACK;
                }
                if( tbb.idCommand == 0 ) {
                    return( -1 );
                }
                return( tbb.idCommand );
            }
        }
    }
    return( -1 );
}

void CToolBar::OnUpdateCmdUI( CFrameWnd *pTarget, BOOL bDisableIfNoHndler )
/*************************************************************************/
{
    AFX_CMDHANDLERINFO  chi;
    CToolCmdUI          cui;
    cui.m_pOther = this;
    int nCount = (int)::SendMessage( m_hWnd, TB_BUTTONCOUNT, 0, 0L );
    for( int i = 0; i < nCount; i++ ) {
        cui.m_nIndex = i;
        cui.m_nID = GetItemID( i );
        cui.m_bContinueRouting = FALSE;
        if( cui.m_nID != ID_SEPARATOR ) {
            if( !pTarget->OnCmdMsg( cui.m_nID, CN_UPDATE_COMMAND_UI, &cui, NULL ) ) {
                if( bDisableIfNoHndler ) {
                    cui.Enable( pTarget->OnCmdMsg( cui.m_nID, CN_COMMAND, NULL, &chi ) );
                }
            }
        }
    }
}

#ifdef _DEBUG

void CToolBar::Dump( CDumpContext &dc ) const
/*******************************************/
{
    CControlBar::Dump( dc );

    dc << "m_hRsrcImageWell = " << m_hRsrcImageWell << "\n";
    dc << "m_hInstImageWell = " << m_hInstImageWell << "\n";
    dc << "m_hbmImageWell = " << m_hbmImageWell << "\n";
    dc << "m_sizeImage = " << m_sizeImage << "\n";
    dc << "m_sizeButton = " << m_sizeButton << "\n";
}

#endif // _DEBUG

void CToolBar::GetButtonInfo( int nIndex, UINT &nID, UINT &nStyle, int &iImage ) const
/************************************************************************************/
{
    TBBUTTON tbb;
    ::SendMessage( m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)&tbb );
    nID = tbb.idCommand;
    nStyle = MAKELONG( tbb.fsStyle, tbb.fsState ) ^ TBBS_DISABLED;
    iImage = tbb.iBitmap;
}

UINT CToolBar::GetButtonStyle( int nIndex ) const
/***********************************************/
{
    TBBUTTON tbb;
    ::SendMessage( m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)&tbb );
    return( MAKELONG( tbb.fsStyle, tbb.fsState ) ^ TBBS_DISABLED );
}

CString CToolBar::GetButtonText( int nIndex ) const
/*************************************************/
{
    TCHAR szBuffer[1024];
    ::SendMessage( m_hWnd, TB_GETBUTTONTEXT, nIndex, (LPARAM)szBuffer );
    return( szBuffer );
}

UINT CToolBar::GetItemID( int nIndex ) const
/******************************************/
{
    TBBUTTON tbb;
    ::SendMessage( m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)&tbb );
    return( tbb.idCommand );
}

BOOL CToolBar::LoadBitmap( LPCTSTR lpszResourceName )
/***************************************************/
{
    m_hInstImageWell = AfxFindResourceHandle( lpszResourceName, RT_BITMAP );
    m_hRsrcImageWell = ::FindResource( m_hInstImageWell, lpszResourceName, RT_BITMAP );
    if( m_hRsrcImageWell == NULL ) {
        m_hInstImageWell = NULL;
        return( FALSE );
    }
    m_hbmImageWell = AfxLoadSysColorBitmap( m_hInstImageWell, m_hRsrcImageWell );
    if( m_hbmImageWell == NULL ) {
        m_hInstImageWell = NULL;
        m_hRsrcImageWell = NULL;
        return( FALSE );
    }
    
    BITMAP bm;
    ::GetObject( m_hbmImageWell, sizeof( BITMAP ), &bm );
    
    TBADDBITMAP tbab;
    tbab.hInst = NULL;
    tbab.nID = (UINT_PTR)m_hbmImageWell;
    ::SendMessage( m_hWnd, TB_BUTTONSTRUCTSIZE, sizeof( TBBUTTON ), 0L );
    ::SendMessage( m_hWnd, TB_ADDBITMAP, bm.bmWidth / m_sizeImage.cx, (LPARAM)&tbab );
    return( TRUE );
}

BOOL CToolBar::LoadToolBar( LPCTSTR lpszResourceName )
/****************************************************/
{
    HINSTANCE hInstance = AfxFindResourceHandle( lpszResourceName, RT_TOOLBAR );
    HRSRC hResource = ::FindResource( hInstance, lpszResourceName, RT_TOOLBAR );
    if( hResource == NULL ) {
        return( FALSE );
    }
    HGLOBAL hMem = ::LoadResource( hInstance, hResource );
    if( hMem == NULL ) {
        return( FALSE );
    }
    TOOLBAR_HEADER *pHeader = (TOOLBAR_HEADER *)::LockResource( hMem );
    if( pHeader == NULL || pHeader->nVersion != 1 ) {
        ::FreeResource( hMem );
        return( FALSE );
    }
    m_sizeImage.cx = pHeader->nWidth;
    m_sizeImage.cy = pHeader->nHeight;
    ::SendMessage( m_hWnd, TB_SETBITMAPSIZE, 0, MAKELPARAM( m_sizeImage.cx,
                                                            m_sizeImage.cy ) );
    if( !LoadBitmap( lpszResourceName ) ) {
        ::FreeResource( hMem );
        return( FALSE );
    }
    WORD *pIDArray1 = (WORD *)(pHeader + 1);
    UINT *pIDArray2 = new UINT[pHeader->nCount];
    for( int i = 0; i < pHeader->nCount; i++ ) {
        pIDArray2[i] = pIDArray1[i];
    }
    SetButtons( pIDArray2, pHeader->nCount );
    delete [] pIDArray2;
    ::FreeResource( hMem );
    return( TRUE );
}

BOOL CToolBar::SetBitmap( HBITMAP hbmImageWell )
/**********************************************/
{
    TBREPLACEBITMAP tbrb;
    BITMAP          bm;
    ::GetObject( hbmImageWell, sizeof( BITMAP ), &bm );
    tbrb.hInstOld = NULL;
    tbrb.nIDOld = (UINT_PTR)m_hbmImageWell;
    tbrb.hInstNew = NULL;
    tbrb.nIDNew = (UINT_PTR)hbmImageWell;
    tbrb.nButtons = bm.bmWidth / m_sizeImage.cx;
    ::SendMessage( m_hWnd, TB_REPLACEBITMAP, 0, (LPARAM)&tbrb );
    ::DeleteObject( m_hbmImageWell );
    m_hbmImageWell = hbmImageWell;
    return( TRUE );
}

void CToolBar::SetButtonInfo( int nIndex, UINT nID, UINT nStyle, int iImage )
/***************************************************************************/
{
    TBBUTTON tbb;
    ::SendMessage( m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)&tbb );
    nStyle ^= TBBS_DISABLED;
    if( tbb.iBitmap != iImage || tbb.idCommand != nID ||
        tbb.fsState != HIWORD( nStyle ) || tbb.fsStyle != LOWORD( nStyle ) ) {
        tbb.iBitmap = iImage;
        tbb.idCommand = nID;
        tbb.fsState = (BYTE)HIWORD( nStyle );
        tbb.fsStyle = (BYTE)LOWORD( nStyle );
        ::SendMessage( m_hWnd, WM_SETREDRAW, FALSE, 0L );
        ::SendMessage( m_hWnd, TB_DELETEBUTTON, nIndex, 0L );
        ::SendMessage( m_hWnd, TB_INSERTBUTTON, nIndex, (LPARAM)&tbb );
        ::SendMessage( m_hWnd, WM_SETREDRAW, TRUE, 0L );
        ::InvalidateRect( m_hWnd, NULL, TRUE );
    }
}

void CToolBar::SetButtonStyle( int nIndex, UINT nStyle )
/******************************************************/
{
    TBBUTTON tbb;
    ::SendMessage( m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)&tbb );
    nStyle ^= TBBS_DISABLED;
    if( tbb.fsState != HIWORD( nStyle ) || tbb.fsStyle != LOWORD( nStyle ) ) {
        tbb.fsState = (BYTE)HIWORD( nStyle );
        tbb.fsStyle = (BYTE)LOWORD( nStyle );
        ::SendMessage( m_hWnd, WM_SETREDRAW, FALSE, 0L );
        ::SendMessage( m_hWnd, TB_DELETEBUTTON, nIndex, 0L );
        ::SendMessage( m_hWnd, TB_INSERTBUTTON, nIndex, (LPARAM)&tbb );
        ::SendMessage( m_hWnd, WM_SETREDRAW, TRUE, 0L );
        ::InvalidateRect( m_hWnd, NULL, TRUE );
    }
}

BOOL CToolBar::SetButtonText( int nIndex, LPCTSTR lpszText )
/**********************************************************/
{
    TBBUTTON tbb;
    ::SendMessage( m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)&tbb );
    tbb.iString = (INT_PTR)::SendMessage( m_hWnd, TB_ADDSTRING, 0, (LPARAM)lpszText );
    if( tbb.iString < 0 ) {
        return( FALSE );
    }
    ::SendMessage( m_hWnd, TB_DELETEBUTTON, nIndex, 0L );
    ::SendMessage( m_hWnd, TB_INSERTBUTTON, nIndex, (LPARAM)&tbb );
    ::InvalidateRect( m_hWnd, NULL, TRUE );
    return( TRUE );
}

BOOL CToolBar::SetButtons( const UINT *lpIDArray, int nIDCount )
/**************************************************************/
{
    ASSERT( lpIDArray != NULL );
    ASSERT( nIDCount > 0 );
    TBBUTTON *lpTBB = new TBBUTTON[nIDCount];
    int iBitmap = 0;
    for( int i = 0; i < nIDCount; i++ ) {
        if( lpIDArray[i] == ID_SEPARATOR ) {
            lpTBB[i].iBitmap = 0;
            lpTBB[i].idCommand = 0;
            lpTBB[i].fsStyle = TBSTYLE_SEP;
        } else {
            lpTBB[i].iBitmap = iBitmap;
            lpTBB[i].idCommand = lpIDArray[i];
            lpTBB[i].fsStyle = 0;
            iBitmap++;
        }
        lpTBB[i].fsState = TBSTATE_ENABLED;
        lpTBB[i].dwData = 0L;
        lpTBB[i].iString = -1;
    }
    ::SendMessage( m_hWnd, TB_ADDBUTTONS, nIDCount, (LPARAM)lpTBB );
    delete [] lpTBB;
    return( TRUE );
}

void CToolBar::SetSizes( SIZE sizeButton, SIZE sizeImage )
/********************************************************/
{
    m_sizeButton = sizeButton;
    m_sizeImage = sizeImage;
    if( m_hWnd != NULL ) {
        ::SendMessage( m_hWnd, TB_SETBUTTONSIZE, 0, MAKELPARAM( m_sizeButton.cx,
                                                                m_sizeButton.cy ) );
        ::SendMessage( m_hWnd, TB_SETBITMAPSIZE, 0, MAKELPARAM( m_sizeImage.cx,
                                                                m_sizeImage.cy ) );
    }
}

void CToolBar::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp )
/****************************************************************************/
{
    UNUSED_ALWAYS( bCalcValidRects );

    ASSERT( lpncsp != NULL );
    CRect rect;
    CalcInsideRect( rect, m_dwStyle & CBRS_ORIENT_HORZ );
    lpncsp->rgrc[0].left += rect.left;
    lpncsp->rgrc[0].top += rect.top;
    lpncsp->rgrc[0].right += rect.right;
    lpncsp->rgrc[0].bottom += rect.bottom;
}

void CToolBar::OnNcPaint()
/************************/
{
    EraseNonClient();
}

void CToolBar::OnPaint()
/**********************/
{
    // Bypass CControlBar::OnPaint and have the toolbar control do the painting.
    // The borders and gripper are painted in OnNcPaint.
    CWnd::OnPaint();
}

void CToolBar::OnSysColorChange()
/*******************************/
{
    if( m_hbmImageWell != NULL ) {
        ASSERT( m_hInstImageWell != NULL );
        ASSERT( m_hRsrcImageWell != NULL );

        SetBitmap( AfxLoadSysColorBitmap( m_hInstImageWell, m_hRsrcImageWell ) );
    }
}

HBITMAP AFXAPI AfxLoadSysColorBitmap( HINSTANCE hInst, HRSRC hRsrc, BOOL bMono )
/******************************************************************************/
{
    HGLOBAL hMem = ::LoadResource( hInst, hRsrc );
    BITMAPINFOHEADER *pBitmap = (BITMAPINFOHEADER *)::LockResource( hMem );
    int nColors = 1 << pBitmap->biBitCount;
    if( pBitmap->biBitCount == 16 || pBitmap->biBitCount == 24 ||
        pBitmap->biBitCount == 32 ) {
        nColors = pBitmap->biClrUsed;
    }
    int nBitmapSize = pBitmap->biSize + nColors * sizeof( RGBQUAD );
    BITMAPINFOHEADER *pNewBitmap = (BITMAPINFOHEADER *)malloc( nBitmapSize );
    memcpy( pNewBitmap, pBitmap, nBitmapSize );
    RGBQUAD *pColors = (RGBQUAD *)((BYTE *)pNewBitmap + pNewBitmap->biSize);
    for( int i = 0; i < nColors; i++ ) {
        if( bMono ) {
            if( pColors[i].rgbBlue != 0x00 || pColors[i].rgbGreen != 0x00 ||
                pColors[i].rgbRed != 0x00 ) {
                pColors[i].rgbBlue = 0xFF;
                pColors[i].rgbGreen = 0xFF;
                pColors[i].rgbRed = 0xFF;
            }
        } else {
            for( int j = 0; j < NUM_SYSCOLORS; j++ ) {
                if( pColors[i].rgbBlue == _Colors[j].rgbq.rgbBlue &&
                    pColors[i].rgbGreen == _Colors[j].rgbq.rgbGreen &&
                    pColors[i].rgbRed == _Colors[j].rgbq.rgbRed ) {
                    COLORREF cr = ::GetSysColor( _Colors[j].dwColorIndex );
                    pColors[i].rgbBlue = GetBValue( cr );
                    pColors[i].rgbGreen = GetGValue( cr );
                    pColors[i].rgbRed = GetRValue( cr );
                    break;
                }
            }
        }
    }
    HDC     hDC = ::GetDC( NULL );
    HBITMAP hBitmap = ::CreateCompatibleBitmap( hDC, pBitmap->biWidth,
                                                pBitmap->biHeight );
    HDC     hMemDC = ::CreateCompatibleDC( hDC );
    HBITMAP hOldBitmap = (HBITMAP)::SelectObject( hMemDC, hBitmap );
    ::StretchDIBits( hMemDC, 0, 0, pBitmap->biWidth, pBitmap->biHeight, 0, 0,
                     pBitmap->biWidth, pBitmap->biHeight, (LPBYTE)pBitmap + nBitmapSize,
                     (BITMAPINFO *)pNewBitmap, DIB_RGB_COLORS, SRCCOPY );
    ::SelectObject( hMemDC, hOldBitmap );
    ::DeleteDC( hMemDC );
    ::ReleaseDC( NULL, hDC );
    ::FreeResource( hMem );
    free( pNewBitmap );
    return( hBitmap );
}
