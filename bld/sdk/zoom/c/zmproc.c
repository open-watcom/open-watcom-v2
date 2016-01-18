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
* Description:  Watcom ZOOM window procedure.
*
****************************************************************************/


#include <stdlib.h>
#include "wzoom.h"
#include "aboutdlg.h"

#define ZOOM_FREQUENCY          100
#define SCROLL_TIMEOUT          1000
#define ZOOM_MAX                ( 30 * ZOOM_FACTOR )
#define ZOOM_MIN                ZOOM_FACTOR

/* the actual magnification is info->magnif / ZOOM_FACTOR */
#define ZOOM_FACTOR             4

#define MIN_WND_WIDTH           130

#define MAX_XSIZE       ( GetSystemMetrics( SM_CXSCREEN ) / 2 )
#define MAX_YSIZE       ( ( 2 * GetSystemMetrics( SM_CYSCREEN ) ) / 3 )

static POINT    Origin = { 0, 0 };

static void DrawMagnifier( HDC dc, MainWndInfo *info );

/*
 * EraseMagnifier - Erase the Magnifier window
 */

static void EraseMagnifier( HDC dc, MainWndInfo *info ) {

    DrawMagnifier( dc, info );
}

/*
 * EndScrolling - erase the scrolling magnifier and clean up
 */

static void EndScrolling( MainWndInfo *info ) {

    HDC         dc;

    if( info->scrolling ) {
        KillTimer( MainWin, ZM_SCROLL_TIMER );
        dc = GetDC( NULL );
        EraseMagnifier( dc, info );
        ReleaseDC( NULL, dc );
        info->scrolling = FALSE;
    }
}

static void UpdateScrollRange( MainWndInfo *info ) {

    SetScrollRange( info->vscroll, SB_CTL, 0,
                GetSystemMetrics( SM_CYSCREEN ) - info->magsize.y, TRUE );
    SetScrollRange( info->hscroll, SB_CTL, 0,
                GetSystemMetrics( SM_CXSCREEN ) - info->magsize.x, TRUE );
}

static void UpdateScrollPos( MainWndInfo *info ) {

    SetScrollPos( info->vscroll, SB_CTL, info->magpos.y, TRUE );
    SetScrollPos( info->hscroll, SB_CTL, info->magpos.x, TRUE );
}

/*
 * CheckMagnifierPos - make sure the magnifier is completely on the screen
 *                      and fix it if it is not
 */
static BOOL CheckMagnifierPos( MainWndInfo *info ) {

    BOOL        ret;
    int         xmax;
    int         ymax;

    ret = TRUE;
    xmax = GetSystemMetrics( SM_CXSCREEN );
    ymax = GetSystemMetrics( SM_CYSCREEN );
    if( info->magpos.x < 0 ) {
        info->magpos.x = 0;
        ret = FALSE;
    }
    if( info->magpos.y < 0 ) {
        info->magpos.y = 0;
        ret = FALSE;
    }
    if( info->magpos.x + info->magsize.x + 1 >= xmax ) {
        info->magpos.x = xmax - info->magsize.x;
        ret = FALSE;
    }
    if( info->magpos.y + info->magsize.y + 1 >= ymax ) {
        info->magpos.y = ymax - info->magsize.y;
        ret = FALSE;
    }
    return( ret );
}

/*
 * DoScroll - process messages from the scrollbars
 */

static void DoScroll( HWND bar, WPARAM wparam, MainWndInfo *info ) {

    int         delta;
    HDC         dc;

    EndScrolling( info );
    delta = 0;
    switch( wparam ) {
    case SB_LINEUP:
        delta = -2;
        break;
    case SB_LINEDOWN:
        delta = 2;
        break;
    case SB_PAGEUP:
        delta = -20;
        break;
    case SB_PAGEDOWN:
        delta = 20;
        break;
    }
    if( bar == info->vscroll ) {
        info->magpos.y += delta;
    } else {
        info->magpos.x += delta;
    }
    CheckMagnifierPos( info );

    GetScreen( info->screen, &info->magpos, &info->magsize, FALSE );
    dc = GetDC( NULL );
    DrawMagnifier( dc, info );
    ReleaseDC( NULL, dc );

    UpdateScrollPos( info );
    DrawScreen( info->screen, NULL, &Origin, &info->wndsize,
                    &Origin, &info->magsize );
    SetTimer( MainWin, ZM_SCROLL_TIMER, SCROLL_TIMEOUT, NULL );
    info->scrolling = TRUE;
}

