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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "spy.h"

static HWND SpyPickWindow;
static HWND SpyPickDialog;
static FARPROC SpyPickInst;
static volatile int QuitPickProc;
static HWND LastFramed;
static BOOL LastFramedPicked;
static BOOL Cancelled;
static BOOL Moving;
static HWND PickDialogOK, PickDialogCancel;
static HWND PickDialogStyleCB, PickDialogStyleClassCB;
static WORD pickProcCmdId;
static RECT NewPosition;
static POINT StartingPoint;
static HDC  MovingDC;
static HDC  MovingPen;
#ifdef USE_SNAP_WINDOW
static HWND snapHwnd;
#endif

/*
 * FrameAWindow - draw a frame around a window
 */
void FrameAWindow( HWND hwnd, BOOL use_snap )
{
    HDC         hdc;
    RECT        rect;
    HPEN        hpen;

    if( hwnd == NULL ) {
        return;
    }

#ifdef USE_SNAP_WINDOW
    if( use_snap ) {
        UpdateWindow( snapHwnd );
        if( snapHwnd == NULL ) {
            return;
        }
        hdc = GetWindowDC( snapHwnd );
    } else  {
        hdc = GetWindowDC( hwnd );
    }
#else
    use_snap = use_snap;
    hdc = GetWindowDC( hwnd );
#endif

    SetROP2( hdc, R2_NOT); /* reverse screen color */

    SelectObject( hdc, GetStockObject( NULL_BRUSH) );

    hpen = CreatePen( PS_INSIDEFRAME, 4 * GetSystemMetrics( SM_CXBORDER ),
                  RGB( 0, 0, 0) );
    SelectObject( hdc, hpen );

    GetWindowRect( hwnd, &rect );

#ifdef USE_SNAP_WINDOW
    if( use_snap ) {
        Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
        ReleaseDC( snapHwnd, hdc );
    } else
#endif
    {
        Rectangle( hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top );
        ReleaseDC( hwnd, hdc );
    }

    DeleteObject( hpen );

} /* FrameAWindow */

/*
 * UpdateFramedInfo
 */
void UpdateFramedInfo( HWND dlg, HWND framedhwnd, BOOL ispick  )
{
    char        name[64];
    char        id[10];
    char        str[512];
    char        *fmtstr;
    int         len;
    RECT        rect;

    if( ispick ) {

        GetWindowName( framedhwnd, str );
        SetDlgItemText( dlg, PEEKMSG_TITLE, str );

        GetWindowName( GetParent( framedhwnd ), str );
        SetDlgItemText( dlg, PEEKMSG_PARENT, str );

        len = GetClassName( framedhwnd, name, sizeof( name ) );
        name[ len ] = 0;
        SetDlgItemText( dlg, PEEKMSG_CLASS, name );

        GetWindowRect( framedhwnd, &rect );
        fmtstr = GetRCString( STR_DIM_COORD_FMT );
        sprintf( str, fmtstr, rect.left, rect.top, rect.right, rect.bottom,
                rect.right-rect.left, rect.bottom - rect.top);
        SetDlgItemText( dlg, PEEKMSG_SIZE, str );

        GetWindowStyleString( framedhwnd, name, str );
        SetDlgItemText( dlg, PEEKMSG_STYLE, name );
        DumpToComboBox( str, GetDlgItem( dlg, PEEKMSG_STYLECB ) );

        GetClassStyleString( framedhwnd, name, str );
        SetDlgItemText( dlg, PEEKMSG_STYLECLASS, name );
        DumpToComboBox( str, GetDlgItem( dlg, PEEKMSG_STYLECLASSCB ) );

    } else {

        GetHexStr( id, (DWORD) framedhwnd, SPYOUT_HWND_LEN );
        id[SPYOUT_HWND_LEN] = 0;
        SetDlgItemText( dlg, WINSEL_HWND, id );
        len = GetWindowText( framedhwnd, name, sizeof( name ) );
        name[ len ] = 0;
        SetDlgItemText( dlg, WINSEL_NAME, name );
    }

} /* UpdateFramedInfo */


/*
 * setLastFramed - set up the last framed window
 */
static BOOL setLastFramed( HWND who )
{
    if( IsMyWindow( who ) ) return( FALSE );
    if( LastFramed == who ) return( TRUE );
    if( LastFramed != NULL ) {
        FrameAWindow( LastFramed, SNAP_MODE );
    }
    FrameAWindow( who, SNAP_MODE );
    LastFramed = who;
    UpdateFramedInfo( SpyPickDialog, LastFramed, (pickProcCmdId == SPY_PEEK_WINDOW) );
    return( TRUE );

} /* setLastFramed */


