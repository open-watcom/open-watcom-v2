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
* Description:  Default GUI colour set.
*
****************************************************************************/


#include "guiwind.h"
#include "uiattrs.h"
#include "guicontr.h"
#include "guiwhole.h"
#include "guicolor.h"
#include "guiwnclr.h"
#include "guixdlg.h"
#include "guidraw.h"
#include <string.h>

static gui_colour_set IBMDialColours[] = {
    #define pick(e,f,b)     { f, b },
    #include "attrdlg.h"
    #undef pick
};

#define DialColours IBMDialColours


static gui_colour_set Normal = { NORMAL_FORE, NORMAL_BACK };

#define GETFG( attr ) ( (attr) & 0x0f )  /* low 4 bits */
#define GETBG( attr ) ( (attr) >> 4 )    /* high 4 bits */

#define MAKEATTR( fore, back )  ( _fg( Colours[(fore)] | \
                                  _bg( Colours[(back)] ) ) )
#define MAKEDLGATTR( attr )     MakeAttr( DialColours[attr].fore, \
                                          DialColours[attr].back )

static ATTR Colours[GUI_NUM_COLOURS] = {
    C_BLACK,            /* GUI_BLACK      */
    C_BLUE,             /* GUI_BLUE       */
    C_GREEN,            /* GUI_GREEN      */
    C_CYAN,             /* GUI_CYAN       */
    C_RED,              /* GUI_RED        */
    C_MAGENTA,          /* GUI_MAGENTA    */
    C_BROWN,            /* GUI_BROWN      */
    C_WHITE,            /* GUI_WHITE      */
    C_BLACK   | BRIGHT, /* GUI_GREY       */
    C_BLUE    | BRIGHT, /* GUI_BR_BLUE    */
    C_GREEN   | BRIGHT, /* GUI_BR_GREEN   */
    C_CYAN    | BRIGHT, /* GUI_BR_CYAN    */
    C_RED     | BRIGHT, /* GUI_BR_RED     */
    C_MAGENTA | BRIGHT, /* GUI_BR_MAGENTA */
    C_BROWN   | BRIGHT, /* GUI_BR_YELLOW  */
    C_WHITE   | BRIGHT  /* GUI_BR_WHITE   */
};

static ATTR MakeAttr( gui_colour fore, gui_colour back )
{
    ATTR        attr;

    attr = MAKEATTR( fore, back );
    attr &= 0x7f;
    return( attr );
}

#if !defined( ISQL_COLOURS )
static ATTR mnu_attr_menu = 0;
static ATTR mnu_attr_active = 0;
static ATTR mnu_attr_inactive = 0;
static ATTR mnu_attr_curr_inactive = 0;
static ATTR mnu_attr_hot = 0;
static ATTR mnu_attr_hot_quiet = 0;
static ATTR mnu_attr_curr_active = 0;
static ATTR mnu_attr_hot_curr = 0;
#endif

/*
 * GUIXSetColours -- record the colours wanted
 */