/*
 * GetDisplayArea - get the size of the area where the
 *                  image should be displayed
 */

static void GetDisplaySize( HWND hwnd, MainWndInfo *info ) {

    RECT        area;

    GetClientRect( hwnd, &area );
    info->wndsize.x = area.right - area.left
                        - GetSystemMetrics( SM_CXVSCROLL );
    info->wndsize.y = area.bottom - area.top
                        - GetSystemMetrics( SM_CYHSCROLL );
    info->magsize.x = ( info->wndsize.x * ZOOM_FACTOR ) / info->magnif;
    info->magsize.y = ( info->wndsize.y * ZOOM_FACTOR ) / info->magnif;
    UpdateScrollRange( info );
}

/*
 * PositionWidgets - position the scroll bars and zoom buttons in the
 *                      window
 */
static void PositionWidgets( HWND hwnd, MainWndInfo *info ) {
    RECT        area;
    WORD        barheight;
    WORD        barwidth;
    WORD        buttonwidth;

    GetClientRect( hwnd, &area );
    barheight = GetSystemMetrics( SM_CYHSCROLL );
    barwidth = GetSystemMetrics( SM_CXVSCROLL );
    buttonwidth = ( area.right - area.left - barwidth ) / 2;
    MoveWindow( info->hscroll, 0, area.bottom - barheight,
                area.right - area.left - barwidth, barheight, TRUE );
    MoveWindow( info->vscroll, area.right - barwidth, 0,
                barwidth, area.bottom - area.top - barheight, TRUE );
}

static void CreateScrollBars( HWND hwnd, MainWndInfo *info ) {


    info->hscroll = CreateWindow(
        "SCROLLBAR",                            /* Window class name */
        "",                                     /* Window caption */
        WS_CHILD | SBS_HORZ,                    /* Window style */
        0,                                      /* Initial X position */
        0,                                      /* Initial Y position */
        0,                                      /* Initial X size */
        0,                                      /* Initial Y size */
        hwnd,                                   /* Parent window handle */
        NULL,                                   /* Window menu handle */
        Instance,                               /* Program instance handle */
        NULL);                                  /* Create parameters */

    info->vscroll = CreateWindow(
        "SCROLLBAR",                            /* Window class name */
        "",                                     /* Window caption */
        WS_CHILD | SBS_VERT,                    /* Window style */
        0,                                      /* Initial X position */
        0,                                      /* Initial Y position */
        0,                                      /* Initial X size */
        0,                                      /* Initial Y size */
        hwnd,                                   /* Parent window handle */
        NULL,                                   /* Window menu handle */
        Instance,                               /* Program instance handle */
        NULL);                                  /* Create parameters */

    UpdateScrollRange( info );

    ShowWindow( info->vscroll, SW_NORMAL );
    UpdateWindow( info->vscroll );
    ShowWindow( info->hscroll, SW_NORMAL );
    UpdateWindow( info->hscroll );
}

static void DrawMagnifier( HDC dc, MainWndInfo *info ) {

    POINT               pts[5];
    HPEN                oldpen;
    HBRUSH              oldbrush;

    pts[0].x = info->magpos.x - 1;
    pts[0].y = info->magpos.y - 1;
    pts[1].x = pts[0].x + info->magsize.x + 2;
    pts[1].y = pts[0].y;
    pts[2].x = pts[0].x + info->magsize.x + 2;
    pts[2].y = pts[0].y + info->magsize.y + 2;
    pts[3].x = pts[0].x;
    pts[3].y = pts[0].y + info->magsize.y + 2;
    pts[4].x = pts[0].x;
    pts[4].y = pts[0].y;
    GetScreen( info->screen, &info->magpos, &info->magsize, TRUE );

    SetROP2( dc, R2_NOT); /* reverse screen color */
    oldbrush = SelectObject( dc, GetStockObject( NULL_BRUSH) );
    oldpen = SelectObject( dc, info->magnifpen );

    Polyline( dc, pts, 5 );

    SelectObject( dc, oldbrush );
    SelectObject( dc, oldpen );
}

