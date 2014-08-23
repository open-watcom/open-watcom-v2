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
* Description:  Default GUI colour set.
*
****************************************************************************/


#include "guiwind.h"
#include "uiattrs.h"
#include "guicontr.h"
#include "guiwhole.h"
#include "guicolor.h"
#include "guiwnclr.h"
#include "guigmous.h"
#include "guixdlg.h"
#include <string.h>

static gui_colour_set IBMDialColours[] = {
    { GUI_WHITE, GUI_BLUE },            // GUI_DLG_NORMAL,
    { GUI_BLUE, GUI_CYAN },             // GUI_DLG_FRAME,
    { GUI_BLACK, GUI_BLUE },            // GUI_DLG_SHADOW,
    { GUI_WHITE, GUI_BLUE },            // GUI_DLG_SCROLL_ICON,
    { GUI_WHITE, GUI_BLUE },            // GUI_DLG_SCROLL_BAR,
    /* hot spot is inverted on purpose */
    { GUI_BLUE, GUI_WHITE },            // GUI_DLG_BUTTON_PLAIN,
    { GUI_BRIGHT_WHITE, GUI_WHITE },    // GUI_DLG_BUTTON_STANDOUT,
    { GUI_BLUE, GUI_CYAN },             // GUI_DLG_BUTTON_ACTIVE,
    { GUI_BRIGHT_WHITE, GUI_CYAN },     // GUI_DLG_BUTTON_ACTIVE_STANDOUT
};

#define DialColours IBMDialColours


static gui_colour_set Normal = { NORMAL_FORE, NORMAL_BACK };

#define GETFG( attr ) ( (attr) & 0x0f )  /* low 4 bits */
#define GETBG( attr ) ( (attr) >> 4 )    /* high 4 bits */

#define MAKEATTR( fore, back ) ( _fg( Colours[(fore)] | \
                                 _bg( Colours[(back)] ) ) )
#define MAKEDLGATTR( attr ) MakeAttr( DialColours[attr].fore, \
                                      DialColours[attr].back )

static ATTR Colours[GUI_NUM_COLOURS] = {
    C_BLACK,            /* GUI_BLACK          */
    C_BLUE,             /* GUI_BLUE           */
    C_GREEN,            /* GUI_GREEN          */
    C_CYAN,             /* GUI_CYAN           */
    C_RED,              /* GUI_RED            */
    C_MAGENTA,          /* GUI_MAGENTA        */
    C_BROWN,            /* GUI_BROWN          */
    C_WHITE,            /* GUI_WHITE          */
    C_BLACK   | BRIGHT, /* GUI_GREY           */
    C_BLUE    | BRIGHT, /* GUI_BRIGHT_BLUE    */
    C_GREEN   | BRIGHT, /* GUI_BRIGHT_GREEN   */
    C_CYAN    | BRIGHT, /* GUI_BRIGHT_CYAN    */
    C_RED     | BRIGHT, /* GUI_BRIGHT_RED     */
    C_MAGENTA | BRIGHT, /* GUI_BRIGHT_MAGENTA */
    C_BROWN   | BRIGHT, /* GUI_BRIGHT_YELLOW  */
    C_WHITE   | BRIGHT  /* GUI_BRIGHT_WHITE   */
};

static ATTR MakeAttr( gui_colour fore, gui_colour back )
{
    ATTR        attr;

    attr = MAKEATTR( fore, back );
    attr &= 0x7f;
    return( attr );
}

/*
 * GUIXSetColours -- record the colours wanted
 */

