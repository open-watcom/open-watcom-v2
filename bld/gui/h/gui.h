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
* Description:  Master GUI library include file.
*
****************************************************************************/


#include "bool.h"
#include "guimem.h"
#include "initmode.h"


#define GUIAPI                  /* public API */
#define GUIAPICALLBACK          /* public callback */

#define GUI_LAST_INTERNAL_MSG   255

#define GUI_LAST_MENU_ID        10000

#define GUI_ARRAY_SIZE( x )     (sizeof( x ) / sizeof( *x ))

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
    GUI_KEYDOWN,                /* keystroke messages */
    GUI_KEYUP,
    GUI_KEY_CONTROL,
    GUI_SCROLL_UP,              /* scrolling messages */
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
    GUI_CLICKED,                /* menu or control clicked */
    GUI_CONTROL_DCLICKED,       /* control double clicked */
    GUI_LBUTTONDOWN,            /* mouse messages */
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

typedef enum {
    GUI_BAD_CLASS = -1,
    #define pick(enumcls,uitype,classn,classn_os2,style,xstyle_nt) enumcls,
    #include "_guicont.h"
    #undef pick
    GUI_NUM_CONTROL_CLASSES
} gui_control_class;

typedef enum {
    #define pick(e,n)   e,
    #include "attrgui.h"
    #undef pick
    GUI_FIRST_UNUSED
} gui_attr;

#define GUI_NUM_ATTRS   GUI_FIRST_UNUSED

typedef enum {
    #define pick(e,f,b)   e,
    #include "attrdlg.h"
    #undef pick
    GUI_DLG_NUM_ATTRS
} gui_dlg_attr;

typedef enum {
    GUI_STYLE_MENU_ENABLED      = 0x00,
    GUI_STYLE_MENU_CHECKED      = 0x01,
    GUI_STYLE_MENU_GRAYED       = 0x02,
    GUI_STYLE_MENU_SEPARATOR    = 0x04,
    GUI_STYLE_MENU_IGNORE       = 0x08, // don't display
    GUI_STYLE_MENU_MDIWINDOW    = 0x10,
    GUI_STYLE_MENU_UTIL_1       = 0x40, // can be used by the application
    GUI_STYLE_MENU_UTIL_2       = 0x80
} gui_menu_styles ;

typedef enum {
    GUI_HELP_CONTENTS
,   GUI_HELP_ON_HELP
,   GUI_HELP_SEARCH
,   GUI_HELP_CONTEXT
,   GUI_HELP_KEY
} gui_help_actions;

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
    GUI_BR_BLUE,
    GUI_BR_GREEN,
    GUI_BR_CYAN,
    GUI_BR_RED,
    GUI_BR_MAGENTA,
    GUI_BR_YELLOW,
    GUI_BR_WHITE,
    GUIEX_DLG_BKGRND,
    GUIEX_WND_BKGRND,
    GUIEX_HIGHLIGHT,
    GUIEX_HIGHLIGHTTEXT,
    GUI_NUM_COLOURS,
} gui_colour;

#define GUIRGB(r,g,b)       ((gui_rgb)(((unsigned char)(r)) | (((unsigned short)(g))<<8) | (((unsigned long)(b))<<16)))

#define GUIGETRVALUE(rgb)   ((rgb) & 0xff)
#define GUIGETGVALUE(rgb)   (((rgb) & 0xff00 ) >> 8)
#define GUIGETBVALUE(rgb)   (((rgb) & 0xff0000) >> 16)

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
    GUI_STYLE_CONTROL_NOSTYLE           = 0x00000000,
    GUI_STYLE_CONTROL_CHECKED           = 0x00000001,
    GUI_STYLE_CONTROL_TAB_GROUP         = 0x00000002,
    GUI_STYLE_CONTROL_AUTOMATIC         = 0x00000004,
    GUI_STYLE_CONTROL_GROUP             = 0x00000008,
    GUI_STYLE_CONTROL_FOCUS             = 0x00000010,
    GUI_STYLE_CONTROL_INIT_INVISIBLE    = 0x00000020,
    GUI_STYLE_CONTROL_LEFTNOWORDWRAP    = 0x00000040,
    GUI_STYLE_CONTROL_NOPREFIX          = 0x00000080,
    GUI_STYLE_CONTROL_CENTRE            = 0x00000100,
    GUI_STYLE_CONTROL_NOINTEGRALHEIGHT  = 0x00000200,
    GUI_STYLE_CONTROL_SORTED            = 0x00000400,
    GUI_STYLE_CONTROL_MULTILINE         = 0x00000800,
    GUI_STYLE_CONTROL_WANTRETURN        = 0x00001000,
    GUI_STYLE_CONTROL_EDIT_INVISIBLE    = 0x00002000,
    GUI_STYLE_CONTROL_3STATE            = 0x00004000,
    GUI_STYLE_CONTROL_WANTKEYINPUT      = 0x00008000,
    GUI_STYLE_CONTROL_READONLY          = 0x00010000,
    GUI_STYLE_CONTROL_BORDER            = 0x00020000,
    GUI_STYLE_CONTROL_CHARCOORD         = 0x00040000,
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

typedef enum {
    GUI_TRACK_NONE      = 0x00,
    GUI_TRACK_LEFT      = 0x01,
    GUI_TRACK_RIGHT     = 0x02,
    GUI_TRACK_BOTH      = GUI_TRACK_LEFT | GUI_TRACK_RIGHT
} gui_mouse_track;

typedef enum {
    GUI_KS_NONE         = 0x00,
    GUI_KS_ALT          = 0x01,
    GUI_KS_SHIFT        = 0x02,
    GUI_KS_CTRL         = 0x04
} gui_keystate;

