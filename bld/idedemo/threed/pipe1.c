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
 This file contains the first half of the 3d pipeline. It consists of the
 following components:
        1. Viewing Transformation (and support routines).
        2. Clipping (including back face culling).
        3. Addition to the intermediate rend_list.

 See "Computer Graphics", by Foley et. al. chapters 5 and 6 to understand how
 the viewing transformation and clipping work. The clipping used here is
 clipping in homogeneous coordinates. The viewing transformation for perspective
 projections includes the perspective transformation.
 See pp. 663,664 for back face culling.

*/

#include "icgr.h"


static float ViewTrans[4][4];


/* this type allows dynamic allocation of 4x4 arrays of floats */
typedef struct matrix4 {
    float   m[4][4];
} matrix4;

static void calc_text_pt(
/***********************/
    text_info *     text
) {
    point *         use_pt;
    text_pt *       tx_pt;
    wcoord          cen;

    tx_pt = text->text;             // shorthand

    /* transform the centre and pt1 */
    text->centre = mult_matrix_pt( ViewTrans, text->centre );
    text->pt1 = mult_matrix_pt( ViewTrans, text->pt1 );
    if( text->pt1.p[2] - text->centre.p[2] < FUZZY_ZERO ) {
        use_pt = &text->pt1;
    } else {
        text->pt2 = mult_matrix_pt( ViewTrans, text->pt2 );
        use_pt = &text->pt2;
    }

    project_pt( text->centre, &cen.xcoord, &cen.ycoord );
    project_pt( *use_pt, &tx_pt->pt.xcoord, &tx_pt->pt.ycoord );
    tx_pt->direction.xcoord = tx_pt->pt.xcoord - cen.xcoord;
    tx_pt->direction.ycoord = tx_pt->pt.ycoord - cen.ycoord;
}


extern void pipe3d_add(
/*********************/
/* perform the operations of the first half of the pipeline on obj then add */
/* it to the list of objects */
    rend_obj *  obj,
    text_info * text,
    bool        disp_now
) {
    if( obj == NULL ) {
        return;
    }

    rend_obj_transform( obj, ViewTrans );
    /* Add clipping function here */
    rend_obj_homo_to_3d( obj );
    rend_obj_calculate_normal( obj );
    if( disp_now ) {
        draw_illuminated_obj( obj );
    } else {
        rend_obj_add_to_list( obj, Pipe3dList );
    }

    /* if the caller whats text placement information */
    if (text != NULL) {
        calc_text_pt( text );
    }

    /* free obj from local memory */
    rend_obj_lfree( obj );
}

extern void pipe3d_trans_pt(
/**************************/
    point *     pt
) {
    *pt = mult_matrix_pt( ViewTrans, *pt );
    point_homo_to_3d( pt );
}


#define KronikerDelta( i, j ) ((i)==(j)) ? 1.0 : 0.0

/* Routines to fill in the various type of matricies */
static void identity_matrix(
/**************************/
    float   m[4][4]
) {
    int     i;
    int     j;

    for (i=0; i < 4; i++) {
        for (j=0; j < 4; j++) {
            m[i][j] = KronikerDelta( i, j );
        }
    }
}

static float * trans_matrix(
/**************************/
    float   m[4][4],
    float   dx,
    float   dy,
    float   dz
) {
    identity_matrix( m );
    m[0][3] = dx;
    m[1][3] = dy;
    m[2][3] = dz;

    return( m );
}

static float * scale_matrix(
/**************************/
    float   m[4][4],
    float   sx,
    float   sy,
    float   sz
) {
    identity_matrix( m );
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;

    return( m );
}

static float * shear_xy_matrix(
/*****************************/
    float   m[4][4],
    float   shx,
    float   shy
) {
    identity_matrix( m );
    m[0][2] = shx;
    m[1][2] = shy;

    return( m );
}

static void matrix_mult(
/**********************/
    float   m[4][4],
    float   n[4][4],
    float   ans[4][4]
) {
    int     i;
    int     j;
    int     k;

    for (i=0; i < 4; i++) {
        for (j=0; j < 4; j++) {
            ans[i][j] = 0.0;
            for (k=0; k < 4; k++) {
                ans[i][j] += m[i][k] * n[k][j];
            }
        }
    }
}

static vector normalize(
/**********************/
/* returns a normalized version of the input vector */
    vector      in
) {
    vector      out;
    float       len;        /* norm of input vecotr */

    len = norm( in );
    out.v[0] = in.v[0] / len;
    out.v[1] = in.v[1] / len;
    out.v[2] = in.v[2] / len;
    out.v[3] = 0.;

    return( out );
}

static vector cross_prod(
/***********************/
    vector      v,
    vector      w
) {
    vector      prod;

    prod.v[0] = v.v[1]*w.v[2] - v.v[2]*w.v[1];
    prod.v[1] = v.v[2]*w.v[0] - v.v[0]*w.v[2];
    prod.v[2] = v.v[0]*w.v[1] - v.v[1]*w.v[0];
    prod.v[3] = 0.;

    return( prod );
}