bool GUIXSetColours( gui_window *wnd, int num_attrs, gui_colour_set *colours )
{
    size_t  size;
    int     i;
    ATTR    *attrs;

    size = sizeof( ATTR ) * num_attrs;
    attrs = (ATTR *)GUIMemAlloc( size );
    if( attrs != NULL ) {
        wnd->attrs = attrs;
        wnd->num_attrs = num_attrs;
        for( i = 0; i < num_attrs; i++ ) {
            attrs[i] = MakeAttr( colours[i].fore, colours[i].back );
        }
        if( wnd->vbarmenu != NULL ) {
#if !defined( ISQL_COLOURS )
            mnu_attr_menu           = uisetattr( ATTR_MENU,          WNDATTR( wnd, GUI_MENU_FRAME ) );
            mnu_attr_active         = uisetattr( ATTR_ACTIVE,        WNDATTR( wnd, GUI_MENU_PLAIN ) );
            mnu_attr_inactive       = uisetattr( ATTR_INACTIVE,      WNDATTR( wnd, GUI_MENU_GRAYED ) );
            mnu_attr_curr_inactive  = uisetattr( ATTR_CURR_INACTIVE, WNDATTR( wnd, GUI_MENU_GRAYED_ACTIVE ) );
            mnu_attr_hot            = uisetattr( ATTR_HOT,           WNDATTR( wnd, GUI_MENU_STANDOUT ) );
            mnu_attr_hot_quiet      = uisetattr( ATTR_HOT_QUIET,     WNDATTR( wnd, GUI_MENU_STANDOUT ) );
            mnu_attr_curr_active    = uisetattr( ATTR_CURR_ACTIVE,   WNDATTR( wnd, GUI_MENU_ACTIVE ) );
            mnu_attr_hot_curr       = uisetattr( ATTR_HOT_CURR,      WNDATTR( wnd, GUI_MENU_ACTIVE_STANDOUT ) );
#endif
            uimenutitlebar();
        }
        return( true );
    }
    return( false );
}

gui_colour GUIGetBack( ATTR attr )
{
    return( GETBG( attr ) );
}

gui_colour GUIGetFore( ATTR attr )
{
    return( GETFG( attr ) );
}

ATTR GUIMakeColour( gui_colour fore, gui_colour back )
{
    return( MakeAttr( fore, back )  );
}

void GUIAPI GUISetWindowColours( gui_window *wnd, int num_attrs, gui_colour_set *colours )
{
    gui_control *control;

    GUIFreeColours( wnd );
    GUISetColours( wnd, num_attrs, colours );
    for( control = wnd->controls; control != NULL; control = control->sibling ) {
        GUIRefreshControl( control->parent, control->id );
    }
    GUIDirtyWhole( wnd );
}

static bool ColoursSet = false;

static ATTR dlg_attr_normal = 0;
#if !defined( ISQL_COLOURS )
static ATTR dlg_attr_dial_frame = 0;
static ATTR dlg_attr_shadow = 0;
static ATTR dlg_attr_scroll_icon = 0;
static ATTR dlg_attr_scroll_bar = 0;
static ATTR dlg_attr_hotspot = 0;
static ATTR dlg_attr_default_hotspot = 0;
static ATTR dlg_attr_curr_hotspot = 0;
static ATTR dlg_attr_curr_hotspot_key = 0;
#endif

bool GUISetDialColours( void )
{
    if( ColoursSet )  {
        return( false );
    }
    ColoursSet = true;
    dlg_attr_normal             = uisetattr( ATTR_NORMAL,           MAKEDLGATTR( GUI_DLG_NORMAL ) );
#if !defined( ISQL_COLOURS )
    dlg_attr_dial_frame         = uisetattr( ATTR_DIAL_FRAME,       MAKEDLGATTR( GUI_DLG_FRAME ) );
    dlg_attr_shadow             = uisetattr( ATTR_SHADOW,           MAKEDLGATTR( GUI_DLG_SHADOW ) );
    dlg_attr_scroll_icon        = uisetattr( ATTR_SCROLL_ICON,      MAKEDLGATTR( GUI_DLG_SCROLL_ICON ) );
    dlg_attr_scroll_bar         = uisetattr( ATTR_SCROLL_BAR,       MAKEDLGATTR( GUI_DLG_SCROLL_BAR ) );
    dlg_attr_hotspot            = uisetattr( ATTR_HOTSPOT,          MAKEDLGATTR( GUI_DLG_BUTTON_PLAIN ) );
    dlg_attr_default_hotspot    = uisetattr( ATTR_DEFAULT_HOTSPOT,  MAKEDLGATTR( GUI_DLG_BUTTON_STANDOUT ) );
    dlg_attr_curr_hotspot       = uisetattr( ATTR_CURR_HOTSPOT,     MAKEDLGATTR( GUI_DLG_BUTTON_ACTIVE ) );
    dlg_attr_curr_hotspot_key   = uisetattr( ATTR_CURR_HOTSPOT_KEY, MAKEDLGATTR( GUI_DLG_BUTTON_ACTIVE_STANDOUT ) );
#endif
    SliderChar[0] = DRAWC1( DIAL_SCROLL_SLIDER );
    UpPoint[0]    = DRAWC1( DIAL_UP_SCROLL_ARROW );
    DownPoint[0]  = DRAWC1( DIAL_DOWN_SCROLL_ARROW );
    return( true );
}

