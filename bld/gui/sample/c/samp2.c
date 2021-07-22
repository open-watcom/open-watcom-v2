/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "gui.h"

#define HEIGHT 5
#define WIDTH 8

#define DLGNEW_CTLS() \
    pick_p5id( STATIC,  GUI_STATIC,         "Please enter install path:", 4, 4, 30 ) \
    pick_p5id( EDIT,    GUI_EDIT,           NULL,                         4, 15, 30 ) \
    pick_p5id( CANCEL,  GUI_PUSH_BUTTON,   "CANCEL",                      6, 30, WIDTH ) \
    pick_p5id( OK,      GUI_DEFPUSH_BUTTON, "OK",                         26, 30, WIDTH )

enum {
    DUMMY_ID = 100,
    #define pick_p5id(id,p1,p2,p3,p4,p5)    CTL_ ## id,
    DLGNEW_CTLS()
    #undef pick_p5id
};

enum {
    #define pick_p5id(id,p1,p2,p3,p4,p5)    id ## _IDX,
    DLGNEW_CTLS()
    #undef pick_p5id
};

static gui_control_info GetNew[] = {
    #define pick_p5id(id,p1,p2,p3,p4,p5) p1,p2,p3,p4,p5, HEIGHT, NULL, GUI_NOSCROLL, GUI_STYLE_CONTROL_NOSTYLE, CTL_ ## id,
    DLGNEW_CTLS()
    #undef pick_p5id
};

static gui_rect Scale = { 0, 0, 80, 100 };
static gui_ord Width = 0;

#define NUM_TEXT 5

static char Text[][NUM_TEXT] = { {"0%"}, {"25%"}, {"50%"}, {"75%"}, {"100%"} };
static gui_ord Strlen[NUM_TEXT] = { 2, 3, 3, 3, 4 };

static GUICALLBACK GetNewGUIEventProc;
static GUICALLBACK StatusGUIEventProc;

static gui_create_info DialogWnd = {
    "Install Program: ",
    { 20, 20, 40, 40 },
    GUI_NOSCROLL,
    GUI_VISIBLE,
    NULL,
    GUI_NO_MENU,                    // Menu array
    GUI_NO_COLOUR,                  // Colour attribute array
    &GetNewGUIEventProc,            // GUI Event Callback function
    NULL,                           // Extra
    NULL,                           // Icon
    NULL                            // Menu Resource
};

static gui_rect Rect;
static gui_text_ord Row;
static int NumEnters = 0;

static gui_colour_set StatusColours[GUI_NUM_ATTRS + 1] = {
    /* Fore          Back        */
    GUI_BLUE,      GUI_WHITE,   /* GUI_MENU_PLAIN     */
    GUI_BLUE,      GUI_WHITE,   /* GUI_MENU_STANDOUT  */
    GUI_BLUE,      GUI_WHITE,   /* GUI_BACKGROUND     */
    GUI_BLUE,      GUI_WHITE,   /* GUI_TITLE_ACTIVE   */
    GUI_GREY,      GUI_WHITE,   /* GUI_TITLE_INACTIVE */
    GUI_BLUE,      GUI_WHITE,   /* GUI_FRAME_ACTIVE   */
    GUI_GREY,      GUI_WHITE,   /* GUI_FRAME_INACTIVE */
    GUI_BR_WHITE,  GUI_MAGENTA  /* GUI_FIRST_UNUSED   */
};

static gui_create_info StatusWnd = {
    "Percent of Installation Complete",
    { 19, 70, 42, 20 },
    GUI_NOSCROLL,
    GUI_VISIBLE | GUI_DIALOG_LOOK,
    NULL,
    GUI_NO_MENU,                            // Menu array
    { GUI_NUM_ATTRS + 1, StatusColours },   // Colour attribute array
    &StatusGUIEventProc,                    // GUI Event Callback function
    NULL,                                   // Extra
    NULL,                                   // Icon
    NULL                                    // Menu Resource
};

static gui_colour_set Colours[GUI_NUM_INIT_COLOURS] = {
    { GUI_BR_WHITE, GUI_BLUE },
    { GUI_BR_WHITE, GUI_BLUE }
};

static char * text = NULL;
static gui_message_return ret_val = GUI_RET_CANCEL;

static gui_window * Status = NULL;

/*
 * GetNewGUIEventProc - call back routine for the GetNewVal dialog
 */

static bool GetNewGUIEventProc( gui_window *wnd, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        ret_val = GUI_RET_CANCEL;
        return( true );
    case GUI_DESTROY:
        if( Status != NULL ) {
            GUIDestroyWnd( Status );
        }
        return( true );
    case GUI_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_CANCEL:
            GUICloseDialog( wnd );
            ret_val = GUI_RET_CANCEL;
            break;
        case CTL_OK:
            text = GUIGetText( wnd, CTL_EDIT );
            if( Status == NULL ) {
                Status = GUICreateWindow( &StatusWnd );
            } else {
                NumEnters++;
                Rect.width = ( NumEnters * Width ) / 4;
                if( NumEnters > 4 ) {
                    GUICloseDialog( wnd );
                } else {
                    GUIWndDirty( Status );
                }
            }
            ret_val = GUI_RET_OK;
            break;
        }
        return( true );
    }
    return( false );
}

/*
 * StatusGUIEventProc - call back routine for the status window
 */

static bool StatusGUIEventProc( gui_window * wnd, gui_event gui_ev, void * param )
{
    int              i;
    gui_ord          pos;
    gui_text_metrics metrics;

    param = param;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        Row = GUIGetNumRows( wnd ) / 2;
        GUIGetTextMetrics( wnd, &metrics );
        GUIGetClientRect( wnd, &Rect );
#if 1
        Rect.x = 1;
        Rect.y = 1;
        Width = Rect.width - 2 * Rect.x;
#endif
#if 0
        Rect.height -= Rect.y;
        Rect.x = 0;
        Rect.y = 0;
#endif
        Rect.width = 0;
        for( i = 0; i < NUM_TEXT; i++ ) {
            Strlen[i] *= metrics.max.x;
        }
        return( true );
    case GUI_DESTROY:
        return( true );
    case GUI_PAINT:
        GUIDrawRect( wnd, &Rect, GUI_FIRST_UNUSED );
        for( i = 0; i < NUM_TEXT; i++ ) {
            pos = ( i * Width / 4 ) - Strlen[i] + Rect.x;
            if( pos < Rect.x ) {
                pos = Rect.x;
            }
            if( ( i > NumEnters ) || ( i == 0 ) && ( NumEnters == 0 ) ) {
                GUIDrawText( wnd, &Text[i], Strlen[i], Row, pos, GUI_TITLE_ACTIVE );
            } else {
                GUIDrawText( wnd, &Text[i], Strlen[i], Row, pos, GUI_FIRST_UNUSED );
            }
        }
        return( true );
    }
    return( false );
}

void GUIAPI GUImain( void )
{
    GUIWndInit( 250 /* ms */, GUI_GMOUSE );
    GUISetScale( &Scale );
    GUISetColour( &Colours );
    GUICreateDialog( &DialogWnd, NUM_CONTROLS, &GetNew );
}
