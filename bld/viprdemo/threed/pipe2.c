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
 This module contains the second half of the 3d pipeline. It has the following
 components:
        1. Visible surface determination (excluding back face culling).
        2. Illumination.
        3. Display (using 2d world functions).

 NOTE: The second half of the pipline does not process solids. These must be
    broken up into polygons before being passed on. The rend_obj_add_to_list
    fuction takes care of this (i.e. call it at the end of the first half of
    the pipeline).
 NOTE: All references to pages or sections in this file refer to "Computer
    Graphics" by Foley et. al. unless otherwise noted.


 Visible Surface Determination
 -----------------------------
 The algorithm used is the depth-sort algorithm. See "Computer Graphics" by
 Foley, et. al. section 15.5.1. The routine is passed rend_list and it figures
 out which the order to display the elements of the list in and does so.

 Lighting
 --------
 The lighting model is Lambertian reflection (diffuse) with ambiant light and
 attentuation. See "Computer Graphics" by Foley, et. al. section 16.1 pp. 722
 to 726.

*/

#include "icgr.h"


/* 1 if x is positive, -1 if x is negitive, 0 if x = 0 */
/* if x is within fuzz of 0 then it is considered to be 0 */
#define _fuzzy_sign( x, fuzz ) ( (fabs(x) < fuzz) ? 0 : (((x) < 0) ? -1 : 1) )

#define _is_zero_norm( n ) ( fabs((n).v[0]) < FUZZY_ZERO && fabs((n).v[1]) \
                            < FUZZY_ZERO && fabs((n).v[2]) < FUZZY_ZERO )

/* unit vector along coordinate axies */
static const vector Xvect = { 1.0, 0.0, 0.0, 0.0 };
static const vector Yvect = { 0.0, 1.0, 0.0, 0.0 };
static const vector Zvect = { 0.0, 0.0, 1.0, 0.0 };

/* current illumination parameters */
static pipe_illum Illum_param;

static bool All_poly_convex = TRUE;

/* 2D display */
extern void project_pt(
/*********************/
/*to project a point take its x and y coords and map them from [-1,1] to [0,1]*/
    point   pt,             // 3d point
    float   *x,             // 2d point
    float   *y
) {
    *x = (pt.p[0] + 1.) / 2.;
    *y = (pt.p[1] + 1.) / 2.;
}

static void draw_projected_poly(
/******************************/
    rend_obj *  obj,
    hls_colour  edge_colour
) {
    wcoord *    pts_2d;         // array of 2d points
    int         num_pts;
    int         curr_pt;
    point       pt_3d;
    COLORREF    edge_rgb;
    polygon *   poly;

    poly = rend_obj_get_poly( obj );

    /* Draw the illuminated surface of the polygon */
    num_pts = _polygon_get_num_pts( *poly );
    _new( pts_2d, num_pts );

    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        pt_3d = _polygon_get_pt( *poly, curr_pt );
        project_pt( pt_3d,  &(pts_2d[ curr_pt ].xcoord),
                            &(pts_2d[ curr_pt ].ycoord) );
    }

    _wpolygon( FILL_INTERIOR, num_pts, pts_2d );


    /* Draw the hilit edges of the polygon and add rgn markers */
    hls_to_rgb( &edge_rgb, edge_colour );
    _wsetrgbcolor( edge_rgb );
    _wsetlinestyle( LINE_SOLID );
    _wsetlinewidth( WIDTH_SINGLE );

    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        if( _polygon_edge_hilight( *poly, curr_pt )) {
            _wmoveto( pts_2d[ curr_pt ].xcoord, pts_2d[ curr_pt ].ycoord );
            _wlineto( pts_2d[ (curr_pt+1) % num_pts ].xcoord,
                        pts_2d[ (curr_pt+1) % num_pts ].ycoord, TRUE );
        }
        if( _polygon_rgn_pt( *poly, curr_pt ) ) {
            wrgn_set_add( &pts_2d[ curr_pt ] );
        }
    }

    _free( pts_2d );
}

