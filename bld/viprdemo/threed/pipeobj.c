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
 Routines to manipulate rend_obj's.
*/


#include "icgr.h"
#include <string.h>

#ifndef PROD
#define INCLUDE_DEBUG_ROUTINES
#endif


static point invalid_pt = { 0., 0., 0., 0. };

typedef struct face_pts {       // used for calculating normal of a face
    face *      side;
    point *     pts;
} face_pts;


extern rend_obj * rend_obj_new_poly(
/**********************************/
    void
) {
    rend_obj *  new;
    polygon *   poly;

    _new( new, 1 );
    new->type = REND_OBJ_POLY;
    poly = &(new->obj.poly);        // shorthand
    poly->num_pts = 0;
    poly->index = NULL;
    _new( poly->pts, 1 );
    poly->pts->num_pts = 0;
    poly->pts->pts = NULL;

    return( new );
}

static index_elt * identity_index(
/********************************/
    int         num_elt,
    bool *      edges
) {
    index_elt * index;
    int         curr_elt;

    _new( index, num_elt );
    if( index != NULL ) {
        for( curr_elt = 0; curr_elt < num_elt; curr_elt++ ) {
            index[ curr_elt ].pt_num = curr_elt;
            if( edges != NULL ) {
                index[ curr_elt ].hilight_edge = edges[ curr_elt ];
            } else {
                index[ curr_elt ].hilight_edge = TRUE;
            }
            index[ curr_elt ].force_rgn_pt = FALSE;
        }
    }

    return( index );
}

extern rend_obj * rend_obj_create_poly(
/*************************************/
    int             num_pts,
    point *         pts,
    hls_colour      base,
    bool            black_edges,
    region_info *   rgn,
    bool *          edges,
    int             rgn_pt_iter
) {
    rend_obj *      new;
    polygon *       poly;

    new = rend_obj_new_poly();
    if( new != NULL ) {
        new->base_colour = base;
        new->black_edges = black_edges;
        new->rgn = *rgn;
        poly = &new->obj.poly;      // shorthand
        poly->pts->num_pts = num_pts;
        poly->pts->pts = pts;
        poly->rgn_pt_iter = rgn_pt_iter;
        poly->num_pts = num_pts;
        poly->index = identity_index( num_pts, edges );
    }

    return( new );
}

extern rend_obj * rend_obj_create_line(
/*************************************/
    point           start,
    point           end,
    hls_colour      colour,
    line_style      style,
    region_info *   rgn
) {
    rend_obj *  obj;

    _new( obj, 1 );
    if( obj != NULL ) {
        obj->type = REND_OBJ_LINE;
        obj->base_colour = colour;
        obj->black_edges = FALSE;
        obj->rgn = *rgn;
        obj->obj.line.start = start;
        obj->obj.line.end = end;
        obj->obj.line.style = style;
    }

    return( obj );
}

static vertex_list * dup_vertex_list(
/***********************************/
    vertex_list *   old
) {
    vertex_list *   new;

    _pipenew( new, 1 );
    new->num_pts = old->num_pts;
    _pipenew( new->pts, new->num_pts );
    memcpy( new->pts, old->pts, new->num_pts * sizeof(point) );

    return( new );
}

static index_elt * dup_index(
/***************************/
    index_elt * old,
    int         num_entries
) {
    index_elt * new;

    _pipenew( new, num_entries );
    memcpy( new, old, num_entries * sizeof(*new) );

    return( new );
}

static void copy_poly(
/********************/
    polygon *   old,
    polygon *   new
) {
    new->normal = old->normal;
    new->num_pts = old->num_pts;
    new->index = dup_index( old->index, old->num_pts );
    new->pts = dup_vertex_list( old->pts );
}

static void point_min(
/********************/
    point       pt1,
    point       pt2,
    point *     min
) {
    min->p[0] = _min( pt1.p[0], pt2.p[0] );
    min->p[1] = _min( pt1.p[1], pt2.p[1] );
    min->p[2] = _min( pt1.p[2], pt2.p[2] );
    min->p[3] = _min( pt1.p[3], pt2.p[3] );
}

static void point_max(
/********************/
    point       pt1,
    point       pt2,
    point *     max
) {
    max->p[0] = _max( pt1.p[0], pt2.p[0] );
    max->p[1] = _max( pt1.p[1], pt2.p[1] );
    max->p[2] = _max( pt1.p[2], pt2.p[2] );
    max->p[3] = _max( pt1.p[3], pt2.p[3] );
}

static bool face_visible(
/***********************/
    face *          curr_face,
    vertex_list *   pts_list,
    visible_info *  vis
) {
    point *         min;
    point *         max;
    float           x1, y1;
    float           x2, y2;
    index_elt *     index;      // shorthand
    int             num_pts;    // shorthand
    point *         pts;        // shorthand
    int             curr_pt;

    index = curr_face->index;
    num_pts = curr_face->num_pts;
    pts = pts_list->pts;
    min = &vis->min;
    max = &vis->max;

    vis->mark = FALSE;

    *min = pts[ index[0].pt_num ];
    *max = pts[ index[0].pt_num ];

    for( curr_pt = 1; curr_pt < num_pts; curr_pt++ ) {
        point_min( *min, pts[ index[ curr_pt ].pt_num ], min );
        point_max( *max, pts[ index[ curr_pt ].pt_num ], max );
    }

    project_pt( *min, &x1, &y1 );
    project_pt( *max, &x2, &y2 );

    return( _wrectvisible( x1, y1, x2, y2 ) );
}