typedef enum {
    GUI_ARROW_CURSOR,
    GUI_HOURGLASS_CURSOR,
    GUI_CROSS_CURSOR
} gui_mcursor_type;

typedef enum {
    GUI_NO_CURSOR,
    GUI_NORMAL_CURSOR,
    GUI_INSERT_CURSOR
} gui_cursor_type;

typedef enum {
    /* active state                inactive state */
    GUI_FRAME_TOP,              GUI_INACT_FRAME_TOP,
    GUI_FRAME_UL_CORNER,        GUI_INACT_FRAME_UL_CORNER,
    GUI_FRAME_LEFT,             GUI_INACT_FRAME_LEFT,
    GUI_FRAME_LL_CORNER,        GUI_INACT_FRAME_LL_CORNER,
    GUI_FRAME_BOTTOM,           GUI_INACT_FRAME_BOTTOM,
    GUI_FRAME_LR_CORNER,        GUI_INACT_FRAME_LR_CORNER,
    GUI_FRAME_RIGHT,            GUI_INACT_FRAME_RIGHT,
    GUI_FRAME_UR_CORNER,        GUI_INACT_FRAME_UR_CORNER,
    GUI_LR_VERT_BAR,            GUI_INACT_LR_VERT_BAR,
    GUI_LR_HORZ_BAR,            GUI_INACT_LR_HORZ_BAR,
    GUI_LEFT_TITLE_MARK,        GUI_INACT_LEFT_TITLE_MARK,
    GUI_RIGHT_TITLE_MARK,       GUI_INACT_RIGHT_TITLE_MARK,
    GUI_LEFT_GADGET_MARK,       GUI_INACT_LEFT_GADGET_MARK,
    GUI_RIGHT_GADGET_MARK,      GUI_INACT_RIGHT_GADGET_MARK,
    GUI_TITLE_SPACE,            GUI_INACT_TITLE_SPACE,
    GUI_CLOSER,                 GUI_INACT_CLOSER,
    GUI_MAXIMIZE_GADGET,        GUI_INACT_MAXIMIZE_GADGET,
    GUI_MINIMIZE_GADGET,        GUI_INACT_MINIMIZE_GADGET,
    GUI_RESIZE_GADGET,          GUI_INACT_RESIZE_GADGET,
    GUI_HOR_SCROLL,             GUI_INACT_HOR_SCROLL,
    GUI_VERT_SCROLL,            GUI_INACT_VERT_SCROLL,
    GUI_LEFT_SCROLL_ARROW,      GUI_INACT_LEFT_SCROLL_ARROW,
    GUI_RIGHT_SCROLL_ARROW,     GUI_INACT_RIGHT_SCROLL_ARROW,
    GUI_UP_SCROLL_ARROW,        GUI_INACT_UP_SCROLL_ARROW,
    GUI_DOWN_SCROLL_ARROW,      GUI_INACT_DOWN_SCROLL_ARROW,
    GUI_SCROLL_SLIDER,          GUI_INACT_SCROLL_SLIDER,
    /* no state */
    GUI_DIAL_VERT_SCROLL,
    GUI_DIAL_UP_SCROLL_ARROW,
    GUI_DIAL_DOWN_SCROLL_ARROW,
    GUI_DIAL_SCROLL_SLIDER,
    GUI_RECT_AREA,
    GUI_BAR_AREA,
    GUI_LINE_VERT,
    GUI_LINE_HOR,
    GUI_BOX_LEFT,
    GUI_BOX_RIGHT,

    GUI_NUM_DRAW_CHARS,
} gui_draw_char;

typedef enum {
    GUI_PLAIN           = 0x00,
    GUI_CHARMAP_MOUSE   = 0x01,
    GUI_INACT_GADGETS   = 0x02,
    GUI_INACT_SAME      = 0x04,
    GUI_CHARMAP_DLG     = 0x08,
    GUI_GMOUSE          = (GUI_CHARMAP_DLG|GUI_CHARMAP_MOUSE)
} gui_window_styles;

#define GUI_SHIFT_STATE( state )    ((state & GUI_KS_SHIFT) != 0)
#define GUI_ALT_STATE( state )      ((state & GUI_KS_ALT) != 0)
#define GUI_CTRL_STATE( state )     ((state & GUI_KS_CTRL) != 0)

#define GUI_NO_COLUMN       ((gui_ord)-1)
#define GUI_NO_ROW          ((gui_ord)-1)

#define GUI_TEXT_NO_COLUMN  ((gui_text_ord)-1)
#define GUI_TEXT_NO_ROW     ((gui_text_ord)-1)

// GUIIsChecked and GUISetChecked values
#define GUI_NOT_CHECKED     0
#define GUI_CHECKED         1
#define GUI_INDETERMINANT   2

typedef int                 gui_ord;

typedef unsigned short      gui_text_ord;

typedef unsigned            gui_ctl_id;
typedef unsigned            gui_res_id;
typedef unsigned            gui_hlp_id;

#if defined( _M_I86 )
typedef const char __far    *res_name_or_id;
#else
typedef const char          *res_name_or_id;
#endif

typedef struct gui_coord {
    gui_ord             x;
    gui_ord             y;
} gui_coord;

typedef struct gui_point {
    int                 x;
    int                 y;
} gui_point;

typedef struct gui_window   gui_window;

typedef struct gui_menu_struct  gui_menu_struct;

typedef void                *gui_help_instance;

typedef struct gui_toolbar_struct {
    const char              *label;
    gui_res_id              bitmap_id;
    gui_ctl_id              id;
    const char              *hinttext;
    const char              *tip;
} gui_toolbar_struct;

