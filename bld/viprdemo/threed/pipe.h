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


#ifndef CGR_PIPE_H
#define CGR_PIPE_H
/*
 Description:
 ============
 This file defines the structures shared by the two parts of the 3d
 pipeline.


 The vectors in the pipeline will have their fourth component 0 unless they
 define the the normal of the plane. In that case the first 3 components will
 be the normal vector and the fourth will be the D paramter of the equation of
 the plane. That is:
        if  A=norm[0]  B=norm[1]  C=norm[2]  D=norm[3]  then the plane is
                Ax + By + Cz + D = 0
 For the reasoning behind this definition see "Computer Graphics" by Foley
 et. al. section 5.6.


 Renderable objects are one of a point, line, polygon, or polyhedron. Points,
 and lines use the obvious representation. Polyhedron use a polygon mesh
 representation as described in "Computer Graphics" section 11.1. The
 represention used here is a list of polygons, each of which is a list of
 pointers (indecies actually) to points. This represention uses less memory
 and permits more effiectient compution since each vertex appears only once
 instead of three times. For consitancy (and since they are not used much)
 polygon objects which are not part of a polygon mesh use the same
 representation, even though it is not the most efficient representaion in
 this case.

*/

/* These definitions of vector and point use homogeneous coordinates */
typedef struct vector {
    float   v[4];           // Use array to allow mult by matrix.
} vector;

typedef struct point {
    float   p[4];           // Use array to allow mult by matrix.
} point;


/* Definition of rend_obj: renderable objects in the 3d pipeline */
typedef struct line {
    point       start;
    point       end;
    line_style  style;
} line;

typedef struct vertex_list {
    int         num_pts;    // in pts array
    point *     pts;        // array of points
} vertex_list;

typedef struct index_elt {          // index element
    int         pt_num;             // point from associated vertex list
    bool        hilight_edge;       // next edge should be hilighed
    bool        force_rgn_pt;       // force a region pt marker for this pt
} index_elt;

typedef struct polygon {
    vector          normal;
    int             num_pts;// in index array
    index_elt *     index;  // index into pts array. Note: not all elements of
                            // the pts array may be references
    int             rgn_pt_iter;    // if 0 no pt markers for this polygon
    vertex_list *   pts;
} polygon;

typedef struct face {
    vector      normal;     // defines the equation of the plane
    int         num_pts;
    index_elt * index;      // array of indecies into a vertex_list
                            // array of points
    int         rgn_pt_iter;    // if 0 no pt markers for this face
    bool        black;      // if true face draw in black, not illuminated
} face;

typedef struct face_list {
    int         num_faces;
    face *      faces;      // array of faces
} face_list;

typedef struct polyhedron {
    vertex_list pts;
    face_list   faces;
} polyhedron;

enum {
    REND_OBJ_PT,
    REND_OBJ_LINE,
    REND_OBJ_POLY,
    REND_OBJ_SOLID
};
typedef unsigned rend_type;

typedef struct visible_info {
    bool    mark;
    point   min;
    point   max;
} visible_info;

typedef struct hls_colour {
    float   h, l, s;
} hls_colour;

enum {
    USE_RGN_NONE,
    USE_RGN_GROUP,
    USE_RGN_SET
};
typedef unsigned use_rgn;

typedef struct region_info {
    use_rgn use_info;
    int     major_id;
    int     minor_id;
    int     data_row;
    int     data_col;
    int     set_num;
} region_info;

typedef struct rend_obj {
    visible_info    vis;        /* used in visible surface determination */
    rend_type       type;
    hls_colour      base_colour;
    bool            black_edges;
    region_info     rgn;
    union {
        point       pt;
        line        line;
        polygon     poly;
        polyhedron  solid;
    } obj;
} rend_obj;

typedef struct pt_edge {
    point       pt;
    bool        edge;           // if true draw the "vertical" edge for this pt
    bool        black_face;     // if true face swept by this edge will be black
    int         face_rgn_iter;  // iteration count for rgn markers on face swept
                                // by this edge;
    bool        force_rgn_pt;   // force this pt to be a rgn marker on the top
                                // and bottom faces
} pt_edge;

typedef struct text_info {
    text_pt *   text;
    point       centre;
    point       pt1;
    point       pt2;            // this value is filled in by rend_obj_add_sweep
} text_info;