static void draw_projected_obj(
/*****************************/
    rend_obj *  obj,
    hls_colour  base,               // colour on which illum is based
    hls_colour  illum
) {
    point       pt1;
    point       pt2;
    float       x1,y1;
    float       x2,y2;
    COLORREF    rgb;

    hls_to_rgb( &rgb, illum );

    _wsetrgbcolor( rgb );
    _wsetfillstyle( FILL_SOLID );

    rend_obj_region_begin( obj );

    switch (_rend_obj_get_type( obj )) {
    case REND_OBJ_PT:
        pt1 = rend_obj_get_pt( obj, 0 );
        project_pt( pt1, &x1, &y1 );
        _wdot( x1, y1 );
        break;
    case REND_OBJ_LINE:
        pt1 = rend_obj_get_pt( obj, 0 );
        pt2 = rend_obj_get_pt( obj, 1 );
        project_pt( pt1, &x1, &y1 );
        project_pt( pt2, &x2, &y2 );
        _wsetlinestyle( _line_get_style( rend_obj_get_line( obj ) ) );
        _wsetlinewidth( WIDTH_SINGLE );
        _wmoveto( x1, y1 );
        _wlineto( x2, y2, FALSE );
        break;
    case REND_OBJ_POLY:
        draw_projected_poly( obj, base );
        break;
    }

    rend_obj_region_end( obj );
}


/* Lighting model */
static point calculate_poly_centre(
/********************************/
/* The centre is the averaged in each coefficient of the points of the poly */
    rend_obj    *obj
) {
    point       centre;
    point       curr;
    int         num_pts;
    int         curr_pt;
    int         i;

    for (i = 0; i < 4; i++) {
        centre.p[i] = 0.;
    }

    num_pts = rend_obj_get_num_pts( obj );
    if (num_pts == 0) {
        return( centre );
    }

    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        curr = rend_obj_get_pt( obj, curr_pt );
        for (i = 0; i < 4; i++) {
            centre.p[i] += curr.p[i];
        }
    }

    for (i = 0; i < 4; i++) {
        centre.p[i] /= num_pts;
    }

    return( centre );
}

static hls_colour illuminate_poly(
/********************************/
    rend_obj    *obj,
    hls_colour  base
) {
    point       centre;
    vector      light_dir;          // from centre of poly to point light source
    float       len;                // norm of light_dir
    vector      normal;
    float       dot;                // dot product of normal and light_dir
    hls_colour  final_col;

    centre = calculate_poly_centre( obj );
    if( Illum_param.at_infinity ) {
        light_dir = Illum_param.source.light_dir;
    } else {
        light_dir = point_diff( centre, Illum_param.source.light_pt );
    }
    len = norm( light_dir );

    /* calculate the dot product */
    normal = rend_obj_get_norm( obj );
    dot = _dot_prod_vv( normal, light_dir );
    /* if the dot product is negative the point light source has no effect */
    /* since surfaces are self-occluding */
    if (dot < 0.) {
        dot = 0.;
    }

    final_col.h = base.h;
    final_col.s = base.s;

    final_col.l = Illum_param.amb_intensity * base.l +
                    Illum_param.pt_intensity * base.l * dot / len;

    /* l must in range [0,1] */
    if (final_col.l < 0.) {
        final_col.l = 0.;
    } else if (final_col.l > 1.) {
        final_col.l = 1.;
    }


    return( final_col );
}

extern void draw_illuminated_obj(
/*******************************/
    rend_obj    *obj
) {
    hls_colour  edge;
    hls_colour  illum;

    illum = rend_obj_get_base_colour( obj );

    if( _rend_obj_black_edges( obj ) ) {
        edge = Black_hls;
    } else {
        edge = illum;
    }

    if (_rend_obj_get_type( obj ) == REND_OBJ_POLY) {
        illum = illuminate_poly( obj, illum );
    }

    draw_projected_obj( obj, edge, illum );
}

extern void set_poly_convex_info(
/*******************************/
    bool        all_poly_convex
) {
    All_poly_convex = all_poly_convex;
}

extern void set_illumination(
/***************************/
    pipe_illum  *illum
) {
    Illum_param = *illum;
}

/* Visible surface determination */
#if 0
static void compute_extents(
/**************************/
    rend_list *     list
) {
    rend_list_ref   ref;
    rend_obj *      obj;

    _rend_list_first( list, &ref );
    while (! _rend_list_is_after_last( list, ref )) {
        obj = _rend_list_get_obj( list, ref );
        rend_obj_compute_vis_info( obj );

        _rend_list_next( list, &ref );
    }
}
#endif

