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
* Description:  Implementation of CDC.
*
****************************************************************************/


#include "stdafx.h"
#include "handmap.h"

IMPLEMENT_DYNCREATE( CDC, CObject )

CDC::CDC()
/********/
{
    m_hDC = NULL;
    m_hAttribDC = NULL;
    m_bPrinting = FALSE;
}

CDC::~CDC()
/*********/
{
    DeleteDC();
}

int CDC::DrawText( LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat )
/******************************************************************************/
{
    return( ::DrawText( m_hDC, lpszString, nCount, lpRect, nFormat ) );
}

int CDC::DrawTextEx( LPTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat,
                     LPDRAWTEXTPARAMS lpDTParams )
/************************************************/
{
    return( ::DrawTextEx( m_hDC, lpszString, nCount, lpRect, nFormat, lpDTParams ) );
}

int CDC::Escape( int nEscape, int nCount, LPCSTR lpszInData, LPVOID lpOutData )
/*****************************************************************************/
{
    return( ::Escape( m_hDC, nEscape, nCount, lpszInData, lpOutData ) );
}

BOOL CDC::ExtTextOut( int x, int y, UINT nOptions, LPCRECT lpRect, LPCTSTR lpszString,
                      UINT nCount, LPINT lpDxWidths )
/***************************************************/
{
    return( ::ExtTextOut( m_hDC, x, y, nOptions, lpRect, lpszString, nCount,
                          lpDxWidths ) );
}

int CDC::GetClipBox( LPRECT lpRect ) const
/****************************************/
{
    return( ::GetClipBox( m_hDC, lpRect ) );
}

BOOL CDC::GrayString( CBrush *pBrush, GRAYSTRINGPROC lpfnOutput, LPARAM lpData,
                      int nCount, int x, int y, int nWidth, int nHeight )
/***********************************************************************/
{
    return( ::GrayString( m_hDC, (HBRUSH)pBrush->GetSafeHandle(), lpfnOutput, lpData,
                          nCount, x, y, nWidth, nHeight ) );
}

CPoint CDC::OffsetViewportOrg( int nWidth, int nHeight )
/******************************************************/
{
    CPoint point;
    ::OffsetViewportOrgEx( m_hDC, nWidth, nHeight, &point );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::OffsetViewportOrgEx( m_hAttribDC, nWidth, nHeight, &point );
    }
    return( point );
}

BOOL CDC::PtVisible( int x, int y ) const
/***************************************/
{
    return( ::PtVisible( m_hAttribDC, x, y ) );
}

BOOL CDC::RectVisible( LPCRECT lpRect ) const
/*******************************************/
{
    return( ::RectVisible( m_hAttribDC, lpRect ) );
}

void CDC::ReleaseAttribDC()
/*************************/
{
    m_hAttribDC = NULL;
}

void CDC::ReleaseOutputDC()
/*************************/
{
    m_hDC = NULL;
}

BOOL CDC::RestoreDC( int nSavedDC )
/*********************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL &&
        !::RestoreDC( m_hAttribDC, nSavedDC ) ) {
        return( FALSE );
    }
    return( ::RestoreDC( m_hDC, nSavedDC ) );
}

int CDC::SaveDC()
/***************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( ::SaveDC( m_hAttribDC ) == 0 ) {
            return( 0 );
        }
        if( ::SaveDC( m_hDC ) == 0 ) {
            return( 0 );
        }
        return( -1 );
    }
    return( ::SaveDC( m_hDC ) );
}

CSize CDC::ScaleViewportExt( int xNum, int xDenom, int yNum, int yDenom )
/***********************************************************************/
{
    CSize size;
    ::ScaleViewportExtEx( m_hDC, xNum, xDenom, yNum, yDenom, &size );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::ScaleViewportExtEx( m_hAttribDC, xNum, xDenom, yNum, yDenom, &size );
    }
    return( size );
}

CSize CDC::ScaleWindowExt( int xNum, int xDenom, int yNum, int yDenom )
/*********************************************************************/
{
    CSize size;
    ::ScaleWindowExtEx( m_hDC, xNum, xDenom, yNum, yDenom, &size );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::ScaleWindowExtEx( m_hAttribDC, xNum, xDenom, yNum, yDenom, &size );
    }
    return( size );
}

