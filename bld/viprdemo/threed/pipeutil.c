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
 Utility routines used by the 3d graphics pipeline.

 The routines in this file use memory as follows:
    1. rend_obj's in the first half of the pipeline are kept in local
        memory since there will only be one of them being acted upon
        at any one time.
    2. At the end of the first half of the pipeline the rend_obj being acted
        upon is added to the rend_list but is first copied into memory
        managed by pipemem (see pipemem.c).
    3. The list part of the rend_list is kept in global memory as an array
        and is realloc'ed as necessary.

*/

#include "icgr.h"
#include <float.h>
#include <string.h>


#ifndef PROD
#define INCLUDE_DEBUG_ROUTINES
#endif

/* amount to grow a rend_list by when needed */
#define REND_LIST_GROW_SIZE     25


/* rend_list manipulation routines */
static void check_list_size(
/**************************/
    rend_list *     list
) {
    if (list->last + 1 >= list->size) {
        list->size += REND_LIST_GROW_SIZE;
        _grenew( list->list, list->size );
    }
}

extern bool rend_list_init(
/*************************/
    rend_list *     list
) {
    _gnew( list->list, REND_LIST_GROW_SIZE );
    list->last = -1;        // no entries in use

    if (list->list == NULL) {
        list->size = 0;
        return( FALSE );
    } else {
        list->size = REND_LIST_GROW_SIZE;
        return( TRUE );
    }
}

#if 0
/* macroed */
extern bool rend_list_is_empty(
/*****************************/
    rend_list *     list
) {
    return( list->last < 0 );
}
#endif

extern void rend_list_add(
/************************/
    rend_list *     list,
    rend_obj *      obj
) {
    check_list_size( list );

    list->last += 1;
    list->list[ list->last ] = obj;
}

extern void rend_list_free(
/*************************/
    rend_list *     list
) {
    _gfree( list->list );
}

extern void rend_list_sort(
/*************************/
    rend_list *     list,
    int             (*compare) (const rend_obj **, const rend_obj **)
) {
    qsort( list->list, list->last + 1, sizeof(rend_obj *), (int (*)(const void *, const void *))compare );
}

extern void rend_list_bin_insert(
/*******************************/
/* Uses a binary search to insert obj into a sublist of list. The sublist is */
/* given by start and end. */
/* NOTE: if start > end obj is inserted after end */
    rend_list *     list,
    rend_list_ref   start,
    rend_list_ref   end,
    rend_obj **     obj,
    int             (*compare) (const rend_obj **, const rend_obj **)
) {
    int             high;
    int             low;
    int             mid;
    int             comp_rc;

    low = start;
    high = end;
    mid = (low + high) / 2;

    while (low <= high ) {
        comp_rc = compare( (const rend_obj **)obj, (const rend_obj **)(list->list + mid) );
        if (comp_rc < 0) {
            high = mid - 1;
        } else if (comp_rc > 0) {
            low = mid + 1;
        } else {
            high = mid;
    /**/    break;
        }
        mid = (low + high) / 2;
    }

    /* Insert the elemtent after high. If high is -1 this means insert */
    /* the element before the first element and rend_list_insert handles */
    /* this case. */
    rend_list_insert( list, &high, *obj, FALSE );
}


extern void rend_list_insert(
/***************************/
    rend_list *     list,
    rend_list_ref * ref,
    rend_obj *      obj,
    bool            before
) {
    check_list_size( list );

    list->last += 1;

    if (before) {
        memmove( &(list->list[ *ref + 1 ]), &(list->list[ *ref ]),
                (list->last - *ref) * sizeof(rend_obj *) );
    } else {
        *ref += 1;
        memmove( &(list->list[ *ref + 1 ]), &(list->list[ *ref ]),
                (list->last - *ref) * sizeof(rend_obj *) );
    }

    list->list[ *ref ] = obj;
}

