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


#ifndef CGR_CHART_H
#define CGR_CHART_H
/*
 Description:
 ============
     This file defines the structures which define a chart.

*/

#define LEN_TEXT                80      // max length of a text_desc line
#define LEN_FORMAT              20      // max length of a prefix or suffix
#include "wpitypes.h"

/* all the enum types are unsigned int since they are set in
   radio buttons through formdev. Toggle fields are declared type 'ibool' */

typedef int     ibool;

enum {
    CGR_WHITE           = 1,
    CGR_BLACK,
    CGR_LBLUE,
    CGR_LRED,
    CGR_LPURPLE,
    CGR_LGREEN,
    CGR_LCYAN,
    CGR_LGREY,
    CGR_LYELLOW,
    CGR_BLUE,
    CGR_RED,
    CGR_PURPLE,
    CGR_GREEN,
    CGR_CYAN,
    CGR_GREY,
    CGR_YELLOW,
    CGR_LAST_COLOR
};
typedef int cgr_color;

enum {          // types of orientation
    COUNTERCLOCKWISE    = 1,
    CLOCKWISE
};
typedef int orientation;

enum {          // the types of charts
    CHART_BAR           = 1,
    CHART_AREA,
    CHART_PIE,
    CHART_XY,
    CHART_BAR_ONLY,
    CHART_AREA_ONLY,
    CHART_LINE_ONLY,
    CHART_PROPORTIONAL,
    CHART_RADAR,
    CHART_STOCK,
    CHART_A100,
    CHART_GANTT,
    CHART_BUBBLE,
    CHART_BOX_WHISKER,
    CHART_LAST
};
typedef unsigned chart_type;

enum {
    ON_TOP,
    ON_BOTTOM,
    ON_LEFT,
    ON_RIGHT,
    ON_FRONT,           // used for 3d bla charts
    ON_BACK
};
typedef unsigned int position_op;

enum {                  // the types of draw items
    DRAW_ARROW          = 1,
    DRAW_TEXT,
    DRAW_LAST
};
typedef unsigned int draw_item_type;

enum {                  // the width of the arrowhead
    VERYNARROW_HEAD     = 1,
    NARROW_HEAD,
    MEDIUM_HEAD,
    WIDE_HEAD,
    VERYWIDTH_HEAD,
    HEAD_WIDTH_LAST
};
typedef unsigned int arrowhead_width;

enum {                  // the length of the arrowhead
    SHORT_HEAD          = 1,
    AVERAGE_HEAD,
    LONG_HEAD,
    VERYLONG_HEAD,
    HEAD_LENGTH_LAST
};
typedef unsigned int arrowhead_length;

enum {                  // the type of the arrowhead
    NO_HEAD             = 1,
    LINE_HEAD,
    FILL_HEAD,
    WHITE_HEAD,
    HEAD_TYPE_LAST
};
typedef unsigned int arrowhead_type;

typedef struct wcoord {
    float               xcoord;
    float               ycoord;
} wcoord;

typedef struct label_coords {
    float       rotation[2][2];
    wcoord      min;
    wcoord      max;
    wcoord      left;           // left side of axis line
    wcoord      right;
    bool        minmax_set;
} label_coords;

enum {
    SCALE_3D_OFF,
    SCALE_3D_ON,
    SCALE_3D_SKIP       // skip first scale label
};
typedef unsigned int three_d_scale;

enum {          // 3d display
    THREE_D_OFF         = 1,
    THREE_D_ON
};
typedef unsigned three_d_display;

enum {          // format of values option
    VALUES_OFF  = 1,
    VALUES_ON,
    VALUES_ON_PERCENT
};
typedef unsigned int values_op;

enum {          // format of labels option
    LABELS_OFF          = 1,
    LABELS_ON,
    LABELS_VERTICAL
};
typedef unsigned int labels_op;
typedef unsigned int pie_labels_op;     // no slanted

enum {
    X_SCALE             = 0,
    Y_SCALE             = 1,
    Y2_SCALE
};
typedef unsigned int scale_xy;

