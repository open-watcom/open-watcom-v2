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
* Description:  Master GUI library include file.
*
****************************************************************************/


#include "bool.h"

#include "guimem.h"

typedef int gui_ord;

#define GUI_LAST_INTERNAL_MSG 255

#define GUI_LAST_MENU_ID 10000

typedef enum {
    GUI_NO_EVENT,
    GUI_INIT_WINDOW,
    GUI_INIT_DIALOG,
    GUI_NOT_ACTIVE,
    GUI_NOW_ACTIVE,
    GUI_MOVE,
    GUI_RESIZE,
    GUI_ICONIFIED,
    GUI_FONT_CHANGED,
    GUI_PAINT,
    GUI_KEYDOWN,         /* keystroke messages */
    GUI_KEYUP,
    GUI_KEY_CONTROL,
    GUI_SCROLL_UP,       /* scrolling messages */
    GUI_SCROLL_PAGE_UP,
    GUI_SCROLL_TOP,
    GUI_SCROLL_DOWN,
    GUI_SCROLL_PAGE_DOWN,
    GUI_SCROLL_BOTTOM,
    GUI_SCROLL_VERTICAL,
    GUI_SCROLL_LEFT,
    GUI_SCROLL_PAGE_LEFT,
    GUI_SCROLL_FULL_LEFT,
    GUI_SCROLL_RIGHT,
    GUI_SCROLL_PAGE_RIGHT,
    GUI_SCROLL_FULL_RIGHT,
    GUI_SCROLL_HORIZONTAL,
    GUI_CLICKED,                /* menu clicked        */
    GUI_CONTROL_DCLICKED,       /* control double clicked */
    GUI_LBUTTONDOWN,      /* mouse messages                 */
    GUI_LBUTTONUP,
    GUI_LBUTTONDBLCLK,
    GUI_RBUTTONDOWN,
    GUI_RBUTTONUP,
    GUI_RBUTTONDBLCLK,
    GUI_MOUSEMOVE,
    GUI_TOOLBAR_DESTROYED,      /* toolbar messages */
    GUI_TOOLBAR_FIXED,
    GUI_TOOLBAR_FLOATING,
    GUI_CLOSE,
    GUI_DESTROY,
    GUI_HSCROLL_NOTIFY,
    GUI_VSCROLL_NOTIFY,
    GUI_CONTROL_NOT_ACTIVE,
    GUI_CONTROL_CLICKED,        /* control clicked */
    GUI_INITMENUPOPUP,          /* popup menu about to open */
    GUI_CONTROL_RCLICKED,       /* control right clicked */
    GUI_DIALOG_ESCAPE,          /* escape was pressed in a dialog */
    GUI_STATUS_CLEARED,         /* the text in the status line was cleared */
    GUI_QUERYENDSESSION,        /* the windowing system is asking if we can close */
                                /* if you return false your session indicating
                                   that it is OK by it to shutdown the
                                   windowing subsystem */
    GUI_ENDSESSION,             /* the windowing system is telling us we must close */
    GUI_KEYTOITEM,
    GUI_ACTIVATEAPP,
    GUI_CONTEXTHELP,            /* the F1 key has been pressed */
    GUI_TIMER_EVENT
} gui_event;

typedef struct gui_coord {
    gui_ord x;
    gui_ord y;
} gui_coord;

typedef struct gui_point {
    int x;
    int y;
} gui_point;

typedef struct gui_window gui_window;

typedef enum {
    GUI_BAD_CLASS = -1,
    GUI_PUSH_BUTTON,
    GUI_DEFPUSH_BUTTON,
    GUI_RADIO_BUTTON,
    GUI_CHECK_BOX,
    GUI_COMBOBOX,
    GUI_EDIT,
    GUI_LISTBOX,
    GUI_SCROLLBAR,
    GUI_STATIC,
    GUI_GROUPBOX,
    GUI_EDIT_COMBOBOX,
    GUI_EDIT_MLE,
    GUI_NUM_CONTROL_CLASSES,
    GUI_FIRST_CONTROL_CLASS = GUI_PUSH_BUTTON,
    GUI_LAST_CONTROL_CLASS = GUI_EDIT_MLE
} gui_control_class;

typedef enum {
    GUI_MENU_PLAIN,
    GUI_MENU_STANDOUT,
    GUI_MENU_GRAYED,
    GUI_MENU_ACTIVE,
    GUI_MENU_ACTIVE_STANDOUT,
    GUI_BACKGROUND,
    GUI_MENU_FRAME,
    GUI_TITLE_INACTIVE,
    GUI_FRAME_ACTIVE,
    GUI_FRAME_INACTIVE,
    GUI_ICON,
    GUI_MENU_GRAYED_ACTIVE,
    GUI_FRAME_RESIZE,
    GUI_CONTROL_BACKGROUND,
    GUI_FIRST_ATTR = GUI_MENU_PLAIN,
    GUI_LAST_ATTR  = GUI_CONTROL_BACKGROUND,
    GUI_FIRST_UNUSED = GUI_LAST_ATTR + 1,
    GUI_NUM_ATTRS = GUI_LAST_ATTR + 1
} gui_attr;

typedef enum {
    GUI_DLG_NORMAL,
    GUI_DLG_FRAME,
    GUI_DLG_SHADOW,
    GUI_DLG_SCROLL_ICON,
    GUI_DLG_SCROLL_BAR,
    GUI_DLG_BUTTON_PLAIN,
    GUI_DLG_BUTTON_STANDOUT,
    GUI_DLG_BUTTON_ACTIVE,
    GUI_DLG_BUTTON_ACTIVE_STANDOUT,
    GUI_DLG_NUM_ATTRS
} gui_dlg_attr;

