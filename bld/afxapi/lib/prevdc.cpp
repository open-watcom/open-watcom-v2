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
* Description:  Implementation of CPreviewDC.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CPreviewDC, CDC )

CPreviewDC::CPreviewDC()
/**********************/
{
    m_nScaleNum = 1;
    m_nScaleDen = 1;
    m_nSaveDCIndex = 0;
    m_hFont = NULL;
    m_hPrinterFont = NULL;
}

CPreviewDC::~CPreviewDC()
/***********************/
{
    if( m_hFont != NULL ) {
        ::DeleteObject( m_hFont );
    }
}

CPoint CPreviewDC::OffsetViewportOrg( int nWidth, int nHeight )
/*************************************************************/
{
    CPoint point;
    ::OffsetViewportOrgEx( m_hAttribDC, nWidth, nHeight, &point );
    MirrorViewportOrg();
    return( point );
}

void CPreviewDC::ReleaseOutputDC()
/********************************/
{
    if( m_nSaveDCIndex != 0 ) {
        ::RestoreDC( m_hDC, m_nSaveDCIndex );
        m_nSaveDCIndex = 0;
    }
    CDC::ReleaseOutputDC();
}

CSize CPreviewDC::ScaleViewportExt( int xNum, int xDenom, int yNum, int yDenom )
/******************************************************************************/
{
    CSize size;
    ::ScaleViewportExtEx( m_hAttribDC, xNum, xDenom, yNum, yDenom, &size );
    MirrorMappingMode( TRUE );
    return( size );
}

CSize CPreviewDC::ScaleWindowExt( int xNum, int xDenom, int yNum, int yDenom )
/****************************************************************************/
{
    CSize size;
    ::ScaleWindowExtEx( m_hAttribDC, xNum, xDenom, yNum, yDenom, &size );
    MirrorMappingMode( TRUE );
    return( size );
}

CFont *CPreviewDC::SelectObject( CFont *pFont )
/*********************************************/
{
    HFONT hOldFont = (HFONT)::SelectObject( m_hAttribDC, pFont->GetSafeHandle() );
    MirrorFont();
    return( CFont::FromHandle( hOldFont ) );
}

CGdiObject *CPreviewDC::SelectStockObject( int nIndex )
/*****************************************************/
{
    if( nIndex == OEM_FIXED_FONT || nIndex == ANSI_FIXED_FONT ||
        nIndex == ANSI_VAR_FONT || nIndex == SYSTEM_FONT ||
        nIndex == DEVICE_DEFAULT_FONT || nIndex == SYSTEM_FIXED_FONT ||
        nIndex == DEFAULT_GUI_FONT ) {
        m_hPrinterFont = (HFONT)::GetStockObject( nIndex );
        HGDIOBJ hOldFont = ::SelectObject( m_hAttribDC, m_hPrinterFont );
        MirrorFont();
        return( CGdiObject::FromHandle( hOldFont ) );
    }
    return( CDC::SelectStockObject( nIndex ) );
}

void CPreviewDC::SetAttribDC( HDC hDC )
/*************************************/
{
    CDC::SetAttribDC( hDC );
    if( m_hDC != NULL ) {
        MirrorAttributes();
        MirrorFont();
        MirrorMappingMode( TRUE );
    }
}

COLORREF CPreviewDC::SetBkColor( COLORREF crColor )
/*************************************************/
{
    COLORREF crOldColor = ::SetBkColor( m_hAttribDC, crColor );
    MirrorFont();
    return( crOldColor );
}

int CPreviewDC::SetMapMode( int nMapMode )
/****************************************/
{
    int nOldMapMode = ::SetMapMode( m_hAttribDC, nMapMode );
    MirrorMappingMode( TRUE );
    return( nOldMapMode );
}

void CPreviewDC::SetOutputDC( HDC hDC )
/*************************************/
{
    CDC::SetOutputDC( hDC );
    m_nSaveDCIndex = ::SaveDC( m_hDC );
    if( m_hAttribDC != NULL ) {
        MirrorAttributes();
        MirrorFont();
        MirrorMappingMode( FALSE );
    }
}

COLORREF CPreviewDC::SetTextColor( COLORREF crColor )
/***************************************************/
{
    COLORREF crOldColor = ::SetTextColor( m_hAttribDC, crColor );
    MirrorAttributes();
    return( crOldColor );
}