enum {
    SCALE_MANUAL        = 1,
    SCALE_AUTO
};
typedef unsigned int scale_type;

#define FILL_SOLID_COLOR        0       // FOR INTERNAL USE ONLY
enum {                          // fill style for slice/bar/area
    FILL_SOLID  = 1,            // solid fill
    FILL_HOLLOW,                // outline, background fill for interior
    FILL_HATCH_LD,             // hatch: left diagonal
    FILL_HATCH_RD,             // hatch: right diagonal
    FILL_HATCH_HC,              // hatch: horz. cross
    FILL_HATCH_H,             // hatch: horz. lines
    FILL_HATCH_V,              // hatch: vert. lines
    FILL_HATCH_DC,             // hatch: diag. cross
    FILL_LAST
};
typedef unsigned int fill_style;

enum {                  // types of markers
    MARKER_DOT          = 1,    // '.'
    MARKER_PLUS,                // '+'
    MARKER_STAR,                // '*'
    MARKER_CIRCLE,              // 'o'
    MARKER_CROSS,               // 'x'
    MARKER_SQUARE,              // a square
    MARKER_SOLID_SQUARE,        // a solid square
    MARKER_SOLID_CIRCLE,        // a solid circle
    MARKER_SOLID_TRIANGLE,      // a solid triangle
    MARKER_SOLID_DIAMOND,       // a solid diamond
    MARKER_NONE,                // no marker
    MARKER_LAST
};
typedef unsigned int marker_style;

enum {                  // types of line styles
    LINE_NONE           = 0,    // FOR INTERNAL USE ONLY
    LINE_SOLID,
    LINE_DASHED,
    LINE_DOTTED,
    LINE_MIXED,                 // dashed and dotted
    LINE_LAST
};
typedef unsigned int line_style;

enum {
    WIDTH_SINGLE        = 1,
    WIDTH_NARROW,
    WIDTH_MEDIUM,
    WIDTH_LAST
};
typedef unsigned int line_width;

enum {                  // arrangement of bars/areas
    ARRANGE_STACKED =   1,      // stacked bars/areas
    ARRANGE_OVERLAP,            // overlapping bars/areas
    ARRANGE_CLUSTERED,          // clustered bars (N/A for areas)
    ARRANGE_PROPORTIONAL        // for rowscale.c
};
typedef unsigned int arrange;

#define ARRANGE_DEPTH ARRANGE_OVERLAP

enum {                  // the alignment of text
    TEXT_ALIGN_LEFT             = 1,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
};
typedef unsigned int text_align;

enum {                  // drawing type for each column (series)
    COLUMN_NO_CHECK     = -2,   // INTERNAL: for column_type
    COLUMN_EMPTY,               // INTERNAL: an empty column flag
    COLUMN_PIE,                 // INTERNAL: pie chart column

    COLUMN_BAR          = 1,    // draw column as bars
    COLUMN_LINE,                // draw column as lines
    COLUMN_CURVE,               // draw column as curved line
    COLUMN_POINT,               // draw column as set of points (scatter)
    COLUMN_TREND,               // draw column as least squares trend line
    COLUMN_CIRCLE,              // draw column as circle
    COLUMN_LAST
};
typedef int col_type;
#define COLUMN_AREA     1       // first in above is this if it is an area chart

enum {
    XY_LINE             = 1,
    XY_CURVE,
    XY_POINT,
    XY_TREND,
    XY_LAST
};
typedef int x_y_type;

enum {
    DRAW_COLOR          = 1,    // draw slices/bars/areas/lines with color only
    DRAW_FILL,                  // draw slices/bars/areas/lines with fill only
    DRAW_BOTH                   // use color and fill styles
};
typedef unsigned int draw_type;

enum {
    GRID_NONE           = 1,    // no grid line
    GRID_DOTTED,                // dotted grid line
    GRID_LINE                   // a solid grid line
};
typedef unsigned int grid_type;

