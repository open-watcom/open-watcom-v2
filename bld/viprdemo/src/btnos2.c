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


#define INCL_PM
#include <os2.h>
#include "button.h"

static void button_size( HWND hwnd, int id, int FAR * pwidth, int FAR * pheight )
{
    HBITMAP             bmp;
    BITMAPINFOHEADER    bitmap;
    HPS                 hps;

    hwnd = hwnd;
    hps = WinGetPS( HWND_DESKTOP );
    bmp = GpiLoadBitmap( hps, 0, (ULONG) id, 0, 0 );
    WinReleasePS( hps );
    if( !bmp ) return;
    bitmap.cbFix = sizeof( BITMAPINFOHEADER );
    GpiQueryBitmapParameters( bmp, &bitmap );
    *pwidth = bitmap.cx +5;
    *pheight = bitmap.cy + 5;
}

void add_button(
    HWND        parent,
    int         top,
    int         left,
    int         id,
    int FAR *   pwidth,
    int FAR *   pheight )
{
    HWND        hbutton;

    // Note that the resource ID is the same as the control ID
    button_size( parent, id, pwidth, pheight );

    hbutton = WinCreateWindow(
        parent,                 // parent window
        WC_BUTTON,
        "",
        WS_VISIBLE | BS_USERBUTTON,     // Window style
        left,
        top - *pheight,
        *pwidth,
        *pheight,
        parent,                         // owner window
        HWND_TOP,
        id,
        NULL,
        NULL
    );
}

ULONG measure_button(HWND parent, MPARAM mp1, MPARAM mp2 )
{
    int width, height;
    int button_id;

    button_id = SHORT1FROMMP( mp1 );
    mp2 = mp2;
    button_size( parent, button_id, &width, &height );
    return( (ULONG) MPFROM2SHORT( width, height ) );
}

static void horizontal( HPS hps, RECTL rect, int row )
{
    POINTL              pt;
    pt.x = rect.xLeft + 1;
    pt.y = row;
    GpiSetCurrentPosition( hps, &pt );
    pt.x = rect.xRight - 2;
    pt.y = row;
    GpiLine( hps, &pt );
}

static void vertical( HPS hps, RECTL rect, int column )
{
    POINTL              pt;
    pt.x = column;
    pt.y = rect.yBottom + 1;
    GpiSetCurrentPosition( hps, &pt );
    pt.y = column;
    pt.y = rect.yTop - 2;
    GpiLine( hps, &pt );
}

