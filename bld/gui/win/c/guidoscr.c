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


#include "guiwind.h"
#include "guiscale.h"
#include "guixutil.h"
#include "guiscrol.h"
#include "guipaint.h"

static void DoScroll( gui_window *wnd, int rows, int cols, int start, int end,
                      bool chars )
{
    int         dx, dy;
    WPI_RECT    client;
    HWND        hwnd;
    int         multx, multy;
    GUI_RECTDIM left, top, right, bottom;
#ifdef __OS2_PM__
    int         bottom_adjust;
#endif

    hwnd = GUIGetScrollHWND( wnd );
    _wpi_getclientrect( wnd->hwnd, &client );
    _wpi_getrectvalues( client, &left, &top, &right, &bottom );
#ifdef __OS2_PM__
    bottom_adjust = bottom - GUIGetScrollScreenSize( wnd, SB_VERT );
    top += bottom_adjust;
#else
    bottom = GUIGetScrollScreenSize( wnd, SB_VERT );
#endif
    multx = 1;
    multy = 1;
    if( chars ) {
        multx = GUIFromTextX( multx, wnd );
        multy = GUIFromTextY( multy, wnd );
    }
    dx = -cols * multx;
    dy = -rows * multy;
    if( dy != 0 ) {
#ifdef __OS2_PM__
        start *= multy;
        end *= multy;
        if( end > -multy ) {
            end += multy;
            if( end > bottom ) {
                end = bottom;
            }
            top = _wpi_getheightrect( client ) - end;
            //bottom = end + bottom_adjust;
        }
        if( start > -multy ) {
            //start += multy;
            if( start > bottom ) {
                start = bottom;
            }
            bottom = _wpi_getheightrect( client ) - start;
            //top = start + bottom_adjust;
        }
#else
        start *= multy;
        end *= multy;
        if( start > -multy ) {
            if( start > bottom ) {
                start = bottom;
            }
            top = start;
        }
        if( end > -multy ) {
            end += multy;
            if( end > bottom ) {
                end = bottom;
            }
            bottom = end;
        }
#endif

    }

    if( dx != 0 ) {
        start *= multx;
        end *= multx;
        if( start > -multx ) {
            if( start > right ) {
                start = right;
            }
            left = start;
        }
        if( end > -multx ) {
            end += multx;
            if( end > right ) {
                end = right;
            }
            right = end;
        }
    }

    _wpi_setrectvalues( &client, left, top, right, bottom );
    GUIInvalidatePaintHandles( wnd );
    _wpi_scrollwindow( hwnd, dx, dy, &client, &client );
    _wpi_updatewindow( hwnd );
}

void GUIDoVScroll( gui_window *wnd, int rows )
{
    DoScroll( wnd, rows, 0, -1, -1, TRUE );
}

void GUIDoHScroll( gui_window *wnd, int cols )
{
    DoScroll( wnd, 0, cols, -1, -1, TRUE );
}


void GUIDoVScrollClip( gui_window *wnd, int rows, int start, int end )
{
    DoScroll( wnd, rows, 0, start, end, TRUE );
}

void GUIDoHScrollClip( gui_window *wnd, int cols, int start, int end )
{
    DoScroll( wnd, 0, cols, start, end, TRUE );
}

void GUIDoScroll( gui_window *wnd, int row_col, int bar )
{
    int         rows;
    int         cols;

    rows = 0;
    cols = 0;
    if( bar == SB_HORZ ) {
        cols = row_col;
    } else {
        rows = row_col;
    }
    DoScroll( wnd, rows, cols, -1, -1, FALSE );
}