extern void rend_list_delete(
/***************************/
    rend_list *     list,
    rend_list_ref * ref
) {
    memmove( &(list->list[ *ref ]), &(list->list[ *ref + 1 ]),
            (list->last - *ref) * sizeof(rend_obj *) );
    list->last -= 1;
    if (*ref > list->last) {
        *ref = list->last;
    }
}

#if 0
/* macroed */
extern rend_obj * rend_list_get_obj(
/**********************************/
    rend_list *     list,
    rend_list_ref   ref
) {
    return( list->list[ ref ] );
}
#endif

#if 0
/* macroed */
extern void rend_list_set_obj(
/****************************/
    rend_list *     list,
    rend_list_ref   ref,
    rend_obj *      obj
) {
    list->list[ ref ] = obj;
}
#endif

#if 0
/* macroed */
extern bool rend_list_next(
/*************************/
    rend_list *     list,
    rend_list_ref * ref
) {
    *ref += 1;
    return( *ref > list->last );
}
#endif

#if 0
/* macroed */
extern bool rend_list_prev(
/*************************/
    rend_list *     list,
    rend_list_ref * ref
) {
    list = list;        // other representaions of rend_list may need list

    *ref -= 1;
    return( *ref < 0 );
}
#endif

#if 0
/* macroed */
extern bool rend_list_is_before_first(
/************************************/
    rend_list *     list,
    rend_list_ref   ref
) {
    list = list;        // other representations of rend_list may need list

    return( ref < 0 );
}
#endif

#if 0
/* macroed */
extern bool rend_list_is_after_last(
/**********************************/
    rend_list *     list,
    rend_list_ref   ref
) {
    return( ref > list->last );
}
#endif

#if 0
/* macroed */
extern void rend_list_first(
/**************************/
    rend_list *     list,
    rend_list_ref * ref
) {
    list = list;        // other representations of rend_list may need list

    *ref = 0;
}
#endif

#if 0
/* macroed */
extern void rend_list_last(
/*************************/
    rend_list *     list,
    rend_list_ref * ref
) {
    *ref = list->last;
}
#endif


/* vector manipulation routines */
extern vector cross_prod(
/***********************/
    vector  v,
    vector  w
) {
    vector  r;

    r.v[0] = v.v[1]*w.v[2] - v.v[2]*w.v[1];
    r.v[1] = v.v[2]*w.v[0] - v.v[0]*w.v[2];
    r.v[2] = v.v[0]*w.v[1] - v.v[1]*w.v[0];

    return( r );
}

extern float norm(
/****************/
/* This function computes the norm of a vector. Don't confuse this with */
/* the normal vector (of a plane) which is sometime called norm in this code */
    vector  v
) {
    return( sqrt(v.v[0]*v.v[0] + v.v[1]*v.v[1] + v.v[2]*v.v[2]) );
}

#if 0
/* macroed */
extern float dot_prod_vp(
/***********************/
/* Dot product of a vector and a point. Used to evaluate plane equations */
    vector  vect,
    point   pt
) {
    return( pt.p[0]*vect.v[0] + pt.p[1]*vect.v[1] + pt.p[2]*vect.v[2] +
                pt.p[3]*vect.v[3] );
}

extern float dot_prod_vv(
/***********************/
/* Dot product of two vectors */
/* bound. */
    vector  vect1,
    vector  vect2
) {
    return( vect1.v[0]*vect2.v[0] + vect1.v[1]*vect2.v[1] +
            vect1.v[2]*vect2.v[2] + vect1.v[3]*vect2.v[3] );
}
#endif

