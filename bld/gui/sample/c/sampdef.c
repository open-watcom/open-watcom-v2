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


#include "sample.h"
#include "dlgstat.h"
#include "samprc.h"

#if hot_spots
gui_resource HotSpots[NUM_HOT_SPOTS] = {
    { HOT1, "\x2\x3"      },
    { HOT2, "HOT"         },
    { HOT3, "\x4\x5\x6"   },
    { HOT4, "\x5"         },
    { HOT5, "HOTTER"      }
};
#endif

#if !default_colours

gui_colour_set BackgroundColours = {
    GUI_BLACK, GUI_BLACK   /* GUI_BACKGROUND_COLOUR */
};

gui_colour_set DialColours = {
    GUI_WHITE, GUI_BLUE     /* GUI_DIALOG_COLOUR     */
};

gui_colour_set DialFrameColours = {
    GUI_WHITE, GUI_BLUE     /* GUI_DIALOG_COLOUR     */
};
#endif

static gui_menu_struct MainSubMenu1[] = {
    { "&Static Dialog",         MENU_STATIC_DIALOG,  GUI_MENU_CHECKED,  "Create Static Dialog" },
    { "&Dynamic Dialog",        MENU_DYNAMIC_DIALOG, GUI_ENABLED,       "Create Dynamic Dialog" },
    { "&Test Dialog",           MENU_TEST_DIALOG,    GUI_ENABLED,       "Create Test Dialog" },
    { "&Resource Dialog",       MENU_RES_DIALOG,     GUI_ENABLED,       "Create Resource Dialog" },
    { "&GetNewVal\tdialog",     MENU_GETNEWVAL,      GUI_ENABLED,       NULL },
    { NULL,                     MENU_SEP_QUIT,       GUI_SEPARATOR,     NULL },
    { "&Quit",                  MENU_QUIT,           GUI_ENABLED,       "Quit", NUM_POPUP_MENUS, PopupMenu },
    { "&Quit",                  MENU_QUIT2,          GUI_ENABLED,       "Quit", NUM_POPUP_MENUS, PopupMenu },
    { "E&xit Sample",           MENU_EXIT,           GUI_ENABLED,       "Exit Sample Program" },
    { "Spawn &VI",              MENU_SPAWN,          GUI_ENABLED,       "Spawn WATCOM editor" }
};
#define NUM_MAIN_SUB_MENUS1 ( sizeof( MainSubMenu1 ) / sizeof( gui_menu_struct ) )

static gui_menu_struct MainSubMenu2[] = {
    { "Redra&w Menu\tlright",   MENU_REDRAW,            GUI_ENABLED,    "Redraw Menu" },
    { "&Resize Window\tright",  MENU_RESIZE,            GUI_GRAYED,     "Resize Window"         },
    { "&Show Window",           MENU_SHOW_WINDOW,       GUI_ENABLED,    "Show WIndow"   },
    { NULL,                     0,                      GUI_SEPARATOR,  NULL    },
    { "Re&place Colours",       MENU_REPLACE_COLOURS,   GUI_ENABLED,    "Replace Colours"       }
};
#define NUM_MAIN_SUB_MENUS2 ( sizeof( MainSubMenu2 ) / sizeof( gui_menu_struct ) )

static gui_menu_struct MainSubMenu3[] = {
    { "&Minimize",              MENU_MINIMIZE,  GUI_ENABLED,    "Minimize" },
    { "&Maximize",              MENU_MAXIMIZE,  GUI_ENABLED | GUI_IGNORE, "Maximize" },
    { NULL,                     0,              GUI_SEPARATOR, NULL },
    { "&Restore",               MENU_RESTORE,   GUI_ENABLED,    "Restore"   }
};
#define NUM_MAIN_SUB_MENUS3 ( sizeof( MainSubMenu3 ) / sizeof( gui_menu_struct ) )