static void add_solid_to_list(
/****************************/
/* solids are not added to the list directly, instead they are broken up */
/* into their component faces and these are added as polygons */
/* NOTE: The memory that is occupied by the points list of the polygon becomes*/
/* shared by the component polygons after this call. This doesn't present a */
/* problem in the pipemem memory manager since it is allocate only. If a real */
/* memory manager is being used then either a reference count would have to */
/* be maintained or each polygon could get a copy of the points that they */
/* reference. */
    rend_obj *      obj,
    rend_list *     list
) {
    rend_obj *      new;
    polygon *       new_poly;
    vertex_list *   pts;
    face *          curr_face;
    face_list *     old_faces;
    visible_info    vis;

    old_faces = &(obj->obj.solid.faces);        // shorthand

    /* All the polygons will refer to the same vertex list. This is OK */
    /* if the solid is consitent */
    pts = dup_vertex_list( &(obj->obj.solid.pts) );

    for (curr_face = old_faces->faces;
            curr_face < old_faces->faces + old_faces->num_faces; curr_face++ ) {
        /* back face culling */
        if( curr_face->normal.v[2] > FUZZY_ZERO &&
                        face_visible( curr_face, pts, &vis ) ) {
            _pipenew( new, 1 );
            new->type = REND_OBJ_POLY;
            new->vis = vis;
            if( curr_face->black ) {
                new->base_colour = Black_hls;
            } else {
                new->base_colour = obj->base_colour;
            }
            new->black_edges = obj->black_edges;
            new->rgn = obj->rgn;
            new_poly = &(new->obj.poly);                // shorthand
            new_poly->normal = curr_face->normal;
            new_poly->num_pts = curr_face->num_pts;
            new_poly->index = dup_index( curr_face->index, curr_face->num_pts );
            new_poly->rgn_pt_iter = curr_face->rgn_pt_iter;
            new_poly->pts = pts;                // use the common vertex list

            rend_list_add( list, new );
        }
    }
}

extern rend_obj * rend_obj_dup(
/*****************************/
/* duplicates obj into pipe_mem memory */
    rend_obj *  obj
) {
    rend_obj *  new_obj;

    switch (obj->type) {
    case REND_OBJ_PT:
    case REND_OBJ_LINE:
        _pipenew( new_obj, 1 );
        *new_obj = *obj;    // point and line structures don't have pointers
        break;

    case REND_OBJ_POLY:
        _pipenew( new_obj, 1 );
        *new_obj = *obj;
        copy_poly( &(obj->obj.poly), &(new_obj->obj.poly) );
        break;

    case REND_OBJ_SOLID:
        new_obj = NULL;
        break;
    }

    return( new_obj );
}


extern void rend_obj_add_to_list(
/*******************************/
    rend_obj *      obj,
    rend_list *     list
) {
    rend_obj *      new_obj;

    if (obj->type == REND_OBJ_SOLID) {
        add_solid_to_list( obj, list );
    } else {
        new_obj = rend_obj_dup( obj );
        rend_obj_compute_vis_info( new_obj );
        rend_list_add( list, new_obj );
    }
}

extern void rend_obj_lfree(
/*************************/
/* free a rend_obj from local memory */
/* NOTE: do not call this function for rend_obj's that have been added */
/* to the rend_list via a rend_obj_add_to_list call; they are not in */
/* local memory. */
    rend_obj *      obj
) {
    polygon *       poly;
    vertex_list *   pts;
    face_list *     faces;
    face *          curr_face;

    switch (obj->type) {
    case REND_OBJ_POLY:
        poly = &(obj->obj.poly);        // shorthand
        pts = poly->pts;                // shorthand
        _free( pts->pts );
        _free( pts );
        _free( poly->index );
        break;
    case REND_OBJ_SOLID:
        pts = &(obj->obj.solid.pts);        // shorthand
        faces = &(obj->obj.solid.faces);    // shorthand

        _free( pts->pts );
        for (curr_face = faces->faces;
                curr_face < faces->faces + faces->num_faces; curr_face++ ) {
            _free( curr_face->index );
        }
        _free( faces->faces );
        break;
    }

    _free( obj );
}

static void compute_poly_vis_info(
/********************************/
    polygon *       poly,
    visible_info *  ext
) {
    int             curr_pt;

    ext->min = _polygon_get_pt( *poly, 0 );
    ext->max = _polygon_get_pt( *poly, 0 );

    for (curr_pt = 1; curr_pt < poly->num_pts; curr_pt++) {
        point_min( ext->min, _polygon_get_pt( *poly, curr_pt ), &ext->min );
        point_max( ext->max, _polygon_get_pt( *poly, curr_pt ), &ext->max );
    }
}

