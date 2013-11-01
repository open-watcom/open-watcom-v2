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
* Description:  Implementation of CDockBar.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_DYNAMIC( CDockBar, CControlBar )

CDockBar::CDockBar( BOOL bFloating )
/**********************************/
{
    m_bFloating = bFloating;
}

BOOL CDockBar::Create( CWnd *pParentWnd, DWORD dwStyle, UINT nID )
/****************************************************************/
{
    m_dwStyle = dwStyle & CBRS_ALL;
    dwStyle &= ~CBRS_ALL;
    return( CWnd::Create( _T("AfxControlBar"), NULL, dwStyle, CRect( 0, 0, 0, 0 ),
                          pParentWnd, nID ) );
}

int CDockBar::GetDockedVisibleCount() const
/*****************************************/
{
    int nResult = 0;
    int nCount = m_arrBars.GetCount();
    for( int i = 0; i < nCount; i++ ) {
        CControlBar *pBar = (CControlBar *)m_arrBars.GetAt( i );
        if( pBar != NULL ) {
            ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
            if( pBar->GetStyle() & WS_VISIBLE ) {
                nResult++;
            }
        }
    }
    return( nResult );
}

CSize CDockBar::CalcFixedLayout( BOOL bStretch, BOOL bHorz )
/**********************************************************/
{
    CSize size( 0, 0 );

    CWnd *pParent = GetParent();
    ASSERT( pParent != NULL );

    CRect rectClient;
    pParent->GetClientRect( &rectClient );

    int     nCount = m_arrBars.GetCount();
    HDWP    hDWP = ::BeginDeferWindowPos( nCount );
    int     nRowWidth = 0;
    int     nRowHeight = 0;
    for( int i = 0; i < nCount; i++ ) {
        CControlBar *pBar = (CControlBar *)m_arrBars.GetAt( i );
        if( pBar != NULL && (pBar->GetStyle() & WS_VISIBLE) ) {
            ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
            CSize sizeBar = pBar->CalcFixedLayout( FALSE, bHorz );
            if( bHorz ) {
                if( nRowWidth + sizeBar.cx > rectClient.Width() ) {
                    if( nRowWidth > size.cx ) {
                        size.cx = nRowWidth;
                    }
                    size.cy += nRowHeight;
                    nRowWidth = 0;
                    nRowHeight = 0;
                }
                ::DeferWindowPos( hDWP, pBar->GetSafeHwnd(), NULL, nRowWidth, size.cy,
                                  sizeBar.cx, sizeBar.cy,
                                  SWP_NOACTIVATE | SWP_NOZORDER );
                nRowWidth += sizeBar.cx;
                if( sizeBar.cy > nRowHeight ) {
                    nRowHeight = sizeBar.cy;
                }
            } else {
                if( nRowHeight + sizeBar.cy > rectClient.Height() ) {
                    if( nRowHeight > size.cy ) {
                        size.cy = nRowHeight;
                    }
                    size.cx += nRowWidth;
                    nRowWidth = 0;
                    nRowHeight = 0;
                }
                ::DeferWindowPos( hDWP, pBar->GetSafeHwnd(), NULL, size.cx, nRowHeight,
                                  sizeBar.cx, sizeBar.cy,
                                  SWP_NOACTIVATE | SWP_NOZORDER );
                nRowHeight += sizeBar.cy;
                if( sizeBar.cx > nRowWidth ) {
                    nRowWidth = sizeBar.cx;
                }
            }
        }
    }
    ::EndDeferWindowPos( hDWP );
    if( bHorz ) {
        if( bStretch ) {
            size.cx = 32767;
        } else if( nRowWidth > size.cx ) {
            size.cx = nRowWidth;
        }
        size.cy += nRowHeight;
    } else {
        if( bStretch ) {
            size.cy = 32767;
        } else if( nRowHeight > size.cy ) {
            size.cy = nRowHeight;
        }
        size.cx += nRowWidth;
    }
    return( size );
}

BOOL CDockBar::IsDockBar() const
/******************************/
{
    return( TRUE );
}