static gui_menu_struct HelpSubMenu[] = {
    { "Help &Contents", MENU_HELP_CONTENTS,     GUI_ENABLED,    "Help Contents" },
    { "Help &On Help",  MENU_HELP_ON_HELP,      GUI_ENABLED,    "Help On Help" },
    { "Help &Search",   MENU_HELP_SEARCH,       GUI_ENABLED,    "Help Search"   },
    { "Help Conte&xt",  MENU_HELP_CONTEXT,      GUI_ENABLED,    "Help Context"   },
    { "Help &Key",      MENU_HELP_KEY,          GUI_ENABLED,    "Help Key"   }
};
#define NUM_HELP_SUB_MENUS ( sizeof( HelpSubMenu ) / sizeof( gui_menu_struct ) )

static gui_menu_struct MainSubMenu4[] = {
    { "Create Fi&xed ToolBar",          MENU_FIXED_TOOLBAR,     GUI_ENABLED, "Create Fixed Toolbar"   },
    { "Create &Floating ToolBar",       MENU_FLOATING_TOOLBAR,  GUI_ENABLED, "Create Floating Toolbar"},
    { NULL,                     0,              GUI_SEPARATOR, NULL },
    { "Fix/Float &ToolBar",             MENU_CHANGE_TOOLBAR,    GUI_GRAYED,  "Fix/Float Toolbar" },
    { "&Close ToolBar",                 MENU_CLOSE_TOOLBAR,     GUI_GRAYED,  "Close Toolbar"    }
};
#define NUM_MAIN_SUB_MENUS4 ( sizeof( MainSubMenu4 ) / sizeof( gui_menu_struct ) )

static gui_menu_struct MainSubMenu5[] = {
    { "&Status Window", MENU_STATUS_WND,        GUI_ENABLED, "Status Window"   },
    { "&Cascade",       MENU_CASCADE1,          GUI_ENABLED, "Cascade"   },
    { "&Help",          MENU_HELP,              GUI_ENABLED, "Help"   },
    { "&Switch Font",   MENU_SWITCH_FONT,       GUI_ENABLED, "Set Window 2 font same as Window 1 font" },
    { "&Resize Status", MENU_RESIZE_STATUS,     GUI_GRAYED,  "Set Status Window 2 lines high" },
    { "Flip Mouse On",  MENU_FLIP_MOUSE_ON,     GUI_ENABLED, "Flip Mouse On"    },
    { "Flip Mouse Off", MENU_FLIP_MOUSE_OFF,    GUI_ENABLED, "Flip Mouse Off"   },
    { "Set Menu",       MENU_SET_MENU,          GUI_ENABLED, "Change Menus"     },
    { "Test Resize",    MENU_TEST_RESIZE,       GUI_ENABLED, "Test GUISetRestoredSize" },
};
#define NUM_MAIN_SUB_MENUS5 ( sizeof( MainSubMenu5 ) / sizeof( gui_menu_struct ) )

gui_menu_struct PopupMenu[NUM_POPUP_MENUS] = {
    { "&Minize" ,          MENU_MINIMIZE,       GUI_ENABLED, "Minimize"  },
    { "Ma&ximize",         MENU_MAXIMIZE,       GUI_ENABLED, "Maximize"  },
    { NULL,                     0,              GUI_SEPARATOR, NULL },
    { "&Restore",          MENU_RESTORE,        GUI_ENABLED, "Restore"  },
    { "&Can't get me",     MENU_CANT_GET_ME,    GUI_GRAYED,  "Can't get me" },
    { "Me &either",        MENU_ME_EITHER,      GUI_GRAYED | GUI_IGNORE, "Me either"   },
    { "Re&size",           MENU_RESIZE,         GUI_ENABLED, "Resize" },
    { "Change Menu",       MENU_CHANGE,         GUI_ENABLED, "Change Menu" },
    { "Cascading Popup",   MENU_CASCADE,        GUI_ENABLED, "Cascading Popup", NUM_MAIN_SUB_MENUS4, MainSubMenu4  },
};

gui_menu_struct ModifyColour = {
"&Modify Colour",       MENU_MODIFY_COLOUR,     GUI_ENABLED,  "Modify Basic Colours"
};

