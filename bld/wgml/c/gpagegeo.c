/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description: WGML implement page layout setup
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   The page geometry and margins are set up here to match the wgml 4     */
/*  behaviour. Some values are guesswork and some are hardcoded, if no     */
/*  formula is found for computing the value                               */
/*  The values used are from the device and the layout :page and :default  */
/*                                                                         */
/*  The system variables &SYSxxx show where the value is stored in wgml4   */
/*                                                                         */
/*                                      incomplete              TBD        */
/*                                                                         */
/***************************************************************************/
void    init_page_geometry( void )
{
    int         k;
    uint32_t    page_depth_org;
    uint32_t    net_top_margin;
    uint32_t    net_y_start;
    uint32_t    rm_test;
    uint32_t    top_margin;
    uint32_t    y_start_correction;
#if 0                                   // activate for multi column TBD
    uint32_t    offset;
#endif


    g_resh = bin_device->horizontal_base_units; // hor resolution  &sysresh
    g_resv = bin_device->vertical_base_units;   // vert resolution &sysresv

    spacing = layout_work.defaults.spacing;

    g_cur_threshold = layout_work.widow.threshold;

    g_max_char_width = 0;
    g_max_line_height = 0;
    for( k = 0; k < wgml_font_cnt; k++ ) {
        if( g_max_char_width < wgml_fonts[k].default_width )
            g_max_char_width = wgml_fonts[k].default_width;
        if( g_max_line_height < wgml_fonts[k].line_height ) {
            g_max_line_height = wgml_fonts[k].line_height;
        }
    }
    g_curr_font = layout_work.defaults.font;

    lm = conv_hor_unit( &layout_work.page.left_margin )
         - bin_device->x_offset;        // left margin &syspagelm
    if( lm < 0 ) {                      // wgml 4.0 limits value
        lm = 0;
    }

    rm = conv_hor_unit( &layout_work.page.right_margin )
         - bin_device->x_offset;        // right margin &syspagerm

    rm_test = bin_device->horizontal_base_units / 4;
    if( (bin_device->horizontal_base_units % 4) > 0 ) {
        rm_test++;                          // round up if any remainder
    }
    if( rm < rm_test ) {                    // wgml 4.0 limits value
        xx_err( err_right_margin_2_small ); // candidate Severe Error
        g_suicide();                        // no recovery possible
    }

    g_page_left_org = lm + bin_device->x_start;
    if( g_page_left_org < bin_device->x_start )
        g_page_left_org = bin_device->x_start;
    g_page_left = g_page_left_org;
    g_cur_left = g_page_left;               // set initial value

    g_page_right_org = rm + bin_device->x_start;
    if( g_page_right_org > bin_device->page_width )
        g_page_right_org = bin_device->page_width;
    g_page_right = g_page_right_org;

    if( g_page_right > bin_device->page_width ) {// output must appear on page
        xx_err( err_margins_inverted );     // candidate Severe Error
        g_suicide();                        // no recovery possible
    }

    if( g_page_left >= g_page_right ) {     // margins cannot be inverted
        xx_err( err_margins_inverted );     // candidate Severe Error
        g_suicide();                        // no recovery possible
    }

    g_net_page_width = rm - lm;
    g_ll = g_net_page_width * CPI / bin_device->horizontal_base_units; // &sysll

    top_margin = conv_vert_unit( &layout_work.page.top_margin, 1 );

    page_depth_org = conv_vert_unit( &layout_work.page.depth, 1 );
    if( bin_device->y_offset > page_depth_org ) {
        xx_err( err_page_depth_too_small ); // candidate Severe Error
        g_suicide();                        // no recovery possible
    } else {
        g_page_depth = page_depth_org - bin_device->y_offset;  // &syspaged
    }
    if( bin_device->y_offset < top_margin ) {
        net_top_margin = top_margin - bin_device->y_offset;
    } else {
        net_top_margin = 0;
    }
    if( bin_driver->y_positive == 0 ) {
        g_page_top = bin_device->y_start - net_top_margin;
        if( g_page_depth > bin_device->y_start ) {
            /* see Wiki for discussion, wgml 4.0 differs here */
            xx_err( err_page_depth_too_big );   // candidate Severe Error
            g_suicide();                        // no recovery possible
        } else {
            g_page_bottom = bin_device->y_start - g_page_depth;// end of text area
        }
        g_net_page_depth = g_page_top - g_page_bottom;

        lcmax = 1 + (g_net_page_depth + bin_device->y_offset)
                 / wgml_fonts[g_curr_font].line_height;   // usable no of lines
    } else {
        net_y_start = bin_device->y_start;
        if( net_y_start < net_top_margin )
            net_y_start = net_top_margin;
        if( bin_device->y_start > net_top_margin ) {
            y_start_correction = bin_device->y_start - net_top_margin;
            if( y_start_correction > wgml_fonts[g_curr_font].line_height ) {
                y_start_correction = wgml_fonts[g_curr_font].line_height;
            }
        } else {
            y_start_correction = 0;
        }
        g_page_top = net_y_start - y_start_correction;
        g_page_bottom = g_page_top + g_page_depth;

        g_net_page_depth = g_page_bottom - g_page_top;
        lcmax = g_net_page_depth;
    }

    g_page_bottom_org = g_page_bottom;// save for possible bot banner calculation
    g_page_top_org = g_page_top;// save top for possible bot banner calculation

    g_cd = layout_work.defaults.columns;// no of columns   &syscd
    g_gutter = conv_hor_unit( &layout_work.defaults.gutter );   // &sysgutter

#if 0                                   // activate for multi column TBD
    if( g_cd > 1 ) {                    // multi column layout
        if( g_cd > 9 ) {
                                        // no more than 9 columns
            g_cd = 9;                   // this limit is found in script_tso.txt
                                        // for .cd control word
        }
        g_cl = (g_net_page_width - (g_cd - 1) * g_gutter )
                / (g_cd - 1);           // column length
        offset = g_page_left;
        for( k = 0; k < g_cd; ++k ) {
            g_offset[k] = offset;       // start of each column
            offset += g_cl + g_gutter;
        }
        for( ; k < 9; ++k ) {
            g_offset[k] = 0;            // dummy start of undefined columns
        }
    } else {
        g_cl = g_ll;
    }
#else
    g_cl = g_ll;                        // column length &syscl
                // This is what wgml 4 does, even if in multi column mode TBD
#endif

//  if( GlobalFlags.firstpass && GlobalFlags.research ) {  // show values TBD
    if( GlobalFlags.firstpass                         ) {
        out_msg( "\ntm:%d lm:%d rm:%d top margin:%d depth:%d\n\n", tm, lm, rm,
                 top_margin, g_page_depth );

        out_msg( "dev:%s page_w:%d page_d:%d hor_u:%d ver_u:%d x_s:%d y_s:%d"
                 " x_o:%d y_o:%d\n\n",
                 bin_device->driver_name,
                 bin_device->page_width,
                 bin_device->page_depth,
                 bin_device->horizontal_base_units,
                 bin_device->vertical_base_units,
                 bin_device->x_start,
                 bin_device->y_start,
                 bin_device->x_offset,
                 bin_device->y_offset
               );
        out_msg( "default font number:%d font_count:%d\n", g_curr_font,
                 wgml_font_cnt );
        for( k = 0; k < wgml_font_cnt; ++k ) {
            out_msg( "font:%d def_width:%d em:%d font_h:%d font_s:%d"
                     " line_h:%d line_s:%d spc_w:%d\n",
                     k,
                     wgml_fonts[k].default_width,
                     wgml_fonts[k].em_base,
                     wgml_fonts[k].font_height,
                     wgml_fonts[k].font_space,
                     wgml_fonts[k].line_height,
                     wgml_fonts[k].line_space,
                     wgml_fonts[k].spc_width
                   );
        }
        out_msg( "\npage top:%d bottom:%d left:%d right:%d lines:%d\n",
                 g_page_top, g_page_bottom, g_page_left, g_page_right, lcmax );
        out_msg(
           "page net depth:%d width:%d line height:%d char width:%d\n\n",
                  g_net_page_depth, g_net_page_width, g_max_line_height,
                  g_max_char_width );
    }
    g_indent = 0;
    g_indentr = 0;

    init_nest_cb();
}

