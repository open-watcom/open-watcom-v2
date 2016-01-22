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
* Description:  Edit window implementation.
*
****************************************************************************/


#include "vi.h"
#include <limits.h>
#include "winaux.h"
#include "win.h"
#include "font.h"
#include "color.h"
#include "utils.h"
#include "wprocmap.h"
#include "winifini.h"
// #include "mdisim.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT LRESULT CALLBACK EditWindowProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT BOOL CALLBACK ResizeExtra( HWND hwnd, LPARAM l );

extern HWND hColorbar, hFontbar, hSSbar;

char *EditWindowClassName = "Buffer Window";

window EditWindow = {
    &editw_info,
    { 0, 0, 0, 0 }
};

long            VScrollBarScale = 1;
int             HScrollBarScale = MAX_INPUT_LINE;

/*
 * init - initialization routine for edit windows
 */
bool EditWindowInit( void )
{
    WNDCLASS    wc;

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = GetWndProc( EditWindowProc );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = EXTRA_WIN_DATA;
    wc.hInstance = InstanceHandle;
    wc.hIcon = LoadIcon( InstanceHandle, "WATCOMICON" );
    wc.hCursor = LoadCursor( (HINSTANCE)NULLHANDLE, IDC_IBEAM );
    wc.hbrBackground = (HBRUSH)NULLHANDLE;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = EditWindowClassName;
    return( RegisterClass( &wc ) != 0 );

} /* init */

bool EditWindowFini( void )
{
    return( true );

} /* fini */

/*
 * GetEditStyle - get the edit window style
 */
DWORD GetEditStyle( bool is_max )
{
    DWORD style = 0;
    is_max = is_max;

    style |= WS_HSCROLL;
    style |= WS_VSCROLL;
    return( style );

} /* GetEditStyle */

/*
 * SetWindowTitle - set the current window title
 */
void SetWindowTitle( HWND hwnd )
{
    info        *cinfo;
    char        buff[MAX_STR];
    window_data *wd;

    wd = DATA_FROM_ID( hwnd );
    cinfo = wd->info;
    if( cinfo != NULL ) {
        if( cinfo->CurrentFile != NULL ) {
            if( cinfo->CurrentFile->dup_count > 0 ) {
                MySprintf( buff, "%s [%d]", cinfo->CurrentFile->name,
                           cinfo->DuplicateID );
                 SetWindowText( cinfo->current_window_id, buff );
            } else {
                SetWindowText( cinfo->current_window_id, cinfo->CurrentFile->name );
            }
        }
    }

} /* SetWindowTitle */

/*
 * NewEditWindow - create a new window for an edit buffer
 */
window_id NewEditWindow( void )
{
    HWND            edit;
    RECT            rect;
    DWORD           style;
    window_data     *wd;
    MDICREATESTRUCT mdinew;

    if( BAD_ID( EditContainer ) ) {
        return( NO_WINDOW );
    }
    style = GetEditStyle( false );
    ResizeRoot();
    GetClientRect( EditContainer, &rect );

    mdinew.szClass = EditWindowClassName;
    mdinew.szTitle = "Edit Buffer";
    mdinew.hOwner = InstanceHandle;
    mdinew.x = rect.top;
    mdinew.y = rect.left;
    mdinew.cx = rect.right - rect.left;
    mdinew.cy = rect.bottom - rect.top;
    mdinew.style = style;
    mdinew.lParam = 0;

    edit =(HWND)SendMessage( EditContainer, WM_MDICREATE, 0, (LPARAM)&mdinew );

    wd = DATA_FROM_ID( edit );

    ShowWindow( edit, SW_NORMAL );
    UpdateWindow( edit );
    SetFocus( Root );

    return( edit );

} /* NewEditWindow */

/*
 * doPaint - handle painting a specific edit window
 */
