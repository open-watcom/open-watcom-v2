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
* Description:  User Interface (UI) library public interface.
*
****************************************************************************/


#ifndef _STDUI_H_INCLUDED_
#define _STDUI_H_INCLUDED_

#include <stddef.h>
#include <stdio.h>
#include "bool.h"
#include "initmode.h"


#ifndef _FAR
#define _FAR
#endif

#ifndef _FARD
#define _FARD
#endif

#define UIAPI                   /* public API */
#define UICALLBACK              /* public callback */
#define UIHOOK                  /* hook functions */

typedef enum ui_event {
    EV_SIGNED_TYPE               = -1,    /* ensure ui_event is signed type, required by uiungetevent */
    EV_NO_EVENT                  = 0x0000,

    EV_FIRST_EDIT_CHAR           = 0x0001,
    EV_CTRL_ENTER                = 0x000A,
    EV_CTRL_BACKSPACE            = 0x007F,
    EV_LAST_EDIT_CHAR            = 0x00fe,

    EV_RUB_OUT                   = 0x0108,
    EV_TAB                       = 0x0109,
    EV_ENTER                     = 0x010D,
    EV_ESCAPE                    = 0x010E,
    EV_SHIFT_TAB                 = 0x010F,  // only some hosts
    EV_ALT_Q                     = 0x0110,
    EV_ALT_W,                   // 0x0111
    EV_ALT_E,                   // 0x0112
    EV_ALT_R,                   // 0x0113
    EV_ALT_T,                   // 0x0114
    EV_ALT_Y,                   // 0x0115
    EV_ALT_U,                   // 0x0116
    EV_ALT_I,                   // 0x0117
    EV_ALT_O,                   // 0x0118
    EV_ALT_P,                   // 0x0119
    EV_ALT_LEFT_BRACKET,        // 0x011a
    EV_ALT_RIGHT_BRACKET,       // 0x011b
    EV_ALT_ENTER,               // 0x011c
    EV_ALT_A                     = 0x011e,
    EV_ALT_S,                   // 0x011f
    EV_ALT_D,                   // 0x0120
    EV_ALT_F,                   // 0x0121
    EV_ALT_G,                   // 0x0122
    EV_ALT_H,                   // 0x0123
    EV_ALT_J,                   // 0x0124
    EV_ALT_K,                   // 0x0125
    EV_ALT_L,                   // 0x0126
    EV_ALT_SEMI_COLON,          // 0x0127
    EV_ALT_QUOTE,               // 0x0128
    EV_ALT_BACKQUOTE,           // 0x0129
    EV_ALT_BACKSLASH             = 0x012b,
    EV_ALT_Z,                   // 0x012c
    EV_ALT_X,                   // 0x012d
    EV_ALT_C,                   // 0x012e
    EV_ALT_V,                   // 0x012f
    EV_ALT_B,                   // 0x0130
    EV_ALT_N,                   // 0x0131
    EV_ALT_M,                   // 0x0132
    EV_ALT_COMMA,               // 0x0133
    EV_ALT_PERIOD,              // 0x0134
    EV_ALT_SLASH,               // 0x0135
    EV_ALT_SPACE,               // 0x0136
    EV_F1                        = 0x013b,  // only some hosts
    EV_F2,                      // 0x013c   // only some hosts
    EV_F3,                      // 0x013d   // only some hosts
    EV_F4,                      // 0x013e   // only some hosts
    EV_F5,                      // 0x013f   // only some hosts
    EV_F6,                      // 0x0140   // only some hosts
    EV_F7,                      // 0x0141   // only some hosts
    EV_F8,                      // 0x0142   // only some hosts
    EV_F9,                      // 0x0143   // only some hosts
    EV_F10,                     // 0x0144   // only some hosts
    EV_HOME                      = 0x0147,
    EV_CURSOR_UP,               // 0x0148
    EV_PAGE_UP,                 // 0x0149
    EV_CURSOR_LEFT               = 0x014B,
    EV_CURSOR_RIGHT              = 0x014D,
    EV_END                       = 0x014F,
    EV_CURSOR_DOWN,             // 0x0150
    EV_PAGE_DOWN,               // 0x0151
    EV_INSERT,                  // 0x0152
    EV_DELETE,                  // 0x0153
    EV_SHIFT_F1                  = 0x0154,  // only some hosts
    EV_SHIFT_F2,                // 0x0155   // only some hosts
    EV_SHIFT_F3,                // 0x0156   // only some hosts
    EV_SHIFT_F4,                // 0x0157   // only some hosts
    EV_SHIFT_F5,                // 0x0158   // only some hosts
    EV_SHIFT_F6,                // 0x0159   // only some hosts
    EV_SHIFT_F7,                // 0x015a   // only some hosts
    EV_SHIFT_F8,                // 0x015b   // only some hosts
    EV_SHIFT_F9,                // 0x015c   // only some hosts
    EV_SHIFT_F10,               // 0x015d   // only some hosts
    EV_CTRL_F1                   = 0x015e,  // only some hosts
    EV_CTRL_F2,                 // 0x015f   // only some hosts
    EV_CTRL_F3,                 // 0x0160   // only some hosts
    EV_CTRL_F4,                 // 0x0161   // only some hosts
    EV_CTRL_F5,                 // 0x0162   // only some hosts
    EV_CTRL_F6,                 // 0x0163   // only some hosts
    EV_CTRL_F7,                 // 0x0164   // only some hosts
    EV_CTRL_F8,                 // 0x0165   // only some hosts
    EV_CTRL_F9,                 // 0x0166   // only some hosts
    EV_CTRL_F10,                // 0x0167   // only some hosts
    EV_ALT_F1                    = 0x0168,  // only some hosts
    EV_ALT_F2,                  // 0x0169   // only some hosts
    EV_ALT_F3,                  // 0x016a   // only some hosts
    EV_ALT_F4,                  // 0x016b   // only some hosts
    EV_ALT_F5,                  // 0x016c   // only some hosts
    EV_ALT_F6,                  // 0x016d   // only some hosts
    EV_ALT_F7,                  // 0x016e   // only some hosts
    EV_ALT_F8,                  // 0x016f   // only some hosts
    EV_ALT_F9,                  // 0x0170   // only some hosts
    EV_ALT_F10,                 // 0x0171   // only some hosts
    EV_CTRL_CURSOR_LEFT          = 0x0173,
    EV_CTRL_CURSOR_RIGHT,       // 0x0174
    EV_CTRL_END,                // 0x0175
    EV_CTRL_PAGE_DOWN,          // 0x0176
    EV_CTRL_HOME,               // 0x0177
    EV_ALT_1                     = 0x0178,
    EV_ALT_2,                   // 0x0179
    EV_ALT_3,                   // 0x017a
    EV_ALT_4,                   // 0x017b
    EV_ALT_5,                   // 0x017c
    EV_ALT_6,                   // 0x017d
    EV_ALT_7,                   // 0x017e
    EV_ALT_8,                   // 0x017f
    EV_ALT_9,                   // 0x0180
    EV_ALT_0,                   // 0x0181
    EV_ALT_MINUS,               // 0x0182
    EV_ALT_EQUAL,               // 0x0183
    EV_CTRL_PAGE_UP              = 0x0184,
#if defined( FD6 )
    EV_LAST_KEYBOARD,
#else
    EV_F11                       = 0x0185,  // only some hosts
    EV_F12,                     // 0x0186   // only some hosts
    EV_SHIFT_F11,               // 0x0187   // only some hosts
    EV_SHIFT_F12,               // 0x0188   // only some hosts
    EV_CTRL_F11,                // 0x0189   // only some hosts
    EV_CTRL_F12,                // 0x018a   // only some hosts
    EV_ALT_F11,                 // 0x018b   // only some hosts
    EV_ALT_F12,                 // 0x018c   // only some hosts
    EV_CTRL_CURSOR_UP            = 0x018d,
    EV_CTRL_CURSOR_DOWN          = 0x0191,
    EV_CTRL_INSERT,             // 0x0192
    EV_CTRL_DELETE,             // 0x0193
    EV_CTRL_TAB,                // 0x0194
    EV_ALT_HOME                  = 0x0197,
    EV_ALT_CURSOR_UP,           // 0x0198
    EV_ALT_PAGE_UP,             // 0x0199
    EV_ALT_CURSOR_LEFT           = 0x019b,
    EV_ALT_CURSOR_RIGHT          = 0x019d,
    EV_ALT_END                   = 0x019f,
    EV_ALT_CURSOR_DOWN           = 0x01a0,
    EV_ALT_PAGE_DOWN,           // 0x01a1
    EV_ALT_INSERT,              // 0x01a2
    EV_ALT_DELETE,              // 0x01a3
    EV_ALT_TAB                   = 0x01a5,
#if !defined( NO_SHIFT_MOVEMENT )
    EV_SHIFT_HOME                = 0x01c0,  // only some hosts
    EV_SHIFT_END,               // 0x01c1   // only some hosts
    EV_SHIFT_CURSOR_UP,         // 0x01C2   // only some hosts
    EV_SHIFT_CURSOR_DOWN,       // 0x01C3   // only some hosts
    EV_SHIFT_CURSOR_LEFT,       // 0x01C4   // only some hosts
    EV_SHIFT_CURSOR_RIGHT,      // 0x01C5   // only some hosts
    EV_SHIFT_PAGE_UP,           // 0x01C6   // only some hosts
    EV_SHIFT_PAGE_DOWN,         // 0x01C7   // only some hosts
#endif
    EV_LAST_KEYBOARD             = 0x01ff,
#endif
    EV_SHOW_FIELD                = 0x0200,
    EV_MODIFIED_FIELD,          // 0x0201
    EV_LINE_CHANGE,             // 0x0202
    EV_MOUSE_HOLD,              // 0x0203
    EV_MOUSE_PRESS,             // 0x0204
    EV_MOUSE_DRAG,              // 0x0205
    EV_MOUSE_RELEASE,           // 0x0206
    EV_BUFFER_FULL,             // 0x0207
    EV_FIELD_CHANGE,            // 0x0208
    EV_ALT_PRESS,               // 0x0209
    EV_ALT_RELEASE,             // 0x020A
    EV_MOUSE_DCLICK,            // 0x020B
    EV_MOUSE_REPEAT,            // 0x020C
    EV_SHIFT_PRESS,             // 0x020D
    EV_SHIFT_RELEASE,           // 0x020E
    EV_CTRL_PRESS,              // 0x020F
    EV_CTRL_RELEASE,            // 0x0210
    EV_SCROLL_PRESS,            // 0x0211
    EV_SCROLL_RELEASE,          // 0x0212
    EV_NUM_PRESS,               // 0x0213
    EV_NUM_RELEASE,             // 0x0214
    EV_CAPS_PRESS,              // 0x0215
    EV_CAPS_RELEASE,            // 0x0216
    EV_INSERT_PRESS,            // 0x0217
    EV_INSERT_RELEASE,          // 0x0218

    EV_CLOCK_TICK,              // 0x0219
    EV_SINK,                    // 0x021A

    EV_MOUSE_HOLD_R,            // 0x021B
    EV_MOUSE_PRESS_R,           // 0x021C
    EV_MOUSE_DRAG_R,            // 0x021D
    EV_MOUSE_RELEASE_R,         // 0x021E
    EV_MOUSE_DCLICK_R,          // 0x021F
    EV_MOUSE_REPEAT_R,          // 0x0220
    EV_MOUSE_HOLD_M,            // 0x0221
    EV_MOUSE_PRESS_M,           // 0x0222
    EV_MOUSE_DRAG_M,            // 0x0223
    EV_MOUSE_RELEASE_M,         // 0x0224
    EV_MOUSE_DCLICK_M,          // 0x0225
    EV_MOUSE_REPEAT_M,          // 0x0226

    EV_TOP,                     // 0x0227
    EV_BOTTOM,                  // 0x0228

    EV_BUMP_RIGHT,              // 0x0229
    EV_BUMP_LEFT,               // 0x022A
    EV_JOIN_RIGHT,              // 0x022B
    EV_JOIN_LEFT,               // 0x022C
    EV_SPLIT,                   // 0x022D
    EV_SCROLL_LINE_UP,          // 0x022E
    EV_SCROLL_LINE_DOWN,        // 0x022F
    EV_SCROLL_PAGE_UP,          // 0x0230
    EV_SCROLL_PAGE_DOWN,        // 0x0231
    EV_SCROLL_LEFT_FIELD,       // 0x0232
    EV_SCROLL_RIGHT_FIELD,      // 0x0233
    EV_SCROLL_LEFT_PAGE,        // 0x0234
    EV_SCROLL_RIGHT_PAGE,       // 0x0235
    EV_SCROLL_VERTICAL,         // 0x0236
    EV_SCROLL_HORIZONTAL,       // 0x0237

    EV_IDLE,                    // 0x0238
    EV_BUFFER_CLEAR,            // 0x0239

    EV_MOUSE_MOVE,              // 0x023A
    EV_LIST_BOX_CHANGED,        // 0x023B
    EV_MENU_ACTIVE,             // 0x023C
    EV_MENU_INITPOPUP,          // 0x023D
    EV_LIST_BOX_DCLICK,         // 0x023E
    EV_LIST_BOX_CLOSED,         // 0x023F
    EV_BACKGROUND_RESIZE,       // 0x0240

    // NOTE: This event should always be passed up, from all functions.
    EV_KILL_UI,                 // 0x0241

    EV_CHECK_BOX_CLICK,         // 0x0242
    EV_REDRAW_SCREEN,           // 0x0243

    EV_STICKY_FUNC               = 0x03F0,
    EV_STICKY_SHIFT,            // 0x03F1
    EV_STICKY_CTRL,             // 0x03F2
    EV_STICKY_ALT,              // 0x03F3
    EV_STICKY_INTRO              = 0x03FE,

    /*
     * This next one isn't all that useful on AT-class machines, I grant you.
     * But - it could be useful on the NEC, to provide a nice interface for
     * quitting or resetting.  Just a thought.
     */
    EV_CTRL_ALT_DELETE           = 0x03FF,

    EV_FIRST_UNUSED              = 0x0400,

    EV_LAST_EVENT                = 0x7FFF,
} ui_event;