static void compute_solid_vis_info(
/*********************************/
/* assume that all points mentioned in the vertex_list are reference by at */
/* at least one face */
    polyhedron *    solid,
    visible_info *  ext
) {
    point *         pt;

    pt = solid->pts.pts;
    ext->min = *pt;
    ext->max = *pt;

    while (pt < solid->pts.pts + solid->pts.num_pts) {
        point_min( ext->min, *pt, &ext->min );
        point_max( ext->max, *pt, &ext->max );

        pt++;
    }
}

extern void rend_obj_compute_vis_info(
/************************************/
    rend_obj *      obj
) {
    switch (obj->type) {
    case REND_OBJ_PT:
        obj->vis.min = obj->obj.pt;
        obj->vis.max = obj->obj.pt;
        break;
    case REND_OBJ_LINE:
        point_min( obj->obj.line.start, obj->obj.line.end, &obj->vis.min );
        point_max( obj->obj.line.start, obj->obj.line.end, &obj->vis.max );
        break;
    case REND_OBJ_POLY:
        compute_poly_vis_info( &(obj->obj.poly), &(obj->vis) );
        break;
    case REND_OBJ_SOLID:
        compute_solid_vis_info( &(obj->obj.solid), &(obj->vis) );
        break;
    }
    obj->vis.mark = FALSE;
}

#if 0
/* macroed */
extern visible_info * rend_obj_get_vis(
/*************************************/
    rend_obj *      obj
) {
    return( &(obj->vis) );
}
#endif

extern vector rend_obj_get_norm(
/******************************/
    rend_obj *  obj
) {
    vector      norm;

    if (obj->type == REND_OBJ_POLY) {
        norm = obj->obj.poly.normal;
    } else {
        norm.v[0] = 0.;
        norm.v[1] = 0.;
        norm.v[2] = 0.;
        norm.v[3] = 0.;
    }

    return( norm );
}

extern int rend_obj_get_num_pts(
/******************************/
    rend_obj *  obj
) {
    switch (obj->type) {
    case REND_OBJ_SOLID:
        return( obj->obj.solid.pts.num_pts );
        break;
    case REND_OBJ_POLY:
        return( obj->obj.poly.num_pts );
        break;
    case REND_OBJ_LINE:
        return( 2 );
        break;
    case REND_OBJ_PT:
        return( 1 );
        break;
    default:
        return( 0 );
        break;
    }
}

extern point rend_obj_get_pt(
/***************************/
    rend_obj *  obj,
    int         pt_num
) {
    point       pt;

    switch (obj->type) {
    case REND_OBJ_PT:
        if (pt_num == 0) {
            pt = obj->obj.pt;
        } else {
            pt = invalid_pt;
        }
        break;
    case REND_OBJ_LINE:
        switch (pt_num) {
        case 0:
            pt = obj->obj.line.start;
            break;
        case 1:
            pt = obj->obj.line.end;
            break;
        default:
            pt = invalid_pt;
            break;
        }
        break;
    case REND_OBJ_POLY:
        if (0 <= pt_num && pt_num < obj->obj.poly.num_pts) {
            pt = _polygon_get_pt( obj->obj.poly, pt_num );
        } else {
            pt = invalid_pt;
        }
        break;
    case REND_OBJ_SOLID:
        if (0 <= pt_num && pt_num < obj->obj.solid.pts.num_pts) {
            pt = obj->obj.solid.pts.pts[ pt_num ];
        } else {
            pt = invalid_pt;
        }
        break;
    default:
        pt = invalid_pt;
        break;
    }

    return( pt );
}

extern polygon * rend_obj_get_poly(
/*********************************/
    rend_obj *  obj
) {
    if (obj->type == REND_OBJ_POLY) {
        return( &obj->obj.poly );
    } else {
        return( NULL );
    }
}

extern line * rend_obj_get_line(
/******************************/
    rend_obj * obj
) {
    if( obj->type == REND_OBJ_LINE ) {
        return( &obj->obj.line );
    } else {
        return( NULL );
    }
}

#if 0
/* macroed */
extern rend_type rend_obj_get_type(
/*********************************/
    rend_obj *  obj
) {
    return( obj->type );
}
#endif

static point polygon_get_pt(
/**************************/
    polygon *   poly,
    int         pt_num
) {
    return( _polygon_get_pt( *poly, pt_num ) );
}

static void calc_norm_poly(
/*************************/
    polygon *   poly
) {
    int         i;

    poly->normal = calculate_normal_vector( poly->num_pts,
                                poly, (point(*)(void*,int))polygon_get_pt );
    /* since the the transformation has been applied, a normal is facing */
    /* the viewer if its z coordinate is positive */
    if( poly->normal.v[2] < 0. ) {
        for( i = 0; i < 4; i++ ) {
            poly->normal.v[i] *= -1.;
        }
    }
}

static point face_get_pt(
/***********************/
    face_pts *  side,
    int         pt_num
) {
    return( side->pts[ side->side->index[ pt_num ].pt_num ] );
}

static void calc_norm_solid(
/**************************/
    polyhedron *    solid
) {
    face *          curr_face;
    face_pts        side;

    side.pts = solid->pts.pts;

    for (curr_face = solid->faces.faces; curr_face < solid->faces.faces
                + solid->faces.num_faces; curr_face++) {
        side.side = curr_face;
        curr_face->normal = calculate_normal_vector( curr_face->num_pts,
                        &side, (point(*)(void*,int))face_get_pt );
    }
}