CFont *CDC::SelectObject( CFont *pFont )
/**************************************/
{
    HFONT hFont = (HFONT)::SelectObject( m_hDC, pFont->GetSafeHandle() );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        hFont = (HFONT)::SelectObject( m_hAttribDC, pFont->GetSafeHandle() );
    }
    return( CFont::FromHandle( hFont ) );
}

CGdiObject *CDC::SelectStockObject( int nIndex )
/**********************************************/
{
    HGDIOBJ hStockObject = ::GetStockObject( nIndex );
    HGDIOBJ hOldObject = ::SelectObject( m_hDC, hStockObject );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        hOldObject = ::SelectObject( m_hAttribDC, hStockObject );
    }
    return( CGdiObject::FromHandle( hOldObject ) );
}

void CDC::SetAttribDC( HDC hDC )
/******************************/
{
    m_hAttribDC = hDC;
}

COLORREF CDC::SetBkColor( COLORREF crColor )
/******************************************/
{
    COLORREF crOldColor = ::SetBkColor( m_hDC, crColor );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        crOldColor = ::SetBkColor( m_hAttribDC, crColor );
    }
    return( crOldColor );
}

int CDC::SetMapMode( int nMapMode )
/*********************************/
{
    int nOldMapMode = ::SetMapMode( m_hDC, nMapMode );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        nOldMapMode = ::SetMapMode( m_hAttribDC, nMapMode );
    }
    return( nOldMapMode );
}

void CDC::SetOutputDC( HDC hDC )
/******************************/
{
    m_hDC = hDC;
}

COLORREF CDC::SetTextColor( COLORREF crColor )
/********************************************/
{
    COLORREF crOldColor = ::SetTextColor( m_hDC, crColor );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        crOldColor = ::SetTextColor( m_hAttribDC, crColor );
    }
    return( crOldColor );
}

CSize CDC::SetViewportExt( int cx, int cy )
/*****************************************/
{
    CSize size;
    ::SetViewportExtEx( m_hDC, cx, cy, &size );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetViewportExtEx( m_hAttribDC, cx, cy, &size );
    }
    return( size );
}

CPoint CDC::SetViewportOrg( int x, int y )
/****************************************/
{
    CPoint point;
    ::SetViewportOrgEx( m_hDC, x, y, &point );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetViewportOrgEx( m_hAttribDC, x, y, &point );
    }
    return( point );
}

CSize CDC::SetWindowExt( int cx, int cy )
/***************************************/
{
    CSize size;
    ::SetWindowExtEx( m_hDC, cx, cy, &size );
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetWindowExtEx( m_hAttribDC, cx, cy, &size );
    }
    return( size );
}

CSize CDC::TabbedTextOut( int x, int y, LPCTSTR lpszString, int nCount,
                          int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin )
/**************************************************************************************/
{
    return( CSize( ::TabbedTextOut( m_hDC, x, y, lpszString, nCount, nTabPositions,
                                    lpnTabStopPositions, nTabOrigin ) ) );
}

BOOL CDC::TextOut( int x, int y, LPCTSTR lpszString, int nCount )
/***************************************************************/
{
    return( ::TextOut( m_hDC, x, y, lpszString, nCount ) );
}

#ifdef _DEBUG

void CDC::Dump( CDumpContext &dc ) const
/**************************************/
{
    CObject::Dump( dc );

    dc << "m_hDC = " << m_hDC << "\n";
    dc << "m_hAttribDC = " << m_hAttribDC << "\n";
    dc << "m_bPrinting = " << m_bPrinting << "\n";
}

#endif // _DEBUG

BOOL CDC::ArcTo( int x1, int y1, int x2, int y2, int x3, int y3,
                 int x4, int y4 )