#define ___                     EV_NO_EVENT         /* readable no event */
#define __rend__                EV_NO_EVENT         /* readable group separator in event list */
#define __end__                 EV_NO_EVENT         /* readable group separator in event list */

#define EV_FIRST_EVENT          EV_FIRST_EDIT_CHAR

#define EV_TAB_FORWARD          EV_TAB
#define EV_TAB_BACKWARD         EV_SHIFT_TAB

#define EV_RETURN               EV_ENTER
#define EV_CTRL_RETURN          EV_CTRL_ENTER

#if defined( NO_SHIFT_MOVEMENT )
#define EV_SHIFT_HOME           EV_HOME
#define EV_SHIFT_END            EV_END
#define EV_SHIFT_CURSOR_UP      EV_CURSOR_UP
#define EV_SHIFT_CURSOR_DOWN    EV_CURSOR_DOWN
#define EV_SHIFT_CURSOR_LEFT    EV_CURSOR_LEFT
#define EV_SHIFT_CURSOR_RIGHT   EV_CURSOR_RIGHT
#define EV_SHIFT_PAGE_UP        EV_PAGE_UP
#define EV_SHIFT_PAGE_DOWN      EV_PAGE_DOWN
#endif
#define EV_SHIFT_INSERT         EV_INSERT
#define EV_SHIFT_DELETE         EV_DELETE