static void doPaint( window *w, RECT *r, window_data *wd )
{
    info        *old_info, *new_info;
    int         start, stop;
    int         height;
    int         max_lines;

    new_info = wd->info;
    old_info = CurrentInfo;
    if( new_info != old_info ) {
        SaveCurrentInfo();
        RestoreInfo( new_info );
    }
    height = FontHeight( WIN_TEXT_FONT( w ) );
    start = r->top / height;
    stop = (r->bottom + height - 1) / height;
    max_lines = WindowAuxInfo( current_window_id, WIND_INFO_TEXT_LINES );
    if( stop + 1 > max_lines ) {
        stop = max_lines - 1;
    }
    DCInvalidateSomeLines( start, stop );
    DCDisplaySomeLines( start, stop );
    DCUpdate();
    if( new_info != old_info ) {
        RestoreInfo( old_info );
    }

} /* doPaint */

static void cancelDrag( void );

/*
 * activateWindow - make a particular edit window active
 */
static void activateWindow( HWND hwnd )
{
    info        *cinfo;
    window_data *wd;

    cancelDrag();
    SaveCurrentInfo();
    wd = DATA_FROM_ID( hwnd );
    cinfo = wd->info;
    if( cinfo != NULL ) {
        BringUpFile( cinfo, true );
    }

} /* activateWindow */

/*
 * Mouse code:
 */
static int      startDragRow, startDragCol;
static bool     buttonDown;
static bool     hasCapture;
static bool     doubleClickPending;
static bool     dragPending;
static UINT_PTR timerID;
static HWND     timerHwnd;
int             MouseX, MouseY;

#define TIMER_ID        0x01

/*
 * startDragTimer - activate a timer that will tell us when to
 *                  update dragging when the mouse leaves the window
 */
static void startDragTimer( HWND hwnd )
{
    timerID = SetTimer( hwnd, TIMER_ID, 10, NULL );
    if( timerID != 0 ) {
        timerHwnd = hwnd;
    }

} /* startDragTimer */

/*
 * stopDragTimer - deactivate the drag timer
 */
static void stopDragTimer( void )
{
    if( timerID ) {
        KillTimer( timerHwnd, TIMER_ID );
        timerID = 0;
    }

} /* stopDragTimer */

/*
 * cancelDrag - cancel any dragging or pending dragging
 */
static void cancelDrag( void )
{
    stopDragTimer();
    EditFlags.Dragging = false;
    buttonDown = false;
    dragPending = false;
    if( hasCapture ) {
        hasCapture = false;
        ReleaseCapture();
    }

} /* cancelDrag */

/*
 * isMouseButtonDown - check if a mouse button is down
 */
static bool isMouseButtonDown( void )
{
    if( (GetKeyState( VK_LBUTTON ) & ~0x01) != 0 ) {
        return( true );
    }
    if( (GetKeyState( VK_RBUTTON ) & ~0x01) != 0 ) {
        return( true );
    }
    return( false );

} /* isMouseButtonDown */

/*
 * jumpToCoord - move to a specific coordinate in an edit window
 */
static bool jumpToCoord( int row, int col )
{
    GoToLineRelCurs( LeftTopPos.line + row - 1 );
    col = RealColumnOnCurrentLine( col + LeftTopPos.column );
    GoToColumnOnCurrentLine( col );
    return( true );

} /* jumpToCoord */

/*
 * regionSelected - handle selection with a mouse
 */
static void regionSelected( HWND id, int x, int y, bool dclick, bool popMenu )
{
    int         row, col;
    vi_key      save;

    MyKillCaret( id );
    ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
    jumpToCoord( row, col );
    MouseX = x;
    MouseY = y;
    if( dclick ) {
        InitWordSearch( EditVars.WordAltDefn );
    }
    /*
     * The hook stuff needs to know if it was invoked by a mouse press
     * or an '_' thingy.
     */
    if( popMenu ) {
        save = LastEvent;
        LastEvent = VI_KEY( FAKEMOUSE );
        DoSelectSelection( popMenu );
        LastEvent = save;
    } else {
        DoSelectSelection( popMenu );
    }
    if( dclick ) {
        InitWordSearch( EditVars.WordDefn );
    }
    MouseY = 0;
    MouseX = 0;
    MyRaiseCaret( id );
    // UnselectRegion();

} /* regionSelected */

