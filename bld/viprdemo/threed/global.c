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
     This file defines all globals with predefined values. Also, external
     list structures are defined at the end.

*/

#define CGR_GLOBAL
#include "icgr.h"

char Empty_str[]="";

/***************** DEFAULT STRUCTS *****************/

#define _default_text( size ) \
    TRUE,                       /* use system default for font info */  \
    "",                         /* no type face                     */  \
    TEXT_NORMAL,                /* no styles                        */  \
    size,                       /* the size                         */  \
    CGR_BLACK,                  /* the colour of the text           */  \
    ANSI_CHARSET,                       \
    DEFAULT_PITCH | FF_DONTCARE


#define _default_text_italic( size ) \
    TRUE,                       /* use system default for font info */  \
    "",                         /* no type face                     */  \
    TEXT_ITALIC,                /* italic text                      */  \
    size,                       /* the size                         */  \
    CGR_BLACK,                  /* the colour of the text           */  \
    ANSI_CHARSET,                       \
    DEFAULT_PITCH | FF_DONTCARE

text_desc Default_subtitle = {
    "",                         // no subtitle
    _default_text_italic( 12 ), // text info
    TEXT_ALIGN_CENTER           // centre subtitle
};

text_desc Default_footnote = {
    "",                         // no footnote
    _default_text_italic( 10 ), // text info
    TEXT_ALIGN_RIGHT            // right-align footnote
};

text_desc Default_title = {
    "",                         // no title
    _default_text( 18 ),        // text info
    TEXT_ALIGN_CENTER           // centre title
};

bla_chart Default_bla = {
    NULL,                       // no attributes yet
    NULL,                       // no attr handle either
    0,                          // num attrs = 0
    ARRANGE_OVERLAP,            // overlap the bars/areas
    VERT_CHART,                 // vertical chart

    VALUES_OFF,                 // no value labels
    _default_text( 12 ),        // text info
    "",                         // format prefix
    "",                         // format suffix
    NOTATION_AS_IS,             // format notation
    COMMAS_OFF,                 // format commas
    0,                          // format precision

    LABELS_ON,                  // show the scale
    _default_text( 12 ),        // text info
    "",                         // format prefix
    "",                         // format suffix
    NOTATION_AS_IS,             // format notation
    COMMAS_OFF,                 // format commas
    0,                          // format precision
    SCALE_AUTO,                 // automatic scaling
    0.0,                        // NO DEFAULTS FOR MANUAL SCALING
    0.0,                        // "" ""
    0.0,                        // "" ""
    25.0,                       // but make proportional charts intelligent

    LABELS_ON,                  // show the xscale,   for xy charts only
    _default_text( 12 ),        // text info
    "",                         // format prefix
    "",                         // format suffix
    NOTATION_AS_IS,             // format notation
    COMMAS_OFF,                 // format commas
    0,                          // format precision
    SCALE_AUTO,                 // automatic scaling
    0.0,                        // NO DEFAULTS FOR MANUAL SCALING
    0.0,                        // "" ""
    0.0,                        // "" ""
    0.0,                        // "" ""

    SCALE_AUTO,                 // automatic scaling
    0.0,                        // NO DEFAULTS FOR MANUAL SCALING
    0.0,                        // "" ""
    0.0,                        // "" ""
    25.0,                       // but make proportional charts intelligent

    LABELS_ON,                  // show the zscale
    _default_text( 12 ),        // text info
    "",                         // format prefix
    "",                         // format suffix
    NOTATION_AS_IS,             // format notation
    COMMAS_OFF,                 // format commas
    0,                          // format precision

    FALSE,                      // no y scale
    FALSE,                      // no y2 scale

    FRAME_FULL,                 // frame the chart

    0,                          // NO DEFAULT FOR ROW RANGE
    0,                          // "" ""

    "",                         // no row label
    _default_text( 16 ),        // text info
    TEXT_ALIGN_CENTER,          // center it

    "",                         // no Y1 label
    _default_text( 16 ),        // text info
    TEXT_ALIGN_CENTER,          // center it

    "",                         // no Y2 label
    _default_text( 16 ),        // text info
    TEXT_ALIGN_CENTER,          // center it

    "",                         // no Z label
    _default_text( 16 ),        // text info
    TEXT_ALIGN_CENTER,          // center it

    GRID_NONE,                  // no row axis grid lines
    GRID_NONE,                  // no z axis grid lines
    GRID_DOTTED,                // dotted column axis grid lines
    GRID_DOTTED,                // dotted y2 axis grid lines
    TICK_OUT,                   // outside row axis tick marks
    TICK_OUT,                   // outside z axis tick marks
    TICK_BOTH,                  // in and out tick marks on column axis
    TICK_BOTH,                  // in and out tick marks on y2 axis
    PRECISION_AUTOMATIC,        // calculate precision by column attribute
    SCALE_PREC_AUTOMATIC,       // auto y scale
    SCALE_PREC_AUTOMATIC,       // auto x scale (XY and bubble)

    SPEC_AUTO,                  // user specified this
    0.50,                       // bar overlap
    SPEC_AUTO,                  // user didn't specify this
    0.66,                       // bar width
    CGR_BLACK,                  // frame/grid/tick color
    DATA_SUBSET_ALL,            // chart full range of data
    GRID_LINE,                   // solid zero axis lines

    GRID_LINE,                  // solid x axis line (radar)
    GRID_LINE,                  // solid y axis line (radar)
    COUNTERCLOCKWISE,           // c.clockwise orientation (radar)
    .75,                        // 75% radar size

    TRUE,                       // a stock-type legend
    STOCK_MARK,                 // stock chart default type
    SPEC_AUTO,                  // user didn't specify this
    0.25,                       // bar width

    FALSE                       // don't overlay the grid
};