gui_menu_struct ResetMenu = {
"Reset Menu",   MENU_RESET_MENU,        GUI_ENABLED, "Change Menus Back"
};
#define NUM_RESET_MENUS ( sizeof( ResetMenu ) / sizeof( gui_menu_struct ) )

gui_menu_struct NewMainMenu = {
    "&New Menu",        MENU_NEW,   GUI_ENABLED, "New Menu Items" , NUM_RESET_MENUS,    &ResetMenu
};

gui_menu_struct MenuMore = {
    "&This Is More",    MENU_THIS_IS_MORE,   GUI_ENABLED, "More Menu Items",
};

static gui_menu_struct ChildSubMenu[] = {
    { "&Bring Main To Front", MENU_BRING_MAIN_TO_FRONT, GUI_ENABLED, "Bring Main To Front" },
    { "Get &Client ",         MENU_GET_CLIENT,          GUI_ENABLED, "Call GUIGetClientRect" },
    { "Get &Rect",            MENU_GET_RECT,            GUI_ENABLED, "Call GUIGetRect" },
    { "Get &Absolute Rect",   MENU_GET_ABS_RECT,        GUI_ENABLED, "Call GUIGetAbsRect" },
    { "More",                 MENU_MORE,                GUI_ENABLED, "Cascading Menu" },
};
#define NUM_CHILD_SUB_MENUS ( sizeof( ChildSubMenu ) / sizeof( gui_menu_struct ) )

gui_menu_struct MainMenu[] = {
    { "&Dialogs",       MENU_DIALOGS,   GUI_ENABLED, "Dialogs", NUM_MAIN_SUB_MENUS1,    MainSubMenu1 },
    { "&Redraw",        MENU_DO_REDRAW, GUI_ENABLED, "Redraw",  NUM_MAIN_SUB_MENUS2,    MainSubMenu2 },
    { "&Size",          MENU_SIZE,      GUI_ENABLED, "Size",    NUM_MAIN_SUB_MENUS3,    MainSubMenu3 },
    { "&Toolbar",       MENU_TOOLBAR,   GUI_ENABLED, "Toolbar",  NUM_MAIN_SUB_MENUS4,   MainSubMenu4 },
#if mdi
    { "&Window",        MENU_WINDOW,    GUI_ENABLED | GUI_MDIWINDOW, "Window Actions", 0,       NULL },
#else
    { "&Window",        MENU_WINDOW,    GUI_ENABLED,  "Window Actions", 0,      NULL },
#endif
    { "&Other",         MENU_OTHER,     GUI_ENABLED,  "Other",  NUM_MAIN_SUB_MENUS5,    MainSubMenu5 },
    { "&Help",          MENU_HELP_PULLDOWN,     GUI_ENABLED,  "Help",   NUM_HELP_SUB_MENUS,     HelpSubMenu }
};

gui_menu_struct ChildMenu[] = {
    { "label &3", MENU_LABEL3, GUI_ENABLED, "label 3", NUM_CHILD_SUB_MENUS, ChildSubMenu }
};

#if !default_colours
static gui_colour_set ChildColours[GUI_NUM_ATTRS+1] = {
    /* Fore              Back        */
    GUI_BRIGHT_BLUE,  GUI_BRIGHT_WHITE,   /* GUI_MENU_PLAIN           */
    GUI_BRIGHT_WHITE, GUI_WHITE,          /* GUI_MENU_STANDOUT        */
    GUI_GREY,         GUI_WHITE,          /* GUI_MENU_GRAYED          */
    GUI_BRIGHT_CYAN,  GUI_BLACK,          /* GUI_MENU_ACTIVE          */
    GUI_BRIGHT_WHITE, GUI_BLACK,          /* GUI_MENU_ACTIVE_STANDOUT */
    GUI_WHITE,        GUI_BRIGHT_WHITE,   /* GUI_BACKGROUND           */
    GUI_BLUE,         GUI_WHITE,          /* GUI_MENU_FRAME           */
    GUI_GREY,         GUI_WHITE,          /* GUI_TITLE_INACTIVE       */
    GUI_WHITE,        GUI_BLUE,           /* GUI_FRAME_ACTIVE         */
    GUI_GREY,         GUI_WHITE,          /* GUI_FRAME_INACTIVE       */
    GUI_BRIGHT_WHITE, GUI_BLUE,           /* GUI_ICON                 */
    GUI_GREY,         GUI_BLACK,          /* GUI_MENU_GRAYED_ACTIVE   */
    GUI_GREY,         GUI_WHITE,          /* GUI_FRAME_RESIZE         */
    GUI_BRIGHT_CYAN,  GUI_CYAN            /* GUI_FIRST_UNUSED         */
};
#endif