enum {
    TICK_NONE           = 1,    // no tick marks
    TICK_IN,                    // inside chart tick marks
    TICK_OUT,                   // outside chart tick marks
    TICK_BOTH                   // in and out tick marks
};
typedef unsigned int tick_type;

#ifdef __NT__
#undef STOCK_LAST
#endif

enum {
    STOCK_MARK          = 1,
    STOCK_BAR,
    STOCK_AREA,
    STOCK_LINE,
    STOCK_LAST
};
typedef unsigned int stck_type;

enum {
    HORZ_CHART          = 1,    // horizontal bar/area chart
    VERT_CHART                  // vertical bar/area chart
};
typedef unsigned int chart_dir_type;

enum {
    PROJ_PARALLEL   = 1,        // parallel projection for 3d charts
    PROJ_PERSPECTIVE            // perspective  "       "  "    "
};
typedef unsigned int proj_type;

enum {
    LIGHT_AUTO          = 1,
    LIGHT_MANUAL
};
typedef unsigned int light_type;

enum {
    LIGHT_VERT_TOP      = 1,    // vertical light placement for 3d charts
    LIGHT_VERT_MID,
    LIGHT_VERT_BOTTOM
};
typedef unsigned int light_vert_pos;

enum {
    LIGHT_HORZ_LEFT     = 1,    // horizontal light placement
    LIGHT_HORZ_MID,
    LIGHT_HORZ_RIGHT
};
typedef unsigned int light_horz_pos;

enum {
    LIGHT_DEPTH_FRONT   = 1,    // depth light placement
    LIGHT_DEPTH_MID,
    LIGHT_DEPTH_BACK
};
typedef unsigned int light_depth_pos;

enum {
    SURFACE_RES_LOW     = 1,    // surface resolutions for 3d pie & ribbon
    SURFACE_RES_MED,
    SURFACE_RES_HIGH
};
typedef unsigned int surface_res;

enum {
    PRECISION_COLUMN    = 1,    // use column attr precision
    PRECISION_GLOBAL,           // use global precision field
    PRECISION_AUTOMATIC,
    PRECISION_LAST
};
typedef unsigned int prec_op;

enum {
    SCALE_PREC_GLOBAL   = 1,
    SCALE_PREC_AUTOMATIC,
    SCALE_PREC_LAST
};
typedef unsigned int scale_prec_op;

enum {
    FRAME_XY            = 1,
    FRAME_FULL,
    FRAME_X_ONLY,
    FRAME_OFF
};
typedef unsigned int frame_op;

enum {
    AS_IS               = 1,
    DEFAULT_NAMES,
    STOCK_LEGEND_LAST
};
typedef unsigned int stock_leg_type;

enum {
    LEGEND_OFF          = 1,
    LEGEND_TOP,
    LEGEND_BOTTOM,
    LEGEND_LEFT,
    LEGEND_RIGHT,
    LEGEND_INSIDE,
    LEGEND_LAST
};
typedef unsigned int legend_op;

enum {
    LEG_POSITION        = 1,
    LEG_FLOATING,
    LEG_TOP,
    LEG_BOTTOM,
    LEG_LEFT,
    LEG_RIGHT,
    LEG_LAST
    };
typedef unsigned int legend_type;

enum {
    GROUP_BY_COLUMN     = 1,    // group by column (bla only)
    GROUP_BY_ROW
};
typedef unsigned int group_by_op;

enum {
    NOTATION_AS_IS      = 1,
    NOTATION_SCIENTIFIC,
    NOTATION_COMPACT
};
typedef unsigned int notation_op;

enum {
    COMPACT_NONE        = 1,
    COMPACT_THOUSANDS,
    COMPACT_MILLIONS
};
typedef unsigned int compact_op;

enum {
    COMMAS_OFF          = 1,
    COMMAS_ON
};
typedef unsigned int commas_op;

enum {
    POS_BELOW           = 1,
    POS_ABOVE,
    POS_LEFT,
    POS_RIGHT,
    POS_LAST
};
typedef unsigned int values_pos;

enum {
    SPEC_MANUAL = 1,
    SPEC_AUTO,
};
typedef unsigned int spec_op;