/*******************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( !::ArcTo( m_hDC, x1, y1, x2, y2, x3, y3, x4, y4 ) ) {
            return( FALSE );
        }
        POINT point;
        if( !::GetCurrentPositionEx( m_hDC, &point ) ) {
            return( FALSE );
        }
        return( ::MoveToEx( m_hAttribDC, point.x, point.y, NULL ) );
    }
    return( ::ArcTo( m_hDC, x1, y1, x2, y2, x3, y3, x4, y4 ) );
}

BOOL CDC::Attach( HDC hDC )
/*************************/
{
    ASSERT( m_hDC == NULL );
    ASSERT( m_hAttribDC == NULL );
    if( hDC == NULL ) {
        return( FALSE );
    }
    m_hDC = hDC;
    m_hAttribDC = hDC;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHDC;
    ASSERT( pHandleMap != NULL );
    pHandleMap->SetPermanent( hDC, this );
    return( TRUE );
}

BOOL CDC::DeleteDC()
/******************/
{
    HDC hDC = m_hDC;
    if( m_hDC == NULL || !::DeleteDC( m_hDC ) ) {
        return( FALSE );
    }
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHDC;
    ASSERT( pHandleMap != NULL );
    pHandleMap->RemoveHandle( m_hDC );
    m_hDC = NULL;
    if( m_hAttribDC != hDC && m_hAttribDC != NULL && !::DeleteDC( m_hAttribDC ) ) {
        return( FALSE );
    }
    m_hAttribDC = NULL;
    return( TRUE );
}

HDC CDC::Detach()
/***************/
{
    if( m_hDC == NULL ) {
        return( NULL );
    }
    HDC hDC = m_hDC;
    m_hDC = NULL;
    m_hAttribDC = NULL;
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHDC;
    ASSERT( pHandleMap != NULL );
    pHandleMap->RemoveHandle( hDC );
    return( hDC );
}

void CDC::Draw3dRect( int x, int y, int cx, int cy, COLORREF clrTopLeft,
                      COLORREF clrBottomRight )
/*********************************************/
{
    FillSolidRect( x, y, cx - 1, 1, clrTopLeft );
    FillSolidRect( x, y, 1, cy - 1, clrTopLeft );
    FillSolidRect( x + cx - 1, y, 1, cy - 1, clrBottomRight );
    FillSolidRect( x, y + cy - 1, cx - 1, 1, clrBottomRight );
}

int CDC::ExcludeClipRect( int x1, int y1, int x2, int y2 )
/********************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::ExcludeClipRect( m_hDC, x1, y1, x2, y2 );
        return( ::ExcludeClipRect( m_hAttribDC, x1, y1, x2, y2 ) );
    }
    return( ::ExcludeClipRect( m_hDC, x1, y1, x2, y2 ) );
}

void CDC::FillSolidRect( LPCRECT lpRect, COLORREF clr )
/*****************************************************/
{
    ASSERT( lpRect != NULL );
    ::SetBkColor( m_hDC, clr );
    ::ExtTextOut( m_hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL );
}

void CDC::FillSolidRect( int x, int y, int cx, int cy, COLORREF clr )
/*******************************************************************/
{
    CRect rect( x, y, x + cx, y + cy );
    ::SetBkColor( m_hDC, clr );
    ::ExtTextOut( m_hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL );
}

int CDC::IntersectClipRect( int x1, int y1, int x2, int y2 )
/**********************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::IntersectClipRect( m_hDC, x1, y1, x2, y2 );
        return( ::IntersectClipRect( m_hAttribDC, x1, y1, x2, y2 ) );
    }
    return( ::IntersectClipRect( m_hDC, x1, y1, x2, y2 ) );
}

BOOL CDC::LineTo( int x, int y )
/******************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( !::LineTo( m_hDC, x, y ) ) {
            return( FALSE );
        }
        return( ::MoveToEx( m_hAttribDC, x, y, NULL ) );
    }
    return( ::LineTo( m_hDC, x, y ) );
}

BOOL CDC::ModifyWorldTransform( const XFORM &rXform, DWORD iMode )
/****************************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( !::ModifyWorldTransform( m_hDC, &rXform, iMode ) ) {
            return( FALSE );
        }
        return( ::ModifyWorldTransform( m_hAttribDC, &rXform, iMode ) );
    }
    return( ::ModifyWorldTransform( m_hDC, &rXform, iMode ) );
}

CPoint CDC::MoveTo( int x, int y )
/********************************/
{
    CPoint point;
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::MoveToEx( m_hDC, x, y, NULL );
        ::MoveToEx( m_hAttribDC, x, y, &point );
    } else {
        ::MoveToEx( m_hDC, x, y, &point );
    }
    return( point );
}

