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


/*
 Description:
 ============
 These are the 3d drawing primitives used in other parts of cgr. The 3d
 pipeline should not be accessed by any part of cgr except this one.
*/

#include "icgr.h"

static hls_colour   Curr_colour = { 0., 0., 0. };
static w3coord      Curr_line_pt = { 0., 0., 0. };
static line_style   Curr_line_style = LINE_SOLID;
static bool         Black_edges = FALSE;
static region_info  Region = { USE_RGN_NONE };

#define _range_constrain( val, low, high ) _max( _min( (val), (high) ), (low) )

/* mathematical constant e */
#define E       2.7182818

/* constants for the simple to real perspective conversion */
#define SIMP_PERSP_MIN  0.
#define SIMP_PERSP_MAX  100.
#define REAL_PERSP_MIN  0.5
#define REAL_PERSP_MAX  40.

static void set_vpn_vup(
/**********************/
/* set the vecotors ViewPlNorm and VUp based on h_angle and v_angle */
    int         h_angle,
    int         v_angle,
    vector *    vpn,
    vector *    vup
) {
    float       sintheta, costheta;     /* h_angle values */
    float       sinphi, cosphi;         /* v_angle values */

    /* calculate the view plane normal */
    sintheta = sin( _deg_to_rad( h_angle ) );
    costheta = cos( _deg_to_rad( h_angle ) );
    cosphi   = cos( _deg_to_rad( v_angle ) );
    sinphi   = sin( _deg_to_rad( v_angle ) );
    vpn->v[0] = cosphi * sintheta;
    vpn->v[1] = sinphi;
    vpn->v[2] = cosphi * costheta;
    vpn->v[3] = 0;

    /* set the up vector to (0,1,0) if  -45 < v_angle < 45 otherwise */
    /* set it according to h_angle */
    if (v_angle >= 45) {
        vup->v[0] = - sintheta;
        vup->v[1] = 0.;
        vup->v[2] = - costheta;
        vup->v[3] = 0.;
    } else if (v_angle <= -45) {
        vup->v[0] = sintheta;
        vup->v[1] = 0.;
        vup->v[2] = costheta;
        vup->v[3] = 0.;
    } else {
        vup->v[0] = 0.;
        vup->v[1] = 1.;
        vup->v[2] = 0.;
        vup->v[3] = 0.;
    }
}

static float calculate_real_perspective(
/**************************************/
    int     simp_persp
) {
    float   x;

    /* set x to be simp_persp capped to SIMP_PERSP_MIN/MAX */
    if( simp_persp < SIMP_PERSP_MIN ) {
        x = SIMP_PERSP_MIN;
    } else if( simp_persp > SIMP_PERSP_MAX ) {
        x = SIMP_PERSP_MAX;
    } else {
        x = simp_persp;
    }

    /* scale x into [0,1] and reverse the direction */
    x = 1 - (x - SIMP_PERSP_MIN)/(SIMP_PERSP_MAX - SIMP_PERSP_MIN);

    /* apply exp to linearize the perspective, then scale into [0,1] */
    x = exp( x );
    x = (x - 1.) / (E - 1.);

    /* futher linearize the perspective */
    x = exp( x );
    x = (x - 1.) / (E - 1.);

    /* scale x into [REAL_PERSP_MIN, REAL_PERSP_MAX] */
    x = x * (REAL_PERSP_MAX - REAL_PERSP_MIN) + REAL_PERSP_MIN;

    return( x );
}

