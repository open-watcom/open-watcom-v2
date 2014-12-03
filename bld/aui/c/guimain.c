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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"//
#include <ctype.h>

gui_coord               WndMax;
gui_coord               WndScreen;
gui_coord               WndScale;
a_window                *WndMain;
wnd_switches            WndSwitches;
int                     WndMaxDirtyRects = 6;
int                     WndDClick = 300;
bool                    WndIgnoreAllEvents = FALSE;

static int              wndProcNesting = 0;
static gui_colour_set Colours = { GUI_BLACK, GUI_BLACK };
static gui_mouse_cursor wndCursorType;
static char *           BusyString;


int WndGetDClick( void )
{
    return( WndDClick );
}

void WndSetDClick( int new )
{
    WndDClick = new;
    GUISetDClickRate( new );
}

static void Rescale( void )
{
    gui_coord   scale;
    gui_rect    rect;

    scale.x = WND_APPROX_SIZE;
    scale.y = WND_APPROX_SIZE;
    WndScreen = scale;
    GUIGetRoundScale( &scale );
    WndScale = scale;
    rect.x = 0;
    rect.y = 0;
    rect.width = scale.x;
    rect.height = scale.y;
    GUISetScale( &rect );
    WndMax = scale;
}

static bool WndInitBody( char *title, int resource_menu )
{
    wnd_create_struct   info;

    if( !GUIWndInit( WndDClick, WndStyle ) ) return( FALSE );
    GUIMDIInitMenuOnly();
    GUI3DDialogInit();
    GUISetBackgroundColour( &Colours );
    GUISetBetweenTitles( 2 );
    Rescale();
    WndSysInit();
    WndInitWndMain( &info );
    info.title = title;
    info.info = &NoInfo;
    info.class = WND_NO_CLASS;
    info.extra = NULL;
    WndSetWndMainSize( &info );
    WndMain = WndCreateWithStructAndMenuRes( &info, resource_menu );
    Rescale();
    if( WndBackgroundChar == 0 ) {
        WndBackgroundChar = GUIGetCharacter( GUI_SCROLL_SLIDER );
    }
    GUISetBackgroundChar( WndMain->gui, WndBackgroundChar );
    WndSetWndMax();
    return( TRUE );
}

bool WndInit( char *title )
{
    return( WndInitBody( title, 0 ) );
}

bool WndInitWithMenuRes( char *title, int resource_menu )
{
    return( WndInitBody( title, resource_menu ) );
}

void WndShowWndMain( void )
{
    WndShowWindow( WndMain );
}

bool WndFini( void )
{
    if( !WndShutDownHook() ) return( FALSE );
    GUIDestroyWnd( NULL );
    if( BusyString != NULL ) {
        WndFree( BusyString );
        BusyString = NULL;
    }
    return( TRUE );
}


void WndInitNumRows( a_window *wnd )
{
    wnd->rows = GUIGetNumRows( wnd->gui );
}


static void WndMoveResize( a_window *wnd )
{
    gui_text_metrics    text;
    gui_rect            rect;
    wnd_coord           save_curr;

    if( WndMain != NULL && WndIsMinimized( WndMain ) ) return;
    GUIGetTextMetrics( wnd->gui, &text );
    GUIGetClientRect( wnd->gui, &rect );
    wnd->width = rect.width;
    wnd->max_char.x = text.max.x;
    wnd->max_char.y = text.max.y;
    wnd->avg_char_x = text.avg.x;
    wnd->mid_char_x = ( text.max.x + text.avg.x ) / 2;
    WndInitNumRows( wnd );
    if( wnd->max_indent != 0 ) {
        if( wnd->width >= wnd->max_indent ) {
            WndSetHScroll( wnd, 0 );
        }
        GUISetHScrollRange( wnd->gui, wnd->max_indent );
    }
    if( WndHasCurrent( wnd ) ) {
        if( wnd->current.row >= wnd->rows ) {
            save_curr = wnd->current;
            WndDirtyCurr( wnd );
            WndScroll( wnd, wnd->current.row - wnd->rows + 1 );
            wnd->current = save_curr;
            wnd->current.row = wnd->rows - 1;
        }
        WndDirtyCurr( wnd );
    }
    WndKillCacheLines( wnd );
    if( wnd == WndMain ) {
        WndSetWndMax();
    }
    WndResizeHook( wnd );
}