typedef enum {
    GUI_ENABLED         = 0x00,
    GUI_MENU_CHECKED    = 0x01,
    GUI_GRAYED          = 0x02,
    GUI_SEPARATOR       = 0x04,
    GUI_IGNORE          = 0x08, // don't display
    GUI_MDIWINDOW       = 0x10,
    GUI_UTIL_1          = 0x40, // can be used by the application
    GUI_UTIL_2          = 0x80
} gui_menu_styles ;

typedef void    *gui_help_instance;

typedef enum {
    GUI_HELP_CONTENTS
,   GUI_HELP_ON_HELP
,   GUI_HELP_SEARCH
,   GUI_HELP_CONTEXT
,   GUI_HELP_KEY
} gui_help_actions;

typedef long gui_bitmap;

typedef struct gui_toolbar_struct {
    char                *label;
    gui_bitmap          bitmap;
    int                 id;
    char                *hinttext;
    char                *tip;
} gui_toolbar_struct;

typedef struct gui_menu_struct {
    char                        *label;
    int                         id;
    gui_menu_styles             style;
    char                        *hinttext;
    int                         num_child_menus;
    struct gui_menu_struct      *child;
} gui_menu_struct;

typedef enum {
        GUI_BAD_COLOUR = -1,
        GUI_BLACK,
        GUI_BLUE,
        GUI_GREEN,
        GUI_CYAN,
        GUI_RED,
        GUI_MAGENTA,
        GUI_BROWN,
        GUI_WHITE,
        GUI_GREY,
        GUI_BRIGHT_BLUE,
        GUI_BRIGHT_GREEN,
        GUI_BRIGHT_CYAN,
        GUI_BRIGHT_RED,
        GUI_BRIGHT_MAGENTA,
        GUI_BRIGHT_YELLOW,
        GUI_BRIGHT_WHITE,
        GUIEX_DLG_BKGRND,
        GUIEX_WND_BKGRND,
        GUIEX_HIGHLIGHT,
        GUIEX_HIGHLIGHTTEXT,
        GUI_NUM_COLOURS,
        GUI_FIRST_COLOUR = GUI_BLACK,
        GUI_LAST_COLOUR = GUIEX_HIGHLIGHTTEXT
} gui_colour;

typedef struct gui_colour_set {
    gui_colour fore;
    gui_colour back;
} gui_colour_set;

typedef unsigned long gui_rgb;

#define GUIRGB(r,g,b)          ((gui_rgb)(((unsigned char)(r)) | (((unsigned short)(g))<<8) | (((unsigned long)(b))<<16)))

#define GUIGETRVALUE(rgb)       ((rgb) & 0xff)
#define GUIGETGVALUE(rgb)       (((rgb) & 0xff00 ) >> 8)
#define GUIGETBVALUE(rgb)       (((rgb) & 0xff0000) >> 16)

typedef struct gui_rect {
    gui_ord     x;
    gui_ord     y;
    gui_ord     width;
    gui_ord     height;
} gui_rect;

typedef enum gui_scroll_styles {
    GUI_NOSCROLL        = 0x00,
    GUI_HSCROLL         = 0x01,
    GUI_VSCROLL         = 0x02,
    GUI_HDRAG           = 0x04, /* change position by draging thumb */
    GUI_VDRAG           = 0x08, /* change position by draging thumb */
    GUI_HTRACK          = 0x10, /* scroll while dragging thumb */
    GUI_VTRACK          = 0x20, /* scroll while dragging thumb */
    GUI_HCOLS           = 0x40, /* scroll by average character width */
    GUI_VROWS           = 0x80, /* scroll by average character height */
    GUI_HSCROLL_ALL     = GUI_HSCROLL | GUI_HDRAG | GUI_HTRACK | GUI_HCOLS,
    GUI_VSCROLL_ALL     = GUI_VSCROLL | GUI_VDRAG | GUI_VTRACK | GUI_VROWS,
    GUI_SCROLL_BOTH     = GUI_HSCROLL | GUI_VSCROLL | GUI_HCOLS | GUI_VROWS,
    GUI_SCROLL_BOTH_ALL = GUI_HSCROLL_ALL | GUI_VSCROLL_ALL
} gui_scroll_styles;

typedef enum gui_create_styles {
    GUI_NONE            = 0x00000000,
    GUI_HSCROLL_EVENTS  = 0x00000001,
    GUI_VSCROLL_EVENTS  = 0x00000002,
    GUI_CURSOR          = 0x00000004,
    GUI_RESIZEABLE      = 0x00000008,
    GUI_MAXIMIZE        = 0x00000010,
    GUI_MINIMIZE        = 0x00000020,
    GUI_CLOSEABLE       = 0x00000040,
    GUI_SYSTEM_MENU     = 0x00000080,
    GUI_VISIBLE         = 0x00000100,
    GUI_DIALOG_LOOK     = 0x00000200,
    GUI_INIT_INVISIBLE  = 0x00000400,
    GUI_CHANGEABLE_FONT = 0x00000800,
    GUI_POPUP           = 0x00001000,
    GUI_INIT_MAXIMIZED  = 0x00002000,
    GUI_INIT_MINIMIZED  = 0x00004000,
    GUI_NOFRAME         = 0x00008000,
    GUI_3D_BORDER       = 0x00010000,
    GUI_SCROLL_EVENTS   = GUI_HSCROLL_EVENTS | GUI_VSCROLL_EVENTS,
    GUI_GADGETS         = GUI_CURSOR | GUI_RESIZEABLE | GUI_MINIMIZE |
                          GUI_MAXIMIZE | GUI_CLOSEABLE | GUI_SYSTEM_MENU,
    GUI_ALL             = GUI_GADGETS | GUI_VISIBLE | GUI_CHANGEABLE_FONT
} gui_create_styles;