bla_attr Default_bla_attr = {
    TRUE,                       // display it
    TRUE,                       // display values
    0,                          // NO DEFAULT FOR COLOR
    0,                          // NO DEFAULT FOR FILL STYLE
    0,                          // default order
    0,                          // 0 precision
    COLUMN_BAR,                 // bar (or area) column type
    LINE_SOLID,                 // solid lines
    WIDTH_SINGLE,               // line width
    0,                          // NO DEFAULT FOR MARKER
    Y_SCALE,                    // no y2 scale
    0,                          // NO DEFAULT FOR X GROUP
    0,                          // NO DEFAULT FOR Y GROUP
    XY_LINE,                    // xy line type
    0,                          // DEFAULT START GROUP FOR GANTT
    1,                          // DEFAULT LENGTH GROUP FOR GANTT
    0,                          // NO DEFAULT X GROUP FOR BUBBLE
    0,                          // NO DEFAULT Y GROUP FOR BUBBLE
    0                           // NO DEFAULT VALUE GROUP FOR BUBBLE
};

pie_chart Default_pie = {
    NULL,                       // no attributes yet
    NULL,
    0,                          // "" ""
    0,                          // use column 0

    VALUES_OFF,                 // no value labels
    _default_text( 12 ),        // text info
    "",                         // value prefix
    "",                         // value suffix
    NOTATION_AS_IS,             // value notation
    COMMAS_OFF,                 // value commas
    0,                          // value precision

    POS_BELOW,                  // values below slice labels

    LABELS_ON,                  // want slice labels
    _default_text( 12 ),        // text info

    .75,                        // pie size
    0,                          // pie angle

    OTHER_OFF,                  // no 'other' slice
    TRUE,                       // display 'other' row slice
    TRUE,                       // display values
    CGR_BLACK,                  // colour
    FILL_SOLID,                 // fill pattern
    9999,                       // default order
    0,                          // no slice cut
    0.0,                        // 'other' cut-off value
    "Other"                     // 'other' slice label
};

pie_attr Default_pie_attr = {
    TRUE,                       // display the row slice
    TRUE,                       // display values
    0,                          // NO DEFAULT COLOR
    0,                          // NO DEFAULT FOR FILL STYLE
    0,                          // default order
    0                           // no slice cut
};

legend_desc Default_legend = {
    LEGEND_OFF,                 // no legend ( positionally defn )
    LEG_POSITION,               // right type of legend when user oriented
    0.0,                        // no x offset
    0.0,                        // no y offset
    FALSE,                      // framed
    FALSE,                      // vertical legend
    _default_text( 12 )         // text info
};

three_d_op Default_three_d_op = {
    {                       // viewing options
        35,                 // h_angle
        15,                 // v_angle
        0,                  // perspective
        100,                // zoom  (100%)
        PROJ_PARALLEL,      // type
    },
    {                       // lighting options
        LIGHT_AUTO,         // auto or manual
        LIGHT_VERT_TOP,     // vertical position
        LIGHT_HORZ_LEFT,    // horizontal position
        LIGHT_DEPTH_FRONT,  // depth position
        50,                 // brightness
        80                  // contrast
    },
    TRUE,                   // black edges
    SURFACE_RES_LOW         // surface resolution
};

