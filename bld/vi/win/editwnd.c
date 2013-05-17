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
// #include "mdisim.h"

char *EditWindowClassName = "Buffer Window";

WINEXPORT LRESULT CALLBACK EditWindowProc( HWND, UINT, WPARAM, LPARAM );

extern HWND hColorbar, hFontbar, hSSbar;

static BOOL Init( window *, void * );
static BOOL Fini( window *, void * );

window EditWindow = {
    &editw_info,
    { 0, 0, 0, 0 },
    Init,
    Fini
};

long            VScrollBarScale = 1;
int             HScrollBarScale = MAX_INPUT_LINE;

/*
 * Init - initialization routine for edit windows
 */
static BOOL Init( window *w, void *parm )
{
    WNDCLASS    wc;

    w = w;
    parm = parm;

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)EditWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = EXTRA_WIN_DATA;
    wc.hInstance = InstanceHandle;
    wc.hIcon = LoadIcon( InstanceHandle, "WATCOMICON" );
    wc.hCursor = LoadCursor( (HINSTANCE)NULLHANDLE, IDC_IBEAM );
    wc.hbrBackground = (HBRUSH)NULLHANDLE;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = EditWindowClassName;
    return( RegisterClass( &wc ) );

} /* Init */

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
                 SetWindowText( cinfo->CurrentWindow, buff );
            } else {
                SetWindowText( cinfo->CurrentWindow, cinfo->CurrentFile->name );
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
    style = GetEditStyle( FALSE );
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
    height = FontHeight( WIN_FONT( w ) );
    start = r->top / height;
    stop = (r->bottom + height - 1) / height;
    max_lines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
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
        BringUpFile( cinfo, TRUE );
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
static int      timerID;
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
    EditFlags.Dragging = FALSE;
    buttonDown = FALSE;
    dragPending = FALSE;
    if( hasCapture ) {
        hasCapture = FALSE;
        ReleaseCapture();
    }

} /* cancelDrag */

/*
 * isMouseButtonDown - check if a mouse button is down
 */
static bool isMouseButtonDown( void )
{
    if( (GetKeyState( VK_LBUTTON ) & ~0x01) != 0 ) {
        return( TRUE );
    }
    if( (GetKeyState( VK_RBUTTON ) & ~0x01) != 0 ) {
        return( TRUE );
    }
    return( FALSE );

} /* isMouseButtonDown */

/*
 * jumpToCoord - move to a specific coordinate in an edit window
 */
static bool jumpToCoord( int row, int col )
{
    GoToLineRelCurs( LeftTopPos.line + row - 1 );
    col = RealColumnOnCurrentLine( col + LeftTopPos.column );
    GoToColumnOnCurrentLine( col );
    return( TRUE );

} /* jumpToCoord */

/*
 * regionSelected - handle selection with a mouse
 */
