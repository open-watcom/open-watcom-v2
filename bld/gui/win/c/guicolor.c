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
* Description:  GUI lib color handling.
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#if defined(__WINDOWS__) && !defined(__WINDOWS_386__)
#include <commdlg.h>
#endif
#include "guicolor.h"
#include "guiwnclr.h"
#include "guix.h"

extern  WPI_INST        GUIMainHInst;

static int init_rgb = 0;

WPI_COLOUR GUIColours[] = {
#ifdef __OS2_PM__
//      R G B
    0x00000000, /* GUI_BLACK           */
    0x00000080, /* GUI_BLUE            */
    0x00008000, /* GUI_GREEN           */
    0x00008080, /* GUI_CYAN            */
    0x00C00000, /* GUI_RED             */
    0x00800080, /* GUI_MAGENTA         */
    0x00808000, /* GUI_BROWN           */
    0x00cccccc, /* GUI_WHITE           */
    0x00808080, /* GUI_GREY            */
    0x000000ff, /* GUI_BRIGHT_BLUE     */
    0x0000ff00, /* GUI_BRIGHT_GREEN    */
    0x0000ffff, /* GUI_BRIGHT_CYAN     */
    0x00ff0000, /* GUI_BRIGHT_RED      */
    0x00ff00ff, /* GUI_BRIGHT_MAGENTA  */
    0x00ffff00, /* GUI_BRIGHT_YELLOW   */
    0x00ffffff, /* GUI_BRIGHT_WHITE    */
    0x00808080, /* GUIEX_DLG_BKGRND    */
    0x00FFFFFF, /* GUIEX_WND_BKGRND    */
    0x00000080, /* GUIEX_HIGHLIGHT     */
    0x00FFFFFF  /* GUIEX_HIGHLIGHTTEXT */
#else
//      B G R
    0x00000000, /* GUI_BLACK           */
    0x00800000, /* GUI_BLUE            */
    0x00008000, /* GUI_GREEN           */
    0x00808000, /* GUI_CYAN            */
    0x000000C0, /* GUI_RED             */
    0x00800080, /* GUI_MAGENTA         */
    0x00008080, /* GUI_BROWN           */
    0x00c0c0c0, /* GUI_WHITE           */
    0x00808080, /* GUI_GREY            */
    0x00ff0000, /* GUI_BRIGHT_BLUE     */
    0x0000ff00, /* GUI_BRIGHT_GREEN    */
    0x00ffff00, /* GUI_BRIGHT_CYAN     */
    0x000000ff, /* GUI_BRIGHT_RED      */
    0x00ff00ff, /* GUI_BRIGHT_MAGENTA  */
    0x0000ffff, /* GUI_BRIGHT_YELLOW   */
    0x00ffffff, /* GUI_BRIGHT_WHITE    */
    0x00808080, /* GUIEX_DLG_BKGRND    */
    0x00FFFFFF, /* GUIEX_WND_BKGRND    */
    0x00800000, /* GUIEX_HIGHLIGHT     */
    0x00FFFFFF  /* GUIEX_HIGHLIGHTTEXT */
#endif
};

#define NUM_COLOURS ( sizeof( GUIColours ) / sizeof( WPI_COLOUR ) )

void InitSystemRGB( void )
{
#ifndef __OS2_PM__
    // All other colours are hardcoded.
    GUIColours[GUIEX_DLG_BKGRND] = GetSysColor( COLOR_BTNFACE );
    GUIColours[GUIEX_WND_BKGRND] = GetSysColor( COLOR_WINDOW );
    GUIColours[GUIEX_HIGHLIGHT] = GetSysColor( COLOR_HIGHLIGHT );
    GUIColours[GUIEX_HIGHLIGHTTEXT] = GetSysColor( COLOR_HIGHLIGHTTEXT );
#endif
}


bool GUISetRGB( gui_colour colour, gui_rgb rgb )
{
    if( colour <= GUI_LAST_COLOUR  ) {
        GUIColours[colour] = GETRGB( rgb );
        return( TRUE );
    }
    return( FALSE );
}

static void FillInRGB( WPI_COLOUR colour, gui_rgb *rgb )
{
    BYTE        r;
    BYTE        g;
    BYTE        b;

    r = GetRValue( colour );
    g = GetGValue( colour );
    b = GetBValue( colour );
    *rgb = GUIRGB( r, g, b );
}

bool GUIGetRGB( gui_colour colour, gui_rgb *rgb )
{
    if( ( colour <= GUI_LAST_COLOUR  )  && ( rgb != NULL ) ) {
        FillInRGB( GUIColours[colour], rgb );
        return( TRUE );
    }
    return( FALSE );
}

bool GUIGetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set )
{
    if( colour_set == NULL ) {
        return( FALSE );
    }
    if( attr < wnd->num_attrs ) {
        colour_set->fore = wnd->colours[attr].fore;
        colour_set->back = wnd->colours[attr].back;
        return( TRUE );
    }
    return( FALSE );
}

void SetBKBrush( gui_window *wnd )
{
    if (!init_rgb){
        InitSystemRGB();
        init_rgb = 1;
    }

    GUIGetRGB( wnd->colours[GUI_BACKGROUND].back, &wnd->bk_rgb );
    wnd->bk_brush = _wpi_createsolidbrush(GUIGetBack( wnd, GUI_BACKGROUND ));
}