typedef enum gui_control_styles {
    GUI_NOSTYLE                         = 0x00000000,
    GUI_CHECKED                         = 0x00000001,
    GUI_TAB_GROUP                       = 0x00000002,
    GUI_AUTOMATIC                       = 0x00000004,
    GUI_GROUP                           = 0x00000008,
    GUI_FOCUS                           = 0x00000010,
    GUI_CONTROL_INIT_INVISIBLE          = 0x00000020,
    GUI_CONTROL_LEFTNOWORDWRAP          = 0x00000040,
    GUI_CONTROL_NOPREFIX                = 0x00000080,
    GUI_CONTROL_CENTRE                  = 0x00000100,
    GUI_CONTROL_NOINTEGRALHEIGHT        = 0x00000200,
    GUI_CONTROL_SORTED                  = 0x00000400,
    GUI_CONTROL_MULTILINE               = 0x00000800,
    GUI_CONTROL_WANTRETURN              = 0x00001000,
    GUI_EDIT_INVISIBLE                  = 0x00002000,
    GUI_CONTROL_3STATE                  = 0x00004000,
    GUI_CONTROL_WANTKEYINPUT            = 0x00008000,
    GUI_CONTROL_READONLY                = 0x00010000,
    GUI_CONTROL_BORDER                  = 0x00020000
} gui_control_styles;

typedef enum gui_line_styles {
    GUI_PEN_SOLID       = 0x00,
    GUI_PEN_DASH        = 0x01,
    GUI_PEN_DOT         = 0x02,
    GUI_PEN_DASHDOT     = 0x04,
    GUI_PEN_DASHDOTDOT  = 0x08
} gui_line_styles;

typedef enum gui_bar_styles {
    GUI_BAR_SIMPLE      = 0x01,
    GUI_BAR_SHADOW      = 0x02,
    GUI_BAR_3DRECT1     = 0x04,
    GUI_BAR_3DRECT2     = 0x08,
    GUI_FULL_BAR        = 0x10
} gui_bar_styles;

typedef struct gui_resource {
    gui_bitmap  res;
    char        *chars; /* Character based              */
} gui_resource;

typedef struct gui_control_info {
    gui_control_class   control_class;
    char                *text;
    gui_rect            rect;
    gui_window          *parent;
    gui_scroll_styles   scroll;
    gui_control_styles  style;
    unsigned            id;
} gui_control_info;

typedef bool (GUICALLBACK)( gui_window *, gui_event, void * );
typedef void (ENUMCALLBACK)( gui_window *, void *param );
typedef void (CONTRENUMCALLBACK)( gui_window *parent, unsigned id, void *param );
typedef void (PICKCALLBACK)( gui_window *, unsigned id );
typedef void (PICKDLGOPEN)( char *name, int rows, int cols,
                             struct gui_control_info *controls_info, int num_controls,
                             GUICALLBACK *rtn, void *extra );
typedef char *(PICKGETTEXT)( const void *data_handle, int item );

typedef struct gui_create_info {
    char                *title;
    gui_rect            rect;
    gui_scroll_styles   scroll;
    gui_create_styles   style;
    gui_window          *parent;
    int                 num_menus;
    gui_menu_struct     *menu;
    int                 num_attrs;
    gui_colour_set      *colours;      /* array of num_attrs gui_attrs */
    GUICALLBACK         *call_back;
    void                *extra;
    gui_resource        *icon;
    int                 resource_menu;
} gui_create_info;

typedef enum {
    GUI_ABORT_RETRY_IGNORE      = 0x001,
    GUI_EXCLAMATION             = 0x002,
    GUI_INFORMATION             = 0x004,
    GUI_QUESTION                = 0x008,
    GUI_STOP                    = 0x010,
    GUI_OK                      = 0x020,
    GUI_OK_CANCEL               = 0x040,
    GUI_RETRY_CANCEL            = 0x080,
    GUI_YES_NO                  = 0x100,
    GUI_YES_NO_CANCEL           = 0x200,
    GUI_SYSTEMMODAL             = 0x400
} gui_message_type;

typedef enum {
    GUI_RET_ABORT,
    GUI_RET_CANCEL,
    GUI_RET_IGNORE,
    GUI_RET_NO,
    GUI_RET_OK,
    GUI_RET_RETRY,
    GUI_RET_YES
} gui_message_return;

typedef struct gui_text_metrics {
    gui_coord avg;
    gui_coord max;
} gui_text_metrics;

typedef struct gui_system_metrics {
    gui_coord   resize_border;
    bool        colour;
    bool        mouse;
    gui_coord   top_left;
    gui_coord   bottom_right;
    gui_coord   scrollbar_size;
    gui_coord   dialog_top_left_size;
    gui_coord   dialog_bottom_right_size;
    gui_ord     caption_size;
} gui_system_metrics;

typedef enum {
    GUI_TRACK_NONE      = 0x00,
    GUI_TRACK_LEFT      = 0x01,
    GUI_TRACK_RIGHT     = 0x02,
    GUI_TRACK_BOTH      = GUI_TRACK_LEFT | GUI_TRACK_RIGHT
} gui_mouse_track;

typedef enum {
    GUI_KS_NONE       =    0x00,
    GUI_KS_ALT        =    0x01,
    GUI_KS_SHIFT      =    0x02,
    GUI_KS_CTRL       =    0x04
} gui_keystate;

typedef enum {
    GUI_ARROW_CURSOR,
    GUI_HOURGLASS_CURSOR,
    GUI_CROSS_CURSOR
} gui_mouse_cursor;

typedef enum {
    GUI_NO_CURSOR,
    GUI_NORMAL_CURSOR,
    GUI_INSERT_CURSOR
} gui_char_cursor;

