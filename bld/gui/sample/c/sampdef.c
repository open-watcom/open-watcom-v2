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


#include "sample.h"
#include "dlgstat.h"
#include "samprc.rh"


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
    GUI_BLACK, GUI_BLACK    /* GUI_BACKGROUND_COLOUR */
};

gui_colour_set DialColours = {
    GUI_WHITE, GUI_BLUE     /* GUI_DIALOG_COLOUR     */
};

gui_colour_set DialFrameColours = {
    GUI_WHITE, GUI_BLUE     /* GUI_DIALOG_COLOUR     */
};
#endif

static gui_menu_struct MainSubMenu4[] = {
    { "Create Fi&xed ToolBar",          MENU_FIXED_TOOLBAR,     GUI_STYLE_MENU_ENABLED,     "Create Fixed Toolbar",     GUI_NO_MENU },
    { "Create &Floating ToolBar",       MENU_FLOATING_TOOLBAR,  GUI_STYLE_MENU_ENABLED,     "Create Floating Toolbar",  GUI_NO_MENU },
    { NULL,                             0,                      GUI_STYLE_MENU_SEPARATOR,   NULL,                       GUI_NO_MENU },
    { "Fix/Float &ToolBar",             MENU_CHANGE_TOOLBAR,    GUI_STYLE_MENU_GRAYED,      "Fix/Float Toolbar",        GUI_NO_MENU },
    { "&Close ToolBar",                 MENU_CLOSE_TOOLBAR,     GUI_STYLE_MENU_GRAYED,      "Close Toolbar",            GUI_NO_MENU }
};

gui_menu_struct PopupMenu[] = {
    { "&Minize" ,          MENU_MINIMIZE,       GUI_STYLE_MENU_ENABLED,                         "Minimize",             GUI_NO_MENU },
    { "Ma&ximize",         MENU_MAXIMIZE,       GUI_STYLE_MENU_ENABLED,                         "Maximize",             GUI_NO_MENU },
    { NULL,                0,                   GUI_STYLE_MENU_SEPARATOR,                       NULL,                   GUI_NO_MENU },
    { "&Restore",          MENU_RESTORE,        GUI_STYLE_MENU_ENABLED,                         "Restore",              GUI_NO_MENU },
    { "&Can't get me",     MENU_CANT_GET_ME,    GUI_STYLE_MENU_GRAYED,                          "Can't get me",         GUI_NO_MENU },
    { "Me &either",        MENU_ME_EITHER,      GUI_STYLE_MENU_GRAYED | GUI_STYLE_MENU_IGNORE,  "Me either",            GUI_NO_MENU },
    { "Re&size",           MENU_RESIZE,         GUI_STYLE_MENU_ENABLED,                         "Resize",               GUI_NO_MENU },
    { "Change Menu",       MENU_CHANGE,         GUI_STYLE_MENU_ENABLED,                         "Change Menu",          GUI_NO_MENU },
    { "Cascading Popup",   MENU_CASCADE,        GUI_STYLE_MENU_ENABLED,                         "Cascading Popup",      GUI_MENU_ARRAY( MainSubMenu4 ) },
};