extern void rend_obj_calculate_normal(
/************************************/
    rend_obj *  obj
) {
    switch (obj->type) {
    case REND_OBJ_PT:
    case REND_OBJ_LINE:
        break;              // lines and points don't have normals
    case REND_OBJ_POLY:
        calc_norm_poly( &(obj->obj.poly) );
        break;
    case REND_OBJ_SOLID:
        calc_norm_solid( &(obj->obj.solid) );
        break;
    }
}

static bool inside(
/*****************/
    point   pt,
    vector  norm
) {
    float   result;

    result = _dot_prod_vp( norm, pt );

    return( result < FUZZY_ZERO );
}

static void output(
/*****************/
    point           pt,
    polygon *       poly,
    bool            edge,
    bool            force_pt
) {
    vertex_list *   pts;

    pts = poly->pts;

    pts->num_pts += 1;
    _renew( pts->pts, pts->num_pts );
    pts->pts[ pts->num_pts - 1 ] = pt;

    poly->num_pts += 1;
    _renew( poly->index, poly->num_pts );
    poly->index[ poly->num_pts - 1 ].pt_num = pts->num_pts - 1;
    poly->index[ poly->num_pts - 1 ].hilight_edge = edge;
    poly->index[ poly->num_pts - 1 ].force_rgn_pt = force_pt;
}

static bool intersect(
/********************/
/* The point of intersection between the line first to second and the plane */
/* defined by normal is place in inter. See Log Notes p. 1 */
/* Returns TRUE if the intersection point falls between first and second. */
    point   first,
    point   second,
    vector  normal,
    point * inter
) {
    float   t;
    float   denom;
    vector  p;
    int     i;

    for (i=0; i < 4; i++) {
        p.v[i] = second.p[i] - first.p[i];
    }

    denom = _dot_prod_vv( normal, p );
    if (fabs( denom ) < FUZZY_ZERO) {
        /* the lines are numerically parallel so set the point arbitraily */
        /* and return FALSE */
        for (i=0; i < 4; i++) {
            inter->p[i] = first.p[i];
        }
        return( FALSE );
    } else {
        t = - _dot_prod_vp( normal, first ) / denom;
    }

    for (i=0; i < 4; i++) {
        inter->p[i] = first.p[i] + t * p.v[i];
    }

    return( 0. <= t && t <= 1. );
}

static bool clip_3d_poly(
/***********************/
/*
clip the polygon defined by 'obj' against the bounding plane defined by
'normal' and return the resulting rend_obj in local memory. See
"Computer Graphics" by Foley, et. al. pp. 124-129. 'normal' is assumed to
be outward pointing.
Returns TRUE if the polygon is outside the clip plane.
*/
    polygon *   poly,
    polygon *   new_poly,
    vector      normal
) {
    point       s;          // starting point of current edge
    point       p;          // ending point
    point       i;          // intersection point of edge with plane
    int         pt_num;
    bool        edge_s;
    bool        edge_p;
    bool        force_p;

    new_poly->normal = poly->normal;
    new_poly->rgn_pt_iter = poly->rgn_pt_iter;

    s = _polygon_get_pt( *poly, poly->num_pts - 1 );
    edge_s = _polygon_edge_hilight( *poly, poly->num_pts - 1 );
    for (pt_num = 0; pt_num < poly->num_pts; pt_num++) {
        p = _polygon_get_pt( *poly, pt_num );
        edge_p = _polygon_edge_hilight( *poly, pt_num );
        force_p = _polygon_force_rgn_pt( *poly, pt_num );
        if (inside( p, normal)) {       // cases 1 and 4
            if (inside( s, normal )) {
                output( p, new_poly, edge_p, force_p ); // case 1
            } else {
                intersect( s, p, normal, &i );  // case 4
                output( i, new_poly, edge_s, FALSE );
                output( p, new_poly, edge_p, force_p );
            }
        } else {                        // cases 2 and 3
            if (inside( s, normal )) {          // case 2
                intersect( s, p, normal, &i );
                output( i, new_poly, FALSE, FALSE );
            }                                   // no action for case 3
        }
        s = p;
        edge_s = edge_p;
    }

    return( new_poly->num_pts == 0 );
}

static bool clip_3d_line(
/***********************/
    line *      in,
    line *      out,
    vector      normal
) {
    bool        start_in;
    bool        end_in;

    start_in = inside( in->start, normal );
    end_in = inside( in->end, normal );

    if (start_in) {
        out->start = in->start;
    } else {
        intersect( in->start, in->end, normal, &(out->start) );
    }
    if (end_in) {
        out->end = in->end;
    } else {
        intersect( in->start, in->end, normal, &(out->end) );
    }

    return (!(start_in || end_in));
}