static void WndKeyEnter( a_window *wnd )
{
    if( WndHasCurrent( wnd ) ) {
        if( !WndPieceIsTab( wnd, wnd->current.row, wnd->current.piece ) &&
            _Is( wnd, WSW_ONLY_MODIFY_TABSTOP ) ) {
            return;
        }
        WndModify( wnd, wnd->current.row, wnd->current.piece );
    } else {
        if( _Is( wnd, WSW_ONLY_MODIFY_TABSTOP ) ) {
            return;
        }
        WndModify( wnd, WND_NO_ROW, WND_NO_PIECE );
    }
}


typedef struct {
    gui_window  *gui;
    gui_event   event;
    void        *parm;
    bool        ret;
} spawn_parms;

WNDCLICKHOOK *WndClickHook;
void WndInstallClickHook( WNDCLICKHOOK *rtn )
{
    WndClickHook = rtn;
}


static void DoMainEventProc( spawn_parms *spawnp )
{
    a_window            *wnd;
    bool                ret;
    gui_key             key;
    gui_keystate        state;
    unsigned            id;
    int                 scroll;
    void                *cursor;

    gui_window          *gui = spawnp->gui;
    gui_event           event = spawnp->event;
    void                *parm = spawnp->parm;

    wnd = GUIGetExtra( gui );
    spawnp->ret = FALSE;
    if( wnd == NULL ) return;
    if( WndIgnoreAllEvents ) {
        if( event == GUI_PAINT ) {
            WndRepaint( wnd );
        }
        if( event == GUI_MOUSEMOVE && GUIIsGUI() ) {
            GUISetMouseCursor( GUI_HOURGLASS_CURSOR );
        }
        return;
    }
    if( event == GUI_MOUSEMOVE ) {
        GUISetMouseCursor( wndCursorType );
        if( WndIgnoreMouseMove( wnd ) ) return;
    }
    if( !WndDoingRefresh && wndProcNesting == 1 ) WndDoInput();
    ret = TRUE;

    WndChooseEvent( wnd, event, parm );
    WndSelectEvent( wnd, event, parm );
    switch( event ) {
    case GUI_STATUS_CLEARED:
        return;
    case GUI_ACTIVATEAPP:
        return;
    case GUI_INITMENUPOPUP:
        GUI_GETID( parm, id );
        WndSetPopup( id );
        break;
    case GUI_INIT_WINDOW:
        WndSetPopUpMenu( wnd, wnd->info->popupmenu, wnd->info->num_popups );
        wnd->gui = gui;
        cursor = WndHourGlass( NULL );
        WndMoveResize( wnd );
        ret = WndEvent( wnd, event, parm );
        WndSetThumb( wnd );
        WndHourGlass( cursor );
        break;
    case GUI_SCROLL_VERTICAL:
        GUI_GET_SCROLL( parm, scroll );
        WndScroll( wnd, scroll );
        break;
    case GUI_SCROLL_UP:
        WndScrollUp( wnd );
        break;
    case GUI_SCROLL_DOWN:
        WndScrollDown( wnd );
        break;
    case GUI_SCROLL_PAGE_UP:
        if( WndEvent( wnd, GUI_SCROLL_PAGE_UP, parm ) ) break;
        WndPageUp( wnd );
        break;
    case GUI_SCROLL_PAGE_DOWN:
        if( WndEvent( wnd, GUI_SCROLL_PAGE_DOWN, parm ) ) break;
        WndPageDown( wnd );
        break;
    case GUI_NOW_ACTIVE:
        if( wnd == WndMain ) {
            WndNextNonIconToFront( WndNext( NULL ) );
        } else {
            _Set( wnd, WSW_ACTIVE );
            WndAddPopupMenu( wnd );
            WndEvent( wnd, event, parm );
        }
        ret = FALSE;
        break;
    case GUI_NOT_ACTIVE:
        _Clr( wnd, WSW_ACTIVE );
        if( wnd != WndMain ) {
            WndEvent( wnd, event, parm );
        }
        ret = FALSE;
        break;
    case GUI_MOVE:
        if( WndMain != NULL && WndIsMinimized( WndMain ) ) return;
        WndResizeHook( wnd );
        break;
    case GUI_FONT_CHANGED :
        if( wnd == WndMain ) {
            GUIResizeStatusWindow( gui, 0, 0 );
        }
        WndFontHook( wnd );
        WndResetScroll( wnd );
    case GUI_RESIZE:
        _Clr( wnd, WSW_ICONIFIED );
        cursor = WndHourGlass( NULL );
        WndMoveResize( wnd );
        ret = WndEvent( wnd, GUI_RESIZE, parm );
        WndSetThumb( wnd );
        WndHourGlass( cursor );
        break;
    case GUI_LBUTTONDBLCLK:
        _Clr( wnd, WSW_CLICKED );
        _Set( wnd, WSW_DCLICKED );
        WndLButtonDown( wnd, parm );
        break;
    case GUI_LBUTTONDOWN:
        _Clr( wnd, WSW_DCLICKED );
        _Set( wnd, WSW_CLICKED );
        WndLButtonDown( wnd, parm );
        break;
    case GUI_LBUTTONUP:
        cursor = WndHourGlass( NULL );
        if( _Is( wnd, WSW_DCLICKED ) ) {
            WndLDblClk( wnd, parm );
        } else if( _Is( wnd, WSW_CLICKED ) ) {
            WndLButtonUp( wnd, parm );
        }
        _Clr( wnd, WSW_CLICKED );
        _Clr( wnd, WSW_DCLICKED );
        WndHourGlass( cursor );
        break;
    case GUI_RBUTTONDOWN:
        WndRButtonDown( wnd, parm );
        break;
    case GUI_RBUTTONUP:
        cursor = WndHourGlass( NULL );
        WndRButtonUp( wnd, parm );
        WndHourGlass( cursor );
        break;
    case GUI_MOUSEMOVE:
        WndMouseMove( wnd, parm );
        break;
    case GUI_CLICKED:
        GUI_GETID( parm, id );
        if( WndClickHook != NULL && WndClickHook( wnd, id ) ) break;
        cursor = WndHourGlass( NULL );
        WndClick( wnd, id );
        WndHourGlass( cursor );
        break;
    case GUI_PAINT:
        if( _Isnt( wnd, WSW_REPAINT ) ) { // going to repaint anyway
            WndProcPaint( wnd, parm );
        }
        break;
    case GUI_SCROLL_BOTTOM :
        WndScrollBottom( wnd );
        break;
    case GUI_SCROLL_TOP :
        WndScrollTop( wnd );
        break;
    case GUI_KEYUP:
        ret = FALSE;
        break;
    case GUI_KEYDOWN:
        GUI_GET_KEY_STATE( parm, key, state );
        if( GUI_CTRL( state ) ) {
            switch( key ) {
            case GUI_KEY_TAB:
                key = GUI_KEY_CTRL_I;
                break;
            case GUI_KEY_ENTER:
                key = GUI_KEY_CTRL_M;
                break;
            case GUI_KEY_BACKSPACE:
                key = GUI_KEY_CTRL_H;
                break;
            case GUI_KEY_ESCAPE:
                key = GUI_KEY_CTRL_LEFT_BRACKET;
                break;
            default :
                break;
            }
        }
        if( GUI_IS_ASCII( key ) && _Is( wnd, WSW_CHOOSING ) &&
            wnd->keypiece != WND_NO_PIECE && WndKeyChar( key ) ) {
            if( !WndKeyChoose( wnd, key ) ) {
                Ring();
            }
            break;
        }
        cursor = WndHourGlass( NULL );
        if( !WndProcMacro( wnd, key ) ) {
            switch( key ) {
            case GUI_KEY_HOME:
                if( GUI_SHIFT( state ) ) WndToSelectMode( wnd );
                WndCursorStart( wnd );
                break;
            case GUI_KEY_ESCAPE:
                WndKeyEscape( wnd );
                break;
            case GUI_KEY_END:
                if( GUI_SHIFT( state ) ) WndToSelectMode( wnd );
                WndCursorEnd( wnd );
                break;
            case GUI_KEY_BACKSPACE:
                WndKeyRubOut( wnd );
                break;
            case GUI_KEY_ENTER:
                WndKeyEnter( wnd );
                break;
            case GUI_KEY_SHIFT_TAB:
                WndTabLeft( wnd, TRUE );
                break;
            case GUI_KEY_TAB:
                WndTabRight( wnd, TRUE );
                break;
            case GUI_KEY_LEFT:
                if( GUI_SHIFT( state ) ) WndToSelectMode( wnd );
                WndCursorLeft( wnd );
                break;
            case GUI_KEY_RIGHT:
                if( GUI_SHIFT( state ) ) WndToSelectMode( wnd );
                WndCursorRight( wnd );
                break;
            case GUI_KEY_PAGEDOWN:
                if( WndEvent( wnd, GUI_SCROLL_PAGE_DOWN, parm ) ) break;
                WndPageDown( wnd );
                break;
            case GUI_KEY_PAGEUP:
                if( WndEvent( wnd, GUI_SCROLL_PAGE_UP, parm ) ) break;
                WndPageUp( wnd );
                break;
            case GUI_KEY_UP:
                if( _Is( wnd, WSW_MAP_CURSOR_TO_SCROLL ) ) {
                    WndScrollUp( wnd );
                } else {
                    if( _Is( wnd, WSW_MULTILINE_SELECT ) && GUI_SHIFT( state ) ) {
                        WndToSelectMode( wnd );
                    }
                    WndCursorUp( wnd );
                }
                break;
            case GUI_KEY_DOWN:
                if( _Is( wnd, WSW_MAP_CURSOR_TO_SCROLL ) ) {
                    WndScrollDown( wnd );
                } else {
                    if( _Is( wnd, WSW_MULTILINE_SELECT ) && GUI_SHIFT( state ) ) {
                        WndToSelectMode( wnd );
                    }
                    WndCursorDown( wnd );
                }
                break;
            case GUI_KEY_CTRL_HOME:
                WndScrollTop( wnd );
                break;
            case GUI_KEY_CTRL_END:
                WndScrollBottom( wnd );
                break;
            default:
                ret = FALSE;
                break;
            }
        }
        if( ret ) {
            GUIFlushKeys();
        }
        WndHourGlass( cursor );
        break;
    case GUI_DESTROY:
        WndDestroy( wnd );
        break;
    case GUI_ICONIFIED:
        _Set( wnd, WSW_ICONIFIED );
        if( wnd == WndNext( NULL ) ) {
            WndNextNonIconToFront( WndNext( wnd ) );
        }
        break;
    case GUI_QUERYENDSESSION:
        ret = !WndQueryEndSessionHook();
        break;
    case GUI_ENDSESSION:
        {
            bool            endsession;

            GUI_GET_BOOL( parm, endsession );
            if( endsession ) {
                WndEndSessionHook();
            }
        }
        break;
    case GUI_CLOSE:
        if( wnd == WndMain ) {
            ret = WndShutDownHook();
        } else {
            WndResizeHook( wnd );
            ret = TRUE;
        }
        break;
    case GUI_TOOLBAR_FLOATING:
    case GUI_TOOLBAR_FIXED:
    case GUI_TOOLBAR_DESTROYED:
        WndSetToolBar( event );
        ret = TRUE;
        break;
    case GUI_HSCROLL_NOTIFY:
        WndHScrollNotify( wnd );
        break;
    default:
        ret = FALSE;
        break;
    }
    if( !WndDoingRefresh && wndProcNesting == 1 ) WndDoInput();
    if( WndValid( wnd ) ) { // could have been freed
        WndEndSelectEvent( wnd );
        WndSayMatchMode( wnd );
    }
    WndFreshAll();
    spawnp->ret = ret;
    return;
}


