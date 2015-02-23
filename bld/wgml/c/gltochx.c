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
* Description: WGML implement :TOCH0 - :TOCH6 tags for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

#include "clibext.h"

/***************************************************************************/
/*   :TOCHx    attributes                                                     */
/***************************************************************************/
const   lay_att     tochx_att[9] =
    { e_group, e_indent, e_skip, e_pre_skip, e_post_skip,
      e_font, e_align, e_display_in_toc, e_dummy_zero };

/***********************************************************************************/
/*Define the characteristics of a table of contents heading, where n is between    */
/*zero and six inclusive.                                                          */
/*:TOCH0                                                                           */
/*        group = 0                                                                */
/*        indent = 0                                                               */
/*        skip = 1                                                                 */
/*        pre_skip = 1                                                             */
/*        post_skip = 1                                                            */
/*        font = 0                                                                 */
/*        align = 0                                                                */
/*        display_in_toc = yes                                                     */
/*                                                                                 */
/*group The group attribute accepts any non-negative number between 0 and          */
/*9. The group value determines which set of table of contents are                 */
/*processed bye the group of level n table of contents heading entries.            */
/*tags/control words.                                                              */
/*                                                                                 */
/*indent The indent attribute accepts any valid horizontal space unit. The         */
/*attribute space value is added to the current left margin before the             */
/*table of contents entry is produced. After all of the subentries under           */
/*the current entry are produced, the left margin is reset to its previous         */
/*value.                                                                           */
/*                                                                                 */
/*skip This attribute accepts vertical space units. A zero value means that        */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped                      */
/*between the individual entries within the group of level n table of              */
/*contents heading entries.                                                        */
/*                                                                                 */
/*pre_skip This attribute accepts vertical space units. A zero value means that    */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped before               */
/*the group of level n table of contents heading entries. The pre-skip             */
/*will be merged with the previous document entity's post-skip value.              */
/*if( a pre-skip occurs at the beginning of an output page, the pre-skip ) {       */
/*value has no effect.                                                             */
/*                                                                                 */
/*post_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped after                */
/*the group of level n table of contents heading entries. The post-skip            */
/*will be merged with the next document entity's pre-skip value. If a              */
/*post-skip occurs at the end of an output page, any remaining part of             */
/*the skip is not carried over to the next output page.                            */
/*                                                                                 */
/*align This attribute accepts any valid horizontal space unit. The align          */
/*value specifies the amount of space reserved for the table of                    */
/*contents heading. After the table of contents heading is produced,               */
/*the align value is added to the current left margin. The left margin             */
/*will be reset to its previous value after the heading entry.                     */
/*                                                                                 */
/*font This attribute accepts a non-negative integer number. If a font             */
/*number is used for which no font has been defined, WATCOM                        */
/*Script/GML will use font zero. The font numbers from zero to three               */
/*correspond directly to the highlighting levels specified by the                  */
/*highlighting phrase GML tags. The font attribute defines the font of             */
/*the heading produced in the table of contents entry.                             */
/*display_in_toc This attribute accepts the keyword values yes and no. The heading */
/*for the table of contents entry is not produced when the value no is             */
/*specified. The entries pre and post skips are still generated.                   */
/***********************************************************************************/


/***************************************************************************/
/*  lay_tochx                                                              */
/***************************************************************************/

void    lay_tochx( lay_tag tag )
{
    char        *   p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    int             cvterr;
    int             hx_l;
    int             hx;

    p = scan_start;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    switch( tag ) {
    case LAY_TAG_TOCH0:
        hx_l = el_toch0;
        break;
    case LAY_TAG_TOCH1:
        hx_l = el_toch1;
        break;
    case LAY_TAG_TOCH2:
        hx_l = el_toch2;
        break;
    case LAY_TAG_TOCH3:
        hx_l = el_toch3;
        break;
    case LAY_TAG_TOCH4:
        hx_l = el_toch4;
        break;
    case LAY_TAG_TOCH5:
        hx_l = el_toch5;
        break;
    case LAY_TAG_TOCH6:
        hx_l = el_toch6;
        break;
    default:
        hx_l = el_toch6;
        out_msg( "WGML logic error in glhx.c\n" );
        err_count++;
        break;
    }
    if( ProcFlags.lay_xxx != hx_l ) {
        ProcFlags.lay_xxx = hx_l;
    }

    hx = hx_l - el_toch0;     // construct TOCHx level

    cc = get_lay_sub_and_value( &l_args );  // get one with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = tochx_att[k]; curr > 0; k++, curr = tochx_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_group:
                    cvterr = i_int8( p, curr, &layout_work.tochx[hx].group );
                    break;
                case   e_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.tochx[hx].indent );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.tochx[hx].skip );
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.tochx[hx].pre_skip );
                    break;
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.tochx[hx].post_skip );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.tochx[hx].font );
                    if( layout_work.tochx[hx].font >= wgml_font_cnt ) {
                        layout_work.tochx[hx].font = 0;
                    }
                    break;
                case   e_align:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.tochx[hx].align );
                    break;
                case   e_display_in_toc:
                    cvterr = i_yes_no( p, curr,
                                     &layout_work.tochx[hx].display_in_toc );
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
        cc = get_lay_sub_and_value( &l_args );  // get one with value
    }
    scan_start = scan_stop;
    return;
}