static int compare_zmin(
/**********************/
    const rend_obj **     obj1,
    const rend_obj **     obj2
) {
    visible_info *  vis1;
    visible_info *  vis2;
    float           zmin1;
    float           zmin2;

    vis1 = _rend_obj_get_vis( (rend_obj *)(*obj1) );
    vis2 = _rend_obj_get_vis( (rend_obj *)(*obj2) );
    zmin1 = vis1->min.p[2];
    zmin2 = vis2->min.p[2];

    if (zmin1 < zmin2) {
        return( -1 );       // 1 here so the sort is in increasing order
    } else if (zmin1 == zmin2) {
        return( 0 );
    } else {
        return( 1 );        // -1 here so the sort is in increasing order
    }
}

#if 0
static bool extent_overlap(
/*************************/
    visible_info *  vis1,
    visible_info *  vis2,
    int             component
) {
    float           min1;
    float           min2;
    float           max1;
    float           max2;

    min1 = vis1->min.p[component];
    min2 = vis2->min.p[component];
    max1 = vis1->max.p[component];
    max2 = vis2->max.p[component];

    return( min1 < max2 && min2 < max1 );
}
#endif

#define _extent_overlap( vis1, vis2, i ) \
            ((vis1)->min.p[i] < (vis2)->max.p[i] && \
             (vis2)->min.p[i] < (vis1)->max.p[i] )

static vector calc_pt_norm(
/*************************/
    rend_obj *  obj,            // obj is a point object
    rend_obj *  other
) {
    vector      norm;
    vector      a;
    point       pt;
    point       start;
    point       end;

    switch( _rend_obj_get_type( other ) ) {
    case REND_OBJ_PT:
        norm = Zvect;
        break;
    case REND_OBJ_LINE:
        start = rend_obj_get_pt( other, 0 );
        end = rend_obj_get_pt( other, 1 );
        a = point_diff( start, end );
        norm = cross_prod( a, Yvect );
        if( _is_zero_norm( norm ) ) {
            norm = cross_prod( a, Xvect );
        }
        break;
    case REND_OBJ_POLY:
        norm = rend_obj_get_norm( other );
        break;
    }

    pt = rend_obj_get_pt( obj, 0 );
    norm.v[3] = 0 - (norm.v[0]*pt.p[0] + norm.v[1]*pt.p[1] +
                    norm.v[2]*pt.p[2]);

    return( norm );
}

static vector calc_line_norm(
/***************************/
    rend_obj *  obj,            // a line object
    rend_obj *  other
) {
    vector      norm;
    vector      line_vect;
    vector      other_vect;
    point       start;
    point       end;
    point       pt1;
    polygon *   poly;
    int         pt_num;

    pt1 = rend_obj_get_pt( obj, 0 );
    end = rend_obj_get_pt( obj, 1 );
    line_vect = point_diff( pt1, end );

    switch( _rend_obj_get_type( other ) ) {
    case REND_OBJ_PT:
        norm = cross_prod( line_vect, Yvect );
        if( _is_zero_norm( norm ) ) {
            norm = cross_prod( line_vect, Xvect );
        }
        break;
    case REND_OBJ_LINE:
        start = rend_obj_get_pt( other, 0 );
        end = rend_obj_get_pt( other, 1 );
        other_vect = point_diff( start, end );
        norm = cross_prod( line_vect, other_vect );
        if( _is_zero_norm( norm ) ) {
            norm = cross_prod( line_vect, Yvect );
            if( _is_zero_norm( norm ) ) {
                norm = cross_prod( line_vect, Xvect );
            }
        }
        break;
    case REND_OBJ_POLY:
        poly = rend_obj_get_poly( other );
        start = _polygon_get_pt( *poly, _polygon_get_num_pts( *poly ) - 1 );
        for( pt_num = 0; pt_num < _polygon_get_num_pts( *poly ); pt_num++ ) {
            end = _polygon_get_pt( *poly, pt_num );
            other_vect = point_diff( start, end );
            norm = cross_prod( line_vect, other_vect );
            if( !_is_zero_norm( norm ) ) {
        /**/    break;
            }
            start = end;
        }
        if( _is_zero_norm( norm ) ) {
            norm = cross_prod( line_vect, Yvect );
            if( _is_zero_norm( norm ) ) {
                norm = cross_prod( line_vect, Xvect );
            }
        }
        break;
    }

    norm.v[3] = 0 - (norm.v[0]*pt1.p[0] + norm.v[1]*pt1.p[1] +
                    norm.v[2]*pt1.p[2]);

    return( norm );
}