static gui_menu_struct MainSubMenu1[] = {
    { "&Static Dialog",         MENU_STATIC_DIALOG,  GUI_STYLE_MENU_CHECKED,    "Create Static Dialog",                 GUI_NO_MENU },
    { "&Dynamic Dialog",        MENU_DYNAMIC_DIALOG, GUI_STYLE_MENU_ENABLED,    "Create Dynamic Dialog",                GUI_NO_MENU },
    { "&Test Dialog",           MENU_TEST_DIALOG,    GUI_STYLE_MENU_ENABLED,    "Create Test Dialog",                   GUI_NO_MENU },
    { "&Resource Dialog",       MENU_RES_DIALOG,     GUI_STYLE_MENU_ENABLED,    "Create Resource Dialog",               GUI_NO_MENU },
    { "&GetNewVal\tdialog",     MENU_GETNEWVAL,      GUI_STYLE_MENU_ENABLED,    NULL,                                   GUI_NO_MENU },
    { NULL,                     MENU_SEP_QUIT,       GUI_STYLE_MENU_SEPARATOR,  NULL,                                   GUI_NO_MENU },
    { "&Quit",                  MENU_QUIT,           GUI_STYLE_MENU_ENABLED,    "Quit",                                 GUI_MENU_ARRAY( PopupMenu ) },
    { "&Quit",                  MENU_QUIT2,          GUI_STYLE_MENU_ENABLED,    "Quit",                                 GUI_MENU_ARRAY( PopupMenu ) },
    { "E&xit Sample",           MENU_EXIT,           GUI_STYLE_MENU_ENABLED,    "Exit Sample Program",                  GUI_NO_MENU },
    { "Spawn &VI",              MENU_SPAWN,          GUI_STYLE_MENU_ENABLED,    "Spawn WATCOM editor",                  GUI_NO_MENU }
};

static gui_menu_struct MainSubMenu2[] = {
    { "Redra&w Menu\tlright",   MENU_REDRAW,            GUI_STYLE_MENU_ENABLED,     "Redraw Menu",                      GUI_NO_MENU },
    { "&Resize Window\tright",  MENU_RESIZE,            GUI_STYLE_MENU_GRAYED,      "Resize Window",                    GUI_NO_MENU },
    { "&Show Window",           MENU_SHOW_WINDOW,       GUI_STYLE_MENU_ENABLED,     "Show WIndow",                      GUI_NO_MENU },
    { NULL,                     0,                      GUI_STYLE_MENU_SEPARATOR,   NULL,                               GUI_NO_MENU },
    { "Re&place Colours",       MENU_REPLACE_COLOURS,   GUI_STYLE_MENU_ENABLED,     "Replace Colours",                  GUI_NO_MENU }
};

static gui_menu_struct MainSubMenu3[] = {
    { "&Minimize",              MENU_MINIMIZE,  GUI_STYLE_MENU_ENABLED,                         "Minimize",             GUI_NO_MENU },
    { "&Maximize",              MENU_MAXIMIZE,  GUI_STYLE_MENU_ENABLED | GUI_STYLE_MENU_IGNORE, "Maximize",             GUI_NO_MENU },
    { NULL,                     0,              GUI_STYLE_MENU_SEPARATOR,                       NULL,                   GUI_NO_MENU },
    { "&Restore",               MENU_RESTORE,   GUI_STYLE_MENU_ENABLED,                         "Restore",              GUI_NO_MENU }
};

static gui_menu_struct HelpSubMenu[] = {
    { "Help &Contents", MENU_HELP_CONTENTS,     GUI_STYLE_MENU_ENABLED,     "Help Contents",                            GUI_NO_MENU },
    { "Help &On Help",  MENU_HELP_ON_HELP,      GUI_STYLE_MENU_ENABLED,     "Help On Help",                             GUI_NO_MENU },
    { "Help &Search",   MENU_HELP_SEARCH,       GUI_STYLE_MENU_ENABLED,     "Help Search",                              GUI_NO_MENU },
    { "Help Conte&xt",  MENU_HELP_CONTEXT,      GUI_STYLE_MENU_ENABLED,     "Help Context",                             GUI_NO_MENU },
    { "Help &Key",      MENU_HELP_KEY,          GUI_STYLE_MENU_ENABLED,     "Help Key",                                 GUI_NO_MENU }
};