typedef enum {
    GUI_FRAME_TOP,
    GUI_FRAME_UL_CORNER,
    GUI_FRAME_LEFT,
    GUI_FRAME_LL_CORNER,
    GUI_FRAME_BOTTOM,
    GUI_FRAME_LR_CORNER,
    GUI_FRAME_RIGHT,
    GUI_FRAME_UR_CORNER,
    GUI_LR_VERT_BAR,
    GUI_LR_HORZ_BAR,
    GUI_LEFT_TITLE_MARK,
    GUI_RIGHT_TITLE_MARK,
    GUI_LEFT_GADGET_MARK,
    GUI_RIGHT_GADGET_MARK,
    GUI_TITLE_SPACE,
    GUI_CLOSER,
    GUI_MAXIMIZE_GADGET,
    GUI_MINIMIZE_GADGET,
    GUI_RESIZE_GADGET,
    GUI_HOR_SCROLL,
    GUI_VERT_SCROLL,
    GUI_LEFT_SCROLL_ARROW,
    GUI_RIGHT_SCROLL_ARROW,
    GUI_UP_SCROLL_ARROW,
    GUI_DOWN_SCROLL_ARROW,
    GUI_SCROLL_SLIDER,
    GUI_INACT_FRAME_TOP,
    GUI_INACT_FRAME_UL_CORNER,
    GUI_INACT_FRAME_LEFT,
    GUI_INACT_FRAME_LL_CORNER,
    GUI_INACT_FRAME_BOTTOM,
    GUI_INACT_FRAME_LR_CORNER,
    GUI_INACT_FRAME_RIGHT,
    GUI_INACT_FRAME_UR_CORNER,
    GUI_INACT_LR_VERT_BAR,
    GUI_INACT_LR_HORZ_BAR,
    GUI_INACT_LEFT_TITLE_MARK,
    GUI_INACT_RIGHT_TITLE_MARK,
    GUI_INACT_LEFT_GADGET_MARK,
    GUI_INACT_RIGHT_GADGET_MARK,
    GUI_INACT_TITLE_SPACE,
    GUI_INACT_CLOSER,
    GUI_INACT_MAXIMIZE_GADGET,
    GUI_INACT_MINIMIZE_GADGET,
    GUI_INACT_RESIZE_GADGET,
    GUI_INACT_HOR_SCROLL,
    GUI_INACT_VERT_SCROLL,
    GUI_INACT_LEFT_SCROLL_ARROW,
    GUI_INACT_RIGHT_SCROLL_ARROW,
    GUI_INACT_UP_SCROLL_ARROW,
    GUI_INACT_DOWN_SCROLL_ARROW,
    GUI_INACT_SCROLL_SLIDER,
    GUI_DIAL_VERT_SCROLL,
    GUI_DIAL_UP_SCROLL_ARROW,
    GUI_DIAL_DOWN_SCROLL_ARROW,
    GUI_DIAL_SCROLL_SLIDER,
    GUI_NUM_DRAW_CHARS,
    GUI_INACTIVE_OFFSET = GUI_INACT_FRAME_TOP
} gui_draw_char;

typedef enum {
    GUI_PLAIN           = 0x00,
    GUI_CHARMAP_MOUSE   = 0x01,
    GUI_INACT_GADGETS   = 0x02,
    GUI_INACT_SAME      = 0x04,
    GUI_CHARMAP_DLG     = 0x08,
    GUI_GMOUSE          = (GUI_CHARMAP_DLG|GUI_CHARMAP_MOUSE)
} gui_window_styles;

#define GUI_SHIFT( state )      ( state & GUI_KS_SHIFT )
#define GUI_ALT( state )        ( state & GUI_KS_ALT  )
#define GUI_CTRL( state )       ( state & GUI_KS_CTRL )

#define GUI_NO_COLUMN ((gui_ord)-1)
#define GUI_NO_ROW ((gui_ord)-1)
#define NO_SELECT -1

// GUIIsChecked and GUISetChecked values
#define GUI_NOT_CHECKED         0
#define GUI_CHECKED             1
#define GUI_INDETERMINANT       2

/*
 **************************************************************
 * GUI_MOUSEMOVE :
 * GUI_LBUTTONUDOWN :
 * GUI_LBUTTONUP :
 * GUI_LBUTTONDBLCLK :
 * GUI_RBUTTONUDOWN :
 * GUI_RBUTTONUP :
 * GUI_RBUTTONDBLCLK : 1 parameter - gui_point : GUI_GET_POINT
 **************************************************************
 * GUI_PAINT : 2 parameters - gui_ord, int : GUI_GET_ROWS
 **************************************************************
 * GUI_ENDSESSION : historical 1 parameter - bool : GUI_GET_BOOL
 * GUI_QUERYENDSESSION : 2 parameters - bool, long : GUI_GET_ENDSESSION
 **************************************************************
 * GUI_CONTROL_NOT_ACTIVE :
 * GUI_CLICKED :
 * GUI_CONTROL_CLICKED
 * GUI_CONTROL_DCLICKED : 1 parameter - unsigned : GUI_GETID
 **************************************************************
 * GUI_RESIZE : 1 parameter - gui_coord : GUI_GET_SIZE
 **************************************************************
 * GUI_SCROLL_VERTICAL :
 * GUI_SCROLL_HORIZONTAL : 1 parametr - int : GUI_GET_SCROLL
 **************************************************************
 *
 *
 */

typedef struct gui_row_num {
    gui_ord     start;
    int         num;
} gui_row_num;

typedef struct gui_end_session {
    bool        endsession;
    bool        logoff;
} gui_end_session;

typedef struct gui_timer_event {
    int id;
} gui_timer_event;

#define GUI_GET_TIMER( param, i ) { \
                                i = ((gui_timer_event *)param)->id; \
                            }

