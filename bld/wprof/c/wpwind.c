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
* Description:  Profiler window callback routines.
*
****************************************************************************/


#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "msg.h"


extern void     WndGetClientRect( a_window *, gui_rect * );

extern a_window *   WndMain;

static int          wndInSetRowHeight;



extern gui_ord WPGetClientHeight( a_window * wnd )
/************************************************/
{
    gui_rect    rect;

    GUIGetClientRect( WndGui( wnd ), &rect );
    return( rect.height );
}



extern gui_ord WPGetClientWidth( a_window * wnd )
/***********************************************/
{
    gui_rect    rect;

    GUIGetClientRect( WndGui( wnd ), &rect );
    return( rect.width+GUIGetHScroll( WndGui( wnd ) ) );
}



extern void WPAdjustRowHeight( a_window * wnd, bint initial_set )
/***************************************************************/
{
    gui_rect    main_rect;
    gui_rect    full_rect;
    gui_rect    rect;
    gui_ord     old_height;
    gui_ord     new_height;

    if( wndInSetRowHeight > 0 ) return;
    if( initial_set ) {
        wndInSetRowHeight++;
    }
    GUIGetRect( WndGui( wnd ), &full_rect );
    GUIGetClientRect( WndGui( wnd ), &rect );
    old_height = full_rect.height;
    full_rect.height -= rect.height;
    full_rect.height += (WndTitleSize( wnd ) + WndRows( wnd ))
                        * WndMaxCharY( wnd );
    if( old_height != full_rect.height ) {
        if( old_height > full_rect.height ) {
            new_height = full_rect.height + WndMaxCharY( wnd );
            GUIGetClientRect( WndGui( WndMain ), &main_rect );
            if( new_height <= main_rect.height ) {
                full_rect.height = new_height;
            }
        }
        if( old_height != full_rect.height ) {
            WndResizeWindow( wnd, &full_rect );
        }
    }
    if( initial_set ) {
        wndInSetRowHeight--;
    }
}



extern void WPSetRowHeight( a_window * wnd )
/******************************************/
{
    WPAdjustRowHeight( wnd, P_TRUE );
}



extern gui_ord WPPixelTruncWidth( gui_ord width )
/***********************************************/
{
    gui_coord       slacker;

    slacker.x = width;
    GUITruncToPixel( &slacker );
    return( slacker.x );
}



extern gui_ord WPPixelWidth( a_window * wnd )
/*******************************************/
{
    gui_coord   pixel_size;
    gui_coord   tmp;

    pixel_size.x = WndMaxCharX( wnd );
    GUITruncToPixel( &pixel_size );
    tmp.x = pixel_size.x - 1;
    GUITruncToPixel( &tmp );
    return( pixel_size.x - tmp.x );
}
