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


#include <string.h>
#include <stdio.h>
#include "imgedit.h"
#include "ieclrpal.h"
#include "ieprofil.h"

/*
 * paintPalette - Repaint the colour palette
 */
static void paintPalette( HWND hwnd )
{
    WPI_PRES            hdc;
    WPI_POINT           pt;
    HPEN                holdpen;
    HPEN                hgraypen;
    HPEN                hwhitepen;
    PAINTSTRUCT         rect;
    WPI_RECT            client;
    int                 height;

    hdc = _wpi_beginpaint(hwnd, NULL, &rect);
#ifdef __OS2_PM__
    WinFillRect( hdc, &rect, CLR_PALEGRAY );
#endif

    _wpi_torgbmode( hdc );
    GetClientRect( hwnd, &client );
    height = _wpi_getheightrect( client );

    hgraypen = _wpi_createpen( PS_SOLID, 0, DKGRAY );
    holdpen = _wpi_selectobject( hdc, hgraypen );
    pt.x = 2;
    pt.y = 50;
    _wpi_cvth_pt( &(pt), height );
    _wpi_movetoex(hdc, &pt, NULL);

    pt.y = 6;
    _wpi_cvth_pt( &(pt), height );
    _wpi_lineto(hdc, &pt);
    pt.x = 90;
    _wpi_lineto(hdc, &pt);

    _wpi_selectobject( hdc, holdpen );
    _wpi_deleteobject( hgraypen );

    hwhitepen = _wpi_createpen( PS_SOLID, 0, WHITE );
    holdpen = _wpi_selectobject( hdc, hwhitepen );
    pt.y = 50;
    _wpi_cvth_pt( &(pt), height );
    _wpi_lineto(hdc, &pt);
    pt.x = 2;
    _wpi_lineto(hdc, &pt);

    _wpi_selectobject( hdc, holdpen );
    _wpi_deleteobject( hwhitepen );
    _wpi_endpaint(hwnd, hdc, &rect);
} /* paintPalette */

/*
 * ColourPalWinProc - handle messages for the colour palette.
 */
MRESULT CALLBACK ColourPalWinProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 mp1, WPI_PARAM2 mp2 )
{
    HMENU               sysmenu;
    WPI_RECT            rcpal;
    IMGED_DIM           left, right, top, bottom;
    static HMENU        menu;
    static HWND         hframe;
    static HBRUSH       hbrush;

    switch( msg ) {

    case WM_CREATE:
        hframe = _wpi_getframe( hwnd );
        sysmenu = _wpi_getcurrentsysmenu( hframe );
        _wpi_deletemenu( sysmenu, SC_RESTORE, FALSE );
        _wpi_deletemenu( sysmenu, SC_SIZE, FALSE );
        _wpi_deletemenu( sysmenu, SC_MINIMIZE, FALSE );
        _wpi_deletemenu( sysmenu, SC_MAXIMIZE, FALSE );
        _wpi_deletemenu( sysmenu, SC_TASKLIST, FALSE );
#ifdef __OS2_PM__
        _wpi_deletemenu( sysmenu, SC_HIDE, FALSE );
#endif
        _wpi_deletesysmenupos( sysmenu, 1 );
        _wpi_deletesysmenupos( sysmenu, 2 );
        hbrush = _wpi_createsolidbrush(LTGRAY );
        menu = GetMenu(_wpi_getframe(HMainWindow));
        break;

    case WM_PAINT:
        paintPalette( hwnd );
        break;

#ifndef __OS2_PM__
#ifdef __NT__
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
        SetBkColor( (HDC)mp1, LTGRAY );
        SetTextColor( (HDC)mp1, BLACK );
        return( (DWORD)hbrush );
#else
    case WM_CTLCOLOR:
        if ((HIWORD(mp2) == CTLCOLOR_STATIC) ||
                (HIWORD(mp2) == CTLCOLOR_BTN)) {
            SetBkColor( (HDC)LOWORD(mp1), LTGRAY );
            SetTextColor( (HDC)LOWORD(mp1), BLACK );
            return( (DWORD)hbrush );
        } else {
            return( (LRESULT)NULL );
        }
#endif
#endif

    case WM_MOVE:
        _wpi_getwindowrect( _wpi_getframe(hwnd), &rcpal );
        _wpi_getrectvalues( rcpal, &left, &top, &right, &bottom );
        ImgedConfigInfo.pal_xpos = (short)left;
        ImgedConfigInfo.pal_ypos = (short)top;
        break;

    case WM_CLOSE:
        CheckPaletteItem( menu );
        break;

    case WM_DESTROY:
        _wpi_deleteobject(hbrush);
        break;

    default:
        return( DefWindowProc(hwnd, msg, mp1, mp2) );
    }
    return 0;

} /* ColourPalWinProc */

/*
 * CheckPaletteItem - This procedure handles when the colour palette menu
 *                    item has been selected.
 */
void CheckPaletteItem( HMENU hmenu )
{
    HWND        frame_wnd;

    if ( !HColourPalette ) {
        _wpi_checkmenuitem(hmenu, IMGED_COLOUR, MF_CHECKED, FALSE);
        return;
    }
    frame_wnd = _wpi_getframe( HColourPalette );

    if ( _wpi_isitemchecked(hmenu, IMGED_COLOUR) ) {
        _wpi_checkmenuitem(hmenu, IMGED_COLOUR, MF_UNCHECKED, FALSE);
        ShowWindow(frame_wnd, SW_HIDE);
        ImgedConfigInfo.show_state &= ~SET_SHOW_CLR;
    } else {
        _wpi_checkmenuitem(hmenu, IMGED_COLOUR, MF_CHECKED, FALSE);
        ShowWindow(frame_wnd, SW_SHOWNA );
        _wpi_setfocus( HMainWindow );
        ImgedConfigInfo.show_state |= SET_SHOW_CLR;
    }
} /* CheckPaletteItem */

/*
 * CreateColourPal - create the colour palette window depending on the OS
 *                   we're compiling for.
 */
void CreateColourPal( void )
{
    HMENU       hmenu;
#ifdef __OS2_PM__
    PM_CreateColourPal();
#else
    Win_CreateColourPal();
#endif

    hmenu = GetMenu(_wpi_getframe(HMainWindow));
    if ( ImgedConfigInfo.show_state & SET_SHOW_CLR ) {
        CheckPaletteItem( hmenu );
    }

    CreateCurrentWnd( HColourPalette );
    CreateColourControls( HColourPalette );
} /* CreateColourPal */

#ifndef __OS2_PM__
/*
 * SetRGBValues - Sets the RGB values for the initialized images.
 */
void SetRGBValues( RGBQUAD *argbvals, int upperlimit )
{
    int                 i;
    RGBQUAD             *argb;
    PALETTEENTRY        *pe;
    int                 num;
    HDC                 hdc;

    hdc = GetDC(HColourPalette);
    pe = MemAlloc( upperlimit*sizeof(PALETTEENTRY) );
    num = GetSystemPaletteEntries(hdc, 0, upperlimit, pe);
    ReleaseDC(HColourPalette, hdc);

    argb = argbvals;

    for (i=0; i < min(upperlimit, num); ++i) {
        argb[i].rgbBlue = pe[i].peBlue;
        argb[i].rgbGreen = pe[i].peGreen;
        argb[i].rgbRed = pe[i].peRed;
        argb[i].rgbReserved = 0;
    }
    MemFree( pe );
} /* SetRGBValues */
#endif
