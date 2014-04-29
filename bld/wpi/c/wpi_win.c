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
* Description:  The Windows version of WPI.
*
****************************************************************************/


#include <stdlib.h>

#include <wwindows.h>
#include "wpi.h"

void _wpi_getbitmapdim( HBITMAP bmp, int *pwidth, int *pheight )
/**********************************************************************/
{
    BITMAP              bmp_info;

    GetObject( bmp, sizeof( BITMAP ), (LPSTR)&bmp_info );
    *pwidth = bmp_info.bmWidth;
    *pheight = bmp_info.bmHeight;
} /* _wpi_getbitmapdim */

void _wpi_getpaintrect( PAINTSTRUCT *ps, WPI_RECT *rect )
/**********************************************************************/
{
    rect->left = ps->rcPaint.left;
    rect->top = ps->rcPaint.top;
    rect->right = ps->rcPaint.right;
    rect->bottom = ps->rcPaint.bottom;
} /* _wpi_getpaintrect */

void _wpi_preparemono( WPI_PRES hdc, WPI_COLOUR colour, WPI_COLOUR back_colour )
/**********************************************************************/
/* This is necessary for bltting a monochrome bitmap.  The colour     */
/* parameter should be the colour of the 0's on the bitmap (normally  */
/* black).  The back_colour parameter should be the colour of the 1's */
/* on the bitmap (normally white)                                     */
{
    SetBkColor( hdc, back_colour );
    SetTextColor( hdc, colour );
} /* _wpi_preparemono */

void _wpi_setpoint( WPI_POINT *pt, int x, int y )
/**********************************************************************/
{
    pt->x = (LONG)x;
    pt->y = (LONG)y;
} /* _wpi_setpoint */

WPI_PROC _wpi_subclasswindow( HWND hwnd, WPI_PROC new )
{
    WPI_PROC    old;

    old = (WPI_PROC)GET_WNDPROC( hwnd );
    SET_WNDPROC( hwnd, (LONG_PTR)new );
    return( old );
}

BOOL _wpi_insertmenu( HMENU hmenu, unsigned pos, unsigned menu_flags,
                      unsigned attr_flags, unsigned id,
                      HMENU popup, const char *text, BOOL by_position )
{
    if( !hmenu ) {
        return( FALSE );
    }

    menu_flags |= ( by_position ? MF_BYPOSITION : MF_BYCOMMAND );

    return( InsertMenu( hmenu, pos, menu_flags | attr_flags,
                        ( (menu_flags & MF_POPUP) ? (UINT_PTR)popup : (UINT_PTR)id ),
                        text ) );
}

BOOL _wpi_appendmenu( HMENU hmenu, unsigned menu_flags,
                      unsigned attr_flags, unsigned id,
                      HMENU popup, const char *text )
{
    return( AppendMenu( hmenu, menu_flags | attr_flags | MF_BYPOSITION,
                        ( (menu_flags & MF_POPUP) ? (UINT_PTR)popup : (UINT_PTR)id ),
                        text ) );
}

BOOL _wpi_getmenustate( HMENU hmenu, unsigned id, WPI_MENUSTATE *state,
                        BOOL by_position )
{
    if( !hmenu || !state ) {
        return( FALSE );
    }
    *state = GetMenuState( hmenu, id, (by_position ? MF_BYPOSITION : MF_BYCOMMAND) );
    return( *state != (unsigned short)-1 );
}

void _wpi_getmenuflagsfromstate( WPI_MENUSTATE *state, unsigned *menu_flags,
                                 unsigned *attr_flags )
{
    *menu_flags = (unsigned)(*state);
    *attr_flags = 0;
}