static void simple_to_pipe_view(
/******************************/
    view_3d *   simp,
    pipe_view * view,
    float       base_zoom
) {
    float       zoom;

    /* calculate the view plane normal and vup */
    set_vpn_vup( simp->h_angle, simp->v_angle, &view->ViewPlNorm, &view->VUp );

    /* calculate the view ref pt. */
    view->ViewRefPt.p[0] = 0.5 + view->ViewPlNorm.v[0]/2.;
    view->ViewRefPt.p[1] = 0.5 + view->ViewPlNorm.v[1]/2.;
    view->ViewRefPt.p[2] = 0.5 + view->ViewPlNorm.v[2]/2.;
    view->ViewRefPt.p[3] = 1.0;

    /* set the window */
    if( simp->zoom <= 0 ) {
        zoom = 100. / base_zoom;
    } else {
        zoom = 100. / (simp->zoom * base_zoom);
    }
    view->vmin = - zoom;
    view->vmax = zoom;
    view->umin = - zoom;
    view->umax = zoom;

    /* set the projection ref. pt. */
    view->ProjRefPt.p[0] = 0.;
    view->ProjRefPt.p[1] = 0.;
    view->ProjRefPt.p[2] = calculate_real_perspective( simp->perspective );
    view->ProjRefPt.p[3] = 1.;

    /* set the front and back clipping planes */
    view->FrontClip = 0.;           /* note FrontClip > BackClip */
    view->BackClip = -1.;

    view->type = simp->type;
}

static void set_light_dir(
/************************/
    vector *            dir,
    light_vert_pos      vert,
    light_horz_pos      horz,
    light_depth_pos     depth
) {
    float               len;
    int                 i;

    switch( horz ) {
    case LIGHT_HORZ_LEFT:
        dir->v[0] = -1.;
        break;
    case LIGHT_HORZ_MID:
        dir->v[0] = 0.;
        break;
    case LIGHT_HORZ_RIGHT:
        dir->v[0] = 1.;
        break;
    }
    switch( vert ) {
    case LIGHT_VERT_TOP:
        dir->v[1] = 1.;
        break;
    case LIGHT_VERT_MID:
        dir->v[1] = 0.;
        break;
    case LIGHT_VERT_BOTTOM:
        dir->v[1] = -1.;
        break;
    }
    switch( depth ) {
    case LIGHT_DEPTH_FRONT:
        dir->v[2] = 1.;
        break;
    case LIGHT_DEPTH_MID:
        dir->v[2] = 0.;
        break;
    case LIGHT_DEPTH_BACK:
        dir->v[2] = -1.;
        break;
    }

    dir->v[3] = 0;

    len = norm( *dir );
    /* test for the middle, middle middle case */
    if( len == 0. ) {
        dir->v[2] = 1.;         // make this middle, middle, front
    } else {
        /* otherwise normalize the vector */
        for( i = 0; i < 4; i++ ) {
            dir->v[i] /= len;
        }
    }
}

static void simple_to_pipe_illum(
/*******************************/
/*
NOTE: that since the lighting model is applied after the viewing
transformation has been performed the fact that h_angle and v_angle are
relitive to the viewing parameters is taken care of automagiclly.
NOTE: the centre of attention for the illumination model, relitive to which
the light source will be placed, is at (.5, .5, .5).
*/
    light_3d *      simp,
    pipe_illum *    pipe,
    float           distance,
    light_vert_pos  auto_pos
) {
    vector          dir;
    float           contrast;
    float           brightness;

    pipe->at_infinity = (distance == 0.);

    /* set the illumination point */
    if( simp->type == LIGHT_AUTO ) {
        set_light_dir( &dir, auto_pos, LIGHT_HORZ_LEFT,
                                LIGHT_DEPTH_FRONT );
    } else {
        set_light_dir( &dir, simp->vert, simp->horz, simp->depth );
    }
    if( pipe->at_infinity ) {
        pipe->source.light_dir = dir;
    } else {
        pipe->source.light_pt.p[0] = .5 + distance * dir.v[0];
        pipe->source.light_pt.p[1] = .5 + distance * dir.v[1];
        pipe->source.light_pt.p[2] = .5 + distance * dir.v[2];
        pipe->source.light_pt.p[3] = 1.;
    }

    contrast = _range_constrain( simp->contrast, 0, 100 ) / 100.;
    brightness = _range_constrain( simp->brightness, 0, 100 ) / 50.;

    pipe->amb_intensity = (1. - contrast) * brightness;
    pipe->pt_intensity = contrast * brightness;
}