static void GetWndSize( MainWndInfo *info, int *xsize, int *ysize ) {

    *xsize = ( info->magsize.x * info->magnif ) / ZOOM_FACTOR
            + GetSystemMetrics( SM_CXHSCROLL )
            + 2 * GetSystemMetrics( SM_CXFRAME );
    *ysize = ( info->magsize.y * info->magnif ) / ZOOM_FACTOR
            + GetSystemMetrics( SM_CYFRAME )
            + info->caption_hite
            + GetSystemMetrics( SM_CYHSCROLL );
}

static void BeginZooming( HWND hwnd, MainWndInfo *info ) {

    RECT        area;

    EndScrolling( info );
    info->new_look = TRUE;
    info->looking = TRUE;
    GetClientRect( hwnd, &area );
    GetDisplaySize( hwnd, info );
    SetCapture( hwnd );
}

static void EndZooming( MainWndInfo *info ) {

    HDC         dc;

    dc = GetDC( NULL );
    info->looking = FALSE;
    if( !info->new_look ) {
        EraseMagnifier( dc, info );
    }
    ReleaseDC( NULL, dc );
    ReleaseCapture();
    UpdateScrollPos( info );
    info->sizing = FALSE;
    info->new_look = TRUE;
}

static void DoMagnify( HWND hwnd, MainWndInfo *info ) {

    POINT       magsize;
    HDC         dc;

    info->magnif += info->zoomincrement;
    if( info->magnif < ZOOM_MIN ) info->magnif = ZOOM_MIN;
    if( info->magnif > ZOOM_MAX ) info->magnif = ZOOM_MAX;

    /* preserve the sign of the zoomincrment */
    info->zoomincrement /= abs( info->zoomincrement );
    info->zoomincrement *= info->magnif / 4 + 1;

    magsize = info->magsize;
    GetDisplaySize( hwnd, info );

    /* correct the magnifier position so we magnify about the centre
     * instead of the corner */
    info->magpos.x += ( magsize.x - info->magsize.x ) / 2;
    info->magpos.y += ( magsize.y - info->magsize.y ) / 2;

    GetScreen( info->screen, &info->magpos, &info->magsize, TRUE );
    dc = GetDC( hwnd );
    DrawScreen( info->screen, dc, &Origin, &info->wndsize,
                    &Origin, &info->magsize );
    ReleaseDC( hwnd, dc );
}

/*
 * DoMouseMove - move or resize the magnifier based on a WM_MOUSEMOVE
 *               message
 */

static void DoMouseMove( MainWndInfo *info, LPARAM lparam ) {

    int                 xpos;
    int                 ypos;
    int                 xsize;
    int                 ysize;
    BOOL                resetcursor;
    POINT               oldmagsize;
    HDC                 dc;

    xpos = (int_16)LOWORD( lparam ) + info->wndpos.x;
    ypos = (int_16)HIWORD( lparam ) + info->wndpos.y;

    dc = GetDC( NULL );
    if( !info->new_look ) {
        EraseMagnifier( dc, info );
    } else {
        info->new_look = FALSE;
    }

    /* resize the magnifier if needed */
    if( info->sizing ) {
        resetcursor = FALSE;
        oldmagsize = info->magsize;

        info->magsize.x = xpos - info->magpos.x;
        info->magsize.y = ypos - info->magpos.y;

        if( info->magsize.y < 1 ) {
            info->magsize.y = 1;
            resetcursor = TRUE;
            ypos = info->magpos.y + info->magsize.y;
        }
        GetWndSize( info, &xsize, &ysize );
        if( info->magsize.x > oldmagsize.x && xsize > MAX_XSIZE ) {
            info->magsize.x = oldmagsize.x;
            resetcursor = TRUE;
        }
        if( info->magsize.y > oldmagsize.y && ysize > MAX_YSIZE ) {
            info->magsize.y = oldmagsize.y;
            resetcursor = TRUE;
        }
        if( xsize < MIN_WND_WIDTH ) {
            info->magsize.x = ( ( MIN_WND_WIDTH -
                    GetSystemMetrics( SM_CXVSCROLL )
                    - 2 * GetSystemMetrics( SM_CXFRAME ) ) * ZOOM_FACTOR )
                    / info->magnif;
            resetcursor = TRUE;
        }
        if( resetcursor ) {
            xpos = info->magpos.x + info->magsize.x;
            ypos = info->magpos.y + info->magsize.y;
            SetCursorPos( xpos, ypos );
        }
    }

    /* move the magnifier */

    info->magpos.x = xpos - info->magsize.x;
    info->magpos.y = ypos - info->magsize.y;
    CheckMagnifierPos( info );
    DrawMagnifier( dc, info );
    if( !info->sizing ) {
        DrawScreen( info->screen, NULL, &Origin, &info->wndsize,
                    &Origin, &info->magsize );
    }
    ReleaseDC( NULL, dc );
}