static gui_menu_struct MainSubMenu5[] = {
    { "&Status Window", MENU_STATUS_WND,        GUI_STYLE_MENU_ENABLED, "Status Window",                                GUI_NO_MENU },
    { "&Cascade",       MENU_CASCADE1,          GUI_STYLE_MENU_ENABLED, "Cascade",                                      GUI_NO_MENU },
    { "&Help",          MENU_HELP,              GUI_STYLE_MENU_ENABLED, "Help",                                         GUI_NO_MENU },
    { "&Switch Font",   MENU_SWITCH_FONT,       GUI_STYLE_MENU_ENABLED, "Set Window 2 font same as Window 1 font",      GUI_NO_MENU },
    { "&Resize Status", MENU_RESIZE_STATUS,     GUI_STYLE_MENU_GRAYED,  "Set Status Window 2 lines high",               GUI_NO_MENU },
    { "Flip Mouse On",  MENU_FLIP_MOUSE_ON,     GUI_STYLE_MENU_ENABLED, "Flip Mouse On",                                GUI_NO_MENU },
    { "Flip Mouse Off", MENU_FLIP_MOUSE_OFF,    GUI_STYLE_MENU_ENABLED, "Flip Mouse Off",                               GUI_NO_MENU },
    { "Set Menu",       MENU_SET_MENU,          GUI_STYLE_MENU_ENABLED, "Change Menus",                                 GUI_NO_MENU },
    { "Test Resize",    MENU_TEST_RESIZE,       GUI_STYLE_MENU_ENABLED, "Test GUISetRestoredSize",                      GUI_NO_MENU },
};

gui_menu_struct ModifyColour[] = {
    "&Modify Colour",      MENU_MODIFY_COLOUR,  GUI_STYLE_MENU_ENABLED, "Modify Basic Colours",                         GUI_NO_MENU
};

gui_menu_struct ResetMenu[] = {
    "Reset Menu",          MENU_RESET_MENU,     GUI_STYLE_MENU_ENABLED, "Change Menus Back",                            GUI_NO_MENU
};

gui_menu_struct NewMainMenu[] = {
    "&New Menu",           MENU_NEW,            GUI_STYLE_MENU_ENABLED, "New Menu Items",                               GUI_MENU_ARRAY( ResetMenu )
};

gui_menu_struct MenuMore[] = {
    "&This Is More",       MENU_THIS_IS_MORE,   GUI_STYLE_MENU_ENABLED, "More Menu Items",                              GUI_NO_MENU
};

static gui_menu_struct ChildSubMenu[] = {
    { "&Bring Main To Front", MENU_BRING_MAIN_TO_FRONT, GUI_STYLE_MENU_ENABLED, "Bring Main To Front",                  GUI_NO_MENU },
    { "Get &Client ",         MENU_GET_CLIENT,          GUI_STYLE_MENU_ENABLED, "Call GUIGetClientRect",                GUI_NO_MENU },
    { "Get &Rect",            MENU_GET_RECT,            GUI_STYLE_MENU_ENABLED, "Call GUIGetRect",                      GUI_NO_MENU },
    { "Get &Absolute Rect",   MENU_GET_ABS_RECT,        GUI_STYLE_MENU_ENABLED, "Call GUIGetAbsRect",                   GUI_NO_MENU },
    { "More",                 MENU_MORE,                GUI_STYLE_MENU_ENABLED, "Cascading Menu",                       GUI_NO_MENU },
};

gui_menu_struct MainMenu[] = {
    { "&Dialogs",       MENU_DIALOGS,       GUI_STYLE_MENU_ENABLED,                             "Dialogs",              GUI_MENU_ARRAY( MainSubMenu1 ) },
    { "&Redraw",        MENU_DO_REDRAW,     GUI_STYLE_MENU_ENABLED,                             "Redraw",               GUI_MENU_ARRAY( MainSubMenu2 ) },
    { "&Size",          MENU_SIZE,          GUI_STYLE_MENU_ENABLED,                             "Size",                 GUI_MENU_ARRAY( MainSubMenu3 ) },
    { "&Toolbar",       MENU_TOOLBAR,       GUI_STYLE_MENU_ENABLED,                             "Toolbar",              GUI_MENU_ARRAY( MainSubMenu4 ) },
#if mdi
    { "&Window",        MENU_WINDOW,        GUI_STYLE_MENU_ENABLED | GUI_STYLE_MENU_MDIWINDOW,  "Window Actions",       GUI_NO_MENU },
#else
    { "&Window",        MENU_WINDOW,        GUI_STYLE_MENU_ENABLED,                             "Window Actions",       GUI_NO_MENU },
#endif
    { "&Other",         MENU_OTHER,         GUI_STYLE_MENU_ENABLED,                             "Other",                GUI_MENU_ARRAY( MainSubMenu5 ) },
    { "&Help",          MENU_HELP_PULLDOWN, GUI_STYLE_MENU_ENABLED,                             "Help",                 GUI_MENU_ARRAY( HelpSubMenu ) }
};

