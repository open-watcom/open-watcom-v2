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

#include <stdio.h>
#include "bool.h"

#ifndef         _FAR
#define         _FAR
#endif

#ifndef         _FARD
#define         _FARD
#endif

#define         EV_NO_EVENT             0x00
#define         EV_FIRST_EVENT          0x01

#define         EV_FIRST_EDIT_CHAR      0x01
#define         EV_LAST_EDIT_CHAR       0xfe

#define         EV_RUB_OUT              0x108
#define         EV_TAB_FORWARD          0x109
#define         EV_RETURN               0x10D
#define         EV_ENTER                0x10D
#define         EV_CTRL_ENTER           0x00A
#define         EV_CTRL_RETURN          0x00A
#define         EV_CTRL_BACKSPACE       0x07F
#define         EV_ESCAPE               0x11B
/*
 * This next one isn't all that useful on AT-class machines, I grant you.
 * But - it could be useful on the NEC, to provide a nice interface for
 * quitting or resetting.  Just a thought.
 */
#define         EV_CTRL_ALT_DELETE      0x3FF

/*
 * I'm putting these hitherto undefined constants at the end of the
 * event list, defining them rather than enumming them, since they don't
 * really correspond to codes which are automatically generated.
 * However, they can be detected by windows, and I wish to be consistent.
 * Be on the lookout, though - not every platform can detect all these
 * different keystrokes.  CSK
 */

#if !defined( NO_SHIFT_MOVEMENT )
#define         EV_SHIFT_HOME           0x1C0
#define         EV_SHIFT_END            0x1C1

#define         EV_SHIFT_CURSOR_UP      0x1C2
#define         EV_SHIFT_CURSOR_DOWN    0x1C3
#define         EV_SHIFT_CURSOR_LEFT    0x1C4
#define         EV_SHIFT_CURSOR_RIGHT   0x1C5

#define         EV_SHIFT_PAGE_UP        0x1C6
#define         EV_SHIFT_PAGE_DOWN      0x1C7
#endif

#define         EV_CTRL( k )            ( (k) - 'a' + 1 )
#define         EV_ALT_KEYPAD( k )      (k)

#define         EV_FUNC( n )            ( 0x13A + (n) )
#define         EV_SHIFT_FUNC( n )      ( 0x153 + (n) )
#define         EV_CTRL_FUNC( n )       ( 0x15d + (n) )
#define         EV_ALT_FUNC( n )        ( 0x167 + (n) )

#define         EV_FUNC_11              0x185
#define         EV_FUNC_12              0x186
#define         EV_SHIFT_FUNC_11        0x187
#define         EV_SHIFT_FUNC_12        0x188
#define         EV_CTRL_FUNC_11         0x189
#define         EV_CTRL_FUNC_12         0x18A
#define         EV_ALT_FUNC_11          0x18B
#define         EV_ALT_FUNC_12          0x18C

#define         EV_TAB_BACKWARD         0x10F

typedef int EVENT;  /* must be signed for uiungetevent */