extern void _w3init(
/******************/
    three_d_op *    view,
    float           base_zoom,
    float           light_distance,         // if 0 light is at infinity
    light_vert_pos  auto_pos,
    bool            all_poly_convex
) {
    pipe_view *     pview;
    pipe_illum *    pillum;

    _new( pview, 1 );
    _new( pillum, 1 );
    simple_to_pipe_view( &view->view, pview, base_zoom );
    simple_to_pipe_illum( &view->lighting, pillum, light_distance, auto_pos );
    pipe3d_init( pview, pillum, all_poly_convex );
    _free( pillum );
    _free( pview );
}

extern void _w3shutdown(
/**********************/
    void
) {
    pipe3d_shutdown();
}

extern void _w3display_all(
/*************************/
    void
) {
    pipe3d_display();
}

extern void _w3get_view_dir(
/**************************/
/* Gets the viewing direction based on the angles given in view */
/* NOTE: The assumtion made in this routine are valid for the restricted */
/* viewing system employed in cgr but are not valid for the complete system */
/* as described in "Computer Graphics" by Foley et. al. */
    view_3d *   view,
    w3coord *   dir
) {
    vector      vpn;
    vector      dummy;
    float       len;

    set_vpn_vup( view->h_angle, view->v_angle, &vpn, &dummy );

    len = norm( vpn );
    dir->xcoord = - vpn.v[0] / len;
    dir->ycoord = - vpn.v[1] / len;
    dir->zcoord = - vpn.v[2] / len;
}

extern void _w3setcolour(
/***********************/
    COLORREF    rgb
) {
    rgb_to_hls( rgb, &Curr_colour );
}

extern void _w3setlinestyle(
/**************************/
    line_style  style
) {
    Curr_line_style = style;
}

extern void _w3setblackedges(
/***************************/
    bool    black_edges
) {
    Black_edges = black_edges;
}

static void rect_to_polar(
/************************/
    float       x,
    float       y,
    float       *r,
    float       *omega
) {
    *r = sqrt( x*x + y*y );
    if (*r == 0.) {
        *omega = 0.;        // this value is arbitrary
    } else {
        *omega = atan2( y, x );
    }
}

static void polar_to_rect(
/************************/
    float       r,
    float       omega,
    float       *x,
    float       *y
) {
    *x = r * cos( omega );
    *y = r * sin( omega );
}

extern void ellipse_pt(
/*********************/
/* Calculated the point on the ellipse at angle alpha */
    float       h_sqr,              // horizontal axis squared
    float       v_sqr,              // vertical axis squared
    float       cx,                 // centre
    float       cy,
    float       alpha,
    float *     x,
    float *     y
) {
    float       calpha;
    float       salpha;
    float       r;

    calpha = cos( alpha );
    salpha = sin( alpha );

    r = sqrt( 1./(calpha*calpha/h_sqr + salpha*salpha/v_sqr));
    polar_to_rect( r, alpha, x, y );
    *x += cx;
    *y += cy;
}

