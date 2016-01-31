/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "winaux.h"
#include "font.h"
#include "wstatus.h"
#include "win.h"
#include "winifini.h"


extern HWND hColorbar, hFontbar, hSSbar;

/*
 * DefaultWindows: figure out some reasonable locations for some of
 * the common windows and store it in their window structures.
 * world: the area we have to work with
 * workspace: what we leave for the edit windows
 */

void DefaultWindows( RECT *world, RECT *workspace )
{
    RECT        *r, *last, tmp;
    window      *w;
    int         border;
    int         screeny;
    int         diff;
#ifdef __NT__
    int         statusHeight = GetStatusHeight();
#endif

    border = GetSystemMetrics( SM_CYBORDER );
    screeny = GetSystemMetrics( SM_CYSCREEN );

    if( EditFlags.StatusInfo ) {
        /* first we do the status bar */
        w = &StatusBar;
        r = &w->def_area;
        *r = *world;

        /*
         * the lovely '4' here comes from the face that we have a border of
         * two pixels and two pixels for drawing the box and I didn't feel
         * like adding yet another define.
         * This whole function reeks a little anyway :)
         */
#ifdef __NT__
        if( statusHeight != 0 ) {
            r->top = r->bottom - statusHeight;
        } else {
#endif
            r->top = r->bottom - (FontHeight( WIN_TEXT_FONT( w ) ) + 2 * border + 7);
#ifdef __NT__
        }
#endif
        last = r;
    } else {
        tmp = *world;
        tmp.top = tmp.bottom;
        last = &tmp;
    }

    /* next the message bar */
    w = &MessageBar;
    r = &w->def_area;
    // let these windows share a common border, except when Win32 common controls
    // are used
    *r = *last;
    r->bottom = last->top;
#ifdef __NT__
    if( statusHeight == 0 ) {
#endif
        r->bottom += border;
#ifdef __NT__
    }
#endif
    r->top = r->bottom - FontHeight( WIN_TEXT_FONT( w ) ) - 4 * border;
    last = r;

    /* the command window */
    #define BORDER  25
    w = &CommandWindow;
    r = &w->def_area;
    // put it right over top of the message bar
    *r = *world;
    r->bottom -= BORDER;
    r->left += BORDER;
    r->right -= BORDER;
    r->top = r->bottom - FontHeight( WIN_TEXT_FONT( w ) ) - 4 * border;

    /* the repeat count window */
    #undef BORDER
    #define BORDER  20
    w = &RepeatCountWindow;
    r = &w->def_area;
    // put it right over top of the message bar
    *r = *world;
    r->bottom -= BORDER;
    r->left += BORDER;
    r->right -= BORDER;
    r->top = r->bottom - FontHeight( WIN_TEXT_FONT( w ) ) - 4 * border;

    /* the file completion window */
    w = &FileCompleteWindow;
    r = &w->def_area;
    #undef BORDER
    #define BORDER  50
    *r = *world;
    r->bottom -= BORDER;
    r->left += BORDER;
    r->right -= BORDER;
    r->top += BORDER;
    if( (r->bottom - r->top) > screeny / 3 ) {
        diff = r->bottom - r->top - screeny / 3;
        r->top += diff;
    }

    *workspace = *world;
    workspace->bottom = last->top;
    NewToolBar( workspace );
}

void InitWindows( void )
{
    StatusBarInit();
    EditWindowInit();
    CommandWindowInit();
    MessageBarInit();
    FileCompleteWindowInit();
    RepeatCountWindowInit();

    EditFlags.WindowsStarted = true;
}

void FiniWindows( void )
{
    StatusBarFini();
    EditWindowFini();
    CommandWindowFini();
    MessageBarFini();
    FileCompleteWindowFini();
    RepeatCountWindowFini();

    EditFlags.WindowsStarted = false;
}