static vector calculate_object_norm(
/**********************************/
/* Calculates the normal for the plane of a given object. See */
/* check_half_space for how this is determined */
    rend_obj *  obj,
    rend_obj *  other
) {
    vector      norm;

    switch (_rend_obj_get_type( obj )) {
    case REND_OBJ_PT:
        norm = calc_pt_norm( obj, other );
        break;
    case REND_OBJ_LINE:
        norm = calc_line_norm( obj, other );
        break;
    case REND_OBJ_POLY:
        norm = rend_obj_get_norm( obj );
        break;
    }

    return( norm );
}

enum {
    SUB_PLANE_UNKNOWN,
    SUB_PLANE_NEG,
    SUB_PLANE_ZERO,
    SUB_PLANE_POS,
    SUB_PLANE_NONE                  // for objects that cross the plane
};
typedef unsigned sub_plane_rc;

/* sign should be the result of a _fuzzy_sign macro */
#define _get_sub_plane_rc( sign ) (((sign) == 0) ? SUB_PLANE_ZERO : \
                    (((sign) == -1) ? SUB_PLANE_NEG : SUB_PLANE_POS ))

static int sub_pt_in_plane(
/*************************/
/* subsitute pt into the plane defined by norm and return the sign of the */
/* result */
    point       pt,
    vector      norm
) {
    float       result;

    result = _dot_prod_vp( norm, pt );
    return( _fuzzy_sign( result, FUZZY_ZERO ) );
}

static sub_plane_rc sub_line_in_plane(
/************************************/
    rend_obj    *obj,
    vector      norm
) {
    int         start_sign;
    int         end_sign;

    start_sign = sub_pt_in_plane( rend_obj_get_pt( obj, 0 ), norm );
    end_sign = sub_pt_in_plane( rend_obj_get_pt( obj, 1 ), norm );

    if (start_sign == end_sign || end_sign == 0) {
        return( _get_sub_plane_rc( start_sign ) );
    } else if (start_sign == 0) {
        return( _get_sub_plane_rc( end_sign ) );
    } else {
        return( SUB_PLANE_NONE );
    }
}

static sub_plane_rc sub_poly_in_plane(
/************************************/
    rend_obj    *obj,
    vector      norm
) {
    int         old_sign;
    int         sign;
    int         curr_pt;
    int         num_pts;
    polygon     *poly;

    poly = rend_obj_get_poly( obj );
    num_pts = _polygon_get_num_pts( *poly );
    old_sign = 0;

    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        sign = sub_pt_in_plane( _polygon_get_pt( *poly, curr_pt ), norm );
        if (sign != 0) {
            if (old_sign == 0) {
                /* this is the first non-zero sign */
                old_sign = sign;
            } else if (sign != old_sign) {
                /* this sign does not agree with the first non-zero sign */
                return( SUB_PLANE_NONE );
            }
        }
    }

    return( _get_sub_plane_rc( old_sign ) );
}

static sub_plane_rc sub_obj_in_plane(
/***********************************/
/*
Subsitute the points of obj into the plane defined by norm and determine the
resulting sign. If the sign of all pts is 0 (they all lie in the plane) return
0. If the sign of all the pts is the same (and non-zero) return it. Otherwise
return 0. Since norm contains the equation of a plane dot_prod_vp of norm
and a pt substitutes that pt into the equation.
*/
    rend_obj *  obj,
    vector      norm
) {
    int         sign;

    switch (_rend_obj_get_type( obj )) {
    case REND_OBJ_PT:
        sign = sub_pt_in_plane( rend_obj_get_pt( obj, 0 ), norm );
        return( _get_sub_plane_rc( sign ) );
        break;
    case REND_OBJ_LINE:
        return( sub_line_in_plane( obj, norm ) );
        break;
    case REND_OBJ_POLY:
        return( sub_poly_in_plane( obj, norm ) );
        break;
    default:
        /* This case should not happen */
        return( SUB_PLANE_UNKNOWN );
    }
}

enum {
    HALF_SPACE_NEAR,
    HALF_SPACE_FAR,
    HALF_SPACE_SAME,            // the objects lie in the same plane
    HALF_SPACE_NONE
};
typedef unsigned check_half_space_rc;