static pt_edge * pie_to_poly(
/***************************/
/* returns a polynomial approximation to the pie wedge given */
/* The points in the polygon approximation are layed out in counter clockwise */
/* order as seen from below so that the sweep by height will put the solid in */
/* the 0-1 cube */
    float       x1,
    float       z1,
    float       x2,
    float       z2,
    float       x3,
    float       z3,
    float       x4,
    float       z4,
    float       y,
    int         resolution,
    int *       num_pts,
    text_info * text,
    bool        all_pie
) {
    pt_edge     *pts;           // polygon that will approximate the pie
    int         num_regions;    // number of regions to devide the pie into
    float       cx, cz;         // centre of ellipse
    float       h_sqr, w_sqr;   // square of the half height & width
    float       alpha;          // angle of current point from x-axis
    float       omega;          // angle between start and end of pie
    float       beta;           // angle of mid point of pie
    float       dummy;
    float       x,z;
    int         curr_region;

    cx = (x1 + x2) / 2;
    cz = (z1 + z2) / 2;

    h_sqr = (z2 - cz) * (z2 - cz);
    w_sqr = (x2 - cx) * (x2 - cx);

    /* set omega to angle of end then subtract angle of start and normalize */
    if( all_pie ) {
        alpha = 0;
        omega = 2 * PI;
    } else {
        rect_to_polar( x4 - cx, z4 - cz, &dummy, &omega );
        rect_to_polar( x3 - cx, z3 - cz, &dummy, &alpha );
    }
    omega -= alpha;
    if (omega < 0.) {
        omega += 2 * PI;
    }
    beta = alpha + omega/2.;

    num_regions = resolution * omega / (2 * PI) + .5;
    if (num_regions == 0) {
        num_regions = 1;
    }

    /* the polygon will have num_regions + 2 pts. 1 for the centre and 1 */
    /* for the last point on the outside */
    *num_pts = num_regions + 2;
    _new( pts, *num_pts );

    for (curr_region = 0; curr_region < num_regions + 1; curr_region++) {
        ellipse_pt( w_sqr, h_sqr, cx, cz, alpha, &x, &z );
        pts[ num_regions - curr_region ].pt.p[0] = x;
        pts[ num_regions - curr_region ].pt.p[1] = y;
        pts[ num_regions - curr_region ].pt.p[2] = z;
        pts[ num_regions - curr_region ].pt.p[3] = 1.;
        /* set the edge and black face indicators to false */
        pts[ num_regions - curr_region ].edge = FALSE;
        pts[ num_regions - curr_region ].black_face = FALSE;
        pts[ num_regions - curr_region ].face_rgn_iter = 0; //no markers on face
        pts[ num_regions - curr_region ].force_rgn_pt = FALSE;

        alpha += omega / num_regions;
    }

    /* set the centre into the points array */
    pts[ num_regions + 1 ].pt.p[0] = cx;
    pts[ num_regions + 1 ].pt.p[1] = y;
    pts[ num_regions + 1 ].pt.p[2] = cz;
    pts[ num_regions + 1 ].pt.p[3] = 1.;

    /* set the edge indicator true for the first, last and centre edges */
    pts[ 0 ].edge = TRUE;
    pts[ 0 ].black_face = FALSE;
    pts[ 0 ].face_rgn_iter = 0;     // no markers on the swepted face
    pts[ 0 ].force_rgn_pt = TRUE;
    pts[ num_regions ].edge = TRUE;
    pts[ num_regions ].black_face = FALSE;
    pts[ num_regions ].face_rgn_iter = 0;   // no markers on the swepted face
    pts[ num_regions ].force_rgn_pt = TRUE;
    pts[ num_regions + 1 ].edge = TRUE;
    pts[ num_regions + 1 ].black_face = FALSE;
    pts[ num_regions + 1 ].face_rgn_iter = 0;   // no markers on the swepted face
    pts[ num_regions + 1 ].force_rgn_pt = TRUE;

    /* set the text points, if requested */
    if( text != NULL ) {
        text->centre.p[0] = cx;
        text->centre.p[1] = y;
        text->centre.p[2] = cz;
        text->centre.p[3] = 1.;
        ellipse_pt( w_sqr, h_sqr, cx, cz, beta, &x, &z );
        text->pt1.p[0] = x;
        text->pt1.p[1] = y;
        text->pt1.p[2] = z;
        text->pt1.p[3] = 1.;
    }

    return( pts );
}

