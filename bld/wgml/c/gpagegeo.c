/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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


#include "wgml.h"


/***************************************************************************/
/*  The page geometry and margins are set up here to match the wgml 4.0    */
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
    int         i;
    uint32_t    page_depth_org;
    uint32_t    net_top_margin;
    uint32_t    net_y_start;
    uint32_t    rm_test;
    uint32_t    top_margin;
    uint32_t    y_start_correction;

    g_resh = bin_device->horizontal_base_units; // hor resolution  &sysresh
    g_resv = bin_device->vertical_base_units;   // vert resolution &sysresv

    g_cur_threshold = layout_work.widow.threshold;

    g_max_char_width = 0;
    g_max_line_height = 0;
    for( i = 0; i < wgml_font_cnt; i++ ) {
        if( g_max_char_width < wgml_fonts[i].default_width )
            g_max_char_width = wgml_fonts[i].default_width;
        if( g_max_line_height < wgml_fonts[i].line_height ) {
            g_max_line_height = wgml_fonts[i].line_height;
        }
    }
    g_curr_font = layout_work.defaults.font;

    g_lm = conv_hor_unit( &layout_work.page.left_margin, g_curr_font )
         - bin_device->x_offset;        // left margin &syspagelm
    if( g_lm < 0 ) {                      // wgml 4.0 limits value
        g_lm = 0;
    }

    g_rm = conv_hor_unit( &layout_work.page.right_margin, g_curr_font )
         - bin_device->x_offset;        // right margin &syspagerm

    rm_test = bin_device->horizontal_base_units / 4;
    if( (bin_device->horizontal_base_units % 4) > 0 ) {
        rm_test++;                          // round up if any remainder
    }
    if( g_rm < rm_test ) {                    // wgml 4.0 limits value
        xx_err( err_right_margin_2_small ); // candidate Severe Error
    }

    g_page_left_org = g_lm + bin_device->x_start;
    if( g_page_left_org < bin_device->x_start )
        g_page_left_org = bin_device->x_start;
    t_page.page_left = g_page_left_org;
    t_page.cur_left = t_page.page_left;     // set initial value

    g_page_right_org = g_rm + bin_device->x_start;
    if( g_page_right_org > bin_device->page_width ) {   // output must appear on page
        xx_err( err_margins_inverted );                 // candidate Severe Error
    }

    if( t_page.page_left >= g_page_right_org ) {    // margins cannot be inverted
        xx_err( err_margins_inverted );             // candidate Severe Error
    }

    g_net_page_width = g_rm - g_lm;
    t_page.page_width = g_net_page_width;

    /****************************************************************/
    /* wgml 4.0 does this without regard to the valus specified in  */
    /* the layout; only the corresponding control words CD, CL, and */
    /* LL affect these values. These are all sytem variables.       */
    /****************************************************************/

    g_cd = 1;
    g_cl = 60;
    g_ll = 60;                              // default value per TSO
    g_gutter = 0;

    top_margin = conv_vert_unit( &layout_work.page.top_margin, 1, g_curr_font );

    page_depth_org = conv_vert_unit( &layout_work.page.depth, 1, g_curr_font );
    if( bin_device->y_offset > page_depth_org ) {
        xx_err( err_page_depth_too_small ); // candidate Severe Error
    } else if( top_margin > 0 ) {           // strange but true
        g_page_depth = page_depth_org;      // &syspaged
    } else {
        g_page_depth = page_depth_org - bin_device->y_offset;  // &syspaged
    }
    if( bin_device->y_offset < top_margin ) {
        net_top_margin = top_margin - bin_device->y_offset;
    } else {
        net_top_margin = 0;
    }
    if( bin_driver->y_positive == 0 ) {
        t_page.panes_top = bin_device->y_start - net_top_margin;
        if( g_page_depth > bin_device->y_start ) {
            /* see Wiki for discussion, wgml 4.0 differs here */
            xx_err( err_page_depth_too_big );   // candidate Severe Error
        } else {
            t_page.bot_ban_top = t_page.panes_top - g_page_depth;// end of text area
        }
        g_net_page_depth = t_page.panes_top - t_page.bot_ban_top;

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
        t_page.panes_top = net_y_start - y_start_correction;
        t_page.bot_ban_top = t_page.panes_top + g_page_depth;

        g_net_page_depth = t_page.bot_ban_top - t_page.panes_top;
        lcmax = g_net_page_depth;
    }
    t_page.panes->col_width_top = t_page.panes_top;
    for( i = 0; i < MAX_COL; i++ ) {
        t_page.panes->cols[i].main_top = t_page.panes_top;
        t_page.panes->cols[i].fig_top = t_page.bot_ban_top;
        t_page.panes->cols[i].fn_top = t_page.bot_ban_top;
    }

    g_page_bottom_org = t_page.bot_ban_top; // save for possible bot banner calculation
    t_page.page_top = t_page.panes_top;     // save top for possible bot banner calculation

    if( GlobalFlags.firstpass && GlobalFlags.research ) {  // show values TBD
        out_msg( "\ntm:%d lm:%d rm:%d top margin:%d depth:%d\n\n",
                g_tm, g_lm, g_rm, top_margin, g_page_depth );

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
        for( i = 0; i < wgml_font_cnt; ++i ) {
            out_msg( "font:%d def_width:%d em:%d font_h:%d font_s:%d"
                     " line_h:%d line_s:%d spc_w:%d\n",
                     i,
                     wgml_fonts[i].default_width,
                     wgml_fonts[i].em_base,
                     wgml_fonts[i].font_height,
                     wgml_fonts[i].font_space,
                     wgml_fonts[i].line_height,
                     wgml_fonts[i].line_space,
                     wgml_fonts[i].spc_width
                   );
        }
        out_msg( "\npage top:%d bottom:%d left:%d right:%d lines:%d\n",
                 t_page.panes_top, t_page.bot_ban_top, t_page.page_left,
                 g_page_right_org, lcmax );
        out_msg(
           "page net depth:%d width:%d line height:%d char width:%d\n\n",
                  g_net_page_depth, g_net_page_width, g_max_line_height,
                  g_max_char_width );
    }
    g_indent = 0;
    g_indentr = 0;
}