/***************************************************************************/
/*  Computes non-attribute fields, checks for one logic error              */
/*  Not finished: unchecked errors exist                                   */
/***************************************************************************/

static void finish_banners( void )
{
    banner_lay_tag  *   cur_ban;
    region_lay_tag  *   cur_reg;
    region_lay_tag  *   top_line_reg;
    font_number         font_save;
    uint32_t            ban_line;
    uint32_t            max_reg_depth;
    font_number         max_reg_font;
    uint32_t            min_top_line;

    font_save = g_curr_font;
    for( cur_ban = layout_work.banner; cur_ban != NULL; cur_ban = cur_ban->next ) {
        ban_line = 0;
        max_reg_depth = 0;
        max_reg_font = 0;
        min_top_line = UINT32_MAX;      // start at very large positive number
        top_line_reg = NULL;
        for( cur_reg = cur_ban->region; cur_reg != NULL; cur_reg = cur_reg->next ) {
            g_curr_font = font_save;   // horizontal attributes use default font
            cur_reg->reg_indent = conv_hor_unit( &cur_reg->indent );
            cur_reg->reg_hoffset = conv_hor_unit( &cur_reg->hoffset );
            cur_reg->reg_width = conv_hor_unit( &cur_reg->width );

            g_curr_font = cur_reg->font; // vertical attributes use the banregion font
            cur_reg->reg_voffset = conv_vert_unit( &cur_reg->voffset, 1 );
            cur_reg->reg_depth = conv_vert_unit( &cur_reg->depth, 1 );

            if( max_reg_depth < cur_reg->reg_voffset + cur_reg->reg_depth ) {
                max_reg_depth = cur_reg->reg_voffset + cur_reg->reg_depth;
            }
            if( ban_line < wgml_fonts[cur_reg->font].line_height ) {
                max_reg_font = cur_reg->font;
                ban_line = wgml_fonts[max_reg_font].line_height;
            }
            if( min_top_line > cur_reg->reg_voffset + cur_reg->reg_depth ) {
                min_top_line = cur_reg->reg_voffset + cur_reg->reg_depth;
                top_line_reg = cur_reg;
            }
        }
        g_curr_font = font_save;       // horizontal attributes use default font
        cur_ban->ban_left_adjust = conv_hor_unit( &cur_ban->left_adjust );
        cur_ban->ban_right_adjust = conv_hor_unit( &cur_ban->right_adjust );

        g_curr_font = max_reg_font; // vertical attribute uses the largest banregion font
        cur_ban->ban_depth = conv_vert_unit( &cur_ban->depth, 1 );

        cur_ban->top_line = top_line_reg;

        if( cur_ban->ban_depth < max_reg_depth ) {
            xx_err( err_banreg_too_deep );
            cur_ban->ban_depth = max_reg_depth;
        }
    }
    g_curr_font = font_save;
    return;
}

