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
* Description:  Public interface to the AUI (Advanced UI) library.
*
****************************************************************************/


#ifndef _AUI_H_INCLUDED
#define _AUI_H_INCLUDED

#include <limits.h>
#include "gui.h"
#include "guikey.h"
#include "guifdlg.h"


#define SAVE_SIZE           512

#define GUI_KEY_IS_ASCII(x) ((x) < 256)

// debugging stuff
#define Say(x)              WndDisplayMessage( x, "Information", GUI_INFORMATION );
#define Say2(y,x)           WndDisplayMessage( x, y, GUI_INFORMATION );

#define MIN_DCLICK          100 /* ms */
#define MAX_DCLICK          1500 /* ms */
#define MAX_MAGIC_STR       20
#define WND_MAX_PIECE       UCHAR_MAX
#define WND_NO_PIECE        UCHAR_MAX
#define MAX_KEY_SIZE        256
#define MAX_POPUPS          20
#define WND_MAX_COLIDX      SHRT_MAX
#define WND_NO_COLIDX       WND_MAX_COLIDX
// make sure WND_MAX_ROW + WndRows(wnd) will never wrap
#define WND_MAX_ROW         (SHRT_MAX - 500)
#define WND_NO_ROW          (-WND_MAX_ROW)
#define WND_APPROX_SIZE     10000
#define WND_NO_CLASS        ((wnd_class)-1)

#define WND_SAVE_ROW        0
#define WND_RESTORE_ROW     1

#define WND_GUIEVENT        WndMainGUIEventProc

#define WND_MENU_POPUP      GUI_STYLE_MENU_UTIL_1

#define WSW_LBUTTON_SELECTS             0x00000001L     // default off
#define WSW_MULTILINE_SELECT            0x00000002L     // default off
#define WSW_SUBWORD_SELECT              0x00000004L     // default off
#define WSW_MAP_CURSOR_TO_SCROLL        0x00000008L     // default off
#define WSW_RBUTTON_CHANGE_CURR         0x00000010L     // default off
#define WSW_SELECT_IN_TABSTOP           0x00000020L     // default on
#define WSW_MUST_CLICK_ON_PIECE         0x00000040L     // default off
#define WSW_RBUTTON_SELECTS             0x00000080L     // default off
#define WSW_CHOOSING                    0x00000100L     // default off
#define WSW_CHAR_CURSOR                 0x00000200L     // default off
#define WSW_ALLOW_POPUP                 0x00000400L     // default on
#define WSW_SEARCH_WRAP                 0x00000800L     // default on
#define WSW_HIGHLIGHT_CURRENT           0x00001000L     // default on
#define WSW_ONLY_MODIFY_TABSTOP         0x00002000L     // default on
#define WSW_UTIL_1                      0x00100000L
#define WSW_CLICKED                     0x00200000L
#define WSW_DCLICKED                    0x00400000L
#define WSW_CACHE_LINES                 0x00800000L
#define WSW_SELECTING_WITH_KEYBOARD     0x01000000L
#define WSW_SELECT_EVENT                0x02000000L
#define WSW_REPAINT                     0x04000000L
#define WSW_ALTERNATE_BIT               0x08000000L     // default off
#define WSW_ICONIFIED                   0x10000000L
#define WSW_SELECTING                   0x20000000L
#define WSW_ACTIVE                      0x40000000L
#define WSW_MENU_ACCURATE_ROW           0x80000000L