int CDC::OffsetClipRgn( int x, int y )
/************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::OffsetClipRgn( m_hDC, x, y );
        return( ::OffsetClipRgn( m_hAttribDC, x, y ) );
    }
    return( ::OffsetClipRgn( m_hDC, x, y ) );
}

CPoint CDC::OffsetWindowOrg( int nWidth, int nHeight )
/****************************************************/
{
    CPoint point;
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::OffsetWindowOrgEx( m_hDC, nWidth, nHeight, NULL );
        ::OffsetWindowOrgEx( m_hAttribDC, nWidth, nHeight, &point );
    } else {
        ::OffsetWindowOrgEx( m_hDC, nWidth, nHeight, &point );
    }
    return( point );
}

BOOL CDC::PolyBezierTo( const POINT *lpPoints, int nCount )
/*********************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( !::PolyBezierTo( m_hDC, lpPoints, nCount ) ) {
            return( FALSE );
        }
        return( ::MoveToEx( m_hAttribDC, lpPoints[nCount - 1].x,
                            lpPoints[nCount - 1].y, NULL ) );
    }
    return( ::PolyBezierTo( m_hDC, lpPoints, nCount ) );
}

BOOL CDC::PolyDraw( const POINT *lpPoints, const BYTE *lpTypes, int nCount )
/**************************************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( !::PolyDraw( m_hDC, lpPoints, lpTypes, nCount ) ) {
            return( FALSE );
        }
        POINT point;
        if( !::GetCurrentPositionEx( m_hDC, &point ) ) {
            return( FALSE );
        }
        return( ::MoveToEx( m_hAttribDC, point.x, point.y, NULL ) );
    }
    return( ::PolyDraw( m_hDC, lpPoints, lpTypes, nCount ) );
}

BOOL CDC::PolylineTo( const POINT *lpPoints, int nCount )
/*******************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( !::PolylineTo( m_hDC, lpPoints, nCount ) ) {
            return( FALSE );
        }
        return( ::MoveToEx( m_hAttribDC, lpPoints[nCount - 1].x,
                            lpPoints[nCount - 1].y, NULL ) );
    }
    return( ::PolylineTo( m_hDC, lpPoints, nCount ) );
}

BOOL CDC::SelectClipPath( int nMode )
/***********************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SelectClipPath( m_hDC, nMode );
        HRGN hRgn = ::PathToRegion( m_hDC );
        return( ::ExtSelectClipRgn( m_hAttribDC, hRgn, nMode ) );
    }
    return( ::SelectClipPath( m_hDC, nMode ) );
}

int CDC::SelectClipRgn( CRgn *pRgn )
/**********************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SelectClipRgn( m_hDC, (HRGN)pRgn->GetSafeHandle() );
        return( ::SelectClipRgn( m_hAttribDC, (HRGN)pRgn->GetSafeHandle() ) );
    }
    return( ::SelectClipRgn( m_hDC, (HRGN)pRgn->GetSafeHandle() ) );
}

int CDC::SelectClipRgn( CRgn *pRgn, int nMode )
/*********************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::ExtSelectClipRgn( m_hDC, (HRGN)pRgn->GetSafeHandle(), nMode );
        return( ::ExtSelectClipRgn( m_hAttribDC, (HRGN)pRgn->GetSafeHandle(), nMode ) );
    }
    return( ::ExtSelectClipRgn( m_hDC, (HRGN)pRgn->GetSafeHandle(), nMode ) );
}

CPen *CDC::SelectObject( CPen *pPen )
/***********************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SelectObject( m_hDC, pPen->GetSafeHandle() );
        return( CPen::FromHandle( (HPEN)::SelectObject( m_hAttribDC,
            pPen->GetSafeHandle() ) ) );
    }
    return( CPen::FromHandle( (HPEN)::SelectObject( m_hDC, pPen->GetSafeHandle() ) ) );
}

CBrush *CDC::SelectObject( CBrush *pBrush )
/*****************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SelectObject( m_hDC, pBrush->GetSafeHandle() );
        return( CBrush::FromHandle( (HBRUSH)::SelectObject( m_hAttribDC,
            pBrush->GetSafeHandle() ) ) );
    }
    return( CBrush::FromHandle( (HBRUSH)::SelectObject( m_hDC,
        pBrush->GetSafeHandle() ) ) );
}

CBitmap *CDC::SelectObject( CBitmap *pBitmap )
/********************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SelectObject( m_hDC, pBitmap->GetSafeHandle() );
        return( CBitmap::FromHandle( (HBITMAP)::SelectObject( m_hAttribDC,
            pBitmap->GetSafeHandle() ) ) );
    }
    return( CBitmap::FromHandle( (HBITMAP)::SelectObject( m_hDC,
        pBitmap->GetSafeHandle() ) ) );
}

CGdiObject *CDC::SelectObject( CGdiObject *pObject )
/**************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SelectObject( m_hDC, pObject->GetSafeHandle() );
        return( CGdiObject::FromHandle( ::SelectObject( m_hAttribDC,
            pObject->GetSafeHandle() ) ) );
    }
    return( CGdiObject::FromHandle( ::SelectObject( m_hDC,
        pObject->GetSafeHandle() ) ) );
}

CPalette *CDC::SelectPalette( CPalette *pPalette, BOOL bForceBackground )
/***********************************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SelectPalette( m_hDC, (HPALETTE)pPalette->GetSafeHandle(), bForceBackground );
        return( CPalette::FromHandle( ::SelectPalette( m_hAttribDC,
            (HPALETTE)pPalette->GetSafeHandle(), bForceBackground ) ) );
    }
    return( CPalette::FromHandle( ::SelectPalette( m_hDC,
        (HPALETTE)pPalette->GetSafeHandle(), bForceBackground ) ) );
}

int CDC::SetArcDirection( int nArcDirection )
/*******************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetArcDirection( m_hDC, nArcDirection );
        return( ::SetArcDirection( m_hAttribDC, nArcDirection ) );
    }
    return( ::SetArcDirection( m_hDC, nArcDirection ) );
}

int CDC::SetBkMode( int nBkMode )
/*******************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetBkMode( m_hDC, nBkMode );
        return( ::SetBkMode( m_hAttribDC, nBkMode ) );
    }
    return( ::SetBkMode( m_hDC, nBkMode ) );
}

UINT CDC::SetBoundsRect( LPCRECT lpRectBounds, UINT flags )
/*********************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetBoundsRect( m_hDC, lpRectBounds, flags );
        return( ::SetBoundsRect( m_hAttribDC, lpRectBounds, flags ) );
    }
    return( ::SetBoundsRect( m_hDC, lpRectBounds, flags ) );
}

CPoint CDC::SetBrushOrg( int x, int y )
/*************************************/
{
    POINT point;
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetBrushOrgEx( m_hDC, x, y, NULL );
        ::SetBrushOrgEx( m_hAttribDC, x, y, &point );
    } else {
        ::SetBrushOrgEx( m_hDC, x, y, &point );
    }
    return( point );
}