void draw_button( MPARAM mp1, MPARAM mp2 )
{
    HBITMAP             bmp;
    HBITMAP             oldbmp;
    LINEBUNDLE          blackpen;
    LINEBUNDLE          shadowpen;
    LINEBUNDLE          brightpen;
    LINEBUNDLE          facepen;
    LINEBUNDLE          oldpen;
    BITMAPINFOHEADER    bitmap;
    HPS                 memdc;
    HPS                 tmp_ps;
    HDC                 hdc;
    HAB                 hab;
    int                 shift;
    SIZEL               sizl = { 0, 0 };
    USERBUTTON          *b2;
    RECTL               rect;
    POINTL              pts[3];
    int                 button_id;
    DEVOPENSTRUC        dop = { 0L, "DISPLAY", NULL, 0L,
                                0L, 0L, 0L, 0L, 0L };

    b2 = (USERBUTTON *) mp2;
    WinQueryWindowRect( b2->hwnd, &rect );
    button_id = SHORT1FROMMP( mp1 );

    hab = WinQueryAnchorBlock( b2->hwnd );
    tmp_ps = WinGetPS( HWND_DESKTOP );
    bmp = GpiLoadBitmap( tmp_ps, 0, (ULONG) button_id, 0, 0 );
    WinReleasePS( tmp_ps );
    if( !bmp ) return;

    bitmap.cbFix = sizeof( BITMAPINFOHEADER );
    GpiQueryBitmapParameters( bmp, &bitmap );

    hdc = DevOpenDC( hab, OD_MEMORY, "*", 5L,
                                        (PDEVOPENDATA)&dop, NULLHANDLE );
    memdc = GpiCreatePS( hab, hdc, &sizl, PU_PELS | GPIA_ASSOC );

    oldbmp = GpiSetBitmap( memdc, bmp );

    if( LOUSHORT( b2->fsState ) == BDS_HILITED ) {
        shift = 4;
    } else {
        shift = 2;
    }

    pts[0].x = rect.xLeft + shift;
    pts[0].y = rect.yBottom + 5 - shift;
    pts[1].x = rect.xLeft + shift + bitmap.cx;
    pts[1].y = rect.yBottom + 5 - shift + bitmap.cy;
    pts[2].x = 0;
    pts[2].y = 0;
    GpiBitBlt( b2->hps, memdc, 3, pts, ROP_SRCCOPY, BBO_IGNORE );

    GpiSetBitmap( memdc, oldbmp );
    GpiDestroyPS( memdc );
    DevCloseDC( hdc );
    GpiDeleteBitmap( bmp );

    // Draw four sides of the button except one pixel in each corner
    blackpen.lColor = CLR_BLACK;
    blackpen.usType = LINETYPE_SOLID;
    blackpen.usMixMode = FM_OVERPAINT;
    brightpen.lColor = CLR_WHITE;
    brightpen.usType = LINETYPE_SOLID;
    brightpen.usMixMode = FM_OVERPAINT;
    shadowpen.lColor = SYSCLR_BUTTONDARK;
    shadowpen.usType = LINETYPE_SOLID;
    shadowpen.usMixMode = FM_OVERPAINT;
    facepen.lColor = SYSCLR_BUTTONMIDDLE;
    facepen.usType = LINETYPE_SOLID;
    facepen.usMixMode = FM_OVERPAINT;

    GpiQueryAttrs( b2->hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                                LBB_MIX_MODE, &oldpen );
    GpiSetAttrs( b2->hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                                LBB_MIX_MODE, 0L, &blackpen );

    horizontal( b2->hps, rect, rect.yBottom );
    horizontal( b2->hps, rect, rect.yTop - 1 );
    vertical( b2->hps, rect, rect.xLeft );
    vertical( b2->hps, rect, rect.xRight - 1 );
   // Now the shading

    GpiSetAttrs( b2->hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                                LBB_MIX_MODE, 0L, &shadowpen );

    if( LOUSHORT( b2->fsState ) == BDS_HILITED ) {
        horizontal( b2->hps, rect, rect.yTop - 2 );
        vertical( b2->hps, rect, rect.xLeft + 1 );

        GpiSetAttrs( b2->hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                                LBB_MIX_MODE, 0L, &facepen );

        horizontal( b2->hps, rect, rect.yTop - 3 );
        vertical( b2->hps, rect, rect.xLeft + 2 );
        horizontal( b2->hps, rect, rect.yTop - 4 );
        vertical( b2->hps, rect, rect.xLeft + 3 );
    } else {
        horizontal( b2->hps, rect, rect.yBottom + 1 );
        horizontal( b2->hps, rect, rect.yBottom + 2 );
        vertical( b2->hps, rect, rect.xRight - 2 );
        vertical( b2->hps, rect, rect.xRight - 3 );

        GpiSetAttrs( b2->hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                                LBB_MIX_MODE, 0L, &brightpen );

        horizontal( b2->hps, rect, rect.yTop - 2 );
        vertical( b2->hps, rect, rect.xLeft + 1 );
    }

    GpiSetAttrs( b2->hps, PRIM_LINE, LBB_COLOR | LBB_WIDTH | LBB_TYPE |
                                                LBB_MIX_MODE, 0L, &oldpen );
    b2->fsStateOld = b2->fsState = 0;
}