enum {
    OTHER_OFF = 1,
    OTHER_ON
};
typedef unsigned int other_op;

enum {
    DATA_SUBSET_ALL             = 1,
    DATA_SUBSET_RANGE
};
typedef unsigned int subset_op;

/***************************************************/

#define TEXT_NORMAL     0
#define TEXT_BOLD       1
#define TEXT_ITALIC     2
#define TEXT_UNDERLINE  4
#define TEXT_STRIKEOUT  8
#define TEXT_SIZEABLE   16

typedef struct text_def {
    ibool               not_set;                // TRUE if font not set yet
    char                face_name[LF_FACESIZE];
    unsigned int        style;
    unsigned int        size;
    cgr_color           color;
    /**** this font info is not user specified: inferred from selection ****/
    unsigned int        char_set;
    char                pitchfamily;
} text_def;

typedef struct draw_text_def {
    char                text[LEN_TEXT]; // the text
    text_def            text_info;      // text information
    wcoord              top_left;       // %-age coordinates of text
    bool                framed;         // draw frame around outside
} draw_text_def;

typedef struct arrow_def {
    wcoord              top_left;       // %-age coordinates
    wcoord              bottom_right;   //      of arrow
    line_style          line;           // line style for arrow
    line_width          width;          // line width for arrow
    cgr_color           color;          // color for drawing arrow
    arrowhead_width     head_width;     // width of the arrowhead
    arrowhead_length    head_length;    // length of the arrowhead
    arrowhead_type      head_type;      // type of the arrowhead
} arrow_def;

typedef union {
    arrow_def           arrow;
    draw_text_def       text;
} draw_item_def;

typedef int     gda_index;      // an index into the graphics data area

typedef struct gda_pos {        // a position in the graphics data area
    gda_index   row;            // row (0 origin)
    gda_index   col;            // column (0 origin)
} gda_pos;

typedef struct gda_range {      // a range in the graphics data area
    gda_index           range_start;    // start
    gda_index           range_end;      // end
} gda_range;

typedef struct common_attr {            // common attributes between chart types
    ibool               display;        // display row (pie) or column (other)
    ibool               val_display;    // show value label(s) for column/row
    cgr_color           color;          // color for drawing attributes
    fill_style          fill;           // for bars, areas, pie slices
    int                 order;          // order of column/row for display
} common_attr;

typedef struct format_desc {
    char                prefix[LEN_FORMAT];
    char                suffix[LEN_FORMAT];
    notation_op         notation;
    commas_op           commas;
    int                 precision;
} format_desc;

typedef struct values_desc {
    values_op           display;
    text_def            text_info;
    format_desc         format;
} values_desc;

typedef struct labels_desc {
    labels_op           display;
    text_def            text_info;
    format_desc         format;
} labels_desc;

typedef struct legend_desc {
    legend_op           display;        // type of fixed legend
    legend_type         type;           // type of legend
    float               x_offset;       // %-age coordinates
    float               y_offset;       //      of legend
    ibool               framed;         // if true, legend has a frame
    ibool               horizontal;     // last legend state: for float only
    text_def            text_info;      // text information for legend
} legend_desc;

typedef struct scale_op {               // scale options for line-bar/area chart
    labels_desc         show_scale;     // details of showing the scale
    scale_type          calc_scale;     // auto or manual scale
    float               start_scale;    // start of scale value
    float               end_scale;      // end of scale value
    float               scale_step;     // step per scale increment
    float               scale_100_step; // step for 100% charts
} scale_op;

typedef struct scale_op_ltd {           // limited scale options
    scale_type          calc_scale;     // auto or manual scale
    float               start_scale;    // start of scale value
    float               end_scale;      // end of scale value
    float               scale_step;     // step per scale increment
    float               scale_100_step; // step for 100% charts
} scale_op_ltd;

typedef struct pie_labels_desc {        // description of pie labels
    pie_labels_op       display;
    text_def            text_info;
} pie_labels_desc;