typedef struct rend_obj_add {   // structure for adding an object by a sweep
    int             num_pts;
    pt_edge         *pts;       // array of pt_edge's
    text_info *     text;
    bool            text_on_bottom; // use the pts corresponding to the bottom
                                    // versions of the ones given in text
    float           height;
    bool            top_edges;
    int             top_rgn_iter;
    bool            bottom_edges;
    int             bottom_rgn_iter;
    hls_colour      base;
    bool            black_edges;
    region_info     rgn;
} rend_obj_add;

/* rend_obj macros */
#define _rend_obj_get_vis( obj ) (&(obj)->vis)
#define _rend_obj_get_type( obj ) ((obj)->type)
#define _rend_obj_black_edges( obj ) ((obj)->black_edges)

#define _polygon_get_pt( poly, num ) \
            ((poly).pts->pts[ (poly).index[ (num) ].pt_num ])
#define _polygon_get_num_pts( poly ) ((poly).num_pts)
#define _polygon_edge_hilight( poly, num ) ((poly).index[ num ].hilight_edge)
#define _polygon_force_rgn_pt( poly, num ) ((poly).index[ num ].force_rgn_pt)
#define _polygon_rgn_pt( poly, num ) ((poly).index[ num ].force_rgn_pt || \
                    ((poly).rgn_pt_iter != 0 && (num)%(poly).rgn_pt_iter == 0))

#define _line_get_style( ln ) ((ln)->style)


/* Definition of the list structure used to hold many rend_obj's */
typedef struct rend_list {
    int                 last;       // index of last entry used
    int                 size;       // size of array
    rend_obj **         list;       // array of pointers to rend_ojb
} rend_list;

typedef int     rend_list_ref;

/* rend_list macros */
#define _rend_list_is_empty( list ) ((list)->last < 0)
#define _rend_list_get_obj( list, ref ) ((list)->list[ref])
#define _rend_list_set_obj( list, ref, obj ) \
                            ((list)->list[ref] = (obj))
#define _rend_list_next( list, ref ) (*(ref) += 1)
#define _rend_list_prev( list, ref ) (*(ref) -= 1)
#define _rend_list_is_before_first( list, ref ) ((ref) < 0)
#define _rend_list_is_after_last( list, ref ) ((ref) > (list)->last)
#define _rend_list_first( list, ref ) (*(ref) = 0)
#define _rend_list_last( list, ref ) (*(ref) = (list)->last)


/* Illumination parameters used internally in the 3d pipeline */
typedef struct pipe_illum {
    bool    at_infinity;
    union {
        vector  light_dir;      // if at_infinity
        point   light_pt;       // if !at_infinity
    } source;
    float   pt_intensity;       // point source intensity
    float   amb_intensity;      // ambiant intensity
} pipe_illum;

/* memory allocation macros for the pipe_alloc function */
#define _pipenew( ptr, size ) ( ptr = pipe_alloc( sizeof( *(ptr) ) * (size) ) )

/* Defintion of the viewing parameters used by the 3d pipeline. See */
/* "Computer Graphics" by Foley et. al. sect. 6.2 pp. 237-242 */
typedef struct pipe_view {
    point       ViewRefPt;      /* Relitive to 3d world coords */
    vector      ViewPlNorm;     /* Relitive to 3d world coords */
    vector      VUp;            /* Relitive to 3d world coords */
    float       vmin;           /* window on view plane */
    float       vmax;
    float       umin;
    float       umax;
    point       ProjRefPt;      /* Relitive to viewing reference coords*/
    proj_type   type;
    float       FrontClip;      /* The front and back limits to the view vol. */
    float       BackClip;       /* rel. to viewing coords, n-axis */
} pipe_view;


/* dot product macros */
#define _dot_prod_vp( vect, pt ) \
    (   (pt).p[0]*(vect).v[0] + (pt).p[1]*(vect).v[1] + \
        (pt).p[2]*(vect).v[2] + (pt).p[3]*(vect).v[3] )

#define _dot_prod_vv( vect1, vect2 ) \
    (   (vect2).v[0]*(vect1).v[0] + (vect2).v[1]*(vect1).v[1] + \
        (vect2).v[2]*(vect1).v[2] + (vect2).v[3]*(vect1).v[3] )

#endif