static void regionSelected( HWND id, int x, int y, BOOL dclick, bool popMenu )
{
    int         row, col;
    vi_key      tmp;

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
        tmp = LastEvent;
        LastEvent = VI_KEY( FAKEMOUSE );
        DoSelectSelection( popMenu );
        LastEvent = tmp;
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
static void mouseButtonDown( HWND id, int x, int y, BOOL shift )
{
    int         row, col;

    if (!EditFlags.WasOverstrike ) {
        ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
    } else {
        ClientToRowCol( id, x, y, &row, &col, DIVIDE_BETWEEN );
    }

    if( CurrentWindow != id ) {
        UnselectRegion();
        activateWindow( id );
        jumpToCoord( row, col );
        return;
    }

    MouseX = x;
    MouseY = y;
    buttonDown = TRUE;
    if( SelRgn.selected && shift ) {
        if( EditFlags.WasOverstrike ) {
            /*  if already dragging, always divide in middle of chars
            */
            ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
        }
        EditFlags.Dragging = TRUE;
        UpdateDrag( id, col, row );
    } else {
        jumpToCoord( row, col );
        UnselectRegion();
        dragPending = TRUE;
        if( !hasCapture ) {
            SetCapture( id );
            hasCapture = TRUE;
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
static void rightButtonDown( HWND id, int x, int y, BOOL shift )
{
    if( SelRgn.selected ) {
        regionSelected( id, x, y, FALSE, TRUE );
    } else {
        mouseButtonDown( id, x, y, shift );
    }

} /* rightButtonDown */

/*
 * leftButtonDown - handle the right mouse button being pressed
 */
static void leftButtonDown( HWND id, int x, int y, BOOL shift )
{
    mouseButtonDown( id, x, y, shift );

} /* leftButtonDown */

/*
 * mouseMove - handle a mouse move event in an edit window
 */
static void mouseMove( HWND id, int x, int y, BOOL not_used )
{
    int     row, col;
    not_used = not_used;

    if( dragPending ) {
        ClientToRowCol( id, x, y, &row, &col, DIVIDE_MIDDLE );
        if( row == startDragRow && col == startDragCol ) {
            return;
        }
        InitSelectedRegion();
        EditFlags.Dragging = TRUE;
        dragPending = FALSE;
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
static void leftButtonUp( HWND id, int x, int y, BOOL shift )
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
static void rightButtonUp( HWND id, int x, int y, BOOL dclick )
{
    cancelDrag();
    regionSelected( id, x, y, dclick, TRUE );

} /* rightButtonUp */

/*
 * leftButtonDoubleClick - handle double click of left button (word selectn)
 */
static void leftButtonDoubleClick( HWND id, int x, int y, BOOL dclick )
{
    cancelDrag();
    regionSelected( id, x, y, dclick, FALSE );

} /* leftButtonDoubleClick */

typedef void (*func)( HWND, int, int, BOOL );

/*
 * mouseEvent - handle all mouse events in an edit window
 */
static void mouseEvent( HWND hwnd, LPARAM l, BOOL flag, func f )
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

    wlines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );

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

    lparam = lparam;            // Shut up the compiler for the NT version
    wd = DATA_FROM_ID( hwnd );

    oldTopOfPage = LeftTopPos.line;
    EditFlags.ScrollCommand = TRUE;
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
    EditFlags.ScrollCommand = FALSE;

    text_lines = WindowAuxInfo( CurrentWindow, WIND_INFO_TEXT_LINES );
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

    lparam = lparam;
    EditFlags.ScrollCommand = TRUE;
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
        MoveScreenLeftRightML( GET_WM_HSCROLL_POS( lparam, wparam ) - 1 );
        break;
    }
    EditFlags.ScrollCommand = FALSE;

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
    window_data *data;
    HWND        win;
    HWND        tbwin;
    bool        killsel;
    info        *cinfo;
    info        *sinfo;

    w = WINDOW_FROM_ID( hwnd );
    switch( msg ) {
    case WM_CREATE:
        data = MemAlloc( sizeof( window_data ) );
        WINDOW_TO_ID( hwnd, (LONG_PTR)&EditWindow );
        DATA_TO_ID( hwnd, (LONG_PTR)data );
        return( 0 );
    case WM_PAINT:
        if( GetUpdateRect( hwnd, &rect, FALSE ) ) {
            data = DATA_FROM_ID( hwnd );
            hdc = BeginPaint( hwnd, &ps );
            if( data->info != NULL ) {
                doPaint( w, &rect, data );
            } else {
                BlankRectIndirect( hwnd, SEType[SE_WHITESPACE].background, &rect );
            }
            EndPaint( hwnd, &ps );
            if( IntersectRect( &rect, &rect, &data->extra ) ) {
                BlankRectIndirect( hwnd, SEType[SE_WHITESPACE].background, &rect );
            }
        }
        return( 0 );
    case WM_MOUSEACTIVATE:
        if( hwnd != CurrentWindow ) {
            UnselectRegion();
            activateWindow( hwnd );
        }
        return( MA_ACTIVATE );

    case WM_MDIACTIVATE:
        if( wparam == FALSE ) {
            // losing focus
            cancelDrag();
            killsel = TRUE;
            win = (HWND) wparam;
            if( win != NULL ) {
                tbwin = GetToolbarWindow();
                if( win == Root || win == tbwin || win == CommandId ||
                    (hColorbar && IsChild( hColorbar, win )) ||
                    (hFontbar && IsChild( hFontbar, win )) ||
                    (hSSbar && IsChild( hSSbar, win )) ) {
                    killsel = FALSE;
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
            if( hwnd != CurrentWindow ) {
                activateWindow( hwnd );
                MyShowCaret( hwnd );
                ResetEditWindowCursor( hwnd );
                SetWindowCursor();
                SetWindowCursorForReal();
            }
        }
        return( 0 );
    case WM_LBUTTONDBLCLK:
        doubleClickPending = TRUE;
        return( 0 );
    case WM_RBUTTONUP:
        mouseEvent( hwnd, lparam, FALSE, rightButtonUp );
        return( 0 );
    case WM_RBUTTONDOWN:
        mouseEvent( hwnd, lparam, wparam & MK_SHIFT, rightButtonDown );
        return( 0 );
    case WM_LBUTTONDOWN:
        mouseEvent( hwnd, lparam, wparam & MK_SHIFT, leftButtonDown );
        return( 0 );
    case WM_LBUTTONUP:
        if( doubleClickPending ) {
            mouseEvent( hwnd, lparam, wparam & MK_SHIFT, leftButtonUp );
            mouseEvent( hwnd, lparam, TRUE, leftButtonDoubleClick );
            doubleClickPending = FALSE;
        } else {
            mouseEvent( hwnd, lparam, wparam & MK_SHIFT, leftButtonUp );
        }
        return( 0 );
    case WM_MOUSEMOVE:
        mouseMove( hwnd, (int)(short)LOWORD( lparam ), (int)(short)HIWORD( lparam ), FALSE );
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
            data = DATA_FROM_ID( hwnd );
            SendMessage( EditContainer, WM_MDIRESTORE, (UINT)hwnd, 0L );
            BringUpFile( data->info, TRUE );
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
        data = DATA_FROM_ID( hwnd );
        MemFree( data );
        return( 0 );
    case WM_KILLFOCUS:
        DoneCurrentInsert( TRUE );
        break;
    case WM_SIZE:
        data = DATA_FROM_ID( hwnd );
        DCResize( data->info );
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
                if( IsIconic( cinfo->CurrentWindow ) ) {
                    cinfo = cinfo->next;
                } else {
                    SaveInfo( sinfo );
                    BringUpFile( cinfo, FALSE );
                    return( 0 );
                }
            }
        }
        /* either way we remember to reset extra */
        GetClientRect( hwnd, &data->extra );
        data->extra.top = WindowAuxInfo( hwnd, WIND_INFO_TEXT_LINES ) *
                                         FontHeight( WIN_FONT( &EditWindow ) );
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
    window_data         *data;
    char                class[MAX_STR];

    l = l;
    class[0] = 0;
    GetClassName( hwnd, class, sizeof( class ) );
    class[sizeof( class ) - 1] = 0;
    if( stricmp( EditWindowClassName, class ) ) {
        return( TRUE );
    }

    data = DATA_FROM_ID( hwnd );
    GetClientRect( hwnd, &data->extra );
    data->extra.top = WindowAuxInfo( hwnd, WIND_INFO_TEXT_LINES ) *
                                     FontHeight( WIN_FONT( &EditWindow ) );

    return( TRUE );

} /* ResizeExtra */

/*
 * ResetExtraRects - reset the leftover rectangle at the bottom of all
 *                   edit windows
 */
void ResetExtraRects( void )
{
    FARPROC     proc;

    if( EditContainer != HNULL ) {
        proc = MakeProcInstance( (FARPROC)ResizeExtra, InstanceHandle );
        EnumChildWindows( EditContainer, (WNDENUMPROC)proc, 0L );
        FreeProcInstance( proc );
    }

} /* ResetExtraRects */

static BOOL Fini( window *w, void *parm )
{
    w = w;
    parm = parm;
    return( TRUE );

} /* Fini */