extern void _w3pie(
/*****************/
/* This function adds a pie wedge to the 3d pipeline. The wedge will be in */
/* the 0-1 cube and will range from y=.5 - height/2 to y=.5 + height/2 */
/* NOTE: The bottom of the pie is layed out in new_obj.pts but */
/* rend_obj_add_sweep calls its input points the "top". */
    float           x1,
    float           z1,
    float           x2,
    float           z2,
    float           x3,
    float           z3,
    float           x4,
    float           z4,
    float           height,
    text_pt *       text,
    bool            above_view,
    int             resolution,
    bool            all_pie
) {
    rend_obj_add    new_obj;
    float           y_bottom;
    text_info       calc_text;      // info for calculation text position

    y_bottom = .5 - height / 2.;    // centre of attention is at (.5,.5,.5)
    new_obj.pts = pie_to_poly( x1, z1, x2, z2, x3, z3, x4, z4,
                y_bottom, resolution, &new_obj.num_pts, &calc_text, all_pie );
    if (new_obj.num_pts == 0) {
        if( text != NULL ) {
            /* set text to an arbitrary quantity */
            text->pt.xcoord = 0.;
            text->pt.ycoord = 0.;
            text->direction.xcoord = 1.;
            text->direction.ycoord = 1.;
        }
        return;
    }

    new_obj.top_edges = TRUE;
    new_obj.top_rgn_iter = CGR_3D_PIE_RGN_ITER;
    new_obj.bottom_edges = TRUE;
    new_obj.bottom_rgn_iter = CGR_3D_PIE_RGN_ITER;
    new_obj.height = height;
    if( text != NULL ) {
        calc_text.text = text;
        new_obj.text = &calc_text;
    } else {
        new_obj.text = NULL;
    }
    new_obj.text_on_bottom = above_view;    // bottom of solid which is top
                                            // of pie.
    new_obj.base = Curr_colour;
    new_obj.black_edges = Black_edges;
    new_obj.rgn = Region;

    rend_obj_add_sweep( &new_obj );

    _free( new_obj.pts );
}

static pt_edge * rect_to_poly(
/****************************/
/* Note: The points are layed out in counter clockwise order as seen from */
/* the positive z axis */
    w3coord     pt1,
    w3coord     pt2,
    bool        black1,         // plane parallel to x-z at pt1.y black?
    bool        black2
) {
    pt_edge *   pts;

    _new( pts, 4 );

    pts[0].pt.p[0] = pt1.xcoord;
    pts[0].pt.p[1] = pt1.ycoord;
    pts[0].pt.p[2] = pt1.zcoord;
    pts[0].pt.p[3] = 1.;
    pts[0].edge = TRUE;
    pts[0].black_face = FALSE;
    pts[0].face_rgn_iter = 1;
    pts[0].force_rgn_pt = FALSE;
    pts[1].pt.p[0] = pt1.xcoord;
    pts[1].pt.p[1] = pt2.ycoord;
    pts[1].pt.p[2] = pt2.zcoord;
    pts[1].pt.p[3] = 1.;
    pts[1].edge = TRUE;
    pts[1].black_face = black2;
    pts[1].face_rgn_iter = 1;
    pts[1].force_rgn_pt = FALSE;
    pts[2].pt.p[0] = pt2.xcoord;
    pts[2].pt.p[1] = pt2.ycoord;
    pts[2].pt.p[2] = pt2.zcoord;
    pts[2].pt.p[3] = 1.;
    pts[2].edge = TRUE;
    pts[2].black_face = FALSE;
    pts[2].face_rgn_iter = 1;
    pts[2].force_rgn_pt = FALSE;
    pts[3].pt.p[0] = pt2.xcoord;
    pts[3].pt.p[1] = pt1.ycoord;
    pts[3].pt.p[2] = pt1.zcoord;
    pts[3].pt.p[3] = 1.;
    pts[3].edge = TRUE;
    pts[3].black_face = black1;
    pts[3].face_rgn_iter = 1;
    pts[3].force_rgn_pt = FALSE;

    return( pts );
}

