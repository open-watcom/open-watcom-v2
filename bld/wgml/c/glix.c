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
* Description: WGML implement :I1 - :I3 tags for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

#include "clibext.h"

/***************************************************************************/
/*   :I1 - :I3 attributes                                                  */
/***************************************************************************/
const   lay_att     ix_att[9] =
    { e_pre_skip, e_post_skip, e_skip, e_font, e_indent, e_wrap_indent,
      e_index_delim, e_string_font, e_dummy_zero };

/**********************************************************************************/
/*Define the characteristics of an index entry level, where n is 1, 2, or 3.      */
/*The string_font attribute is only valid with index entry levels one and two.    */
/*                                                                                */
/*:I1                                                                             */
/*        pre_skip = 1                                                            */
/*        post_skip = 1                                                           */
/*        skip = 1                                                                */
/*        font = 0                                                                */
/*        indent = 0                                                              */
/*        wrap_indent = '0.4i'                                                    */
/*        index_delim = " "                                                       */
/*        string_font = 0                                                         */
/*                                                                                */
/*pre_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped before              */
/*the index entry. The pre-skip will be merged with the previous                  */
/*document entity's post-skip value. If a pre-skip occurs at the                  */
/*beginning of an output page, the pre-skip value has no effect.                  */
/*                                                                                */
/*post_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped after               */
/*the index entry. The post-skip will be merged with the next                     */
/*document entity's pre-skip value. If a post-skip occurs at the end of           */
/*an output page, any remaining part of the skip is not carried over to           */
/*the next output page.                                                           */
/*                                                                                */
/*skip This attribute accepts vertical space units. A zero value means that       */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped                     */
/*between each entry in an index level.                                           */
/*                                                                                */
/*font This attribute accepts a non-negative integer number. If a font            */
/*number is used for which no font has been defined, WATCOM                       */
/*Script/GML will use font zero. The font numbers from zero to three              */
/*correspond directly to the highlighting levels specified by the                 */
/*highlighting phrase GML tags. The font attribute defines the font of            */
/*the index entry. The font value is linked to the wrap_indent, skip,             */
/*pre_skip and post_skip attributes (see "Font Linkage" on page 77).              */
/*                                                                                */
/*indent The indent attribute accepts any valid horizontal space unit. The        */
/*attribute space value is added to the current left margin before the            */
/*index entry is produced in the index. After the index entries under             */
/*the current entry are produced, the left margin is reset to its previous        */
/*value.                                                                          */
/*                                                                                */
/*wrap_indent This attribute accepts as a value any valid horizontal space unit.  */
/*if the list of references for an index entry in the index does not fit on       */
/*one output line, the specified attribute value indicates the                    */
/*indentation that is to occur on the following output lines.                     */
/*                                                                                */
/*index_delim This attribute accepts a string value which is placed between the   */
/*index text and the index page number(s). If the text, page number(s)            */
/*and delimiter does not fit on one output line, the delimiter text is not        */
/*used.                                                                           */
/*                                                                                */
/*string_font This attribute accepts a positive integer number, and is valid with */
/*the :i1 and :i2 layout tags. If a font number is used for which no              */
/*font has been defined, WATCOM Script/GML will use font zero.                    */
/*The font numbers from zero to three correspond directly to                      */
/*highlighting levels specified by the highlighting phrase GML tags.              */
/*The string_font attribute defines the font of the the see and see_also          */
/*attribute strings defined by the :INDEX layout tag.                             */
/**********************************************************************************/



/***************************************************************************/
/*  lay_ix          for :I1 - :I3                                          */
/***************************************************************************/

void    lay_ix( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;
    int                 ix_l;
    int                 ix;

    p = scan_start;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    switch( tag ) {
    case LAY_TAG_I1:
        ix_l = el_i1;
        break;
    case LAY_TAG_I2:
        ix_l = el_i2;
        break;
    case LAY_TAG_I3:
        ix_l = el_i3;
        break;
    default:
        ix_l = el_i3;
        out_msg( "WGML logic error in glix.c\n" );
        err_count++;
        break;
    }
    if( ProcFlags.lay_xxx != ix_l ) {
        ProcFlags.lay_xxx = ix_l;
    }
    ix = ix_l - el_i1;      // construct Ix level  0 - 2

    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = ix_att[k]; curr > 0; k++, curr = ix_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ix[ix].pre_skip );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ix[ix].post_skip );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ix[ix].skip );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.ix[ix].font );
                    if( layout_work.ix[ix].font >= wgml_font_cnt ) {
                        layout_work.ix[ix].font = 0;
                    }
                    break;
                case   e_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ix[ix].indent );
                    break;
                case   e_wrap_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.ix[ix].wrap_indent );
                    break;
                case   e_index_delim:
                    cvterr = i_xx_string( p, curr, layout_work.ix[ix].index_delim );
                    break;
                case   e_string_font:
                    if( ix < 2 ) {
                        cvterr = i_font_number( p, curr, &layout_work.ix[ix].string_font );
                        if( layout_work.ix[ix].string_font >= wgml_font_cnt ) {
                            layout_work.ix[ix].string_font = 0;
                        }
                    }
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
