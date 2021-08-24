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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "_aui.h"
#include <ctype.h>
#include "wspawn.h"


typedef struct {
    gui_window  *gui;
    gui_event   gui_ev;
    void        *parm;
    bool        ret;
} spawn_parms;

gui_coord               WndMax;
gui_coord               WndScreen;
gui_coord               WndScale;
a_window                WndMain;
wnd_switches            WndSwitches;
int                     WndMaxDirtyRects = 6;
bool                    WndIgnoreAllEvents = false;
WNDCLICKHOOK            *WndClickHook;

static unsigned         wndDClick_ms = 300 /* ms */;
static int              wndProcNesting = 0;
static gui_colour_set   Colours = { GUI_BLACK, GUI_BLACK };
static gui_mcursor_type wndCursorType = GUI_ARROW_CURSOR;
static char             *BusyString = NULL;

unsigned WndGetDClick( void )
{
    return( wndDClick_ms );
}

void WndSetDClick( unsigned dclick_ms )
{
    wndDClick_ms = dclick_ms;
    GUISetDClickRate( dclick_ms );
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

static bool WndInitBody( char *title, res_name_or_id resource_menu )
{
    wnd_create_struct   info;

    if( !GUIWndInit( wndDClick_ms, WndStyle ) )
        return( false );
    GUIMDIInitMenuOnly();
    GUI3DDialogInit();
    GUISetBackgroundColour( &Colours );
    GUISetBetweenTitles( 2 );
    Rescale();
    WndSysInit();
    WndInitWndMain( &info );
    info.title = title;
    info.info = &NoInfo;
    info.wndclass = WND_NO_CLASS;
    info.extra = NULL;
    WndSetWndMainSize( &info );
    WndMain = WndCreateWithStructAndMenuRes( &info, resource_menu );
    Rescale();
    if( WndBackgroundChar == 0 ) {
        WndBackgroundChar = GUIGetCharacter( GUI_SCROLL_SLIDER );
    }
    GUISetBackgroundChar( WndMain->gui, WndBackgroundChar );
    WndSetWndMax();
    return( true );
}

bool WndInit( char *title )
{
    return( WndInitBody( title, NULL ) );
}

bool WndInitWithMenuRes( char *title, res_name_or_id resource_menu )
{
    return( WndInitBody( title, resource_menu ) );
}

void WndShowWndMain( void )
{
    WndShowWindow( WndMain );
}

bool WndFini( void )
{
    if( !WndShutDownHook() )
        return( false );
    GUIDestroyWnd( NULL );
    if( BusyString != NULL ) {
        WndFree( BusyString );
        BusyString = NULL;
    }
    return( true );
}

void WndInitNumRows( a_window wnd )
{
    wnd->rows = (wnd_row)GUIGetNumRows( wnd->gui );
}

static void WndMoveResize( a_window wnd )
{
    gui_text_metrics    text;
    gui_rect            rect;
    wnd_coord           save_curr;

    if( WndMain != NULL && WndIsMinimized( WndMain ) )
        return;
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
            WndVScroll( wnd, wnd->current.row - wnd->rows + 1 );
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

static void WndKeyEnter( a_window wnd )
{
    if( WndHasCurrent( wnd ) ) {
        if( !WndPieceIsTab( wnd, wnd->current.row, wnd->current.piece ) &&
            WndSwitchOn( wnd, WSW_ONLY_MODIFY_TABSTOP ) ) {
            return;
        }
        WndModify( wnd, wnd->current.row, wnd->current.piece );
    } else {
        if( WndSwitchOn( wnd, WSW_ONLY_MODIFY_TABSTOP ) ) {
            return;
        }
        WndModify( wnd, WND_NO_ROW, WND_NO_PIECE );
    }
}

void WndInstallClickHook( WNDCLICKHOOK *rtn )
{
    WndClickHook = rtn;
}

static void DoMainGUIEventProc( void *_spawnp )
{
    a_window            wnd;
    bool                ret;
    gui_key             key;
    gui_keystate        state;
    gui_ctl_id          id;
    int                 vscroll;
    gui_mcursor_handle  old_cursor;

    gui_window          *gui = ((spawn_parms *)_spawnp)->gui;
    gui_event           gui_ev = ((spawn_parms *)_spawnp)->gui_ev;
    void                *parm = ((spawn_parms *)_spawnp)->parm;

    wnd = GUIGetExtra( gui );
    ((spawn_parms *)_spawnp)->ret = false;
    if( wnd == NULL )
        return;
    if( WndIgnoreAllEvents ) {
        if( gui_ev == GUI_PAINT ) {
            WndSetRepaint( wnd );
        }
        if( gui_ev == GUI_MOUSEMOVE ) {
            GUISetMouseCursor( GUI_HOURGLASS_CURSOR );
        }
        return;
    }
    if( gui_ev == GUI_MOUSEMOVE ) {
        GUISetMouseCursor( wndCursorType );
        if( WndIgnoreMouseMove( wnd ) ) {
            return;
        }
    }
    if( !WndDoingRefresh && wndProcNesting == 1 )
        WndDoInput();

    WndChooseEvent( wnd, gui_ev, parm );
    WndSelectEvent( wnd, gui_ev, parm );
    ret = true;
    switch( gui_ev ) {
    case GUI_STATUS_CLEARED:
        return;
    case GUI_ACTIVATEAPP:
        return;
    case GUI_INITMENUPOPUP:
        GUI_GETID( parm, id );
        WndSetPopup( id );
        break;
    case GUI_INIT_WINDOW:
        wnd->gui = gui;
        wnd->popup = wnd->info->popup;
        old_cursor = WndHourGlass( NULL );
        WndMoveResize( wnd );
        ret = WNDEVENT( wnd, gui_ev, parm );
        WndSetThumb( wnd );
        WndHourGlass( old_cursor );
        break;
    case GUI_SCROLL_VERTICAL:
        GUI_GET_SCROLL( parm, vscroll );
        WndVScroll( wnd, vscroll );
        break;
    case GUI_SCROLL_UP:
        WndScrollUp( wnd );
        break;
    case GUI_SCROLL_DOWN:
        WndScrollDown( wnd );
        break;
    case GUI_SCROLL_PAGE_UP:
        if( WNDEVENT( wnd, gui_ev, parm ) )
            break;
        WndPageUp( wnd );
        break;
    case GUI_SCROLL_PAGE_DOWN:
        if( WNDEVENT( wnd, gui_ev, parm ) )
            break;
        WndPageDown( wnd );
        break;
    case GUI_NOW_ACTIVE:
        if( wnd == WndMain ) {
            WndNextNonIconToFront( WndNext( NULL ) );
        } else {
            WndSetSwitches( wnd, WSW_ACTIVE );
            WndAddPopupMenu( wnd );
            WNDEVENT( wnd, gui_ev, parm );
        }
        ret = false;
        break;
    case GUI_NOT_ACTIVE:
        WndClrSwitches( wnd, WSW_ACTIVE );
        if( wnd != WndMain ) {
            WNDEVENT( wnd, gui_ev, parm );
        }
        ret = false;
        break;
    case GUI_MOVE:
        if( WndMain != NULL && WndIsMinimized( WndMain ) )
            return;
        WndResizeHook( wnd );
        break;
    case GUI_FONT_CHANGED :
        if( wnd == WndMain ) {
            GUIResizeStatusWindow( gui, 0, 0 );
        }
        WndFontHook( wnd );
        WndResetScroll( wnd );
        /* fall through */
    case GUI_RESIZE:
        WndClrSwitches( wnd, WSW_ICONIFIED );
        old_cursor = WndHourGlass( NULL );
        WndMoveResize( wnd );
        ret = WNDEVENT( wnd, gui_ev, parm );
        WndSetThumb( wnd );
        WndHourGlass( old_cursor );
        break;
    case GUI_LBUTTONDBLCLK:
        WndClrSwitches( wnd, WSW_CLICKED );
        WndSetSwitches( wnd, WSW_DCLICKED );
        WndLButtonDown( wnd, parm );
        break;
    case GUI_LBUTTONDOWN:
        WndClrSwitches( wnd, WSW_DCLICKED );
        WndSetSwitches( wnd, WSW_CLICKED );
        WndLButtonDown( wnd, parm );
        break;
    case GUI_LBUTTONUP:
        old_cursor = WndHourGlass( NULL );
        if( WndSwitchOn( wnd, WSW_DCLICKED ) ) {
            WndLDblClk( wnd, parm );
        } else if( WndSwitchOn( wnd, WSW_CLICKED ) ) {
            WndLButtonUp( wnd, parm );
        }
        WndClrSwitches( wnd, WSW_CLICKED | WSW_DCLICKED );
        WndHourGlass( old_cursor );
        break;
    case GUI_RBUTTONDOWN:
        WndRButtonDown( wnd, parm );
        break;
    case GUI_RBUTTONUP:
        old_cursor = WndHourGlass( NULL );
        WndRButtonUp( wnd, parm );
        WndHourGlass( old_cursor );
        break;
    case GUI_MOUSEMOVE:
        WndMouseMove( wnd, parm );
        break;
    case GUI_CLICKED:
        GUI_GETID( parm, id );
        if( WndClickHook != NULL && WndClickHook( wnd, id ) )
            break;
        old_cursor = WndHourGlass( NULL );
        WndClick( wnd, id );
        WndHourGlass( old_cursor );
        break;
    case GUI_PAINT:
        if( WndSwitchOff( wnd, WSW_REPAINT ) ) { // going to repaint anyway
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
        ret = false;
        break;
    case GUI_KEYDOWN:
        GUI_GET_KEY_STATE( parm, key, state );
        if( GUI_CTRL_STATE( state ) ) {
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
        if( WndKeyIsPrintChar( key ) && WndSwitchOn( wnd, WSW_CHOOSING ) && WndKeyPiece( wnd ) != WND_NO_PIECE ) {
            if( !WndKeyChoose( wnd, UCHAR_VALUE( key ) ) ) {
                Ring();
            }
            break;
        }
        old_cursor = WndHourGlass( NULL );
        if( !WndProcMacro( wnd, key ) ) {
            switch( key ) {
            case GUI_KEY_HOME:
                if( GUI_SHIFT_STATE( state ) )
                    WndToSelectMode( wnd );
                WndCursorStart( wnd );
                break;
            case GUI_KEY_ESCAPE:
                WndKeyEscape( wnd );
                break;
            case GUI_KEY_END:
                if( GUI_SHIFT_STATE( state ) )
                    WndToSelectMode( wnd );
                WndCursorEnd( wnd );
                break;
            case GUI_KEY_BACKSPACE:
                WndKeyRubOut( wnd );
                break;
            case GUI_KEY_ENTER:
                WndKeyEnter( wnd );
                break;
            case GUI_KEY_SHIFT_TAB:
                WndTabLeft( wnd, true );
                break;
            case GUI_KEY_TAB:
                WndTabRight( wnd, true );
                break;
            case GUI_KEY_LEFT:
                if( GUI_SHIFT_STATE( state ) )
                    WndToSelectMode( wnd );
                WndCursorLeft( wnd );
                break;
            case GUI_KEY_RIGHT:
                if( GUI_SHIFT_STATE( state ) )
                    WndToSelectMode( wnd );
                WndCursorRight( wnd );
                break;
            case GUI_KEY_PAGEDOWN:
                if( WNDEVENT( wnd, GUI_SCROLL_PAGE_DOWN, parm ) )
                    break;
                WndPageDown( wnd );
                break;
            case GUI_KEY_PAGEUP:
                if( WNDEVENT( wnd, GUI_SCROLL_PAGE_UP, parm ) )
                    break;
                WndPageUp( wnd );
                break;
            case GUI_KEY_UP:
                if( WndSwitchOn( wnd, WSW_MAP_CURSOR_TO_SCROLL ) ) {
                    WndScrollUp( wnd );
                } else {
                    if( WndSwitchOn( wnd, WSW_MULTILINE_SELECT ) && GUI_SHIFT_STATE( state ) ) {
                        WndToSelectMode( wnd );
                    }
                    WndCursorUp( wnd );
                }
                break;
            case GUI_KEY_DOWN:
                if( WndSwitchOn( wnd, WSW_MAP_CURSOR_TO_SCROLL ) ) {
                    WndScrollDown( wnd );
                } else {
                    if( WndSwitchOn( wnd, WSW_MULTILINE_SELECT ) && GUI_SHIFT_STATE( state ) ) {
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
                ret = false;
                break;
            }
        }
        if( ret ) {
            GUIFlushKeys();
        }
        WndHourGlass( old_cursor );
        break;
    case GUI_DESTROY:
        WndDestroy( wnd );
        wnd = NULL;
        break;
    case GUI_ICONIFIED:
        WndSetSwitches( wnd, WSW_ICONIFIED );
        if( wnd == WndNext( NULL ) ) {
            WndNextNonIconToFront( WndNext( wnd ) );
        }
        break;
    case GUI_QUERYENDSESSION:
        ret = !WndQueryEndSessionHook();
        break;
    case GUI_ENDSESSION:
        {
            bool    endsession;
            bool    dummy;

            GUI_GET_ENDSESSION( parm, endsession, dummy );
            (void)dummy;    /* reference to unused variable */
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
            ret = true;
        }
        break;
    case GUI_TOOLBAR_FLOATING:
    case GUI_TOOLBAR_FIXED:
    case GUI_TOOLBAR_DESTROYED:
        WndSetToolBar( gui_ev );
        ret = true;
        break;
    case GUI_HSCROLL_NOTIFY:
        WndHScrollNotify( wnd );
        break;
    default:
        ret = false;
        break;
    }
    if( !WndDoingRefresh && wndProcNesting == 1 )
        WndDoInput();
    if( WndValid( wnd ) ) { // could have been freed
        WndEndSelectEvent( wnd );
        WndSayMatchMode( wnd );
    }
    WndFreshAll();
    ((spawn_parms *)_spawnp)->ret = ret;
    return;
}

gui_mcursor_handle WndHourCursor( void )
{
    if( !GUIIsGUI() ) {
        if( BusyString == NULL ) {
            BusyString = WndLoadString( LITERAL_Busy );
        }
        WndInternalStatusText( BusyString );
    }
    wndCursorType = GUI_HOURGLASS_CURSOR;
    return( GUISetMouseCursor( GUI_HOURGLASS_CURSOR ) );
}

gui_mcursor_handle WndArrowCursor( void )
{
    if( !GUIIsGUI() ) {
        WndResetStatusText();
    }
    wndCursorType = GUI_ARROW_CURSOR;
    return( GUISetMouseCursor( GUI_ARROW_CURSOR ) );
}

static gui_mcursor_handle WndSetCursor( gui_mcursor_handle to )
{
    if( !GUIIsGUI() ) {
        WndResetStatusText();
    }
    wndCursorType = GUI_ARROW_CURSOR;
    GUIResetMouseCursor( to );
    return( NULL );
}

gui_mcursor_handle WndHourGlass( gui_mcursor_handle to )
{
    if( to != NULL ) {
        return( WndSetCursor( to ) );
    } else {
        return( WndHourCursor() );
    }
}

bool WndMainGUIEventProc( gui_window *gui, gui_event gui_ev, void *parm )
{
    spawn_parms         spawnp;

    spawnp.gui = gui;
    spawnp.gui_ev = gui_ev;
    spawnp.parm = parm;
    wndProcNesting++;
    SpawnP( DoMainGUIEventProc, &spawnp );
    wndProcNesting--;
    return( spawnp.ret );
}