BOOL _wpi_modifymenu( HMENU hmenu, unsigned pos, unsigned menu_flags,
                      unsigned attr_flags, unsigned new_id,
                      HMENU new_popup, const char *new_text, BOOL by_position )
{
    if( !hmenu ) {
        return( FALSE );
    }

    menu_flags |= (by_position ? MF_BYPOSITION : MF_BYCOMMAND);

    return( ModifyMenu( hmenu, pos, menu_flags | attr_flags,
                        ( (menu_flags & MF_POPUP) ? (UINT_PTR)new_popup : (UINT_PTR)new_id ),
                        new_text ) );
}

BOOL _wpi_setmenutext( HMENU hmenu, unsigned id, const char *text, BOOL by_position )
{
    WPI_MENUSTATE       state;
    HMENU               popup;

    if( !_wpi_getmenustate( hmenu, id, (WPI_MENUSTATE *)&state, by_position ) ) {
        return( FALSE );
    }
    popup = (HMENU)NULLHANDLE;
    if( state & MF_POPUP ) {
        if( !by_position ) {
            return( FALSE );
        }
        popup = _wpi_getsubmenu( hmenu, id );
    }
    state = MF_STRING | ((by_position) ? MF_BYPOSITION : MF_BYCOMMAND);
    if( popup != (HMENU)NULLHANDLE ) {
        state |= MF_POPUP;
    }
    return(
        ModifyMenu( hmenu, id, state,
                    (popup != (HMENU)NULLHANDLE) ? (UINT_PTR)popup : (UINT_PTR)id,
                    text ) );
}

BOOL _wpi_getmenutext( HMENU hmenu, unsigned id, char *text, int ctext,
                       BOOL by_position )
{
    return( GetMenuString( hmenu, id, text, ctext, (by_position) ? MF_BYPOSITION : MF_BYCOMMAND ) );
}

BOOL _wpi_setmenu( HWND hwnd, HMENU hmenu )
{
    return( SetMenu( hwnd, hmenu ) );
}

WPI_PRES _wpi_createcompatiblepres( WPI_PRES pres, WPI_INST inst, HDC *hdc )
/**************************************************************************/
{
    HDC         memdc;

    inst = inst;                        // to eliminate compiler messages
    hdc = hdc;
    memdc = CreateCompatibleDC( pres );
    return( memdc );
} /* _wpi_createcompatiblepres */

void _wpi_getbitmapparms( HBITMAP bitmap, int *width, int *height, int *planes,
                                            int *widthbytes, int *bitspixel )
/******************************************************************/
/* Note that the 'bitcount' is the same as widthbytes.            */
{
    BITMAP                      bm;

    GetObject( bitmap, sizeof(BITMAP), &bm );

    if( width ) *width = bm.bmWidth;
    if( height ) *height = bm.bmHeight;
    if( planes ) *planes = (int)bm.bmPlanes;
    if( widthbytes ) *widthbytes = bm.bmWidthBytes;

    // This parameter is the bitcount in PM
    if( bitspixel ) *bitspixel = (int)bm.bmBitsPixel;
} /* _wpi_getbitmapparms */

void _wpi_setqmsgvalues( WPI_QMSG *qmsg, HWND hwnd, WPI_MSG wpi_msg,
                        WPI_PARAM1 wparam, WPI_PARAM2 lparam, ULONG wpi_time,
                        WPI_POINT pt )
/***************************************************************************/
{
    qmsg->hwnd = hwnd;
    qmsg->message = wpi_msg;
    qmsg->wParam = wparam;
    qmsg->lParam = lparam;
    qmsg->time = wpi_time;
    qmsg->pt.x = pt.x;
    qmsg->pt.y = pt.y;
} /* _wpi_setqmsgvalues */

void _wpi_getqmsgvalues( WPI_QMSG qmsg, HWND *hwnd, WPI_MSG *wpi_msg,
                        WPI_PARAM1 *wparam, WPI_PARAM2 *lparam, ULONG *wpi_time,
                        WPI_POINT *pt )