enum {
        EV_ALT_Q                        = 0x110,
        EV_ALT_W,
        EV_ALT_E,
        EV_ALT_R,
        EV_ALT_T,
        EV_ALT_Y,
        EV_ALT_U,
        EV_ALT_I,
        EV_ALT_O,
        EV_ALT_P,
        EV_ALT_LEFT_BRACKET,
        EV_ALT_RIGHT_BRACKET,
        EV_ALT_ENTER,
        EV_ALT_A                        = 0x11e,
        EV_ALT_S,
        EV_ALT_D,
        EV_ALT_F,
        EV_ALT_G,
        EV_ALT_H,
        EV_ALT_J,
        EV_ALT_K,
        EV_ALT_L,
        EV_ALT_SEMI_COLON,
        EV_ALT_QUOTE,
        EV_ALT_BACKQUOTE,
        EV_ALT_BACKSLASH                = 0x12b,
        EV_ALT_Z                        = 0x12c,
        EV_ALT_X,
        EV_ALT_C,
        EV_ALT_V,
        EV_ALT_B,
        EV_ALT_N,
        EV_ALT_M,
        EV_ALT_COMMA,
        EV_ALT_PERIOD,
        EV_ALT_SLASH,
        EV_ALT_SPACE,
        EV_HOME                         = 0x147,
        EV_CURSOR_UP,
        EV_PAGE_UP,
        EV_CURSOR_LEFT                  = 0x14B,
        EV_CURSOR_RIGHT                 = 0x14D,
        EV_END                          = 0x14F,
        EV_CURSOR_DOWN,
        EV_PAGE_DOWN,
        EV_INSERT,
        EV_DELETE,
        EV_CTRL_CURSOR_LEFT             = 0x173,
        EV_CTRL_CURSOR_RIGHT,
        EV_CTRL_END,
        EV_CTRL_PAGE_DOWN,
        EV_CTRL_HOME,
        EV_ALT_1                        = 0x178,
        EV_ALT_2,
        EV_ALT_3,
        EV_ALT_4,
        EV_ALT_5,
        EV_ALT_6,
        EV_ALT_7,
        EV_ALT_8,
        EV_ALT_9,
        EV_ALT_0,
        EV_ALT_MINUS,
        EV_ALT_EQUAL,
        EV_CTRL_PAGE_UP                 = 0x184,
#ifdef FD6
        EV_LAST_KEYBOARD,
#else
        EV_CTRL_CURSOR_UP               = 0x18d,
        EV_CTRL_CURSOR_DOWN             = 0x191,
        EV_CTRL_INSERT,
        EV_CTRL_DELETE,
        EV_CTRL_TAB,
        EV_ALT_HOME                     = 0x197,
        EV_ALT_CURSOR_UP,
        EV_ALT_PAGE_UP,
        EV_ALT_CURSOR_LEFT              = 0x19b,
        EV_ALT_CURSOR_RIGHT             = 0x19d,
        EV_ALT_END                      = 0x19f,
        EV_ALT_CURSOR_DOWN              = 0x1a0,
        EV_ALT_PAGE_DOWN,
        EV_ALT_INSERT,
        EV_ALT_DELETE,

        EV_LAST_KEYBOARD                = 0x1ff,
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

        EV_LAST_PLUS_1,
        EV_LAST_ENUMERATED              = EV_LAST_PLUS_1-1,

        EV_FIRST_UNUSED                 = 0x400
};

/*
 * The following section provides compatibility with earlier UI based
 * programs which may have relied on these names. The names have been
 * changed for consistency.
 */

#define EV_CTRL_LEFT            EV_CTRL_CURSOR_LEFT
#define EV_CTRL_RIGHT           EV_CTRL_CURSOR_RIGHT
#define EV_CTRL_UP              EV_CTRL_CURSOR_UP
#define EV_CTRL_DOWN            EV_CTRL_CURSOR_DOWN

#define EV_CTRL_PGUP            EV_CTRL_PAGE_UP
#define EV_CTRL_PGDN            EV_CTRL_PAGE_DOWN
#define EV_ALT_PGUP             EV_ALT_PAGE_UP
#define EV_ALT_PGDN             EV_ALT_PAGE_DOWN

#define         MAX_EVENT_LISTS         30

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

#define         iseditchar( ev )        ( ( ev >= EV_FIRST_EDIT_CHAR ) \
                                       && ( ev <= EV_LAST_EDIT_CHAR ) )

#if defined(__NT__)
    typedef struct pixel {
            unsigned short  ch;
            unsigned short  attr;
    } PIXEL;
    #define __FAR
    #undef HAVE_FAR
#elif defined(__OS2__) && defined(__386__)
    typedef struct pixel {
            char            ch;
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
            char            ch;
            ATTR            attr;
    } PIXEL;
    #define __FAR __far
    #define HAVE_FAR
#else
    #error pixel structure not configured for system
#endif

typedef PIXEL __FAR *LPPIXEL;

typedef struct buffer {
    LPPIXEL     origin;
    int         increment;
} BUFFER;

