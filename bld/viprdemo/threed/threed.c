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


#include "icgr.h"
#include "threed.h"

// Definitions for the 3D graphics library

static three_d_op Three_d;

#if defined(__OS2__)
    static WPI_INST hInstDLL;   // passed as an argument but not used
#else
    extern WPI_INST hInstDLL;
#endif

static void init_3d( three_d_op * three_d )
{
    three_d->view.h_angle = 45;
    three_d->view.v_angle = 10;
    three_d->view.perspective = 70;             // Percentage, 100 is too high
    three_d->view.zoom = 100;                   // Percentage
    three_d->view.type = PROJ_PERSPECTIVE;      // PARALLEL or PERSPECTIVE
    three_d->lighting.type = LIGHT_MANUAL;      // AUTO (based on chart type) or MANUAL
    three_d->lighting.vert = LIGHT_VERT_TOP;    // TOP, MID or BOTTOM
    three_d->lighting.horz = LIGHT_HORZ_MID;    // LEFT, MID or RIGHT
    three_d->lighting.depth = LIGHT_DEPTH_FRONT;// FRONT, MID or BACK
    three_d->lighting.brightness = 50;          // between 0 and 100
    three_d->lighting.contrast = 70;            // between 0 and 100
    three_d->black_edges = FALSE;
    three_d->resolution = SURFACE_RES_MED;      // LOW, MED or HIGH
}

void ENTRYPOINT three_d_init()
{
    tr_mem_open( hInstDLL, "THREE_D" );
    init_3d( &Three_d );
}

void ENTRYPOINT three_d_fini()
{
    tr_mem_close();
}

three_d_handle ENTRYPOINT three_d_begin(WPI_PRES win_dc, RECTL * draw_area )
{
    int                 save_state;
    WPI_POINT pt;

    rgn_off();
    Win_dc = win_dc;
    Is_metafile = FALSE;
    save_state = _wpi_savepres( win_dc );
    _wpi_setmapmode( win_dc, MM_TEXT );
    _winitwindow( draw_area );
    _wsetwindow( 0.0, 0.0, 1.0, 1.0 );
    _wpi_torgbmode( win_dc );
    _wpi_setbackmode( win_dc, OPAQUE );
    pt.x = 0;
    pt.y = 0;
    _wpi_setbrushorigin( win_dc, &pt );
    #define MY_ZOOM 1.4
    #define MY_LIGHT 2.0
    _w3init( &Three_d, MY_ZOOM, MY_LIGHT, LIGHT_VERT_MID, TRUE );
    return( save_state );
}

void ENTRYPOINT three_d_draw( three_d_handle handle )
{
    handle = handle;
    _w3display_all();
}

void ENTRYPOINT three_d_end( three_d_handle handle )
{
    _w3shutdown();
    _wpi_restorepres( Win_dc, handle );
}

void ENTRYPOINT three_d_box(
    three_d_handle handle,
    float       x1,
    float       x2,
    float       y1,
    float       y2,
    float       z1,
    float       z2,
    unsigned    red,
    unsigned    green,
    unsigned    blue )
{
    w3coord     pt1;
    w3coord     pt2;
    float       depth;

    handle = handle;
    pt1.xcoord = min( x1, x2 );
    pt1.ycoord = min( y1, y2 );
    pt1.zcoord = max( z1, z2 );
    pt2.xcoord = max( x1, x2 );
    pt2.ycoord = max( y1, y2 );
    pt2.zcoord = pt1.zcoord;
    depth = fabs( z1 - z2 );
    _w3setcolour( RGB(red,green,blue) );
    _w3bar( pt1, pt2, depth, FALSE, FALSE );
}

int ENTRYPOINT three_d_get( three_d_handle handle, three_d_attr attr )
{
    handle = handle;
    switch( attr ) {
    case THREE_D_HORZ_ANGLE:    return( Three_d.view.h_angle );
    case THREE_D_VERT_ANGLE:    return( Three_d.view.v_angle );
    case THREE_D_ZOOM:          return( Three_d.view.zoom );
    case THREE_D_PERSPECTIVE:   return( Three_d.view.perspective );
    case THREE_D_BRIGHTNESS:    return( Three_d.lighting.brightness );
    case THREE_D_CONTRAST:      return( Three_d.lighting.contrast );
    case THREE_D_LIGHT_HORZ:    return( Three_d.lighting.horz );
    case THREE_D_LIGHT_VERT:    return( Three_d.lighting.vert );
    case THREE_D_LIGHT_DEPTH:   return( Three_d.lighting.depth );
    }
    return( -1 );
}

void ENTRYPOINT three_d_set( three_d_handle handle, three_d_attr attr, int new_val )
{
    handle = handle;
    switch( attr ) {
    case THREE_D_HORZ_ANGLE:    Three_d.view.h_angle = new_val; break;
    case THREE_D_VERT_ANGLE:    Three_d.view.v_angle = new_val; break;
    case THREE_D_ZOOM:          Three_d.view.zoom = new_val; break;
    case THREE_D_PERSPECTIVE:   Three_d.view.perspective = new_val; break;
    case THREE_D_BRIGHTNESS:    Three_d.lighting.brightness = new_val; break;
    case THREE_D_CONTRAST:      Three_d.lighting.contrast = new_val; break;
    case THREE_D_LIGHT_HORZ:    Three_d.lighting.horz = new_val; break;
    case THREE_D_LIGHT_VERT:    Three_d.lighting.vert = new_val; break;
    case THREE_D_LIGHT_DEPTH:   Three_d.lighting.depth = new_val; break;
    }
}
