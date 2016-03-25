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


#include "imgedit.h"
#include <io.h>
#include "iemem.h"
#include "title.h"
#include "jdlg.h"

#define TITLE_TIMER       666

static HINSTANCE        wMainInst = NULL;
static char             *appName;

#ifdef __NT__
typedef HANDLE (WINAPI *PFNLI)( HINSTANCE, LPCSTR, UINT, int, int, UINT );
#endif

/*
 * wTitle - callback function for the displaying of the title screen
 */
BOOL CALLBACK wTitle( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
    UINT        msecs, start;
    UINT_PTR    timer;
    HDC         dc, tdc;
    HBITMAP     old;
    HWND        w666;
    RECT        rect, arect;
    PAINTSTRUCT ps;
#ifdef __NT__
    HINSTANCE   hInstUser;
    PFNLI       pfnLoadImage;
#endif

    static BITMAP    bm;
    static HBITMAP   logo;
    static HBRUSH    brush;
    static COLORREF  color;

    switch ( message ) {
    case WM_INITDIALOG:
        SetWindowText( hwnd, appName );
        msecs = *((UINT *)lparam);
        if( msecs != 0 ) {
            timer = SetTimer( hwnd, TITLE_TIMER, msecs, NULL );
            if( timer != 0 ) {
                SET_DLGDATA( hwnd, timer );
            }
        }

#ifdef __NT__
        hInstUser = GetModuleHandle( "USER32.DLL" );
        pfnLoadImage = (PFNLI)GetProcAddress( hInstUser, "LoadImageA" );
        if( pfnLoadImage != NULL ) {
            logo = pfnLoadImage( wMainInst, "APPLBITMAP", IMAGE_BITMAP, 0, 0,
                                 LR_LOADMAP3DCOLORS );
        } else {
#endif
            logo = LoadBitmap( wMainInst, "APPLBITMAP" );
#ifdef __NT__
        }
#endif

        color = GetSysColor( COLOR_BTNFACE );
        brush = CreateSolidBrush( color );

        GetObject( logo, sizeof( BITMAP ), &bm );
        return( TRUE );

#ifdef __NT__
    case WM_CTLCOLORSTATIC:
        if( brush != NULL ) {
            dc = (HDC)wparam;
            SetBkColor( dc, color );
            return( brush != NULL );
        }
        break;
#else
    case WM_CTLCOLOR:
        if( brush != NULL ) {
            dc = (HDC)wparam;
            if( HIWORD( lparam ) == CTLCOLOR_STATIC ) {
                SetBkColor( dc, color );
            }
            return( brush != NULL );
        }
        break;
#endif

    case WM_ERASEBKGND:
        if( brush != NULL ) {
            GetClientRect( hwnd, &rect );
            UnrealizeObject( brush );
            FillRect( (HDC)wparam, &rect, brush );
            return( TRUE );
        }
        break;

    case WM_PAINT:
        dc = BeginPaint( hwnd, &ps );
        if( dc != NULL ) {
            w666 = GetDlgItem( hwnd, 666 );
            GetClientRect( w666, &rect );
            GetClientRect( hwnd, &arect );
            start = (arect.right - arect.left - bm.bmWidth) / 2;
            MapWindowPoints( w666, hwnd, (POINT *)&rect, 2 );
            tdc = CreateCompatibleDC( dc );
            old = SelectObject( tdc, logo );
            BitBlt( dc, start, rect.top + 5, bm.bmWidth, bm.bmHeight, tdc, 0, 0, SRCCOPY );
            SelectObject( tdc, old );
            DeleteDC( tdc );
            EndPaint( hwnd, &ps );
        }
        break;

    case WM_TIMER:
        timer = (UINT_PTR)GET_DLGDATA( hwnd );
        if( timer != 0 ) {
            KillTimer( hwnd, timer );
        }
        EndDialog( hwnd, TRUE );
        return( TRUE );

    case WM_DESTROY:
        if( logo != NULL ) {
            DeleteObject( logo );
        }
        if( brush != NULL ) {
            DeleteObject( brush );
        }
        break;

    default:
        return( FALSE );
    }
    return( FALSE );

} /* wTitle */

/*
 * DisplayTitleScreen - display the title screen on startup
 */
void DisplayTitleScreen( HINSTANCE inst, HWND parent, UINT msecs, char *app_name )
{
    FARPROC     fp;
    int         len;

    len = strlen( app_name );
    appName = MemAlloc( len + 1 );
    strcpy( appName, app_name );

    wMainInst = inst;
    fp = MakeProcInstance( (FARPROC)wTitle, inst );
    JDialogBoxParam( inst, "WTitleScreen", parent, (DLGPROC)fp, (LPARAM)&msecs );
    FreeProcInstance( fp );
    MemFree( appName );

} /* DisplayTitleScreen */
