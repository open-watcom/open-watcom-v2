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
* Description: WGML implement :INDEX tag for LAYOUT processing
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :INDEX attributes                                                     */
/***************************************************************************/
const   lay_att     index_att[14] =
    { e_post_skip, e_pre_top_skip, e_left_adjust, e_right_adjust,
      e_spacing, e_columns, e_see_string, e_see_also_string, e_header,
      e_index_string, e_page_eject, e_page_reset, e_font, e_dummy_zero };

/***********************************************************************************/
/*Define the characteristics of the index section.                                 */
/*                                                                                 */
/*:INDEX                                                                           */
/*        post_skip = 0                                                            */
/*        pre_top_skip = 0                                                         */
/*        left_adjust = 0                                                          */
/*        right_adjust = 0                                                         */
/*        spacing = 1                                                              */
/*        columns = 1                                                              */
/*        see_string = "See "                                                      */
/*        see_also_string = "See also "                                            */
/*        header = no                                                              */
/*        index_string = "Index"                                                   */
/*        page_eject = yes                                                         */
/*        page_reset = no                                                          */
/*        font = 1                                                                 */
/*                                                                                 */
/*post_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped after                */
/*the heading. The post-skip will be merged with the next document                 */
/*entity's pre-skip value. If a post-skip occurs at the end of an output           */
/*page, any remaining part of the skip is not carried over to the next             */
/*output page. If the index heading is not displayed (the header                   */
/*attribute has a value of NO), the post-skip value has no effect.                 */
/*                                                                                 */
/*pre_top_skip This attribute accepts vertical space units. A zero value means     */
/*that no lines are skipped. If the skip value is a line unit, it is multiplied    */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped before               */
/*the heading. The pre-top-skip will be merged with the previous                   */
/*document entity's post-skip value. The specified space is still                  */
/*skipped at the beginning of a new page.                                          */
/*                                                                                 */
/*left_adjust The left_adjust attribute accepts any valid horizontal space unit.   */
/*The left margin is set to the page left margin plus the specified left           */
/*adjustment.                                                                      */
/*                                                                                 */
/*right_adjust The right_adjust attribute accepts any valid horizontal space unit. */
/*The right margin is set to the page right margin minus the specified             */
/*right adjustment.                                                                */
/*                                                                                 */
/*spacing This attribute accepts a positive integer number. The spacing            */
/*determines the number of blank lines that are output between text                */
/*lines. If the line spacing is two, each text line will take two lines in         */
/*the output. The number of blank lines between text lines will                    */
/*therefore be the spacing value minus one. The spacing attribute                  */
/*defines the line spacing within the index.                                       */
/*                                                                                 */
/*columns The columns attribute accepts a positive integer number. The             */
/*columns value determines how many columns are created for the                    */
/*index.                                                                           */
/*                                                                                 */
/*see_string This attribute accepts a character string. The specified string       */
/*precedes any see text generated in the index.                                    */
/*                                                                                 */
/*see_also_string This attribute accepts a character string. The specified string  */
/*precedes any see also text generated in the index.                               */
/*                                                                                 */
/*header The header attribute accepts the keyword values yes and no. If the        */
/*value yes is specified, the index heading is generated. If the value             */
/*no is specified, the header text is not generated.                               */
/*                                                                                 */
/*index_string This attribute accepts a character string. If the index header is   */
/*generated, the specified string is used for the heading text.                    */
/*                                                                                 */
/*page_eject This attribute accepts the keyword values yes, no, odd, and even. If  */
/*the value no is specified, the heading is one column wide and is not             */
/*forced to a new page. The heading is always placed on a new page                 */
/*when the value yes is specified. Values other than no cause the                  */
/*heading to be treated as a page wide heading in a multi-column                   */
/*document.                                                                        */
/*The values odd and even will place the heading on a new page if the              */
/*parity (odd or even) of the current page number does not match the               */
/*specified value. When two headings appear together, the attribute                */
/*value stop_eject=yes of the :heading layout tag will normally                    */
/*prevent the the second heading from going to the next page. The                  */
/*odd and even values act on the heading without regard to the                     */
/*stop_eject value.                                                                */
/*                                                                                 */
/*page_reset This attribute accepts the keyword values yes and no. If the value    */
/*yes is specified, the page number is reset to one at the beginning of            */
/*the section.                                                                     */
/*                                                                                 */
/*font This attribute accepts a non-negative integer number. If a font             */
/*number is used for which no font has been defined, WATCOM                        */
/*Script/GML will use font zero. The font numbers from zero to three               */
/*correspond directly to the highlighting levels specified by the                  */
/*highlighting phrase GML tags. The font attribute defines the font of             */
/*the header attribute value. The font value is linked to the                      */
/*left_adjust, right_adjust, pre_top_skip and post_skip attributes (see            */
/*"Font Linkage" on page 77).                                                      */
/***********************************************************************************/


/***************************************************************************/
/*  lay_index      for :INDEX                                              */
/***************************************************************************/

void    lay_index( const gmltag * entry )
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
    if( ProcFlags.lay_xxx != el_index ) {
        ProcFlags.lay_xxx = el_index;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = index_att[k]; curr > 0; k++, curr = index_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.index.post_skip );
                    break;
                case   e_pre_top_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.index.pre_top_skip );
                    break;
                case   e_left_adjust:
                    cvterr = i_space_unit( p, curr, &layout_work.index.left_adjust );
                    break;
                case   e_right_adjust:
                    cvterr = i_space_unit( p, curr, &layout_work.index.right_adjust );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.index.spacing );
                    break;
                case   e_columns:
                    cvterr = i_int8( p, curr, &layout_work.index.columns );
                    break;
                case   e_see_string:
                    cvterr = i_xx_string( p, curr, layout_work.index.see_string );
                    break;
                case   e_see_also_string:
                    cvterr = i_xx_string( p, curr, layout_work.index.see_also_string );
                    break;
                case   e_header:
                    cvterr = i_yes_no( p, curr, &layout_work.index.header );
                    break;
                case   e_index_string:
                    cvterr = i_xx_string( p, curr, layout_work.index.index_string );
                    break;
                case   e_page_eject:
                    cvterr = i_page_eject( p, curr, &layout_work.index.page_eject );
                    break;
                case   e_page_reset:
                    cvterr = i_yes_no( p, curr, &layout_work.index.page_reset );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.index.font );
                    if( layout_work.index.font >= wgml_font_cnt ) {
                        layout_work.index.font = 0;
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
    scan_start = scan_stop + 1;
    return;
}