#if !default_colours
static gui_colour_set DialogColours[GUI_NUM_ATTRS] = {
    /* Fore              Back        */
    GUI_BRIGHT_WHITE, GUI_BLUE,          /* GUI_MENU_PLAIN           */
    GUI_BRIGHT_WHITE, GUI_BLUE,          /* GUI_MENU_STANDOUT        */
    GUI_GREY,         GUI_BLUE,          /* GUI_MENU_GRAYED          */
    GUI_CYAN,         GUI_BLACK,         /* GUI_MENU_ACTIVE          */
    GUI_BRIGHT_WHITE, GUI_BLACK,         /* GUI_MENU_ACTIVE_STANDOUT */
    GUI_BRIGHT_WHITE, GUI_BLUE,          /* GUI_BACKGROUND           */
    GUI_CYAN,         GUI_BLUE,          /* GUI_MENU_FRAME           */
    GUI_GREY,         GUI_BLUE,          /* GUI_TITLE_INACTIVE       */
    GUI_BRIGHT_BLUE,  GUI_CYAN,          /* GUI_FRAME_ACTIVE         */
    GUI_BRIGHT_BLUE,  GUI_CYAN,          /* GUI_FRAME_INACTIVE       */
    GUI_BRIGHT_WHITE, GUI_BLUE,          /* GUI_ICON                 */
    GUI_GREY,         GUI_BLACK,         /* GUI_MENU_GRAYED_ACTIVE   */
    GUI_GREY,         GUI_CYAN           /* GUI_FRAME_RESIZE         */
};
#endif

gui_colour_set ToolPlain = {
    GUI_BLACK,          GUI_BLUE,        /* GUI_HOTSPOT         */
};

gui_colour_set ToolStandout = {
    GUI_BRIGHT_WHITE,   GUI_BLUE,           /* GUI_HOTSPOT_STANDOUT     */
};

#if !default_colours
gui_colour_set ParentColours[GUI_NUM_ATTRS] = {
    /* Fore              Back        */
    GUI_BLUE,           GUI_WHITE,          /* GUI_MENU_PLAIN           */
    GUI_BRIGHT_WHITE,   GUI_WHITE,          /* GUI_MENU_STANDOUT        */
    GUI_GREY,           GUI_WHITE,          /* GUI_MENU_GRAYED          */
    GUI_BLUE,           GUI_BLACK,          /* GUI_MENU_ACTIVE          */
    GUI_BRIGHT_WHITE,   GUI_BLACK,          /* GUI_MENU_ACTIVE_STANDOUT */
    GUI_BRIGHT_BLUE,    GUI_BRIGHT_WHITE,   /* GUI_BACKGROUND           */
    GUI_BLUE,           GUI_WHITE,          /* GUI_MENU_FRAME           */
    GUI_GREY,           GUI_WHITE,          /* GUI_TITLE_INACTIVE       */
    GUI_WHITE,          GUI_BLUE,           /* GUI_FRAME_ACTIVE         */
    GUI_GREY,           GUI_WHITE,          /* GUI_FRAME_INACTIVE       */
    GUI_BRIGHT_WHITE,   GUI_BLUE,           /* GUI_ICON                 */
    GUI_GREY,           GUI_BLACK,          /* GUI_MENU_GRAYED_ACTIVE   */
    GUI_GREY,           GUI_WHITE           /* GUI_FRAME_RESIZE       */
};
#endif