#define MAX_EVENT_LISTS         30

typedef enum {
    V_DIALOGUE      = 0x0001,
    V_UNBUFFERED    = 0x0002,
    V_UNFRAMED      = 0x0004,
    V_PASSIVE       = 0x0008,
    V_UNPROTECTED   = 0x0010,
    V_HIDDEN        = 0x0020,
    V_LISTBOX       = 0x0040,
    V_GUI_WINDOW    = 0x1000      /* reserved for use by gui project */
} screen_flags;

#define ISFRAMED(f)     (((f) & V_UNFRAMED) == 0)
#define ISBUFFERED(f)   (((f) & V_UNBUFFERED) == 0)
#define ISPROTECTED(f)  (((f) & V_UNPROTECTED) == 0)

#define V_GUIWINDOW     (V_UNFRAMED | V_GUI_WINDOW)
#define ISGUIWINDOW(f)  (((f)->flags & V_GUI_WINDOW) != 0)

typedef struct ui_event_list {
    int         num_lists;
    ui_event    _FARD *events[MAX_EVENT_LISTS];
} ui_event_list;

typedef enum {
    ATTR_MENU,
    ATTR_ACTIVE,
    ATTR_CURR_ACTIVE,
    ATTR_INACTIVE,
    ATTR_CURR_INACTIVE,
    ATTR_SHADOW,
    ATTR_DEFAULT_HOTSPOT,
    ATTR_NORMAL,
    ATTR_BRIGHT,
    ATTR_EDIT,
    ATTR_REVERSE,
    ATTR_EDIT2,
    ATTR_HOTSPOT,
    ATTR_HELP,
    ATTR_HOT,
    ATTR_HOT_CURR,
    ATTR_HOT_QUIET,
    ATTR_CURR_EDIT,
    ATTR_CURR_MARK_EDIT,
    ATTR_MARK_NORMAL,
    ATTR_MARK_EDIT,
    ATTR_CURR_HOTSPOT_KEY,
    ATTR_EDIT_DIAL,
    ATTR_UNUSED3,
    ATTR_UNUSED4,
    ATTR_CURR_SELECT_DIAL,
    ATTR_FRAME,
    ATTR_SCROLL_ICON,
    ATTR_SCROLL_BAR,
    ATTR_DIAL_FRAME,
    ATTR_BROWSE,
    ATTR_CURR_HOTSPOT,
    ATTR_ERROR,
    ATTR_HINT,
    ATTR_WARNING,
    ATTR_OFF_HOTSPOT,
    ATTR_RADIO_HOTSPOT,
    ATTR_LAST
} UIATTR;