typedef struct gui_toolbar_items {
    int                     num_items;
    gui_toolbar_struct      *toolbar;
} gui_toolbar_items;

#define GUI_NO_TOOLBAR          {0, NULL}
#define GUI_TOOLBAR_ARRAY(x)    {sizeof( x ) / sizeof( *x ), x}

typedef struct gui_menu_items {
    int                     num_items;
    gui_menu_struct         *menu;
} gui_menu_items;

#define GUI_NO_MENU         {0, NULL}
#define GUI_MENU_ARRAY(x)   {sizeof( x ) / sizeof( *x ), x}

typedef struct gui_menu_struct {
    const char              *label;
    gui_ctl_id              id;
    gui_menu_styles         style;
    const char              *hinttext;
    gui_menu_items          child;
} gui_menu_struct;

typedef struct gui_colour_set {
    gui_colour              fore;
    gui_colour              back;
} gui_colour_set;

typedef struct gui_colour_items {
    int                     num_items;
    gui_colour_set          *colour;
} gui_colour_items;

#define GUI_NO_COLOUR       {0, NULL}
#define GUI_COLOUR_ARRAY(x) {sizeof( x ) / sizeof( *x ), x}

typedef unsigned long       gui_rgb;

typedef struct gui_rect {
    gui_ord                 x;
    gui_ord                 y;
    gui_ord                 width;
    gui_ord                 height;
} gui_rect;

typedef struct gui_resource {
    gui_res_id              res_id;
    char                    *chars; /* Character based */
} gui_resource;

typedef struct gui_control_info {
    gui_control_class       control_class;
    const char              *text;
    gui_rect                rect;
    gui_window              *parent;
    gui_scroll_styles       scroll_style;
    gui_control_styles      style;
    gui_ctl_id              id;
} gui_control_info;

typedef bool (GUIAPICALLBACK GUICALLBACK)( gui_window *wnd, gui_event gui_ev, void *param );
typedef void (GUIAPICALLBACK ENUMCALLBACK)( gui_window *wnd, void *param );
typedef void (GUIAPICALLBACK CONTRENUMCALLBACK)( gui_window *parent_wnd, gui_ctl_id id, void *param );
typedef void (GUIAPICALLBACK GUIPICKCALLBACK)( gui_window *wnd, gui_ctl_id id );
typedef void (GUIAPICALLBACK PICKDLGOPEN)( const char *title, gui_text_ord rows, gui_text_ord cols,
                             gui_control_info *controls_info, int num_controls,
                             GUICALLBACK *gui_call_back, void *extra );
typedef const char *(GUIAPICALLBACK GUIPICKGETTEXT)( const void *data_handle, int item );

typedef struct gui_create_info {
    const char          *title;
    gui_rect            rect;
    gui_scroll_styles   scroll_style;
    gui_create_styles   style;
    gui_window          *parent;
    gui_menu_items      menus;
    gui_colour_items    colours;
    GUICALLBACK         *gui_call_back;
    void                *extra;
    gui_resource        *icon;
    res_name_or_id      resource_menu;
} gui_create_info;