bool GUIXSetColours( gui_window *wnd, gui_colour_set *colours )
{
    int size;
    int i;

    size = sizeof( ATTR ) * wnd->num_attrs;
    wnd->colours = ( ATTR * )GUIMemAlloc( size );
    if( wnd->colours == NULL ) {
        wnd->num_attrs = 0;
        return( false );
    }
    for( i = 0; i < wnd->num_attrs; i++ ) {
        wnd->colours[i] = MakeAttr( colours[i].fore, colours[i].back );
    }
    if( wnd->vbarmenu != NULL ) {
#if !defined( ISQL_COLOURS )
        UIData->attrs[ATTR_MENU] = wnd->colours[GUI_MENU_FRAME];
        UIData->attrs[ATTR_ACTIVE] = wnd->colours[GUI_MENU_PLAIN];
        UIData->attrs[ATTR_INACTIVE] = wnd->colours[GUI_MENU_GRAYED];
        UIData->attrs[ATTR_CURR_INACTIVE] = wnd->colours[GUI_MENU_GRAYED_ACTIVE];
        UIData->attrs[ATTR_HOT] = wnd->colours[GUI_MENU_STANDOUT];
        UIData->attrs[ATTR_HOT_QUIET] = wnd->colours[GUI_MENU_STANDOUT];
        UIData->attrs[ATTR_CURR_ACTIVE] = wnd->colours[GUI_MENU_ACTIVE];
        UIData->attrs[ATTR_HOT_CURR] = wnd->colours[GUI_MENU_ACTIVE_STANDOUT];
#endif
        uimenutitlebar();
    }
    return( true );
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

void GUISetWindowColours( gui_window *wnd, int num_attrs,
                          gui_colour_set *colours )
{
    gui_control *control;

    GUIFreeColours( wnd );
    GUISetColours( wnd, num_attrs, colours );
    for( control = wnd->controls; control != NULL; control = control->sibling ) {
        GUIRefreshControl( control->parent, control->id );
    }
    GUIWholeWndDirty( wnd );
}

static bool ColoursSet = false;

bool GUISetDialColours( void )
{
    if( ColoursSet )  {
        return( false );
    }
    ColoursSet = true;
    UIData->attrs[ATTR_NORMAL] = MAKEDLGATTR( GUI_DLG_NORMAL );
#if !defined( ISQL_COLOURS )
    UIData->attrs[ATTR_DIAL_FRAME] = MAKEDLGATTR( GUI_DLG_FRAME );
    UIData->attrs[ATTR_SHADOW] = MAKEDLGATTR( GUI_DLG_SHADOW );
    UIData->attrs[ATTR_SCROLL_ICON] = MAKEDLGATTR( GUI_DLG_SCROLL_ICON );
    UIData->attrs[ATTR_SCROLL_BAR] = MAKEDLGATTR( GUI_DLG_SCROLL_BAR );
    UIData->attrs[ATTR_HOTSPOT] = MAKEDLGATTR( GUI_DLG_BUTTON_PLAIN );
    UIData->attrs[ATTR_DEFAULT_HOTSPOT] = MAKEDLGATTR( GUI_DLG_BUTTON_STANDOUT );
    UIData->attrs[ATTR_CURR_HOTSPOT] = MAKEDLGATTR( GUI_DLG_BUTTON_ACTIVE );
    UIData->attrs[ATTR_CURR_HOTSPOT_KEY] = MAKEDLGATTR( GUI_DLG_BUTTON_ACTIVE_STANDOUT );
#endif
    SliderChar[0] = GUIGetCharacter( GUI_DIAL_SCROLL_SLIDER );
    UpPoint[0] = GUIGetCharacter( GUI_DIAL_UP_SCROLL_ARROW );
    DownPoint[0] =  GUIGetCharacter( GUI_DIAL_DOWN_SCROLL_ARROW );
    return( true );
}

void GUIResetDialColours( void )
{
    UIData->attrs[ATTR_NORMAL] = MakeAttr( Normal.fore, Normal.back );
    ColoursSet = false;
}

void GUISetBackgroundColour( gui_colour_set *colour )
{
    Normal.back = colour->back;
    Normal.fore = colour->fore;
}

void GUIGetDialogColours( gui_colour_set *colours )
{
    memcpy( colours, DialColours, GUI_DLG_NUM_ATTRS*sizeof( gui_colour_set ) );
}

void GUISetDialogColours( gui_colour_set *colours )
{
    memcpy( DialColours, colours, GUI_DLG_NUM_ATTRS*sizeof( gui_colour_set ) );
    GUIResetDialColours();
}

gui_colour GetColour( ATTR attr )
{
    gui_colour i;

    for( i = GUI_FIRST_COLOUR; i < GUI_NUM_COLOURS; i++ ) {
        if( Colours[i] == attr ) {
            return( i );
        }
    }
    return( GUI_BLACK );
}

void GUIXGetWindowColours( gui_window * wnd, gui_colour_set * colours )
{
    int i;

    for( i = 0; i < wnd->num_attrs; i++ ) {
        colours[i].fore = GetColour( GETFG( wnd->colours[i] ) );
        colours[i].back = GetColour( GETBG( wnd->colours[i] ) );
    }
}

bool GUISetRGB( gui_colour colour, gui_rgb rgb )
{
    colour = colour;
    rgb = rgb;
    return( false );
}

bool GUIGetRGB( gui_colour colour, gui_rgb *rgb )
{
    colour = colour;
    rgb = rgb;
    return( false );
}

bool GUIGetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set )
{
    if( colour_set == NULL ) {
        return( false );
    }
    if( attr < wnd->num_attrs ) {
        colour_set->fore = GUIGetFore( wnd->colours[attr] );
        colour_set->back = GUIGetBack( wnd->colours[attr] );
        return( true );
    }
    return( false );
}

bool GUISetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set )
{
    if( colour_set == NULL ) {
        return( false );
    }
    if( attr < wnd->num_attrs ) {
        wnd->colours[attr] = MakeAttr( colour_set->fore, colour_set->back );
        return( true );
    }
    return( false );
}

bool GUIGetRGBFromUser( gui_rgb rgb, gui_rgb *new_rgb )
{
    rgb = rgb;
    new_rgb = new_rgb;
    return( false );
}
