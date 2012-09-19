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


#ifndef CGR_ROWSCALE_H
#define CGR_ROWSCALE_H
/*
 Description:
 ============
     This file defines the structure which describes the row axis scaling
     information. If there are bars on a bar chart, then they determine
     the scaling information, otherwise simple scaling is based on the
     number of data points.

*/

typedef struct horz_scale {             // horizontal scale info
    float       start;
    float       width;
    float       next;
} horz_scale;

typedef struct row_scale {
    /**** this info is for bar charts only *****/
    horz_scale          cluster;
    horz_scale          bar;
    /* this info is for bar/area charts */
    bool                bar_areas;      // TRUE: bar/areas on the chart
} row_scale;

typedef struct col_scale {          // used for 3d bar charts
    horz_scale          col;
} col_scale;

typedef struct chart_cube {         // "cube" in which 3d bla charts are draw
    w3coord             start;
    w3coord             end;
} chart_cube;

#endif