static void add_zero_bar(
/***********************/
    w3coord     pt1,
    w3coord     pt2,
    float       depth
) {
    point *     pts;
    rend_obj *  new_obj;

    _new( pts, 4 );

    pts[0].p[0] = pt1.xcoord;
    pts[0].p[1] = pt1.ycoord;
    pts[0].p[2] = pt1.zcoord;
    pts[0].p[3] = 1.;
    pts[1].p[0] = pt2.xcoord;
    pts[1].p[1] = pt1.ycoord;
    pts[1].p[2] = pt1.zcoord;
    pts[1].p[3] = 1.;
    pts[2].p[0] = pt2.xcoord;
    pts[2].p[1] = pt1.ycoord;
    pts[2].p[2] = pt1.zcoord + depth;
    pts[2].p[3] = 1.;
    pts[3].p[0] = pt1.xcoord;
    pts[3].p[1] = pt1.ycoord;
    pts[3].p[2] = pt1.zcoord + depth;
    pts[3].p[3] = 1.;

    new_obj = rend_obj_create_poly( 4, pts, Curr_colour, Black_edges,
                                    &Region, NULL, 1 );

    pipe3d_add( new_obj, NULL, FALSE );
}

extern void _w3bar(
/*****************/
/* Adds a bar to the 3d pipeline whose front face is the rect with corners */
/* pt1 and pt2, and which has top and bottom edges parallel to the x axis */
    w3coord     pt1,
    w3coord     pt2,
    float       depth,
    bool        black1,         // plane parallel to x-z at pt1.y black?
    bool        black2
) {
    rend_obj_add    new_obj;

    if( fabs( pt1.ycoord - pt2.ycoord ) < FUZZY_ZERO ) {
        if( fabs( pt1.xcoord - pt2.xcoord ) < FUZZY_ZERO &&
                fabs( pt1.zcoord - pt1.zcoord ) < FUZZY_ZERO ) {
            return;
        } else {
            add_zero_bar( pt1, pt2, depth );
            return;
        }
    }

    new_obj.pts = rect_to_poly( pt1, pt2, black1, black2 );
    new_obj.num_pts = 4;
    new_obj.top_edges = TRUE;
    new_obj.top_rgn_iter = 1;
    new_obj.bottom_edges = TRUE;
    new_obj.bottom_rgn_iter = 1;
    new_obj.height = depth;
    new_obj.text = NULL;
    new_obj.base = Curr_colour;
    new_obj.black_edges = Black_edges;
    new_obj.rgn = Region;

    rend_obj_add_sweep( &new_obj );

    _free( new_obj.pts );
}

extern void _w3moveto(
/********************/
    w3coord     pt
) {
    Curr_line_pt = pt;
}

extern void _w3lineto(
/********************/
    w3coord     pt,
    bool        disp_now
) {
    point       start;
    point       end;
    rend_obj *  new_obj;

    start.p[0] = Curr_line_pt.xcoord;
    start.p[1] = Curr_line_pt.ycoord;
    start.p[2] = Curr_line_pt.zcoord;
    start.p[3] = 1.;
    end.p[0] = pt.xcoord;
    end.p[1] = pt.ycoord;
    end.p[2] = pt.zcoord;
    end.p[3] = 1.;

    new_obj = rend_obj_create_line( start, end, Curr_colour,
                    Curr_line_style, &Region );
    pipe3d_add( new_obj, NULL, disp_now );

    Curr_line_pt = pt;
}

extern void _w3transformpoint(
/****************************/
    const w3coord *     pt_3d,
    wcoord *            pt_2d
) {
    point               homo_pt;

    homo_pt.p[0] = pt_3d->xcoord;
    homo_pt.p[1] = pt_3d->ycoord;
    homo_pt.p[2] = pt_3d->zcoord;
    homo_pt.p[3] = 1.;

    pipe3d_trans_pt( &homo_pt );
    project_pt( homo_pt, &(pt_2d->xcoord), &(pt_2d->ycoord) );
}