/*
 * mouseButtonDown - handle a mouse down event in an edit window
 */
static void mouseButtonDown( HWND id, int x, int y, bool shift )
{
    int         row, col;

    if (!EditFlags.WasOverstrike ) {
        ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
    } else {
        ClientToRowCol( id, x, y, &row, &col, DIVIDE_BETWEEN );
    }

    if( current_window_id != id ) {
        UnselectRegion();
        activateWindow( id );
        jumpToCoord( row, col );
        return;
    }

    MouseX = x;
    MouseY = y;
    buttonDown = true;
    if( SelRgn.selected && shift ) {
        if( EditFlags.WasOverstrike ) {
            /*  if already dragging, always divide in middle of chars
            */
            ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
        }
        EditFlags.Dragging = true;
        UpdateDrag( id, col, row );
    } else {
        jumpToCoord( row, col );
        UnselectRegion();
        dragPending = true;
        if( !hasCapture ) {
            SetCapture( id );
            hasCapture = true;
        }
        if( EditFlags.WasOverstrike ) {
            /*  dragging always based on middle of chars
            */
            ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
        }
        startDragRow = row;
        startDragCol = col;
    }

} /* mouseButtonDown */

/*
 * rightButtonDown - handle the right mouse button being pressed
 */
static void rightButtonDown( HWND id, int x, int y, bool shift )
{
    if( SelRgn.selected ) {
        regionSelected( id, x, y, false, true );
    } else {
        mouseButtonDown( id, x, y, shift );
    }

} /* rightButtonDown */

/*
 * leftButtonDown - handle the right mouse button being pressed
 */
static void leftButtonDown( HWND id, int x, int y, bool shift )
{
    mouseButtonDown( id, x, y, shift );

} /* leftButtonDown */

/*
 * mouseMove - handle a mouse move event in an edit window
 */
static void mouseMove( HWND id, int x, int y, bool not_used )
{
    int     row, col;

    not_used = not_used;

    if( dragPending ) {
        ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
        if( row == startDragRow && col == startDragCol ) {
            return;
        }
        InitSelectedRegion();
        EditFlags.Dragging = true;
        dragPending = false;
        startDragTimer( id );

        MyKillCaret( id );
    }

    if( !EditFlags.InsertModeActive && EditFlags.Dragging && buttonDown ) {
        MouseX = x;
        MouseY = y;
        ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
        UpdateDrag( id, col, row );
        DCUpdate();
        SetWindowCursorForReal();
    }

} /* mouseMove */

/*
 * leftButtonUp - handle a mouse up event in an edit window
 */
static void leftButtonUp( HWND id, int x, int y, bool shift )
{
    id = id;
    x = x;
    y = y;
    shift = shift;

    cancelDrag();
    MouseX = MouseY = 0;

    MyRaiseCaret( id );

} /* leftButtonUp */

/*
 * rightButtonUp - handle right mouse button coming up
 */
static void rightButtonUp( HWND id, int x, int y, bool dclick )
{
    cancelDrag();
    regionSelected( id, x, y, dclick, true );

} /* rightButtonUp */

/*
 * leftButtonDoubleClick - handle double click of left button (word selectn)
 */
static void leftButtonDoubleClick( HWND id, int x, int y, bool dclick )
{
    cancelDrag();
    regionSelected( id, x, y, dclick, false );

} /* leftButtonDoubleClick */

typedef void (*func)( HWND, int, int, bool );

/*
 * mouseEvent - handle all mouse events in an edit window
 */