BOOL CDC::SetColorAdjustment( const COLORADJUSTMENT *lpColorAdjust )
/******************************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        if( !::SetColorAdjustment( m_hDC, lpColorAdjust ) ) {
            return( FALSE );
        }
        return( ::SetColorAdjustment( m_hAttribDC, lpColorAdjust ) );
    }
    return( ::SetColorAdjustment( m_hDC, lpColorAdjust ) );
}

int CDC::SetGraphicsMode( int iMode )
/***********************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetGraphicsMode( m_hDC, iMode );
        return( ::SetGraphicsMode( m_hAttribDC, iMode ) );
    }
    return( ::SetGraphicsMode( m_hDC, iMode ) );
}

DWORD CDC::SetMapperFlags( DWORD dwFlag )
/***************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetMapperFlags( m_hDC, dwFlag );
        return( ::SetMapperFlags( m_hAttribDC, dwFlag ) );
    }
    return( ::SetMapperFlags( m_hDC, dwFlag ) );
}

BOOL CDC::SetMiterLimit( float fMiterLimit )
/******************************************/
{
    float fOldMiterLimit;
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetMiterLimit( m_hDC, fMiterLimit, NULL );
        ::SetMiterLimit( m_hAttribDC, fMiterLimit, &fOldMiterLimit );
    } else {
        ::SetMiterLimit( m_hDC, fMiterLimit, &fOldMiterLimit );
    }
    return( fOldMiterLimit );
}

