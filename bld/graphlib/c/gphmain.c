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


#include "gdefn.h"
#undef HANDLE           // already defined by WPI
#undef GLOBALHANDLE
#include "win.h"

#if defined( __OS2__ )
#include "pmmenu.h"
#endif

// Amount of pixels to scroll for a line
#define LINEAMT 20

// Structure that is used to contain the scrolling information
static struct ScrollStruct {
    int         max;
    int         pixls;
    int         maxpos;
    int         minpos;
    int         currpos;
    int         line;
    int         page;
    int         currcoord;
};
#if defined( __OS2__ )
//Original Frame processing procedure
PFNWP _OldFrameProc;
extern HWND     _GetWinMenuHandle();
#endif
// Variable that contains the position of where to start the repaint
struct xycoord  _BitBlt_Coord;

// Static functions
static double CalScrollAmt( double, double, double, double, double );
static double CalScrollPos( double, double, double, double, double );
static struct ScrollStruct getscrolldata( HWND Wnd, int dir );
static void CalPos( struct ScrollStruct *info, WPI_PARAM1 wParam,
/*================*/WPI_PARAM2 lParam, int *newpos, int *newcoord );




WPI_MRESULT CALLBACK GraphWndProc( HWND         Wnd,
                                   WPI_MSG      message,
                                   WPI_PARAM1   wParam,
                                   WPI_PARAM2   lParam )
//=============================================================

