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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "imgedit.h"
#include <io.h>
#include "title.h"
#include "jdlg.h"
#include "wclbproc.h"


/* Local Window callback functions prototypes */
WINEXPORT WPI_DLGRESULT CALLBACK wTitleDlgProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );

#define TITLE_TIMER       666

static HINSTANCE        wMainInst = NULL;
static char             *appName;

#ifdef __NT__
typedef HANDLE (WINAPI *PFNLI)( HINSTANCE, LPCSTR, UINT, int, int, UINT );
#endif

/*
 * wTitleDlgProc - callback function for the displaying of the title screen
 */
WPI_DLGRESULT CALLBACK wTitleDlgProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
    UINT        msecs, start;
    UINT_PTR    timer;
    HDC         dc, tdc;
    HBITMAP     old_hbitmap;
    HWND        w666;
    RECT        rect, arect;
    PAINTSTRUCT ps;
#ifdef __NT__
    HINSTANCE   hInstUser;
    PFNLI       pfnLoadImage;
#endif
    bool        ret;

    static BITMAP   bm;
    static HBITMAP  logo_hbitmap;
    static HBRUSH   brush;
    static COLORREF color;

    ret = false;

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
            logo_hbitmap = pfnLoadImage( wMainInst, "APPLBITMAP", IMAGE_BITMAP, 0, 0,
                                 LR_LOADMAP3DCOLORS );
        } else {
#endif
            logo_hbitmap = LoadBitmap( wMainInst, "APPLBITMAP" );
#ifdef __NT__
        }
#endif

        color = GetSysColor( COLOR_BTNFACE );
        brush = CreateSolidBrush( color );

        GetObject( logo_hbitmap, sizeof( BITMAP ), &bm );
        ret = true;
        break;

#ifdef __NT__
    case WM_CTLCOLORSTATIC:
        if( brush != NULL ) {
            dc = (HDC)wparam;
            SetBkColor( dc, color );
            ret = true;
        }
        break;
#else
    case WM_CTLCOLOR:
        if( brush != NULL ) {
            dc = (HDC)wparam;
            if( HIWORD( lparam ) == CTLCOLOR_STATIC ) {
                SetBkColor( dc, color );
            }
            ret = true;
        }
        break;
#endif

    case WM_ERASEBKGND:
        if( brush != NULL ) {
            GetClientRect( hwnd, &rect );
            UnrealizeObject( brush );
            FillRect( (HDC)wparam, &rect, brush );
            ret = true;
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
            old_hbitmap = SelectObject( tdc, logo_hbitmap );
            BitBlt( dc, start, rect.top + 5, bm.bmWidth, bm.bmHeight, tdc, 0, 0, SRCCOPY );
            SelectObject( tdc, old_hbitmap );
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
        ret = true;
        break;

    case WM_DESTROY:
        if( logo_hbitmap != NULL ) {
            DeleteObject( logo_hbitmap );
        }
        if( brush != NULL ) {
            DeleteObject( brush );
        }
        break;
    }
    _wpi_dlgreturn( ret );

} /* wTitleDlgProc */

/*
 * DisplayTitleScreen - display the title screen on startup
 */
void DisplayTitleScreen( HINSTANCE inst, HWND parent, UINT msecs, char *app_name )
{
    DLGPROC     dlgproc;
    int         len;

    len = strlen( app_name );
    appName = MemAlloc( len + 1 );
    strcpy( appName, app_name );

    wMainInst = inst;
    dlgproc = MakeProcInstance_DLG( wTitleDlgProc, inst );
    JDialogBoxParam( inst, "WTitleScreen", parent, dlgproc, (LPARAM)&msecs );
    FreeProcInstance_DLG( dlgproc );
    MemFree( appName );

} /* DisplayTitleScreen */