static void ChangeBKBrush( gui_window *wnd )
{
    HBRUSH      prev;
    HBRUSH      curr;

    curr = wnd->bk_brush;
    prev = GUIFreeBKBrush( wnd );
    SetBKBrush( wnd );
#ifndef __OS2_PM__
    if( prev != curr ) {
        /* replce brush if it wasn't really ours */
        SET_HBRBACKGROUND( wnd->hwnd, prev );
    } else {
        SET_HBRBACKGROUND( wnd->hwnd, wnd->bk_brush );
    }
#endif
}

void GUICheckBKBrush( gui_window *wnd )
{
    gui_rgb             rgb;
    gui_colour_set      set;

    GUIGetWndColour( wnd, GUI_BACKGROUND, &set );
    GUIGetRGB( set.back, &rgb );
    if( rgb != wnd->bk_rgb ) {
        ChangeBKBrush( wnd );
    }
}

bool GUISetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set )
{
    if( colour_set == NULL ) {
        return( FALSE );
    }
    if( attr < wnd->num_attrs ) {
        wnd->colours[attr].fore = colour_set->fore;
        wnd->colours[attr].back = colour_set->back;
        if( attr == GUI_BACKGROUND ) {
            ChangeBKBrush( wnd );
        }
        return( TRUE );
    }
    return( FALSE );
}

bool GUIGetRGBFromUser( gui_rgb init_rgb, gui_rgb *new_rgb )
{
#ifndef __OS2_PM__
    CHOOSECOLOR choose;
    WPI_PROC    func;
    bool        ret;
#if !(defined(__NT__) || defined(WILLOWS))
    HINSTANCE   h;
#endif
#ifdef __WINDOWS_386__
    HINDIR      hIndir;
    DWORD       guiColoursAlias;
#endif

    if( new_rgb == NULL ) {
        return( FALSE );
    }
    memset( &choose, 0, sizeof( CHOOSECOLOR ) );
    choose.Flags = CC_PREVENTFULLOPEN;
    choose.hInstance = GUIMainHInst;
    choose.Flags |= CC_RGBINIT;
    choose.rgbResult = GETRGB( init_rgb );
#ifndef __WINDOWS_386__
    choose.lpCustColors = GUIColours;
#endif
    choose.lStructSize = sizeof( CHOOSECOLOR );

#if defined(__NT__) || defined(WILLOWS)
    func = ChooseColor;
#else
    h = LoadLibrary( "COMMDLG.DLL" );
    if( (UINT)h < 32 ) {
        return( FALSE );
    }
    func = GetProcAddress( h, "ChooseColor" );
#endif
#ifdef __WINDOWS_386__
    hIndir = GetIndirectFunctionHandle( func, INDIR_PTR, INDIR_ENDLIST );
    if( hIndir == NULL ) {
        FreeLibrary( h );
        return( FALSE );
    }
    guiColoursAlias = AllocAlias16( (void *)GUIColours );
    choose.lpCustColors = (void *)guiColoursAlias;
    ret = (bool)InvokeIndirectFunction( hIndir, &choose );
    if( guiColoursAlias != NULL ) {
        FreeAlias16( guiColoursAlias );
    }
#else
    /* was missing WINAPI */
    ret = ((BOOL(WINAPI *)(LPCHOOSECOLOR))func)( &choose );
#endif
#if !(defined(__NT__) || defined(WILLOWS))
    FreeLibrary( h );
#endif

    if( ret ) {
        FillInRGB( choose.rgbResult, new_rgb );
    }
    return( ret );
#else
    init_rgb = init_rgb;
    new_rgb = new_rgb;
    return( FALSE );
#endif
}

/*
 * GUISetColours -- record the colours selected by the application
 */

bool GUIXSetColours( gui_window *wnd, gui_colour_set *colours )
{
    int size;

    if( colours != NULL ) {
        size = sizeof( gui_colour_set ) * wnd->num_attrs;
        wnd->colours = ( gui_colour_set * )GUIMemAlloc( size );
        if( wnd->colours == NULL ) {
            return( FALSE );
        }
        memcpy( wnd->colours, colours, size );
        SetBKBrush( wnd );
        return( TRUE );
    }
    return( FALSE );
}

void GUIXGetWindowColours( gui_window *wnd, gui_colour_set *colours )
{
    memcpy( colours, wnd->colours, sizeof( gui_colour_set ) * wnd->num_attrs );
}

HBRUSH GUIFreeBKBrush( gui_window * wnd )
{
    HBRUSH brush;

    if( wnd->bk_brush != NULLHANDLE ) {
        /* make sure bk_brush is not the currently the background brush
         * and, therefore, a system resource */
#ifndef __OS2_PM__
        if( wnd->hwnd != NULLHANDLE ) {  /* NULL if create failed */
            brush = SET_HBRBACKGROUND( wnd->hwnd, NULL );
        }
#else
        brush = NULLHANDLE;
#endif
        _wpi_deletebrush( wnd->bk_brush );
        wnd->bk_brush = NULLHANDLE;
    }
    return( brush );
}

void GUISetWindowColours( gui_window *wnd, int num_colours,
                          gui_colour_set *colours )
{
    GUIFreeColours( wnd );
    GUIFreeBKBrush( wnd );
    GUISetColours( wnd, num_colours, colours );
    GUIWndDirty( wnd );
}

WPI_COLOUR GUIGetFore( gui_window *wnd, gui_attr attr )
{
    return( GUIColours[wnd->colours[attr].fore] );
}

WPI_COLOUR GUIGetBack( gui_window *wnd, gui_attr attr )
{
    return( GUIColours[wnd->colours[attr].back] );
}
