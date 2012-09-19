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


#ifndef CGR_CONST_H
#define CGR_CONST_H
/*
 Description:
 ============
     This file defines the various constants used by chart graphics.
     of BGP.
*/

#define PI                  3.1415927
#define _deg_to_rad( deg )  ((2*PI) * (deg) / 360.)

#define DEFAULT_PALETTE_SIZE 16

#if 0           /***** NONE of these should be necessary anymore! *****/
#define CGR_CHAR_BASE   .625    // ratio of char height to base
#define CGR_CHAR_SPACE  -.2     // character spacing
/* note the fudge factor in next line: needed since spacing gets rounded up
   in pixel coords, and hence spacing gets off */
#define CGR_CHAR_WIDTH  (CGR_CHAR_BASE + CGR_CHAR_SPACE + .04 )
#define _text_width( len, size ) ( (size) * ( ( (len) - 1 ) * CGR_CHAR_WIDTH \
#define CGR_SLANT_HEIGHT        .707    // % height of slanted text (1/sqrt(2))
                                                         + CGR_CHAR_BASE ) )
#endif

#define _window_width( x ) ((x) * Window_width )
#define _window_height( x ) ((x) * Window_height )

typedef enum {                          // text direction descriptor
    TEXT_HORIZONTAL,                    // text across
    TEXT_VERTICAL                      // text vertical, top to bottom
} text_dir;

typedef enum {                          // title descriptor
    CHART_TITLE                 = 1,    // the chart title
    CHART_SUBTITLE,                     // the chart subtitle
    CHART_FOOTNOTE,                     // the chart footnote
} title_type;

#define BAR_OVERLAP             .5      // 50% bar overlap

#define LINE_CURVE_COUNT        120     // max 120 lines to a B spline curve

#define MAX_ERR_LEN             100     // maximum error msg length

#define CGR_MAX_SCALE_INCS      30      // max scale increments for manual

#define CGR_MARKER_SIZE         .028    // default marker size (%chart height)
#define CGR_RDR_MARKER_SIZE     .021    // default marker size (%chart height)
#define CGR_MARKER_POINTS_MAX   10      // max size of markers in points

#define CGR_3D_PIE_RGN_ITER     4

#define BUBBLE_SIZE             .1      // % of chart max. bubble will take up
                                        // in a bubble chart

#define MAX_LABEL_LEN           80      // maximum label length

/* thresholds for centring labels on a pie chart */
/* if a unit vector is within this much of the appropriate axis then the text */
/* is centred */
#define CGR_HCENTRE_LIMIT       .3
// #define      CGR_VCENTRE_LIMIT       .3

/* Angles in quadrant I for centring bla chart labels */
#define CGR_HCENTRE_LIMIT_LBL   _deg_to_rad( 60 )
#define CGR_VCENTRE_LIMIT_LBL   _deg_to_rad( 5 )

/* base zoom values for 3d charts (i.e. 100% zoom) */
#define CGR_PIE_3D_ZOOM         2.5
#define CGR_WIRE_3D_ZOOM        .80
#define CGR_BLA_3D_ZOOM         1.13

/* lighting distances for various 3d charts */
#define CGR_PIE_3D_LIGHT        2.
#define CGR_BLA_3D_LIGHT_MIN    2.
#define CGR_BLA_3D_LIGHT_MAX    20.

/* sizes for 3d bar charts */
#define CGR_AUTO_3D_BARSIZE     .5      // % size of 3d bars

/* height of 3d pie charts */
#define CGR_PIE_3D_HEIGHT       .2

/* legend width constants */
#define LEGEND_GAP1             .03     // chart to symbol gap
#define LEGEND_GAP2             .01     // symbol to text gap
#define LEGEND_GAP3             .015    // legend to frame gap
#define LEGEND_GAP4             .01     // line to line gap
#define LEGEND_GAP5             .01     // legend frame to other stuff

/* decorator gaps */
#define CGR_TITLE_GAP1          .04     // title to chart frame
#define CGR_TITLE_GAP2          .2      // % char height gap (title to title)
#define CGR_3D_ROW_GAP1         .1      // row labels to frame in 3d bla chart
#define CGR_3D_ROW_TICK_WIDTH   .015
#define CGR_3D_AXIS_LABEL_GAP   .05     // gap between row/col/scale labels and
                                        // axis label
#define CGR_SCALE_GAP1          .025    // scale labels to axis
#define CGR_ROW_GAP1            CGR_SCALE_GAP1  // should be the same for view.c
#define CGR_LABEL_GAP1          .01     // col/row label to inside stuff
#define CGR_VALUE_GAP1          .01     // gap between values and points
#define CGR_VALUE_GAP2          .01     // gap between values and graph edge
#define CGR_PIE_GAP1            .1      // gap between lbl and pie slice
#define CGR_PIE_GAP2            .1      // vert. gap between row and value label
#define CGR_PIE_GAP3            .1      // horz. gap between row and value
#define CGR_PIE_3D_GAP1         .1      // gap between lbl and slice in 3d pie

#define CGR_CHART_GAP           .05     // space around chart and edge

#define FUZZY_ZERO ( 0.00001 )          // for clipping algorithm
                                        // and for pie drawing
                                        // and 3d stuff

#define COMPACT_SIZE            .8     // % of scale size for compact label
#define COMPACT_GAP1            .005    // compact label to whats below

#define CGR_REAL_BLACK          0       // use this as CGR clr to get black
                                        // regardless of the palette
#endif