/*
 * PickDialog - select a window
 */
BOOL CALLBACK PickDialog( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        PickDialogOK = GetDlgItem( hwnd, IDOK );
        PickDialogCancel = GetDlgItem( hwnd, IDCANCEL );
        if( pickProcCmdId == SPY_PEEK_WINDOW ) {
            PickDialogStyleCB = GetDlgItem( hwnd, PEEKMSG_STYLECB );
            PickDialogStyleClassCB = GetDlgItem( hwnd, PEEKMSG_STYLECLASSCB );
        }
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        Ctl3dColorChange();
        break;
#endif
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDCANCEL:
            Cancelled = TRUE;
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case IDOK:
            PostMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        }
        break;
    case WM_CLOSE:
        if( LastFramed != NULL ) {
            FrameAWindow( LastFramed, SNAP_MODE );
        }
        EndDialog( hwnd, 0 );
        DestroyWindow( SpyPickWindow );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );

} /* PickDialog */

/*
 * GetWindowID - get window ID from mouse coordinates
 */
void GetWindowID( HWND hwnd, HWND *who, DWORD lparam )
{
    POINT       p;
    HWND        child;

    p.x = (int_16)LOWORD( lparam );
    p.y = (int_16)HIWORD( lparam );


    ClientToScreen( hwnd, &p );
    *who = WindowFromPoint( p );
    ScreenToClient( *who, &p );
    child = ChildWindowFromPoint( *who, p );
    if( child != NULL ) {
        *who = child;
    }
#ifdef USE_SNAP_WINDOW
    if( *who == snapHwnd ) {
        *who = GetHwndFromPt( &p );
    }
#endif
} /* GetWindowID */

/*
 * MoveMe - move the rectangle
 */
void MoveMe( DWORD pnt, BOOL justdraw )
{
    POINT       p;
    int         dx,dy;

    Rectangle( MovingDC, NewPosition.left, NewPosition.top,
            NewPosition.right, NewPosition.bottom );
    if( justdraw ) return;

    p.x = (int_16)LOWORD( pnt );
    p.y = (int_16)HIWORD( pnt );
    dx = p.x - StartingPoint.x;
    dy = p.y - StartingPoint.y;
    NewPosition.right += dx;
    NewPosition.left += dx;
    NewPosition.top += dy;
    NewPosition.bottom += dy;
    Rectangle( MovingDC, NewPosition.left, NewPosition.top,
            NewPosition.right, NewPosition.bottom );
    StartingPoint = p;

} /* MoveMe */

/*
 * SpyPickProc - handle messages for picking procedure
 */