static void mouseEvent( HWND hwnd, LPARAM l, bool flag, func f )
{
    if( EditFlags.HoldEverything ) {
        return;
    }
    if( EditFlags.InsertModeActive ) {
        PushMode();
        f( hwnd, (short)LOWORD( l ), (short)HIWORD( l ), flag );
        PopMode();
    } else {
        f( hwnd, (short)LOWORD( l ), (short)HIWORD( l ), flag );
    }
    DCUpdate();
    SetWindowCursorForReal();

} /* mouseEvent */

/*
 * sendDragMessage - when we get a timer event, pretend we got a mouse move
 */
static void sendDragMessage( HWND hwnd )
{
    if( timerID ) {
        if( isMouseButtonDown() ) {
            SendMessage( hwnd, WM_MOUSEMOVE, 0, MAKELONG( MouseX, MouseY ) );
        } else {
            /* it's possible that someone ate our WM_BUTTONUP
             * or something stupid (WVIDEO perhaps...) */
            stopDragTimer();
        }
    }

} /* sendDragMessage */

/*
 * PositionVerticalScrollThumb - place the vertical thumb
 */
void PositionVerticalScrollThumb( window_id id, linenum top, linenum last )
{
    /*
     * NOTE: This will not work well yet - linenum's are 32-bit while
     * windows takes a stupid 16-bit int.
     */
    int wlines;
    int min, max, pos;
    int newtop, newlast;

    if( BAD_ID( id ) ) {
        return;
    }

    /* if last > 32000+ and ints are short, then set scaling factor to 2
     *   otherwise scale is 1
     */
    VScrollBarScale = 1 + last / INT_MAX;

    wlines = WindowAuxInfo( current_window_id, WIND_INFO_TEXT_LINES );

    /* Reduce number of redraws by checking current range and position
    */
    GetScrollRange( id, SB_VERT, (LPINT)&min, (LPINT)&max );

    newlast = last / VScrollBarScale;
    newtop = top / VScrollBarScale;

    if( newlast > 1 ) {
        /* have enough lines to set position normally
        */
        if( max != newlast ){
            // always set position with range to avoid screen draws
            SetScrollRange( id, SB_VERT, 1, newlast, FALSE );
            SetScrollPos( id, SB_VERT, newtop, TRUE );
        } else {
            pos = GetScrollPos( id, SB_VERT );
            if( newtop != pos ){
                SetScrollPos( id, SB_VERT, newtop, TRUE );
            }
        }
    } else {
        /* set dummy range & leave at bottom
        */
        SetScrollRange( id, SB_VERT, 1, 2, FALSE );
        SetScrollPos( id, SB_VERT, 2, TRUE );
    }

} /* PositionVerticalScrollThumb */


/*
 * PositionHorizontalScrollThumb - place the horizontal thumb
 */
void PositionHorizontalScrollThumb( window_id id, int left )
{
    if( BAD_ID( id ) ) {
        return;
    }

    // must reset range every time (size changes -> mdisim trashes it)
    SetScrollRange( id, SB_HORZ, 1, HScrollBarScale, FALSE );
    SetScrollPos( id, SB_HORZ, left, TRUE );

} /* PositionHorizontalScrollThumb */

/*
 * EditDrawScrollBars
 */
void EditDrawScrollBars( HWND hwnd )
{
    window_data *wd;

    wd = DATA_FROM_ID( hwnd );
    if( wd->info != NULL ) {
        PositionVerticalScrollThumb( hwnd, wd->info->LeftTopPos.line,
                                     wd->info->CurrentFile->fcbs.tail->end_line );
        PositionHorizontalScrollThumb( hwnd, wd->info->LeftTopPos.column );
    }

} /* EditDrawScrollBars */

/*
 * doVScroll - handle various scroll events (vertical)
 */
