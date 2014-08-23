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
* Description:  GUI sample program local constants and declarations.
*
****************************************************************************/


#define keys                    1
#define controls_on_main        0
#define parent_keys             1
#define default_colours         1
#define combo                   0
#define hot_spots               0
#define swap_enable             0
#define test_colours            0
#define controls_on_parent      0
#define two_check_boxes         0
#define mdi                     1
#define controls_on_child3      1
#define dynamic_menus           1

#ifndef NULL
#define NULL            0
#endif

#include "gui.h"
#include "guikey.h"

#define DLG_CHAR_WIDTH  20
#define DLG_CHAR_HEIGHT 35

#define BUTTON_WIDTH  125
#define BUTTON_HEIGHT 45

#define AUTOMATIC GUI_AUTOMATIC

#define WIDTH 1000
#define HEIGHT 1100

typedef struct attr_entry {
    unsigned            start;
    unsigned            end;
    gui_attr            attr;
    struct attr_entry * next;
} attr_entry;

typedef struct {
    attr_entry *        attr_list;
    char *              data;
    gui_ord             indent; /* in the user's coodinate system */
} disp_entry;

typedef struct {
    int          numrows;
    gui_ord      start_row;
    disp_entry * display;
} out_info;

extern  GUICALLBACK     DialogEventWnd;
extern  GUICALLBACK     MainEventWnd;
extern  GUICALLBACK     ControlEventWnd;
extern  GUICALLBACK     ChildEventWnd;
extern  GUICALLBACK     Child1EventWnd;
extern  GUICALLBACK     Child3EventWnd;

enum {
    MENU_STATIC_DIALOG,
    MENU_DYNAMIC_DIALOG,
    MENU_TEST_DIALOG,
    MENU_RES_DIALOG,
    MENU_GETNEWVAL,
    MENU_SEP_QUIT,
    MENU_SPAWN,
    MENU_EXIT,
    MENU_QUIT,
    MENU_QUIT2,
    MENU_REDRAW,
    MENU_RESIZE,
    MENU_CHANGE,
    MENU_CASCADE,
    MENU_REPLACE_COLOURS,
    MENU_SHOW_WINDOW,
    MENU_BRING_MAIN_TO_FRONT,
    MENU_GET_CLIENT,
    MENU_GET_RECT,
    MENU_GET_ABS_RECT,
    MENU_MORE,
    MENU_THIS_IS_MORE,
    MENU_MINIMIZE,
    MENU_MAXIMIZE,
    MENU_RESTORE,
    MENU_CANT_GET_ME,
    MENU_ME_EITHER,
    MENU_FIXED_TOOLBAR,
    MENU_FLOATING_TOOLBAR,
    MENU_CHANGE_TOOLBAR,
    MENU_CLOSE_TOOLBAR,
    MENU_STATUS_WND,
    MENU_HELP,
    MENU_CASCADE1,
    MENU_SWITCH_FONT,
    MENU_RESIZE_STATUS,
    MENU_FLIP_MOUSE_ON,
    MENU_FLIP_MOUSE_OFF,
    MENU_SET_MENU,
    MENU_TEST_RESIZE,
    MENU_RESET_MENU,
    MENU_DIALOGS,
    MENU_DO_REDRAW,
    MENU_SIZE,
    MENU_TOOLBAR,
    MENU_WINDOW,
    MENU_OTHER,
    MENU_LABEL3,
    MENU_MODIFY_COLOUR,
    MENU_NEW,
    MENU_HELP_PULLDOWN,
    MENU_HELP_CONTENTS,
    MENU_HELP_ON_HELP,
    MENU_HELP_SEARCH,
    MENU_HELP_CONTEXT,
    MENU_HELP_KEY,
    EDITWINDOW_CONTROL,
    EDITWINDOW_CONTROL2,
    COMBOBOX_CONTROL,
    COMBOOKAY_CONTROL
};

typedef struct {
    int         num;
    char        *string;
    int         length;
    gui_window  *parent;
    unsigned    edit_contr;
    gui_window  *wnd_to_update;
} change_struct;


#define NUM_CHILD1_ROWS         13
#define NUM_TOOLBAR_BUTTONS     5
#define NUM_HOT_SPOTS           5
#define NUM_CHILD2_ROWS         7
#define NUM_POPUP_MENUS         9
#define NUM_NEW_MAIN_MENUS      1
#define NUM_MAIN_MENUS          7
#define NUM_CHILD_MENUS         1

extern  gui_control_info        okbutton;
extern  gui_control_info        Controls[];
extern  gui_control_info        ComboBoxControl;
extern  gui_control_info        ComboBoxControl2;
extern  gui_control_info        ListBoxControl;
extern  gui_control_info        ComboOkayControl;
extern  char                    *ListBoxData[];
extern  gui_create_info         Parent;
extern  gui_create_info         Child1;
extern  gui_create_info         Child2;
extern  gui_create_info         Child3;
extern  gui_create_info         Child4;
extern  gui_create_info         DialogWndControl;
extern  gui_toolbar_struct      ToolBar[];
extern  gui_menu_struct         PopupMenu[];
extern  gui_resource            HotSpots[];
extern  disp_entry              IndentData[];
extern  disp_entry              Child2Data[];
extern  gui_colour_set          ToolPlain;
extern  gui_colour_set          ToolStandout;
#if !default_colours
extern  gui_colour_set          ParentColours[];
extern  gui_colour_set          BackgroundColours;
extern  gui_colour_set          DialColours;
extern  gui_colour_set          DialFrameColours;
#endif
extern  gui_menu_struct         ModifyColour;
extern  gui_menu_struct         NewMainMenu;
extern  gui_menu_struct         MenuMore;
extern  gui_menu_struct         MainMenu[];
extern  gui_menu_struct         ChildMenu[];

extern void SetWidthHeight( gui_rect *rect, bool has_parent );

extern  char            *OldValue;
extern  gui_window      *DialogWindow;

extern void * GUIAlloc( unsigned size );
extern void GUIFree( void *chunk );