void *WndHourCursor( void )
{
    if( GUIIsGUI() ) {
        wndCursorType = GUI_HOURGLASS_CURSOR;
        return( GUISetMouseCursor( GUI_HOURGLASS_CURSOR ) );
    } else {
        if( BusyString == NULL ) {
            BusyString = WndLoadString( LITERAL_Busy );
        }
        WndInternalStatusText( BusyString );
        return( "" );
    }
}

void *WndArrowCursor( void )
{
    if( GUIIsGUI() ) {
        wndCursorType = GUI_ARROW_CURSOR;
        return( GUISetMouseCursor( GUI_ARROW_CURSOR ) );
    } else {
        WndResetStatusText();
        return( "" );
    }
}

void *WndSetCursor( void *to )
{
    if( GUIIsGUI() ) {
        wndCursorType = GUI_ARROW_CURSOR;
        GUIResetMouseCursor( to );
    } else {
        WndResetStatusText();
    }
    return( NULL );
}

void *WndHourGlass( void *to )
{
    if( to != NULL ) {
        return( WndSetCursor( to ) );
    } else {
        return( WndHourCursor() );
    }
}


extern GUICALLBACK WndMainEventProc;
bool WndMainEventProc( gui_window * gui, gui_event event, void *parm )
{
    spawn_parms         spawnp;

    spawnp.gui = gui;
    spawnp.event = event;
    spawnp.parm = parm;
    wndProcNesting++;
    SpawnP( ( void (*) (void *) )DoMainEventProc, &spawnp );
    wndProcNesting--;
    return( spawnp.ret );
}