#if 0   // this was the old palette, with dark/light colour combos
WPI_COLOUR Default_palette[]={
    _wpi_getrgb( 255,   255,    255 ),
    _wpi_getrgb( 0,     0,      0 ),
    _wpi_getrgb( 0,     0,      128 ),
    _wpi_getrgb( 0,     0,      255 ),
    _wpi_getrgb( 128,   0,      0 ),
    _wpi_getrgb( 255,   0,      0 ),
    _wpi_getrgb( 128,   0,      128 ),
    _wpi_getrgb( 255,   0,      255 ),
    _wpi_getrgb( 0,     128,    0 ),
    _wpi_getrgb( 0,     255,    0 ),
    _wpi_getrgb( 0,     128,    128 ),
    _wpi_getrgb( 0,     255,    255 ),
    _wpi_getrgb( 128,   128,    128 ),
    _wpi_getrgb( 192,   192,    192 ),
    _wpi_getrgb( 128,   128,    0 ),
    _wpi_getrgb( 255,   255,    0 )
};
#endif

#if 0
WPI_COLOUR Default_palette[]={
    CLR_WHITE,
    CLR_BLACK,
    CLR_BLUE,
    CLR_RED,
    CLR_PINK,
    CLR_GREEN,
    CLR_BLUE,
    CLR_PALEGRAY,
    CLR_YELLOW,
    CLR_DARKBLUE,
    CLR_DARKRED,
    CLR_DARKPINK,
    CLR_DARKGREEN,
    CLR_CYAN,
    CLR_DARKGRAY,
    CLR_BROWN
};
#endif
WPI_COLOUR Default_palette[]={
    _wpi_getrgb( 255,   255,    255 ),
    _wpi_getrgb( 0,     0,      0 ),
    _wpi_getrgb( 0,     0,      255 ),
    _wpi_getrgb( 255,   0,      0 ),
    _wpi_getrgb( 255,   0,      255 ),
    _wpi_getrgb( 0,     255,    0 ),
    _wpi_getrgb( 0,     255,    255 ),
    _wpi_getrgb( 192,   192,    192 ),
    _wpi_getrgb( 255,   255,    0 ),
    _wpi_getrgb( 0,     0,      128 ),
    _wpi_getrgb( 128,   0,      0 ),
    _wpi_getrgb( 128,   0,      128 ),
    _wpi_getrgb( 0,     128,    0 ),
    _wpi_getrgb( 0,     128,    128 ),
    _wpi_getrgb( 128,   128,    128 ),
    _wpi_getrgb( 128,   128,    0 ),
};

const hls_colour Black_hls = { 0., 0., 0. };


/*********** external name lists *****************/

/* These are no longer nesc. in BGP since all this stuff is in the resource
   file */
#if 0
char *Cgr_chart_types[]={
    "Bar/line chart",
    "Area/line chart",
    "Pie chart",
    "XY chart",
    "Bar chart",
    "Area chart",
    "Line chart",
    "100% Bar Chart",
    NULL
};

char *Cgr_scale_types[]={
    "Y1",
    "Y2",
    NULL
};

char *Cgr_fill_types[]={
    "Solid",
    "Hollow",
    "\\\\\\\\-Diagonal hatch",
    "////-Diagonal hatch",
    "Small  cross  hatch",
    " \\ \\-Diagonal hatch",
    " / /-Diagonal hatch",
    "Big  cross    hatch",
    NULL
};

char *Cgr_marker_types[]={
    "Dot:    .",
    "Plus:   +",
    "Star:   *",
    "Circle: o",
    "Cross:  x",
    "Square: \xfe",
    "No marker",
    NULL
};

char *Cgr_draw_types[]={
    "Colour",
    "Pattern",
    "Colour and pattern",
    NULL
};

char *Cgr_line_types[]={
    "Solid",
    "Dashed",
    "Dotted",
    "Dashed and dotted",
    NULL
};

char *Cgr_font_types[]={
    "Normal",
    "Bold",
    "Roman",
    "Italic",
    NULL
};

char *Cgr_align_types[]={
    "Left",
    "Center",
    "Right",
    NULL
};

char *Cgr_print_names[]={
    "No printer chosen",
    "IBM Graphics",
    "HP LaserJet",
    "Toshiba P321/P351",
    "PostScript",
    "Pixel Array (PXA)",
    "HP Plotter (HP-GL)",
    "Epson FX80/LX100",
    "TIFF Image File",
    NULL
};


#endif