gui_menu_struct ChildMenu[] = {
    { "label &3",       MENU_LABEL3,        GUI_STYLE_MENU_ENABLED,                             "label 3",              GUI_MENU_ARRAY( ChildSubMenu ) }
};

#if !default_colours
static gui_colour_set ChildColours[GUI_NUM_ATTRS + 1] = {
    /* Fore          Back        */
    GUI_BR_BLUE,    GUI_BR_WHITE,   /* GUI_MENU_PLAIN           */
    GUI_BR_WHITE,   GUI_WHITE,      /* GUI_MENU_STANDOUT        */
    GUI_GREY,       GUI_WHITE,      /* GUI_MENU_GRAYED          */
    GUI_BR_CYAN,    GUI_BLACK,      /* GUI_MENU_ACTIVE          */
    GUI_BR_WHITE,   GUI_BLACK,      /* GUI_MENU_ACTIVE_STANDOUT */
    GUI_WHITE,      GUI_BR_WHITE,   /* GUI_BACKGROUND           */
    GUI_BLUE,       GUI_WHITE,      /* GUI_MENU_FRAME           */
    GUI_GREY,       GUI_WHITE,      /* GUI_TITLE_INACTIVE       */
    GUI_WHITE,      GUI_BLUE,       /* GUI_FRAME_ACTIVE         */
    GUI_GREY,       GUI_WHITE,      /* GUI_FRAME_INACTIVE       */
    GUI_BR_WHITE,   GUI_BLUE,       /* GUI_ICON                 */
    GUI_GREY,       GUI_BLACK,      /* GUI_MENU_GRAYED_ACTIVE   */
    GUI_GREY,       GUI_WHITE,      /* GUI_FRAME_RESIZE         */
    GUI_BR_CYAN,    GUI_CYAN        /* GUI_FIRST_UNUSED         */
};
#endif

#if !default_colours
static gui_colour_set DialogColours[GUI_NUM_ATTRS] = {
    /* Fore          Back        */
    GUI_BR_WHITE,   GUI_BLUE,       /* GUI_MENU_PLAIN           */
    GUI_BR_WHITE,   GUI_BLUE,       /* GUI_MENU_STANDOUT        */
    GUI_GREY,       GUI_BLUE,       /* GUI_MENU_GRAYED          */
    GUI_CYAN,       GUI_BLACK,      /* GUI_MENU_ACTIVE          */
    GUI_BR_WHITE,   GUI_BLACK,      /* GUI_MENU_ACTIVE_STANDOUT */
    GUI_BR_WHITE,   GUI_BLUE,       /* GUI_BACKGROUND           */
    GUI_CYAN,       GUI_BLUE,       /* GUI_MENU_FRAME           */
    GUI_GREY,       GUI_BLUE,       /* GUI_TITLE_INACTIVE       */
    GUI_BR_BLUE,    GUI_CYAN,       /* GUI_FRAME_ACTIVE         */
    GUI_BR_BLUE,    GUI_CYAN,       /* GUI_FRAME_INACTIVE       */
    GUI_BR_WHITE,   GUI_BLUE,       /* GUI_ICON                 */
    GUI_GREY,       GUI_BLACK,      /* GUI_MENU_GRAYED_ACTIVE   */
    GUI_GREY,       GUI_CYAN        /* GUI_FRAME_RESIZE         */
};
#endif

gui_colour_set ToolPlain = {
    GUI_BLACK,      GUI_BLUE,       /* GUI_HOTSPOT              */
};

gui_colour_set ToolStandout = {
    GUI_BR_WHITE,   GUI_BLUE,       /* GUI_HOTSPOT_STANDOUT     */
};