extern vector calculate_normal_vector(
/************************************/
/* This uses the techniques describe in "Computer Graphics" by Foley et. al. */
/* section 11.1.3, pp. 476, 477 */
    int     num_pts,
    void    *data,          // passed to get_next_pt only
    point   (* get_pt) ( void *, int point_num )
) {
    vector  normal;
    float   len;            // norm of normal
    int     count;
    point   curr_pt;
    point   next_pt;

    normal.v[0] = 0.;
    normal.v[1] = 0.;
    normal.v[2] = 0.;
    normal.v[3] = 0.;

    for (count = 0; count < num_pts; count++) {
        curr_pt = (*get_pt)( data, count );
        next_pt = (*get_pt)( data, (count + 1) % num_pts );
        normal.v[0] += (next_pt.p[2] + curr_pt.p[2]) *
                            (next_pt.p[1] - curr_pt.p[1]);
        normal.v[1] += (next_pt.p[2] + curr_pt.p[2]) *
                            (next_pt.p[0] - curr_pt.p[0]);
        normal.v[2] += (next_pt.p[1] + curr_pt.p[1]) *
                            (next_pt.p[0] - curr_pt.p[0]);
    }

    normal.v[0] *=  0.5;
    normal.v[1] *= -0.5;
    normal.v[2] *=  0.5;

    len = norm( normal );
    normal.v[0] /= len;
    normal.v[1] /= len;
    normal.v[2] /= len;


    next_pt = (*get_pt)( data, 0 );
    normal.v[3] = 0 - (normal.v[0]*next_pt.p[0] +
                        normal.v[1]*next_pt.p[1] +
                        normal.v[2]*next_pt.p[2]);

    return( normal );
}

extern vector point_diff(
/***********************/
    point   pt1,
    point   pt2
) {
    vector  v;
    int     i;

    for (i = 0; i < 4; i++) {
        v.v[i] = pt2.p[i] - pt1.p[i];
    }

    return( v );
}


/* matrix manipulation routines */
extern point mult_matrix_pt(
/**************************/
    float   matrix[4][4],
    point   in
) {
    point   out;
    int     i;
    int     j;

    for (i = 0; i < 4; i++) {
        out.p[i] = 0.;
        for (j = 0; j < 4; j++) {
            out.p[i] += matrix[i][j] * in.p[j];
        }
    }

    return( out );
}

/* line intersection */
static bool line_intersection(
/****************************/
/* Sets the param where the lines (as infinite lines) intersect. Returns FALSE*/
/* if the lines are parallel */
/* See Log Notes, set 5 */
/* NOTE: this is a 2d algorithm used for the projection of lines */
    wcoord  line1[2],
    wcoord  line2[2],
    float   *t,
    float   *k
) {
    wcoord  v1;         /* vector between the two points */
    wcoord  v2;
    float   d;

    v1.xcoord = line1[1].xcoord - line1[0].xcoord;
    v1.ycoord = line1[1].ycoord - line1[0].ycoord;
    v2.xcoord = line2[1].xcoord - line2[0].xcoord;
    v2.ycoord = line2[1].ycoord - line2[0].ycoord;

    d = v1.xcoord*v2.ycoord - v1.ycoord*v2.xcoord;
    if (fabs( d ) < FUZZY_ZERO) {
        /* lines are parallel so don't intersect */
        return( FALSE );
    } else {
        *t = ( v2.xcoord * (line1[0].ycoord - line2[0].ycoord) -
                v2.ycoord * (line1[0].xcoord - line2[0].xcoord) ) / d;
        if (fabs( v2.xcoord ) < fabs( v2.ycoord )) {
            *k = (line1[0].ycoord + *t *v1.ycoord - line2[0].ycoord)/v2.ycoord;
        } else {
            *k = (line1[0].xcoord + *t *v1.xcoord - line2[0].xcoord)/v2.xcoord;
        }

        /* the lines intersect where the parameter are t and k resp. so */
        /* the line segments intersects if both t and k are in [0,1] */
        return( TRUE );
    }
}

