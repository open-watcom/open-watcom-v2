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


#ifndef CGR_FILE_H

#define cmd(x,y) x
#define lcmd(x,y) ,x

typedef enum {

#else

#undef cmd
#define cmd(x,y) y
#undef lcmd
#define lcmd(x,y)

static char *Commands[]={

#endif

/* Common chart information */
cmd(    CMD_TYPE,                       "ty"    ),
cmd(    CMD_TITLE_TEXT,                 "tt"    ),
cmd(    CMD_TITLE_DEF,                  "td"    ),
cmd(    CMD_3D_CHART,                   "3d"    ),
cmd(    CMD_OLD_LEGEND,                 "lg"    ),
cmd(    CMD_LEGEND,                     "ln"    ),
cmd(    CMD_LEGEND_TYPE,                "lt"    ),
cmd(    CMD_LEGEND_XOFF,                "lx"    ),
cmd(    CMD_LEGEND_YOFF,                "ly"    ),
cmd(    CMD_LEGEND_FRAMED,              "lf"    ),
cmd(    CMD_LEGEND_DEF,                 "ld"    ),
cmd(    CMD_LEGEND_HORIZ,               "lh"    ),
cmd(    CMD_LEGEND_POS,                 "lp"    ),
cmd(    CMD_GROUP_BY,                   "gb"    ),
cmd(    CMD_DRAW,                       "dr"    ),
cmd(    CMD_PRECISION_SPECIFY,          "dp"    ),  // old data/scale prec
cmd(    CMD_PALETTE,                    "pl"    ),
cmd(    CMD_PALETTE_COLOR,              "co"    ),
cmd(    CMD_DRAW_ITEM_BEGIN,            "db"    ),
cmd(    CMD_DATA,                       "data"  ), // marks start of data
cmd(    CMD_SUBTITLE_TEXT,              "ut"    ),
cmd(    CMD_SUBTITLE_DEF,               "ud"    ),
cmd(    CMD_FOOTNOTE_TEXT,              "ft"    ),
cmd(    CMD_FOOTNOTE_DEF,               "fd"    ),
cmd(    CMD_DATA_PREC_SPEC,             "pd"    ),

/* Bla chart - specific information */
cmd(    CMD_ARRANGE,                    "ag"    ),
cmd(    CMD_VERTICAL_CHART,             "vc"    ),
cmd(    CMD_FRAME,                      "fr"    ),
cmd(    CMD_ROW_RANGE,                  "rr"    ),
cmd(    CMD_DATA_SUBSET,                "dd"    ),
cmd(    CMD_ROW_LABEL_TEXT,             "rl"    ),
cmd(    CMD_COL_LABEL_TEXT,             "cl"    ),
cmd(    CMD_COL2_LABEL_TEXT,            "l2"    ),
cmd(    CMD_ZCOL_LABEL_TEXT,            "zl"    ),
cmd(    CMD_ROW_LABEL_DEF,              "rd"    ),
cmd(    CMD_COL_LABEL_DEF,              "cd"    ),
cmd(    CMD_COL2_LABEL_DEF,             "d2"    ),
cmd(    CMD_ZCOL_LABEL_DEF,             "zd"    ),
cmd(    CMD_ROW_TEXT_SIZE,              "rs"    ),
cmd(    CMD_COL_TEXT_SIZE,              "cs"    ),
cmd(    CMD_ROW_GRID,                   "rg"    ),
cmd(    CMD_Z_GRID,                     "zg"    ),
cmd(    CMD_COL_GRID,                   "cg"    ),
cmd(    CMD_COL2_GRID,                  "g2"    ),
cmd(    CMD_ROW_TICK,                   "rt"    ),
cmd(    CMD_Z_TICK,                     "zt"    ),
cmd(    CMD_COL_TICK,                   "ct"    ),
cmd(    CMD_COL2_TICK,                  "t2"    ),
cmd(    CMD_BAR_OVERLAP,                "ba"    ),
cmd(    CMD_BAR_WIDTH,                  "bw"    ),
cmd(    CMD_FRAME_COLOR,                "cc"    ),
cmd(    CMD_ZERO_AXES,                  "za"    ),
cmd(    CMD_RADAR_XAXIS,                "rx"    ),
cmd(    CMD_RADAR_YAXIS,                "ry"    ),
cmd(    CMD_RADAR_SIZE,                 "rz"    ),
cmd(    CMD_RADAR_ORIENT,               "ro"    ),
cmd(    CMD_STOCK_LEGEND,               "sl"    ),
cmd(    CMD_STOCK_TYPE,                 "sy"    ),
cmd(    CMD_STOCK_U_WIDTH,              "su"    ),
cmd(    CMD_STOCK_B_WIDTH,              "sb"    ),

/* Pie chart - specific information */
cmd(    CMD_COLUMN,                     "cm"    ),
cmd(    CMD_TEXT_SIZE,                  "ts"    ), // not used any more
cmd(    CMD_TEXT_FONT,                  "tf"    ), // not used any more
cmd(    CMD_VALUES_POSITION,            "vo"    ), // attribute of values
cmd(    CMD_PIE_SIZE,                   "ps"    ),
cmd(    CMD_ANGLE,                      "pa"    ),
cmd(    CMD_OTHER_SLICE,                "os"    ),
cmd(    CMD_OTHER_LABEL,                "ol"    ),
cmd(    CMD_OTHER_VALUE,                "ov"    ),
cmd(    CMD_OTHER_ATTR,                 "oa"    ),

/* Common attribute information */
cmd(    CMD_ATTRIBUTES,                 "at"    ), // marks start of attrs
cmd(    CMD_ATTR,                       "ar"    ), // marks each attr
cmd(    CMD_COLOR,                      "cr"    ),
cmd(    CMD_FILL_STYLE,                 "fs"    ),
cmd(    CMD_DISPLAY,                    "ds"    ),
cmd(    CMD_ORDER,                      "or"    ),

/* Bla attribute information */
cmd(    CMD_PRECISION,                  "pr"    ),
cmd(    CMD_BLA_TYPE,                   "bt"    ),
cmd(    CMD_LINE_STYLE,                 "ls"    ),
cmd(    CMD_LINE_WIDTH,                 "lw"    ),
cmd(    CMD_MARKER_STYLE,               "ms"    ),
cmd(    CMD_X_GROUP,                    "xg"    ),
cmd(    CMD_Y_GROUP,                    "yg"    ),
cmd(    CMD_Y2_SCALE,                   "y2"    ),
cmd(    CMD_XY_TYPE,                    "xy"    ),
cmd(    CMD_START_GROUP,                "sg"    ),
cmd(    CMD_LENGTH_GROUP,               "eg"    ),
cmd(    CMD_BUBBLE_X_GROUP,             "bx"    ),
cmd(    CMD_BUBBLE_Y_GROUP,             "by"    ),
cmd(    CMD_BUBBLE_VALUE_GROUP,         "bv"    ),

/* Pie attribute information */
cmd(    CMD_SLICE_CUT,                  "sc"    ),

/* Draw Item attribute information */
cmd(    CMD_DRAW_TYPE,                  "dt"    ),
cmd(    CMD_DRAW_DELETED,               "dk"    ),
cmd(    CMD_ARROW_TOPLEFT,              "a1"    ),
cmd(    CMD_ARROW_BOTTOMRIGHT,          "a2"    ),
cmd(    CMD_ARROW_LINESTYLE,            "as"    ),
cmd(    CMD_ARROW_LINEWIDTH,            "aw"    ),
cmd(    CMD_ARROW_COLOR,                "ac"    ),
cmd(    CMD_ARROW_HEAD_WIDTH,           "ah"    ),
cmd(    CMD_ARROW_HEAD_LENGTH,          "al"    ),
cmd(    CMD_ARROW_HEAD_TYPE,            "ay"    ),
cmd(    CMD_DTEXT_TEXT,                 "dx"    ),
cmd(    CMD_DTEXT_TEXT_INFO,            "di"    ),
cmd(    CMD_DTEXT_TOPLEFT,              "dl"    ),
cmd(    CMD_DTEXT_FRAMED,               "df"    ),

/* Scale Information */
cmd(    CMD_SCALE_ON,                   "so"    ),
cmd(    CMD_SCALE_AUTO,                 "sa"    ),
cmd(    CMD_SCALE_MANUAL,               "sm"    ),
cmd(    CMD_SCALE_DEF,                  "sd"    ),
cmd(    CMD_SCALE_PREFIX,               "sp"    ),
cmd(    CMD_SCALE_SUFFIX,               "ss"    ),
cmd(    CMD_SCALE_FORMAT_DEF,           "sf"    ),
cmd(    CMD_SCALE_PRO,                  "st"    ),
cmd(    CMD_Y2SCALE_PRO,                "s2"    ),
cmd(    CMD_SCALE_PREC_SPEC,            "py"    ),

/* X Scale/Labels Information -- X scale if a XY chart, Labels if a Bla chart */
cmd(    CMD_LABELS,                     "lb"    ),
cmd(    CMD_XSCALE_AUTO,                "xa"    ),
cmd(    CMD_XSCALE_MANUAL,              "xm"    ),
cmd(    CMD_LABELS_DEF,                 "xd"    ),
cmd(    CMD_LABELS_PREFIX,              "xp"    ),
cmd(    CMD_LABELS_SUFFIX,              "xs"    ),
cmd(    CMD_LABELS_FORMAT_DEF,          "xf"    ),
cmd(    CMD_XSCALE_PREC_SPEC,           "px"    ),

/* Y2 Scale Information */
/* Description of Y2 is same as that of Y1.  Hence no additional commands */
cmd(    CMD_Y2SCALE_AUTO,               "ya"    ),
cmd(    CMD_Y2SCALE_MANUAL,             "ym"    ),

/* Z Labels Information */
cmd(    CMD_Z_LABELS,                   "zll"   ),
cmd(    CMD_Z_LABELS_DEF,               "zld"   ),

/* Data labels information -- called "values" here */
cmd(    CMD_VALUES,                     "va"    ),
cmd(    CMD_VALUE_DISPLAY,              "vd"    ),
cmd(    CMD_VALUES_DEF,                 "ve"    ),
cmd(    CMD_VALUES_PREFIX,              "vp"    ),
cmd(    CMD_VALUES_SUFFIX,              "vs"    ),
cmd(    CMD_VALUES_FORMAT_DEF,          "vf"    ),
cmd(    CMD_VALUES_SIZE,                "vz"    ),

/* 3d view information */
cmd(    CMD_VIEW_HANGLE,                "3h"    ),
cmd(    CMD_VIEW_VANGLE,                "3v"    ),
cmd(    CMD_VIEW_PERSP,                 "3p"    ),
cmd(    CMD_VIEW_ZOOM_OLD,              "3z"    ),
cmd(    CMD_VIEW_ZOOM,                  "3zn"   ),
cmd(    CMD_VIEW_TYPE,                  "3t"    ),

/* 3d black edging information */
cmd(    CMD_BLACK_EDGE,                 "3b"    ),

cmd(    CMD_SURFACE_RES,                "3s"    ),

/* 3d lighting information */
cmd(    CMD_LIGHT_TYPE,                 "3lt"    ),
cmd(    CMD_LIGHT_HORZ,                 "3lh"    ),
cmd(    CMD_LIGHT_VERT,                 "3lv"    ),
cmd(    CMD_LIGHT_DEPTH,                "3ld"    ),
cmd(    CMD_LIGHT_BRIGHT,               "3lbn"   ),
£²Yfh