#define WndExtra( w )               (w)->extra
#define WndPopItem( w )             (w)->popitem
#define WndSrchItem( w )            (w)->searchitem
#define WndMaxCharX( w )            (w)->max_char.x
#define WndMaxCharY( w )            (w)->max_char.y
#define WndAvgCharX( w )            (w)->avg_char_x
#define WndMidCharX( w )            (w)->mid_char_x
#define WndTop( w )                 (w)->top
#define WndSetTop( w, x )           (w)->top = (x)
#define WndHasClass( w )            ((w)->wndclass != WND_NO_CLASS)
#define WndClass( w )               (w)->wndclass
#define WndTitleRows( w )           (w)->title_rows
#define WndRows( w )                ((w)->rows - (w)->title_rows)
#define WndMaxRow( w )              (w)->max_row
#define WndGui( w )                 (w)->gui
#define WndWidth( w )               (w)->width
#define WndSetRepaint( w )          (w)->switches |= WSW_REPAINT
#define WndSetKeyPiece( w, x )      (w)->keypiece = (x)
#define WndKeyPiece( w )            (w)->keypiece
#define WndSetSwitches( w, x )      (w)->switches |= (x)
#define WndClrSwitches( w, x )      (w)->switches &= ~(x)
#define WndSwitchOn( w, x )         (((w)->switches & (x)) != 0)
#define WndSwitchOff( w, x )        (((w)->switches & (x)) == 0)
#define WndNumPopups( w )           (w)->popup.num_items
#define WndPopupMenu( w )           (w)->popup.menu
#define WndSetPopUpMenu( w, n, x )  {(w)->popup.num_items = (n); (w)->popup.menu = (x);}

typedef struct {
    unsigned char       area[SAVE_SIZE];
    unsigned            first_cmd;
    unsigned            first_free;
    unsigned            curr_cmd;
    bool                last_was_next;
} save_area;

typedef enum {
    WND_FIRST_UNUSED = GUI_FIRST_UNUSED,
} wnd_attr;

typedef unsigned long   wnd_switches;

typedef int             wnd_row;
typedef int             wnd_colidx;
typedef unsigned char   wnd_piece;

typedef signed char     wnd_class;

typedef struct wnd_line_piece {
    const char  *text;                  // default ""
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
    size_t      length;                 // INTERNAL -- do not use
    const char  *hint;                  // set for Hint Text
} wnd_line_piece;

#define WND_NO_EXTEND   0
#define WND_MAX_EXTEND  GUI_NO_COLUMN

typedef struct wnd_create_struct {
    struct wnd_info     *info;
    char                *title;
    wnd_class           wndclass;
    void                *extra;
    gui_create_styles   style;
    gui_scroll_styles   scroll_style;
    gui_colour_set      *colour;
    gui_rect            rect;
    wnd_row             title_rows;
} wnd_create_struct;

typedef struct wnd_posn {
    float               x;
    float               y;
    float               width;
    float               height;
} wnd_posn;

typedef int wnd_menu_id;

#define MENU_INITIALIZE ((wnd_menu_id)-1)

typedef struct {
    wnd_row             row;
    wnd_piece           piece;
    wnd_colidx          colidx;
} wnd_coord;

typedef struct {
    wnd_row             row;
    wnd_piece           piece;
    wnd_colidx          colidx;
    wnd_colidx          end_colidx;
} wnd_rect;