CSize CPreviewDC::SetViewportExt( int cx, int cy )
/************************************************/
{
    CSize size;
    ::SetViewportExtEx( m_hAttribDC, cx, cy, &size );
    MirrorMappingMode( TRUE );
    return( size );
}

CPoint CPreviewDC::SetViewportOrg( int x, int y )
/***********************************************/
{
    CPoint point;
    ::SetViewportOrgEx( m_hAttribDC, x, y, &point );
    MirrorViewportOrg();
    return( point );
}

CSize CPreviewDC::SetWindowExt( int cx, int cy )
/**********************************************/
{
    CSize size;
    ::SetWindowExtEx( m_hAttribDC, cx, cy, &size );
    MirrorMappingMode( TRUE );
    return( size );
}

void CPreviewDC::ClipToPage()
/***************************/
{
    POINT point;
    point.x = ::GetDeviceCaps( m_hAttribDC, HORZRES );
    point.y = ::GetDeviceCaps( m_hAttribDC, VERTRES );
    PrinterDPtoScreenDP( &point );
    ::SetMapMode( m_hDC, MM_TEXT );
    ::SetViewportOrgEx( m_hDC, m_sizeTopLeft.cx, m_sizeTopLeft.cy, NULL );
    ::SetWindowOrgEx( m_hDC, 0, 0, NULL );
    ::IntersectClipRect( m_hDC, 1, 1, point.x - 2, point.y - 2 );
    MirrorMappingMode( FALSE );
}

void CPreviewDC::MirrorAttributes()
/*********************************/
{
    if( m_hAttribDC != NULL && m_hDC != NULL ) {
        ::SelectObject( m_hDC, ::GetCurrentObject( m_hAttribDC, OBJ_PEN ) );
        ::SelectObject( m_hDC, ::GetCurrentObject( m_hAttribDC, OBJ_BRUSH ) );
        
        ::SetBkMode( m_hDC, ::GetBkMode( m_hAttribDC ) ) ;
        ::SetPolyFillMode( m_hDC, ::GetPolyFillMode( m_hAttribDC ) );
        ::SetROP2( m_hDC, ::GetROP2( m_hAttribDC ) );
        ::SetStretchBltMode( m_hDC, ::GetStretchBltMode( m_hAttribDC ) );
        ::SetTextAlign( m_hDC, ::GetTextAlign( m_hAttribDC ) );

        ::SetBkColor( m_hDC, ::GetNearestColor( m_hAttribDC,
                                                ::GetBkColor( m_hAttribDC ) ) );
        ::SetTextColor( m_hDC, ::GetNearestColor( m_hAttribDC,
                                                  ::GetTextColor( m_hAttribDC ) ) );
    }
}

void CPreviewDC::MirrorFont()
/***************************/
{
    if( m_hPrinterFont == NULL ) {
        SelectStockObject( DEVICE_DEFAULT_FONT );

        // SelectStockObject() calls MirrorFont(), so there's no need to do anything
        // else here.
    } else {
        LOGFONT     lf;
        TEXTMETRIC  tm;
        ::GetObject( m_hPrinterFont, sizeof( LOGFONT ), &lf );
        ::GetTextFace( m_hAttribDC, LF_FACESIZE, lf.lfFaceName );
        ::GetTextMetrics( m_hAttribDC, &tm );
        lf.lfHeight = tm.tmHeight;
        lf.lfWidth = tm.tmAveCharWidth;
        lf.lfWeight = tm.tmWeight;
        lf.lfItalic = tm.tmItalic;
        lf.lfUnderline = tm.tmUnderlined;
        lf.lfStrikeOut = tm.tmStruckOut;
        lf.lfCharSet = tm.tmCharSet;
        lf.lfPitchAndFamily = tm.tmPitchAndFamily;

        HFONT hFont = ::CreateFontIndirect( &lf );
        ::SelectObject( m_hDC, hFont );
        if( m_hFont != NULL ) {
            ::DeleteObject( m_hFont );
        }
        m_hFont = hFont;
    }
}

