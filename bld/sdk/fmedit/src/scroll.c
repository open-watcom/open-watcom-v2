/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  Routines used for scrolling.
*
****************************************************************************/


#include <stdlib.h>
#include <limits.h>
#include <windows.h>
#include "wi163264.h"

#include "fmedit.def"
#include "state.def"
#include "object.def"
#include "global.h"

#define PG_SC_PERCENT           80
#define LN_SC_AMOUNT            10
#define UNLOCK_SCROLL_FUDGE     10

static void DoScroll( HWND wnd, int dx, int dy, BOOL lock );

extern void VerticalScroll( WPARAM wparam, LPARAM lparam, HWND wnd )
/******************************************************************/
{
    int     ydel;
    RECT    clrect;
    POINT   offset;
    RECT    scrollrect;

    wparam = wparam;
    lparam = lparam;

    if( !(GetScrollConfig() & SCROLL_VERT) ) {
        return;
    }
    GetClientRect( GetAppWnd(), &clrect );
    scrollrect = GetScrollRect();
    ydel = 0;
    GetOffset( &offset );
    switch( LOWORD( wparam ) ) {
    case SB_BOTTOM:
        ydel = scrollrect.bottom - offset.y - clrect.bottom;
        break;
    case SB_LINEDOWN:
        ydel = LN_SC_AMOUNT;
        break;
    case SB_LINEUP:
        ydel = -LN_SC_AMOUNT;
        break;
    case SB_PAGEDOWN:
        ydel = (long)clrect.bottom * PG_SC_PERCENT / 100;
        break;
    case SB_PAGEUP:
        ydel = -((long)clrect.bottom * PG_SC_PERCENT / 100);
        break;
    case SB_THUMBPOSITION:
        //ydel = GET_WM_VSCROLL_POS( wparam, lparam ) - offset.y - clrect.bottom;
        ydel = GET_WM_VSCROLL_POS( wparam, lparam ) - offset.y;
        break;
    case SB_TOP:
        ydel = -offset.y;
        break;
    default:
        break;
    }

    // make sure we do not expose area's not in the scroll rect
    if( clrect.bottom <= scrollrect.bottom && clrect.top >= scrollrect.top ) {
        if( ydel ) {
            if( clrect.bottom + offset.y + ydel > scrollrect.bottom ) {
                ydel = scrollrect.bottom - clrect.bottom - offset.y;
            }
        } else {
            if( clrect.top + offset.y + ydel < scrollrect.top ) {
                ydel = clrect.top + offset.y - scrollrect.top;
            }
        }
    }

    DoScroll( wnd, 0, ydel, TRUE );
}

extern void HorizontalScroll( WPARAM wparam, LPARAM lparam, HWND wnd )
/********************************************************************/
{
    int     xdel;
    RECT    clrect;
    POINT   offset;
    RECT    scrollrect;

    wparam = wparam;
    lparam = lparam;

    if( !(GetScrollConfig() & SCROLL_HORZ) ) {
        return;
    }
    GetClientRect( GetAppWnd(), &clrect );
    scrollrect = GetScrollRect();
    xdel = 0;
    GetOffset( &offset );
    switch( LOWORD( wparam ) ) {
    case SB_BOTTOM:
        xdel = scrollrect.right - offset.x - clrect.right;
        break;
    case SB_LINEDOWN:
        xdel = LN_SC_AMOUNT;
        break;
    case SB_LINEUP:
        xdel = -LN_SC_AMOUNT;
        break;
    case SB_PAGEDOWN:
        xdel = (long)clrect.right * PG_SC_PERCENT / 100;
        break;
    case SB_PAGEUP:
        xdel = -(long)clrect.right * PG_SC_PERCENT / 100;
        break;
    case SB_THUMBPOSITION:
        //xdel = GET_WM_VSCROLL_POS( wparam, lparam ) - offset.x - clrect.right;
        xdel = GET_WM_VSCROLL_POS( wparam, lparam ) - offset.x;
        break;
    case SB_TOP:
        xdel = -offset.x;
        break;
    default:
        break;
    }

    // make sure we do not expose area's not in the scroll rect
    if( clrect.right <= scrollrect.right && clrect.left >= scrollrect.left ) {
        if( xdel ) {
            if( clrect.right + offset.x + xdel > scrollrect.right ) {
                xdel = scrollrect.right - clrect.right - offset.x;
            }
        } else {
            if( clrect.left + offset.x + xdel < scrollrect.left ) {
                xdel = clrect.left + offset.x - scrollrect.left;
            }
        }
    }

    DoScroll( wnd, xdel, 0, TRUE );
}