static void doVScroll( HWND hwnd, WPARAM wparam, LPARAM lparam )
{
    window_data *wd;
    int         scrollAmount;
    linenum     newTopOfPage, oldTopOfPage;
    int         text_lines;
    int         diff;

#ifdef __NT__
    lparam = lparam;
#endif
    wd = DATA_FROM_ID( hwnd );

    oldTopOfPage = LeftTopPos.line;
    EditFlags.ScrollCommand = true;
    switch( GET_WM_VSCROLL_CODE( wparam, lparam ) ) {
    case SB_LINEUP:
        newTopOfPage = LeftTopPos.line - 1;
        if( EditFlags.JumpyScroll ) {
            newTopOfPage = LeftTopPos.line - SCROLL_VLINE;
        }
        MoveScreenML( newTopOfPage );
        break;
    case SB_LINEDOWN:
        newTopOfPage = LeftTopPos.line + 1;
        if( EditFlags.JumpyScroll ) {
            newTopOfPage = LeftTopPos.line + SCROLL_VLINE;
        }
        scrollAmount = newTopOfPage - LeftTopPos.line;
        MoveScreenML( newTopOfPage );
        break;
    case SB_PAGEUP:
        MoveScreenUpPageML();
        break;
    case SB_PAGEDOWN:
        MoveScreenDownPageML();
        break;
    case SB_THUMBTRACK:
        MoveScreenML( GET_WM_VSCROLL_POS( wparam, lparam ) * VScrollBarScale );
        break;
    }
    EditFlags.ScrollCommand = false;

    text_lines = WindowAuxInfo( current_window_id, WIND_INFO_TEXT_LINES );
    diff = LeftTopPos.line - oldTopOfPage;
    if( diff != 0 ){
        if( abs( diff ) > text_lines / 2 ) {
            //  faster to redraw whole screen
            DCInvalidateAllLines();
            GoToLineNoRelCurs( LeftTopPos.line );
            DCDisplayAllLines();
            DCUpdate();
        } else {
            //  faster to shift screen up and redraw rest
            // but its difficult to figure out how!
            DCInvalidateAllLines();
            GoToLineNoRelCurs( LeftTopPos.line );
            DCDisplayAllLines();
            DCUpdate();
        }

        SetWindowCursor();
        SetWindowCursorForReal();
    }

} /* doVScroll */

/*
 * doHScroll - handle various scroll events (horizontal)
 */
static void doHScroll( HWND hwnd, WPARAM wparam, LPARAM lparam )
{
    int newLeftColumn;

#ifdef __NT__
    lparam = lparam;
#endif
    EditFlags.ScrollCommand = true;
    switch( GET_WM_HSCROLL_CODE( wparam, lparam ) ) {
    case SB_LINEUP:
        newLeftColumn = LeftTopPos.column - 1;
        if( EditFlags.JumpyScroll ) {
            newLeftColumn = LeftTopPos.column - SCROLL_HLINE;
        }
        MoveScreenLeftRightML( newLeftColumn );
        break;
    case SB_LINEDOWN:
        newLeftColumn = LeftTopPos.column + 1;
        if( EditFlags.JumpyScroll ) {
            newLeftColumn = LeftTopPos.column + SCROLL_HLINE;
        }
        MoveScreenLeftRightML( newLeftColumn );
        break;
    case SB_PAGEUP:
        MoveScreenLeftPageML();
        break;
    case SB_PAGEDOWN:
        MoveScreenRightPageML();
        break;
    case SB_THUMBTRACK:
        MoveScreenLeftRightML( GET_WM_HSCROLL_POS( wparam, lparam ) - 1 );
        break;
    }
    EditFlags.ScrollCommand = false;

    DCInvalidateAllLines();
    DCDisplayAllLines();
    DCUpdate();
    SetWindowCursor();
    SetWindowCursorForReal();

    PositionHorizontalScrollThumb( hwnd, LeftTopPos.column );

} /* doHScroll */

/*
 * EditWindowProc - window procedure for all edit windows
 */