/***************************************************************************/
/*  Check the list tag layouts for skipped level numbers                   */
/***************************************************************************/

static void finish_lists( void )
{
    dl_lay_level    *   dl_layout;
    gl_lay_level    *   gl_layout;
    ol_lay_level    *   ol_layout;
    sl_lay_level    *   sl_layout;
    uint8_t             curr_level;
    ul_lay_level    *   ul_layout;

    dl_layout = layout_work.dl.first;
    curr_level = 0;
    while( dl_layout != NULL ) {
        curr_level++;
        if( curr_level != dl_layout->level ) {
            list_level_err( "DL", curr_level );
        }
        dl_layout = dl_layout->next;
    }

    if( layout_work.dl.max_level != curr_level ) {
        layout_work.dl.max_level = curr_level;
    }

    gl_layout = layout_work.gl.first;
    curr_level = 0;
    while( gl_layout != NULL ) {
        curr_level++;
        if( curr_level != gl_layout->level ) {
            list_level_err( "GL", curr_level );
        }
        gl_layout = gl_layout->next;
    }

    if( layout_work.gl.max_level != curr_level ) {
        layout_work.gl.max_level = curr_level;
    }

    ol_layout = layout_work.ol.first;
    curr_level = 0;
    while( ol_layout != NULL ) {
        curr_level++;
        if( curr_level != ol_layout->level ) {
            list_level_err( "OL", curr_level );
        }
        ol_layout = ol_layout->next;
    }

    if( layout_work.ol.max_level != curr_level ) {
        layout_work.ol.max_level = curr_level;
    }

    sl_layout = layout_work.sl.first;
    curr_level = 0;
    while( sl_layout != NULL ) {
        curr_level++;
        if( curr_level != sl_layout->level ) {
            list_level_err( "SL", curr_level );
        }
        sl_layout = sl_layout->next;
    }

    if( layout_work.sl.max_level != curr_level ) {
        layout_work.sl.max_level = curr_level;
    }

    ul_layout = layout_work.ul.first;
    curr_level = 0;
    while( ul_layout != NULL ) {
        curr_level++;
        if( curr_level != ul_layout->level ) {
            list_level_err( "UL", curr_level );
        }
        ul_layout = ul_layout->next;
    }

    if( layout_work.ul.max_level != curr_level ) {
        layout_work.ul.max_level = curr_level;
    }

    return;
}

/***************************************************************************/
/*  Split banregion into left middle right part if region is script format */
/***************************************************************************/

static void preprocess_script_region( region_lay_tag * reg )
{

    char    *   pl;
    char        sep;
    int         k;

    if( reg != NULL ) {
        if( (reg->contents.content_type == string_content)
            && reg->script_format ) {

            /***************************************************************/
            /*  script format is a 3 part region: left middle right        */
            /*  first char is separator char                               */
            /*  /left//right/  empty middle part in this case              */
            /***************************************************************/

            /***************************************************************/
            /*  preprocess script format banner region for speed           */
            /***************************************************************/
            pl = reg->contents.string;
            sep = *pl;                      // first char is separator
                                            // isolate region parts
            for( k = 0; k < 3; ++k ) {      // left, center, right
                pl++;

                /***************************************************************/
                /* this makes sense but only works like wgml 4.0 when k == 2   */
                /* wgml 4.0 starts 2 spaces to the right when k == 0 when k == */
                /* 1 per space skipped here, although the spaces themselves    */
                /* are removed                                                 */
                /***************************************************************/

                SkipSpaces( pl );       // remove leading spaces
                reg->script_region[k].string = pl;
                while( *pl &&  *pl != sep ) {
                    pl++;
                }
                reg->script_region[k].len =  pl - reg->script_region[k].string ;

                if( reg->script_region[k].len == 0 ) {
                    reg->script_region[k].string = NULL;
                } else {
                    *pl = '\0';             // null terminate
                }
            }
        }
    }
}

/****************************************************************************/
/* Computes non-attribute fields, re-sorts regions for output               */
/* Finalizes region start positions and sizes                               */
/* Also validates the banner and issues error messages as needed            */
/****************************************************************************/