extern bool proj_line_intersection(
/*********************************/
/* Determine if the projection of line1 and line2 into the x-y plane intersect*/
/* Returns true if the line segments line1 and line2 intersect. */
    point   line1[2],
    point   line2[2]
) {
    wcoord  ln1[2];
    wcoord  ln2[2];
    float   t,k;

    /* perform the projection */
    ln1[0].xcoord = line1[0].p[0];
    ln1[0].ycoord = line1[0].p[1];
    ln1[1].xcoord = line1[1].p[0];
    ln1[1].ycoord = line1[1].p[1];
    ln2[0].xcoord = line2[0].p[0];
    ln2[0].ycoord = line2[0].p[1];
    ln2[1].xcoord = line2[1].p[0];
    ln2[1].ycoord = line2[1].p[1];

    if (line_intersection( ln1, ln2, &t, &k )) {
        return( 0. <= t && t <= 1. && 0. <= k && k <= 1. );
    } else {
        /* the lines are parallel so they don't intersect */
        return( FALSE );
    }
}


/* RGB to/from HLS conversions */
extern void rgb_to_hls(
/*********************/
/* This is base on the procedure given in "Computer Graphics" by Foley et. */
/* al. p. 595 */
    COLORREF    rgb,
    hls_colour  *hls
) {
    float       r, g, b;
    float       max, min;
    float       delta;

    r = _wpi_getrvalue( rgb ) / 255.;
    g = _wpi_getgvalue( rgb ) / 255.;
    b = _wpi_getbvalue( rgb ) / 255.;

    max = _max( _max( r, g ), b );
    min = _min( _min( r, g ), b );

    /* set the lightness */
    hls->l = (max + min) / 2.;

    /* set the saturation */
    if (max == min) {       /* anchromatic case because r==b==g */
        hls->s = 0;
        hls->h = 0;         /* UNDEFINDED */
    } else {                /* chromatic case */
        if (hls->l < 0.5) {
            hls->s = (max - min) / (max + min);
        } else {
            hls->s = (max - min) / (2 - max - min);
        }

        /* set the hue */
        delta = max - min;

        if (r == max) {         // colour is between yellow and magenta
            hls->h = (g - b) / delta;
        } else if (g == max) {  // colour is between cyan and yellow
            hls->h = 2. + (b - r) / delta;
        } else {                // colour is between magenta and cyan
            hls->h = 4. + (r - g) / delta;
        }

        /* convert h to degrees */
        hls->h *= 60.;

        /* make sure h is positive */
        if (hls->h < 0.) {
            hls->h += 360.;
        }
    }
}

static float value(
/****************/
    float   n1,
    float   n2,
    float   hue
) {
    if (hue > 360.) {
        hue -= 360.;
    } else if (hue < 0.) {
        hue += 360.;
    }

    if (hue < 60.) {
        return( n1 + (n2-n1) * hue / 60. );
    } else if (hue < 180.) {
        return( n2 );
    } else if (hue < 240.) {
        return( n1 + (n2-n1) * (240.-hue) / 60. );
    } else {
        return( n1 );
    }
}

extern void hls_to_rgb(
/*********************/
    COLORREF    *rgb,
    hls_colour  hls
) {
    float       r, g, b;
    float       m1, m2;

    if (hls.l <= .5) {
        m2 = hls.l * (1 + hls.s);
    } else {
        m2 = hls.l + hls.s - hls.l * hls.s;
    }
    m1 = 2. * hls.l - m2;

    if (hls.s == 0.) {              // anchromatic case
        r = hls.l;
        g = hls.l;
        b = hls.l;
    } else {                        // chromatic case
        r = value( m1, m2, hls.h + 120. );
        g = value( m1, m2, hls.h );
        b = value( m1, m2, hls.h - 120. );
    }

    *rgb = _wpi_getrgb( r * 255, g * 255, b * 255 );
}