WINEXPORT LRESULT CALLBACK EditWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    window      *w;
    PAINTSTRUCT ps;
    HDC         hdc;
    RECT        rect;
    window_data *wd;
    HWND        win;
    HWND        tbwin;
    bool        killsel;
    info        *cinfo;
    info        *sinfo;

    w = WINDOW_FROM_ID( hwnd );
    switch( msg ) {
    case WM_CREATE:
        wd = MemAlloc( sizeof( window_data ) );
        WINDOW_TO_ID( hwnd, (LONG_PTR)&EditWindow );
        DATA_TO_ID( hwnd, (LONG_PTR)wd );
        return( 0 );
    case WM_PAINT:
        if( GetUpdateRect( hwnd, &rect, FALSE ) ) {
            wd = DATA_FROM_ID( hwnd );
            hdc = BeginPaint( hwnd, &ps );
            if( wd->info != NULL ) {
                doPaint( w, &rect, wd );
            } else {
                BlankRectIndirect( hwnd, SEType[SE_WHITESPACE].background, &rect );
            }
            EndPaint( hwnd, &ps );
            if( IntersectRect( &rect, &rect, &wd->extra ) ) {
                BlankRectIndirect( hwnd, SEType[SE_WHITESPACE].background, &rect );
            }
        }
        return( 0 );
    case WM_MOUSEACTIVATE:
        if( hwnd != current_window_id ) {
            UnselectRegion();
            activateWindow( hwnd );
        }
        return( MA_ACTIVATE );

    case WM_MDIACTIVATE:
        if( !wparam ) {
            // losing focus
            cancelDrag();
            killsel = true;
            win = (HWND)wparam;
            if( win != NULL ) {
                tbwin = GetToolbarWindow();
                if( win == Root || win == tbwin || win == CommandId ||
                    (!BAD_ID( hColorbar ) && IsChild( hColorbar, win )) ||
                    (!BAD_ID( hFontbar ) && IsChild( hFontbar, win )) ||
                    (!BAD_ID( hSSbar ) && IsChild( hSSbar, win )) ) {
                    killsel = false;
                }
            }
            if( killsel ) {
                UnselectRegion();
            }
            GoodbyeCursor( hwnd );
        } else {
            // Gaining focus
            MyShowCaret( hwnd );
            ResetEditWindowCursor( hwnd );
            if( hwnd != current_window_id ) {
                activateWindow( hwnd );
                MyShowCaret( hwnd );
                ResetEditWindowCursor( hwnd );
                SetWindowCursor();
                SetWindowCursorForReal();
            }
        }
        return( 0 );
    case WM_LBUTTONDBLCLK:
        doubleClickPending = true;
        return( 0 );
    case WM_RBUTTONUP:
        mouseEvent( hwnd, lparam, false, rightButtonUp );
        return( 0 );
    case WM_RBUTTONDOWN:
        mouseEvent( hwnd, lparam, (wparam & MK_SHIFT) != 0, rightButtonDown );
        return( 0 );
    case WM_LBUTTONDOWN:
        mouseEvent( hwnd, lparam, (wparam & MK_SHIFT) != 0, leftButtonDown );
        return( 0 );
    case WM_LBUTTONUP:
        if( doubleClickPending ) {
            mouseEvent( hwnd, lparam, (wparam & MK_SHIFT) != 0, leftButtonUp );
            mouseEvent( hwnd, lparam, true, leftButtonDoubleClick );
            doubleClickPending = false;
        } else {
            mouseEvent( hwnd, lparam, (wparam & MK_SHIFT) != 0, leftButtonUp );
        }
        return( 0 );
    case WM_MOUSEMOVE:
        mouseMove( hwnd, (int)(short)LOWORD( lparam ), (int)(short)HIWORD( lparam ), false );
        return( 0 );
    case WM_ERASEBKGND:
        return( TRUE );
    case WM_SYSCHAR:
    case WM_SYSKEYUP:
        return( SendMessage( Root, msg, wparam, lparam ) );
    case WM_SYSKEYDOWN:
        if( WindowsKeyPush( wparam, HIWORD( lparam ) ) ) {
            return( 0 );
        }
        return( SendMessage( Root, msg, wparam, lparam ) );
    case WM_KEYDOWN:
        if( WindowsKeyPush( wparam, HIWORD( lparam ) ) ) {
            return( 0 );
        }
        break;
    case WM_VSCROLL:
        doVScroll( hwnd, wparam, lparam );
        return( 0 );
    case WM_HSCROLL:
        doHScroll( hwnd, wparam, lparam );
        return( 0 );
    case WM_CLOSE:
        if( !EditFlags.HoldEverything ) {
            PushMode();
            wd = DATA_FROM_ID( hwnd );
            SendMessage( EditContainer, WM_MDIRESTORE, (UINT)hwnd, 0L );
            BringUpFile( wd->info, true );
            if( NextFile() > ERR_NO_ERR ) {
                FileExitOptionSaveChanges( CurrentFile );
            }
            PopMode();
        }
        return( 0 );
    case WM_TIMER:
        sendDragMessage( hwnd );
        return( 0 );
    case WM_DESTROY:
        wd = DATA_FROM_ID( hwnd );
        MemFree( wd );
        return( 0 );
    case WM_KILLFOCUS:
        DoneCurrentInsert( true );
        break;
    case WM_SIZE:
        wd = DATA_FROM_ID( hwnd );
        DCResize( wd->info );
        if( wparam == SIZE_MINIMIZED ) {
            sinfo = CurrentInfo;
            cinfo = CurrentInfo->next;
            for( ;; ) {
                if( cinfo == NULL ) {
                    cinfo = InfoHead;
                }
                if( cinfo == sinfo ) {
                    SetFocus( Root );
                    return( 0 );
                }
                if( IsIconic( cinfo->current_window_id ) ) {
                    cinfo = cinfo->next;
                } else {
                    SaveInfo( sinfo );
                    BringUpFile( cinfo, false );
                    return( 0 );
                }
            }
        }
        /* either way we remember to reset extra */
        GetClientRect( hwnd, &wd->extra );
        wd->extra.top = WindowAuxInfo( hwnd, WIND_INFO_TEXT_LINES ) *
                                         FontHeight( WIN_TEXT_FONT( &EditWindow ) );
        break;
    default:
        break;
    }
    return( DefMDIChildProc( hwnd, msg, wparam, lparam ) );

} /* EditWindowProc */