int WindowAuxInfo( window_id wid, int type )
{
    window      *win;
    int         value, height;
    RECT        area;

    if( BAD_ID( wid ) || !IsWindow( wid ) ) {
        return( 0 );
    }
    win = WINDOW_FROM_ID( wid );
    GetClientRect( wid, &area );
    switch( type ) {
    case WIND_INFO_X1:
        value = area.left;
        break;
    case WIND_INFO_Y1:
        value = area.top;
        break;
    case WIND_INFO_X2:
        value = area.right;
        break;
    case WIND_INFO_Y2:
        value = area.bottom;
        break;
    case WIND_INFO_TEXT_LINES:
        height = FontHeight( WIN_TEXT_FONT( win ) );
        // the 4/5 is a rather arbitrary constant chosen so that we don't show
        // less than 20% of a line
        // value = area.bottom - area.top + (height / 5);
        value = area.bottom - area.top; // + height - 1;
        value /= height;
        break;
    case WIND_INFO_TEXT_COLS:
        value = area.right - area.left;
        value /= FontAverageWidth( WIN_TEXT_FONT( win ) );
        break;
    case WIND_INFO_HEIGHT:
        value = area.bottom - area.top;
        break;
    case WIND_INFO_WIDTH:
        value = area.right - area.left;
        break;
    case WIND_INFO_TEXT_COLOR:
        value = WIN_TEXT_COLOR( win );
        break;
    case WIND_INFO_BACKGROUND_COLOR:
        value = WIN_TEXT_BACKCOLOR( win );
        break;
    case WIND_INFO_HAS_SCROLL_GADGETS:
    case WIND_INFO_HAS_BORDER:
        value = false;
        break;
    case WIND_INFO_TEXT_FONT:
        value = WIN_TEXT_FONT( win );
        break;
    case WIND_INFO_BORDER_COLOR1:
    case WIND_INFO_BORDER_COLOR2:
    default:
        value = -1;
    }
    return( value );
}

vi_rc NewWindow2( window_id *wid, window_info *info )
{
    if( info == &editw_info ) {
        *wid = NewEditWindow();
    } else if( info == &cmdlinew_info ) {
        *wid = NewCommandWindow();
    } else if( info == &statusw_info ) {
        *wid = NewStatWindow();
    } else if( info == &messagew_info ) {
        *wid = NewMsgWindow();
    } else if( info == &filecw_info ) {
        *wid = NewFileCompleteWindow();
    } else if( info == &repcntw_info ) {
        *wid = NewRepeatCountWindow();
    } else {
        *wid = NO_WINDOW;
        return( ERR_WIND_INVALID );
    }
    return( ERR_NO_ERR );
}

void CloseAWindow( window_id wid )
{
    if( !BAD_ID( wid ) ) {
        DestroyWindow( wid );
    }
}

void CloseAChildWindow( window_id wid )
{
    if( !BAD_ID( wid ) ) {
        SendMessage( edit_container_id, WM_MDIDESTROY, (UINT)wid, 0L );
    }
}

/*
 * Note that for Windows we want the (x,y) in PIXEL COORDS, not
 * row/col coords. Since this function is only used from the
 * selection stuff this is not a problem.
 */

bool InsideWindow( window_id wid, int x, int y )
{
    POINT       pt;
    RECT        rect;

    pt.x = x;
    pt.y = y;
    GetClientRect( wid, &rect );
    return( PtInRect( &rect, pt ) );
}

void InactiveWindow( window_id wid )
{
    // not needed under real MDI
    wid = wid;
    return;
}

void ActiveWindow( window_id wid )
{
    if( !BAD_ID( wid ) ) {
        SetActiveWindow( wid );
        SetWindowCursor();
        SetWindowCursorForReal();
    }
}

void MoveWindowToFront( window_id wid )
{
    if( !BAD_ID( wid ) ) {
        BringWindowToTop( wid );
        ActiveWindow( wid );
    }
}

void MoveWindowToFrontDammit( window_id wid, bool scrflag )
{
    scrflag = scrflag;
    if( BAD_ID( wid ) ) {
        return;
    }
    MoveWindowToFront( wid );
}

vi_rc MaximizeCurrentWindow( void )
{
    if( !BAD_ID( current_window_id ) ) {
        SendMessage( edit_container_id, WM_MDIMAXIMIZE, (UINT)current_window_id, 0L );
    }
    return( ERR_NO_ERR );
}

vi_rc MinimizeCurrentWindow( void )
{
    if( !BAD_ID( current_window_id ) ) {
        SendMessage( current_window_id, WM_SYSCOMMAND, SC_MINIMIZE, 0L );
    }
    return( ERR_NO_ERR );
}

void FinishWindows( void )
{
    if( IsWindow( hColorbar ) ) {
        SendMessage( hColorbar, WM_CLOSE, 0, 0L );
    }
    if( IsWindow( hFontbar ) ) {
        SendMessage( hFontbar, WM_CLOSE, 0, 0L );
    }
    if( IsWindow( hSSbar ) ) {
        SendMessage( hSSbar, WM_CLOSE, 0, 0L );
    }
    if( IsWindow( root_window_id ) ) {
        DestroyWindow( root_window_id );
    }
    FiniWindows();
    FiniInstance();
}
