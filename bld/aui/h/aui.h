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
* Description:  Public interface to the AUI (Advanced UI) library.
*
****************************************************************************/


#ifndef _AUI_H_INCLUDED
#define _AUI_H_INCLUDED

#include "gui.h"
#include "guikey.h"
#include "guifdlg.h"

#if defined(_AUIPVT_H_INCLUDED) || defined(AUI_DBG)
    #define a_window struct aui_private_window_structure
#else
    struct a_window;
    typedef struct a_window a_window;
#endif

#include "bool.h"
#ifndef NULL
    #define NULL ((void*)0)
#endif

#define SAVE_SIZE               512

typedef struct {
    unsigned char       area[SAVE_SIZE];
    int                 first_cmd;
    int                 first_free;
    int                 curr_cmd;
    bool                last_was_next;
} save_area;

#define MIN_DCLICK      100
#define MAX_DCLICK      1500
#define MAX_MAGIC_STR   20
#define WND_NO_PIECE    255
#define MAX_KEY_SIZE    256
#define MAX_POPUPS      20
#define WND_MAX_COL     32767
#define WND_NO_COL      WND_MAX_COL
// make sure WND_MAX_ROW + WndRows(wnd) will never wrap
#define WND_MAX_ROW     ((wnd_row)32000)
#define WND_NO_ROW      (-WND_MAX_ROW)
#define WND_APPROX_SIZE 10000
#define WND_NO_CLASS    ((wnd_class)-1)

#define WND_SAVE_ROW    0
#define WND_RESTORE_ROW 1

#define WND_CURRENT_CHANGED     GUI_NO_EVENT // sent when WndDirtyCurr called



typedef enum {
    WND_FIRST_UNUSED = GUI_FIRST_UNUSED,
} wnd_attr;

#define WND_MENU_POPUP  GUI_UTIL_1

#define WSW_LBUTTON_SELECTS             0x00000001L     // default off
#define WSW_MULTILINE_SELECT            0x00000002L     // default off
#define WSW_SUBWORD_SELECT              0x00000004L     // default off
#define WSW_MAP_CURSOR_TO_SCROLL        0x00000008L     // default off
#define WSW_RBUTTON_CHANGE_CURR         0x00000010L     // default off
#define WSW_SELECT_IN_TABSTOP           0x00000020L     // default on
#define WSW_MUST_CLICK_ON_PIECE         0x00000040L     // default off
#define WSW_RBUTTON_SELECTS             0x00000080L  // default off
#define WSW_CHOOSING                    0x00000100L  // default off
#define WSW_CHAR_CURSOR                 0x00000200L  // default off
#define WSW_ALLOW_POPUP                 0x00000400L  // default on
#define WSW_SEARCH_WRAP                 0x00000800L  // default on
#define WSW_HIGHLIGHT_CURRENT           0x00001000L  // default on
#define WSW_ONLY_MODIFY_TABSTOP         0x00002000L  // default on
#define WSW_UTIL_1                      0x00100000L
#define WSW_CLICKED                     0x00200000L
#define WSW_DCLICKED                    0x00400000L
#define WSW_CACHE_LINES                 0x00800000L
#define WSW_SELECTING_WITH_KEYBOARD     0x01000000L
#define WSW_SELECT_EVENT                0x02000000L
#define WSW_REPAINT                     0x04000000L
#define WSW_ALTERNATE_BIT               0x08000000L  // default off
#define WSW_ICONIFIED                   0x10000000L
#define WSW_SELECTING                   0x20000000L
#define WSW_ACTIVE                      0x40000000L
#define WSW_MENU_ACCURATE_ROW           0x80000000L

typedef unsigned long wnd_switches;


typedef int             wnd_row;
typedef unsigned char   wnd_class;
typedef unsigned long   wnd_update_list;