/* line drawing and graphics characters */
enum {
    /* single line box drawing */
    UI_LLCORNER = 1,
    UI_LRCORNER,
    UI_ULCORNER,
    UI_URCORNER,
    UI_HLINE,
    UI_VLINE,
    UI_TTEE,
    UI_RTEE,
    UI_LTEE,

    /* double line box drawing */
    UI_DLLCORNER,
    UI_DLRCORNER,
    UI_DULCORNER,
    UI_DURCORNER,
    UI_DHLINE,
    UI_DVLINE,

    /* triangles */
    UI_DPOINT,
    UI_LPOINT,
    UI_RPOINT,
    UI_UPOINT,

    /* arrows */
    UI_DARROW,
    UI_UDARROW,

    /* boxes */
    UI_DBLOCK,
    UI_LBLOCK,
    UI_RBLOCK,
    UI_UBLOCK,
    UI_CKBOARD,
    UI_BOARD,
    UI_BLOCK,

    /* misc */
    UI_SQUARE,
    UI_ROOT,
    UI_EQUIVALENT = 31
    /* we use 31 of them: don't add any more!
       they have to fit in the C0 ASCII range */
};

typedef enum {
    MOUSE_ON  = 1,
    MOUSE_OFF = 2
} mouse_func;

#define NO_SELECT       -1