#define GUI_GET_BOOL( param, b ) {                                          \
                                b = ((gui_end_session *)param)->endsession; \
                            }

#define GUI_GET_ENDSESSION( param, b, l ) {                                 \
                                b = ((gui_end_session *)param)->endsession; \
                                l = ((gui_end_session *)param)->logoff;     \
                            }

#define GUI_GET_POINT( param, point ) ( point = *( gui_point * )param )

#define GUI_GET_ROWS( param, gui_start, gui_num ) {                          \
                                gui_start = (( gui_row_num * )param)->start; \
                                gui_num =   (( gui_row_num * )param)->num;   \
                                                   }

#define GUI_GETID( param, id ) ( id = ( unsigned short )(*( unsigned * )param ) )

#define GUI_GET_SIZE( param, size ) { size.x = (( gui_coord * )param)->x; \
                                      size.y = (( gui_coord * )param)->y; }

#define GUI_GET_SCROLL( param, scroll ) ( scroll = *( int * )param )

/* Initialization Functions */

extern bool GUIWndInit( unsigned rate, gui_window_styles style );
extern void GUIGMouseOn( void );
extern void GUIGMouseOff( void );
extern void GUIMDIInit( void );
extern void GUIMDIInitMenuOnly( void );
extern bool GUI3DDialogInit( void );
extern void GUI3DDialogFini( void );
extern void GUISetBetweenTitles( int between_titles );
extern void GUIGetRoundScale( gui_coord *scale );
extern void GUISetScale( gui_rect *screen );
extern void GUIGetScale( gui_rect *screen );
extern void GUIGetScreen( gui_rect *rect );
extern void GUISetDClickRate( unsigned rate );
extern void GUISetCharacter( gui_draw_char draw_char, int new_char );
extern int  GUIGetCharacter( gui_draw_char draw_char );
extern bool GUIIsInit( void );
extern void GUISetF10Menus( bool setting );
extern void GUICleanup( void );

/* colour functions */

extern void GUISetBackgroundColour( gui_colour_set *colour );
extern void GUIGetDialogColours( gui_colour_set *colours );
extern void GUISetDialogColours( gui_colour_set *colours );
extern void GUISetWindowColours( gui_window *wnd, int num_colours, gui_colour_set *colours );
extern bool GUISetRGB( gui_colour colour, gui_rgb rgb );
extern bool GUIGetRGB( gui_colour colour, gui_rgb *rgb );
extern bool GUIGetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set );
extern bool GUISetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set );
extern bool GUIGetRGBFromUser( gui_rgb init_rgb, gui_rgb *new_rgb );
extern bool GUIGetColourFromUser( char *title, gui_colour *init, gui_colour *new_colour );

/* Hot Spot (sizzle spot) functions */

extern bool GUIInitHotSpots( int num_hot_spots, gui_resource *hot );
extern int  GUIGetNumHotSpots( void );
extern bool GUIGetHotSpotSize( int hot_spot, gui_coord *size );
extern void GUIDrawHotSpot( gui_window *wnd, int hot_spot, gui_ord row,
                            gui_ord indent, gui_attr attr );

/* Window Functions */

extern gui_window *GUICreateWindow( gui_create_info *dlg_info );
extern int GUIGetNumWindowColours( gui_window *wnd );
extern gui_colour_set *GUIGetWindowColours( gui_window *wnd );
extern void GUIDestroyWnd( gui_window *wnd );
extern void GUIWndDirty( gui_window *wnd );
extern void GUIControlDirty( gui_window *wnd, unsigned id );
extern void GUIWndDirtyRow( gui_window *wnd, gui_ord row );
extern void GUIWndDirtyRect( gui_window *wnd, gui_rect *rect );
extern void GUIRefresh( void );
extern void GUIWndUpdate( gui_window *wnd );
extern bool GUISetBackgroundChar( gui_window *wnd, char background );

extern void GUIActivateNC( gui_window *wnd, bool activate );
extern void GUIBringToFront( gui_window *wnd );
extern gui_window *GUIGetFront( void );
extern gui_window *GUIGetParentWindow( gui_window *wnd );
extern bool GUIIsRootWindow( gui_window *wnd );
extern gui_window *GUIGetRootWindow( void );
extern gui_window *GUIGetNextWindow( gui_window *wnd );
extern gui_window *GUIGetFirstSibling( gui_window *wnd );
extern bool GUIIsValidWindow( gui_window *wnd );
extern bool GUISetFocus( gui_window *wnd, unsigned id );
extern bool GUIGetFocus( gui_window *wnd, unsigned *id );

extern bool GUIResizeWindow( gui_window *wnd, gui_rect *rect );
extern bool GUIIsMinimized( gui_window *wnd );
extern bool GUIIsMaximized( gui_window *wnd );
extern void GUIMaximizeWindow( gui_window *wnd );
extern void GUIMinimizeWindow( gui_window *wnd );
extern void GUIRestoreWindow( gui_window *wnd );
extern void GUIHideWindow( gui_window *wnd );
extern void GUIShowWindow( gui_window *wnd );
extern void GUIShowWindowNA( gui_window *wnd );
extern bool GUIIsWindowVisible( gui_window *wnd );
extern void GUISetRestoredSize( gui_window *wnd, gui_rect *rect );
extern bool GUIGetRestoredSize( gui_window *wnd, gui_rect *rect );
extern bool GUISetIcon( gui_window * wnd, gui_resource *res );
extern bool GUISetRedraw( gui_window *wnd, bool redraw );

extern bool GUICascadeWindows( void );
extern void GUIWantPartialRows( gui_window *wnd, bool want );
extern void GUISetCheckResizeAreaForChildren( gui_window *wnd, bool check );

/* Cursor Functions */