static check_half_space_rc check_half_space(
/******************************************/
    rend_obj *  obj1,
    rend_obj *  obj2
/*
This procedure checks if obj1 is completly in one of the half spaces of
the plane of obj2. The plane of obj2 could be any plane that contains obj2
and is defined differently for different types of objects:
    point:      plane is parallel to x-y plane (i.e. norm is (0,0,1))
    line:       let  a  be vector from start to end of line
                then n (the normal vector) is give by  a x (0,1,0) or
                a x (1,0,0) if the first one = 0  (x is the vector
                cross product).
    polygon:    norm is given
The plane normal is used in homogeneous coordinates as described at the top of
pipe.h, so the D parameter is calculated after A B and C are known.

Once the plane of obj2 is known each point associated with obj1 is substituted
into the plane equation and the resulting sign is noted. If all the signs agree
then the object is completely in one or the other of the half spaces. If the
common sign agrees with the sign of the z-component of the norm then obj1 is
in the near half space; if the signs are opposite then obj1 is in the far
half space.

NOTE: This implimentation was suggested by exercise 15.6 on p. 718.
*/
) {
    vector          norm;
    sub_plane_rc    rel1;       /* relationship of obj1 to plane of obj2 */
    int             sign2_z;    /* sign of z-component of norm of obj2 */

    norm = calculate_object_norm( obj2, obj1 );

    sign2_z = _fuzzy_sign( norm.v[2], FUZZY_ZERO );
    if (sign2_z == 0) {
        /* the plane of obj2 is perpendicular to the x-y plane so has no */
        /* near or far half spaces */
        return( HALF_SPACE_NONE );
    }

    rel1 = sub_obj_in_plane( obj1, norm );

    switch (rel1) {
    case SUB_PLANE_NONE:
        return( HALF_SPACE_NONE );
        break;
    case SUB_PLANE_ZERO:
        return( HALF_SPACE_SAME );
        break;
    case SUB_PLANE_NEG:
        if (sign2_z == -1) {
            return( HALF_SPACE_NEAR );
        } else {
            return( HALF_SPACE_FAR );
        }
        break;
    case SUB_PLANE_POS:
        if (sign2_z == -1) {
            return( HALF_SPACE_FAR );
        } else {
            return( HALF_SPACE_NEAR );
        }
        break;
    default:
        /* this case should not happen */
        return( HALF_SPACE_NONE );
        break;
    }
}

static bool near_half_space(
/**************************/
/* This tests whether obj1 is completly in the near half space (as seen by */
/* the viewer) of the plane of obj2. */
    rend_obj                *obj1,
    rend_obj                *obj2
) {
    check_half_space_rc     rc;

    rc = check_half_space( obj1, obj2 );
    return( rc == HALF_SPACE_NEAR || rc == HALF_SPACE_SAME );
}

static bool far_half_space(
/*************************/
/* This tests whether obj1 is completly in the far half space (as seen by */
/* the viewer) of the plane of obj2. */
    rend_obj                *obj1,
    rend_obj                *obj2
) {
    check_half_space_rc     rc;

    rc = check_half_space( obj1, obj2 );
    return( rc == HALF_SPACE_FAR || rc == HALF_SPACE_SAME );
}

static bool proj_overlap_ln_ln(
/*****************************/
/* Tests whether 2 line objects overlap in their projection into the x-y plane*/
    rend_obj *  obj1,
    rend_obj *  obj2
) {
    point       line1[2];
    point       line2[2];

    line1[0] = rend_obj_get_pt( obj1, 0 );
    line1[1] = rend_obj_get_pt( obj1, 1 );
    line2[0] = rend_obj_get_pt( obj2, 0 );
    line2[1] = rend_obj_get_pt( obj2, 1 );

    return( proj_line_intersection( line1, line2 ) );
}

static bool proj_overlap_ln_poly(
/*******************************/
/* Test whether a line and a poly overlap in their projections */
    rend_obj *  line,
    rend_obj *  obj_poly
) {
    point       ln[2];
    point       edge[2];
    int         num_pts;
    int         curr_pt;
    polygon     *poly;

    ln[0] = rend_obj_get_pt( line, 0 );
    ln[1] = rend_obj_get_pt( line, 1 );
    poly = rend_obj_get_poly( obj_poly );

    num_pts = _polygon_get_num_pts( *poly );
    /* start with the edge between point 0 and point (num_pts-1) */
    edge[0] = _polygon_get_pt( *poly, num_pts-1 );
    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        edge[1] = _polygon_get_pt( *poly, curr_pt );
        if (proj_line_intersection( ln, edge )) {
            return( TRUE );
        }
        edge[0] = edge[1];
    }
    return( FALSE );
}