static void finish_banners( void )
{
    banner_lay_tag  *   bot_ban;
    banner_lay_tag  *   cur_ban;
    banner_lay_tag  *   top_ban;
    ban_reg_group   *   cur_grp;
    ban_reg_group   *   old_grp;
    ban_reg_group   *   sav_grp;
    ban_reg_group   *   src_grp;
    font_number         max_reg_font;
    region_lay_tag  *   cur_reg;
    region_lay_tag  *   nxt_reg;
    region_lay_tag  *   old_reg;
    region_lay_tag  *   sav_reg;
    uint32_t            ban_line;
    uint32_t            ban_bot_depth;
    uint32_t            ban_top_depth;

    ban_bot_depth = 0;
    ban_top_depth = 0;
    bot_ban = NULL;
    top_ban = NULL;

    for( cur_ban = layout_work.banner; cur_ban != NULL; cur_ban = cur_ban->next ) {
        ban_line = wgml_fonts[FONT0].line_height;       // minimum line height for banner
        max_reg_font = 0;

        /* horizontal attributes use default font */
        cur_ban->ban_left_adjust = conv_hor_unit( &cur_ban->left_adjust, g_curr_font );
        cur_ban->ban_right_adjust = conv_hor_unit( &cur_ban->right_adjust, g_curr_font );

        if( (cur_ban->ban_left_adjust + cur_ban->ban_right_adjust) >= g_net_page_width ) {
            ban_reg_err( err_ban_width, cur_ban, NULL, NULL, NULL );
        }

        /****************************************************************/
        /* Compute values used for vertical space occupied by banner    */
        /****************************************************************/

        for( cur_reg = cur_ban->region; cur_reg != NULL; cur_reg = cur_reg->next ) {
            if( wgml_fonts[cur_reg->font].line_height > wgml_fonts[FONT0].line_height ) {
                if( wgml_fonts[cur_reg->font].line_height > ban_line ) {
                    max_reg_font = cur_reg->font;
                    ban_line = wgml_fonts[max_reg_font].line_height;
                }
            }
        }

        /* vertical attribute uses the largest banregion font */
        cur_ban->ban_depth = conv_vert_unit( &cur_ban->depth, 1, max_reg_font );

        if( cur_ban->ban_depth == 0 ) {
            ban_reg_err( err_ban_depth1, cur_ban, NULL, NULL, NULL );
        }

        if( cur_ban->ban_depth > g_page_depth ) {
            ban_reg_err( err_ban_depth2, cur_ban, NULL, NULL, NULL );
        }

        /****************************************************************/
        /* Set curr_ban->style to the number style, if any              */
        /* The search order is the refnum order                         */
        /* The first number style found is used                         */
        /****************************************************************/

        switch( cur_ban->docsect ) {
        case abstract_ban :
        case appendix_ban :
        case backm_ban :
        case body_ban :
        case preface_ban :
            for( cur_reg = cur_ban->region; cur_reg != NULL; cur_reg = cur_reg->next ) {

                /* update curr_ban->style if the region content_type is "pgnumX" */

                switch( cur_reg->contents.content_type ) {
                case pgnuma_content :
                case pgnumad_content :
                case pgnumr_content :
                case pgnumrd_content :
                case pgnumc_content :
                case pgnumcd_content :
                    cur_ban->style = cur_reg->contents.content_type;
                }
                if( cur_ban->style != no_content ) {    // exit when first style found
                    break;
                }
            }
        }

        /****************************************************************/
        /* Compute values for each region before re-sorting them        */
        /****************************************************************/

        for( cur_reg = cur_ban->region; cur_reg != NULL; cur_reg = cur_reg->next ) {
            /* horizontal attributes use default font */
            cur_reg->reg_indent = conv_hor_unit( &cur_reg->indent, g_curr_font );
            if( cur_reg->hoffset.su_u == SU_lay_left ) {            // left banner margin
                cur_reg->reg_hoffset = cur_ban->ban_left_adjust;
                cur_reg->reg_h_type = SU_lay_left;                  // h_offset is left boundary
            } else if( cur_reg->hoffset.su_u == SU_lay_centre ) {   // banner center point
                cur_reg->reg_hoffset = cur_ban->ban_left_adjust +
                ((g_net_page_width - cur_ban->ban_right_adjust - cur_ban->ban_left_adjust) / 2 );
                cur_reg->reg_h_type = SU_lay_centre;                // h_offset is center
            } else if( cur_reg->hoffset.su_u == SU_lay_right ) {    // right banner margin
                cur_reg->reg_hoffset = g_net_page_width - cur_ban->ban_right_adjust;
                cur_reg->reg_h_type = SU_lay_right;                 // h_offset is right boundary
            } else {                                                // numeric value specified
                cur_reg->reg_hoffset = conv_hor_unit( &cur_reg->hoffset, g_curr_font );
                cur_reg->reg_h_type = SU_lay_left;                  // h_offset is left boundry
            }
            cur_reg->reg_width = conv_hor_unit( &cur_reg->width, g_curr_font );

            /* temporary value to allow region lines to be processed properly */
            cur_reg->reg_voffset = conv_vert_unit( &cur_reg->voffset, 1, max_reg_font );

            if( cur_ban->ban_depth < cur_reg->reg_voffset + cur_reg->reg_depth ) {
                ban_reg_err( err_banreg_depth, cur_ban, NULL, cur_reg, NULL );
            }

            preprocess_script_region( cur_reg );

        }

        /****************************************************************/
        /* Re-sort the regions by voffset                               */
        /* Regions with the same voffset are linked in hoffset order    */
        /****************************************************************/

        sav_reg = cur_ban->region;          // detach current region
        cur_ban->region = sav_reg->next;    // from start of list
        sav_reg->next = NULL;
        while( sav_reg != NULL ) {
            if( cur_ban->by_line == NULL ) {    // first region
                cur_ban->by_line = mem_alloc( sizeof(ban_reg_group) );
                cur_ban->by_line->next = NULL;
                cur_ban->by_line->first = sav_reg;
                cur_ban->by_line->voffset = sav_reg->reg_voffset;
                cur_ban->by_line->max_depth = sav_reg->reg_depth;
            } else {
                old_grp = NULL;
                for( cur_grp = cur_ban->by_line; cur_grp != NULL; cur_grp = cur_grp->next ) {
                    if( cur_grp->voffset == sav_reg->reg_voffset ) { // add to group
                        old_reg = NULL;
                        for( cur_reg = cur_grp->first; cur_reg != NULL; cur_reg = cur_reg->next ) {
                            if( cur_reg->reg_hoffset == sav_reg->reg_hoffset ) {
                                ban_reg_err( err_banreg_overlap, cur_ban, NULL, cur_reg, sav_reg );
                            } else if( cur_reg->reg_hoffset > sav_reg->reg_hoffset ) {  // insert/add region
                                if( cur_reg == cur_grp->first ) {   // insert before first region
                                    sav_reg->next = cur_reg;
                                    cur_grp->first = sav_reg;
                                } else {                            // insert before current region
                                    old_reg->next = sav_reg;
                                    sav_reg->next = cur_reg;
                                }
                                if( cur_grp->max_depth < sav_reg->reg_depth ) {
                                    cur_grp->max_depth = sav_reg->reg_depth;
                                }
                                sav_reg = NULL;
                            } else {
                                old_reg = cur_reg;
                                continue;
                            }
                            break;
                        }
                        if( sav_reg != NULL ) {                 // add to end of list
                            old_reg->next = sav_reg;
                            if( cur_grp->max_depth < sav_reg->reg_depth ) {
                                cur_grp->max_depth = sav_reg->reg_depth;
                            }
                            sav_reg = NULL;
                        }
                        break;
                    } else if( cur_grp->voffset > sav_reg->reg_voffset ) {  // insert/add new group
                        if( cur_grp == cur_ban->by_line ) {                 // insert before first group
                            cur_grp = mem_alloc( sizeof(ban_reg_group) );
                            cur_grp->next = cur_ban->by_line;
                            cur_ban->by_line = cur_grp;
                            cur_grp->first = sav_reg;
                            cur_grp->voffset = sav_reg->reg_voffset;
                            cur_grp->max_depth = sav_reg->reg_depth;
                        } else {                                            // insert before current group
                            sav_grp = mem_alloc( sizeof(ban_reg_group) );
                            old_grp->next = sav_grp;
                            sav_grp->next = cur_grp;
                            sav_grp->first = sav_reg;
                            sav_grp->voffset = sav_reg->reg_voffset;
                            sav_grp->max_depth = sav_reg->reg_depth;
                        }
                        sav_reg = NULL;
                    } else {
                        old_grp = cur_grp;
                        continue;
                    }
                    break;
                }
                if( sav_reg != NULL ) {                                     // add new group at end
                    old_grp->next = mem_alloc( sizeof(ban_reg_group) );
                    old_grp = old_grp->next;
                    old_grp->next = NULL;
                    old_grp->first = sav_reg;
                    old_grp->voffset = sav_reg->reg_voffset;
                    old_grp->max_depth = sav_reg->reg_depth;
                    sav_reg = NULL;
                }
            }

            sav_reg = cur_ban->region;                  // detach current region
            if( sav_reg != NULL ) {
                cur_ban->region = sav_reg->next;        // from start of list
                sav_reg->next = NULL;
            }
        }

        /****************************************************************/
        /* Finalize the hoffset and width of each region                */
        /* This includes moving reg_hoffset if necessary so that it     */
        /* at the left boundary of the region, and using reg_h_type     */
        /* to avoid moving it more than once                            */
        /* This also includes resetting the voffset and setting the     */
        /* depth, if specified in lines, using the region's own font    */
        /****************************************************************/

        old_grp = NULL;
        for( cur_grp = cur_ban->by_line; cur_grp != NULL; cur_grp = cur_grp->next ) {
            old_reg = NULL;
            for( cur_reg = cur_grp->first; cur_reg != NULL; cur_reg = cur_reg->next ) {
                if( cur_reg == cur_grp->first ) {               // first region
                    /* set hoffset to left boundary of region */
                    if( cur_reg->width.su_u == SU_lay_extend ) {// extend to left margin
                        if( cur_reg->reg_h_type != SU_lay_left ) {  // center or right
                            cur_reg->reg_hoffset = cur_ban->ban_left_adjust;
                            cur_reg->reg_h_type = SU_lay_left;
                        }
                    } else {                                    // explicit width
                        if( cur_reg->reg_h_type == SU_lay_centre ) {
                            cur_reg->reg_hoffset = cur_reg->reg_hoffset - (cur_reg->reg_width / 2);
                            cur_reg->reg_h_type = SU_lay_left;
                        } else if( cur_reg->reg_h_type == SU_lay_right ) {
                            if( cur_reg->reg_hoffset < cur_reg->reg_width ) {
                                /* correct position to left of banner left boundary */
                                cur_reg->reg_hoffset = cur_ban->ban_left_adjust;
                            } else {
                                cur_reg->reg_hoffset = cur_reg->reg_hoffset - cur_reg->reg_width;
                            }
                            cur_reg->reg_h_type = SU_lay_left;
                        }
                    }
                }
                if( cur_reg->next != NULL ) {                   // interior region
                    if( (cur_reg->width.su_u == SU_lay_extend) &&
                            (cur_reg->next->width.su_u == SU_lay_extend) ) {    // both can't use "extend"
                        ban_reg_err( err_banreg_extend, cur_ban, NULL, cur_reg, cur_reg->next );
                    } else {
                        /* set hoffset to left boundary of region */
                        if( cur_reg->next->width.su_u != SU_lay_extend ) {  // explicit width
                            if( cur_reg->next->reg_h_type == SU_lay_centre ) {
                                cur_reg->next->reg_hoffset = cur_reg->next->reg_hoffset -
                                                                    (cur_reg->next->reg_width / 2);
                                cur_reg->next->reg_h_type = SU_lay_left;
                            } else if( cur_reg->next->reg_h_type == SU_lay_right ) {
                                cur_reg->next->reg_hoffset = cur_reg->next->reg_hoffset -
                                                                        cur_reg->next->reg_width;
                                cur_reg->next->reg_h_type = SU_lay_left;
                            }
                        }
                        /* set width to reach right boundary of region */
                        if( cur_reg->width.su_u == SU_lay_extend ) {                // next has fixed width
                            cur_reg->reg_width = cur_reg->next->reg_hoffset - cur_reg->reg_hoffset;
                        } else if( cur_reg->next->width.su_u == SU_lay_extend ) {   // cur_reg has fixed width
                            cur_reg->next->reg_hoffset = cur_reg->reg_hoffset + cur_reg->reg_width;
                        } else {                                                    // both have fixed widths
                            if( (cur_reg->reg_hoffset + cur_reg->reg_width) >
                                    cur_reg->next->reg_hoffset ) {      // overlap
                                ban_reg_err( err_banreg_overlap, cur_ban, NULL, cur_reg, cur_reg->next );
                            }
                        }
                    }
                } else {                                        // last region
                    /* set width to reach right boundary of region */
                    if( cur_reg->width.su_u == SU_lay_extend ) {// extend to right margin
                        cur_reg->reg_width = g_net_page_width - cur_ban->ban_right_adjust -
                                                                cur_reg->reg_hoffset;
                    }
                    if( (old_reg != NULL) && ((old_reg->reg_hoffset + old_reg->reg_width) >
                                cur_reg->reg_hoffset) ) {       // overlap
                        ban_reg_err( err_banreg_overlap, cur_ban, NULL, cur_reg, old_reg );
                    }
                    if( (cur_reg->reg_hoffset + cur_reg->reg_width) >
                            (g_net_page_width - cur_ban->ban_right_adjust) ) {
                        ban_reg_err( err_banreg_width, cur_ban, NULL, cur_reg, NULL );
                    }
                }
                old_reg = cur_reg;

                /* vertical attributes use the banregion font */
                cur_reg->reg_voffset = conv_vert_unit( &cur_reg->voffset, 1, cur_reg->font );
                cur_reg->reg_depth = conv_vert_unit( &cur_reg->depth, 1, cur_reg->font );
            }
        }

        /****************************************************************/
        /* Reset each group to the first region's voffset/line_height   */
        /****************************************************************/

        old_grp = NULL;
        for( sav_grp = cur_ban->by_line; sav_grp != NULL; sav_grp = sav_grp->next ) {
            sav_grp->voffset = sav_grp->first->reg_voffset;
            sav_grp->line_height = wgml_fonts[sav_grp->first->font].line_height;
        }

        /****************************************************************/
        /* Re-sort by voffset, forming new groups as needed as          */
        /* different values of voffset are encountered                  */
        /****************************************************************/

        src_grp = cur_ban->by_line;                 // pull entire list from banner
        cur_ban->by_line = NULL;
        while( src_grp != NULL ) {
            sav_grp = src_grp;
            src_grp = src_grp->next;                    // detach first group
            sav_grp->next = NULL;
            old_reg = NULL;
            sav_reg = sav_grp->first;
            while( sav_reg != NULL ) {
                if( sav_grp->voffset == sav_reg->reg_voffset ) {    // leave in group
                    old_reg = sav_reg;
                    nxt_reg = sav_reg->next;
                } else {                    // different line height: find/create matching group
                    nxt_reg = sav_reg->next;    // next region to be processed
                    if( sav_reg == sav_grp->first ) {
                        sav_grp->first = sav_reg->next; // detach first region
                    } else {
                        old_reg->next = sav_reg->next; // detach subsequent region
                    }
                    sav_reg->next = NULL;
                    old_grp = NULL;
                    for( cur_grp = cur_ban->by_line; cur_grp != NULL; cur_grp = cur_grp->next ) {
                        if( cur_grp->voffset == sav_reg->reg_voffset ) {   // append sav_reg to cur_grp
                            for( cur_reg = cur_grp->first; cur_reg->next != NULL;
                                                cur_reg = cur_reg->next ); // find last region
                            cur_reg->next = sav_reg;
                            sav_reg = NULL;
                        } else if( cur_grp->voffset > sav_reg->reg_voffset ) {  // insert/add new group
                            if( cur_grp == sav_grp ) {                  // insert before first group
                                cur_ban->by_line = mem_alloc( sizeof(ban_reg_group) );
                                cur_ban->by_line->next = cur_grp;
                                cur_ban->by_line->first = sav_reg;
                                cur_ban->by_line->voffset = sav_reg->reg_voffset;
                                cur_ban->by_line->line_height = wgml_fonts[sav_reg->font].line_height;
                                cur_ban->by_line->max_depth = sav_reg->reg_depth;
                            } else {                                            // insert before current group
                                old_grp->next = mem_alloc( sizeof(ban_reg_group) );
                                old_grp->next->next = cur_grp;
                                old_grp->next->first = sav_reg;
                                old_grp->next->voffset = sav_reg->reg_voffset;
                                old_grp->next->line_height = wgml_fonts[sav_reg->font].line_height;
                                old_grp->next->max_depth = sav_reg->reg_depth;
                            }
                            old_grp = cur_grp;
                            old_reg = sav_reg;
                            sav_reg = NULL;
                        } else {
                            old_grp = cur_grp;
                            continue;
                        }
                        break;
                    }
                    if( cur_ban->by_line == NULL ) {                // add new group at start
                        cur_ban->by_line = mem_alloc( sizeof(ban_reg_group) );
                        cur_ban->by_line->next = NULL;
                        cur_ban->by_line->first = sav_reg;
                        cur_ban->by_line->voffset = sav_reg->reg_voffset;
                        cur_ban->by_line->line_height = wgml_fonts[sav_reg->font].line_height;
                        cur_ban->by_line->max_depth = sav_reg->reg_depth;
                        sav_reg = NULL;
                    } else if( sav_reg != NULL ) {                  // add new group at end
                        old_grp->next = mem_alloc( sizeof(ban_reg_group) );
                        old_grp = old_grp->next;
                        old_grp->next = NULL;
                        old_grp->first = sav_reg;
                        old_grp->voffset = sav_reg->reg_voffset;
                        old_grp->line_height = wgml_fonts[sav_reg->font].line_height;
                        old_grp->max_depth = sav_reg->reg_depth;
                        sav_reg = NULL;
                    }
                }
                sav_reg = nxt_reg;   // reacquire next region to be processed
            }

            /* sav_grp now has only regions with the same voffset */
            old_grp = NULL;
            for( cur_grp = cur_ban->by_line; cur_grp != NULL; cur_grp = cur_grp->next ) {
                if( cur_grp->voffset == sav_grp->voffset ) {        // shouldn't be possible
                    internal_err( __FILE__, __LINE__ );
                } else if( cur_grp->voffset > sav_grp->voffset ) {  // insert/add new group
                    if( cur_grp == cur_ban->by_line ) {             // insert before first group
                        sav_grp->next = cur_ban->by_line;
                        cur_ban->by_line = sav_grp;
                    } else {                                        // insert before current group
                        old_grp->next = sav_grp;
                        sav_grp->next = cur_grp;
                    }
                    old_grp = cur_grp;
                    sav_grp = NULL;
                } else {
                    old_grp = cur_grp;
                    continue;
                }
                break;
            }
            if( sav_grp != NULL ) {                                 // add new group at end
                if( old_grp == NULL ) {                             // first group
                    cur_ban->by_line = sav_grp;
                    old_grp = sav_grp;
                } else {
                    old_grp->next = sav_grp;
                    old_grp = old_grp->next;
                }
                old_grp->next = NULL;
            }
        }

        /****************************************************************/
        /* Set max_depth for each group                                 */
        /****************************************************************/

        for( sav_grp = cur_ban->by_line; sav_grp != NULL; sav_grp = sav_grp->next ) {
            sav_grp->max_depth = sav_grp->first->reg_depth;
            for( cur_reg = sav_grp->first; cur_reg != NULL; cur_reg = cur_reg->next ) {
                if( cur_reg->reg_depth > sav_grp->max_depth ) {  // update max_depth as needed
                    sav_grp->max_depth = cur_reg->reg_depth;
                }
            }
        }

        /****************************************************************/
        /* Trap any overlapping regions not yet detected                */
        /* Specifially, those that overlap in the vertical direction    */
        /* NOTE: overlaps detected previously fall into two categories: */
        /*       exact match of voffset and hoffset                     */
        /*       horizontal overlap within the same value of voffset    */
        /*       here, it is necessary to verify that the two regions,  */
        /*       which have different voffsets, overlap horizontally    */
        /*       since, if they don't, the separate voffsets mean that  */
        /*       no overlap exists                                      */
        /*       that is, if one region has depth 2 and voffset 0, and  */
        /*       another has voffset 1, they will overlap vertically    */
        /*       only if they also overlap horizontally; otherwise,     */
        /*       will simply be placed side-by-side, one starting a bit */
        /*       lower than the other                                   */
        /****************************************************************/

        for( cur_grp = cur_ban->by_line; cur_grp != NULL; cur_grp = cur_grp->next ) {
            /* identify possible vertical overlap */
            if( (cur_grp->next != NULL) && (cur_grp->voffset + cur_grp->max_depth) > cur_grp->next->voffset ) {
                /* horizontal overlap is also needed */
                for( cur_reg = cur_grp->first; cur_reg != NULL; cur_reg = cur_reg->next ) {
                    /* only proceed if this cur_reg may overlap a region in cur_grp->next */
                    if( (cur_reg->reg_voffset + cur_reg->reg_depth) > cur_grp->next->voffset ) {
                        /* using sav_reg to preserve the value of cur_reg */
                        for( sav_reg = cur_grp->next->first; sav_reg != NULL; sav_reg = sav_reg->next ) {
                            /* test for overlap */
                            if( ((sav_reg->reg_hoffset + sav_reg->reg_width) > cur_reg->reg_hoffset) &&
                                    ((sav_reg->reg_voffset + sav_reg->reg_depth) > cur_reg->reg_voffset) ) {
                                ban_reg_err( err_banreg_overlap, cur_ban, NULL, cur_reg, sav_reg );
                            }
                        }
                    }
                }
            }
        }

        /****************************************************************/
        /* At this point the groups are in true voffset order           */
        /* Finalize the sort by separating each voffset by line_height  */
        /****************************************************************/

        src_grp = cur_ban->by_line;                 // pull entire list from banner
        cur_ban->by_line = NULL;
        while( src_grp != NULL ) {
            sav_grp = src_grp;
            src_grp = src_grp->next;                // detach first group
            sav_grp->next = NULL;
            old_reg = NULL;
            sav_reg = sav_grp->first;
            while( sav_reg != NULL ) {
                nxt_reg = sav_reg->next;    // next region to be processed
                if( sav_grp->line_height == wgml_fonts[sav_reg->font].line_height ) {   // keep in sav_grp
                    old_reg = sav_reg;
                } else {                    // different line height: move sav_reg from sav_grp to a new grp
                    if( sav_reg == sav_grp->first ) {
                        sav_grp->first = sav_reg->next; // detach first region
                    } else {
                        old_reg->next = sav_reg->next; // detach subsequent region
                    }
                    sav_reg->next = NULL;
                    old_grp = NULL;
                    for( cur_grp = cur_ban->by_line; cur_grp != NULL; cur_grp = cur_grp->next ) {
                        if( cur_grp->line_height == wgml_fonts[sav_reg->font].line_height ) {   // append sav_reg to cur_grp
                            for( cur_reg = cur_grp->first; cur_reg->next != NULL;
                                                cur_reg = cur_reg->next );  // find last region
                            cur_reg->next = sav_reg;
                            sav_reg = NULL;
                        } else if( cur_grp->line_height > wgml_fonts[sav_reg->font].line_height ) {    // insert/add new group
                            if( cur_grp == sav_grp ) {                  // insert before first group
                                cur_ban->by_line = mem_alloc( sizeof(ban_reg_group) );
                                cur_ban->by_line->next = cur_grp;
                                cur_ban->by_line->first = sav_reg;
                                cur_ban->by_line->voffset = sav_reg->reg_voffset;
                                cur_ban->by_line->line_height = wgml_fonts[sav_reg->font].line_height;
                                cur_ban->by_line->max_depth = sav_reg->reg_depth;
                            } else {                                            // insert before current group
                                old_grp->next = mem_alloc( sizeof(ban_reg_group) );
                                old_grp->next->next = cur_grp;
                                old_grp->next->first = sav_reg;
                                old_grp->next->voffset = sav_reg->reg_voffset;
                                old_grp->next->line_height = wgml_fonts[sav_reg->font].line_height;
                                old_grp->next->max_depth = sav_reg->reg_depth;
                            }
                            old_grp = cur_grp;
                            old_reg = sav_reg;
                            sav_reg = NULL;
                        } else {
                            old_grp = cur_grp;
                            continue;
                        }
                        break;
                    }
                    if( cur_ban->by_line == NULL ) {                // add new group at start
                        cur_ban->by_line = mem_alloc( sizeof(ban_reg_group) );
                        cur_ban->by_line->next = NULL;
                        cur_ban->by_line->first = sav_reg;
                        cur_ban->by_line->voffset = sav_reg->reg_voffset;
                        cur_ban->by_line->line_height = wgml_fonts[sav_reg->font].line_height;
                        cur_ban->by_line->max_depth = sav_reg->reg_depth;
                        sav_reg = NULL;
                    } else if( sav_reg != NULL ) {                  // add new group at end
                        old_grp->next = mem_alloc( sizeof(ban_reg_group) );
                        old_grp = old_grp->next;
                        old_grp->next = NULL;
                        old_grp->first = sav_reg;
                        old_grp->voffset = sav_reg->reg_voffset;
                        old_grp->line_height = wgml_fonts[sav_reg->font].line_height;
                        old_grp->max_depth = sav_reg->reg_depth;
                        sav_reg = NULL;
                    }
                }
                sav_reg = nxt_reg;   // reacquire next region to be processed
            }

            /* sav_grp now has only regions with the same voffset */
            old_grp = NULL;
            for( cur_grp = cur_ban->by_line; cur_grp != NULL; cur_grp = cur_grp->next ) {
                if( (cur_grp->voffset + cur_grp->line_height) ==
                        (sav_grp->voffset + sav_grp->line_height) ) {        // shouldn't be possible
                    internal_err( __FILE__, __LINE__ );
                } else if( (cur_grp->voffset + cur_grp->line_height) >
                        (sav_grp->voffset + sav_grp->line_height) ) {  // insert/add new group
                    if( cur_grp == cur_ban->by_line ) {             // insert before first group
                        sav_grp->next = cur_ban->by_line;
                        cur_ban->by_line = sav_grp;
                    } else {                                        // insert before current group
                        old_grp->next = sav_grp;
                        sav_grp->next = cur_grp;
                    }
                    old_grp = cur_grp;
                    sav_grp = NULL;
                } else {
                    old_grp = cur_grp;
                    continue;
                }
                break;
            }
            if( sav_grp != NULL ) {                                 // add new group at end
                if( old_grp == NULL ) {                             // first group
                    cur_ban->by_line = sav_grp;
                    old_grp = sav_grp;
                } else {
                    old_grp->next = sav_grp;
                    old_grp = old_grp->next;
                }
                old_grp->next = NULL;
            }
        }

        /* Set top_ban and bot_ban */

        switch( cur_ban->place ) {
        case bottom_place :
        case boteven_place :
        case botodd_place :
            if( cur_ban->ban_depth > ban_bot_depth ) {
                ban_bot_depth = cur_ban->ban_depth;
                bot_ban = cur_ban;
            }
            break;
        case top_place :
        case topeven_place :
        case topodd_place :
            if( cur_ban->ban_depth > ban_top_depth ) {
                ban_top_depth = cur_ban->ban_depth;
                top_ban = cur_ban;
            }
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }

    /* overall banner error checks */
    if( (ban_bot_depth + ban_top_depth) > g_page_depth ) {
        ban_reg_err( err_ban_depth2, bot_ban, top_ban, NULL, NULL );
    }

    return;
}

/***************************************************************************/
/*  Layout end processing / document start processing                      */
/*  will be called either before the first non LAYOUT tag is processed,    */
/*  or when the first line without tags is found, i.e. produces output     */
/***************************************************************************/

void    do_layout_end_processing( void )
{
    size_t          spc_cnt;
    size_t          txt_cnt;

    /***********************************************************************/
    /*  Since the LAYOUT blocks are only processed on the first pass, most */
    /*  of this function is similarly restricted to the first pass.        */
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

        /* If a note_string exists, split it into text and spaces */

        txt_cnt = strlen( layout_work.note.string );
        if( txt_cnt > 0 ) {
            spc_cnt = 0;
            while( layout_work.note.string[txt_cnt - 1] == ' ' ) {
                txt_cnt--;
                spc_cnt++;
            }
            strncpy( layout_work.note.text, layout_work.note.string, txt_cnt );
            layout_work.note.text[txt_cnt] = '\0';
            memset( layout_work.note.spaces, ' ', spc_cnt );
            layout_work.note.spaces[spc_cnt] = '\0';
        }

        finish_lists();
        finish_banners();

        ProcFlags.justify = layout_work.defaults.justify;

        layout_work.defaults.def_gutter = conv_hor_unit( &layout_work.defaults.gutter, g_curr_font );

        /*******************************************************************/
        /*  Since we have given BODY a columns value, this ensures that it */
        /*  is set to the default value.                                   */
        /*******************************************************************/

        layout_work.body.columns = layout_work.defaults.columns;

        /*******************************************************************/
        /*  Since we have given BODY a spacing value, this ensures that it */
        /*  is set to the default value.                                   */
        /*******************************************************************/

        layout_work.hx.hx_sect[hds_body].spacing = layout_work.defaults.spacing;

        /*******************************************************************/
        /*  This is a good place to start document processing.             */
        /*******************************************************************/

        g_info_lm( inf_fmt_start );

        fb_document();                  // DOCUMENT :PAUSE & :INIT processing.

        set_banners();                  // prepare banners for selection
        set_pgnum_style();              // set the pgnum_style values
        reset_t_page();                 // set up t_page
    }

    /* These items must be done at this point on every pass */

    g_text_spacing = layout_work.defaults.spacing;
    ProcFlags.fb_document_done = true;  // done on pass 1
}


