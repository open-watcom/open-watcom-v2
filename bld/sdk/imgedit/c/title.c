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

/*
 * wTitle - The callback function for the displaying of the title screen.
 */
BOOL CALLBACK wTitle( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
    UINT         msecs, timer, start;
    HDC          dc, tdc;
    HBITMAP      old;
    HWND         w666;
    RECT         rect, arect;
    PAINTSTRUCT  ps;

    static BITMAP    bm;
    static HBITMAP   logo;
    static HBRUSH    brush;
    static COLORREF  color;

    switch ( message ) {
        case WM_INITDIALOG:
            SetWindowText( hwnd, appName );
            msecs = *((UINT *)lparam);
            if ( msecs ) {
                timer = SetTimer( hwnd, TITLE_TIMER, msecs, NULL );
                if ( timer ) {
                    SetWindowLong( hwnd, DWL_USER, (LONG)timer );
                    SendMessage( hwnd, WM_SETTEXT, 0, (LPARAM)IEAppTitle );
                }
            }

            logo = LoadBitmap ( wMainInst, "APPLBITMAP" );
            color = RGB( 192, 192, 192);
            brush = CreateSolidBrush ( color );

            GetObject ( logo, sizeof(BITMAP), &bm );
            return( TRUE );

#ifdef __NT__
        case WM_CTLCOLORSTATIC:
            if ( brush ) {
                dc = (HDC)wparam;
                SetBkColor( dc, color );
                return( (LRESULT) brush );
            }
            break;
#else
        case WM_CTLCOLOR:
            if ( brush ) {
                dc = (HDC) wparam;
                if ( HIWORD(lparam) == CTLCOLOR_STATIC ) {
                    SetBkColor( dc, color );
                }
                return( (LRESULT) brush );
            }
            break;
#endif

        case WM_ERASEBKGND:
            if ( brush ) {
                GetClientRect( hwnd, &rect );
                UnrealizeObject( brush );
                FillRect( (HDC)wparam, &rect, brush );
                return ( TRUE );
            }
            break;

        case WM_PAINT:
            dc = BeginPaint( hwnd, &ps );
            if ( dc ) {
                w666 = GetDlgItem ( hwnd, 666 );
                GetClientRect ( w666, &rect );
                GetClientRect ( hwnd, &arect );
                start = ( arect.right - arect.left - bm.bmWidth ) / 2;
                MapWindowPoints( w666, hwnd, (POINT *)&rect, 2 );
                tdc = CreateCompatibleDC( dc );
                old = SelectObject( tdc, logo );
                BitBlt( dc, start, rect.top + 5, bm.bmWidth, bm.bmHeight,
                         tdc, 0, 0, SRCCOPY );
                SelectObject ( tdc, old );
                DeleteDC ( tdc );
                EndPaint ( hwnd, &ps );
            }
            break;

        case WM_TIMER:
            timer = (UINT) GetWindowLong( hwnd, DWL_USER );
            if ( timer ) {
                KillTimer( hwnd, timer );
            }
            EndDialog( hwnd, TRUE );
            return ( TRUE );
            break;

        case WM_DESTROY:
            if ( logo ) {
                DeleteObject( logo );
            }
            if ( brush ) {
                DeleteObject( brush );
            }
            break;

        default:
            return( FALSE );
    }
    return ( FALSE );
} /* wTitle */

/*
 * DisplayTitleScreen - displays the title screen on startup
 */
void DisplayTitleScreen( HINSTANCE inst, HWND parent, UINT msecs, char *app_name )
{
    FARPROC     fp;
    int         len;

    len = strlen( app_name );
    appName = MemAlloc( len+1 );
    strcpy( appName, app_name );

    wMainInst = inst;
    fp = MakeProcInstance( (FARPROC)wTitle, inst );
    JDialogBoxParam(inst, "WTitleScreen", parent, (DLGPROC)fp, (LPARAM)&msecs);
    FreeProcInstance ( fp );
    MemFree( appName );
}