extern void InitScroll( SCR_CONFIG scroll, HWND wnd )
/***************************************************/
{
    RECT  clrect;
    POINT point;

    SetScrollConfig( scroll );
    GetClientRect( wnd, &clrect );
    if( scroll & SCROLL_VERT ) {
        SetScrollRange( wnd, SB_VERT, 0, clrect.bottom, FALSE );
        SetScrollPos( wnd, SB_VERT, 0, TRUE );
    }
    if( scroll & SCROLL_HORZ ) {
      SetScrollRange( wnd, SB_HORZ, 0, clrect.right, FALSE );
      SetScrollPos( wnd, SB_HORZ, 0, TRUE );
    }
    point.x = 0;
    point.y = 0;
    SetOffset( point );
    SetScrollRect( clrect );
}

extern void ScrollResize( HWND wnd, WPARAM lparam )
/*************************************************/
{
    RECT  newrect;
    POINT offset;
    RECT  clrect;

    if( GetScrollConfig() == SCROLL_NONE ) {
        return;
    }
    clrect.left = 0;
    clrect.top = 0;
    clrect.right = LOWORD( lparam );
    clrect.bottom = HIWORD( lparam );
    newrect = clrect;
    RequestScrollRect( &newrect );
    SetScrollRect( newrect );
    GetOffset( &offset );
    if( GetScrollConfig() & SCROLL_HORZ ) {
        SetScrollRange( wnd, SB_HORZ, newrect.left, newrect.right - clrect.right, FALSE );
        SetScrollPos( wnd, SB_HORZ, offset.x, TRUE );
    }
    if( GetScrollConfig() & SCROLL_VERT ) {
         SetScrollRange( wnd, SB_VERT, newrect.top, newrect.bottom - clrect.bottom, FALSE );
         SetScrollPos( wnd, SB_VERT, offset.y, TRUE );
    }
}

#define SLOW_DOWN_N 2
#define SLOW_DOWN_D 3

extern void AutoScroll( RECT eatom, POINT delta )
/***********************************************/
{
    POINT   offset;
    RECT    clrect;
    HWND    wnd;
    int     xdel;
    int     ydel;
    POINT   mouse;

    eatom = eatom;   /* ref'd to avoid warnings */
    if( GetScrollConfig() == SCROLL_NONE ) {
        return;
    }
    wnd = GetAppWnd();
    GetClientRect( wnd, &clrect );
    GetOffset( &offset );
    OffsetRect( &clrect, offset.x, offset.y );
    xdel = 0;
    ydel = 0;
    mouse = GetPrevMouse();
    mouse.x += delta.x;
    mouse.y += delta.y;

    if( (delta.x > 0 &&
         (abs( mouse.x - clrect.right ) < GetHorizontalInc() ||
          mouse.x > clrect.right)) ||
        (delta.x < 0 &&
         (abs( mouse.x - clrect.left ) < GetHorizontalInc() ||
          mouse.x < clrect.left)) ) {
        xdel = delta.x;
        if( xdel > 0 ) {
            xdel = max( 1, (xdel * SLOW_DOWN_N) / SLOW_DOWN_D );
        } else {
            xdel = min( -1, (xdel * SLOW_DOWN_N) / SLOW_DOWN_D );
        }
    }
    if( (delta.y > 0 &&
         (abs( mouse.y - clrect.bottom ) < GetVerticalInc() ||
          mouse.y > clrect.bottom))  ||
        (delta.y < 0 &&
         (abs( mouse.y - clrect.top ) < GetVerticalInc() ||
          mouse.y < clrect.top)) ) {
        ydel = delta.y;
        if( ydel > 0 ) {
            ydel = max( 1, (ydel * SLOW_DOWN_N) / SLOW_DOWN_D );
        } else {
            ydel = min( -1, (ydel * SLOW_DOWN_N) / SLOW_DOWN_D );
        }
    }

    if( !(GetScrollConfig() & SCROLL_VERT) ) {
        ydel = 0;
    }
    if( !(GetScrollConfig() & SCROLL_HORZ) ) {
        xdel = 0;
    }
    DoScroll( wnd, xdel, ydel, TRUE );
    UpdateWindow( wnd );
}