static bool proj_overlap_poly_poly(
/*********************************/
/* Test whether the projection into the x-y plane of two polygons overlap */
/*
    NOTE: This routine does not work the way it is supposed to. What it is
    supposed to do is to check if the intersection of the two polygons is
    non-empty. What it actually test for is whether the intersection of the
    edges is non-empty. The cases that get missed are the ones in which one
    polygon is contained in the other. One possible way to fix this would be
    to test one vertex from each polygon against the other polygon. If this
    test is performed after the edge test then a point in the other polygon
    means that the entire polygon is contained.

    One posible source of a method to carry out this testing could be
        Preparata, F. P., and M. I. Shamos. "Computational Geometry:
                An Introduction". Springer-Verlag, New York, 1985.
    I have not seen this book but the title suggests that it may contain
    what we need.

    S.Bosnick
*/
    rend_obj *  obj_poly1,
    rend_obj *  obj_poly2
) {
    point       edge1[2];
    point       edge2[2];
    int         num_pts1;
    int         num_pts2;
    int         curr_pt1;
    int         curr_pt2;
    polygon     *poly1;
    polygon     *poly2;
    int         num_crosses;
    visible_info *vis1;
    visible_info *vis2;

    vis1 = _rend_obj_get_vis( obj_poly1 );
    vis2 = _rend_obj_get_vis( obj_poly2 );
    poly1 = rend_obj_get_poly( obj_poly1 );
    poly2 = rend_obj_get_poly( obj_poly2 );

    num_pts1 = _polygon_get_num_pts( *poly1 );
    num_pts2 = _polygon_get_num_pts( *poly2 );

    /* start with the edge between point 0 and point (num_pts1-1) */
    edge1[0] = _polygon_get_pt( *poly1, num_pts1-1 );
    for (curr_pt1 = 0; curr_pt1 < num_pts1; curr_pt1++) {
        edge1[1] = _polygon_get_pt( *poly1, curr_pt1 );

        /* start with the edge between point 0 and point (num_pts2-1) */
        edge2[0] = _polygon_get_pt( *poly2, num_pts2-1 );
        for (curr_pt2 = 0; curr_pt2 < num_pts2; curr_pt2++) {
            edge2[1] = _polygon_get_pt( *poly2, curr_pt2 );

            if (proj_line_intersection( edge1, edge2 )) {
                return( TRUE );
            }

            edge2[0] = edge2[1];
        }

        edge1[0] = edge1[1];
    }

    /* either poly 1 is contained in poly 2 . . .       */

    num_crosses = 0;
    edge2[0] = _polygon_get_pt( *poly2, num_pts2-1 );
    edge2[1] = edge2[0];
    edge2[1].p[0] = vis1->min.p[0] - .1;
    edge1[0] = _polygon_get_pt( *poly1, num_pts1-1 );
    for (curr_pt1 = 0; curr_pt1 < num_pts1; curr_pt1++) {
        edge1[1] = _polygon_get_pt( *poly1, curr_pt1 );

        if (proj_line_intersection( edge1, edge2 )) {
            num_crosses += 1;
        }

        edge1[0] = edge1[1];
    }
    if( num_crosses % 2 == 1 ) {
        return( TRUE );
    }

    /* or poly 2 is contained in poly 1 . . .           */

    num_crosses = 0;
    edge2[0] = _polygon_get_pt( *poly1, num_pts1-1 );
    edge2[1] = edge2[0];
    edge2[1].p[0] = vis2->min.p[0] - .1;
    edge1[0] = _polygon_get_pt( *poly2, num_pts2-1 );
    for (curr_pt2 = 0; curr_pt2 < num_pts2; curr_pt2++) {
        edge1[1] = _polygon_get_pt( *poly2, curr_pt2 );

        if (proj_line_intersection( edge1, edge2 )) {
            num_crosses += 1;
        }
        edge1[0] = edge1[1];
    }
    if( num_crosses % 2 == 1 ) {
        return( TRUE );
    }

    /* or they are truly disjoint                       */
    return( FALSE );
}


