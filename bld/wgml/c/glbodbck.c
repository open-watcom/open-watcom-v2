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
* Description: WGML implement :BACKM and :BODY tags for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :BACKM and :BODY attributes                                           */
/***************************************************************************/
const   lay_att     backbod_att[10] =
    { e_post_skip, e_pre_top_skip, e_header, e_backm_string, e_body_string,
      e_page_eject, e_page_reset, e_columns, e_font, e_dummy_zero };

/*********************************************************************************/
/*Define the characteristics of the back material section.                       */
/*:BACKM                                                                         */
/*        post_skip = 0                                                          */
/*        pre_top_skip = 0                                                       */
/*        header = no                                                            */
/*        backm_string = ""                                                      */
/*        page_eject = yes                                                       */
/*        page_reset = no                                                        */
/*        columns = 1                                                            */
/*        font = 1                                                               */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the back material. The post-skip will be merged with the next                  */
/*document entitys pre-skip value. If a post-skip occurs at the end of           */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page. If the back material heading is not displayed            */
/*(the header attribute has a value of NO), the post-skip value has no           */
/*effect.                                                                        */
/*                                                                               */
/*pre_top_skip This attribute accepts vertical space units. A zero value means   */
/*that no lines are skipped. If the skip value is a line unit, it is multiplied  */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the back material. The pre-top-skip will be merged with the                    */
/*previous document entitys post-skip value. The specified space is              */
/*still skipped at the beginning of a new page.                                  */
/*                                                                               */
/*header The header attribute accepts the keyword values yes and no. If the      */
/*value yes is specified, the back material heading is generated. If the         */
/*value no is specified, the header text is not generated.                       */
/*backm_string This attribute accepts a character string. If the back material   */
/*header is generated, the specified string is used for the heading text.        */
/*                                                                               */
/*page_eject This attribute accepts the keyword values yes, no, odd, and even.   */
/*if the value no is specified, the heading is one column wide and is not        */
/*forced to a new page. The heading is always placed on a new page               */
/*when the value yes is specified. Values other than no cause the                */
/*heading to be treated as a page wide heading in a multi-column                 */
/*document.                                                                      */
/*The values odd and even will place the heading on a new page if the            */
/*parity (odd or even) of the current page number does not match the             */
/*specified value. When two headings appear together, the attribute              */
/*value stop_eject=yes of the :heading layout tag will normally                  */
/*prevent the the second heading from going to the next page. The                */
/*odd and even values act on the heading without regard to the                   */
/*stop_eject value.                                                              */
/*                                                                               */
/*page_reset This attribute accepts the keyword values yes and no. If the value  */
/*yes is specified, the page number is reset to one at the beginning of          */
/*the section.                                                                   */
/*                                                                               */
/*columns The columns attribute accepts a positive integer number. The           */
/*columns value determines how many columns are created for the                  */
/*back material.                                                                 */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the header attribute value. The font value is linked to the                    */
/*pre_top_skip and post_skip attributes (see "Font Linkage" on page              */
/*77).                                                                           */
/*********************************************************************************/

/***************************************************************************/
/*  lay_backbod   for :BACKM and :BODY                                     */
/***************************************************************************/

void    lay_backbod( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;
    lay_sub             x_tag;
    backbod_lay_tag *   ap;

    p = scan_start;
    cvterr = false;
    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    switch( tag ) {
    case LAY_TAG_BACKM:
        x_tag = el_backm;
        ap = &layout_work.backm;
        break;
    case LAY_TAG_BODY:
        x_tag = el_body;
        ap = &layout_work.body;
        break;
    default:
        out_msg( "WGML logic error glbodbck.c.\n");
        file_mac_info();
        err_count++;
        break;
    }
    if( ProcFlags.lay_xxx != x_tag ) {
        ProcFlags.lay_xxx = x_tag;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = backbod_att[k]; curr > 0; k++, curr = backbod_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_post_skip:
                    cvterr = i_space_unit( p, curr, &(ap->post_skip) );
                    break;
                case   e_pre_top_skip:
                    cvterr = i_space_unit( p, curr, &(ap->pre_top_skip) );
                    break;
                case   e_header:
                    cvterr = i_yes_no( p, curr, &(ap->header) );
                    break;
                case   e_body_string:
                    if( x_tag == el_body ) {
                        cvterr = i_xx_string( p, curr, ap->string );
                    }
                    break;
                case   e_backm_string:
                    if( x_tag == el_backm ) {
                        cvterr = i_xx_string( p, curr, ap->string );
                    }
                    break;
                case   e_page_eject:
                    cvterr = i_page_eject( p, curr, &(ap->page_eject) );
                    break;
                case   e_page_reset:
                    cvterr = i_yes_no( p, curr, &(ap->page_reset) );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &(ap->font) );
                    if( ap->font >= wgml_font_cnt ) {
                        ap->font = 0;
                    }
                    break;
                case   e_columns:
                    if( x_tag == el_backm ) {
                        cvterr = i_int8( p, curr, &(ap->columns) );
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