void CDockBar::OnUpdateCmdUI( CFrameWnd *pTarget, BOOL bDisableIfNoHndler )
/*************************************************************************/
{
    UNUSED_ALWAYS( pTarget );
    UNUSED_ALWAYS( bDisableIfNoHndler );
}

void CDockBar::DockControlBar( CControlBar *pBar, LPCRECT lpRect )
/****************************************************************/
{
    UNUSED_ALWAYS( lpRect );
    
    if( pBar->m_pDockBar != this ) {
        DWORD dwStyle = pBar->GetBarStyle();
        dwStyle &= ~(CBRS_ALIGN_ANY | CBRS_BORDER_ANY | CBRS_FLOATING);
        dwStyle |= m_dwStyle & (CBRS_ALIGN_ANY | CBRS_BORDER_ANY);
        if( m_bFloating ) {
            dwStyle |= CBRS_FLOATING;
        }
        pBar->SetBarStyle( dwStyle );
        
        int nCount = m_arrBars.GetCount();
        for( int i = 0; i < nCount; i++ ) {
            if( m_arrBars.GetAt( i ) == pBar ) {
                return;
            }
        }
        m_arrBars.Add( pBar );
        pBar->SetParent( this );
        
        if( pBar->m_pDockBar != NULL ) {
            pBar->m_pDockBar->RemoveControlBar( pBar );
        }
        pBar->m_pDockBar = this;

        CFrameWnd *pFrame = GetParentFrame();
        ASSERT( pFrame != NULL );
        pFrame->RecalcLayout();

        CFrameWnd *pOwner = (CFrameWnd *)GetOwner();
        if( pOwner != NULL && pOwner != pFrame ) {
            ASSERT( pOwner->IsKindOf( RUNTIME_CLASS( CFrameWnd ) ) );
            pOwner->RecalcLayout();
        }
    }
}

int CDockBar::GetDockedCount() const
/**********************************/
{
    int nResult = 0;
    int nCount = m_arrBars.GetCount();
    for( int i = 0; i < nCount; i++ ) {
        CControlBar *pBar = (CControlBar *)m_arrBars.GetAt( i );
        if( pBar != NULL ) {
            i++;
        }
    }
    return( nResult );
}

BOOL CDockBar::RemoveControlBar( CControlBar *pBar, int nPosExclude,
                                 int nAddPlaceHolder )
/****************************************************/
{
    nPosExclude = nPosExclude;
    nAddPlaceHolder = nAddPlaceHolder;

    int nCount = m_arrBars.GetCount();
    for( int i = 0; i < nCount; i++ ) {
        if( m_arrBars.GetAt( i ) == pBar ) {
            m_arrBars.RemoveAt( i );
            break;
        }
    }
    CFrameWnd *pFrame = GetParentFrame();
    ASSERT( pFrame != NULL );
    if( m_bFloating && GetDockedVisibleCount() == 0 ) {
        if( GetDockedCount() == 0 ) {
            pFrame->DestroyWindow();
            return( TRUE );
        } else {
            pFrame->ShowWindow( SW_HIDE );
        }
    } else {
        pFrame->RecalcLayout();
    }
    return( FALSE );
}

void CDockBar::ShowAll( BOOL bShow )
/**********************************/
{
    int nCount = m_arrBars.GetCount();
    for( int i = 0; i < nCount; i++ ) {
        CControlBar *pBar = (CControlBar *)m_arrBars.GetAt( i );
        if( pBar != NULL ) {
            ASSERT( pBar->IsKindOf( RUNTIME_CLASS( CControlBar ) ) );
            if( bShow ) {
                pBar->ShowWindow( SW_SHOW );
            } else {
                pBar->ShowWindow( SW_HIDE );
            }
        }
    }
    if( m_bFloating ) {
        CFrameWnd *pFrame = GetParentFrame();
        ASSERT( pFrame != NULL );
        if( bShow && GetDockedVisibleCount() > 0 ) {
            pFrame->ShowWindow( SW_SHOW );
            pFrame->UpdateWindow();
        } else {
            pFrame->ShowWindow( SW_HIDE );
        }
    }
}