static void form_rotation_matrix(
/*******************************/
    float       r[4][4],
    vector      vpn,            /* View plane normal */
    vector      vup             /* VUp vector */
) {
    vector      *rvects;        /* 3 vectors */
    int         i, j;

    /* allocate the 3 vectors */
    _new( rvects, 3 );

    rvects[2] = normalize( vpn );
    rvects[0] = normalize( cross_prod( vup, rvects[2] ) );
    rvects[1] = cross_prod( rvects[2], rvects[0] );

    identity_matrix( r );
    for (i=0; i < 3; i++) {
        for (j=0; j < 3; j++) {
            r[i][j] = rvects[i].v[j];
        }
    }

    /* free the 3 vectors */
    _free( rvects );
}

static void persp_trans_matrix(
/*****************************/
    float       p[4][4],
    float       zmin
) {
    identity_matrix( p );
    p[2][2] = 1. / (1. + zmin);
    p[2][3] = - zmin / (1+zmin);
    p[3][2] = -1.;
    p[3][3] = 0.;
}

static void set_view_trans_mat(
/*****************************/
/* This function examines the values in view and fills in  the ViewTrans */
/* matrix. This will be either the Nper' or Npar matrix described in */
/* sections 6.5.1 and 6.5.2. The Nper' matrix include the perspective  */
/* transformation matrix. */
    pipe_view   *view
) {
    matrix4     *mat1,*mat2,*mat3;  /* temporary matricies */
    vector      dop;                /* direction of projection after */
                                    /* trans and rotation */
    float       vrpz1;              /* vprz' used only for perspective */

    /* allocate the temporary matricies */
    _new( mat1, 1 );
    _new( mat2, 1 );
    _new( mat3, 1 );

    /* these matrices are common to both types of projection */
    trans_matrix( mat1->m, -view->ViewRefPt.p[0], -view->ViewRefPt.p[1],
                 -view->ViewRefPt.p[2] );

    form_rotation_matrix( mat2->m, view->ViewPlNorm, view->VUp );
    matrix_mult( mat2->m, mat1->m, mat3->m );
    /* we can reuse mat2->m and mat1->m now */

    if (view->type == PROJ_PARALLEL) {
    /* form the Npar matrix */
        dop.v[0] = (view->umax + view->umin) / 2.0 - view->ProjRefPt.p[0];
        dop.v[1] = (view->vmax + view->vmin) / 2.0 - view->ProjRefPt.p[1];
        dop.v[2] = -view->ProjRefPt.p[2];
        dop.v[3] = 0;
        shear_xy_matrix( mat1->m, -dop.v[0]/dop.v[2], -dop.v[1]/dop.v[2] );
        matrix_mult( mat1->m, mat3->m, mat2->m );
        /* reuse 1 and 3 */

        trans_matrix( mat1->m, (-view->umax - view->umin) / 2.0,
                            (-view->vmax - view->vmin) / 2.0,
                            -view->FrontClip );
        matrix_mult( mat1->m, mat2->m, mat3->m );
        /* reuse 1 and 2 */

        scale_matrix( mat1->m, 2.0 / (view->umax - view->umin),
                            2.0 / (view->vmax - view->vmin),
                            1. / (view->FrontClip - view->BackClip) );
        matrix_mult( mat1->m, mat3->m, ViewTrans );
    } else {
    /* form the Nper' matrix including the perspective transformation */
        trans_matrix( mat1->m, -view->ProjRefPt.p[0],
                            -view->ProjRefPt.p[1], -view->ProjRefPt.p[2] );
        matrix_mult( mat1->m, mat3->m, mat2->m );
        /* reuse 1 and 3 */

        /* This is the same shear as for Npar. See p. 269 for why. */
        dop.v[0] = (view->umax + view->umin) / 2.0 - view->ProjRefPt.p[0];
        dop.v[1] = (view->vmax + view->vmin) / 2.0 - view->ProjRefPt.p[1];
        dop.v[2] = -view->ProjRefPt.p[2];
        dop.v[3] = 0;
        shear_xy_matrix( mat1->m, -dop.v[0]/dop.v[2], -dop.v[1]/dop.v[2] );
        matrix_mult( mat1->m, mat2->m, mat3->m );
        /* reuse 1 and 2 */

        vrpz1 = -view->ProjRefPt.p[2];
        scale_matrix( mat1->m,
            2.0*vrpz1 / ((view->umax - view->umin)*(vrpz1 + view->BackClip)),
            2.0*vrpz1 / ((view->vmax - view->vmin)*(vrpz1 + view->BackClip)),
            -1.0 / (vrpz1 + view->BackClip) );
        matrix_mult( mat1->m, mat3->m, mat2->m );
        /* reuse 1 and 3 */

        persp_trans_matrix( mat1->m,
             -(vrpz1 + view->FrontClip)/(vrpz1 + view->BackClip) );

        matrix_mult( mat1->m, mat2->m, ViewTrans );
    }

    /* free the temporary matrices */
    _free( mat1 );
    _free( mat2 );
    _free( mat3 );
}

extern void pipe3d_init(
/**********************/
    pipe_view   *view,
    pipe_illum  *illum,
    bool        all_poly_convex
) {
    set_view_trans_mat( view );
    set_illumination( illum );
    set_poly_convex_info( all_poly_convex );

    _new( Pipe3dList, 1 );
    rend_list_init( Pipe3dList );
}

extern void pipe3d_shutdown(
/**************************/
    void
) {
    rend_list_free( Pipe3dList );
    _free( Pipe3dList );

    pipe_free_all();
}