typedef struct gui_text_metrics {
    gui_coord   avg;
    gui_coord   max;
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

typedef void            *gui_mcursor_handle;

/*
 *************************************************************************
 * GUI_MOUSEMOVE:
 * GUI_LBUTTONUDOWN:
 * GUI_LBUTTONUP:
 * GUI_LBUTTONDBLCLK:
 * GUI_RBUTTONUDOWN:
 * GUI_RBUTTONUP:
 * GUI_RBUTTONDBLCLK: 1 parameter - gui_point : GUI_GET_POINT
 *************************************************************************
 * GUI_PAINT: 2 parameters - gui_text_ord, gui_text_ord : GUI_GET_ROWS
 *************************************************************************
 * GUI_ENDSESSION:
 * GUI_QUERYENDSESSION: 2 parameters - bool, bool : GUI_GET_ENDSESSION
 *************************************************************************
 * GUI_CONTROL_NOT_ACTIVE:
 * GUI_CLICKED:
 * GUI_CONTROL_CLICKED:
 * GUI_CONTROL_DCLICKED: 1 parameter - gui_ctl_id : GUI_GETID
 *************************************************************************
 * GUI_RESIZE: 1 parameter - gui_coord : GUI_GET_SIZE
 *************************************************************************
 * GUI_SCROLL_VERTICAL:
 * GUI_SCROLL_HORIZONTAL: 1 parametr - int : GUI_GET_SCROLL
 *************************************************************************
 * GUI_ACTIVATEAPP:
 * GUI_CONTEXTHELP: 1 parametr - bool : GUI_GET_BOOL
 *************************************************************************
 *
 *
 */

typedef struct gui_row_num {
    gui_text_ord    start;
    gui_text_ord    num;
} gui_row_num;

typedef struct gui_end_session {
    bool        endsession;
    bool        logoff;
} gui_end_session;

#if defined( _WIN64 )
typedef unsigned __int64    gui_timer_id;
#else
typedef unsigned long       gui_timer_id;
#endif

typedef struct gui_timer_event {
    gui_timer_id    id;
} gui_timer_event;

#define GUI_GET_TIMER( param, i )   ( i = ((gui_timer_event *)param)->id )

#define GUI_GET_BOOL( param, b )    ( b = *(bool *)param )

#define GUI_GET_ENDSESSION( param, b, l )           \
    {                                               \
        b = ((gui_end_session *)param)->endsession; \
        l = ((gui_end_session *)param)->logoff;     \
    }

#define GUI_GET_POINT( param, point ) ( point = *(gui_point *)param )

#define GUI_GET_ROWS( param, gui_start, gui_num )   \
    {                                               \
        gui_start = ((gui_row_num *)param)->start;  \
        gui_num =   ((gui_row_num *)param)->num;    \
    }

#define GUI_GETID( param, id ) ( id = *(gui_ctl_id *)param )

#define GUI_GET_SIZE( param, size )         \
    {                                       \
        size.x = ((gui_coord *)param)->x;   \
        size.y = ((gui_coord *)param)->y;   \
    }

#define GUI_GET_SCROLL( param, scroll ) ( scroll = *(int *)param )

/* Initialization Functions */

extern bool     GUIAPI GUIWndInit( unsigned dclick_ms, gui_window_styles style );
extern void     GUIAPI GUIGMouseOn( void );
extern void     GUIAPI GUIGMouseOff( void );
extern void     GUIAPI GUIMDIInit( void );
extern void     GUIAPI GUIMDIInitMenuOnly( void );
extern bool     GUIAPI GUI3DDialogInit( void );
extern void     GUIAPI GUI3DDialogFini( void );
extern void     GUIAPI GUISetBetweenTitles( int between_titles );
extern void     GUIAPI GUIGetRoundScale( gui_coord *scale );
extern void     GUIAPI GUISetScale( const gui_rect *scale );
extern void     GUIAPI GUIGetScale( gui_rect *scale );
extern void     GUIAPI GUIGetScreen( gui_rect *screen );
extern void     GUIAPI GUISetDClickRate( unsigned dclick_ms );
extern void     GUIAPI GUISetCharacter( gui_draw_char draw_char, int new_char );
extern int      GUIAPI GUIGetCharacter( gui_draw_char draw_char );
extern bool     GUIAPI GUIIsInit( void );
extern void     GUIAPI GUISetF10Menus( bool setting );
extern void     GUIAPI GUICleanup( void );

/* colour functions */

extern void     GUIAPI GUISetBackgroundColour( gui_colour_set *colour );
extern void     GUIAPI GUIGetDialogColours( gui_colour_set *colours );
extern void     GUIAPI GUISetDialogColours( gui_colour_set *colours );
extern void     GUIAPI GUISetWindowColours( gui_window *wnd, int num_colours, gui_colour_set *colours );
extern bool     GUIAPI GUISetRGB( gui_colour colour, gui_rgb rgb );
extern bool     GUIAPI GUIGetRGB( gui_colour colour, gui_rgb *rgb );
extern bool     GUIAPI GUIGetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set );
extern bool     GUIAPI GUISetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set );
extern bool     GUIAPI GUIGetRGBFromUser( gui_rgb init_rgb, gui_rgb *new_rgb );
extern bool     GUIAPI GUIGetColourFromUser( const char *title, gui_colour *init, gui_colour *new_colour );

/* Hot Spot (sizzle spot) functions */

extern bool     GUIAPI GUIInitHotSpots( int num_hot_spots, gui_resource *hot );
extern int      GUIAPI GUIGetNumHotSpots( void );
extern bool     GUIAPI GUIGetHotSpotSize( int hotspot_no, gui_coord *size );
extern void     GUIAPI GUIDrawHotSpot( gui_window *wnd, int hotspot_no, gui_text_ord row, gui_ord indent, gui_attr attr );

/* Window Functions */

extern gui_window * GUIAPI GUICreateWindow( gui_create_info *dlg_info );
extern int      GUIAPI GUIGetNumWindowColours( gui_window *wnd );
extern gui_colour_set * GUIAPI GUIGetWindowColours( gui_window *wnd );
extern void     GUIAPI GUIDestroyWnd( gui_window *wnd );
extern void     GUIAPI GUIWndDirty( gui_window *wnd );
extern void     GUIAPI GUIWndDirtyControl( gui_window *wnd, gui_ctl_id id );
extern void     GUIAPI GUIWndDirtyRow( gui_window *wnd, gui_text_ord row );
extern void     GUIAPI GUIWndDirtyRect( gui_window *wnd, const gui_rect *rect );
extern void     GUIAPI GUIRefresh( void );
extern void     GUIAPI GUIWndUpdate( gui_window *wnd );
extern bool     GUIAPI GUISetBackgroundChar( gui_window *wnd, char background );