/***************************************************************************/
{
    if( hwnd ) {
        *hwnd = qmsg.hwnd;
    }
    if( wpi_msg ) {
        *wpi_msg = qmsg.message;
    }
    if( wparam ) {
        *wparam = qmsg.wParam;
    }
    if( lparam ) {
        *lparam = qmsg.lParam;
    }
    if( wpi_time ) {
        *wpi_time = qmsg.time;
    }
    if( pt ) {
        pt->x = qmsg.pt.x;
        pt->y = qmsg.pt.y;
    }
} /* _wpi_getqmsgvalues */

void _wpi_setrectvalues( WPI_RECT *rect, WPI_RECTDIM left, WPI_RECTDIM top,
                                    WPI_RECTDIM right, WPI_RECTDIM bottom )
/***********************************************************************/
{
    rect->left = left;
    rect->right = right;
    rect->top = top;
    rect->bottom = bottom;
} /* _wpi_setrectvalues */

void _wpi_setwrectvalues( WPI_RECT *rect, WPI_RECTDIM left, WPI_RECTDIM top,
                                WPI_RECTDIM right, WPI_RECTDIM bottom)
/***********************************************************************/
{
    rect->left = left;
    rect->right = right;
    rect->top = top;
    rect->bottom = bottom;
} /* _wpi_setwrectvalues */

void _wpi_getrectvalues( WPI_RECT rect, WPI_RECTDIM *left, WPI_RECTDIM *top,
                                WPI_RECTDIM *right, WPI_RECTDIM *bottom )
/***********************************************************************/
{
    if( left ) *left = rect.left;
    if( right ) *right = rect.right;
    if( top ) *top = rect.top;
    if( bottom ) *bottom = rect.bottom;
} /* _wpi_getrectvalues */

void _wpi_getwrectvalues( WPI_RECT rect, WPI_RECTDIM *left, WPI_RECTDIM *top,
                                    WPI_RECTDIM *right, WPI_RECTDIM *bottom)
/***********************************************************************/
{
    if( left ) *left = rect.left;
    if( right ) *right = rect.right;
    if( top ) *top = rect.top;
    if( bottom ) *bottom = rect.bottom;
} /* _wpi_getwrectvalues */

void _wpi_setintrectvalues( WPI_RECT *rect, int left, int top, int right,
                                                                    int bottom )
/***********************************************************************/
{
    rect->left = left;
    rect->right = right;
    rect->top = top;
    rect->bottom = bottom;
} /* _wpi_setintrectvalues */

void _wpi_setintwrectvalues( WPI_RECT *rect, int left, int top, int right,
                                                                    int bottom)
/***********************************************************************/
{
    rect->left = left;
    rect->right = right;
    rect->top = top;
    rect->bottom = bottom;
} /* _wpi_setintwrectvalues */

void _wpi_getintwrectvalues( WPI_RECT rect, int *left, int *top, int *right,
                                                                    int *bottom)
/***********************************************************************/
{
    if( left ) *left = rect.left;
    if( right ) *right = rect.right;
    if( top ) *top = rect.top;
    if( bottom ) *bottom = rect.bottom;
} /* _wpi_getintwrectvalues */

void _wpi_getintrectvalues( WPI_RECT rect, int *left, int *top, int *right,
                                                                   int *bottom )
/***********************************************************************/
{
    if( left ) *left = rect.left;
    if( right ) *right = rect.right;
    if( top ) *top = rect.top;
    if( bottom ) *bottom = rect.bottom;
} /* _wpi_getintrectvalues */

void _wpi_getcurrpos( WPI_PRES pres, WPI_POINT *pt )
/**************************************************/
{
    POINT               new_pt;

    GetCurrentPositionEx( pres, &new_pt );
    pt->x = new_pt.x;
    pt->y = new_pt.y;
} /* _wpi_getcurrpos */