typedef struct {
    int                 min_rows;
    int                 min_cols;
    int                 max_rows;
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

typedef struct _a_window {
    gui_window              *gui;
    struct {
        wnd_row     row;
        wnd_piece   piece;
    }                       button_down;
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
    wnd_row                 top;
    int                     keyindex;
    wnd_piece               keypiece;
    wnd_class               wndclass;
    wnd_switches            switches;
    int                     vscroll_pending;
    int                     hscroll_pending;
    wnd_row                 max_row;
    const char              *select_chars;
    wnd_row                 title_rows;
    gui_ord                 avg_char_x;
    gui_ord                 mid_char_x;
    gui_ctl_id              last_popup;
    wnd_colidx              current_colidx;
    gui_menu_items          popup;
    int                     dirtyrects;
    wnd_rect                dirty[1];
} *a_window;

typedef bool        (WNDCALLBACK)( a_window, gui_event, void * );
typedef void        (WNDREFRESH)( a_window );
typedef void        (WNDMENU)( a_window, gui_ctl_id id, wnd_row, wnd_piece );
typedef void        (WNDMODIFY)( a_window, wnd_row, wnd_piece );
typedef int         (WNDSCROLL)( a_window, int );
typedef wnd_row     (WNDNUMROWS)( a_window );
typedef wnd_row     (WNDNEXTROW)( a_window, wnd_row, int );
typedef bool        (WNDGETLINE)( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece * );
typedef void        (WNDNOTIFY)( a_window wnd, wnd_row row, wnd_piece piece );
typedef void        (WNDBEGPAINT)( a_window wnd, wnd_row row, int num );
typedef void        (WNDENDPAINT)( a_window wnd, wnd_row row, int num );
typedef bool        (WNDCHKUPDATE)( void );
typedef a_window    (WNDOPEN)( void );
typedef a_window    (WNDCREATE)( char *, struct wnd_info *, wnd_class, void * );
typedef void        (WNDCLOSE)( a_window );
typedef bool        (WNDPICKER)( const char *, GUIPICKCALLBACK *, int * );
typedef bool        (WNDCLICKHOOK)( a_window wnd, gui_ctl_id id );

typedef struct wnd_info {
    WNDCALLBACK             *event;
    WNDREFRESH              *refresh;
    WNDGETLINE              *getline;
    WNDMENU                 *menuitem;
    WNDSCROLL               *vscroll;
    WNDBEGPAINT             *begpaint;
    WNDENDPAINT             *endpaint;
    WNDMODIFY               *modify;
    WNDNUMROWS              *numrows;
    WNDNEXTROW              *nextrow;
    WNDNOTIFY               *notify;
    WNDCHKUPDATE            *chkupdate;
    gui_menu_items          popup;
} wnd_info;

typedef struct {
    char        *buff;
    size_t      buff_len;
    bool        cancel;
} dlgnew_ctl;

extern bool                 DlgPickWithRtn( const char *title, const void *data_handle, int def_item, GUIPICKGETTEXT *getstring, int num_items, int *choice );
extern bool                 DlgPickWithRtn2( const char *title, const void *data_handle, int def_item, GUIPICKGETTEXT *getstring, int num_items, WNDPICKER *pickfn, int *choice );
extern bool                 DlgPick( const char *title, const void *data_handle, int def_item, int num_items, int *choice );
extern bool                 DlgNew( const char *title, char *buff, size_t buff_len );
extern bool                 DlgNewWithCtl( const char *title, char *buff, size_t buff_len, gui_control_info *controls, int num_controls,
                                            GUICALLBACK *gui_call_back, gui_text_ord rows, gui_text_ord cols, gui_text_ord max_cols );
extern void                 DlgOpen( const char *title, gui_text_ord, gui_text_ord, gui_control_info *, int, GUICALLBACK *, void * );
extern void                 ResDlgOpen( GUICALLBACK *, void *, int dlg_id );
extern int                  DlgGetFileName( open_file_name *ofn );
extern bool                 DlgFileBrowse( char *title, char *filter, char *path, unsigned len, fn_flags flags );
extern int                  DlgSearch( a_window wnd, void *history );
extern bool                 DlgSearchAll( char **expr, void *history );

extern a_window             WndMain;

//extern a_window             WndFindOwner( a_window );
//extern a_window             WndFirst( void );

extern gui_ord              WndExtentX( a_window, const char * );

extern void                 WndFreshAll( void );
extern bool                 WndStopRefresh( bool );

extern void                 WndNoMemory( void );
extern void                 WndMemInit( void );
extern void                 WndMemFini( void );
extern void                 *WndAlloc( size_t );
extern void                 *WndRealloc( void *, size_t );
extern void                 WndFree( void * );
extern void                 *WndMustAlloc( size_t );
extern void                 *WndMustRealloc( void *, size_t );
extern void                 WndCreateFloatingPopup( a_window, gui_point *, const gui_menu_items *, gui_ctl_id *id );

extern void                 WndFixedThumb( a_window );
extern void                 WndSetThumbPos( a_window, int );
extern void                 WndSetThumbPercent( a_window, int );
extern void                 WndSetThumb( a_window );
extern WNDSCROLL            WndVScroll;
extern WNDSCROLL            WndVScrollAbs;

extern wnd_row              WndCurrRow( a_window );
extern bool                 WndHasCurrent( a_window );
extern void                 WndNewCurrent( a_window, wnd_row, wnd_piece );
extern void                 WndMoveCurrent( a_window wnd, wnd_row row, wnd_piece piece );
extern void                 WndGetCurrent( a_window, wnd_row *, wnd_piece * );
extern void                 WndNoCurrent( a_window );
extern bool                 WndFirstCurrent( a_window );
extern bool                 WndLastCurrent( a_window );

extern void                 WndNoSelect( a_window );

extern void                 WndCleanUp( void );
extern a_window             WndFindActive( void );

extern void                 WndDestroy( a_window );
extern void                 WndClose( a_window );
extern void                 WndRestoreToFront( a_window );
extern void                 WndToFront( a_window );   // won't restore an icon!

extern void                 WndCurrToGUIPoint( a_window wnd, gui_point *point );

extern WNDCREATE            WndCreate;
extern void                 WndInitCreateStruct( wnd_create_struct * );
extern a_window             WndCreateWithStruct( wnd_create_struct * );
extern a_window             WndCreateWithStructAndMenuRes( wnd_create_struct *, res_name_or_id menu_id );
extern void                 WndShrinkToMouse( a_window wnd, wnd_metrics * );
extern bool                 WndInit( char *title );
extern bool                 WndInitWithMenuRes( char *title, res_name_or_id menu_id );
extern bool                 WndFini( void );
extern bool                 WndMainMenuProc( a_window, gui_ctl_id id );
extern GUICALLBACK          WndMainGUIEventProc;
extern void                 WndSetSrchItem( a_window wnd, const char *expr );
extern bool                 WndSearch( a_window, bool, int );
extern void                 WndInitNumRows( a_window );
extern void                 WndRXError( int );

extern a_window             WndNext( a_window );

extern wnd_info             NoInfo;

extern WNDCALLBACK          NoWndEventProc;
extern WNDREFRESH           NoRefresh;
extern WNDGETLINE           NoGetLine;
extern WNDMENU              NoMenuItem;
extern WNDMODIFY            NoModify;
extern WNDSCROLL            NoVScroll;
extern WNDBEGPAINT          NoBegPaint;
extern WNDENDPAINT          NoEndPaint;
extern WNDNOTIFY            NoNotify;
extern WNDNUMROWS           NoNumRows;
extern WNDNEXTROW           NoNextRow;

#define NoChkUpdate         NULL

extern WNDREFRESH           WndRefresh;
extern WNDGETLINE           WndGetLine;
extern WNDGETLINE           WndGetLineAbs;
extern WNDMENU              WndMenuItem;
extern WNDMODIFY            WndModify;
extern WNDMODIFY            WndFirstMenuItem;
extern WNDBEGPAINT          WndBegPaint;
extern WNDENDPAINT          WndEndPaint;
extern WNDNOTIFY            WndNotify;
extern WNDNUMROWS           WndNumRows;
extern WNDNEXTROW           WndNextRow;

extern bool                 WndHasNumRows( a_window );

extern wnd_switches         WndSwitches;
extern a_window             Windows;
extern gui_coord            WndMax;
extern gui_coord            WndScreen;
extern gui_coord            WndScale;
extern bool                 WndOkToShow;
extern bool                 WndIgnoreAllEvents;

extern void                 WndSetTitle( a_window wnd, const char *title );
extern int                  WndGetTitle( a_window wnd, char *buff, unsigned buff_len );

extern void                 Ring( void );

extern void                 WndSysInit( void );
extern void                 WndDoInput( void );
extern void                 WndZapped( a_window );

extern a_window             WndFindClass( a_window, wnd_class );
extern void                 WndForAllClass( wnd_class wndclass, void (*rtn)( a_window ) );

extern const char           *WndSetIDChars( a_window wnd, const char *id_chars );

extern bool                 WndIDChar( a_window, int ch );
extern bool                 WndKeyIsPrintChar( gui_key key );

extern void                 WndCursorRight( a_window wnd );
extern void                 WndCursorLeft( a_window wnd );
extern bool                 WndTabLeft( a_window wnd, bool wrap );
extern bool                 WndTabRight( a_window wnd, bool wrap );

extern void                 WndCursorStart( a_window wnd );
extern void                 WndCursorEnd( a_window wnd );

extern gui_ord              WndScrollBarWidth( a_window wnd );
extern void                 WndResetScroll( a_window wnd );
extern void                 WndPageDown( a_window wnd );
extern void                 WndPageUp( a_window wnd );
extern void                 WndCursorDown( a_window wnd );
extern void                 WndScrollDown( a_window wnd );
extern void                 WndScrollTop( a_window wnd );
extern void                 WndScrollBottom( a_window wnd );
extern void                 WndCursorUp( a_window wnd );
extern void                 WndScrollUp( a_window wnd );
extern void                 WndSetVScrollRange( a_window wnd, wnd_row rows );
extern void                 WndChooseNew( void );
extern void                 WndKeyPopUp( a_window, const gui_menu_struct * );
extern void                 WndPopUp( a_window, const gui_menu_struct * );
extern void                 WndMenuIgnoreAll( a_window wnd );
extern void                 WndMenuRespectAll( a_window wnd );
extern void                 WndMenuEnableAll( a_window wnd );
extern void                 WndMenuGrayAll( a_window wnd );
extern void                 WndMenuIgnore( a_window wnd, gui_ctl_id id, bool ignore );
extern void                 WndMenuEnable( a_window wnd, gui_ctl_id id, bool enable );
extern void                 WndMenuCheck( a_window wnd, gui_ctl_id id, bool check );
extern void                 WndCheckMainMenu( gui_ctl_id id, bool check );
extern void                 WndEnableMainMenu( gui_ctl_id id, bool enable );
//extern void                 WndSetHintText( a_window wnd, gui_ctl_id id, char *text );

extern void                 WndPieceDirty( a_window wnd, wnd_row row, wnd_piece piece );
extern void                 WndRowDirty( a_window wnd, wnd_row row );
extern void                 WndRowDirtyImmed( a_window wnd, wnd_row row );
extern void                 WndDirty( a_window );
extern void                 WndDirtyCurr( a_window );
extern void                 WndSetColours( a_window, int, gui_colour_set * );
extern void                 WndBackGround( gui_colour colour );
extern void                 WndDirtyRect( a_window wnd, gui_ord x, wnd_row y, gui_ord width, wnd_row height );

extern void                 WndReDrawAll( void );
extern void                 WndSetIcon( a_window, gui_resource * );
extern void                 WndSetMainMenuText( gui_menu_struct * );
extern void                 WndShowAll( void );
extern void                 WndShowWndMain( void );
extern void                 WndInitWndMain( wnd_create_struct * );
extern void                 WndShowWindow( a_window wnd );
extern void                 WndResizeWindow( a_window wnd, gui_rect * );
extern void                 WndMinimizeWindow( a_window wnd );
extern void                 WndMaximizeWindow( a_window wnd );
extern bool                 WndIsMinimized( a_window wnd );
extern bool                 WndIsMaximized( a_window wnd );
extern void                 WndRestoreWindow( a_window wnd );
extern void                 WndGetRect( a_window wnd, gui_rect *rect );

extern void                 WndStartChoose( a_window wnd );

extern void                 WndCreateToolBar( gui_ord, bool, const gui_toolbar_items * );
extern void                 WndCreateToolBarWithTips( gui_ord, bool, const gui_toolbar_items * );
extern bool                 WndHaveToolBar( void );
extern void                 WndCloseToolBar( void );
extern gui_ord              WndToolHeight( void );
extern bool                 WndToolFixed( void );

extern gui_mcursor_handle   WndHourGlass( gui_mcursor_handle );
extern gui_mcursor_handle   WndHourCursor( void );
extern gui_mcursor_handle   WndArrowCursor( void );

extern void                 WndCreateStatusWindow( gui_colour_set * );
extern bool                 WndStatusText( const char * );
extern bool                 WndHaveStatusWindow( void );
extern void                 WndCloseStatusWindow( void );

extern void                 WndMainResized( void );
extern bool                 WndShutDownHook( void );
extern bool                 WndQueryEndSessionHook( void );
extern void                 WndEndSessionHook( void );
extern void                 WndResizeHook( a_window );
extern void                 WndFontHook( a_window );

extern void                 WndSetTitleRows( a_window, int );
extern void                 WndForcePaint( a_window wnd );

extern bool                 WndDoingSearch;

extern bool                 WndDisplayHelp( char *file, char *topic );

extern bool                 WndSetFontInfo( a_window, char * );
extern bool                 WndSetSysFont( a_window, bool fixed );
extern char                 *WndGetFontInfo( a_window );

extern void                 *WndInitHistory( void );
extern void                 WndFiniHistory( void * );
extern bool                 WndNextFromHistory( save_area *save, char *cmd );
extern bool                 WndPrevFromHistory( save_area *save, char *cmd );
extern void                 WndSaveToHistory( save_area *save, char *cmd );

extern void                 WndSetDClick( unsigned dclick_ms );
extern unsigned             WndGetDClick( void );

extern char                 *WndLoadString( gui_res_id id );
extern void                 NullPopupMenu( const gui_menu_struct *menu );

extern void                 WndChangeMenuAll( gui_menu_items *menus, bool on, int bit );
extern gui_message_return   WndDisplayMessage( const char *msg, const char *cap, gui_message_type type );

extern void                 WndRectToPos( gui_rect *rect, wnd_posn *posn, gui_coord *scale );
extern void                 WndPosToRect( wnd_posn *posn, gui_rect *rect, gui_coord *scale );
extern void                 WndInstallClickHook( WNDCLICKHOOK *rtn );

extern void                 WndForceRefresh( void );
extern void                 WndSetWndMainSize( wnd_create_struct *info );

typedef int                 wnd_gadget_type;
#define WND_GADGET_NONE     ((wnd_gadget_type)-1)

extern void                 WndGadgetInit( void );
extern void                 WndSetGadgetLine( a_window wnd, wnd_line_piece *line, wnd_gadget_type type, size_t length );
extern void                 WndGetGadgetSize( wnd_gadget_type type, gui_coord * );

extern wnd_attr             WndMapTabAttr( wnd_attr );
extern char                 WndBackgroundChar;

#define PopUp( x )          (sizeof( x ) / sizeof( *(x) )), x
#define NoPopUp             0, NULL

#define WndMenuFields( x )  (sizeof( x ) / sizeof( *(x) )), x
extern void                 WndSetMainMenu( gui_menu_items *menus );

/* following function are all window refresh hooks */
/* may be defined in application otherwise default will be used */

extern void                 WndStartFreshAll( void );
extern void                 WndEndFreshAll( void );

/* following function may be defined in application otherwise default will be used */

extern bool                 WndProcMacro( a_window wnd, gui_key key );

/* following data may be setup in application */

extern int                  WndMaxDirtyRects;

/* following data must be setup in application if this feature is used */

extern gui_resource         WndGadgetArray[];
extern int                  WndGadgetArraySize;
extern char                 *WndGadgetHint[];
extern wnd_attr             WndGadgetAttr;
extern wnd_gadget_type      WndGadgetSecondary;

/* following data must be defined in application */

extern gui_menu_items       WndMainMenu;

/* following data may be defined in application otherwise default will be used */

extern gui_window_styles    WndStyle;

/* following block of data (colours/attributes) may be defined in application otherwise default will be used */

extern gui_colour_set       WndColours[];
extern int                  WndNumColours;

extern wnd_attr             WndPlainAttr;
extern wnd_attr             WndTabStopAttr;
extern wnd_attr             WndSelectedAttr;
extern wnd_attr             WndCursorAttr;

#endif // _AUI_H_INCLUDED