extern void     GUIAPI GUIActivateNC( gui_window *wnd, bool activate );
extern void     GUIAPI GUIBringToFront( gui_window *wnd );
extern gui_window * GUIAPI GUIGetFront( void );
extern gui_window * GUIAPI GUIGetParentWindow( gui_window *wnd );
extern bool     GUIAPI GUIIsRootWindow( gui_window *wnd );
extern gui_window * GUIAPI GUIGetRootWindow( void );
extern gui_window * GUIAPI GUIGetNextWindow( gui_window *wnd );
extern gui_window * GUIAPI GUIGetFirstSibling( gui_window *wnd );
extern bool     GUIAPI GUIIsValidWindow( gui_window *wnd );
extern bool     GUIAPI GUISetFocus( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUIGetFocus( gui_window *wnd, gui_ctl_id *id );

extern bool     GUIAPI GUIResizeWindow( gui_window *wnd, const gui_rect *rect );
extern bool     GUIAPI GUIIsMinimized( gui_window *wnd );
extern bool     GUIAPI GUIIsMaximized( gui_window *wnd );
extern void     GUIAPI GUIMaximizeWindow( gui_window *wnd );
extern void     GUIAPI GUIMinimizeWindow( gui_window *wnd );
extern void     GUIAPI GUIRestoreWindow( gui_window *wnd );
extern void     GUIAPI GUIHideWindow( gui_window *wnd );
extern void     GUIAPI GUIShowWindow( gui_window *wnd );
extern void     GUIAPI GUIShowWindowNA( gui_window *wnd );
extern bool     GUIAPI GUIIsWindowVisible( gui_window *wnd );
extern void     GUIAPI GUISetRestoredSize( gui_window *wnd, const gui_rect *rect );
extern bool     GUIAPI GUIGetRestoredSize( gui_window *wnd, gui_rect *rect );
extern bool     GUIAPI GUISetIcon( gui_window *wnd, gui_resource *res );
extern bool     GUIAPI GUISetRedraw( gui_window *wnd, bool redraw );

extern bool     GUIAPI GUICascadeWindows( void );
extern void     GUIAPI GUIWantPartialRows( gui_window *wnd, bool want );
extern void     GUIAPI GUISetCheckResizeAreaForChildren( gui_window *wnd, bool check );

/* Cursor Functions */

extern bool     GUIAPI GUIGetCursorPos( gui_window *wnd, gui_point *point );
extern bool     GUIAPI GUISetCursorPos( gui_window *wnd, const gui_point *point );
extern bool     GUIAPI GUIGetCursorType( gui_window *wnd, gui_cursor_type *cursor );
extern bool     GUIAPI GUISetCursorType( gui_window *wnd, gui_cursor_type cursor );

extern gui_mcursor_handle GUIAPI GUISetMouseCursor( gui_mcursor_type type );
extern void     GUIAPI GUIResetMouseCursor( gui_mcursor_handle old_cursor );

/* Font Functions */

extern bool     GUIAPI GUIFontsSupported( void );
extern bool     GUIAPI GUIChangeFont( gui_window *wnd );
extern char     * GUIAPI GUIGetFontInfo( gui_window *wnd );
extern bool     GUIAPI GUISetFontInfo( gui_window *wnd, char *fontinfo );
extern bool     GUIAPI GUISetSystemFont( gui_window *wnd, bool fixed );
extern char     * GUIAPI GUIGetFontFromUser( char *fontinfo );

/* Painting functions */

extern bool     GUIAPI GUIFillRect( gui_window *wnd, const gui_rect *rect, gui_attr attr );
extern bool     GUIAPI GUIFillBar( gui_window *wnd, const gui_rect *rect, gui_attr attr );
extern bool     GUIAPI GUIDrawRect( gui_window *wnd, const gui_rect *rect, gui_attr attr );
extern bool     GUIAPI GUIDrawLine( gui_window *wnd, const gui_point *start, const gui_point *end,
                             gui_line_styles style, gui_ord thickness, gui_attr attr );
extern bool     GUIAPI GUIFillRectRGB( gui_window *wnd, const gui_rect *rect, gui_rgb rgb );
extern bool     GUIAPI GUIDrawRectRGB( gui_window *wnd, const gui_rect *rect, gui_rgb rgb );
extern bool     GUIAPI GUIDrawLineRGB( gui_window *wnd, const gui_point *start, const gui_point *end,
                             gui_line_styles style, gui_ord thickness, gui_rgb rgb );
extern void     GUIAPI GUIDrawText( gui_window *wnd, const char *text, size_t length,
                             gui_text_ord row, gui_ord indent, gui_attr attr );
extern void     GUIAPI GUIDrawTextPos( gui_window *wnd, const char *text, size_t length,
                                const gui_coord *pos, gui_attr attr );
extern void     GUIAPI GUIDrawTextExtent( gui_window *wnd, const char *text, size_t length,
                                   gui_text_ord row, gui_ord indent, gui_attr attr,
                                   gui_ord extentx );
extern void     GUIAPI GUIDrawTextExtentPos( gui_window *wnd, const char *text, size_t length,
                                   const gui_coord *pos, gui_attr attr, gui_ord extentx );
extern void     GUIAPI GUIDrawTextRGB( gui_window *wnd, const char *text, size_t length,
                                gui_text_ord row, gui_ord indent, gui_rgb fore, gui_rgb back );
extern void     GUIAPI GUIDrawTextPosRGB( gui_window *wnd, const char *text, size_t length,
                                   const gui_coord *pos, gui_rgb fore, gui_rgb back );
extern void     GUIAPI GUIDrawTextExtentRGB( gui_window *wnd, const char *text, size_t length,
                                      gui_text_ord row, gui_ord indent,
                                      gui_rgb fore, gui_rgb back, gui_ord extentx );
extern void     GUIAPI GUIDrawTextExtentPosRGB( gui_window *wnd, const char *text, size_t length,
                                         const gui_coord *pos,
                                         gui_rgb fore, gui_rgb back, gui_ord extentx );
extern bool     GUIAPI GUIDrawBar( gui_window *wnd, gui_text_ord row, gui_ord start,
                            gui_ord width, gui_bar_styles bstyle, gui_attr attr,
                            bool selected );
extern bool     GUIAPI GUIDrawBarGroup( gui_window *wnd, gui_text_ord row, gui_ord start,
                            gui_ord width1, gui_ord width2, gui_bar_styles bstyle,
                            gui_attr attr1, gui_attr attr2, bool selected );

/* Text Functions */

extern bool     GUIAPI GUISetWindowText( gui_window *wnd, const char *data );
extern size_t   GUIAPI GUIGetWindowTextLength( gui_window *wnd );
extern size_t   GUIAPI GUIGetWindowText( gui_window *wnd, char *buff, size_t buff_len );
extern gui_text_ord GUIAPI GUIGetRow( gui_window *wnd, const gui_point *pos );
extern gui_text_ord GUIAPI GUIGetCol( gui_window *wnd, const char *text, const gui_point *pos );
extern gui_text_ord GUIAPI GUIGetStringPos( gui_window *wnd, gui_ord indent, const char *string, gui_ord mouse_x );
extern gui_ord  GUIAPI GUIGetExtentX( gui_window *wnd, const char *text, size_t length );
extern gui_ord  GUIAPI GUIGetExtentY( gui_window *wnd, const char *text );
extern gui_ord  GUIAPI GUIGetControlExtentX( gui_window *wnd, gui_ctl_id id, const char *text, size_t length );
extern gui_ord  GUIAPI GUIGetControlExtentY( gui_window *wnd, gui_ctl_id id, const char *text );
extern void     GUIAPI GUIGetTextMetrics( gui_window *wnd, gui_text_metrics *metrics );
extern void     GUIAPI GUIGetDlgTextMetrics( gui_text_metrics *metrics );
extern void     GUIAPI GUIGetMaxDialogSize( gui_coord *size );
extern void     GUIAPI GUIGetPoint( gui_window *wnd, gui_ord extentx, gui_text_ord row, gui_point *point );

/* Menu Functions */

extern const gui_menu_items     NoMenu;

extern bool     GUIAPI GUICreateFloatingPopup( gui_window *wnd, const gui_point *location,
                                        const gui_menu_items *menus,
                                        gui_mouse_track track, gui_ctl_id *curr_id );
extern bool     GUIAPI GUITrackFloatingPopup( gui_window *wnd, const gui_point *location,
                                   gui_mouse_track track, gui_ctl_id *curr_id );
extern bool     GUIAPI GUIEnableMenuItem( gui_window *wnd, gui_ctl_id id, bool enabled, bool floating );
extern bool     GUIAPI GUICheckMenuItem( gui_window *wnd, gui_ctl_id id, bool check, bool floating );
extern bool     GUIAPI GUISetMenuText( gui_window *wnd, gui_ctl_id id, const char *text, bool floating );
extern bool     GUIAPI GUISetMenuHintText( gui_window *wnd, gui_ctl_id id, const char *hinttext );

extern bool     GUIAPI GUIEnableMDIMenus( bool enable );
extern bool     GUIAPI GUIEnableMenus( gui_window *wnd, bool enable ); // NYI
extern bool     GUIAPI GUIDeleteMenuItem( gui_window *wnd, gui_ctl_id id, bool floating );

extern bool     GUIAPI GUIResetMenus( gui_window *wnd, const gui_menu_items *menus );

extern int      GUIAPI GUIGetMenuPopupCount( gui_window *wnd, gui_ctl_id id );

extern bool     GUIAPI GUIInsertMenuByIdx( gui_window *wnd, int position, const gui_menu_struct *menu, bool floating );
extern bool     GUIAPI GUIInsertMenuByID( gui_window *wnd, gui_ctl_id id, const gui_menu_struct *menu );
extern bool     GUIAPI GUIAppendMenu( gui_window *wnd, const gui_menu_struct *menu, bool floating );
extern bool     GUIAPI GUIAppendMenuByIdx( gui_window *wnd, int position, const gui_menu_struct *menu );
extern bool     GUIAPI GUIAppendMenuToPopup( gui_window *wnd, gui_ctl_id id, const gui_menu_struct *menu, bool floating );
extern bool     GUIAPI GUIInsertMenuToPopup( gui_window *wnd, gui_ctl_id id, int position, const gui_menu_struct *menu, bool floating );

/* Toolbar Functions */

extern const gui_toolbar_items  NoToolbar;

extern bool     GUIAPI GUICreateFloatToolBar( gui_window *wnd, bool fixed, gui_ord height,
                                        const gui_toolbar_items *toolinfo,
                                        bool excl, gui_colour_set *plain,
                                        gui_colour_set *standout, const gui_rect *float_pos );
extern bool     GUIAPI GUICreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                                  const gui_toolbar_items *toolinfo,
                                  bool excl, gui_colour_set *plain,
                                  gui_colour_set *standout );