typedef struct wnd_line_piece {
    char        *text;                  // default ""
    wnd_attr    attr;                   // default WND_PLAIN
    gui_ord     indent;                 // default 0
    gui_ord     extent;                 // default WND_NO_EXTEND
    unsigned    tabstop         : 1;    // default on
    unsigned    static_text     : 1;    // default off
    unsigned    hot             : 1;    // default off
    unsigned    underline       : 1;    // default off
    unsigned    use_key         : 1;    // default on
    unsigned    use_piece0_attr : 1;    // default off
    unsigned    use_prev_attr   : 1;    // default off
    unsigned    bitmap          : 1;    // default off
    unsigned    master_tabstop  : 1;    // default off
    unsigned    draw_bar        : 1;    // default off
    unsigned    vertical_line   : 1;    // default off
    unsigned    draw_hook       : 1;    // default off
    unsigned    draw_line_hook  : 1;    // default off
    unsigned    length;                 // INTERNAL -- do not use
    char        *hint;                  // set for Hint Text
} wnd_line_piece;

#define WND_NO_EXTEND   0
#define WND_MAX_EXTEND  GUI_NO_COLUMN

typedef struct wnd_create_struct {
    struct wnd_info     *info;
    char                *title;
    wnd_class           class;
    void                *extra;
    gui_create_styles   style;
    gui_scroll_styles   scroll;
    gui_colour_set      *colour;
    gui_rect            rect;
    int                 title_size;
} wnd_create_struct;

typedef struct wnd_posn {
    float               x,y;
    float               width,height;
} wnd_posn;


typedef int wnd_menu_id;

#define MENU_INITIALIZE ((wnd_menu_id)-1)

typedef struct {
    wnd_row             row;
    int                 piece;
    int                 col;
} wnd_coord;

typedef struct {
    wnd_row             row;
    int                 piece;
    int                 col;
    int                 end_col;
} wnd_dirt;

typedef struct {
    wnd_row             row;
    int                 piece;
    int                 col;
    int                 end;
} wnd_subpiece;

typedef struct {
    wnd_row             min_rows;
    int                 min_cols;
    wnd_row             max_rows;
    int                 max_cols;
} wnd_metrics;

typedef struct {
    gui_bar_styles      bar_style;
    gui_colour          bar_colour;
    gui_colour          bar_colour2;
    gui_ord             bar_size2;
    unsigned            bar_group    : 1;
    unsigned            bar_selected : 1;
} wnd_bar_info;


#define __WND_FIELD( wnd, field ) (((struct aui_private_window_structure *)(wnd))->field )
#define WndExtra( wnd ) __WND_FIELD( wnd, extra )
#define WndPopItem( wnd ) __WND_FIELD( wnd, popitem )
#define WndSrchItem( wnd ) __WND_FIELD( wnd, searchitem )
#define WndMaxCharX( wnd ) __WND_FIELD( wnd, max_char.x )
#define WndMaxCharY( wnd ) __WND_FIELD( wnd, max_char.y )
#define WndAvgCharX( wnd ) __WND_FIELD( wnd, avg_char_x )
#define WndMidCharX( wnd ) __WND_FIELD( wnd, mid_char_x )
#define WndTop( wnd ) __WND_FIELD( wnd, top )
#define WndSetTop( wnd, x ) __WND_FIELD( wnd, top ) = x
#define WndHasClass( wnd ) ( __WND_FIELD( wnd, class ) != WND_NO_CLASS )
#define WndClass( wnd ) __WND_FIELD( wnd, class )
#define WndTitleSize( wnd ) (__WND_FIELD( wnd, title_size ))
#define WndRows( wnd ) (__WND_FIELD( wnd, rows )-__WND_FIELD( wnd, title_size ))
#define WndMaxRow( wnd ) __WND_FIELD( wnd, max_row )
#define WndGui( wnd ) __WND_FIELD( wnd, gui )
#define WndWidth( wnd ) __WND_FIELD( wnd, width )
#define WndRepaint( wnd ) __WND_FIELD( wnd, switches ) |= WSW_REPAINT
#define WndSetKey( wnd, x ) __WND_FIELD( wnd, keypiece ) = (x)
#define WndKeyPiece( wnd ) __WND_FIELD( wnd, keypiece )
#define WndSetSwitches( wnd, x ) __WND_FIELD( wnd, switches ) |= (x)
#define WndSwitchOn( wnd, x ) ( __WND_FIELD( wnd, switches ) & (x) )
#define WndClrSwitches( wnd, x ) __WND_FIELD( wnd, switches ) &= ~(x)
#define WndNumPopups( wnd ) __WND_FIELD( wnd, num_popups )
#define WndPopupMenu( wnd ) __WND_FIELD( wnd, popupmenu )