void CPreviewDC::MirrorMappingMode( BOOL bCompute )
/*************************************************/
{
    if( bCompute ) {
        ::GetWindowExtEx( m_hAttribDC, &m_sizeWinExt );
        ::GetViewportExtEx( m_hAttribDC, &m_sizeVpExt );

        // Increase the extents so that they won't get rounded down to zero when doing
        // the conversions.
        while( m_sizeWinExt.cx > -20000 && m_sizeWinExt.cx < 20000 &&
               m_sizeVpExt.cx > -20000 && m_sizeWinExt.cx < 20000 ) {
            m_sizeWinExt.cx <<= 1;
            m_sizeVpExt.cx <<= 1;
        }
        while( m_sizeWinExt.cy > -20000 && m_sizeWinExt.cy < 20000 &&
               m_sizeVpExt.cy > -20000 && m_sizeWinExt.cy < 20000 ) {
            m_sizeWinExt.cy <<= 1;
            m_sizeVpExt.cy <<= 1;
        }

        HDC hScreenDC = ::GetDC( NULL );
        int nScreenLogX = ::GetDeviceCaps( hScreenDC, LOGPIXELSX );
        int nScreenLogY = ::GetDeviceCaps( hScreenDC, LOGPIXELSY );
        int nPrinterLogX = ::GetDeviceCaps( m_hAttribDC, LOGPIXELSX );
        int nPrinterLogY = ::GetDeviceCaps( m_hAttribDC, LOGPIXELSY );
        ::ReleaseDC( NULL, hScreenDC );

        m_sizeVpExt.cx = ::MulDiv( m_sizeVpExt.cx, nScreenLogX, nPrinterLogX );
        m_sizeVpExt.cy = ::MulDiv( m_sizeVpExt.cy, nScreenLogY, nPrinterLogY );
        m_sizeVpExt.cx = ::MulDiv( m_sizeVpExt.cx, m_nScaleNum, m_nScaleDen );
        m_sizeVpExt.cy = ::MulDiv( m_sizeVpExt.cy, m_nScaleNum, m_nScaleDen );
    }
    if( m_hDC != NULL ) {
        ::SetMapMode( m_hDC, MM_ANISOTROPIC );
        ::SetWindowExtEx( m_hDC, m_sizeWinExt.cx, m_sizeWinExt.cy, NULL );
        ::SetViewportExtEx( m_hDC, m_sizeVpExt.cx, m_sizeVpExt.cy, NULL );
        MirrorViewportOrg();
    }
}

void CPreviewDC::MirrorViewportOrg()
/**********************************/
{
    if( m_hAttribDC != NULL && m_hDC != NULL ) {
        CPoint point;
        ::GetViewportOrgEx( m_hAttribDC, &point );
        PrinterDPtoScreenDP( &point );
        point += m_sizeTopLeft;
        ::SetViewportOrgEx( m_hDC, point.x, point.y, NULL );
        ::GetWindowOrgEx( m_hAttribDC, &point );
        ::SetWindowOrgEx( m_hAttribDC, point.x, point.y, NULL );
    }
}

void CPreviewDC::PrinterDPtoScreenDP( LPPOINT lpPoint ) const
/***********************************************************/
{
    CSize   sizePrinterWinExt;
    CSize   sizePrinterVpExt;
    ::GetWindowExtEx( m_hAttribDC, &sizePrinterWinExt );
    ::GetViewportExtEx( m_hAttribDC, &sizePrinterVpExt );

    ASSERT( lpPoint != NULL );
    lpPoint->x = ::MulDiv( lpPoint->x, sizePrinterWinExt.cx, sizePrinterVpExt.cx );
    lpPoint->y = ::MulDiv( lpPoint->y, sizePrinterWinExt.cy, sizePrinterVpExt.cy );
    lpPoint->x = ::MulDiv( lpPoint->x, m_sizeVpExt.cx, m_sizeWinExt.cx );
    lpPoint->y = ::MulDiv( lpPoint->y, m_sizeVpExt.cy, m_sizeWinExt.cy );
}

void CPreviewDC::SetScaleRatio( int nNumerator, int nDenominator )
/****************************************************************/
{
    m_nScaleNum = nNumerator;
    m_nScaleDen = nDenominator;
    MirrorMappingMode( TRUE );
    MirrorFont();
}

void CPreviewDC::SetTopLeftOffset( CSize sizeTopLeft )
/****************************************************/
{
    m_sizeTopLeft = sizeTopLeft;
    MirrorViewportOrg();
}