int CDC::SetPolyFillMode( int nPolyFillMode )
/*******************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetPolyFillMode( m_hDC, nPolyFillMode );
        return( ::SetPolyFillMode( m_hAttribDC, nPolyFillMode ) );
    }
    return( ::SetPolyFillMode( m_hDC, nPolyFillMode ) );
}

int CDC::SetROP2( int nDrawMode )
/*******************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetROP2( m_hDC, nDrawMode );
        return( ::SetROP2( m_hAttribDC, nDrawMode ) );
    }
    return( ::SetROP2( m_hDC, nDrawMode ) );
}

int CDC::SetStretchBltMode( int nStretchMode )
/********************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetStretchBltMode( m_hDC, nStretchMode );
        return( ::SetStretchBltMode( m_hAttribDC, nStretchMode ) );
    }
    return( ::SetStretchBltMode( m_hDC, nStretchMode ) );
}

UINT CDC::SetTextAlign( UINT nFlags )
/***********************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetTextAlign( m_hDC, nFlags );
        return( ::SetTextAlign( m_hAttribDC, nFlags ) );
    }
    return( ::SetTextAlign( m_hDC, nFlags ) );
}

int CDC::SetTextCharacterExtra( int nCharExtra )
/**********************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetTextCharacterExtra( m_hDC, nCharExtra );
        return( ::SetTextCharacterExtra( m_hAttribDC, nCharExtra ) );
    }
    return( ::SetTextCharacterExtra( m_hDC, nCharExtra ) );
}

int CDC::SetTextJustification( int nBreakExtra, int nBreakCount )
/***************************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetTextJustification( m_hDC, nBreakExtra, nBreakCount );
        return( ::SetTextJustification( m_hAttribDC, nBreakExtra, nBreakCount ) );
    }
    return( ::SetTextJustification( m_hDC, nBreakExtra, nBreakCount ) );
}

CPoint CDC::SetWindowOrg( int x, int y )
/**************************************/
{
    POINT point;
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetWindowOrgEx( m_hDC, x, y, NULL );
        ::SetWindowOrgEx( m_hAttribDC, x, y, &point );
    } else {
        ::SetWindowOrgEx( m_hDC, x, y, &point );
    }
    return( point );
}

BOOL CDC::SetWorldTransform( const XFORM &rXform )
/************************************************/
{
    if( m_hAttribDC != m_hDC && m_hAttribDC != NULL ) {
        ::SetWorldTransform( m_hDC, &rXform );
        return( ::SetWorldTransform( m_hAttribDC, &rXform ) );
    }
    return( ::SetWorldTransform( m_hDC, &rXform ) );
}

int CDC::StartDoc( LPCTSTR lpszDocName )
/**************************************/
{
    DOCINFO di;
    di.cbSize = sizeof( DOCINFO );
    di.lpszDocName = lpszDocName;
    di.lpszOutput = NULL;
    di.lpszDatatype = NULL;
    di.fwType = 0L;
    return( ::StartDoc( m_hDC, &di ) );
}

void PASCAL CDC::DeleteTempMap()
/******************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHDC;
    ASSERT( pHandleMap != NULL );
    pHandleMap->DeleteTemp();
}

CDC * PASCAL CDC::FromHandle( HDC hDC )
/*************************************/
{
    AFX_MODULE_THREAD_STATE *pState = AfxGetModuleThreadState();
    ASSERT( pState != NULL );
    CHandleMap *pHandleMap = pState->m_pmapHDC;
    ASSERT( pHandleMap != NULL );
    return( (CDC *)pHandleMap->FromHandle( hDC ) );
}
