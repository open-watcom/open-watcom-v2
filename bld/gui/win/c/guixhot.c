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
#include "guixhot.h"
#include "guixdraw.h"

extern  WPI_INST        GUIMainHInst;
extern  WPI_INST        GUIResHInst;

extern  int             GUINumHotSpots;
        hotspot_info    *GUIHotSpots;

bool GUIXInitHotSpots( int num_hot_spots, gui_resource *hot )
{
    int         i;

    for( i = 0; i < num_hot_spots; i++ ) {
        GUIHotSpots[i].bitmap = _wpi_loadbitmap( GUIResHInst,
                                        _wpi_makeintresource( hot[i].res ) );
        _wpi_getbitmapdim( GUIHotSpots[i].bitmap, &GUIHotSpots[i].size.x,
                           &GUIHotSpots[i].size.y );
    }
    return( TRUE );
}

void GUIXCleanupHotSpots( void )
{
    int i;

    for( i = 0; i < GUINumHotSpots; i++ ) {
        _wpi_deletebitmap( GUIHotSpots[i].bitmap );
    }
}

void GUIDrawHotSpot( gui_window *wnd, int hot_spot, gui_ord row,
                     gui_ord indent, gui_attr attr )
{
    gui_text_metrics    metrics;
    gui_coord           pos;

    if( ( hot_spot > 0 ) && ( hot_spot <= GUINumHotSpots ) ) {
        GUIGetTextMetrics( wnd, &metrics );
        pos.x = indent;
        pos.y = row * metrics.avg.y;
        GUIDrawTextBitmapAttr( wnd, NULL, GUIHotSpots[hot_spot - 1].size.x,
                               GUIHotSpots[hot_spot - 1].size.y,
                               &pos, attr, GUI_NO_COLUMN, FALSE,
                               hot_spot );
    }
}

void GUIDrawBitmap( int hot_spot, WPI_PRES hdc, int nDrawX, int nDrawY )
{
    WPI_POINT   src_org;
    WPI_POINT   dst_org;
    WPI_POINT   size;
    WPI_PRES    memDC;
    HBITMAP     old_bmp;
    HBITMAP     bitmap;
    HDC         new_hdc;

    bitmap = GUIHotSpots[hot_spot - 1].bitmap;
    size.x = GUIHotSpots[hot_spot - 1].size.x;
    size.y = GUIHotSpots[hot_spot - 1].size.y;

    src_org.x = 0;
    src_org.y = 0;
    dst_org.x = nDrawX;
    dst_org.y = nDrawY;

    _wpi_dptolp( hdc, &size, 1 );
    _wpi_dptolp( hdc, &dst_org, 1 );
    _wpi_dptolp( hdc, &src_org, 1 );

    memDC = _wpi_createcompatiblepres( hdc, GUIMainHInst, &new_hdc );
    old_bmp = _wpi_selectbitmap( memDC, bitmap );

    _wpi_bitblt( hdc, dst_org.x, dst_org.y, size.x, size.y, memDC,
                 src_org.x, src_org.y, SRCCOPY );

    if( old_bmp != NULLHANDLE ) {
        _wpi_getoldbitmap( memDC, old_bmp );
    }
    _wpi_deletecompatiblepres( memDC, new_hdc );
}