/*
 * ResizeExtra - reset the left over rectange for an edit window
 */
WINEXPORT BOOL CALLBACK ResizeExtra( HWND hwnd, LPARAM l )
{
    window_data         *wd;
    char                class[MAX_STR];

    l = l;
    class[0] = 0;
    GetClassName( hwnd, class, sizeof( class ) );
    class[sizeof( class ) - 1] = 0;
    if( stricmp( EditWindowClassName, class ) ) {
        return( TRUE );
    }

    wd = DATA_FROM_ID( hwnd );
    GetClientRect( hwnd, &wd->extra );
    wd->extra.top = WindowAuxInfo( hwnd, WIND_INFO_TEXT_LINES ) *
                                     FontHeight( WIN_TEXT_FONT( &EditWindow ) );

    return( TRUE );

} /* ResizeExtra */

/*
 * ResetExtraRects - reset the leftover rectangle at the bottom of all
 *                   edit windows
 */
void ResetExtraRects( void )
{
    FARPROC     proc;

    if( !BAD_ID( EditContainer ) ) {
        proc = MakeWndEnumProcInstance( ResizeExtra, InstanceHandle );
        EnumChildWindows( EditContainer, (WNDENUMPROC)proc, 0L );
        FreeProcInstance( proc );
    }

} /* ResetExtraRects */