#if !default_colours
gui_colour_set ParentColours[GUI_NUM_ATTRS] = {
    /* Fore          Back        */
    GUI_BLUE,       GUI_WHITE,      /* GUI_MENU_PLAIN           */
    GUI_BR_WHITE,   GUI_WHITE,      /* GUI_MENU_STANDOUT        */
    GUI_GREY,       GUI_WHITE,      /* GUI_MENU_GRAYED          */
    GUI_BLUE,       GUI_BLACK,      /* GUI_MENU_ACTIVE          */
    GUI_BR_WHITE,   GUI_BLACK,      /* GUI_MENU_ACTIVE_STANDOUT */
    GUI_BR_BLUE,    GUI_BR_WHITE,   /* GUI_BACKGROUND           */
    GUI_BLUE,       GUI_WHITE,      /* GUI_MENU_FRAME           */
    GUI_GREY,       GUI_WHITE,      /* GUI_TITLE_INACTIVE       */
    GUI_WHITE,      GUI_BLUE,       /* GUI_FRAME_ACTIVE         */
    GUI_GREY,       GUI_WHITE,      /* GUI_FRAME_INACTIVE       */
    GUI_BR_WHITE,   GUI_BLUE,       /* GUI_ICON                 */
    GUI_GREY,       GUI_BLACK,      /* GUI_MENU_GRAYED_ACTIVE   */
    GUI_GREY,       GUI_WHITE       /* GUI_FRAME_RESIZE         */
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
    GUI_NO_MENU,                        // Menu array
#else
    GUI_MENU_ARRAY( MainMenu ),         // Menu array
#endif
#if default_colours
    GUI_NO_COLOUR,                      // Colour attribute array
#else
    GUI_COLOUR_ARRAY( ParentColours ),  // Colour attribute array
#endif
    &MainWndGUIEventProc,               // GUI Event Callback function
    NULL,                               // Extra
    &MainIcon,                          // Icon
    NULL                                // Menu Resource
};