typedef struct aui_private_window_structure {
    gui_window              *gui;
    union { // to avoid global recompiles
        struct aui_private_window_structure *__free_field;
        struct {
            char            row;
            char            piece;
        } button_down;
    } u;
    void                    *extra;
    struct wnd_info         *info;
    char                    *popitem;
    char                    *searchitem;
    wnd_coord               sel_start;
    wnd_coord               sel_end;
    wnd_coord               current;
    wnd_row                 rows;
    gui_coord               max_char;
    gui_ord                 width;
    gui_ord                 max_indent;
    int                     top;
    char                    button_down_screen_row;
    unsigned char           keyindex;
    unsigned char           keypiece;
    wnd_class               class;
    wnd_switches            switches;
    int                     vscroll_pending;
    int                     hscroll_pending;
    wnd_row                 max_row;
    char                    *select_chars;
    int                     title_size;
    gui_ord                 avg_char_x;
    gui_ord                 mid_char_x;
    int                     last_popup;
    int                     current_col;
    char                    num_popups;
    gui_menu_struct         *popupmenu;
    char                    dirtyrects;
    wnd_dirt                dirty[1];
} aui_private_window_structure;



typedef bool (WNDCALLBACK)( a_window *, gui_event, void * );
typedef void (WNDREFRESH)( a_window * );
typedef void (WNDMENU)( a_window *, unsigned, int, int );
typedef void (WNDMODIFY)( a_window *, int, int );
typedef int (WNDSCROLL)( a_window *, int );
typedef int (WNDNUMROWS)( a_window * );
typedef int (WNDNEXTROW)( a_window *, int, int );
typedef bool (WNDGETLINE)( a_window *wnd, wnd_row row, int piece, wnd_line_piece * );
typedef void (WNDNOTIFY)( a_window *wnd, wnd_row row, int piece );
typedef void (WNDBEGPAINT)( a_window *wnd, wnd_row row, int num );
typedef void (WNDENDPAINT)( a_window *wnd, wnd_row row, int num );
typedef bool (WNDCHKFLAGS)( wnd_update_list );
typedef a_window * (WNDOPEN)( void );
typedef a_window *(WNDCREATE)( char *, struct wnd_info *, wnd_class, void * );
typedef void (WNDCLOSE)( a_window * );


typedef struct wnd_info {
        WNDCALLBACK             *event;
        WNDREFRESH              *refresh;
        WNDGETLINE              *getline;
        WNDMENU                 *menuitem;
        WNDSCROLL               *scroll;
        WNDBEGPAINT             *begpaint;
        WNDENDPAINT             *endpaint;
        WNDMODIFY               *modify;
        WNDNUMROWS              *numrows;
        WNDNEXTROW              *nextrow;
        WNDNOTIFY               *notify;
        WNDCHKFLAGS             *chkflags;
        wnd_update_list         flags;
        char                    num_popups;
        gui_menu_struct         *popupmenu;
} wnd_info;


typedef int             GUIPICKER( char *, PICKCALLBACK * );
extern int              DlgPickWithRtn( char *title, const void *data_handle, int def, PICKGETTEXT *getstring, int items );
extern int              DlgPickWithRtn2( char *title, const void *data_handle, int def, PICKGETTEXT *getstring, int items, GUIPICKER * );
extern int              DlgPick( char *title, const void *data_handle, int def, int items );
extern bool             DlgNew( char *title, char *buff, unsigned buff_len );
extern void             DlgOpen( char *title, int, int, gui_control_info *, int, GUICALLBACK *, void * );
extern void             ResDlgOpen( GUICALLBACK *, void *, int );
extern int              DlgGetFileName( open_file_name *ofn );
extern bool             DlgFileBrowse( char *title, char *filter, char *path,
                                       unsigned len, unsigned long flags );

extern a_window         **WndFindOwner( a_window * );
extern a_window         *WndFirst( void );

extern gui_ord          WndExtentX( a_window *, const char * );

extern void             WndFreshAll( void );
extern bool             WndStopRefresh( bool );

extern void             WndNoMemory( void );
extern void             WndMemInit( void );
extern void             WndMemFini( void );
extern void             *WndAlloc( size_t );
extern void             *WndRealloc( void *, size_t );
extern void             WndFree( void * );
extern void             *WndMustAlloc( size_t );
extern void             *WndMustRealloc( void *, size_t );
extern void             WndCreateFloatingPopup( a_window *, gui_point *, char,
                                               gui_menu_struct *, int * );