/***************************************************************************/
/*  Layout end processing / document start processing                      */
/*  will be called either before the first non LAYOUT tag is processed,    */
/*  or when the first line without tags is found, i.e. produces output     */
/***************************************************************************/

void    do_layout_end_processing( void )
{
    /***********************************************************************/
    /*  Since the LAYOUT blocks are only processed on the first pass, this */
    /*  function is similarly restricted to the first pass.                */
    /***********************************************************************/

    if( GlobalFlags.firstpass == 1) {

        /*******************************************************************/
        /*  init_page_geometry must be called first since many items used  */
        /*  in the subsequent functions are initialized here, as are the   */
        /*  sysvars &SYSPAGERM and &SYSPAGED which are used in document    */
        /*  :init for device PS                                            */
        /*******************************************************************/
        init_page_geometry();

        /*******************************************************************/
        /*  This should be where all processing and error-checking         */
        /*  required after all LAYOUT blocks have been processed is done.  */
        /*******************************************************************/
        finish_banners();

        /*******************************************************************/
        /*  This is a good place to start document processing.             */
        /*******************************************************************/
        out_msg( "Formatting document\n" );

        fb_document();                 // DOCUMENT :PAUSE & :INIT processing.
        ProcFlags.fb_document_done = true;

        set_banners();                  // prepare banners for selection
    }
}