static disp_entry Child1Data[NUM_CHILD1_ROWS] = {
    { NULL, "This is data in the child window 1", 0 },
    { NULL, "This is a second row of data in the child window 1", 0 },
    { NULL, "This is a third row of data in the child window 1", 0 },
    { NULL, "This is a fourth row of data in the child window 1", 0 },
    { NULL, "This is a fifth row of data in the child window 1", 0 },
    { NULL, "This is a sixth row of data in the child window 1", 0 },
    { NULL, "This is a seventh row of data in the child window 1", 0 },
    { NULL, "This is a eighth row of data in the child window 1", 0 },
    { NULL, "This is a ninth row of data in the child window 1", 0 },
    { NULL, "This is a tenth row of data in the child window 1", 0 },
    { NULL, "This is a eleventh row of data in the child window 1", 0 },
    { NULL, "This is a twelth row of data in the child window 1", 0 },
    { NULL, "This is a thirteenth row of data in the child window 1", 0 }
};

disp_entry Child2Data[NUM_CHILD2_ROWS] = {
    { NULL, "This is data in the child window 2", 0 },
    { NULL, "This is a second row of data in the child window 2", 0 },
    { NULL, "This is a third row of data in the child window 2", 0 },
    { NULL, "This is a fourth row of data in the child window 2", 0 },
    { NULL, "This is a fifth row of data in the child window 2", 0 },
    { NULL, "This is a sixth row of data in the child window 2", 0 },
    { NULL, "This is a seventh row of data in the child window 2", 0 }
};

disp_entry IndentData[NUM_CHILD1_ROWS] = {
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 },
    { NULL, "Indent", 0 }
};

out_info Child1_Out = { NUM_CHILD1_ROWS, 0, Child1Data };
out_info Child2_Out = { NUM_CHILD2_ROWS, 0, Child2Data };

static gui_resource MainIcon = { MAIN, "Sample" };
static gui_resource Win1Icon = { WIN1, "Win1" };
static gui_resource Win2Icon = { WIN2, "Win2" };
static gui_resource Win3Icon = { WIN3, "Win3" };
static gui_resource DlgIcon = { DLG, "Dlg" };

gui_create_info Parent = {
    "Sample Application",
    { 500, 500, 1000, 1100 },
    GUI_NOSCROLL,
    GUI_GADGETS | GUI_SYSTEM_MENU | GUI_CHANGEABLE_FONT,
    NULL,
#if dynamic_menus
    0, NULL,
#else
    NUM_MAIN_MENUS,
    MainMenu,
#endif
#if default_colours
    0, NULL,
#else
    GUI_NUM_ATTRS,
    &ParentColours,
#endif
    &MainEventWnd,
    NULL,
    &MainIcon,
    0
};