{
    WPI_PRES            Win_dc;
    PAINTSTRUCT         ps;
    int                 width, height;
    int                 x, y, t;
    int                 h_minpos, h_maxpos, h_currpos;
    int                 v_minpos, v_maxpos, v_currpos;
    WPI_RECTDIM         left, top, right, bottom;
    WPI_RECT            rect;
    struct ScrollStruct scroll_info;
    LPWDATA             w;
    HWND                frame;
#if defined( __OS2__ )
    int                 wheight;
    WPI_RECT            wrect;
#endif

    w = _GetWindowData( Wnd );
    if( w == NULL ) {
        return( _wpi_defwindowproc( Wnd, message, wParam, lParam ) );
    }
#if defined( __OS2__ )
    frame = w->frame;
#else
    frame = Wnd;
#endif

    switch( message ) {
#if defined( __WINDOWS__ )
    case WM_SETFOCUS:
        SetFocus( _MainWindow );
    case WM_SYSCOMMAND:
    case WM_MOUSEACTIVATE:
        _MakeWindowActive( w );
        return( _wpi_defwindowproc( Wnd, message, wParam, lParam ) );

    case WM_KILLFOCUS:
        if( ( wParam != NULL) && ( wParam != _MainWindow ) ) {
            _ShowWindowActive( NULL, w );
        }
        return( _wpi_defwindowproc( Wnd, message, wParam, lParam ) );
#else
    case WM_FOCUSCHANGE:
    case WM_SYSCOMMAND:
        _MakeWindowActive( w );
        return( _wpi_defwindowproc( Wnd, message, wParam, lParam ) );
#endif

    case WM_PAINT:
        // Setup
        Win_dc = _wpi_beginpaint( Wnd, NULLHANDLE, &ps );
        _wpi_torgbmode( Win_dc );
        _wpi_getpaintrect( &ps, &rect );
        _wpi_getwrectvalues( rect, &left, &top, &right, &bottom );
        width = _wpi_getwidthrect( rect );
        height = _wpi_getheightrect( rect );

        // Copy from the memory dc to the screen
#if defined( __OS2__ )
        GetClientRect( Wnd, &wrect );
        wheight = _wpi_getheightrect( wrect );
        y = _GetPresHeight() - wheight - _BitBlt_Coord.ycoord + top;
#else
        y = _BitBlt_Coord.ycoord + top;
#endif
        _wpi_bitblt( Win_dc,
                left,
                top,
                width, height,
                _Mem_dc,
                _BitBlt_Coord.xcoord + left,
                y,
                SRCCOPY );

        //Cleanup
        _wpi_endpaint( Wnd, Win_dc, &ps );
        break;
#if defined( __OS2__ )
    case WM_CLOSE:
        WinDestroyWindow( w->frame );
        return( 0 );
#endif
    case WM_DESTROY:
        // free the system resources allocated

        if( _Mem_dc ){
            _wpi_deletecliprgn( _Mem_dc, _ClipRgn );
            _wpi_deletecompatiblepres( _Mem_dc, _Hdc );
            if( _Mem_bmp ){
                _wpi_deletebitmap( _Mem_bmp );
                if( !_IsStockFont() ){
                    _wpi_f_deletefont( _CurFnt );
                }
            }
#if defined( __OS2__ )
            WinSendMsg( _GetWinMenuHandle(), ( ULONG )MM_DELETEITEM,
                MPFROM2SHORT( ( w->handles[0] + DID_WIND_STDIO ), FALSE ), 0 );
#endif
            _DestroyAWindow( w );
        }
        break;

    case WM_VSCROLL:
        scroll_info = getscrolldata( frame, SB_VERT );
        CalPos( &scroll_info, wParam, lParam, &v_currpos, &y );

        // Make sure we have to refresh first
        if( _BitBlt_Coord.ycoord != y ) {
            _BitBlt_Coord.ycoord = y;
            _wpi_setscrollpos( frame,
                          SB_VERT,
                          v_currpos,
                          TRUE );
            _wpi_invalidaterect( Wnd, NULL, 0 );
            _wpi_updatewindow( Wnd );
        }
        return( _wpi_defwindowproc( Wnd, message, wParam, lParam ) );

    case WM_HSCROLL:
        scroll_info = getscrolldata( frame, SB_HORZ );
        CalPos( &scroll_info, wParam, lParam, &h_currpos, &x );

        // make sure we need to refresh first
        if( _BitBlt_Coord.xcoord != x ) {
            _BitBlt_Coord.xcoord = x;
            _wpi_setscrollpos(  frame,
                                SB_HORZ,
                                h_currpos,
                                TRUE );
            _wpi_invalidaterect( Wnd, NULL, 0 );
            _wpi_updatewindow( Wnd );
        }
        return( _wpi_defwindowproc( Wnd, message, wParam, lParam ) );

    case WM_SIZE:
        x = _wpi_getsystemmetrics( SM_CXSCREEN );
        y = _wpi_getsystemmetrics( SM_CYSCREEN );

        _wpi_getscrollrange( frame, SB_VERT, &v_minpos, &v_maxpos );
        _wpi_getscrollrange( frame, SB_HORZ, &h_minpos, &h_maxpos );
        _wpi_getclientrect( Wnd, &rect );
        _wpi_getrectvalues( rect, &left, &top, &right, &bottom );
        height = bottom - top + 1;
        width = right - left + 1;

        if( width >= x ) {
            // hide the scroll bar
#if defined( __OS2__ )
            WinShowWindow( WinWindowFromID( frame, FID_HORZSCROLL ),
                           FALSE );
#else
            ShowScrollBar( Wnd, SB_HORZ, FALSE );
#endif
            _BitBlt_Coord.xcoord = 0;
            h_currpos = 0;
        } else {
            // if the window isn't as big as the device context
            // show the scroll bar
            t = x - width + 1;
            if( x - _BitBlt_Coord.xcoord + 1 < width ) {
                _BitBlt_Coord.xcoord = x - width + 1;
            }
            if( _BitBlt_Coord.xcoord == t ) {
                h_currpos = h_maxpos;
            } else {
            h_currpos = CalScrollAmt( h_maxpos, h_minpos,
                                      _BitBlt_Coord.xcoord,
                                      x, width );
            }
#if defined( __OS2__ )
            WinShowWindow( WinWindowFromID( frame, FID_HORZSCROLL ),
                           TRUE );
#else
            ShowScrollBar( Wnd, SB_HORZ, 1 );
#endif
        }

        if( height >= y ) {
            // hide the scroll bar
#if defined( __OS2__ )
            WinShowWindow( WinWindowFromID( frame, FID_VERTSCROLL ),
                           FALSE );
#else
            ShowScrollBar( Wnd, SB_VERT, 0 );
#endif
            _BitBlt_Coord.ycoord = 0;
            v_currpos = 0;
        } else {
            // if the the window isn't as big as the device context
            // then show the scroll bar
            t = y - height + 1;
            if( y - _BitBlt_Coord.ycoord + 1 < height ) {
                _BitBlt_Coord.ycoord = t;
            }
            if( _BitBlt_Coord.ycoord == t ) {
                v_currpos = v_maxpos;
            } else {
                v_currpos = CalScrollAmt( v_maxpos, v_minpos,
                                          _BitBlt_Coord.ycoord,
                                          y, height );
            }
#if defined( __OS2__ )
            WinShowWindow( WinWindowFromID( frame, FID_VERTSCROLL ),
                           TRUE );
#else
            ShowScrollBar( Wnd, SB_VERT, 1 );
#endif
        }


        // Adjust the scroll bar thumbs' positions
        _wpi_setscrollpos( frame, SB_HORZ, h_currpos, TRUE );
        _wpi_setscrollpos( frame, SB_VERT, v_currpos, TRUE );
#if defined( __OS2__ )
        Win_dc = WinBeginPaint( Wnd, NULL, &rect );
        WinFillRect( Win_dc, &rect, CLR_BLACK );
        WinEndPaint( Win_dc );
#endif
        _wpi_invalidaterect( Wnd, NULL, 0 );
        break;
    default:
        return( _wpi_defwindowproc( Wnd, message, wParam, lParam ) );
    }
   return( NULL );
}


