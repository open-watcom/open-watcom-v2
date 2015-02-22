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
* Description: WGML implement :IXHEAD tag for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :IXHEAD   attributes                                                    */
/***************************************************************************/
const   lay_att     ixhead_att[7] =
    { e_pre_skip, e_post_skip, e_font, e_indent, e_ixhead_frame,
      e_header, e_dummy_zero };


/*********************************************************************************/
/*Define the characteristics of the index headings. In most cases, the index     */
/*heading is the letter which starts the index terms following it.               */
/*:IXHEAD                                                                        */
/*        pre_skip = 2                                                           */
/*        post_skip = 0                                                          */
/*        font = 2                                                               */
/*        indent = 0                                                             */
/*        frame = box                                                            */
/*        header = yes                                                           */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the index heading. The pre-skip will be merged with the previous               */
/*document entity's post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the index heading. The post-skip will be merged with the next                  */
/*document entity's pre-skip value. If a post-skip occurs at the end of          */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page.                                                          */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the index heading. The font value is linked to the indent, pre_skip            */
/*and post_skip attributes (see "Font Linkage" on page 77).                      */
/*                                                                               */
/*indent The indent attribute accepts any valid horizontal space unit. The       */
/*attribute space value is added to the current left margin before the           */
/*index heading is generated in the index. The left margin is reset to           */
/*its previous value after the heading is generated.                             */
/*                                                                               */
/*frame This attribute accepts the values rule, box, none, and 'character        */
/*string'. The specified attribute value determines the type of framing          */
/*around the index heading. See the discussion of the frame attribute            */
/*under "FIG" on page 92 for an explanation of the attribute values.             */
/*                                                                               */
/*header This attribute accepts the keyword values yes and no. If 'no' is        */
/*specified, the index heading is not displayed. The font and frame              */
/*attributes are ignored, and the pre and post skip values are merged.           */
/*********************************************************************************/

/***************************************************************************/
/*  lay_ixhead                                                             */
/***************************************************************************/

void    lay_ixhead( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    tag = tag;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_ixhead ) {
        ProcFlags.lay_xxx = el_ixhead;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = ixhead_att[k]; curr > 0; k++, curr = ixhead_att[k] ) {
            if( curr == e_ixhead_frame ) {
                curr = e_frame;         // use correct externalname
            }

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ixhead.pre_skip );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ixhead.post_skip );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.ixhead.font );
                    if( layout_work.ixhead.font >= wgml_font_cnt ) {
                        layout_work.ixhead.font = 0;
                    }
                    break;
                case   e_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ixhead.indent );
                    break;
                case   e_frame:
                    cvterr = i_default_frame( p, curr,
                                           &layout_work.ixhead.frame );
                    break;
                case   e_header:
                    cvterr = i_yes_no( p, curr,
                                           &layout_work.ixhead.header );
                    break;
                default:
                    out_msg( "WGML logic error.\n");
                    cvterr = true;
                    break;
                }
                if( cvterr ) {          // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            err_count++;
            g_err( err_att_name_inv );
            file_mac_info();
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop;
    return;
}

