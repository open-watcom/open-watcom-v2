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


#ifndef CGR_GLOBAL_H
#define CGR_GLOBAL_H
/*
 Description:
 ============
     This file defines global constants for Chart graphics.

*/

/************ globals without predefined values **************/

/* These ptrs are used a lot so they have globals for them */
cgrglobal cgr_chart     *Curr_chart;    // chart currently being displayed
cgrglobal bla_chart     *Curr_bla_options;
cgrglobal pie_chart     *Curr_pie_options;

cgrglobal WPI_COLOUR    *Curr_palette;
cgrglobal int           Palette_size;
cgrglobal WPI_PRES      Win_dc;
cgrglobal chart_hld     Curr_chart_hld;
cgrglobal float         Pie_other_val;


cgrglobal Error_int1;                   // for cgr error handling

cgrglobal int           Curr_groups;    // num groups for current chart

cgrglobal cgr_window    Curr_window;    // the charts graphics window
cgrglobal float         Window_width;   // current window width
cgrglobal float         Window_height;  // current window height

cgrglobal char          Dollar_sign[];
cgrglobal char          Percent_sign[];


cgrglobal float         Aspect_ratio;   // the aspect ratio of the current
                                        // display
cgrglobal float         Pixel_aspect_ratio;     // pixel aspect ratio

enum {          // to tell what a scale_info struct is
    INFO_YSCALE,
    INFO_XSCALE,
};

typedef struct scale_information {
    float       scale_ratio;            // 1/(max - min)
    int         precision;              // precision of scale
    float       min;                    // max data value
    float       max;                    // min data value
    float       lbl_min;                // min labelled value
    bool        lbl_min_set;            // is there a minimum label?
    float       lbl_max;                // max labelled value
    bool        lbl_max_set;            // is there a maximum label?
    float       offset;                 // grid, label, tick offset
    float       start;                  // start scale value
    float       end;                    // ending scale value
    float       step;                   // step for scale
    compact_op  compact;                // amount by which to compact labels
    int         which_scale;            // INFO_[Y/X]SCALE
} scale_information;
cgrglobal scale_information
  Scale_info,                           // stuff about column axis scaling
  Xscale_info,                          // for XY charts
  Y2scale_info;                         // for y2 scale

cgrglobal row_scale     *Scale;         // Row scaling for bar/area chart
cgrglobal row_scale     *Scale_stock;   // Row scaling for stock chart
cgrglobal col_scale     *Scale_col;     // used for 3d bar charts
cgrglobal chart_cube    *Scale_cube;    // used for 3d bla charts

cgrglobal void          *(*Cgr_alloc)();
cgrglobal void          *(*Cgr_realloc)();
cgrglobal void          (*Cgr_free)();

#ifndef __NT__
cgrglobal WPI_CATCHBUF  Cgr_jump;               // used by file.c and dwsdata.c
#endif
cgrglobal char far      *(WINEXP *File_get)(void far *); // "" ""
cgrglobal void far      *File_ctl;              // "" ""

cgrglobal float         Legend_p_size;
cgrglobal bool          Is_metafile;

cgrglobal rend_list     *Pipe3dList;    // intermediate list for 3d pipeline

/******** globals with predefined values (see global.c) **********/

/* default chart structs */
extern text_desc Default_title;
extern text_desc Default_subtitle;
extern text_desc Default_footnote;
extern legend_desc Default_legend;
extern bla_chart Default_bla;
extern bla_attr Default_bla_attr;
extern pie_chart Default_pie;
extern pie_attr Default_pie_attr;
extern three_d_op Default_three_d_op;
extern char Empty_str[];
extern WPI_COLOUR Default_palette[];
extern const hls_colour Black_hls;

#endif