static double CalScrollAmt( double max, double min, double pixel,
/*==================*/double pix_scr, double pix_win )
//===============================================================
{
    return( ( ( max - min + 1 ) * pixel ) / ( pix_scr - pix_win + 1 ) );
}

static double CalScrollPos( double maxpos, double minpos,
/*========================*/double currpos, double num_pix, double win_pix)
//=========================================================================
{
    return( ( num_pix - win_pix + 1 ) * ( currpos - minpos + 1) /
            ( maxpos - minpos + 1) );
}


static struct ScrollStruct getscrolldata( HWND Wnd, int dir )
/*===========================================================

   This function gets the information needed to scroll a window properly
*/
{
    WPI_RECT            rect;
    struct ScrollStruct info;
    WPI_RECTDIM         left, right, top, bottom;

    //Get the necessary data
   _wpi_getscrollrange( Wnd, dir, &info.minpos, &info.maxpos );
   info.currpos = _wpi_getscrollpos( Wnd, dir );
   _wpi_getclientrect( Wnd, &rect );
   _wpi_getrectvalues( rect, &left, &top, &right, &bottom );

   if( dir == SB_VERT ){
       info.max = _wpi_getsystemmetrics( SM_CYSCREEN );
       info.pixls = bottom - top;
       info.currcoord = _BitBlt_Coord.ycoord;
   } else {
       info.max = _wpi_getsystemmetrics( SM_CXSCREEN );
       info.pixls = right - left;
       info.currcoord = _BitBlt_Coord.xcoord;
   }

   // Amount to move the thumb by for every line scrolled
   if( info.pixls > LINEAMT ) {
       info.line = CalScrollAmt( info.maxpos, info.minpos,
                                  LINEAMT, info.max,
                                  info.pixls + 1);
   } else {
       info.line = info.pixls;
   }

   // Amount to move the thumb by for every page scrolled
   info.page = CalScrollAmt( info.maxpos, info.minpos,
                              info.pixls, info.max,
                              info.pixls + 1);
   return( info );
}