extern void             WndFixedThumb( a_window * );
extern void             WndSetThumbPos( a_window *, int );
extern void             WndSetThumbPercent( a_window *, int );
extern void             WndSetThumb( a_window * );
extern WNDSCROLL        WndScroll;
extern WNDSCROLL        WndScrollAbs;


extern wnd_row          WndCurrRow( a_window * );
extern bool             WndHasCurrent( a_window * );
extern void             WndNewCurrent( a_window *, wnd_row, int );
extern void             WndMoveCurrent( a_window *wnd, wnd_row row, int piece );
extern void             WndGetCurrent( a_window *, wnd_row *, int *);
extern void             WndNoCurrent( a_window * );
extern bool             WndFirstCurrent( a_window * );
extern bool             WndLastCurrent( a_window * );

extern void             WndNoSelect( a_window * );

extern void             WndCleanUp( void );
extern a_window         *WndFindActive( void );

extern void             WndDestroy( a_window * );
extern void             WndClose( a_window * );
extern void             WndRestoreToFront( a_window * );
extern void             WndToFront( a_window * );   // won't restore an icon!

extern void             WndCurrToGUIPoint( a_window *wnd, gui_point *point );

extern WNDCREATE        WndCreate;
extern void             WndInitCreateStruct( wnd_create_struct * );
extern a_window         *WndCreateWithStruct( wnd_create_struct * );
extern a_window         *WndCreateWithStructAndMenuRes( wnd_create_struct *, int );
extern void             WndShrinkToMouse( a_window *wnd, wnd_metrics * );
extern bool             WndInit( char *title );
extern bool             WndInitWithMenuRes( char *title, int );
extern bool             WndFini( void );
extern bool             WndMainMenuProc( a_window *, unsigned );
extern void             WndSetSrchItem( a_window *wnd, char *expr );
extern bool             WndSearch( a_window *, bool, int );
extern void             WndInitNumRows( a_window * );
extern void             WndRXError( int );

extern void             WndFreshAll( void );
extern a_window         *WndNext( a_window * );

extern WNDCALLBACK      NoEventProc;
extern WNDREFRESH       NoRefresh;
extern WNDGETLINE       NoGetLine;
extern WNDMENU          NoMenuItem;
extern WNDMODIFY        NoModify;
extern WNDMODIFY        WndFirstMenuItem;
extern WNDSCROLL        NoScroll;
extern WNDBEGPAINT      NoBegPaint;
extern WNDENDPAINT      NoEndPaint;
extern WNDNOTIFY        NoNotify;
extern WNDNUMROWS       NoNumRows;
extern WNDNEXTROW       NoNextRow;

extern WNDCALLBACK      WndEvent;
extern WNDREFRESH       WndRefresh;
extern WNDGETLINE       WndGetLine;
extern WNDGETLINE       WndGetLineAbs;
extern WNDMENU          WndMenuItem;
extern WNDMODIFY        WndModify;
extern WNDSCROLL        WndScroll;
extern WNDBEGPAINT      WndBegPaint;
extern WNDENDPAINT      WndEndPaint;
extern WNDNOTIFY        WndNotify;
extern WNDNUMROWS       WndNumRows;
extern WNDNEXTROW       WndNextRow;
extern bool             WndHasNumRows( a_window * );

extern wnd_switches     WndSwitches;
extern a_window         *Windows;
extern gui_coord        WndMax;
extern gui_coord        WndScreen;
extern bool             WndOkToShow;
extern bool             WndIgnoreAllEvents;

extern bool             WndProcMacro( a_window *wnd, unsigned key );

extern  void            WndSetTitle( a_window *wnd, char *title );
extern  int             WndGetTitle( a_window *wnd, char *buff, unsigned buff_len );

extern void             Ring( void );

extern void             WndSysInit( void );
extern void             WndDoInput( void );
extern void             WndStartFreshAll( void );
extern void             WndEndFreshAll( void );
extern void             WndZapped( a_window * );

extern a_window         *WndFindClass( a_window *, wnd_class );
extern void             WndForAllClass( wnd_class class, void (*rtn)( a_window * ) );