extern rend_obj * rend_obj_clip(
/******************************/
/* Clip obj by the plane defined by norm and put the result in memory */
/* allocated by alloc and return a pointer to it */
/* NOTE: points are not handled by this algorithm as they can't be clipped */
    rend_obj *  obj,
    vector      norm
) {
    rend_obj *  new_obj;

    switch (obj->type) {
    case REND_OBJ_LINE:
        _new( new_obj, 1 );
        new_obj->type = REND_OBJ_LINE;
        new_obj->base_colour = obj->base_colour;
        new_obj->black_edges = obj->black_edges;
        new_obj->rgn = obj->rgn;
        clip_3d_line( &(obj->obj.line), &(new_obj->obj.line), norm );
        break;
    case REND_OBJ_POLY:
        new_obj = rend_obj_new_poly();
        new_obj->base_colour = obj->base_colour;
        new_obj->black_edges = obj->black_edges;
        new_obj->rgn = obj->rgn;
        clip_3d_poly( &(obj->obj.poly), &(new_obj->obj.poly), norm );
        break;
    default:
        new_obj = NULL;
        break;
    }

    return( new_obj );
}

static point get_pt_from_array(
/*****************************/
    point   *array,
    int     pt_num
) {
    return( array[ pt_num ] );
}

static point * create_points_array(
/*********************************/
/* See rend_obj_create_sweep for details of how this works. */
/* This routine double the number of points. */
    pt_edge         *in_pts,
    int             num_pts,
    float           height,
    text_info *     text,
    bool            on_bottom       // is the text on bottom
) {
    point           *out_pts;
    vector          normal;
    int             curr_pt;

    _new( out_pts, 2 * num_pts );

    /* copy the original points into out_pts */
    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        out_pts[ curr_pt ] = in_pts[ curr_pt ].pt;
    }

    normal = calculate_normal_vector( num_pts, out_pts, (point(*)(void*,int))get_pt_from_array );

    /* add the new points to the points list */
    for (curr_pt = num_pts; curr_pt < 2*num_pts; curr_pt++) {
        out_pts[ curr_pt ].p[0] = in_pts[ curr_pt - num_pts ].pt.p[0] -
                                height * normal.v[0];
        out_pts[ curr_pt ].p[1] = in_pts[ curr_pt - num_pts ].pt.p[1] -
                                height * normal.v[1];
        out_pts[ curr_pt ].p[2] = in_pts[ curr_pt - num_pts ].pt.p[2] -
                                height * normal.v[2];
        out_pts[ curr_pt ].p[3] = 1.;
    }

    if( text != NULL ) {
        if( on_bottom ) {
            text->pt2 = text->pt1;
            text->pt1.p[0] = text->pt2.p[0] - height * normal.v[0];
            text->pt1.p[1] = text->pt2.p[1] - height * normal.v[1];
            text->pt1.p[2] = text->pt2.p[2] - height * normal.v[2];
            text->pt1.p[3] = 1.;
            text->centre.p[0] = text->centre.p[0] - height * normal.v[0];
            text->centre.p[1] = text->centre.p[1] - height * normal.v[1];
            text->centre.p[2] = text->centre.p[2] - height * normal.v[2];
            text->centre.p[3] = 1.;
        } else {
            text->pt2.p[0] = text->pt1.p[0] - height * normal.v[0];
            text->pt2.p[1] = text->pt1.p[1] - height * normal.v[1];
            text->pt2.p[2] = text->pt1.p[2] - height * normal.v[2];
            text->pt2.p[3] = 1.;
        }
    }

    return( out_pts );
}

/* these macros are used in the layout_faces fuction only */
#define _get_top_pt( pt_num, num_pts ) (pt_num) % (num_pts)
#define _get_bottom_pt( pt_num, num_pts) (pt_num) % (num_pts) + (num_pts)

static void layout_faces(
/***********************/
/* See rend_obj_create_sweep for how this routine works */
/* NOTE: This routine is sensitive to change. Make sure the side faces are */
/* set properly. */
    face_list *     list,
    rend_obj_add *  add
) {
    int             num_faces;
    int             curr_pt;
    int             curr_face;
    face            *faces;
    index_elt       *ind;
    int             num_pts;    // in the top face
    pt_edge         *edges;

    num_pts = add->num_pts;
    edges = add->pts;

    /* set up the face_list */
    num_faces = num_pts + 2;
    list->num_faces = num_faces;
    _new( list->faces, num_faces );
    faces = list->faces;

    /* add the top face */
    faces[0].num_pts = num_pts;
    faces[0].black = FALSE;
    faces[0].rgn_pt_iter = add->top_rgn_iter;
    _new( faces[0].index, faces[0].num_pts );
    ind = faces[0].index;                   // shorthand
    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        ind[ curr_pt ].pt_num = curr_pt;
        ind[ curr_pt ].hilight_edge = add->top_edges;
        ind[ curr_pt ].force_rgn_pt = edges[ curr_pt ].force_rgn_pt;
    }

    /* add the bottom face */
    faces[1].num_pts = num_pts;
    faces[1].black = FALSE;
    faces[1].rgn_pt_iter = add->bottom_rgn_iter;
    _new( faces[1].index, faces[1].num_pts );
    ind = faces[1].index;                   // shorthand
    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        ind[ curr_pt ].pt_num = (2*num_pts - 1) - curr_pt;
        ind[ curr_pt ].hilight_edge = add->bottom_edges;
        ind[ curr_pt ].force_rgn_pt =
                        edges[ (num_pts - 1) - curr_pt ].force_rgn_pt;
    }

    /* add the side faces */
    for (curr_face = 2; curr_face < num_faces; curr_face++) {
        curr_pt = curr_face - 2;
        faces[ curr_face ].num_pts = 4;     // side faces are rectangles
        faces[ curr_face ].black = edges[ curr_pt ].black_face;
        faces[ curr_face ].rgn_pt_iter = edges[ curr_pt ].face_rgn_iter;
        _new( faces[ curr_face ].index, faces[ curr_face ].num_pts );
        ind = faces[ curr_face  ].index;            // shorthand
        ind[0].pt_num = _get_top_pt( curr_pt, num_pts );
        ind[1].pt_num = _get_bottom_pt( curr_pt, num_pts );
        ind[2].pt_num = _get_bottom_pt( curr_pt+1, num_pts );
        ind[3].pt_num = _get_top_pt( curr_pt+1, num_pts );
        ind[0].hilight_edge = edges[ curr_pt ].edge;
        ind[1].hilight_edge = add->bottom_edges;
        ind[2].hilight_edge = edges[ curr_pt+1 ].edge;
        ind[3].hilight_edge = add->top_edges;
        ind[0].force_rgn_pt = FALSE;
        ind[1].force_rgn_pt = FALSE;
        ind[2].force_rgn_pt = FALSE;
        ind[3].force_rgn_pt = FALSE;
    }
}