typedef struct text_desc {              // a title, row or column label
    char                text[LEN_TEXT]; // the text
    text_def            text_info;
    text_align          align;          // alignment of text about top or axis
} text_desc;

typedef struct light_3d {               // 3d lighting parameters
    light_type          type;
    light_vert_pos      vert;
    light_horz_pos      horz;
    light_depth_pos     depth;
    int                 brightness;
    int                 contrast;
} light_3d;

typedef struct view_3d {
    int                 h_angle;        // spherical coord of viewing point
    int                 v_angle;
    int                 perspective;    // amount of perspective to use
    int                 zoom;           // amount of zoom
    proj_type           type;           // projection type
} view_3d;

typedef struct three_d_op {
    view_3d             view;
    light_3d            lighting;
    ibool               black_edges;
    surface_res         resolution;
} three_d_op;

typedef struct pie_attr {               // attributes for each pie slice (row)
    common_attr         c_attr;         // common attributes
    int                 slice_cut;      // slice cut
} pie_attr;

typedef struct pie_chart {              // pie chart options
    pie_attr far        *attr;          // array of attributes for each row
    HANDLE              attr_hld;       // hld of above memory
    int                 num_attr;       // # of alloced attrs
    gda_index           column;         // column to make pie chart
    values_desc         values;         // how values are to be displayed
    values_pos          val_pos;        // position of values
    pie_labels_desc     labels;         // whether labels are to be displayed
    float               pie_size;       // size of the pie
    int                 angle;          // angle of the pie
    other_op            other_slice;    // is the other slice on or off?
    pie_attr            other_attr;     // attributes for 'other' slice
    float               other_value;    // 'other' slice value
    char                other_label[LEN_TEXT];  // 'other' slice label
} pie_chart;

typedef struct bla_attr {               // bar-line or area column attributes
    common_attr         c_attr;         // common attributes for column
    int                 precision;      // precision of data for column
    col_type            type;           // type of drawing for column
    line_style          line;           // line style for column
    line_width          width;          // line width for column
    marker_style        marker;         // marker style for column
    scale_xy            y2_scale;       // use y1 or y2?
    gda_index           x_group;        // for xy chart only
    gda_index           y_group;        // for xy chart only
    x_y_type            xy_type;        // for xy chart only
    gda_index           start_group;    // for gantt chart only
    gda_index           length_group;   // for gantt chart only
    gda_index           bx_group;       // for bubble chart only
    gda_index           by_group;       // for bubble chart only
    gda_index           bval_group;     // for bubble chart only
} bla_attr;

typedef struct draw_attr {
    bool                deleted;        // NOT A USER OPTION!!!!
    draw_item_type      type;           // type of draw item (arrow, text)
    draw_item_def       d_item;         // the draw item
} draw_attr;