gui_create_info Child1 = {
    "Child 1",
    { 0, 0, 1000, 300 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    NUM_CHILD_MENUS,
    ChildMenu,
#if default_colours
    0,
    NULL,
#else
    GUI_NUM_ATTRS+1,
    &ChildColours,
#endif
    &Child1EventWnd,
    &Child1_Out,
    &Win1Icon,
    0
};

gui_create_info Child2 = {
    "Window 2",
    { 0, 500, 1000, 300 },
    NULL, //GUI_SCROLL_BOTH,
    //GUI_POPUP | GUI_MINIMIZE | GUI_MAXIMIZE | GUI_NOFRAME | GUI_VISIBLE | GUI_SYSTEM_MENU | GUI_CHANGEABLE_FONT,
    GUI_POPUP | GUI_CLOSEABLE | GUI_MINIMIZE | GUI_MAXIMIZE | GUI_RESIZEABLE | GUI_VISIBLE | GUI_SYSTEM_MENU | GUI_CHANGEABLE_FONT,
    //GUI_MINIMIZE | GUI_MAXIMIZE | GUI_RESIZEABLE | GUI_VISIBLE | GUI_SYSTEM_MENU | GUI_CHANGEABLE_FONT,
    NULL,
#if 0
    0, NULL,
#else
    NUM_CHILD_MENUS,
    ChildMenu,
#endif
#if default_colours
    0, NULL,
#else
    GUI_NUM_ATTRS+1,
    &ChildColours,
#endif
    &ChildEventWnd,
    &Child2_Out,
    &Win2Icon,
    0
};

gui_create_info Child3 = {
    "Window 3",
    { 0, 500, 1000, 300 },
    GUI_SCROLL_BOTH,
    GUI_VISIBLE+GUI_SYSTEM_MENU+GUI_CLOSEABLE+GUI_MAXIMIZE+GUI_RESIZEABLE+GUI_MINIMIZE,
    NULL,
    0,
    NULL,
#if default_colours
    0, NULL,
#else
    GUI_NUM_ATTRS+1,
    &ChildColours,
#endif
    Child3EventWnd,
    NULL,
    &Win3Icon,
    0
};

gui_create_info Child4 = {
    "Child 4",
    { 20, 20, 200, 200 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    0,
    NULL,
    0,
    NULL,
    &Child1EventWnd,
    &Child1_Out,
    &Win1Icon,
    0
};

gui_control_info ComboBoxControl = {
    GUI_EDIT_COMBOBOX,
    NULL,
    { 0, 325, 800, 50 },
    NULL,
    GUI_NOSCROLL,
    GUI_NONE,
    EDITWINDOW_CONTROL
};

gui_control_info ComboBoxControl2 = {
    GUI_EDIT,
    "",
    { 0, 450, 800, 50 },
    NULL,
    GUI_NOSCROLL,
    GUI_NONE,
    EDITWINDOW_CONTROL2
};

gui_control_info ComboOkayControl = {
    GUI_DEFPUSH_BUTTON,
    "OK",
    { 850, 325, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    AUTOMATIC,
    COMBOOKAY_CONTROL,
};


gui_control_info ListBoxControl = {
#if combo
    GUI_COMBOBOX,
#else
    GUI_COMBOBOX,
    //GUI_EDIT,
    //GUI_DEFPUSH_BUTTON,
    //GUI_GROUPBOX,
    //GUI_STATIC,
    //GUI_LISTBOX,
#endif
    NULL, //"Title",
    { 0, 0, 800, 150 },
    NULL,
    GUI_NOSCROLL,
    //GUI_NONE, // AUTOMATIC, //GUI_CONTROL_CENTRE,
    GUI_CONTROL_CENTRE,
    COMBOBOX_CONTROL
};

gui_create_info DialogWndControl = {
    "Enter new value :",
    { 150, 250, 700, 700 },
    GUI_NOSCROLL,
    GUI_CLOSEABLE | GUI_VISIBLE | GUI_DIALOG_LOOK | GUI_SYSTEM_MENU,
    NULL,
    0,
    NULL,
#if default_colours
    0, NULL,
#else
    GUI_NUM_ATTRS,
    &DialogColours,
#endif
    &ControlEventWnd,
    NULL,
    &DlgIcon,
    0
};

gui_control_info okbutton =
  { GUI_PUSH_BUTTON,
    "OK",
    { 175, 450+BUTTON_HEIGHT+5, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_TAB_GROUP | AUTOMATIC,
    OKBUTTON_CONTROL };

#define NUM_COMBOBOX_STRINGS 5

#if combo
static char * ComboBoxStrings[NUM_COMBOBOX_STRINGS] = {
    { "string1" },
    { "string2" },
    { "string3" },
    { "string4" },
    { "string5" }
};
#endif

gui_toolbar_struct ToolBar[NUM_TOOLBAR_BUTTONS] = {
    { "&Min",           MINIMIZE,       MENU_MINIMIZE, "Minimize"       },
    { "Ma&x",           MAXIMIZE,       MENU_MAXIMIZE, "Maximize"       },
    { "&Restore",       RESTORE,        MENU_RESTORE,  "Restore"        },
    { "Re&size",        RESIZE,         MENU_RESIZE,   "Resize"         },
    { "&Change Menu",   CHANGEMENU,     MENU_CHANGE,   "Change Menu"    }
};

