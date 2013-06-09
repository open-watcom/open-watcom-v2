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
* Description: WGML implement :ADDRESS and :ALINE tags for LAYOUT processing
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :ADDRESS  attributes                                                  */
/***************************************************************************/
const   lay_att     address_att[6] =
    { e_left_adjust, e_right_adjust, e_page_position, e_font, e_pre_skip,
      e_dummy_zero };

/***************************************************************************/
/*   :ALINE  attribute                                                     */
/***************************************************************************/
const   lay_att     aline_att[2] =
    { e_skip, e_dummy_zero };


/***********************************************************************************/
/*Define the characteristics of the address entity.                                */
/*:ADDRESS                                                                         */
/*        left_adjust = 0                                                          */
/*        right_adjust = '1i'                                                      */
/*        page_position = right                                                    */
/*        font = 0                                                                 */
/*        pre_skip = 2                                                             */
/*                                                                                 */
/*left_adjust The left_adjust attribute accepts any valid horizontal space unit.   */
/*The left margin is set to the page left margin plus the specified left           */
/*adjustment.                                                                      */
/*                                                                                 */
/*right_adjust The right_adjust attribute accepts any valid horizontal space unit. */
/*The right margin is set to the page right margin minus the specified             */
/*right adjustment.                                                                */
/*                                                                                 */
/*page_position This attribute accepts the values left, right, center, and centre. */
/*The position of the address between the left and right margins is                */
/*determined by the value selected. If left is the attribute value, the            */
/*text is output at the left margin. If right is the attribute value, the          */
/*text is output next to the right margin. When center or centre is                */
/*specified, the text is centered between the left and right margins.              */
/*                                                                                 */
/*font This attribute accepts a non-negative integer number. If a font             */
/*number is used for which no font has been defined, WATCOM                        */
/*Script/GML will use font zero. The font numbers from zero to three               */
/*correspond directly to the highlighting levels specified by the                  */
/*highlighting phrase GML tags. The font attribute defines the font of             */
/*the address. The font value is linked to the left_adjust, right_adjust           */
/*and pre_skip attributes of the :ADDRESS tag, and the skip attribute              */
/*of the :ALINE tag (see "Font Linkage" on page 77).                               */
/*                                                                                 */
/*pre_skip This attribute accepts vertical space units. A zero value means that    */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped before               */
/*the address. The pre-skip will be merged with the previous                       */
/*document entity's post-skip value. If a pre-skip occurs at the                   */
/*beginning of an output page, the pre-skip value has no effect.                   */
/***********************************************************************************/

/****************************************************************************/
/*Define the characteristics of the address line entity.                    */
/*:ALINE                                                                    */
/*        skip = 1                                                          */
/*                                                                          */
/*skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied  */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for     */
/*more information). The resulting amount of space is skipped               */
/*between address lines.                                                    */
/****************************************************************************/


/***************************************************************************/
/*  lay_address                                                            */
/***************************************************************************/

void    lay_address( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    entry = entry;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_address ) {
        ProcFlags.lay_xxx = el_address;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = address_att[k]; curr > 0; k++, curr = address_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_left_adjust:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.address.left_adjust );
                    break;
                case   e_right_adjust:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.address.right_adjust );
                    break;
                case   e_page_position:
                    cvterr = i_page_position( p, curr,
                                          &layout_work.address.page_position );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.address.font );
                    if( layout_work.address.font >= wgml_font_cnt ) {
                        layout_work.address.font = 0;
                    }
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.address.pre_skip );
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
    scan_start = scan_stop + 1;
    return;
}

/***************************************************************************/
/*  lay_aline                                                              */
/***************************************************************************/

void    lay_aline( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    bool                cvterr;

    entry = entry;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_aline ) {
        ProcFlags.lay_xxx = el_aline;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = true;
        for( k = 0, curr = aline_att[k]; curr > 0; k++, curr = aline_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_skip:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.aline.skip );
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
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop + 1;
    return;
}