static bool proj_overlap(
/***********************/
/* Tests whether the projection of obj1 and obj2 into the x-y plane overlaps */
/* NOTE: This function assumes that extent testing has been performed and */
/* that the extents do overlap */
    rend_obj *  obj1,
    rend_obj *  obj2
) {
    rend_type   type1;
    rend_type   type2;

    type1 = _rend_obj_get_type( obj1 );
    type2 = _rend_obj_get_type( obj2 );

    /* the extent for a point is the point itself so if the extents overlap */
    /* then the projections will overlap */
    if (type1 == REND_OBJ_PT || type2 == REND_OBJ_PT) {
        return( TRUE );
    }

    switch (type1) {
    case REND_OBJ_LINE:
        switch (type2) {
        case REND_OBJ_LINE:
            return( proj_overlap_ln_ln( obj1, obj2 ) );
            break;
        case REND_OBJ_POLY:
            return( proj_overlap_ln_poly( obj1, obj2 ) );
            break;
        }
        break;
    case REND_OBJ_POLY:
        switch (type2) {
        case REND_OBJ_LINE:
            return( proj_overlap_ln_poly( obj2, obj1 ) );
            break;
        case REND_OBJ_POLY:
            return( proj_overlap_poly_poly( obj1, obj2 ) );
            break;
        }
        break;
    }

    return( TRUE );
}


static bool test_for_overlap(
/***************************/
/* This function implements tests 1-5 for the depth-sort algorithm. */
    rend_obj *      qobj,           // p and q refer to names on p. 673
    rend_obj *      pobj
) {
    visible_info *  qvis;
    visible_info *  pvis;

    qvis = _rend_obj_get_vis( qobj );
    pvis = _rend_obj_get_vis( pobj );

    /* test 1 */
    if (!_extent_overlap( qvis, pvis, 0 )) {
        return( FALSE );
    }
    /* test 2 */
    if (!_extent_overlap( qvis, pvis, 1 )) {
        return( FALSE );
    }
    /* test 3 */
    if (far_half_space( pobj, qobj )) {
        return( FALSE );
    }
    /* test 4 */
    if (near_half_space( qobj, pobj )) {
        return( FALSE );
    }
    /* test 5 */
    if (!proj_overlap( qobj, pobj )) {
        return( FALSE );
    }
    return( TRUE );
}

static bool test_for_mtf(
/************************/
/* Tests whether it is safe to move qobj to before pobj in the list. */
/* mtf -- move to front */
/* This function impliments test 3' and 4' from the depth-sort alogorithm */
    rend_obj *      qobj,           // p and q refer to names on p. 673
    rend_obj *      pobj
) {
    /* test 3' */
    if (far_half_space( qobj, pobj )) {
        return( TRUE );
    }
    /* test 4' */
    if (near_half_space( pobj, qobj )) {
        return( TRUE );
    }
    return( FALSE );
}

static void get_first_unmarked(
/*****************************/
    rend_list *     list,
    rend_list_ref * ref
) {
    visible_info *  vis;

    _rend_list_first( list, ref );
    while (!_rend_list_is_after_last( list, *ref )) {
        vis = _rend_obj_get_vis( _rend_list_get_obj( list, *ref ) );
        if (!vis->mark) {
            return;
        }
        _rend_list_next( list, ref );
    }
}

static void split_object(
/***********************/
/* Split obj1 by the plane of obj2 and insert the pieces into list in proper */
/* z-min order after any marked objects. */
    rend_list *     list,
    rend_obj *      obj1,
    rend_obj *      obj2
) {
    vector          norm;   // plane of obj2
    rend_obj *      p;      // pieces of obj1 in local memory
    rend_obj *      g;      // in pipe memory
    rend_list_ref   start;  // for specifying the sublist to insert into
    rend_list_ref   end;
    int             i;

    norm = calculate_object_norm( obj2, obj1 );

    p = rend_obj_clip( obj1, norm );
    if (p != NULL) {
        g = rend_obj_dup( p );
        rend_obj_lfree( p );
        rend_obj_compute_vis_info( g );
        get_first_unmarked( list, &start );
        _rend_list_last( list, &end );
        rend_list_bin_insert( list, start, end, &g, compare_zmin );
    }

    /* Change the direction of the normal */
    for (i=0; i<4; i++) {
        norm.v[i] *= -1;
    }

    p = rend_obj_clip( obj1, norm );
    if (p != NULL) {
        g = rend_obj_dup( p );
        rend_obj_lfree( p );
        rend_obj_compute_vis_info( g );
        get_first_unmarked( list, &start );
        _rend_list_last( list, &end );
        rend_list_bin_insert( list, start, end, &g, compare_zmin );
    }
}