typedef struct bla_chart {              // bar-line or area chart options
    bla_attr far        *attr;          // array of attributes for each column
    HANDLE              attr_hld;       // hld of above memory
    int                 num_attr;       // # of alloced attrs
    arrange             arrange;        // arrangement of bars/areas
    chart_dir_type      vertical_chart; // vertical or horizontal chart
    values_desc         data_labels;    // how data labels are displayed
    scale_op            scale;          // scaling options
    scale_op            xscale;         // for XY and radar charts only
    scale_op_ltd        y2_scale;       // for second y scale
    labels_desc         z_labels;       // z axis labels for 3d charts
    bool                y_scale_shown;  // is y scale used?
    bool                y2_scale_shown; // is y2 scale used?
    frame_op            frame;          // TRUE: frame chart
    gda_range           row_range;      // row range to chart
    text_desc           row_label;      // row label
    text_desc           col_label;      // column label
    text_desc           col2_label;     // y2 label
    text_desc           zcol_label;     // z axis label for 3d charts
    grid_type           row_grid;       // grid lines up from row axis
    grid_type           z_grid;         // grid lines for z axis in 3d charts
    grid_type           col_grid;       // grid lines across from column axis
    grid_type           col2_grid;      // grid lines across from y2 axis
    tick_type           row_tick;       // row axis tick marks
    tick_type           z_tick;         // z axis ticks for 3d charts
    tick_type           col_tick;       // column axis tick marks
    tick_type           col2_tick;      // y2 axis tick marks
    prec_op             data_prec;      // data label precision
    scale_prec_op       yscale_prec;    // y scale precision
    scale_prec_op       xscale_prec;    // x scale precision
    spec_op             user_overlap;   // user specification
    float               bar_overlap;    // overlap % of the bars
    spec_op             user_width;     // user specification
    float               bar_width;      // width % of the clusters
    cgr_color           frame_color;    // frame/grid/tick colour
    subset_op           data_subset;    // row/column range type
    grid_type           zero_line;      // style for zero axes
    grid_type           radar_xaxis;    // style for x_axis
    grid_type           radar_yaxis;    // style for y_axis
    orientation         radar_orient;   // orientation of radar
    float               radar_size;     // size of the radar circle
    stock_leg_type      stock_legend;   // column names or hi/lo/open/close
    stck_type           stock_type;     // type of stock chart
    spec_op             stock_u_width;  // user specification for stock charts
    float               stock_b_width;  // width % of the clusters  "    "
    ibool               overlay_grid;   // if TRUE, overlay the grid ( radar )
} bla_chart;

typedef unsigned chart_hld;

typedef struct cgr_chart {              // this defines a chart
    chart_type          type;           // the type of chart
    three_d_display     three_d;        // is the chart a 3d chart
    text_desc           title;          // title of the chart
    gda_index           max_row;        // row data access: 0 to max_row
    gda_index           max_col;        // column data access: 0 to max_col
    draw_type           draw;           // use color, patterns, or both
    legend_desc         legend;         // TRUE: put legend on chart
    group_by_op         group_by;       // grouped by bar/column
    pie_chart           pie;            // options for a pie chart
    bla_chart           bar_area;       // options for a bar-line or area chart
    three_d_op          view;           // viewing parameters for 3d charts
    LPSTR (APIENTRY *row_names)
        (chart_hld chart, unsigned row);// row name fetch routine
    LPSTR (APIENTRY *col_names)
        (chart_hld chart, unsigned col);// column name fetch routine
    BOOL (APIENTRY *get_data)
        (chart_hld chart, unsigned row,
         unsigned col, float far *value);       // fetch data routine
    void (APIENTRY *use_data)
        ( chart_hld chart, BOOL );      // toggle data useage by CGR
    LPSTR (APIENTRY *macro_value)       // chart macros
        ( chart_hld chart, LPSTR macro_name );
    WPI_COLOUR far      *palette;       // colour palette of the chart
    HANDLE              palette_hld;    // gbl handle of palette
    int                 palette_size;   // size of the palette
    draw_attr far       *draw_attr;     // array of attr's for each draw item
    HANDLE              draw_attr_hld;  // hld of above memory
    int                 num_draw_attr;  // number of draw items
    text_desc           subtitle;       // subtitle of the chart
    text_desc           footnote;       // footnote of the chart
} cgr_chart;

/* this structure is used to add or change a chart. The application
   passes it into CGR */
typedef struct {
    unsigned            max_row;
    unsigned            max_col;
    LPSTR (APIENTRY *row_names)
        (chart_hld chart, unsigned row);// row name fetch routine
    LPSTR (APIENTRY *col_names)
        (chart_hld chart, unsigned col);// column name fetch routine
    BOOL (APIENTRY *get_data)
        (chart_hld chart, unsigned row,
         unsigned col, float far *value);       // fetch data routine
    void (APIENTRY *use_data)
        ( chart_hld chart, BOOL );      // toggle data useage by CGR
    LPSTR (APIENTRY *macro_value)       // chart macros
        ( chart_hld chart, LPSTR macro_name );
} cgr_add_chart;


/* some constants of interest */
#define CGR_MAX_PRECISION       6       // maximum value precision
#define CGR_MAX_SLICE_CUT       9       // max slice cut

#endif