#undef _get_top_pt
#undef _get_bottom_pt

#if 0

static void set_line(
/*******************/
/* Fills in obj to be a line from start to end */
    rend_obj        *obj,
    point           start,
    point           end,
    hls_colour      colour
) {
    obj->type = REND_OBJ_LINE;
    obj->base_colour = colour;
    obj->obj.line.start = start;
    obj->obj.line.end = end;
}

#endif

extern rend_obj * rend_obj_create_sweep(
/**************************************/
    rend_obj_add    *add
/*
Creates a solid object by performing a transitional sweep of the polygon
defined by pts downwards for a distance of height. See "Computer Graphics" by
Foley, et. al. pp. 540,541. The polygon will end up being the "top" face of the
polygon, where top is the side of the polygon from which the points in pts
proceed in a counter-clockwise fasion. This side will be outside of the solid.
There will be num_pts+2 faces in the solid: num_pts side faces, a top face,
and a bottom face. In order to keep the "counter-clockwise from the outside"
convention the faces of the solid are constructed as follows:
    1. number the points in pts 1,2,3,...,n
    2. add points 1',2',3',...,n' to pts which are calculated as
            1' = 1 - height * normal_to_plane
            2' = 2 - height * normal_to_plane
            etc.
    3. Add the top face with points 1,2,3,...,n (in order).
    4. Add the side faces:
            side face 1: points 1,1',2',2
            side face 2: points 2,2',3',3
            ...
            side face n: points n,n',1',1
    5. Add the bottom face with points n',(n-1)',...,3',2',1'
*/
) {
    rend_obj *      new_obj;
    point *         pts;

    if( add->height < FUZZY_ZERO ) {
        return( NULL );
    }

    _new( new_obj, 1 );
    if( new_obj == NULL ){
        return( NULL );
    }

    pts = create_points_array( add->pts, add->num_pts, add->height,
                    add->text, add->text_on_bottom );

    /* set up the solid object */
    new_obj->type = REND_OBJ_SOLID;
    new_obj->base_colour = add->base;
    new_obj->black_edges = add->black_edges;
    new_obj->rgn = add->rgn;
    new_obj->obj.solid.pts.num_pts = 2 * add->num_pts;
    new_obj->obj.solid.pts.pts = pts;
    layout_faces( &new_obj->obj.solid.faces, add );

    return( new_obj );
}

extern void rend_obj_add_sweep(
/*****************************/
    rend_obj_add    *add
) {
    rend_obj        *new_obj;       // array of line and solid objects to add

    new_obj = rend_obj_create_sweep( add );

    pipe3d_add( new_obj, add->text, FALSE );
}

static void transform_poly(
/*************************/
    polygon *   poly,
    float       matrix[4][4]
) {
   int          curr_pt;

   for (curr_pt = 0; curr_pt < poly->num_pts; curr_pt++) {
       _polygon_get_pt( *poly, curr_pt ) = mult_matrix_pt( matrix,
                                    _polygon_get_pt( *poly, curr_pt ) );
   }
}

static void transform_solid(
/**************************/
/* transform a solid by transforming all of the points in the vertex list */
    polyhedron *    solid,
    float           matrix[4][4]
) {
    int             curr_pt;

    for (curr_pt = 0; curr_pt < solid->pts.num_pts; curr_pt++) {
        solid->pts.pts[ curr_pt ] = mult_matrix_pt( matrix,
                                        solid->pts.pts[ curr_pt ] );
    }
}