static void split_marked_object(
/******************************/
/* This routine determines which of obj1 or obj2 to split the splits it and */
/* inserts the pieces into list */
    rend_list *     list,
    rend_obj *      obj1,
    rend_obj *      obj2
) {

    if (check_half_space( obj1, obj2 ) == HALF_SPACE_NONE) {
        split_object( list, obj1, obj2 );
    } else if (check_half_space( obj2, obj1 ) == HALF_SPACE_NONE) {
        split_object( list, obj2, obj1 );
    } else {
        // Neither obj1 nor obj2 is split by the plane of the other so some
        // other splitting criterion should be applied to one of the objects
        // and the resulting pieces inserted into the list.
        // Since this case should not occur and I can't think of such a
        // criterion this case is considered a fatal error.
        WPI_INST                inst;
        inst = inst;
#ifdef PLAT_OS2
        _wpi_setanchorblock( HWND_DESKTOP, inst );
#endif
        _wpi_fatalappexit( inst, 0, "Internal wcgr error." );
    }
}

static void check_overlap(
/************************/
    rend_list *     list,
    rend_list_ref * dref            // reference to the candidate for drawing
) {
    rend_list_ref   cref;           // reference to the obj to check
    rend_obj *      dobj;
    rend_obj *      cobj;
    visible_info *  dvis;
    visible_info *  cvis;

    cref = *dref;
    _rend_list_next( list, &cref );
    /* note: a for loop is used here so the continue will work */
    for (; !_rend_list_is_after_last( list, cref );
            _rend_list_next( list, &cref ) ) {

        cobj = _rend_list_get_obj( list, cref );
        dobj = _rend_list_get_obj( list, *dref );
        cvis = _rend_obj_get_vis( cobj );
        dvis = _rend_obj_get_vis( dobj );

        /* Check if the objects overlap in z */
        if (dvis->max.p[2] < cvis->min.p[2]) {
            if (cvis->mark) {
                /* if the object is marked it is not in sorted order so */
                /* continue checking other objects in the list */
    /**/        continue;
            } else {
                /* if the object is not marked it is in sorted order in the */
                /* list so all objects later in the list have zmin >= zmin of */
                /* cobj so it is safe to draw dobj now */
    /**/        break;
            }
        }

        if (test_for_overlap( cobj, dobj )) {
            if (cvis->mark) {
                /* This case handles cyclical overlap which should not occur */
                /* in a 3d chart, but just in case ... */
                split_marked_object( list, dobj, cobj );
            } else {
                if (test_for_mtf( cobj, dobj )) {
                    /* mark cobj and move it to the front of the list */
                    cvis->mark = TRUE;
                    rend_list_delete( list, &cref );
                    _rend_list_first( list, &cref );
                    rend_list_insert( list, &cref, cobj, TRUE );
                } else {
                    /* ASSERT: dobj is not a point */
                    /* remove dref from list */
                    rend_list_delete( list, dref );
                    /* if we get here we know that dobj lies on both sides of */
                    /* the plane of cobj so split dobj by the plane of cobj */
                    /* and insert the pieces into the list in the proper spots*/
                    split_object( list, dobj, cobj );
                }
            }
            _rend_list_first( list, dref );
            cref = *dref;
        }
    }
}


#if 0
static void unmark_list(
/**********************/
    rend_list *     list
) {
    rend_list_ref   curr;
    visible_info *  vis;

    if (_rend_list_is_empty( list )) {
        return;
    }

    _rend_list_first( list, &curr );
    while (!_rend_list_is_after_last( list, curr )) {
        vis = _rend_obj_get_vis( _rend_list_get_obj( list, curr ) );
        vis->mark = FALSE;
        _rend_list_next( list, &curr );
    }
}
#endif

static void display_visible_surfaces(
/***********************************/
    rend_list *     list
) {
    rend_list_ref   curr_ref;       // reference to obj we wish to draw

//  compute_extents( list );
    rend_list_sort( list, compare_zmin );

    while (!_rend_list_is_empty( list )) {
        _rend_list_first( list, &curr_ref );
        check_overlap( list, &curr_ref );
        draw_illuminated_obj( _rend_list_get_obj( list, curr_ref ) );
        rend_list_delete( list, &curr_ref );
    }
}

extern void pipe3d_display(
/*************************/
    void
) {
    display_visible_surfaces( Pipe3dList );
}