extern int              SpawnP( void (*func)( void * ), void *parm );
extern int              Spawn( void (*func)( void ) );
extern void             Suicide( void );

#define WND_ALNUM_CHAR  '@'
#define WND_ALNUM_STR   "@"
extern  char            *WndSetIDChars( a_window *, char * );

extern  bool            WndIDChar( a_window *, char ch );
extern  bool            WndKeyChar( char ch );

extern void             WndCursorRight( a_window *wnd );
extern void             WndCursorLeft( a_window *wnd );
extern bool             WndTabLeft( a_window *wnd, bool wrap );
extern bool             WndTabRight( a_window *wnd, bool wrap );

extern void             WndCursorStart( a_window *wnd );
extern void             WndCursorEnd( a_window *wnd );

extern gui_ord          WndVScrollWidth( a_window *wnd );
extern void             WndResetScroll( a_window *wnd );
extern void             WndPageDown( a_window *wnd );
extern void             WndPageUp( a_window *wnd );
extern void             WndCursorDown( a_window *wnd );
extern void             WndScrollDown( a_window *wnd );
extern void             WndScrollTop( a_window *wnd );
extern void             WndScrollBottom( a_window *wnd );
extern void             WndCursorUp( a_window *wnd );
extern void             WndScrollUp( a_window *wnd );
extern void             WndSetVScrollRange( a_window *wnd, wnd_row rows );
extern void             WndChooseNew( void );
extern void             WndKeyPopUp( a_window *, gui_menu_struct * );
extern void             WndPopUp( a_window *, gui_menu_struct * );
extern void             WndMenuIgnoreAll( a_window *wnd );
extern void             WndMenuRespectAll( a_window *wnd );
extern void             WndMenuEnableAll( a_window *wnd );
extern void             WndMenuGrayAll( a_window *wnd );
extern void             WndMenuIgnore( a_window *wnd, int id, bool ignore );
extern void             WndMenuEnable( a_window *wnd, int id, bool enable );
extern void             WndMenuCheck( a_window *wnd, int id, bool check );
extern void             WndCheckMainMenu( int id, bool check );
extern void             WndEnableMainMenu( int id, bool enable );
//extern void           WndSetHintText( a_window *wnd, int id, char *text );

extern  void            WndPieceDirty( a_window *wnd, wnd_row row, int piece );
extern void             WndRowDirty( a_window *wnd, wnd_row row );
extern void             WndRowDirtyImmed( a_window *wnd, wnd_row row );
extern void             WndDirty( a_window * );
extern void             WndDirtyCurr( a_window * );
extern void             WndSetColours( a_window *, int, gui_colour_set *);
extern void             WndBackGround( gui_colour colour );
extern void             WndDirtyRect( a_window *wnd, gui_ord x, wnd_row y,
                                      gui_ord width, wnd_row height );

extern void             WndReDrawAll( void );
extern void             WndSetIcon( a_window *, gui_resource *);
extern void             WndSetMainMenuText( gui_menu_struct * );
extern void             WndShowAll( void );
extern void             WndShowWndMain( void );
extern void             WndInitWndMain( wnd_create_struct *);
extern void             WndShowWindow( a_window *wnd );
extern void             WndResizeWindow( a_window *wnd, gui_rect * );
extern void             WndMinimizeWindow( a_window *wnd );
extern void             WndMaximizeWindow( a_window *wnd );
extern bool             WndIsMinimized( a_window *wnd );
extern bool             WndIsMaximized( a_window *wnd );
extern void             WndRestoreWindow( a_window *wnd );
extern void             WndGetRect( a_window *wnd, gui_rect *rect );

extern void             WndStartChoose( a_window *wnd );

extern void             WndCreateToolBar( gui_ord, bool, int, gui_toolbar_struct * );
extern void             WndCreateToolBarWithTips( gui_ord, bool, int, gui_toolbar_struct * );
extern bool             WndHaveToolBar( void );
extern void             WndCloseToolBar( void );
extern gui_ord          WndToolHeight( void );
extern bool             WndToolFixed( void );

extern void             *WndHourGlass( void * );
extern void             *WndHourCursor( void );
extern void             *WndArrowCursor( void );