extern void rend_obj_transform(
/*****************************/
/* apply the transformation matrix to the points of obj */
    rend_obj *  obj,
    float       matrix[4][4]
) {
    switch (obj->type) {
    case REND_OBJ_PT:
        obj->obj.pt = mult_matrix_pt( matrix, obj->obj.pt );
        break;
    case REND_OBJ_LINE:
        obj->obj.line.start = mult_matrix_pt( matrix, obj->obj.line.start );
        obj->obj.line.end = mult_matrix_pt( matrix, obj->obj.line.end );
        break;
    case REND_OBJ_POLY:
        transform_poly( &(obj->obj.poly), matrix );
        break;
    case REND_OBJ_SOLID:
        transform_solid( &(obj->obj.solid), matrix );
        break;
    }
}

extern void point_homo_to_3d(
/***************************/
    point *     pt
) {
    float       w;
    int         i;

    w = pt->p[3];
    if (w != 0.) {
        for (i = 0; i < 4; i++) {
            pt->p[i] /= w;
        }
    }
}

static void homo_to_3d_poly(
/**************************/
    polygon *   poly
) {
    int         curr_pt;

    for (curr_pt = 0; curr_pt < poly->num_pts; curr_pt++) {
        point_homo_to_3d( &( _polygon_get_pt( *poly, curr_pt ) ) );
    }
}

static void homo_to_3d_solid(
/***************************/
    polyhedron *    solid
) {
    int             curr_pt;

    for (curr_pt = 0; curr_pt < solid->pts.num_pts; curr_pt++) {
        point_homo_to_3d( &(solid->pts.pts[ curr_pt ] ) );
    }
}

extern void rend_obj_homo_to_3d(
/******************************/
/* Change the points of obj from homogenous coordinates to 3d coordinates */
/* by deviding by W */
    rend_obj *  obj
) {
    switch (obj->type) {
    case REND_OBJ_PT:
        point_homo_to_3d( &(obj->obj.pt) );
        break;
    case REND_OBJ_LINE:
        point_homo_to_3d( &(obj->obj.line.start) );
        point_homo_to_3d( &(obj->obj.line.end) );
        break;
    case REND_OBJ_POLY:
        homo_to_3d_poly( &(obj->obj.poly) );
        break;
    case REND_OBJ_SOLID:
        homo_to_3d_solid( &(obj->obj.solid) );
        break;
    }
}

extern hls_colour rend_obj_get_base_colour(
/*****************************************/
    rend_obj    *obj
) {
    return( obj->base_colour );
}

#if 0
/* macroed */
extern bool polygon_edge_hilight(
/*******************************/
    rend_obj    *obj,           // must be a polygon type
    int         edge_num
) {
    if (obj->type == REND_OBJ_POLY) {
        if (0 <= edge_num && edge_num < obj->obj.poly.num_pts) {
            return( obj->obj.poly.index[ edge_num ].hilight_edge );
        } else {
            return( FALSE );
        }
    } else {
        return( FALSE );
    }
}
#endif

extern void rend_obj_region_begin(
/********************************/
    rend_obj *  obj
) {
    switch( obj->rgn.use_info ) {
    case USE_RGN_GROUP:
        rgn_man_group_begin( obj->rgn.set_num );
        rgn_set_ids( obj->rgn.major_id, obj->rgn.minor_id,
                                        obj->rgn.data_row, obj->rgn.data_col );
        break;
    case USE_RGN_SET:
        rgn_man_set_begin( obj->rgn.set_num );
        rgn_set_ids( obj->rgn.major_id, obj->rgn.minor_id,
                                        obj->rgn.data_row, obj->rgn.data_col );
        break;
    }
}

extern void rend_obj_region_end(
/******************************/
    rend_obj *  obj
) {
    switch( obj->rgn.use_info ) {
    case USE_RGN_GROUP:
        rgn_man_group_end();
        break;
    case USE_RGN_SET:
        rgn_man_set_end();
        break;
    }
}


#ifdef INCLUDE_DEBUG_ROUTINES

/* DEBUG ROUTINES */
#include <stdio.h>

extern void dbg_print_pt(
/***********************/
    FILE    *fp,
    point   pt,
    char    *prefix
) {
    fprintf( fp, "%s % 12.5g % 12.5g % 12.5g % 12.5g\n", prefix,
                    (double) pt.p[0], (double) pt.p[1],
                    (double) pt.p[2], (double) pt.p[3] );
}

extern void dbg_print_vect(
/*************************/
    FILE    *fp,
    vector  vect,
    char    *prefix
) {
    fprintf( fp, "%s % 12.5g % 12.5g % 12.5g % 12.5g\n", prefix,
                    (double) vect.v[0], (double) vect.v[1],
                    (double) vect.v[2], (double) vect.v[3] );
}

extern void dbg_print_vertex_list(
/********************************/
    FILE        *fp,
    vertex_list *list
) {
    int         curr_pt;

    fprintf( fp, "\tVertex list: num_pts = %d\n", list->num_pts );
    for (curr_pt = 0; curr_pt < list->num_pts; curr_pt++) {
        dbg_print_pt( fp, list->pts[ curr_pt ], "\t    " );
    }
}