LONG CALLBACK SpyPickProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    HWND        who;
    static BOOL pdscb=FALSE;
    static BOOL pdsccb=FALSE;

    switch ( msg ) {
    case WM_LBUTTONDOWN:
        GetWindowID( hwnd, &who, lparam );
        if( who == SpyPickWindow ) {
            if( LastFramed != NULL ) {
                FrameAWindow( LastFramed, SNAP_MODE );
            }
            Moving = TRUE;
            GetWindowRect( SpyPickWindow, &NewPosition );
            MovingDC = GetWindowDC( GetDesktopWindow() );
//          SetROP2( MovingDC, R2_XORPEN );
            SetROP2( MovingDC, R2_NOT);
            SelectObject( MovingDC, GetStockObject( NULL_BRUSH) );
            MovingPen = CreatePen( PS_INSIDEFRAME, 1, RGB( 0, 0, 0) );
            SelectObject( MovingDC, MovingPen );
            StartingPoint.x = (int_16)LOWORD( lparam );
            StartingPoint.y = (int_16)HIWORD( lparam );
            MoveMe( 0L, TRUE );
            break;
        }
        if( who == PickDialogStyleCB ) {
            pdscb = !pdscb;
            SendMessage( who, CB_SHOWDROPDOWN, pdscb, 0L );
            break;
        }
        if( who == PickDialogStyleClassCB ) {
            pdsccb = !pdsccb;
            SendMessage( who, CB_SHOWDROPDOWN, pdsccb, 0L );
            break;
        }
        if( who == PickDialogOK || who == PickDialogCancel ) {
            SetFocus( who );
            SendMessage( who, WM_KEYDOWN, VK_SPACE, 0x48390001L );
            SetCapture( SpyPickWindow );
            break;
        }
        if( LastFramedPicked ) {
            setLastFramed( who );
        } else {
            if( !IsMyWindow( who ) ) {
                LastFramedPicked = TRUE;
            }
        }
        break;

    case WM_LBUTTONUP:
        if( Moving ) {
            Moving = FALSE;
            MoveMe( 0L, TRUE );
            ReleaseDC( GetDesktopWindow(), MovingDC );
            DeleteObject( MovingPen );
            MoveWindow( SpyPickWindow, NewPosition.left, NewPosition.top,
                        NewPosition.right - NewPosition.left,
                        NewPosition.bottom - NewPosition.top, TRUE );
            if( LastFramed != NULL ) {
                FrameAWindow( LastFramed, SNAP_MODE );
            }
            break;
        }
        GetWindowID( hwnd, &who, lparam );
        if( who == PickDialogOK || who == PickDialogCancel ) {
            SetFocus( who );
            SendMessage( who, WM_KEYUP, VK_SPACE, 0xC8390001L );
            SetCapture( SpyPickWindow );
            break;
        }
        break;
    case WM_LBUTTONDBLCLK:
        GetWindowID( hwnd, &who, lparam );
        if( setLastFramed( who ) ) {
            PostMessage( SpyPickDialog, WM_COMMAND,
                         GET_WM_COMMAND_MPS( IDOK, 0, 0 ) );
        }
        break;
    case WM_MOUSEMOVE:
        if( Moving ) {
            MoveMe( lparam, FALSE );
            break;
        }
        if( !LastFramedPicked ) {
            GetWindowID( hwnd, &who, lparam );
            setLastFramed( who );
        }
        break;
    case WM_DESTROY:
        QuitPickProc = TRUE;
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* SpyPickProc */

/*
 * DoPickDialog - start dialog for window selection
 */
HWND DoPickDialog( WORD cmdid )
{
    MSG         msg;
    RECT        rect,rect2;
    DWORD       style;
    char        *caption;

    pickProcCmdId = cmdid;
    ShowWindow( SpyMainWindow, SW_MINIMIZE );

    style = WS_CAPTION | DS_MODALFRAME | WS_POPUP;
    if( cmdid == SPY_PEEK_WINDOW ) {
        caption = GetRCString( STR_PEEK_AT_WIN );
    } else {
        caption = GetRCString( STR_SELECT_WIN );
    }

    SpyPickWindow = CreateWindow(
        SpyPickClass,       /* Window class name */
        caption,            /* Window caption */
        style,              /* Window style */
        20,                 /* Initial X position */
        20,                 /* Initial Y position */
        0,                  /* Initial X size */
        0,                  /* Initial Y size */
        SpyMainWindow,      /* Parent window handle */
        (HMENU) NULL,       /* Window menu handle */
        Instance,           /* Program instance handle */
        NULL );             /* Create parameters */

    if( SpyPickWindow == NULL ) return( NULL );

    QuitPickProc = FALSE;
    LastFramed = NULL;
    LastFramedPicked = FALSE;
    Cancelled = FALSE;

#ifdef USE_SNAP_WINDOW
    snapHwnd = DisplayDesktop( (HWND)NULL );
    SetWindowPos( SpyPickWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    SetWindowPos( snapHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    SetWindowPos( SpyPickWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    IdentifyWindows( SpyPickWindow, snapHwnd );
    RemoveWindow( snapHwnd );
#endif
    SpyPickInst = MakeProcInstance( (FARPROC) PickDialog, Instance );
    if( cmdid == SPY_PEEK_WINDOW ) {
        SpyPickDialog = JCreateDialog( ResInstance, "PEEKMSGS", SpyPickWindow,
                (LPVOID) SpyPickInst );
    } else {
        SpyPickDialog = JCreateDialog( ResInstance, "WINDOWPICK", SpyPickWindow,
                (LPVOID) SpyPickInst );
    }
    GetWindowRect( SpyPickWindow, &rect);
    GetWindowRect( SpyPickDialog, &rect2);
    MoveWindow( SpyPickWindow, rect.left, rect.top, rect2.right - rect2.left,
                rect2.bottom - rect2.top + GetSystemMetrics(SM_CYCAPTION), TRUE );
    ShowWindow( SpyPickWindow, SW_NORMAL );
    UpdateWindow( SpyPickWindow );
    SetCapture( SpyPickWindow );
    SetActiveWindow( SpyPickWindow );
    while( !QuitPickProc ) {
        GetMessage( &msg, (HWND) NULL, 0, 0 );
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ReleaseCapture();
    FreeProcInstance( SpyPickInst );
#ifdef USE_SNAP_WINDOW
    DestroyWindow( snapHwnd );
    snapHwnd = NULL;
#endif
    ShowWindow( SpyMainWindow, SW_NORMAL );
    if( !Cancelled ) {
        return( LastFramed );
    }
    return( NULL );

} /* DoPickDialog */
