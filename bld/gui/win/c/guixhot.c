/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "guiwind.h"
#include "guihot.h"
#include "guixdraw.h"
#include "guixwind.h"
#include "guirdlg.h"


#ifdef __NT__
typedef HANDLE (WINAPI *PFNLI)( HINSTANCE, LPCSTR, UINT, int, int, UINT );
#endif

#ifdef __NT__
// For TransparentBlt function
// #include "wptoolbr.h"
// #include <windows.h>
static HBITMAP      hbitmap2 = WPI_NULL;
static HINSTANCE    hInstUser = NULL;
static PFNLI        pfnLoadImage;
#endif

bool GUIXInitHotSpots( int num_hot_spots, gui_resource *hot )
{
    int         i;
    int         bm_w;
    int         bm_h;

    for( i = 0; i < num_hot_spots; i++ ) {
#ifdef __NT__
        if( hInstUser == NULL ) {
            hInstUser = GetModuleHandle( "USER32.DLL" );
            pfnLoadImage = (PFNLI)GetProcAddress( hInstUser, "LoadImageA" );
        }
        if( pfnLoadImage != NULL ) {
            GUIHotSpots[i].hbitmap = pfnLoadImage( GUIResHInst,
                                                  MAKEINTRESOURCE( hot[i].res_id ),
                                                  IMAGE_BITMAP, 0, 0,
                                                  LR_LOADMAP3DCOLORS );
        } else {
#endif
            GUIHotSpots[i].hbitmap = _wpi_loadbitmap( GUIResHInst, MAKEINTRESOURCE( hot[i].res_id ) );
#ifdef __NT__
        }
#endif
        _wpi_getbitmapdim( GUIHotSpots[i].hbitmap, &bm_w, &bm_h );
        GUIHotSpots[i].size.x = bm_w;
        GUIHotSpots[i].size.y = bm_h;
    }
    return( true );
}

void GUIXCleanupHotSpots( void )
{
    int i;

    for( i = 0; i < GUINumHotSpots; i++ ) {
        _wpi_deletebitmap( GUIHotSpots[i].hbitmap );
    }
#ifdef __NT__
    if( hbitmap2 != WPI_NULL ) {
        _wpi_deletebitmap( GUIHotSpots[i].hbitmap );
    }
#endif
}

void GUIAPI GUIDrawHotSpot( gui_window *wnd, int hotspot_no, gui_text_ord row, gui_ord indent, gui_attr attr )
{
    gui_text_metrics    metrics;
    gui_coord           pos;

    if( ( hotspot_no > 0 ) && ( hotspot_no <= GUINumHotSpots ) ) {
        GUIGetTextMetrics( wnd, &metrics );
        pos.x = indent;
        pos.y = row * metrics.avg.y;
        GUIDrawBitmapAttr( wnd, &GUIHotSpots[hotspot_no - 1].size, &pos, attr, hotspot_no );
    }
}

void GUIDrawBitmap( int hotspot_no, WPI_PRES hdc, int nDrawX, int nDrawY, WPI_COLOUR bkcolour )
{
    WPI_POINT   src_wpi_point;
    WPI_POINT   dst_wpi_point;
    WPI_POINT   size_wpi_point;
    WPI_PRES    memDC;
    WPI_HBITMAP old_hbitmap;
    WPI_HBITMAP hbitmap;
    HDC         new_hdc;
#ifdef __NT__
    HDC         new_hdc2;
    HBITMAP     old_hbitmap2;
    HDC         mem2;
    COLORREF    cr;
#endif

#ifndef __NT__
    /* unused parameters */ (void)bkcolour;
#endif

    hbitmap = GUIHotSpots[hotspot_no - 1].hbitmap;
    size_wpi_point.x = GUIHotSpots[hotspot_no - 1].size.x;
    size_wpi_point.y = GUIHotSpots[hotspot_no - 1].size.y;

    src_wpi_point.x = 0;
    src_wpi_point.y = 0;
    dst_wpi_point.x = nDrawX;
    dst_wpi_point.y = nDrawY;

    _wpi_dptolp( hdc, &size_wpi_point, 1 );
    _wpi_dptolp( hdc, &dst_wpi_point, 1 );
    _wpi_dptolp( hdc, &src_wpi_point, 1 );

    memDC = _wpi_createcompatiblepres( hdc, GUIMainHInst, &new_hdc );
    old_hbitmap = _wpi_selectbitmap( memDC, hbitmap );

#ifdef __NT__
    /* Skip transparency for huge bitmaps, only splashes and such... */
    if( size_wpi_point.x < 50 && size_wpi_point.y < 50) {
        /* New, on WIN32 platforms, use TB_TransparentBlt() */
        mem2 = _wpi_createcompatiblepres( hdc, GUIMainHInst, &new_hdc2 );
        if( hbitmap2 == WPI_NULL)
           hbitmap2 = CreateCompatibleBitmap( hdc, 50, 50 );
        old_hbitmap2 = _wpi_selectbitmap( mem2, hbitmap2 );
        /* Get background color of bitmap */
        /* Expects 0,0 pos in original to be in background/transp. color */
        cr = GetPixel(memDC, 0, 0);
        /* IMPORTANT: must set required new background color for dest bmp */
        // SetBkColor( mem2, GetSysColor(COLOR_BTNFACE) );
        SetBkColor( mem2, bkcolour );

        TB_TransparentBlt( mem2, src_wpi_point.x, src_wpi_point.y, size_wpi_point.x, size_wpi_point.y, memDC, cr );

        _wpi_bitblt( hdc, dst_wpi_point.x, dst_wpi_point.y, size_wpi_point.x, size_wpi_point.y, mem2,
                     src_wpi_point.x, src_wpi_point.y, SRCCOPY );

        /* Clean up */
        _wpi_selectbitmap( mem2, old_hbitmap2 );
        _wpi_deletecompatiblepres( mem2, hdc );
    } else {
        // Normal for large bitmaps...
        _wpi_bitblt( hdc, dst_wpi_point.x, dst_wpi_point.y, size_wpi_point.x, size_wpi_point.y, memDC,
                     src_wpi_point.x, src_wpi_point.y, SRCCOPY );
    }

#else

    _wpi_bitblt( hdc, dst_wpi_point.x, dst_wpi_point.y, size_wpi_point.x, size_wpi_point.y, memDC,
                 src_wpi_point.x, src_wpi_point.y, SRCCOPY );

#endif

    if( old_hbitmap != WPI_NULL ) {
        _wpi_getoldbitmap( memDC, old_hbitmap );
    }
    _wpi_deletecompatiblepres( memDC, new_hdc );
}