extern bool     GUIAPI GUICreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord height,
                                          const gui_toolbar_items *toolinfo,
                                          bool excl, gui_colour_set *plain,
                                          gui_colour_set *standout );
extern bool     GUIAPI GUICloseToolBar( gui_window *wnd );
extern bool     GUIAPI GUIHasToolBar( gui_window *wnd );
extern bool     GUIAPI GUIChangeToolBar( gui_window *wnd );
extern bool     GUIAPI GUIToolBarFixed( gui_window *wnd );

/* Status Window Functions */

extern bool     GUIAPI GUICreateStatusWindow( gui_window *wnd, gui_ord x, gui_ord height, gui_colour_set *colour );
extern bool     GUIAPI GUICloseStatusWindow( gui_window *wnd );
extern bool     GUIAPI GUIHasStatus( gui_window *wnd );
extern bool     GUIAPI GUIDrawStatusText( gui_window *wnd, const char *text );
extern bool     GUIAPI GUIClearStatusText( gui_window *wnd );
extern bool     GUIAPI GUIResizeStatusWindow( gui_window *wnd, gui_ord x, gui_ord height );

/* Help Functions */

// The new breed of GUI help
//                                             required for pm only
//                                                           |
//                                                           V
extern gui_help_instance GUIAPI GUIHelpInit( gui_window *wnd, const char *file, const char *title );
extern void     GUIAPI GUIHelpFini( gui_help_instance inst, gui_window *wnd, const char *file );
extern bool     GUIAPI GUIShowHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act, const char *file, const char *topic );
extern bool     GUIAPI GUIShowHtmlHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act, const char *file, const char *topic );

// the obsolete, crotchety old guard
// please use the above functions instead
extern bool     GUIAPI GUIDisplayHelp( gui_window *wnd, const char *file, const char *topic );
extern bool     GUIAPI GUIDisplayHelpWin4( gui_window *wnd, const char *file, const char *topic );
extern bool     GUIAPI GUIDisplayHelpId( gui_window *wnd, const char *file, gui_hlp_id id );

