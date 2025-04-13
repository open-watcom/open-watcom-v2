/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
#include "guidoscr.h"


static void DoScroll( gui_window *wnd, int rows_diff, int cols_diff, int start, int end, bool char_unit )
{
    int         dx, dy;
    WPI_RECT    wpi_rect;
    HWND        hwnd;
    guix_ord    multx, multy;
    WPI_RECTDIM left, top, right, bottom;
#ifdef __OS2_PM__
    int         bottom_adjust;
#endif

    hwnd = GUIGetScrollHWND( wnd );
    _wpi_getclientrect( wnd->hwnd, &wpi_rect );
    _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
#ifdef __OS2_PM__
    bottom_adjust = bottom - GUIGetScrollScreenSize( wnd, SB_VERT );
    top += bottom_adjust;
#else
    bottom = GUIGetScrollScreenSize( wnd, SB_VERT );
#endif
    multx = 1;
    multy = 1;
    if( char_unit ) {
        multx = GUITextToScreenH( 1, wnd );
        multy = GUITextToScreenV( 1, wnd );
    }
    dx = -cols_diff * multx;
    dy = -rows_diff * multy;
    if( dy != 0 ) {
#ifdef __OS2_PM__
        start *= multy;
        end *= multy;
        if( end > -multy ) {
            end += multy;
            if( end > bottom ) {
                end = bottom;
            }
            top = _wpi_getheightrect( wpi_rect ) - end;
            //bottom = end + bottom_adjust;
        }
        if( start > -multy ) {
            //start += multy;
            if( start > bottom ) {
                start = bottom;
            }
            bottom = _wpi_getheightrect( wpi_rect ) - start;
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

    _wpi_setrectvalues( &wpi_rect, left, top, right, bottom );
    GUIInvalidatePaintHandles( wnd );
    _wpi_scrollwindow( hwnd, dx, dy, &wpi_rect, &wpi_rect );
    _wpi_updatewindow( hwnd );
}

void GUIAPI GUIDoVScroll( gui_window *wnd, int rows_diff )
{
    DoScroll( wnd, rows_diff, 0, -1, -1, true );
}

void GUIAPI GUIDoHScroll( gui_window *wnd, int cols_diff )
{
    DoScroll( wnd, 0, cols_diff, -1, -1, true );
}


void GUIAPI GUIDoVScrollClip( gui_window *wnd, int rows_diff, int start_row, int end_row )
{
    DoScroll( wnd, rows_diff, 0, start_row, end_row, true );
}

void GUIAPI GUIDoHScrollClip( gui_window *wnd, int cols_diff, int start_col, int end_col )
{
    DoScroll( wnd, 0, cols_diff, start_col, end_col, true );
}

void GUIDoScroll( gui_window *wnd, int diff, int bar )
{
    int         cols_diff;
    int         rows_diff;

    if( bar == SB_HORZ ) {
        cols_diff = diff;
        rows_diff = 0;
    } else {
        rows_diff = diff;
        cols_diff = 0;
    }
    DoScroll( wnd, rows_diff, cols_diff, -1, -1, false );
}