extern void             WndCreateStatusWindow( gui_colour_set * );
extern bool             WndStatusText( char * );
extern bool             WndHaveStatusWindow( void );
extern void             WndCloseStatusWindow( void );

extern void             WndMainResized( void );
extern bool             WndShutDownHook( void );
extern bool             WndQueryEndSessionHook( void );
extern void             WndEndSessionHook( void );
extern void             WndResizeHook( a_window * );
extern void             WndFontHook( a_window * );

extern void             WndSetTitleSize( a_window *, int );
extern void             WndForcePaint( a_window *wnd );

extern bool             WndDoingSearch;

extern bool             WndDisplayHelp( char *file, char *topic );

extern bool             WndSetFontInfo( a_window *,char* );
extern bool             WndSetSysFont( a_window *, bool fixed );
extern char             *WndGetFontInfo( a_window * );

extern void             *WndInitHistory( void );
extern void             WndFiniHistory( void * );
extern bool             WndNextFromHistory( save_area *save, char *cmd );
extern bool             WndPrevFromHistory( save_area *save, char *cmd );
extern void             WndSaveToHistory( save_area *save, char *cmd );

extern void             WndSetDClick( int );
extern int              WndGetDClick( void );

extern char             *WndLoadString( int );
extern void             NullPopupMenu( gui_menu_struct *menu );

extern void             WndChangeMenuAll( gui_menu_struct *menu, int num_popups, bool on, int bit );
extern gui_message_return WndDisplayMessage( char *msg, char *cap,
                                             gui_message_type type );

extern void     WndRectToPos( gui_rect *rect, wnd_posn *posn, gui_coord *scale );
extern void     WndPosToRect( wnd_posn *posn, gui_rect *rect, gui_coord *scale );
typedef bool (WNDCLICKHOOK)( a_window *wnd, unsigned id );
extern void WndInstallClickHook( WNDCLICKHOOK *rtn );

extern void     WndForceRefresh( void );
extern void     WndSetWndMainSize( wnd_create_struct *info );

typedef int wnd_gadget_type;
#define WND_GADGET_NONE ((wnd_gadget_type)-1)
extern void             WndGadgetInit( void );
extern void             WndSetGadgetLine( a_window *wnd, wnd_line_piece *line, wnd_gadget_type type, unsigned length );
extern void             WndGetGadgetSize( wnd_gadget_type type, gui_coord * );
extern wnd_gadget_type  WndGadgetSecondary;
extern char             *WndGadgetHint[];
extern gui_resource     WndGadgetArray[];
extern int              WndGadgetArraySize;
extern wnd_attr         WndGadgetAttr;

extern wnd_info         NoInfo;

extern wnd_attr         WndPlainAttr;
extern wnd_attr         WndTabStopAttr;
extern wnd_attr         WndSelectedAttr;
extern wnd_attr         WndCursorAttr;
extern wnd_attr         WndMapTabAttr( wnd_attr );
extern int              WndMaxDirtyRects;
extern gui_window_styles        WndStyle;
extern char             WndBackgroundChar;

#define ArraySize( x ) ( sizeof( x ) / sizeof( (x)[0] ) )
#define WndSetPopUpMenu( wnd, x, num ) { \
    __WND_FIELD( wnd, num_popups ) = num; \
    __WND_FIELD( wnd, popupmenu ) = x; }
#define DefPopUp( x ) ArraySize( x ), x
#define NoPopUp       0, NULL

#define WndMenuSize( x ) ArraySize( x )
#define WndMenuFields( x ) WndMenuSize( x ), x
extern  void    WndSetMainMenu( gui_menu_struct *menu, int num_menus );

// debugging stuff

#ifndef OLD_GUI
#define Say( x ) \
        WndDisplayMessage( (char*)(x), "Information", GUI_INFORMATION ); \

#define Say2( y, x ) \
        WndDisplayMessage( (char*)(x), (char*)y, GUI_INFORMATION );
#else
#define Say( x ) \
        GUIDisplayMessage( (char*)(x), "Information", GUI_INFORMATION ); \

#define Say2( y, x ) \
        GUIDisplayMessage( (char*)(x), (char*)y, GUI_INFORMATION );
#endif

#define GUI_IS_ASCII( x ) ( (x) < 256 )


#endif // _AUI_H_INCLUDED