static void CalPos( struct ScrollStruct *info, WPI_PARAM1 wParam,
/*================*/WPI_PARAM2 lParam, int *newpos, int *newcoord )
/*===========================================================

   This function calculates the new scroll bar position
   and the new BitBlt coordinates */
{
    int         t;

#if defined( __OS2__ )

    wParam = wParam;
    switch( SHORT2FROMMP( lParam ) ) {
#else
    switch( wParam ) {

        case SB_BOTTOM:
            // Goto the bottom
            *newcoord = info->max - info->pixls;
            if( *newcoord < 0 ){
                newcoord = 0;
            }
            *newpos = info->maxpos;
            break;

        case SB_TOP:
            // Goto the top
            *newcoord = 0;
            *newpos = info->minpos;
            break;
#endif

        case SB_LINEDOWN:
            // Scroll down one line
            *newcoord = info->max - info->pixls;
            t = info->currcoord + LINEAMT;

            // Make sure we don't go past the limit
            if( t < *newcoord ) {
                *newcoord = t;
                if( info->currpos < ( info->maxpos - info->line ) ) {
                    *newpos = info->currpos + info->line;
                }
            } else {
                *newpos = info->maxpos;
            }
            break;

        case SB_LINEUP:
            // Scroll up one line
            *newcoord = 0;
            t = info->currcoord - LINEAMT;

            // Make sure we don't go past the limit
            if( t > *newcoord ) {
                *newcoord = t;
                if( info->currpos > ( info->minpos + info->line ) ) {
                    *newpos = info->currpos - info->line;
                }
            } else {
                *newpos = info->minpos;
            }
            break;

        case SB_PAGEUP:
            // Scroll one page up
            *newcoord = 0;
            t = info->currcoord - info->pixls;

            // Make sure we don't go past the limit
            if( t > *newcoord ) {
                *newcoord = t;
                if( info->currpos > ( info->minpos + info->page ) ) {
                    *newpos = info->currpos - info->page;
                }
            } else {
                *newpos = info->minpos;
            }
            break;

        case SB_PAGEDOWN:
            // Scroll down one page
            *newcoord = info->max - info->pixls;
            t = info->currcoord + info->pixls;

            // Make sure we don't go pass the limit
            if( t < *newcoord ) {
                *newcoord = t;
                if( info->currpos < ( info->maxpos - info->page ) ) {
                    *newpos = info->currpos + info->page;
                }
            } else {
                *newpos = info->maxpos;
            }
            break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            // go to a specific location
            *newpos = LOWORD( lParam );
            *newcoord = CalScrollPos( info->maxpos, info->minpos,
                                      *newpos, info->max, info->pixls);
            break;

        default:
            *newcoord = info->currcoord;
            *newpos   = info->currpos;
    }
}

#if defined( __OS2__ )
WPI_MRESULT CALLBACK GraphFrameProc( HWND       Wnd,
                                     WPI_MSG    message,
                                     WPI_PARAM1 wParam,
                                     WPI_PARAM2 lParam )
/*======================================================
  This function is used to control the frame of the window in OS/2. */
{
    MRESULT     rc;
    TRACKINFO*  Track;

    switch( message ) {
    case WM_QUERYTRACKINFO:
        rc = _OldFrameProc( Wnd, message, wParam, lParam );
        Track = lParam;
        // Set the maximum size
        Track->ptlMaxTrackSize.x =
                WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN ) +
                WinQuerySysValue( _MainWindow, SV_CXBORDER ) * 2;
        Track->ptlMaxTrackSize.y =
                WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) +
                ( WinQuerySysValue( _MainWindow, SV_CYBORDER ) * 2 ) +
                WinQuerySysValue( _MainWindow, SV_CYTITLEBAR );
        return( rc );
    default:
        return( _OldFrameProc( Wnd, message, wParam, lParam ) );
    }
}

#endif
