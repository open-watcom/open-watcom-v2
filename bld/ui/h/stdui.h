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
* Description:  User Interface (UI) library public interface.
*
****************************************************************************/


#ifndef _STDUI_H_INCLUDED_
#define _STDUI_H_INCLUDED_

#include <stddef.h>
#include <stdio.h>
#include "bool.h"

#ifndef         _FAR
#define         _FAR
#endif

#ifndef         _FARD
#define         _FARD
#endif


typedef enum EVENT {
    EV_SIGNED_TYPE               = -1,    /* ensure EVENT is signed type, required by uiungetevent */
    EV_NO_EVENT                  = 0x0000,

    EV_FIRST_EDIT_CHAR           = 0x0001,
    EV_CTRL_ENTER                = 0x000A,
    EV_CTRL_BACKSPACE            = 0x007F,
    EV_LAST_EDIT_CHAR            = 0x00fe,

    EV_RUB_OUT                   = 0x0108,
    EV_TAB                       = 0x0109,
    EV_ENTER                     = 0x010D,
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
#endif
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
#if !defined( FD6 )
    EV_LAST_KEYBOARD             = 0x01ff,
#endif
    EV_SHOW_FIELD,
    EV_MODIFIED_FIELD,
    EV_LINE_CHANGE,
    EV_MOUSE_HOLD,
    EV_MOUSE_PRESS,
    EV_MOUSE_DRAG,
    EV_MOUSE_RELEASE,
    EV_BUFFER_FULL,
    EV_FIELD_CHANGE,
    EV_ALT_PRESS,
    EV_ALT_RELEASE,
    EV_MOUSE_DCLICK,
    EV_MOUSE_REPEAT,
    EV_SHIFT_PRESS,
    EV_SHIFT_RELEASE,
    EV_CTRL_PRESS,
    EV_CTRL_RELEASE,
    EV_SCROLL_PRESS,
    EV_SCROLL_RELEASE,
    EV_NUM_PRESS,
    EV_NUM_RELEASE,
    EV_CAPS_PRESS,
    EV_CAPS_RELEASE,
    EV_INSERT_PRESS,
    EV_INSERT_RELEASE,

    EV_CLOCK_TICK,
    EV_SINK,

    EV_MOUSE_HOLD_R,
    EV_MOUSE_PRESS_R,
    EV_MOUSE_DRAG_R,
    EV_MOUSE_RELEASE_R,
    EV_MOUSE_DCLICK_R,
    EV_MOUSE_REPEAT_R,
    EV_MOUSE_HOLD_M,
    EV_MOUSE_PRESS_M,
    EV_MOUSE_DRAG_M,
    EV_MOUSE_RELEASE_M,
    EV_MOUSE_DCLICK_M,
    EV_MOUSE_REPEAT_M,

    EV_TOP,
    EV_BOTTOM,

    EV_BUMP_RIGHT,
    EV_BUMP_LEFT,
    EV_JOIN_RIGHT,
    EV_JOIN_LEFT,
    EV_SPLIT,
    EV_SCROLL_LINE_UP,
    EV_SCROLL_LINE_DOWN,
    EV_SCROLL_PAGE_UP,
    EV_SCROLL_PAGE_DOWN,
    EV_SCROLL_LEFT_FIELD,
    EV_SCROLL_RIGHT_FIELD,
    EV_SCROLL_LEFT_PAGE,
    EV_SCROLL_RIGHT_PAGE,
    EV_SCROLL_VERTICAL,
    EV_SCROLL_HORIZONTAL,

    EV_IDLE,
    EV_BUFFER_CLEAR,

    EV_MOUSE_MOVE,
    EV_LIST_BOX_CHANGED,
    EV_MENU_ACTIVE,
    EV_MENU_INITPOPUP,
    EV_LIST_BOX_DCLICK,
    EV_LIST_BOX_CLOSED,
    EV_BACKGROUND_RESIZE,

    // NOTE: This event should always be passed up, from all functions.
    EV_KILL_UI,

    EV_CHECK_BOX_CLICK,
    EV_REDRAW_SCREEN,

/*
 * This next one isn't all that useful on AT-class machines, I grant you.
 * But - it could be useful on the NEC, to provide a nice interface for
 * quitting or resetting.  Just a thought.
 */
    EV_CTRL_ALT_DELETE           = 0x03FF,

    EV_FIRST_UNUSED              = 0x0400,

    EV_LAST_EVENT                = 0x7FFF,
} EVENT;

#define EV_FIRST_EVENT          EV_FIRST_EDIT_CHAR

#define EV_ESCAPE               EV_ALT_RIGHT_BRACKET

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

typedef struct eventlist {
    int             num_lists;
    EVENT _FARD*    events[ MAX_EVENT_LISTS ];
} EVENTLIST;