/* rectangle, ray intersection */
static bool ray_line_inter(
/*************************/
    float       x1,             // point 1 of the line
    float       y1,
    float       x2,             // point 2
    float       y2,
    wcoord      ray_start,
    wcoord      ray_dir,
    wcoord      *inter
) {
    wcoord      ln1[2];
    wcoord      ln2[2];
    float       t,k;

    ln1[0].xcoord = x1;
    ln1[0].ycoord = y1;
    ln1[1].xcoord = x2;
    ln1[1].ycoord = y2;
    ln2[0] = ray_start;
    ln2[1].xcoord = ray_start.xcoord + ray_dir.xcoord;
    ln2[1].ycoord = ray_start.ycoord + ray_dir.ycoord;

    if (line_intersection( ln1, ln2, &t, &k )) {
        /* don't accept if k is close to 0 since that means the ray */
        /* starts on the line */
        if ( FUZZY_ZERO < k ) {
            inter->xcoord = ray_start.xcoord + k*ray_dir.xcoord;
            inter->ycoord = ray_start.ycoord + k*ray_dir.ycoord;
            return( TRUE );
        } else {
            return( FALSE );
        }
    } else {
        return( FALSE );
    }
}

extern bool rect_ray_inter(
/*************************/
    wcoord      rect[2],
    wcoord      ray_start,
    wcoord      ray_dir,
    wcoord      *inter
) {
    float       len;

    /* normalize ray_dir */
    len = sqrt( ray_dir.xcoord*ray_dir.xcoord + ray_dir.ycoord*ray_dir.ycoord );
    ray_dir.xcoord /= len;
    ray_dir.ycoord /= len;

    /* is the ray more vertical than horizontal? */
    if( fabs( ray_dir.ycoord ) > fabs( ray_dir.xcoord ) ) {
        /* check against the horizontal edges of rect */
        if (ray_line_inter( rect[0].xcoord, rect[0].ycoord,
                            rect[1].xcoord, rect[0].ycoord,
                            ray_start, ray_dir, inter )) {
            return( TRUE );
        }
        if (ray_line_inter( rect[1].xcoord, rect[1].ycoord,
                            rect[0].xcoord, rect[1].ycoord,
                            ray_start, ray_dir, inter )) {
            return( TRUE );
        }
    } else {
        /* check against the vertical edges of rect */
        if (ray_line_inter( rect[1].xcoord, rect[0].ycoord,
                            rect[1].xcoord, rect[1].ycoord,
                            ray_start, ray_dir, inter )) {
            return( TRUE );
        }
        if (ray_line_inter( rect[0].xcoord, rect[0].ycoord,
                            rect[0].xcoord, rect[1].ycoord,
                            ray_start, ray_dir, inter )) {
            return( TRUE );
        }
    }

    /* no intersection on any of the edges so set inter arbitrarily */
    inter->xcoord = rect[0].xcoord;
    inter->ycoord = rect[0].ycoord;

    return( FALSE );
}



#ifdef INCLUDE_DEBUG_ROUTINES

/* DEBUG ROUTINES */
#include <stdio.h>

extern void dbg_print_list(
/*************************/
    rend_list *     list
) {
    FILE *          fp;
    int             curr_obj;
    char            path[ _MAX_PATH ];

    dbg_get_filename( path );

    fp = fopen( path, "w" );
    if (fp == NULL) {
        return;
    }

    fprintf( fp, "list address: %p\n", (void *) list );

    for (curr_obj = 0; curr_obj <= list->last; curr_obj++) {
        fprintf( fp, "OBJECT NUMBER %d--------------------------"
                        "-------------------\n", curr_obj );
        dbg_print_one_obj( fp, list->list[ curr_obj ], FALSE );
    }

    fclose( fp );
}

extern void dbg_print_list_long(
/******************************/
    rend_list *     list
) {
    FILE *          fp;
    int             curr_obj;
    char            path[ _MAX_PATH ];

    dbg_get_filename( path );

    fp = fopen( path, "w" );
    if (fp == NULL) {
        return;
    }

    fprintf( fp, "list address: %p\n", (void *) list );

    for (curr_obj = 0; curr_obj <= list->last; curr_obj++) {
        fprintf( fp, "OBJECT NUMBER %d--------------------------"
                        "-------------------\n", curr_obj );
        dbg_print_one_obj( fp, list->list[ curr_obj ], TRUE );
    }

    fclose( fp );
}

#endif
