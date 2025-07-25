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
static const lay_att    fig_att[] = {
    e_left_adjust, e_right_adjust, e_pre_skip, e_post_skip, e_spacing,
    e_font, e_default_place, e_default_frame
};


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
    char                *p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;

    p = g_scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( fig_att ); k++ ) {
            curr = fig_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.fig.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.fig.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.fig.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.fig.post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_spacing( p, &attr_val, &layout_work.fig.spacing );
                    AttrFlags.spacing = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.fig.font );
                    AttrFlags.font = true;
                    break;
                case e_default_place:
                    if( AttrFlags.default_place ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_place( p, &attr_val,
                                      &layout_work.fig.default_place );
                    AttrFlags.default_place = true;
                    break;
                case e_default_frame:
                    if( AttrFlags.default_frame ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_default_frame( p, &attr_val,
                                              &layout_work.fig.default_frame );
                    AttrFlags.default_frame = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                if( cvterr ) {          // there was an error
                    xx_err_exit( ERR_ATT_VAL_INV );
                    /* never return */
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err_exit( ERR_ATT_NAME_INV );
            /* never return */
        }
    }
    g_scandata.s = g_scandata.e;
    return;
}



/***************************************************************************/
/*   :FIG       output figur attribute values                              */
/***************************************************************************/
void    put_lay_fig( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":FIG\n" );

    for( k = 0; k < TABLE_SIZE( fig_att ); k++ ) {
        curr = fig_att[k];
        switch( curr ) {
        case e_left_adjust:
            o_space_unit( fp, curr, &lay->fig.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( fp, curr, &lay->fig.right_adjust );
            break;
        case e_pre_skip:
            o_space_unit( fp, curr, &lay->fig.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( fp, curr, &lay->fig.post_skip );
            break;
        case e_spacing:
            o_spacing( fp, curr, &lay->fig.spacing );
            break;
        case e_font:
            o_font_number( fp, curr, &lay->fig.font );
            break;
        case e_default_place:
            o_place( fp, curr, &lay->fig.default_place );
            break;
        case e_default_frame:
            o_default_frame( fp, curr, &lay->fig.default_frame );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}