void _wpi_suspendthread( UINT thread_id, WPI_QMSG *msg )
/******************************************************/
{
    thread_id = thread_id;              // not used in windows

    for( ;; ) {
        _wpi_getmessage( NULL, msg, (HWND)NULLHANDLE, 0, 0 );

        if( _wpi_ismessage( (*msg), WM_QUIT ) ) {
            break;
        }
    }
} /* _wpi_suspendthread */

void _wpi_getbmphdrvalues( WPI_BITMAPINFOHEADER bmih, ULONG *size,
            int *cx, int *cy, short *planes, short *bc, int *comp,
            int *size_image, int *xpels, int *ypels, int *used, int *important )
/******************************************************************************/
{
    if( size ) *size = bmih.biSize;
    if( cx ) *cx = bmih.biWidth;
    if( cy ) *cy = bmih.biHeight;
    if( planes ) *planes = bmih.biPlanes;
    if( bc ) *bc = bmih.biBitCount;
    if( comp ) *(comp) = bmih.biCompression;
    if( size_image ) *size_image = bmih.biSizeImage;
    if( xpels ) *xpels = bmih.biXPelsPerMeter;
    if( ypels ) *ypels = bmih.biYPelsPerMeter;
    if( used ) *used = bmih.biClrUsed;
    if( important ) *important = bmih.biClrImportant;
} /* _wpi_getbmphdrvalues */

void _wpi_setbmphdrvalues( WPI_BITMAPINFOHEADER *bmih, ULONG size,
            int cx, int cy, short planes, short bc, int comp,
            int size_image, int xpels, int ypels, int used, int important )
/*************************************************************************/
{
    bmih->biSize = size;
    bmih->biWidth = cx;
    bmih->biHeight = cy;
    bmih->biPlanes = planes;
    bmih->biBitCount = bc;
    bmih->biCompression = comp;
    bmih->biSizeImage = size_image;
    bmih->biXPelsPerMeter = xpels;
    bmih->biYPelsPerMeter = ypels;
    bmih->biClrUsed = used;
    bmih->biClrImportant = important;
} /* _wpi_setbmphdrvalues */

void _wpi_gettextextent( WPI_PRES pres, LPCSTR string, int len_string,
                                                    int *width, int *height )
/***************************************************************************/
{
    SIZE        size;

    GetTextExtentPoint( pres, string, len_string, &size );

    *width = (int)size.cx;
    *height = (int)size.cy;
} /* _wpi_gettextextent */

void _wpi_getrestoredrect( HWND hwnd, WPI_RECT *prect )
/*****************************************************/
{
    WINDOWPLACEMENT     place;

    place.length = sizeof( WINDOWPLACEMENT );
    GetWindowPlacement( hwnd, &place );
    CopyRect( prect, &place.rcNormalPosition );
} /* _wpi_getrestoredrect */

void _wpi_setrestoredrect( HWND hwnd, WPI_RECT *prect )
/*****************************************************/
{
    WINDOWPLACEMENT     place;

    place.length = sizeof( WINDOWPLACEMENT );
    CopyRect( &place.rcNormalPosition, (prect) );
    place.showCmd = SW_SHOWNORMAL;

    SetWindowPlacement( (hwnd), &place );
} /* _wpi_setrestoredrect */

void _wpi_setrgbquadvalues( WPI_RGBQUAD *rgb, BYTE red, BYTE green,
                                                    BYTE blue, BYTE option )
/**************************************************************************/
{
    rgb->rgbRed = red;
    rgb->rgbGreen = green;
    rgb->rgbBlue = blue;
    rgb->rgbReserved = option;
} /* _wpi_setrgbquadvalues */

int _wpi_dlg_command( HWND dlg_hld, WPI_MSG *msg, WPI_PARAM1 *parm1,
                                                            WPI_PARAM2 *parm2 )
/*****************************************************************************/
{
    dlg_hld = dlg_hld;
    parm1 = parm1;
    parm2 = parm2;

    if( *msg == WM_COMMAND ) {
        return( TRUE );
    }

    return( FALSE );
} /* _wpi_dlg_command */