#ifdef UI_GUI
/*
    ORD needs to be an unsigned for the WINDOWS scaling system
    in WVIDEO since values in the range 0-10000 are used.
    Only the data structures are use.. No UI functions are actually
    called for Windows so UI does not need to be rebuilt
*/

typedef unsigned        ORD;

#elif __RDOS__

/*
    This needs to be fixed so scaling for a mouse also need a larger range!!
    An improper reference to UI_GUI is used. Fixed for now so it works for RDOS.
*/

typedef int             ORD;

#else

typedef unsigned char   ORD;

#endif

typedef unsigned short  uisize;
typedef signed short    uiord;

typedef unsigned short  MOUSEORD;
typedef unsigned long   MOUSETIME;

typedef signed short    CURSORORD;
#define CURSOR_INVALID  ((CURSORORD)-1)

typedef struct sarea {
    ORD             row;
    ORD             col;
    uisize          height;
    uisize          width;
} SAREA;

typedef unsigned char   ATTR;           /* character attributes type */
typedef int             CATTR;          /* cursor attributes type */
#define CATTR_VOFF      ((CATTR)(-2))   /* cursor attribute OFF virtual window */
#define CATTR_OFF       ((CATTR)(-1))   /* cursor attribute OFF global */
#define CATTR_NONE      0

#define iseditchar( ev )        ( ( ev >= EV_FIRST_EDIT_CHAR ) && ( ev <= EV_LAST_EDIT_CHAR ) )
#define iskeyboardchar( ev )    ( ( ev >= EV_FIRST_EVENT ) && ( ev <= EV_LAST_KEYBOARD ) )

#if defined( _M_I86 )
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR       __far
    #define HAVE_FAR
    #define UIDBCS
