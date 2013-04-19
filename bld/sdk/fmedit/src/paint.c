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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <windows.h>

/* imports */

#include "fmedit.def"
#include "state.def"
#include "object.def"

/* colour constants */

#define DARKGRAY    0x00808080
#define LIGHTGRAY   0x00FFFFFF

extern void DrawTempRect( LPRECT rect )
/*************************************/
{
    /* draws the shadow of an object being drawn for the first time, moved or
     * sized
     */
    HANDLE wnd;
    POINT  offset;
    HDC    hdc;

    wnd = GetAppWnd();
    hdc = GetDC( wnd ) ;
    GetOffset( &offset );
#ifdef __NT__
    SetWindowOrgEx( hdc, offset.x, offset.y, NULL );
#else
    SetWindowOrg( hdc, offset.x, offset.y );
#endif
    DrawFocusRect( hdc, rect );
    ReleaseDC( wnd, hdc );
}


static void OutlineRect( LPRECT rect, BOOL dbl, HDC hdc )
/*******************************************************/
{
    /* draws a rectangle at the indicated location */
    RECT    temprect;

    temprect = *rect;
    FrameRect( hdc, rect, (HBRUSH) GetStockObject( BLACK_BRUSH ) );
    if( dbl ) {
        temprect = *rect;
        InflateRect( &temprect, -1, -1 );
        FrameRect( hdc, &temprect, (HBRUSH) GetStockObject( BLACK_BRUSH ) );
    }
}


void FMEDITAPI OutlineDoubleRect( LPRECT rect, HDC hdc )
/******************************************************/
{
    /* draw a double outline around the passed rect */
    OutlineRect( rect, TRUE, hdc );
}


void FMEDITAPI OutlineSingleRect( LPRECT rect, HDC hdc )
/******************************************************/
{
    /* draw a double outline around the passed rect */
    OutlineRect( rect, FALSE, hdc );
}

void FMEDITAPI DarkGreyRect( LPRECT rect, LPSTR label, HDC hdc )
/**************************************************************/
{
    /* draw a framed grey rectangle with the given label */
    DWORD savebg;

    savebg = SetBkColor( hdc, DARKGRAY );
    FillRect( hdc, rect, (HBRUSH) GetStockObject( GRAY_BRUSH ) );
    if( label != NULL ) {
        DrawText( hdc, label, -1, rect, DT_WORDBREAK );
    }
    SetBkColor( hdc, savebg );
    OutlineRect( rect, FALSE, hdc );
}


void FMEDITAPI DrawConstText( LPRECT rect, LPSTR value, HDC hdc )
/***************************************************************/
{
    /*  draw a constant text value. Adjust the size of the rect
     *  so that no partial text lines appear.
     */
    if( value == NULL ) {
        return;
    }
    DrawText( hdc, value, -1, rect, DT_WORDBREAK );
}


#define CTL3D_BORDER        2

void FMEDITAPI MarkInvalid( LPRECT rect )
/***************************************/
{
    /* mark a rectangular region invalid */
    POINT   offset;
    RECT    temprect;
    HWND    wnd;

    wnd = GetAppWnd();
    if( wnd == NULL ) {
        return;
    }
    temprect = *rect;
    GetOffset( &offset );
    InflateRect( &temprect, CTL3D_BORDER, CTL3D_BORDER );
    OffsetRect( &temprect, -offset.x, -offset.y );
    InvalidateRect( wnd, &temprect, TRUE );
}

static void DrawCurr( STATE_ID st, RECT *rect, HDC *hdc )
/*******************************************************/
{
    OBJPTR eatom;

    switch( st ) {
    case MOVING:
    case CREATING:
    case SIZING:
    case PASTEING:
        ExecuteCurrObject( DRAW, rect, hdc );
        break;
    case SELECTING:
        eatom = GetSelectEatom();
        if( eatom != NULL ) {
            Draw( eatom, rect, *hdc );
        }
        break;
    }
}


extern BOOL DoPainting( void )
/****************************/
{
    /* repaint the areas of the screen which require updating */
    PAINTSTRUCT ps;
    HANDLE      wnd;
    POINT       offset;
    HDC         hdc;
    OBJPTR      obj;
    STATE_ID    st;
    BOOL        ret;

    wnd = GetAppWnd();
    BeginPaint( wnd, &ps );
    st = GetState();
    if( !IsRectEmpty( &ps.rcPaint ) ) {
        GetOffset( &offset );
        hdc = ps.hdc;
#ifdef __NT__
        SetWindowOrgEx( hdc, offset.x, offset.y, NULL );
#else
        SetWindowOrg( hdc, offset.x, offset.y );
#endif
        OffsetRect( &ps.rcPaint, offset.x, offset.y );
        DrawCurr( st, &ps.rcPaint, &hdc );
#if 0
        FillRect( hdc, &ps.rcPaint, (HBRUSH) GetStockObject( WHITE_BRUSH ) );
#endif
        obj = GetMainObject();
        if( obj != NULL ) {
            Draw( obj, &ps.rcPaint, hdc );
        }
        DrawCurr( st, &ps.rcPaint, &hdc );
        ret = TRUE;
    } else {
        ret = FALSE;
    }
    EndPaint( wnd, &ps );
    return( ret );
}