void GUIResetDialColours( void )
{
//    UIData->attrs[ATTR_NORMAL] = MakeAttr( Normal.fore, Normal.back );
    uisetattr( ATTR_NORMAL,             dlg_attr_normal );
#if !defined( ISQL_COLOURS )
    uisetattr( ATTR_DIAL_FRAME,         dlg_attr_dial_frame );
    uisetattr( ATTR_SHADOW,             dlg_attr_shadow );
    uisetattr( ATTR_SCROLL_ICON,        dlg_attr_scroll_icon );
    uisetattr( ATTR_SCROLL_BAR,         dlg_attr_scroll_bar );
    uisetattr( ATTR_HOTSPOT,            dlg_attr_hotspot );
    uisetattr( ATTR_DEFAULT_HOTSPOT,    dlg_attr_default_hotspot );
    uisetattr( ATTR_CURR_HOTSPOT,       dlg_attr_curr_hotspot );
    uisetattr( ATTR_CURR_HOTSPOT_KEY,   dlg_attr_curr_hotspot_key );
#endif
    ColoursSet = false;
}

void GUIAPI GUISetBackgroundColour( gui_colour_set *colour )
{
    Normal.back = colour->back;
    Normal.fore = colour->fore;
}

void GUIAPI GUIGetDialogColours( gui_colour_set *colours )
{
    memcpy( colours, DialColours, GUI_DLG_NUM_ATTRS * sizeof( gui_colour_set ) );
}

void GUIAPI GUISetDialogColours( gui_colour_set *colours )
{
    memcpy( DialColours, colours, GUI_DLG_NUM_ATTRS * sizeof( gui_colour_set ) );
    GUIResetDialColours();
}

static gui_colour GetColour( ATTR attr )
{
    gui_colour i;

    for( i = 0; i < GUI_NUM_COLOURS; i++ ) {
        if( Colours[i] == attr ) {
            return( i );
        }
    }
    return( GUI_BLACK );
}

void GUIXGetWindowColours( gui_window *wnd, gui_colour_set *colours )
{
    int i;

    for( i = 0; i < wnd->num_attrs; i++ ) {
        colours[i].fore = GetColour( GETFG( WNDATTR( wnd, i ) ) );
        colours[i].back = GetColour( GETBG( WNDATTR( wnd, i ) ) );
    }
}

bool GUIAPI GUISetRGB( gui_colour colour, gui_rgb rgb )
{
    /* unused parameters */ (void)colour; (void)rgb;

    return( false );
}

bool GUIAPI GUIGetRGB( gui_colour colour, gui_rgb *rgb )
{
    /* unused parameters */ (void)colour; (void)rgb;

    return( false );
}

bool GUIAPI GUIGetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set )
{
    if( colour_set == NULL ) {
        return( false );
    }
    if( attr < wnd->num_attrs ) {
        colour_set->fore = GETFG( WNDATTR( wnd, attr ) );
        colour_set->back = GETBG( WNDATTR( wnd, attr ) );
        return( true );
    }
    return( false );
}

bool GUIAPI GUISetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set )
{
    if( colour_set == NULL ) {
        return( false );
    }
    if( attr < wnd->num_attrs ) {
        WNDATTR( wnd, attr ) = MakeAttr( colour_set->fore, colour_set->back );
        return( true );
    }
    return( false );
}

bool GUIAPI GUIGetRGBFromUser( gui_rgb rgb, gui_rgb *new_rgb )
{
    /* unused parameters */ (void)rgb; (void)new_rgb;

    return( false );
}