#elif defined(__NT__)
    typedef struct pixel {
        unsigned short  ch;
        unsigned short  attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
    #define UIDBCS
#elif defined(__OS2__)
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
    #define UIDBCS
#elif defined(__RDOS__)
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
    #undef UIDBCS
#elif defined(__UNIX__)
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
    #undef UIDBCS
#elif defined( __NETWARE__ )
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
  #if 1
    #define __FAR
    #undef HAVE_FAR
  #else
    #define __FAR       __far   /* ?? on netware probably should be near ?? */
    #define HAVE_FAR
  #endif
    #undef UIDBCS
#elif defined( _M_IX86 )        /* 32-bit DOS */
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR       __far
    #define HAVE_FAR
    #define UIDBCS
#else
    #error pixel structure not configured for system
#endif

typedef PIXEL       __FAR *LP_PIXEL;
typedef char        __FAR *LP_STRING;
typedef const char  __FAR *LPC_STRING;
typedef void        __FAR *LP_VOID;

typedef enum {
    C_OFF,
    C_NORMAL,
    C_INSERT
} CURSOR_TYPE;

typedef struct buffer {
    LP_PIXEL    origin;
    int         increment;
} BUFFER;

typedef void            (_FAR *window_update_func)(SAREA, void *);

typedef struct ui_window {
    SAREA               area;
    SAREA               dirty_area;
    int                 priority;
    window_update_func  update_func;
    void                _FARD *update_parm;
    struct ui_window    _FARD *next;
    struct ui_window    _FARD *prev;
    BUFFER              buffer;
} UI_WINDOW;

typedef struct vscreen {
    ui_event        event;              /* garbage                          */
    const char      _FARD *title;       /* title of virtual screen          */
    SAREA           area;               /* position on physical screen      */
    screen_flags    flags;              /* dialogue, unframed, movable etc. */
    CURSORORD       cursor_row;         /* position of cursor on the screen */
    CURSORORD       cursor_col;         /* position of cursor on the screen */
    CURSOR_TYPE     cursor_type;        /* cursor type                      */
    bool            open;               /* bool: init to false, set by ui   */
    bool            dynamic_title;      /* title is allocated dynamicaly    */
    UI_WINDOW       window;             /* used by the window manager       */
} VSCREEN;

typedef struct monitor {
    uisize          height;             /* number of rows                   */
    uisize          width;              /* number of columns                */
    int             colour;             /* M_MONO, M_CGA, M_EGA             */
    ATTR            _FARD *attrs;       /* attributes                       */
    ui_event_list   _FARD *events;      /* event list pointer               */
    UI_WINDOW       _FARD *area_head;   /* head of area list                */
    UI_WINDOW       _FARD *area_tail;   /* tail of area list                */
    unsigned        mouse_acc_delay;    /* acceleration delay               */
    unsigned        mouse_rpt_delay;    /* repeat delay                     */
    unsigned        mouse_clk_delay;    /* double click delay               */
    unsigned        tick_delay;         /* clock tick delay                 */
    CATTR           cursor_attr;        /* cursor attribute                 */
    CURSORORD       cursor_row;         /* cursor row                       */
    CURSORORD       cursor_col;         /* cursor column                    */
    CURSOR_TYPE     cursor_type;        /* cursor type                      */
    UI_WINDOW       blank_window;       /* blank window                     */
    BUFFER          screen;             /* screen                           */
    unsigned        mouse_speed;        /* mouse speed factor               */
    unsigned short  old_shift;          /* status of shift keys             */
    unsigned char   mouse_xscale;       /* factor to divide mouse x posn    */
    unsigned char   mouse_yscale;       /* factor to divide mouse y posn    */
    boolbit         no_snow       :1;   /* snow check flag                  */
    boolbit         cursor_on     :1;   /* cursor on flag                   */
    boolbit         desqview      :1;   /* desqview present flag            */
    boolbit         f10menus      :1;   /* F10 active for menus             */
    boolbit         busy_wait     :1;   /* SINK, MOUSE_HOLD or NO_EVENT     */
    boolbit         mouse_swapped :1;   /* mouse swap flag                  */
    boolbit         no_idle_int   :1;   /* disable idle interrupt           */
    boolbit         no_refresh    :1;   /* disable refresh on EV_NO_EVENT   */
    boolbit         no_graphics   :1;   /* disable character mapping        */
    boolbit         dbcs          :1;   /* double-byte character set        */
} MONITOR;

enum {
    M_MONO,
    M_CGA,
    M_EGA,
    M_VGA,
    M_BW,
    M_NEC_NORM,
    M_UNUSED,
    M_NEC_HIRES,
    M_FMR,
#if defined( __UNIX__ )
    M_TERMINFO_MONO
#endif
};

#ifdef __cplusplus
    extern "C" {
#endif

extern MONITOR          _FARD *UIData;

extern void             UIAPI finimouse( void );
extern bool             UIAPI initmouse( init_mode );
extern void             UIAPI uiactivatemenus( void );
extern void             * UIAPI uialloc( size_t );
extern bool             UIAPI uiattrs( void );
extern bool             UIAPI uivgaattrs( void );
extern void             UIAPI uisetblinkattr( bool );
extern bool             UIAPI uigetblinkattr( void );
extern void             UIAPI uibandinit( SAREA, ATTR );
extern void             UIAPI uibandmove( SAREA );
extern void             UIAPI uibandfini( void );
extern void             UIAPI uirestorebackground( void );
extern void             UIAPI uibarf( void );
extern void             UIAPI uiblankarea( SAREA );
extern void             UIAPI uiblankscreen( void );
extern void             UIAPI uiblankattr( ATTR );
extern void             UIAPI uicntrtext( VSCREEN _FARD *, SAREA *, ATTR, unsigned, const char * );
extern bool             UIAPI uiconfig( const char *, char ** );
extern void             UIAPI uicursor( VSCREEN _FARD *, CURSORORD, CURSORORD, CURSOR_TYPE );
extern ui_event         UIAPI uidialogevent( VSCREEN _FARD * );
extern void             UIAPI uidirty( SAREA );
extern void             UIAPI uidrawbox( VSCREEN _FARD *, SAREA *area, ATTR attr, const char * );
extern ui_event         UIAPI uieventsource( bool );
extern ui_event         UIAPI uieventsourcehook( ui_event );
extern void             UIAPI uifini( void );
extern void             UIAPI uifinigmouse( void );
extern void             UIAPI uiflush( void );
extern void             UIAPI uiflushevent( void );
extern void             UIAPI uifree( void * );
extern MOUSETIME        UIAPI uiclock( void );
extern ui_event         UIAPI uiget( void );
extern ui_event_list    _FARD * UIAPI uigetlist( void );
extern void             UIAPI uigetmouse( ORD _FARD *, ORD _FARD *, bool _FARD * );
extern void             UIAPI uiignorealt( void );
extern bool             UIAPI uiextkeyboard( void );
extern bool             UIAPI uiinit( init_mode );
extern bool             UIAPI uiinitgmouse( init_mode );
extern bool             UIAPI uiinlist( ui_event, ui_event _FARD * );
extern bool             UIAPI uiinlists( ui_event );
extern bool             UIAPI uiintoplist( ui_event );
extern void             * UIAPI uimalloc( size_t );
extern void             UIAPI uimouse( mouse_func );
extern void             UIAPI uimouseforceoff( void );
extern void             UIAPI uimouseforceon( void );
extern void             UIAPI uinocursor( VSCREEN _FARD * );
extern void             UIAPI uioffmouse( void );
extern void             UIAPI uionmouse( void );
extern void             UIAPI uihidemouse( void );
extern unsigned         UIAPI uiclockdelay( unsigned milli );
extern ui_event         _FARD * UIAPI uipoplist( void );
extern void             UIAPI uiposition( SAREA *, unsigned, unsigned, int, int, bool );
extern void             UIAPI uiprotect( VSCREEN _FARD * );
extern void             UIAPI uipushlist( ui_event _FARD * );
extern void             UIAPI uiputlist( ui_event_list _FARD * );
extern void             * UIAPI uirealloc( void *, size_t );
extern void             UIAPI uirefresh( void );
extern bool             UIAPI uiset80col( void );
extern SAREA            * UIAPI uisetarea( SAREA *,  VSCREEN _FARD * );
extern void             UIAPI uisetmouse( MOUSEORD, MOUSEORD );
extern void             UIAPI uisetmouseposn( ORD, ORD );
extern SAREA            * UIAPI uisetscreenarea( SAREA *, bool, bool );
extern void             UIAPI uisetsnow( bool );
extern void             UIAPI uiscreeninit( VSCREEN *vs, SAREA *area, screen_flags flags );
extern void             UIAPI uispawnend( void );
extern void             UIAPI uispawnstart( void );
extern bool             UIAPI uistart( void );
extern void             UIAPI uistop( void );
extern void             UIAPI uiswap( void );
extern void             UIAPI uiswapmouse( void );
extern void             UIAPI uiungetevent( void );
extern void             UIAPI uiunprotect( VSCREEN _FARD * );
extern void             UIAPI uivattrflip( VSCREEN _FARD *, SAREA );
extern void             UIAPI uivattribute( VSCREEN _FARD *, SAREA, ATTR );
extern void             UIAPI uivclose( VSCREEN _FARD * );
extern void             UIAPI uivdirty( VSCREEN _FARD *, SAREA );
extern void             UIAPI uivfill( VSCREEN _FARD *, SAREA, ATTR, char );
extern ui_event         UIAPI uivget( VSCREEN  _FARD * );
extern ui_event         UIAPI uivgetevent( VSCREEN _FARD * );
extern ui_event         UIAPI uivgetprime( VSCREEN _FARD * );
extern ui_event         UIAPI uivgetprimevent( VSCREEN _FARD * );
extern bool             UIAPI uivhide( VSCREEN _FARD * );
extern bool             UIAPI uivmouseinstalled( void );
extern VSCREEN          _FARD * UIAPI uivmousepos( VSCREEN _FARD *, ORD _FARD *, ORD _FARD * );
extern VSCREEN          _FARD * UIAPI uimousepos( VSCREEN _FARD *, int _FARD *, int _FARD * );
extern void             UIAPI uivmoveblock( VSCREEN _FARD *, SAREA, int, int );
extern VSCREEN          _FARD * UIAPI uivopen( VSCREEN _FARD * );
extern VSCREEN          _FARD * UIAPI uivresize( VSCREEN _FARD *, SAREA );
extern void             UIAPI uivmove( VSCREEN _FARD *, ORD, ORD );
extern void             UIAPI uivrawput( VSCREEN _FARD *, ORD, ORD, LP_PIXEL, unsigned );
extern void             UIAPI uivsetactive( VSCREEN _FARD * );
extern void             UIAPI uivsetcursor( VSCREEN _FARD * );
extern bool             UIAPI uivshow( VSCREEN _FARD * );
extern void             UIAPI uivtextput( VSCREEN _FARD *, ORD, ORD, ATTR, const char _FARD *, unsigned );
extern void             UIAPI uitextfield( VSCREEN _FARD *, ORD, ORD, unsigned, ATTR, LPC_STRING, unsigned );
extern void             UIAPI uimousespeed( unsigned );
extern unsigned char    UIAPI uicheckshift( void );
extern ui_event         UIAPI uikeyboardevent( void );
extern bool             UIAPI uimouseinstalled( void );

extern bool             UIAPI FlipCharacterMap( void );
extern bool             UIAPI UIMapCharacters( unsigned char mapchar[], unsigned char mapdata[][16] );

extern void             UIAPI uiyield( void );
extern bool             UIAPI uiforceevadd( ui_event );
extern void             UIAPI uiforceevflush( void );
extern bool             UIAPI uiisdbcs( void );
extern int              UIAPI uicharlen( int ch );        // returns 2 if dbcs lead byte

extern int              UIAPI uimousealign( void );

extern void             UIAPI UIMemOpen( void );
extern void             UIAPI UIMemClose( void );

extern ATTR             UIAPI uisetattr( UIATTR uiattr, ATTR new_attr );

/*
 * Application related functions
 * may be implemented by application, stub functions
 */

extern LP_VOID          UIAPI uifaralloc( size_t );
extern void             UIAPI uifarfree( LP_VOID );

extern void             UIAPI uistartevent( void );
extern void             UIAPI uidoneevent( void );

/*
 * below are OS specific API functions
 */

#if defined( __DOS__ )

extern LP_PIXEL         UIAPI dos_uishadowbuffer( LP_PIXEL vbuff );

#elif defined( __WINDOWS__ )

extern void             UIAPI win_uisetmono( void );
extern void             UIAPI win_uisetcolor( int clr );

#elif defined( __RDOS__ )

typedef void            (UICALLBACK rdos_uitimer_callback)( void );
extern void             UIAPI rdos_uitimer( rdos_uitimer_callback *proc, int ms );

extern void             UIAPI rdos_uisendescape( void );

#elif defined( __NETWARE__ )

extern char             * UIAPI uigetscreenname( void );
extern void             UIAPI uiwakethread( void );
extern void             UIAPI uiforceinfloop( void );

#elif defined( __UNIX__ )

/*
 * Somebody wants us to pretend that the specified event has occurred
 * (one of EV_SHIFT/CTRL/ALT_RELEASE) so that the corresponding press event
 * will be generated for the next keystroke (if that shift key is pressed).
 */
extern void             UIAPI uishiftrelease( ui_event ui_ev );

#endif

#ifdef __cplusplus
}
#endif

#endif /* _STDUI_H_INCLUDED_ */
