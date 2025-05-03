/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description: WGML implement :FIG tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :FIG   attributes                                                     */
/***************************************************************************/
const   lay_att     fig_att[9] =
    { e_left_adjust, e_right_adjust, e_pre_skip, e_post_skip, e_spacing,
      e_font, e_default_place, e_default_frame, e_dummy_zero };


/**********************************************************************************/
/*Define the characteristics of the figure entity.                                */
/*                                                                                */
/*:FIG                                                                            */
/*        left_adjust = 0                                                         */
/*        right_adjust = 0                                                        */
/*        pre_skip = 2                                                            */
/*        post_skip = 0                                                           */
/*        spacing = 1                                                             */
/*        font = 0                                                                */
/*        default_place = top                                                     */
/*        default_frame = rule                                                    */
/*                                                                                */
/*left_adjust The left_adjust attribute accepts any valid horizontal space unit.  */
/*The left margin is set to the page left margin plus the specified left          */
/*adjustment.                                                                     */
/*                                                                                */
/*right_adjust The right_adjust attribute accepts any valid horizontal space unit.*/
/*The right margin is set to the page right margin minus the specified            */
/*right adjustment.                                                               */
/*                                                                                */
/*pre_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped before              */
/*the figure. The pre-skip will be merged with the previous document              */
/*entity's post-skip value. If a pre-skip occurs at the beginning of an           */
/*output page, the pre-skip value has no effect.                                  */
/*                                                                                */
/*post_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped after               */
/*the figure. The post-skip will be merged with the next document                 */
/*entity's pre-skip value. If a post-skip occurs at the end of an output          */
/*page, any remaining part of the skip is not carried over to the next            */
/*output page. figure.                                                            */
/*                                                                                */
/*spacing This attribute accepts a positive integer number. The spacing           */
/*determines the number of blank lines that are output between text               */
/*lines. If the line spacing is two, each text line will take two lines in        */
/*the output. The number of blank lines between text lines will                   */
/*therefore be the spacing value minus one. The spacing attribute                 */
/*defines the line spacing within the figure.                                     */
/*                                                                                */
/*font This attribute accepts a non-negative integer number. If a font            */
/*number is used for which no font has been defined, WATCOM                       */
/*Script/GML will use font zero. The font numbers from zero to three              */
/*correspond directly to the highlighting levels specified by the                 */
/*highlighting phrase GML tags. The font attribute defines the font of            */
/*the figure text. The font value is linked to the left_adjust,                   */
/*right_adjust, pre_skip and post_skip attributes (see "Font Linkage"             */
/*on page 77).                                                                    */
/*                                                                                */
/*default_place This attribute accepts the values top, bottom, and inline. The    */
/*specified attribute value is used as the default value for the place            */
/*attribute of the GML figure tag.                                                */
/*                                                                                */
/*default_frame This attribute accepts the values rule, box, none, and 'character */
/*string'. The specified attribute value is used as the default value for         */
/*the frame attribute of the GML figure tag. See the discussion about             */
/*the frame attribute under "FIG" on page 92 for an explanation of the            */
/*attribute values                                                                */
/**********************************************************************************/

/***************************************************************************/
/*  lay_fig                                                                */
/***************************************************************************/

void    lay_fig( const gmltag * entry )
{
    char        *   p;
    condcode        cc;
    int             cvterr;
    int             k;
    lay_att         curr;

    (void)entry;

    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_fig ) {
        ProcFlags.lay_xxx = el_fig;
    }
    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = fig_att[k]; curr > 0; k++, curr = fig_att[k] ) {

            if( !strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.fig.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case   e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.fig.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr, &layout_work.fig.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr, &layout_work.fig.post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_spacing( p, curr, &layout_work.fig.spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_font_number( p, curr, &layout_work.fig.font );
                    if( layout_work.fig.font >= wgml_font_cnt ) {
                        layout_work.fig.font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_default_place:
                    if( AttrFlags.default_place ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_place( p, curr,
                                      &layout_work.fig.default_place );
                    AttrFlags.default_place = true;
                    break;
                case   e_default_frame:
                    if( AttrFlags.default_frame ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_default_frame( p, curr,
                                              &layout_work.fig.default_frame );
                    AttrFlags.default_frame = true;
                    break;
                default:
                    internal_err( __FILE__, __LINE__ );
                }
                if( cvterr ) {          // there was an error
                    xx_err( err_att_val_inv );
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err( err_att_name_inv );
        }
        cc = get_attr_and_value();            // get att with value
    }
    scan_start = scan_stop + 1;
    return;
}