static void displayAbout( HWND hwnd ) {

    about_info          ai;

    ai.owner = hwnd;
    ai.inst = Instance;
    ai.name = AllocRCString( STR_ABOUT_NAME );
    ai.version = AllocRCString( STR_ABOUT_VERSION );
    ai.first_cr_year = "1994";
    ai.title = AllocRCString( STR_ABOUT_TITLE );
    DoAbout( &ai );
    FreeRCString( (char *)ai.name );
    FreeRCString( (char *)ai.version );
    FreeRCString( (char *)ai.title );
}

LRESULT CALLBACK ZOOMMainWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    MainWndInfo         *info;
    HDC                 dc;
    int                 xsize;
    int                 ysize;
    PAINTSTRUCT         paintinfo;
    MINMAXINFO          *mminfo;
    HMENU               mh;
    WORD                item;
    WORD                flags;


    info = (MainWndInfo *)GET_WNDINFO( hwnd );
    switch( msg ) {
    case WM_CREATE:
        info = (MainWndInfo *)( ( (CREATESTRUCT *)lparam )->lpCreateParams );
        info->screen = InitScreenBitmap( hwnd );
        info->magnif = 5 * ZOOM_FACTOR;
        info->magnifpen = CreatePen( PS_INSIDEFRAME, 1, RGB( 0, 0, 0) );
        info->zoomincrement = 0;
        mh = GetMenu( hwnd );
        if( ConfigInfo.topmost ) {
            SetWindowPos( hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE );
        }
        if( ConfigInfo.autorefresh ) {
            DoAutoRefresh( hwnd, TRUE );
        }
        CreateScrollBars( hwnd, info );
        PositionWidgets( hwnd, info );
        SET_WNDINFO( hwnd, (LONG_PTR)info );
        break;
    case WM_PAINT:
        BeginPaint( hwnd, &paintinfo );
        DrawScreen( info->screen, paintinfo.hdc, &Origin, &info->wndsize, &Origin, &info->magsize );
        EndPaint( hwnd, &paintinfo );
        break;
    case WM_SIZE:
        EndScrolling( info );
        PositionWidgets( hwnd, info );
        GetDisplaySize( hwnd, info );
        CheckMagnifierPos( info );
        GetScreen( info->screen, &info->magpos, &info->magsize, FALSE );
        DrawScreen( info->screen, NULL, &Origin, &info->wndsize, &Origin, &info->magsize );
        break;
    case WM_HSCROLL:
    case WM_VSCROLL:
        DoScroll( GET_WM_VSCROLL_HWND( wparam, lparam ),
                  GET_WM_VSCROLL_CODE( wparam, lparam ), info );
        break;
    case WM_MENUSELECT:
        mh = GET_WM_MENUSELECT_HMENU( wparam, lparam );
        flags = GET_WM_MENUSELECT_FLAGS( wparam, lparam );
        item = GET_WM_MENUSELECT_ITEM( wparam, lparam );
        if( flags == (WORD)-1 && mh == 0
            && info->zoomincrement != 0 ) {
            info->zoomincrement = 0;
            KillTimer( hwnd, ZM_TIMER );
            break;
        } else if( item == ZMMENU_ZOOMIN ) {
            EndScrolling( info );
            info->zoomincrement = 1;
            info->zoomincrement *= info->magnif / 4 + 1;
        } else if( item == ZMMENU_ZOOMOUT ) {
            EndScrolling( info );
            info->zoomincrement = -1;
            info->zoomincrement *= info->magnif / 4 + 1;
        } else {
            break;
        }
        DoMagnify( hwnd, info );
        SetTimer( hwnd, ZM_TIMER, ZOOM_FREQUENCY, NULL );
        break;
    case WM_TIMER:
        switch( wparam ) {
        case ZM_TIMER:
            DoMagnify( hwnd, info );
            break;
        case ZM_SCROLL_TIMER:
            EndScrolling( info );
            break;
        case ZM_REFRESH_TIMER:
#if(0)
            if( info->scrolling || ( info->looking && !info->new_look ) ) {
                dc = GetDC( NULL );
                DrawMagnifier( dc, info );
                ReleaseDC( NULL, dc );
            }
#endif
            if( !info->sizing ) {
                GetScreen( info->screen, &info->magpos, &info->magsize, TRUE );
                DrawScreen( info->screen, NULL, &Origin, &info->wndsize,
                                &Origin, &info->magsize );
            }
#if(0)
            if( info->scrolling || ( info->looking && !info->new_look ) ) {
                dc = GetDC( NULL );
                DrawMagnifier( dc, info );
                ReleaseDC( NULL, dc );
            }
#endif
            break;
        }
        break;
    case WM_GETMINMAXINFO:
        mminfo = (MINMAXINFO *)lparam;
        mminfo->ptMinTrackSize.x = MIN_WND_WIDTH;
        break;
    case WM_MOVE:
        {
            RECT        wndarea;

            info->wndpos.x = (int_16)LOWORD( lparam );
            info->wndpos.y = (int_16)HIWORD( lparam );
            GetWindowRect( hwnd, &wndarea );
            info->caption_hite = info->wndpos.y - wndarea.top;
        }
        break;
    case WM_RBUTTONDOWN:
        info->sizing = TRUE;
        break;
    case WM_RBUTTONUP:
        info->sizing = FALSE;
        dc = GetDC( NULL );
        EraseMagnifier( dc, info );
        ReleaseDC( NULL, dc );
        GetWndSize( info, &xsize, &ysize );
        SetWindowPos( hwnd, (HWND)NULL, 0, 0, xsize, ysize, SWP_NOZORDER | SWP_NOMOVE );
        dc = GetDC( NULL );
        DrawMagnifier( dc, info );
        ReleaseDC( NULL, dc );
        break;
    case WM_LBUTTONDOWN:
        info->looking = !info->looking;
        if( info->looking ) {
            BeginZooming( hwnd, info );
        } else {
            EndZooming( info );
        }
        break;
    case WM_LBUTTONUP:
        if( info->looking && !ConfigInfo.stickymagnifier ) {
            EndZooming( info );
        }
        break;
    case WM_MOUSEMOVE:
        if( info->looking ) {
            DoMouseMove( info, lparam );
        }
        break;
    case WM_KEYDOWN:
        if( info->looking ) {
            switch( wparam ) {
            case VK_RETURN:
            case VK_ESCAPE:
                EndZooming( info );
                break;
            }
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case ZMMENU_BEGIN:
            BeginZooming( hwnd, info );
            break;
        case ZMMENU_CONFIG:
            DoConfig( hwnd );
            break;
        case ZMMENU_PASTE:
            CopyToClipBoard( info );
            break;
        case ZMMENU_ABOUT:
            displayAbout( hwnd );
            break;
        case ZMMENU_SETLIMITS:
            break;
        case ZMMENU_EXIT:
            DestroyWindow( hwnd );
            break;
        }
        break;
    case WM_QUERYENDSESSION:
        SaveConfig();
        return( TRUE );
        break;
    case WM_DESTROY:
        EndScrolling( info );
        FiniScreenBitmap( info->screen );
        SaveConfig();
        DeleteObject( info->magnifpen );
        FreeRCString( AppName );
        PostQuitMessage( 0 );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );
}

void DoAutoRefresh( HWND mainhwnd, BOOL on ) {

    UINT        ret;

    KillTimer( mainhwnd, ZM_REFRESH_TIMER );
    ConfigInfo.autorefresh = FALSE;
    if( on ) {
        ret = SetTimer( mainhwnd, ZM_REFRESH_TIMER,
                        ConfigInfo.refresh_interval * 100, NULL );
        if( !ret ) {
            RCMessageBox( mainhwnd, STR_UNABLE_TO_AUTOREFRESH,
                        AppName, MB_OK | MB_ICONEXCLAMATION );
            ConfigInfo.autorefresh = FALSE;
        } else {
            ConfigInfo.autorefresh = TRUE;
        }
    }
}