typedef struct image_hld {
    struct image_hld __FAR      *next_hld;
    SAREA                       area;
    int                         kill_image;
    void __FAR                  *hld;
} IMAGE_HLD;

typedef struct image_def {      // this gets attached to the graphic field
    void __FAR                  *(_FAR *get_image)( void );
    void                        (_FAR *put_image)( void );
    void                        (_FAR *done_image)( void );
    IMAGE_HLD __FAR             *images;
} IMAGE_DEF;


typedef struct window {
        SAREA               area;
        SAREA               dirty;
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
        char _FARD      *name;          /* title of virtual screen          */
        SAREA           area;           /* position on physical screen      */
        unsigned int    flags;          /* dialogue, unframed, movable etc. */
        ORD             row;
        ORD             col;            /* position of cursor on the screen */
        int             cursor;         /* cursor type                      */
        int             open;           /* bool: init to FALSE, set by ui   */
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
        int             no_snow;        /* snow check flag                  */
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

extern          MONITOR _FARD*  UIData;

extern          void            finimouse( void );
extern          bool            initmouse( int );
extern          void            uiactivatemenus( void );
extern          void           *uialloc( unsigned );
extern          bool            uiattrs( void );
extern          bool            uivgaattrs( void );
extern          void            uisetblinkattr( int );
extern          char            uigetblinkattr( void );
extern          bool            uibackground( char * );
extern          BUFFER    _FARD *uibackgroundbuffer( void );
extern          void            uibandinit( SAREA, ATTR );
extern          void            uibandmove( SAREA );
extern          void            uibandfini( void );
extern          void            uirestorebackground( void );
extern          void            uibarf( void );
extern          void            uiblankarea( SAREA );
extern          void            uiblankscreen( void );
extern          void            uiblankattr( ATTR );
extern          void           *uicalloc( size_t , size_t );
extern          bool            uichecklist( EVENT, EVENT _FARD * );
extern          void            uiclose( VSCREEN _FARD * );
extern          void            uicntrtext( VSCREEN _FARD *, SAREA *, ATTR,
                                            unsigned int, const char * );
extern          bool            uiconfig( char *, char ** );
extern          void            uicursor( VSCREEN _FARD *, ORD, ORD, int );
extern          int             uidialogevent( VSCREEN _FARD * );
extern          void            uidirty( SAREA );
extern          void            uidrawbox( VSCREEN _FARD *, SAREA *area,
                                           ATTR attr, const char * );
extern          EVENT           uieventsource( int );
extern          void __FAR*     uifaralloc( int );
extern          void            uifarfree( void __FAR * );
extern          void            uifini( void );
extern          void            uifinicursor( void );
extern          void            uifinigmouse( void );
extern          void            uiflush( void );
extern          void            uiflushevent( void );
extern          void            uifree( void * );
extern          EVENT           uiget( void );
extern          void            uigetcursor( ORD _FARD*, ORD _FARD*,
                                int _FARD*, int _FARD* );
extern          EVENTLIST _FARD *uigetlist( void );
extern          void            uigetmouse( ORD _FARD*, ORD _FARD*, int _FARD* );
extern          void            uiignorealt( void );
extern          unsigned int    uiextkeyboard( void );
extern          bool            uiinit( int );
extern          void            uiinitcursor( void );
extern          bool            uiinitgmouse( int );
extern          bool            uiinlist( EVENT );
extern          bool            uiintoplist( EVENT );
extern          bool            uikeepbackground( void );
extern          void           *uimalloc( size_t );
extern          void            uimouse( int );
extern          void            uimouseforceoff( void );
extern          void            uimouseforceon( void );
extern          void            uinocursor( VSCREEN _FARD *);
extern          void            uioffcursor( void );
extern          void            uioncursor( void );
extern          void            uioffmouse( void );
extern          void            uionmouse( void );
extern          VSCREEN _FARD  *uiopen( SAREA *, char *, unsigned int );
extern          void            uihidemouse( void );
extern          unsigned        uiclockdelay( unsigned milli );
extern          EVENT   _FARD  *uipoplist( void );
extern          void            uiposition( SAREA *, ORD, ORD, int, int, bool );
extern          void            uiprotect( VSCREEN _FARD* );
extern          void            uipushlist( EVENT _FARD* );
extern          void            uiputlist( EVENTLIST _FARD* );
extern          void           *uirealloc( void *, size_t );
extern          void            uirefresh( void );
extern          bool            uiremovebackground( void );
extern          bool            uiset80col( void );
extern          SAREA          *uisetarea( SAREA *,  VSCREEN _FARD * );
extern          void            uisetcursor( ORD, ORD, int, int );
extern          void            uisetmouse( MOUSEORD, MOUSEORD );
extern          void            uisetmouseposn( ORD, ORD );
extern          SAREA          *uisetscreenarea( SAREA *, bool, bool );
extern          void            uisetsnow( bool );
extern          void            uispawnend( void );
extern          void            uispawnstart( void );
extern          bool            uistart( void );
extern          void            uistop( void );
extern          void            uiswap( void );
extern          void            uiswapcursor( void );
extern          void            uiswapmouse( void );
extern          void            uiungetevent( void );
extern          void            uiunprotect( VSCREEN _FARD* );
extern          void            uivattrflip( VSCREEN _FARD*, SAREA );
extern          void            uivattribute( VSCREEN _FARD*, SAREA, ATTR );
extern          void            uivclose( VSCREEN _FARD* );
extern          void            uivdirty( VSCREEN _FARD*, SAREA );
extern          void            uivfill( VSCREEN _FARD*, SAREA, ATTR, char );
extern          EVENT           uivgetevent( VSCREEN _FARD* );
extern          EVENT           uivgetprime( VSCREEN _FARD* );
extern          EVENT           uivgetprimevent( VSCREEN _FARD* );
extern          unsigned        uivhide( VSCREEN _FARD * );
extern          int             uivmouseinstalled( void );
extern          VSCREEN _FARD*  uivmousepos( VSCREEN _FARD*, ORD _FARD*,
                                ORD _FARD* );
extern          VSCREEN _FARD*  uimousepos( VSCREEN _FARD*, int _FARD*,
                                int _FARD* );
extern          void            uivmoveblock( VSCREEN _FARD*, SAREA, int, int );
extern          VSCREEN _FARD*  uivopen( VSCREEN _FARD* );
extern          VSCREEN _FARD*  uivresize( VSCREEN _FARD*, SAREA );
extern          void            uivmove( VSCREEN _FARD*, ORD, ORD );
extern          void            uivrawput( VSCREEN _FARD*, ORD, ORD,
                                PIXEL _FARD*, int );
extern          void            uivsetactive( VSCREEN _FARD* );
extern          void            uivsetcursor( VSCREEN _FARD* );
extern          unsigned        uivshow( VSCREEN _FARD * );
extern          void            uivtextput( VSCREEN _FARD*, ORD, ORD,
                                ATTR, const char _FARD*, int );
extern          void            uitextfield( VSCREEN _FARD*, ORD, ORD,
                                ORD, ATTR, const char __FAR *, int );
extern          void            uimousespeed( unsigned );
extern          unsigned char   uicheckshift( void );
extern          EVENT           uikeyboardevent( void );
extern          int             uimouseinstalled( void );

extern          bool            FlipCharacterMap( void );
extern          bool            UIMapCharacters( unsigned char mapchar[], unsigned char mapdata[][16] );

extern          void            uiyield( void );
extern          bool            uiforceevadd( EVENT );  // int is a bool
extern          void            uiforceevflush( void );
extern          int             uiisdbcs( void ); // bool
extern          int             uionnec( void );  // bool
extern          int             uicharlen( int ); // returns 2 if dbcs lead byte
extern          void            UIMemOpen( void );
extern          void            UIMemClose( void );

#ifdef __cplusplus
}
#endif

#endif /* _STDUI_H_INCLUDED_ */
