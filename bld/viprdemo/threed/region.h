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


#ifndef CGR_REGION_H
#define CGR_REGION_H
/*
 Description:
 ============
    Structures for the pick 'n poke chart stuff.

*/

#include "wpitypes.h"

typedef enum {
    CGR_RGN_RECT,
    CGR_RGN_LINE,
    CGR_RGN_POLY,
    CGR_RGN_PIE,
    CGR_RGN_ELLIPSE,
    CGR_RGN_ELLIPSE_BORDER,
    CGR_RGN_LINE_BOXES
} rgn_type;

typedef struct {
    unsigned short      size;           // size of the rgn_def
    rgn_type            type;
    short               set_num;        // -1 if no set info
    int                 major_id;
    int                 minor_id;
    int                 data_row;
    int                 data_col;
} rgn_info_def;

typedef struct {
    WPI_RECT                rect;
} rgn_rect_def;

typedef struct {
    WPI_RECT                rect;
} rgn_ellipse_def;

typedef struct {
    WPI_POINT               p1;
    WPI_POINT               p2;
} rgn_line_def;

typedef struct {
    int                     num_pts;
    WPI_POINT               pts[1];         // 'num_pts' in size
} rgn_poly_def;

typedef struct {
    WPI_POINT               pie_pts[4];     // p1, p2, arc1, arc2
} rgn_pie_def;

typedef union {
    rgn_rect_def        rect;
    rgn_line_def        line;
    rgn_poly_def        poly;
    rgn_pie_def         pie;
    rgn_ellipse_def     ellipse;
    rgn_line_def        line_boxes;
} rgn_data_def;

typedef struct {
    rgn_info_def        info;
    rgn_data_def        data;
} rgn_def;

typedef struct {
    unsigned short      rgn_offset;
    unsigned short      rgn_size;
    unsigned short      set_coll_offset;
} rgn_tag_def;

typedef struct {
    HBITMAP                 bmp;
    WPI_POINT               pt;
} rgn_marker_def;

typedef struct {
    short               num_bmps;
    rgn_marker_def      mark[1];
} rgn_mark_def;

typedef union {
    unsigned short          offset;
    WPI_POINT               pt;
} rgn_set_list_def;

typedef struct {
    BOOL                exact;
    short               num_items;
    short               num_used;
} rgn_set_header_def;

typedef struct {
    rgn_set_header_def  info;
    rgn_set_list_def    list[1];
} rgn_set_def;

typedef struct {
    short               num_sets;
} rgn_set_coll_header;

typedef struct {
    short               num_sets;
    short               num_used;
    rgn_set_def **      coll;
} rgn_set_coll;

typedef struct {
    rgn_set_coll_header info;
    unsigned short      offset[1];
} rgn_set_index;

#endif
