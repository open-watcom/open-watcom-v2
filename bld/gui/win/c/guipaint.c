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


#include "guiwind.h"
#include "guixscal.h"
#include "guicolor.h"
#include "guipaint.h"

extern  WPI_INST        GUIMainHInst;

/*
 * GUIPaint -- respond to WM_PAINT message
 */

#ifndef __OS2_PM__FOO
void GUIPaint( gui_window *wnd, HWND hwnd, bool isdlg )
{
    HDC         prev_hdc;
    PAINTSTRUCT *prev_ps;
    gui_row_num row_num;
    PAINTSTRUCT ps;
    WPI_RECT    fill_area;
#ifdef __OS2_PM__
    ULONG       flags;
    RECTL       client;
#endif

    isdlg=isdlg;
    /* save old state */
    prev_hdc = wnd->hdc;
    prev_ps = wnd->ps;

    /* Setup Paint */
    wnd->ps = &ps;
    wnd->hdc = _wpi_beginpaint( hwnd, NULLHANDLE, wnd->ps );
    _wpi_torgbmode( wnd->hdc );
    _wpi_getpaintrect( wnd->ps, &fill_area );
#ifdef __OS2_PM__
    fill_area = *(wnd->ps);
    if( isdlg ) {
        _wpi_inflaterect( GUIMainHInst, &fill_area, 10, 10);
    }
    WinFillRect( wnd->hdc, &fill_area, GUIGetBack( wnd, GUI_BACKGROUND ) );
#endif
#if defined( __NT__ )
    if( isdlg ) {
        _wpi_fillrect( wnd->hdc, &fill_area, GUIGetBack( wnd, GUI_BACKGROUND ),
                       wnd->bk_brush );
    }
#endif
    if( wnd->font != NULL ) {
        wnd->prev_font = _wpi_selectfont( wnd->hdc, wnd->font );
    } else {
        wnd->prev_font = NULL;
    }

    /* send paint to app */
    GUIGetUpdateRows( wnd, hwnd, &row_num.start, &row_num.num );
    if( row_num.num > 0 ) {
        GUIEVENTWND( wnd, GUI_PAINT, &row_num );
    }

    /* finish painting */
    if( wnd->prev_font != NULL ) {
        _wpi_getoldfont( wnd->hdc, wnd->prev_font );
        wnd->prev_font = NULL;
    }

#ifdef __OS2_PM__
    if( isdlg ) {
        flags = DB_AREAATTRS | DB_DLGBORDER;
        #ifdef __FLAT__
        if( WinQueryActiveWindow( HWND_DESKTOP ) != hwnd ) {
        #else
        if( WinQueryActiveWindow( HWND_DESKTOP, FALSE ) != hwnd ) {
        #endif
        //if( _wpi_getfocus() != hwnd ) {
            flags |= DB_PATINVERT;
        }
        WinQueryWindowRect( hwnd, &client );
        WinDrawBorder( wnd->hdc, &client, 1, 1, 0, 0, flags );
    }
#endif

    _wpi_endpaint( hwnd, wnd->hdc, wnd->ps );

    /* restore old state */
    wnd->hdc = prev_hdc;
    wnd->ps = prev_ps;
}

#else

// this is some experimental PM stuff
void GUIPaint( gui_window *wnd, HWND hwnd, bool isdlg )
{
    HDC         prev_hdc;
    PAINTSTRUCT *prev_ps;
    gui_row_num row_num;
    PAINTSTRUCT ps;
    PAINTSTRUCT fill_area;

    // experimenal stuff
    HPS                 hps;
    RECTL               client;
    LONG                width, height;
    ULONG               flags;
    int                 compat_created;
    gui_paint_info      *pinfo;

    // figure out which paint info to use
    if( !isdlg && wnd->root == hwnd ) {
        pinfo = &wnd->root_pinfo;
    } else {
        pinfo = &wnd->hwnd_pinfo;
    }
    pinfo->in_use++;
    compat_created = FALSE;

    /* save old state */
    prev_hdc = wnd->hdc;
    prev_ps = wnd->ps;
    if( isdlg ) {
        _wpi_getclientrect( hwnd, &client );
    } else {
        WinQueryWindowRect( hwnd, &client );
    }
    width = client.xRight - client.xLeft;
    height = client.yTop - client.yBottom;

    wnd->ps = &ps;
    //hps = _wpi_beginpaint( hwnd, pinfo->normal_pres, wnd->ps );
    hps = _wpi_beginpaint( hwnd, NULL, wnd->ps );
    if( pinfo->compatible_pres == (WPI_PRES)NULL ) {
        compat_created = TRUE;
        pinfo->compatible_pres = _wpi_createcompatiblepres( hps, GUIMainHInst, &pinfo->compatible_hdc );
        pinfo->draw_bmp = _wpi_createcompatiblebitmap( hps, width, height );
        pinfo->old_bmp = _wpi_selectbitmap( pinfo->compatible_pres, pinfo->draw_bmp );
    }

    // the condition is here VERY conservative!!
    // its gains have to analyzed
    if( compat_created || pinfo->force_count ) {
        wnd->hdc = pinfo->compatible_pres;
        //wnd->hdc = hps;
        _wpi_torgbmode( wnd->hdc );
        fill_area = *(wnd->ps);
        if( isdlg ) {
            _wpi_inflaterect( GUIMainHInst, &fill_area, 10, 10);
        }
        WinFillRect( wnd->hdc, &fill_area, GUIGetBack( wnd, GUI_BACKGROUND ) );
        wnd->prev_font = NULL;
        if( wnd->font != NULL ) {
            wnd->prev_font = _wpi_selectfont( wnd->hdc, wnd->font );
        }

        /* send paint to app */
        GUIGetUpdateRows( wnd, hwnd, &row_num.start, &row_num.num );
        if( row_num.num > 0 ) {
            GUIEVENTWND( wnd, GUI_PAINT, &row_num );
        }

        /* finish painting */
        if( wnd->prev_font != NULL ) {
            _wpi_getoldfont( wnd->hdc, wnd->prev_font );
            wnd->prev_font = NULL;
        }

        if( pinfo->force_count ) {
            //pinfo->force_count--;
        }
    }

    //_wpi_bitblt( hps, client.xLeft, client.yBottom, width, height,
    //       wnd->hdc, 0, 0, SRCCOPY );

    if( pinfo->in_use == 1 ) {
        //_wpi_bitblt( hps, client.xLeft, client.yBottom, width, height,
        //               wnd->hdc, 0, 0, SRCCOPY );
        _wpi_bitblt( hps, ps.xLeft, ps.yBottom,
                     ps.xRight - ps.xLeft,
                     ps.yTop - ps.yBottom,
                     wnd->hdc,
                     ps.xLeft, ps.yBottom,
                     SRCCOPY );
        if( pinfo->delete_when_done ) {
            GUIFreePaintHandles( pinfo, TRUE );
        }
    }

    if( isdlg ) {
        flags = DB_AREAATTRS | DB_DLGBORDER;
        #ifdef __FLAT__
        if( WinQueryActiveWindow( HWND_DESKTOP ) != hwnd ) {
        #else
        if( WinQueryActiveWindow( HWND_DESKTOP, FALSE ) != hwnd ) {
        #endif
        //if( _wpi_getfocus() != hwnd ) {
            flags |= DB_PATINVERT;
        }
        WinQueryWindowRect( hwnd, &client );
        WinDrawBorder( hps, &client, 1, 1, 0, 0, flags );
    }

    _wpi_endpaint( hwnd, hps, wnd->ps );

    /* restore old state */
    wnd->hdc = prev_hdc;
    wnd->ps = prev_ps;

    if( pinfo->in_use ) {
        pinfo->in_use--;
    }
}

#endif

void GUIInvalidatePaintHandles( gui_window *wnd )
{
    GUIFreeWndPaintHandles( wnd, FALSE );
}

void GUIFreeWndPaintHandles( gui_window *wnd, int force )
{
    force=force;
    if( wnd == NULL ) {
        return;
    }
#ifdef __OS2_PM__
    GUIFreePaintHandles( &wnd->root_pinfo, force );
    GUIFreePaintHandles( &wnd->hwnd_pinfo, force );
#endif
}

void GUIFreePaintHandles( gui_paint_info *pinfo, int force )
{
    if( pinfo == NULL ) {
        return;
    }

    if( !force && pinfo->in_use ) {
        pinfo->delete_when_done = TRUE;
        return;
    }

#ifdef __OS2_PM__
    //bmp = _wpi_selectbitmap( pinfo->compatible_pres, pinfo->old_bmp );
    _wpi_getoldbitmap( pinfo->compatible_pres, pinfo->old_bmp );
    pinfo->old_bmp = (WPI_HANDLE)NULL;
    if( pinfo->draw_bmp != (WPI_HANDLE)NULL ) {
        _wpi_deletebitmap( pinfo->draw_bmp );
        pinfo->draw_bmp = (WPI_HANDLE)NULL;
    }
    if( pinfo->compatible_pres != (WPI_PRES)NULL ) {
        _wpi_deletecompatiblepres( pinfo->compatible_pres,
                                   pinfo->compatible_hdc );
        pinfo->compatible_pres = (WPI_PRES)NULL;
        pinfo->compatible_hdc = (HDC)NULL;
    }
    pinfo->in_use = 0;
    pinfo->delete_when_done = FALSE;
#endif
}