static int AdjustScroll( int curr, int low, int high, int delta )
/***************************************************************/
{
    /* Adjust 'delta' to keep the scrolling in the current scrolling rect. */
    /* 'low' and 'high' are the boundries of the scrolling rect */
    if( curr + delta < low ) {
        return( low - curr );
    } else if( curr + delta > high ) {
        return( high - curr );
    } else {
        return( delta );
    }
}

#define _constrain_short( l ) \
        max( (long)SHRT_MIN, min( (long)SHRT_MAX, (l) ) )

static void GetScrollBounds( LPRECT rect, BOOL lock )
/***************************************************/
{
    // if this is a locked scrolling the scroll bound is the scroll rect,
    // otherwise the bottom and right of bounds are the largest possible that
    // will satisfy the following:
    //      1. They are both shorts (i.e. in the range SHRT_MIN to SHRT_MAX).
    //      2. The maximum distance that you can scroll away from the top, left
    //          corner of the scroll rect is SHRT_MAX - UNLOCK_SCROLL_FUDGE.
    RECT    clrect;
    long    bound;

    *rect = GetScrollRect();
    if( !lock ) {
        GetClientRect( GetAppWnd(), &clrect );
        bound = (long)rect->left + SHRT_MAX - clrect.right - UNLOCK_SCROLL_FUDGE;
        rect->right = _constrain_short( bound );
        bound = (long)rect->top + SHRT_MAX - clrect.bottom - UNLOCK_SCROLL_FUDGE;
        rect->bottom = _constrain_short( bound );
    }
}

static void DoScroll( HWND wnd, int dx, int dy, BOOL lock )
/*********************************************************/
{
    /* scroll 'wnd' by 'dx' and 'dy'. 'lock' says to lock the scrolling on the */
    /* bottom and right into the scrolling rectangle */
    POINT   offset;
    RECT    srect;

    GetOffset( &offset );
    GetScrollBounds( &srect, lock );

    dx = AdjustScroll( offset.x, srect.left, srect.right, dx );
    dy = AdjustScroll( offset.y, srect.top, srect.bottom, dy );
    if( dx != 0 || dy != 0 ) {
        offset.x += dx;
        offset.y += dy;
        SetOffset( offset );
        ScrollWindow( wnd, -dx, -dy, NULL, NULL );
        if( GetScrollConfig() & SCROLL_HORZ ) {
            SetScrollPos( wnd, SB_HORZ, offset.x, TRUE );
        }
        if( GetScrollConfig() & SCROLL_VERT ) {
            SetScrollPos( wnd, SB_VERT, offset.y, TRUE );
        }
    }
}


void WINEXP UpdateScroll( void )
/******************************/
{
    RECT  newrect;
    RECT  clrect;
    POINT offset;
    HWND  wnd;

    if( GetScrollConfig() == SCROLL_NONE ) {
        return;
    }
    wnd = GetAppWnd();
    GetClientRect( wnd, &clrect );
    newrect = clrect;
    RequestScrollRect( &newrect );
    SetScrollRect( newrect );
    GetOffset( &offset );
    if( GetScrollConfig() & SCROLL_HORZ ) {
        SetScrollRange( wnd, SB_HORZ, newrect.left, newrect.right - clrect.right, FALSE );
        SetScrollPos( wnd, SB_HORZ, offset.x, TRUE );
    }
    if( GetScrollConfig() & SCROLL_VERT ) {
        SetScrollRange( wnd, SB_VERT, newrect.top, newrect.bottom - clrect.bottom, FALSE );
        SetScrollPos( wnd, SB_VERT, offset.y, TRUE );
    }
}