extern void _w3rgnon(
/*******************/
    int         major_id,
    int         minor_id,
    int         data_row,
    int         data_col,
    int         set_num,
    bool        use_group
) {
    if( use_group ) {
        Region.use_info = USE_RGN_GROUP;
    } else {
        Region.use_info = USE_RGN_SET;
    }
    Region.major_id = major_id;
    Region.minor_id = minor_id;
    Region.data_row = data_row;
    Region.data_col = data_col;
    Region.set_num = set_num;
}

extern void _w3rgnoff(
/********************/
    void
) {
    Region.use_info = USE_RGN_NONE;
}

extern void _w3rgnsetadd(
/***********************/
/* Adds the projection of a point to the current manual set, if there is one */
    w3coord *   pt_3
) {
    wcoord      pt_2;

    if( Region.use_info == USE_RGN_SET ) {
        _w3transformpoint( pt_3, &pt_2 );
        rgn_man_set_begin( Region.set_num );
        wrgn_set_add( &pt_2 );
        rgn_man_set_end();
    }
}

extern void _w3polygon(
/*********************/
    int         num_pts,
    w3coord *   pts,
    bool *      edges
) {
    point *     h_pts;      // homogenous points
    int         pt_num;
    rend_obj *  new_obj;

    _new( h_pts, num_pts );
    if( h_pts != NULL ) {
        for( pt_num = 0; pt_num < num_pts; pt_num++ ) {
            h_pts[ pt_num ].p[0] = pts[ pt_num ].xcoord;
            h_pts[ pt_num ].p[1] = pts[ pt_num ].ycoord;
            h_pts[ pt_num ].p[2] = pts[ pt_num ].zcoord;
            h_pts[ pt_num ].p[3] = 1.;
        }
        new_obj = rend_obj_create_poly( num_pts, h_pts, Curr_colour,
                                Black_edges, &Region, edges, 0 );
        pipe3d_add( new_obj, NULL, FALSE );
    }
}

extern void _w3area(
/*******************/
/*
The points are assumed to be in counter clockwise order as seen from
the outside of the solid.
NOTE: This routine requires some conditions on the points:
        1. The points are not co-linear.
        2. The points are co-plannar (within numerical limits).
        3. The points define a simple polygon (doesn't cross itself).
*/
    int                 num_pts,
    const w3coord *     pts,
    float               height,
    int                 black_face      // -1 for no black face
) {
    rend_obj_add        add;
    int                 curr_pt;

    _new( add.pts, num_pts );
    for( curr_pt = 0; curr_pt < num_pts; curr_pt++ ) {
        add.pts[ curr_pt ].pt.p[0] = pts[ curr_pt ].xcoord;
        add.pts[ curr_pt ].pt.p[1] = pts[ curr_pt ].ycoord;
        add.pts[ curr_pt ].pt.p[2] = pts[ curr_pt ].zcoord;
        add.pts[ curr_pt ].pt.p[3] = 1.;
        add.pts[ curr_pt ].edge = TRUE;
        add.pts[ curr_pt ].black_face = (curr_pt == black_face);
        add.pts[ curr_pt ].face_rgn_iter = 0;
        add.pts[ curr_pt ].force_rgn_pt = FALSE;
    }
    add.num_pts = num_pts;
    add.text = NULL;
    add.height = height;
    add.top_edges = TRUE;
    add.top_rgn_iter = 1;
    add.bottom_edges = TRUE;
    add.bottom_rgn_iter = 1;
    add.base = Curr_colour;
    add.black_edges = Black_edges;
    add.rgn = Region;

    rend_obj_add_sweep( &add );

    _free( add.pts );
}