gui_create_info Child1 = {
    "Child 1",
    { 0, 0, 1000, 300 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    GUI_MENU_ARRAY( ChildMenu ),        // Menu array
#if default_colours
    GUI_NO_COLOUR,                      // Colour attribute array
#else
    GUI_COLOUR_ARRAY( ChildColours ),   // Colour attribute array
#endif
    &Child1WndGUIEventProc,             // GUI Event Callback function
    &Child1_Out,                        // Extra
    &Win1Icon,                          // Icon
    NULL                                // Menu Resource
};

gui_create_info Child2 = {
    "Window 2",
    { 0, 500, 1000, 300 },
    GUI_NOSCROLL, //GUI_SCROLL_BOTH,
    //GUI_POPUP | GUI_MINIMIZE | GUI_MAXIMIZE | GUI_NOFRAME | GUI_VISIBLE | GUI_SYSTEM_MENU | GUI_CHANGEABLE_FONT,
    GUI_POPUP | GUI_CLOSEABLE | GUI_MINIMIZE | GUI_MAXIMIZE | GUI_RESIZEABLE | GUI_VISIBLE | GUI_SYSTEM_MENU | GUI_CHANGEABLE_FONT,
    //GUI_MINIMIZE | GUI_MAXIMIZE | GUI_RESIZEABLE | GUI_VISIBLE | GUI_SYSTEM_MENU | GUI_CHANGEABLE_FONT,
    NULL,
#if 0
    GUI_NO_MENU,                        // Menu array
#else
    GUI_MENU_ARRAY( ChildMenu ),        // Menu array
#endif
#if default_colours
    GUI_NO_COLOUR,                      // Colour attribute array
#else
    GUI_COLOUR_ARRAY( ChildColours ),   // Colour attribute array
#endif
    &Child2WndGUIEventProc,             // GUI Event Callback function
    &Child2_Out,                        // Extra
    &Win2Icon,                          // Icon
    NULL                                // Menu Resource
};

gui_create_info Child3 = {
    "Window 3",
    { 0, 500, 1000, 300 },
    GUI_SCROLL_BOTH,
    GUI_VISIBLE+GUI_SYSTEM_MENU+GUI_CLOSEABLE+GUI_MAXIMIZE+GUI_RESIZEABLE+GUI_MINIMIZE,
    NULL,
    GUI_NO_MENU,                        // Menu array
#if default_colours
    GUI_NO_COLOUR,                      // Colour attribute array
#else
    GUI_COLOUR_ARRAY( ChildColours ),   // Colour attribute array
#endif
    Child3WndGUIEventProc,              // GUI Event Callback function
    NULL,                               // Extra
    &Win3Icon,                          // Icon
    NULL                                // Menu Resource
};

gui_create_info Child4 = {
    "Child 4",
    { 20, 20, 200, 200 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    GUI_NO_MENU,                        // Menu array
    GUI_NO_COLOUR,                      // Colour attribute array
    &Child1WndGUIEventProc,             // GUI Event Callback function
    &Child1_Out,                        // Extra
    &Win1Icon,                          // Icon
    NULL                                // Menu Resource
};

gui_control_info ComboBoxControl = {
    GUI_EDIT_COMBOBOX,
    NULL,
    { 0, 325, 800, 50 },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_NOSTYLE,
    EDITWINDOW_CONTROL
};

gui_control_info ComboBoxControl2 = {
    GUI_EDIT,
    "",
    { 0, 450, 800, 50 },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_NOSTYLE,
    EDITWINDOW_CONTROL2
};

gui_control_info ComboOkayControl = {
    GUI_DEFPUSH_BUTTON,
    "OK",
    { 850, 325, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_AUTOMATIC,
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
    GUI_STYLE_CONTROL_CENTRE,
    COMBOBOX_CONTROL
};

gui_create_info DialogWndControl = {
    "Enter new value :",
    { 150, 250, 700, 700 },
    GUI_NOSCROLL,
    GUI_CLOSEABLE | GUI_VISIBLE | GUI_DIALOG_LOOK | GUI_SYSTEM_MENU,
    NULL,
    GUI_NO_MENU,                        // Menu array
#if default_colours
    GUI_NO_COLOUR,                      // Colour attribute array
#else
    GUI_COLOUR_ARRAY( DialogColours ),  // Colour attribute array
#endif
    &ControlWndGUIEventProc,            // GUI Event Callback function
    NULL,                               // Extra
    &DlgIcon,                           // Icon
    NULL                                // Menu Resource
};

gui_control_info okbutton = {
    GUI_PUSH_BUTTON,
    "OK",
    { 175, 450+BUTTON_HEIGHT+5, BUTTON_WIDTH, BUTTON_HEIGHT },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_TAB_GROUP | GUI_STYLE_CONTROL_AUTOMATIC,
    OKBUTTON_CONTROL
};

#if combo
static char * ComboBoxStrings[] = {
    { "string1" },
    { "string2" },
    { "string3" },
    { "string4" },
    { "string5" }
};
#endif

static gui_toolbar_struct ToolBar[] = {
    { "&Min",           MINIMIZE,       MENU_MINIMIZE, "Minimize"       },
    { "Ma&x",           MAXIMIZE,       MENU_MAXIMIZE, "Maximize"       },
    { "&Restore",       RESTORE,        MENU_RESTORE,  "Restore"        },
    { "Re&size",        RESIZE,         MENU_RESIZE,   "Resize"         },
    { "&Change Menu",   CHANGEMENU,     MENU_CHANGE,   "Change Menu"    }
};

gui_toolbar_items   tb_ToolBar = GUI_TOOLBAR_ARRAY( ToolBar );

gui_menu_items      menu_PopupMenu      = GUI_MENU_ARRAY( PopupMenu );
gui_menu_items      menu_NewMainMenu    = GUI_MENU_ARRAY( NewMainMenu );
gui_menu_items      menu_MainMenu       = GUI_MENU_ARRAY( MainMenu );
gui_menu_items      menu_ChildMenu      = GUI_MENU_ARRAY( ChildMenu );