/* Scroll Functions                            */
/* Init - set value, don't draw anything       */
/* Set  - set value, do scrolling indicated    */
/* Do   - just do scrolling, don't touch value */
/* Get  - just return current value            */
/* Thumb - does an init                        */

/* deals with character units */

extern void     GUIAPI GUIInitHScrollCol( gui_window *wnd, gui_text_ord hscroll_pos );
extern void     GUIAPI GUIInitVScrollRow( gui_window *wnd, gui_text_ord vscroll_pos );
extern void     GUIAPI GUISetHScrollCol( gui_window *wnd, gui_text_ord hscroll_pos );
extern void     GUIAPI GUISetVScrollRow( gui_window *wnd, gui_text_ord vscroll_pos );
extern gui_text_ord GUIAPI GUIGetHScrollCol( gui_window *wnd );
extern gui_text_ord GUIAPI GUIGetVScrollRow( gui_window *wnd );
extern void     GUIAPI GUISetHScrollRangeCols( gui_window *wnd, gui_text_ord range );
extern void     GUIAPI GUISetVScrollRangeRows( gui_window *wnd, gui_text_ord range );
extern gui_text_ord GUIAPI GUIGetHScrollRangeCols( gui_window *wnd );
extern gui_text_ord GUIAPI GUIGetVScrollRangeRows( gui_window *wnd );
extern void     GUIAPI GUIDoHScroll( gui_window *wnd, int cols );
extern void     GUIAPI GUIDoVScroll( gui_window *wnd, int rows );
extern void     GUIAPI GUIDoHScrollClip( gui_window *wnd, int cols, int start, int end );
extern void     GUIAPI GUIDoVScrollClip( gui_window *wnd, int rows, int start, int end );

/* deals in percent of range */

extern void     GUIAPI GUISetHScrollThumb( gui_window *wnd, int percent );
extern void     GUIAPI GUISetVScrollThumb( gui_window *wnd, int percent );

/* deals with user defined scale */

extern void     GUIAPI GUIInitHScroll( gui_window *wnd, gui_ord hscroll_pos );
extern void     GUIAPI GUIInitVScroll( gui_window *wnd, gui_ord vscroll_pos );
extern void     GUIAPI GUISetHScroll( gui_window *wnd, gui_ord hscroll_pos );
extern void     GUIAPI GUISetVScroll( gui_window *wnd, gui_ord vscroll_pos );
extern gui_ord  GUIAPI GUIGetHScroll( gui_window *wnd );
extern gui_ord  GUIAPI GUIGetVScroll( gui_window *wnd );
extern void     GUIAPI GUISetHScrollRange( gui_window *wnd, gui_ord range );
extern void     GUIAPI GUISetVScrollRange( gui_window *wnd, gui_ord range );
extern gui_ord  GUIAPI GUIGetHScrollRange( gui_window *wnd );
extern gui_ord  GUIAPI GUIGetVScrollRange( gui_window *wnd );
extern gui_text_ord GUIAPI GUIGetNumRows( gui_window *wnd );

/* Built in user interactions */

extern gui_message_return GUIAPI GUIDisplayMessage( gui_window *wnd, const char *message, const char *title, gui_message_type type );
extern gui_message_return GUIAPI GUIGetNewVal( const char *title, const char *old, char **new_val );
extern bool     GUIAPI GUIDlgPick( const char *title, GUIPICKCALLBACK *pickinit, int *choice );
extern bool     GUIAPI GUIDlgPickWithRtn( const char *title, GUIPICKCALLBACK *pickinit, PICKDLGOPEN *, int *choice );

/* Dialog Functions */

extern bool     GUIAPI GUICreateDialog( gui_create_info *dlg_info, int num_controls, gui_control_info *controls_info );
extern bool     GUIAPI GUICreateSysModalDialog( gui_create_info *dlg_info, int num_controls, gui_control_info *controls_info );
extern bool     GUIAPI GUICreateResDialog( gui_create_info *dlg_info, res_name_or_id dlg_id );
extern void     GUIAPI GUICloseDialog( gui_window *wnd );
extern void     GUIAPI GUISetModalDlgs( bool );

/* Control Functions */