extern bool GUIGetCursorPos( gui_window *wnd, gui_point *point );
extern bool GUISetCursorPos( gui_window *wnd, gui_point *point );
extern bool GUIGetCursorType( gui_window *wnd, gui_char_cursor *cursor );
extern bool GUISetCursorType( gui_window *wnd, gui_char_cursor cursor );

extern void *GUISetMouseCursor( gui_mouse_cursor type );
extern void GUIResetMouseCursor( void* old_cursor );

/* Font Functions */

extern bool GUIFontsSupported( void );
extern bool GUIChangeFont( gui_window *wnd );
extern char *GUIGetFontInfo( gui_window *wnd );
extern bool GUISetFontInfo( gui_window *wnd, char *fontinfo );
extern bool GUISetSystemFont( gui_window *wnd, bool fixed );
extern char *GUIGetFontFromUser( char *fontinfo );

/* Painting functions */

extern bool GUIFillRect( gui_window *wnd, gui_rect *rect, gui_attr attr );
extern bool GUIFillBar( gui_window *wnd, gui_rect *rect, gui_attr attr );
extern bool GUIDrawRect( gui_window *wnd, gui_rect *rect, gui_attr attr );
extern bool GUIDrawLine( gui_window *wnd, gui_point *start, gui_point *end,
                         gui_line_styles style, gui_ord thickness, gui_attr attr );
extern bool GUIFillRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb );
extern bool GUIDrawRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb );
extern bool GUIDrawLineRGB( gui_window *wnd, gui_point *start, gui_point *end,
                         gui_line_styles style, gui_ord thickness, gui_rgb rgb );
extern void GUIDrawText( gui_window *wnd, const char *text, size_t length,
                         gui_ord row, gui_ord indent, gui_attr attr );
extern void GUIDrawTextPos( gui_window *wnd, const char *text, size_t length,
                            gui_coord *pos, gui_attr attr );
extern void GUIDrawTextExtent( gui_window *wnd, const char *text, size_t length,
                               gui_ord row, gui_ord indent, gui_attr attr,
                               gui_ord extentx );
extern void GUIDrawTextExtentPos( gui_window *wnd, const char *text, size_t length,
                               gui_coord *pos, gui_attr attr, gui_ord extentx );
extern void GUIDrawTextRGB( gui_window *wnd, const char *text, size_t length,
                            gui_ord row, gui_ord indent,
                            gui_rgb fore, gui_rgb back );
extern void GUIDrawTextPosRGB( gui_window *wnd, const char *text, size_t length,
                               gui_coord *pos, gui_rgb fore, gui_rgb back );
extern void GUIDrawTextExtentRGB( gui_window *wnd, const char *text, size_t length,
                                  gui_ord row, gui_ord indent,
                                  gui_rgb fore, gui_rgb back,
                                  gui_ord extentx );
extern void GUIDrawTextExtentPosRGB( gui_window *wnd, const char *text, size_t length,
                                     gui_coord *pos,
                                     gui_rgb fore, gui_rgb back,
                                     gui_ord extentx );
extern bool GUIDrawBar( gui_window *wnd, gui_ord row, gui_ord start,
                        gui_ord width, gui_bar_styles bstyle, gui_attr attr,
                        bool selected );
extern bool GUIDrawBarGroup( gui_window *wnd, gui_ord row, gui_ord start,
                        gui_ord width1, gui_ord width2, gui_bar_styles bstyle,
                        gui_attr attr1, gui_attr attr2, bool selected );

/* Text Functions */

extern bool GUISetWindowText( gui_window * wnd, const char * data );
extern size_t GUIGetWindowTextLength( gui_window *wnd );
extern size_t GUIGetWindowText( gui_window *wnd, char *data, size_t max_length );
extern gui_ord GUIGetRow( gui_window * wnd, gui_point *pos );
extern gui_ord GUIGetCol( gui_window * wnd, char *text, gui_point *pos );
extern gui_ord GUIGetStringPos( gui_window *wnd, gui_ord indent,
                                char * string, int mouse_x );
extern gui_ord GUIGetExtentX( gui_window *wnd, const char * text, size_t length );
extern gui_ord GUIGetExtentY( gui_window *wnd, const char * text );
extern gui_ord GUIGetControlExtentX( gui_window * wnd, unsigned id, const char * text, size_t length );
extern gui_ord GUIGetControlExtentY( gui_window * wnd, unsigned id, const char * text );
extern void GUIGetTextMetrics( gui_window *wnd, gui_text_metrics *metrics );
extern void GUIGetDlgTextMetrics( gui_text_metrics *metrics );
extern void GUIGetPoint( gui_window* wnd, gui_ord extent, gui_ord row,
                         gui_point *point );

/* Menu Functions */

extern bool GUICreateFloatingPopup( gui_window *wnd, gui_point *location,
                                    int num_menu_items, gui_menu_struct *menu,
                                    gui_mouse_track track, int *curr_item );
extern bool GUITrackFloatingPopup( gui_window *wnd, gui_point *location,
                               gui_mouse_track track, int *curr_item );
extern bool GUIEnableMenuItem( gui_window *wnd, int id, bool enabled, bool floating );
extern bool GUICheckMenuItem( gui_window *wnd, int id, bool check, bool floating );
extern bool GUISetMenuText( gui_window *wnd, int id, const char *text, bool floating );
extern bool GUISetHintText( gui_window *wnd, int id, const char *hinttext );

extern bool GUIEnableMDIMenus( bool enable );
extern bool GUIEnableMenus( gui_window *wnd, bool enable ); // NYI
extern bool GUIDeleteMenuItem( gui_window *wnd, int id, bool floating );

extern bool GUIResetMenus( gui_window *wnd, int num_menus, gui_menu_struct *menu );