enum    {
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
};

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

#ifdef __GUI__
/*
    ORD needs to be an unsigned int for the WINDOWS scaling system
    in WVIDEO since values in the range 0-10000 are used.
    Only the data structures are use.. No UI functions are actually
    called for Windows so UI does not need to be rebuilt
*/

typedef unsigned int    ORD;

#elif __RDOS__

/*      This needs to be fixed so scaling for a mouse also need a larger range!!
    An improper reference to __GUI__ is used. Fixed for now so it works for RDOS. 
*/
    
typedef int    ORD;

#else

typedef unsigned char   ORD;

#endif

typedef unsigned short  MOUSEORD;

typedef struct sarea {
    ORD             row;
    ORD             col;
    ORD             height;
    ORD             width;
} SAREA;

typedef unsigned char   ATTR;

#define iseditchar( ev )        ( ( ev >= EV_FIRST_EDIT_CHAR ) && ( ev <= EV_LAST_EDIT_CHAR ) )
#define iskeyboardchar( ev )    ( ( ev >= EV_FIRST_EVENT ) && ( ev <= EV_LAST_KEYBOARD ) )

#if defined(__NT__)
    typedef struct pixel {
        unsigned short  ch;
        unsigned short  attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
#elif defined(__OS2__) && defined(__386__)
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
#elif defined(__UNIX__)
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
#elif defined(__RDOS__)
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
#elif defined( _M_IX86 )
    typedef struct pixel {
        unsigned char   ch;
        ATTR            attr;
    } PIXEL;
    #define __FAR __far
    #define HAVE_FAR
#else
    #error pixel structure not configured for system
#endif

typedef PIXEL       __FAR *LP_PIXEL;
typedef char        __FAR *LP_STRING;
typedef const char  __FAR *LPC_STRING;
typedef void        __FAR *LP_VOID;

typedef struct buffer {
    LP_PIXEL    origin;
    int         increment;
} BUFFER;

typedef struct image_hld {
    struct image_hld __FAR      *next_hld;
    SAREA                       area;
    int                         kill_image;
    LP_VOID                     hld;
} IMAGE_HLD;

typedef struct image_def {      // this gets attached to the graphic field
    LP_VOID                     (_FAR *get_image)( void );
    void                        (_FAR *put_image)( void );
    void                        (_FAR *done_image)( void );
    IMAGE_HLD __FAR             *images;
} IMAGE_DEF;


typedef struct window {
    SAREA               area;
    SAREA               dirty_area;
    int                 priority;
    void                (_FAR *update)( struct sarea, void * );
    void _FARD          *parm;
    struct window _FARD *next;
    struct window _FARD *prev;
    union {
        IMAGE_DEF __FAR *graphic;
        BUFFER          buffer;
    }                   type;
} UI_WINDOW;

typedef struct vscreen {
    EVENT           event;          /* garbage                          */
    const char      _FARD *title;   /* title of virtual screen          */
    SAREA           area;           /* position on physical screen      */
    unsigned int    flags;          /* dialogue, unframed, movable etc. */
    ORD             row;
    ORD             col;            /* position of cursor on the screen */
    int             cursor;         /* cursor type                      */
    bool            open;           /* bool: init to FALSE, set by ui   */
    bool            dynamic_title;  /* title is allocated dynamicaly    */
    UI_WINDOW       window;         /* used by the window manager       */
} VSCREEN;

typedef struct monitor {
    ORD             height;         /* number of rows                   */
    ORD             width;          /* number of columns                */
    int             colour;         /* M_MONO, M_CGA, M_EGA             */
    ATTR _FARD      *attrs;         /* attributes                       */
    EVENTLIST _FARD *events;        /* event list pointer               */
    UI_WINDOW _FARD *area_head;     /* head of area list                */
    UI_WINDOW _FARD *area_tail;     /* tail of area list                */
    unsigned        mouse_acc_delay;/* acceleration delay               */
    unsigned        mouse_rpt_delay;/* repeat delay                     */
    unsigned        mouse_clk_delay;/* double click delay               */
    unsigned        tick_delay;     /* clock tick delay                 */
    int             cursor_on;      /* cursor on flag                   */
    int             cursor_attr;    /* cursor attribute                 */
    ORD             cursor_row;     /* cursor row                       */
    ORD             cursor_col;     /* cursor column                    */
    int             cursor_type;    /* cursor type                      */
    unsigned char   old_shift;      /* status of shift keys             */
    bool            no_snow;        /* snow check flag                  */
    UI_WINDOW       blank;          /* blank window                     */
    BUFFER          screen;         /* screen                           */
    unsigned        desqview:1;     /* desqview present flag            */
    unsigned        f10menus:1;     /* F10 active for menus             */
    unsigned        busy_wait:1;    /* SINK, MOUSE_HOLD or NO_EVENT     */
    unsigned        mouse_swapped:1;/* mouse swap flag                  */
    unsigned        no_idle_int:1;  /* disable idle interrupt           */
    unsigned        no_refresh:1;   /* disable refresh on EV_NO_EVENT   */
    unsigned        no_graphics:1;  /* disable character mapping        */
    unsigned        dbcs:1;         /* double-byte character set        */
    unsigned        no_blowup:1;    /* disable exploding windows        */
    unsigned        mouse_speed;    /* mouse speed factor               */
    unsigned char   mouse_xscale;   /* factor to divide mouse x posn    */
    unsigned char   mouse_yscale;   /* factor to divide mouse y posn    */
} MONITOR;

#define         V_DIALOGUE              0x0001
#define         V_UNBUFFERED            0x0002
#define         V_UNFRAMED              0x0004
#define         V_NO_ZOOM               0x0008
#define         V_PASSIVE               0x0010
#define         V_UNPROTECTED           0x0020
#define         V_HIDDEN                0x0040
#define         V_LISTBOX               0x0100
#define         V_GUI_WINDOW            0x1000      /* reserved for use by gui project */

enum {
    C_OFF,
    C_NORMAL,
    C_INSERT
};

enum {
    M_MONO,
    M_CGA,
    M_EGA,
    M_VGA,
    M_BW,
    M_NEC_NORM,
    M_UNUSED,
    M_NEC_HIRES,
    M_FMR

#if defined( __UNIX__ )
    ,M_TERMINFO_MONO
#endif
};

#ifdef __cplusplus
    extern "C" {
#endif

extern      MONITOR _FARD   *UIData;

extern      void            finimouse( void );
extern      int             initmouse( int );
extern      void            uiactivatemenus( void );
extern      void            *uialloc( size_t );
extern      bool            uiattrs( void );
extern      bool            uivgaattrs( void );
extern      void            uisetblinkattr( int );
extern      char            uigetblinkattr( void );
extern      bool            uibackground( char * );
extern      BUFFER    _FARD *uibackgroundbuffer( void );
extern      void            uibandinit( SAREA, ATTR );
extern      void            uibandmove( SAREA );
extern      void            uibandfini( void );
extern      void            uirestorebackground( void );
extern      void            uibarf( void );
extern      void            uiblankarea( SAREA );
extern      void            uiblankscreen( void );
extern      void            uiblankattr( ATTR );
extern      void            *uicalloc( size_t , size_t );
extern      bool            uichecklist( EVENT, EVENT _FARD * );
extern      void            uiclose( VSCREEN _FARD * );
extern      void            uicntrtext( VSCREEN _FARD *, SAREA *, ATTR, unsigned int, const char * );
extern      bool            uiconfig( char *, char ** );
extern      void            uicursor( VSCREEN _FARD *, ORD, ORD, int );
extern      EVENT           uidialogevent( VSCREEN _FARD * );
extern      void            uidirty( SAREA );
extern      void            uidrawbox( VSCREEN _FARD *, SAREA *area, ATTR attr, const char * );
extern      EVENT           uieventsource( bool );
extern      EVENT           uieventsourcehook( EVENT );
extern      LP_VOID         uifaralloc( size_t );
extern      void            uifarfree( LP_VOID );
extern      void            uifini( void );
extern      void            uifinicursor( void );
extern      void            uifinigmouse( void );
extern      void            uiflush( void );
extern      void            uiflushevent( void );
extern      void            uifree( void * );
extern      unsigned long   uiclock( void );
extern      EVENT           uiget( void );
extern      void            uigetcursor( ORD _FARD*, ORD _FARD*, int _FARD*, int _FARD* );
extern      EVENTLIST _FARD *uigetlist( void );
extern      void            uigetmouse( ORD _FARD*, ORD _FARD*, bool _FARD* );
extern      void            uiignorealt( void );
extern      unsigned int    uiextkeyboard( void );
extern      bool            uiinit( int );
extern      void            uiinitcursor( void );
extern      bool            uiinitgmouse( int );
extern      bool            uiinlist( EVENT );
extern      bool            uiintoplist( EVENT );
extern      bool            uikeepbackground( void );
extern      void            *uimalloc( size_t );
extern      void            uimouse( int );
extern      void            uimouseforceoff( void );
extern      void            uimouseforceon( void );
extern      void            uinocursor( VSCREEN _FARD *);
extern      void            uioffcursor( void );
extern      void            uioncursor( void );
extern      void            uioffmouse( void );
extern      void            uionmouse( void );
extern      VSCREEN _FARD   *uiopen( SAREA *, const char *, unsigned int );
extern      void            uihidemouse( void );
extern      unsigned        uiclockdelay( unsigned milli );
extern      EVENT   _FARD   *uipoplist( void );
extern      void            uiposition( SAREA *, ORD, ORD, int, int, bool );
extern      void            uiprotect( VSCREEN _FARD* );
extern      void            uipushlist( EVENT _FARD* );
extern      void            uiputlist( EVENTLIST _FARD* );
extern      void            *uirealloc( void *, size_t );
extern      void            uirefresh( void );
extern      bool            uiremovebackground( void );
extern      bool            uiset80col( void );
extern      SAREA           *uisetarea( SAREA *,  VSCREEN _FARD * );
extern      void            uisetcursor( ORD, ORD, int, int );
extern      void            uisetmouse( MOUSEORD, MOUSEORD );
extern      void            uisetmouseposn( ORD, ORD );
extern      SAREA           *uisetscreenarea( SAREA *, bool, bool );
extern      void            uisetsnow( bool );
extern      void            uispawnend( void );
extern      void            uispawnstart( void );
extern      bool            uistart( void );
extern      void            uistop( void );
extern      void            uiswap( void );
extern      void            uiswapcursor( void );
extern      void            uiswapmouse( void );
extern      void            uiungetevent( void );
extern      void            uiunprotect( VSCREEN _FARD* );
extern      void            uivattrflip( VSCREEN _FARD*, SAREA );
extern      void            uivattribute( VSCREEN _FARD*, SAREA, ATTR );
extern      void            uivclose( VSCREEN _FARD* );
extern      void            uivdirty( VSCREEN _FARD*, SAREA );
extern      void            uivfill( VSCREEN _FARD*, SAREA, ATTR, unsigned char );
extern      EVENT           uivget( VSCREEN  _FARD* );
extern      EVENT           uivgetevent( VSCREEN _FARD* );
extern      EVENT           uivgetprime( VSCREEN _FARD* );
extern      EVENT           uivgetprimevent( VSCREEN _FARD* );
extern      bool            uivhide( VSCREEN _FARD * );
extern      bool            uivmouseinstalled( void );
extern      VSCREEN _FARD   *uivmousepos( VSCREEN _FARD*, ORD _FARD*, ORD _FARD* );
extern      VSCREEN _FARD   *uimousepos( VSCREEN _FARD*, int _FARD*, int _FARD* );
extern      void            uivmoveblock( VSCREEN _FARD*, SAREA, int, int );
extern      VSCREEN _FARD   *uivopen( VSCREEN _FARD* );
extern      VSCREEN _FARD   *uivresize( VSCREEN _FARD*, SAREA );
extern      void            uivmove( VSCREEN _FARD*, ORD, ORD );
extern      void            uivrawput( VSCREEN _FARD*, ORD, ORD, PIXEL _FARD*, int );
extern      void            uivsetactive( VSCREEN _FARD* );
extern      void            uivsetcursor( VSCREEN _FARD* );
extern      bool            uivshow( VSCREEN _FARD * );
extern      void            uivtextput( VSCREEN _FARD*, ORD, ORD, ATTR, const char _FARD*, int );
extern      void            uitextfield( VSCREEN _FARD*, ORD, ORD, ORD, ATTR, LPC_STRING, int );
extern      void            uimousespeed( unsigned );
extern      unsigned char   uicheckshift( void );
extern      EVENT           uikeyboardevent( void );
extern      bool            uimouseinstalled( void );

extern      bool            FlipCharacterMap( void );
extern      bool            UIMapCharacters( unsigned char mapchar[], unsigned char mapdata[][16] );

extern      void            uiyield( void );
extern      bool            uiforceevadd( EVENT );  // int is a bool
extern      void            uiforceevflush( void );
extern      int             uiisdbcs( void );       // bool
extern      int             uionnec( void );        // bool
extern      int             uicharlen( int );       // returns 2 if dbcs lead byte
extern      void            UIMemOpen( void );
extern      void            UIMemClose( void );

extern      int             uimousealign( void );

typedef     void            (uitimer_callback)( void );
extern      void            uitimer( uitimer_callback *proc, int ms );

extern      void            uistartevent( void );
extern      void            uidoneevent( void );

#if defined( __DOS__ )

extern      LP_VOID         dos_uivideobuffer( LP_VOID vbuff );

#elif defined( __WINDOWS__ )

extern      void            win_uisetmono( void );
extern      void            win_uisetcolor( int clr );

#pragma aux win_uihookrtn far parm [ax] [cx] modify exact [];
extern      void __far __loadds win_uihookrtn( unsigned event, unsigned info );

#endif

#ifdef __cplusplus
}
#endif

#endif /* _STDUI_H_INCLUDED_ */