extern bool     GUIAPI GUIAddControl( gui_control_info *ctl_info, gui_colour_set *plain, gui_colour_set *standout );
extern bool     GUIAPI GUIDeleteControl( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUIResizeControl( gui_window *wnd, gui_ctl_id id, const gui_rect *rect );
extern bool     GUIAPI GUIEnableControl( gui_window *wnd, gui_ctl_id id, bool enable );
extern bool     GUIAPI GUIIsControlEnabled( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUIGetControlRect( gui_window *wnd, gui_ctl_id id, gui_rect *rect );
extern bool     GUIAPI GUIGetControlClass( gui_window *wnd, gui_ctl_id id, gui_control_class *control_class );
extern void     GUIAPI GUIHideControl( gui_window *wnd, gui_ctl_id id );
extern void     GUIAPI GUIShowControl( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUIIsControlVisible( gui_window *wnd, gui_ctl_id id );

/* combo/list box functions */
extern bool     GUIAPI GUIControlSetRedraw( gui_window *wnd, gui_ctl_id id, bool redraw );
extern bool     GUIAPI GUIAddText( gui_window *wnd, gui_ctl_id id, const char *text );
extern bool     GUIAPI GUISetListItemData( gui_window *wnd, gui_ctl_id id, int choice, void *data );
extern void     * GUIAPI GUIGetListItemData( gui_window *wnd, gui_ctl_id id, int choice );
extern bool     GUIAPI GUIAddTextList( gui_window *wnd, gui_ctl_id id, int num_items, const void *data_handle, GUIPICKGETTEXT *getstring );
extern bool     GUIAPI GUIInsertText( gui_window *wnd, gui_ctl_id id, int choice, const char *text );
extern bool     GUIAPI GUISetTopIndex( gui_window *wnd, gui_ctl_id id, int choice );
extern int      GUIAPI GUIGetTopIndex( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUISetHorizontalExtent( gui_window *wnd, gui_ctl_id id, gui_ord extentx );
extern bool     GUIAPI GUIClearList( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUIDeleteItem( gui_window *wnd, gui_ctl_id id, int choice );
extern int      GUIAPI GUIGetListSize( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUIGetCurrSelect( gui_window *wnd, gui_ctl_id id, int *choice );
extern bool     GUIAPI GUISetCurrSelect( gui_window *wnd, gui_ctl_id id, int choice );
extern char     * GUIAPI GUIGetListItem( gui_window *wnd, gui_ctl_id id, int choice );

extern bool     GUIAPI GUISetText( gui_window *wnd, gui_ctl_id id, const char *text );
extern bool     GUIAPI GUIClearText( gui_window *wnd, gui_ctl_id id );
extern char     * GUIAPI GUIGetText( gui_window *wnd, gui_ctl_id id );
extern size_t   GUIAPI GUIDlgBuffGetText( gui_window *wnd, gui_ctl_id id, char *buff, size_t buff_len );
extern bool     GUIAPI GUISelectAll( gui_window *wnd, gui_ctl_id id, bool select );
extern bool     GUIAPI GUISetEditSelect( gui_window *wnd, gui_ctl_id id, int start, int end );
extern bool     GUIAPI GUIGetEditSelect( gui_window *wnd, gui_ctl_id id, int *start, int *end );
extern bool     GUIAPI GUILimitEditText( gui_window *wnd, gui_ctl_id id, int len );
extern bool     GUIAPI GUIDropDown( gui_window *wnd, gui_ctl_id id, bool drop );
extern void     GUIAPI GUIScrollCaret( gui_window *wnd, gui_ctl_id id );

extern unsigned GUIAPI GUIIsChecked( gui_window *wnd, gui_ctl_id id );
extern bool     GUIAPI GUISetChecked( gui_window *wnd, gui_ctl_id id, unsigned check );

/* Information Functions */

extern void     GUIAPI GUIGetKeyState( gui_keystate *state );
extern void     GUIAPI GUIFlushKeys( void );
extern void     GUIAPI GUIDrainEvents( void );
extern void     GUIAPI GUISetExtra( gui_window *wnd, void *extra );
extern void     * GUIAPI GUIGetExtra( gui_window *wnd );
extern void     GUIAPI GUIGetClientRect( gui_window *wnd, gui_rect *rect );
extern bool     GUIAPI GUIGetPaintRect( gui_window *wnd, gui_rect *rect );
extern void     GUIAPI GUIGetAbsRect( gui_window *wnd, gui_rect *rect );
extern void     GUIAPI GUIGetRect( gui_window *wnd, gui_rect *rect );
extern gui_scroll_styles GUIAPI GUIGetScrollStyle( gui_window *wnd );
extern gui_create_styles GUIAPI GUIGetCreateStyle( gui_window *wnd );
extern void     GUIAPI GUITruncToPixel( gui_coord *coord );
extern bool     GUIAPI GUIGetMousePosn( gui_window *wnd, gui_point *point );
extern void     GUIAPI GUIGetSystemMetrics( gui_system_metrics *metrics );
extern bool     GUIAPI GUIGetMinSize( gui_coord *size );
extern void     GUIAPI GUIEnumChildWindows( gui_window *wnd, ENUMCALLBACK *func, void *param );
extern void     GUIAPI GUIEnumControls( gui_window *wnd, CONTRENUMCALLBACK *func, void *param );
extern bool     GUIAPI GUIIsGUI( void );
extern bool     GUIAPI GUIGetArgs( char ***argv, int *argc );

/* Spawn functions */
extern void     GUIAPI GUISpawnStart( void );
extern void     GUIAPI GUISpawnEnd( void );

/* Resource String Functions */
extern bool     GUIAPI GUILoadStrInit( const char *fname );
extern bool     GUIAPI GUILoadStrFini( void );
extern bool     GUIAPI GUILoadString( gui_res_id string_id, char *buffer, int buffer_length );
extern bool     GUIAPI GUIIsLoadStrInitialized( void );

/* Hooking the F1 key */
extern void     GUIAPI GUIHookF1( void );
extern void     GUIAPI GUIUnHookF1( void );

/* DBCS functions */
extern int      GUIAPI GUICharLen( int );
extern bool     GUIAPI GUIIsDBCS( void );
extern void     GUIAPI GUISetJapanese( void );

extern bool     GUIAPI GUIIsFirstInstance( void );

extern void     GUIAPI GUIHookFileDlg( bool hook );

/* Application related functions */
/* must be implemented by application */
extern void     GUIAPI GUImain( void );
#if defined( __RDOS__ ) || defined( __NT__ )
extern void     GUIAPI GUITimer( void );
#endif

/* may be implemented by application, stub functions */
extern bool     GUIAPI GUIFirstCrack( void );
extern bool     GUIAPI GUIDead( void );
extern bool     GUIAPI GUIDeath( void );
extern char     * GUIAPI GUIGetWindowClassName( void );

extern bool     GUIAPI GUISysInit( init_mode install );
extern void     GUIAPI GUISysFini( void  );