extern int  GUIGetMenuPopupCount( gui_window *wnd, int id );

extern bool GUIInsertMenu( gui_window *wnd, int offset, gui_menu_struct *menu, bool floating );
extern bool GUIInsertMenuByID( gui_window *wnd, unsigned id, gui_menu_struct *menu );
extern bool GUIAppendMenu( gui_window *wnd, gui_menu_struct *menu, bool floating );
extern bool GUIAppendMenuByOffset( gui_window *wnd, int offset, gui_menu_struct *menu );
extern bool GUIAppendMenuToPopup( gui_window *wnd, unsigned id, gui_menu_struct *menu, bool floating );
extern bool GUIInsertMenuToPopup( gui_window *wnd, unsigned id, int offset, gui_menu_struct *menu, bool floating );

/* Toolbar Functions */

extern bool GUICreateFloatToolBar( gui_window *wnd, bool fixed,
                                   gui_ord height, int num_items,
                                   gui_toolbar_struct *toolbar, bool excl,
                                   gui_colour_set *plain,
                                   gui_colour_set *standout, gui_rect *rect );
extern bool GUICreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                              int num_items, gui_toolbar_struct *toolbar,
                              bool excl, gui_colour_set *plain,
                              gui_colour_set *standout );
extern bool GUICreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord height,
                                      int num_items, gui_toolbar_struct *toolbar,
                                      bool excl, gui_colour_set *plain,
                                      gui_colour_set *standout );
extern bool GUICloseToolBar( gui_window *wnd );
extern bool GUIHasToolBar( gui_window *wnd );
extern bool GUIChangeToolBar( gui_window *wnd );
extern bool GUIToolBarFixed( gui_window *wnd );

/* Status Window Functions */

extern bool GUICreateStatusWindow( gui_window *wnd, gui_ord x, gui_ord height,
                                   gui_colour_set *colour );
extern bool GUICloseStatusWindow( gui_window *wnd );
extern bool GUIHasStatus( gui_window *wnd );
extern bool GUIDrawStatusText( gui_window *wnd, const char *text );
extern bool GUIClearStatusText( gui_window *wnd );
extern bool GUIResizeStatusWindow( gui_window *wnd, gui_ord x, gui_ord height );

/* Help Functions */

// The new breed of GUI help
//                                             required for pm only
//                                                           |
//                                                           V
extern gui_help_instance GUIHelpInit( gui_window *wnd, char *file, char *title );
extern void GUIHelpFini( gui_help_instance inst, gui_window *wnd, char *file );
extern bool GUIShowHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act, char *file, char *topic );
extern bool GUIShowHtmlHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act, char *file, char *topic );

// the obsolete, crotchety old guard
// please use the above functions instead
extern bool GUIDisplayHelp( gui_window *wnd, char *file, char *topic );
extern bool GUIDisplayHelpWin4( gui_window *wnd, char *file, char *topic );
extern bool GUIDisplayHelpId( gui_window *wnd, char *file, int id );

/* Scroll Functions                            */
/* Init - set value, don't draw anything       */
/* Set  - set value, do scrolling indicated    */
/* Do   - just do scrolling, don't touch value */
/* Get  - just return current value            */
/* Thumb - does an init                        */

/* deals with character units */

extern void GUIInitHScrollCol( gui_window *wnd, int hscroll_pos );
extern void GUIInitVScrollRow( gui_window *wnd, int vscroll_pos );
extern void GUISetHScrollCol( gui_window *wnd, int hscroll_pos );
extern void GUISetVScrollRow( gui_window *wnd, int vscroll_pos );
extern int GUIGetHScrollCol( gui_window *wnd );
extern int GUIGetVScrollRow( gui_window *wnd );
extern void GUISetHScrollRangeCols( gui_window *wnd, gui_ord range );
extern void GUISetVScrollRangeRows( gui_window *wnd, gui_ord range );
extern gui_ord GUIGetHScrollRangeCols( gui_window *wnd );
extern gui_ord GUIGetVScrollRangeRows( gui_window *wnd );
extern void GUIDoHScroll( gui_window *wnd, int cols );
extern void GUIDoVScroll( gui_window *wnd, int rows );
extern void GUIDoHScrollClip( gui_window *wnd, int cols, int start, int end );
extern void GUIDoVScrollClip( gui_window *wnd, int rows, int start, int end );

/* deals in percent of range */

extern void GUISetHScrollThumb( gui_window * wnd, int percent );
extern void GUISetVScrollThumb( gui_window * wnd, int percent );

/* deals with user defined scale */

extern void GUIInitHScroll( gui_window *wnd, gui_ord hscroll_pos );
extern void GUIInitVScroll( gui_window *wnd, gui_ord vscroll_pos );
extern void GUISetHScroll( gui_window *wnd, gui_ord hscroll_pos );
extern void GUISetVScroll( gui_window *wnd, gui_ord vscroll_pos );
extern gui_ord GUIGetHScroll( gui_window *wnd );
extern gui_ord GUIGetVScroll( gui_window *wnd );
extern void GUISetHScrollRange( gui_window *wnd, gui_ord range );
extern void GUISetVScrollRange( gui_window *wnd, gui_ord range );
extern gui_ord GUIGetHScrollRange( gui_window *wnd );
extern gui_ord GUIGetVScrollRange( gui_window *wnd );
extern gui_ord GUIGetNumRows( gui_window *wnd );

/* Built in user interactions */

extern gui_message_return GUIDisplayMessage( gui_window *wnd,
                                             char *message, char *caption,
                                             gui_message_type type );
extern gui_message_return GUIGetNewVal( char *title, char *old, char **new_val );
extern int GUIDlgPick( char *text, PICKCALLBACK *pickinit );
extern int GUIDlgPickWithRtn( char *text, PICKCALLBACK *pickinit, PICKDLGOPEN * );

