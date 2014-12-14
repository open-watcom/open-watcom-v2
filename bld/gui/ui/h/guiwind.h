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


#ifndef _GUIWIND_H_
#define _GUIWIND_H_

#include "stdui.h"

#include "guitypes.h"
#include "gui.h"
#include "uivedit.h"
#include "uiledit.h"
#include "uidialog.h"
#include "uimenu.h"
#include "guimin.h"
#include "guihint.h"

#define GUI_IS_GUI      false

#define GUI_WINDOW      0x1000

#define BORDER_AMOUNT           2
#define HALF_BORDER_AMOUNT      1
#define MENU_AMOUNT             2

#define CLOSER_COL              2
#define CLOSER_WIDTH            3
#define MAXOFFSET               3
#define MINOFFSET               6

#define MINMAX_WIDTH    ( MIN_WIDTH + MINOFFSET )

#define MIN_GADGET_SIZE         2

typedef enum {
        NONE                            = 0x0000,
        TITLE_INVALID                   = 0x0001,
        FRAME_INVALID                   = 0x0002,
        CONTENTS_INVALID                = 0x0004,
        HSCROLL_INVALID                 = 0x0008,
        VSCROLL_INVALID                 = 0x0010,
        MAXIMIZED                       = 0x0020,
        MINIMIZED                       = 0x0040,
        SETHRANGE                       = 0x0080,
        SETVRANGE                       = 0x0100,
        DONT_SEND_PAINT                 = 0x0200,
        NEEDS_RESIZE_REDRAW             = 0x0400,
        DIALOG                          = 0x0800,
        IS_ROOT                         = 0x1000,
        CHECK_CHILDREN_ON_RESIZE        = 0x2000,
        WHOLE_WND_INVALID       = TITLE_INVALID | FRAME_INVALID \
                                | CONTENTS_INVALID | HSCROLL_INVALID | VSCROLL_INVALID,
        NON_CLIENT_INVALID      = FRAME_INVALID | TITLE_INVALID | \
                                  VSCROLL_INVALID | HSCROLL_INVALID
} gui_flags;

#define GUI_HRANGE_SET( wnd ) ( ( wnd->hgadget != NULL ) && ( ( wnd->flags & SETHRANGE ) != 0 ) )
#define GUI_VRANGE_SET( wnd ) ( ( wnd->vgadget != NULL ) && ( ( wnd->flags & SETVRANGE ) != 0 ) )
#define GUI_HSCROLL_EVENTS_SET( wnd ) ( ( wnd->hgadget != NULL ) && ( ( wnd->style & GUI_HSCROLL_EVENTS ) != 0 ) )
#define GUI_VSCROLL_EVENTS_SET( wnd ) ( ( wnd->vgadget != NULL ) && ( ( wnd->style & GUI_VSCROLL_EVENTS ) != 0 ) )
#define GUI_WND_MINIMIZED( wnd ) ( wnd->flags & MINIMIZED )
#define GUI_WND_MAXIMIZED( wnd ) ( wnd->flags & MAXIMIZED )
#define GUI_WND_VISIBLE( wnd )   ( wnd->style & GUI_VISIBLE )
#define GUI_HAS_CLOSER( wnd ) ( ( wnd->style & GUI_CLOSEABLE ) || ( wnd->menu != NULL ) )
#define GUI_RESIZE_GADGETS_USEABLE( wnd ) ( wnd->screen.area.width >= MINMAX_WIDTH )
#define GUI_IS_DIALOG( wnd ) ( ( wnd->flags & DIALOG ) != 0 )

#define COPYAREA( old, new ) { (new).row = (old).row; (new).height = (old).height; \
                               (new).col = (old).col; (new).width = (old).width; }

#define EMPTY_AREA( sarea ) ( ( (sarea).width == 0 ) || ( (sarea).height == 0 ) )

typedef struct gui_control gui_control;

typedef struct toolbarinfo {
    bool                fixed;          // true if toolbar is fixed, false if floating)
    int                 num_items;      // number of items in toolb bar
    gui_window          *floattoolbar;  // NULL if fixed
    gui_toolbar_struct  *info;          // initialization information
    bool                switching;      // set if between fixed and floating
    bool                excl;           // true if exclamation marks used for fixed
    bool                has_colours;    // true if plain and standout colour
    gui_colour_set      plain;          // given, FLASE if defaults are to
    gui_colour_set      standout;       // be used.
} toolbarinfo;

typedef struct statusinfo {
    SAREA       area;           // (relative) location of status window
    char        *text;          // text in status window
    ATTR        attr;           // colour to use to draw status text
} statusinfo;

struct gui_window {
    VSCREEN             screen; /* must be first field - see call to uivopen */
    gui_create_styles   style;          // style window was created with
    int                 num_attrs;      // number of colours
    ATTR                *colours;       // colours for window
    SAREA               use;            // area inside the frame
    SAREA               prev_area;      // location for restore after max/min
    SAREA               dirty;          // area that is dirty
    gui_flags           flags;          // flags to keep state
    GUICALLBACK         *call_back;     // app's callback routine
    gui_control         *controls;      // list of controls in window
    void                *extra;         // extra pointer for app to use
    gui_window          *child;         // pointer to first child window
    gui_window          *sibling;       // pointer to next sibling window
    gui_window          *parent;        // pointer to parent window
    gui_window          *next;          // Used by guizlist for z-order
    p_gadget            vgadget;        // vertical scroll gadget
    p_gadget            hgadget;        // horizontal scroll gadget
    MENUITEM            *menu;          // pulldown menu under closer
    VBARMENU            *vbarmenu;      // top level menu
    int                 min_pos;        // unused
    toolbarinfo         *toolbar;       // pointer to toolbar, if exists
    statusinfo          *status;        // pointer to status window, if exists
    hintinfo            hint;           // hint text
    char                background;     // character to use to draw background
    char                *icon_name;     // string to draw on icon
};

#define YMAX UIData->height
#define XMAX UIData->width
#define YMIN uimenuheight()
#define XMIN 0

enum {
    EV_SCROLL_UP = EV_FIRST_UNUSED,
    EV_SCROLL_DOWN,
    EV_SCROLL_LEFT,
    EV_SCROLL_RIGHT,
    EV_PAGE_LEFT,
    EV_PAGE_RIGHT,
    EV_DESTROY
};

#include "guix.h"
#include "guixmdi.h"

#define NUM_GUI_EVENTS 100

#define GUI_FIRST_USER_EVENT ( EV_FIRST_UNUSED + NUM_GUI_EVENTS )

#define LAST_EVENT ( GUI_FIRST_USER_EVENT + GUI_LAST_MENU_ID )
#define FIRST_GUI_EVENT ( LAST_EVENT + 1 )
#define LAST_GUI_EVENT ( GUI_FIRST_USER_EVENT + GUI_MDI_MENU_LAST )

enum {
  GUI_MENU_RESTORE      = EV_FIRST_UNUSED,
  GUI_FIRST_SYS_MENU    = GUI_MENU_RESTORE,
  GUI_MENU_MOVE,
  GUI_MENU_SIZE,
  GUI_MENU_MINIMIZE,
  GUI_MENU_MAXIMIZE,
  GUI_MENU_CLOSE,
  GUI_LAST_SYS_MENU     = GUI_MENU_CLOSE
};

#endif // _GUIWIND_H_