extern void dbg_print_index(
/**************************/
    FILE        *fp,
    int         num_pts,
    index_elt   *index,
    char        *prefix
) {
    int         curr_pt;
    char *      hilight_string;

    for (curr_pt = 0; curr_pt < num_pts; curr_pt++) {
        if (curr_pt % 5 == 0) {
            fprintf( fp, "%s", prefix );
        }
        if (index[ curr_pt ].hilight_edge) {
            hilight_string = "edge";
        } else {
            hilight_string = "";
        }
        fprintf( fp, "%3d %4.4s   ", index[ curr_pt ].pt_num, hilight_string );
    }
    fputc( '\n', fp );
}

extern void dbg_print_poly(
/*************************/
    FILE    *fp,
    polygon *poly
) {
    dbg_print_vect( fp, poly->normal, "\tNormal: " );

    fprintf( fp, "\tIndex: num_pts= %d", poly->num_pts );
    dbg_print_index( fp, poly->num_pts, poly->index, "\n\t    " );

    dbg_print_vertex_list( fp, poly->pts );
}

extern void dbg_print_face(
/*************************/
    FILE *      fp,
    face *      side
) {
    dbg_print_vect( fp, side->normal, "\tNormal: " );

    fprintf( fp, "\tIndex: num_pts= %d", side->num_pts );
    dbg_print_index( fp, side->num_pts, side->index, "\n\t    " );
}

extern void dbg_print_solid(
/**************************/
    FILE        *fp,
    polyhedron  *solid
) {
    int         curr_face;

    dbg_print_vertex_list( fp, &(solid->pts) );

    fprintf( fp, "\tFaces: num_faces = %d\n", solid->faces.num_faces );
    for (curr_face = 0; curr_face < solid->faces.num_faces; curr_face++) {
        dbg_print_face( fp, solid->faces.faces + curr_face );
    }
}

static void dbg_print_hls_colour(
/*******************************/
    FILE *      fp,
    hls_colour  hls,
    char *      prefix
) {
    fprintf( fp, "%s  h: % 12.5g  l: % 12.5g  s: % 12.5g\n", prefix,
                (double) hls.h, (double) hls.l, (double) hls.s );
}

static void dbg_print_region_info(
/********************************/
    FILE *      fp,
    region_info rgn,
    char *      prefix
) {
    if( rgn.use_info ) {
        fprintf( fp, "%s  major: %5d  minor: %5d  set: %5d\n", prefix,
                    rgn.major_id, rgn.minor_id, rgn.set_num );
    } else {
        fprintf( fp, "%s  Not Used\n", prefix );
    }
}

static void dbg_print_common_obj(
/*******************************/
    FILE *      fp,
    rend_obj *  obj,
    bool        contents
) {
    if (contents) {
        fprintf( fp, "Visible surface info:\n" );
    }
    fprintf( fp, "\tmark: %s\n", obj->vis.mark ? "TRUE" : "FALSE" );
    dbg_print_pt( fp, obj->vis.min, "\tmin:" );
    dbg_print_pt( fp, obj->vis.max, "\tmax:" );

    dbg_print_hls_colour( fp, obj->base_colour, "\tbase colour:" );
    fprintf( fp, "\tblack edges: %s\n", obj->black_edges ? "TRUE" : "FALSE" );
    dbg_print_region_info( fp, obj->rgn, "\tregion:" );
}

extern void dbg_print_one_obj(
/****************************/
    FILE        *fp,
    rend_obj    *obj,
    bool        contents
) {
    dbg_print_common_obj( fp, obj, contents );

    switch (obj->type) {
    case REND_OBJ_PT:
        fprintf( fp, "Type: Point\n" );
        if (contents) {
            dbg_print_pt( fp, obj->obj.pt, "\t" );
        }
        break;
    case REND_OBJ_LINE:
        fprintf( fp, "Type: Line\n" );
        if (contents) {
            dbg_print_pt( fp, obj->obj.line.start, "\tstart:" );
            dbg_print_pt( fp, obj->obj.line.end, "\tend:" );
        }
        break;
    case REND_OBJ_POLY:
        fprintf( fp, "Type: Polygon\n" );
        if (contents) {
            dbg_print_poly( fp, &(obj->obj.poly) );
        }
        break;
    case REND_OBJ_SOLID:
        fprintf( fp, "Type: Solid\n" );
        if (contents) {
            dbg_print_solid( fp, &(obj->obj.solid) );
        }
        break;
    default:
        fprintf( fp, "Type: UNKNOWN!!!!!!!!\n" );
        break;
    }
}

extern void dbg_get_filename(
/***************************/
/* path should be a buffer of size _MAX_PATH */
    char *  path
) {
    char *  tmp_dir;

    tmp_dir = getenv( "TMP" );
    if( tmp_dir == NULL ) {
        tmp_dir = "c:\\";
    }
    _makepath( path, NULL, tmp_dir, "dbgout", "tmp" );
}


extern void dbg_print_obj(
/************************/
    rend_obj *  obj
) {
    FILE *      fp;
    char        path[ _MAX_PATH ];

    dbg_get_filename( path );

    fp = fopen( path, "w" );
    if (fp == NULL) {
        return;
    }

    fprintf( fp, "obj address: %p\n", (void *) obj );

    dbg_print_one_obj( fp, obj, TRUE );

    fclose( fp );
}

#endif