/* Dialog Functions */

extern bool GUICreateDialog( gui_create_info *dlg_info, int num_controls,
                             gui_control_info *controls_info );
extern bool GUICreateSysModalDialog( gui_create_info *dlg_info,
                                     int num_controls,
                                     gui_control_info *controls_info );
extern bool GUICreateResDialog( gui_create_info *dlg_info, long dlg_id );
extern bool GUICreateDialogFromRes( int id, gui_window *parent,
                                    GUICALLBACK cb, void *extra );
extern void GUICloseDialog( gui_window * wnd );

/* Control Functions */

extern bool GUIAddControl( gui_control_info *ctl_info, gui_colour_set *plain,
                           gui_colour_set *standout );
extern bool GUIDeleteControl( gui_window *wnd, unsigned id );
extern bool GUIResizeControl( gui_window *wnd, unsigned id, gui_rect *rect );
extern bool GUIEnableControl( gui_window *wnd, unsigned id, bool enable );
extern bool GUIIsControlEnabled( gui_window *wnd, unsigned id );
extern bool GUIGetControlRect( gui_window *wnd, unsigned id, gui_rect *rect );
extern bool GUIGetControlClass( gui_window *wnd, unsigned id, gui_control_class *control_class );
extern void GUIHideControl( gui_window *wnd, unsigned id );
extern void GUIShowControl( gui_window *wnd, unsigned id );
extern bool GUIIsControlVisible( gui_window *wnd, unsigned id );

/* combo/list box functions */
extern bool GUIControlSetRedraw( gui_window *wnd, unsigned control, bool redraw );
extern bool GUIAddText( gui_window *wnd, unsigned id, char *text );
extern bool GUISetListItemData( gui_window *wnd, unsigned id, unsigned choice, void *data );
extern void *GUIGetListItemData( gui_window *wnd, unsigned id, unsigned choice );
extern bool GUIAddTextList( gui_window *wnd, unsigned id, int items,
                            const void *data_handle, PICKGETTEXT *getstring );
extern bool GUIInsertText( gui_window *wnd, unsigned id, int choice, char *text );
extern bool GUISetTopIndex( gui_window *wnd, unsigned id, int choice );
extern int GUIGetTopIndex( gui_window *wnd, unsigned id );
extern bool GUISetHorizontalExtent( gui_window *wnd, unsigned id, int extent );
extern bool GUIClearList( gui_window *wnd, unsigned id );
extern bool GUIDeleteItem( gui_window *wnd, unsigned id, int choice );
extern int GUIGetListSize( gui_window *wnd, unsigned id );
extern int GUIGetCurrSelect( gui_window *wnd, unsigned id );
extern bool GUISetCurrSelect( gui_window *wnd, unsigned id, int choice );
extern char *GUIGetListItem( gui_window *wnd, unsigned id, int choice );

extern bool GUISetText( gui_window *wnd, unsigned id, const char *text );
extern bool GUIClearText( gui_window *wnd, unsigned id );
extern char * GUIGetText( gui_window *wnd, unsigned id );
extern bool GUISelectAll( gui_window *wnd, unsigned id, bool select );
extern bool GUISetEditSelect( gui_window *wnd, unsigned id, int start, int end );
extern bool GUIGetEditSelect( gui_window *wnd, unsigned id, int *start, int *end );
extern bool GUILimitEditText( gui_window *wnd, unsigned id, int len );
extern bool GUIDropDown( gui_window *wnd, unsigned id, bool drop );
extern void GUIScrollCaret( gui_window *wnd, unsigned id );

extern unsigned GUIIsChecked( gui_window *wnd, unsigned id );
extern bool GUISetChecked( gui_window *wnd, unsigned id, unsigned check );

/* Information Functions */

extern void GUIGetKeyState( gui_keystate *state );
extern void GUIFlushKeys( void );
extern void GUIDrainEvents( void );
extern void GUISetExtra( gui_window *wnd, void *extra );
extern void *GUIGetExtra( gui_window *wnd );
extern void GUIGetClientRect( gui_window *wnd, gui_rect *client );
extern bool GUIGetPaintRect( gui_window *wnd, gui_rect *paint );
extern void GUIGetAbsRect( gui_window *wnd, gui_rect *rect );
extern void GUIGetRect( gui_window *wnd, gui_rect *rect );
extern gui_scroll_styles GUIGetScrollStyle( gui_window *wnd );
extern gui_create_styles GUIGetCreateStyle( gui_window *wnd );
extern void GUITruncToPixel( gui_coord *coord );
extern bool GUIGetMousePosn( gui_window *wnd, gui_point *point );
extern void GUIGetSystemMetrics( gui_system_metrics *metrics );
extern bool GUIGetMinSize( gui_coord *size );
extern void GUIEnumChildWindows( gui_window *wnd, ENUMCALLBACK *func, void *param );
extern void GUIEnumControls( gui_window *wnd, CONTRENUMCALLBACK *func, void *param );
extern bool GUIIsGUI( void );
extern bool GUIGetArgs( char ***argv, int *argc );

/* Spawn functions */
extern void GUISpawnStart( void );
extern void GUISpawnEnd( void );

/* Resource String Functions */
extern bool GUILoadStrInit( const char *fname );
extern bool GUILoadStrFini( void );
extern bool GUILoadString( int string_id, char *buffer, int buffer_length );
extern bool GUIIsLoadStrInitialized( void );

/* Hooking the F1 key */
void GUIHookF1( void );
void GUIUnHookF1( void );

/* DBCS functions */
extern int GUICharLen( int );

bool GUIIsFirstInstance( void );

void GUIHookFileDlg( bool hook );

