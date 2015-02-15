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
* Description: WGML implement :DEFAULT LAYOUT tag
*
****************************************************************************/
 
#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */
 
#include "wgml.h"
#include "gvars.h"
 
 
 
/***************************************************************************/
/*   :DEFAULT attributes                                                   */
/***************************************************************************/
const   lay_att     default_att[8] =
    { e_spacing, e_columns, e_font, e_justify, e_input_esc, e_gutter, e_binding, e_dummy_zero };
 
/***************************************************************************/
/*Define default characteristics for document processing.                  */
/*                                                                         */
/*:DEFAULT                                                                 */
/*        spacing = 1                                                      */
/*        columns = 1                                                      */
/*        font = 0                                                         */
/*        justify = yes                                                    */
/*        input_esc = ' '                                                  */
/*        gutter = '0.5i'                                                  */
/*        binding = 0                                                      */
/*                                                                         */
/*spacing This attribute accepts a positive integer number. The spacing    */
/*determines the number of blank lines that are output between text        */
/*lines. If the line spacing is two, each text line will take two lines in */
/*the output. The number of blank lines between text lines will            */
/*therefore be the spacing value minus one. The spacing attribute          */
/*defines the line spacing within the document when there is no layout     */
/*entry for spacing with a specific document element.                      */
/*                                                                         */
/*columns The columns attribute accepts a positive integer number. The     */
/*columns value determines how many columns are created on each            */
/*output page.                                                             */
/*                                                                         */
/*font This attribute accepts a non-negative integer number. If a font     */
/*number is used for which no font has been defined, WATCOM                */
/*Script/GML will use font zero. The font numbers from zero to three       */
/*correspond directly to the highlighting levels specified by the          */
/*highlighting phrase GML tags. The font attribute defines the font of     */
/*the document when the font is not explicitly determined by the           */
/*document element.                                                        */
/*                                                                         */
/*justify The justify attribute accepts the keyword values yes and no.     */
/*Right justification of text is performed if this attribute has a value   */
/*of yes.                                                                  */
/*If justification is not desired, the value should be no.                 */
/*                                                                         */
/*input_esc The input escape attribute accepts the keyword value none or a */
/*quoted character. Input escapes are not recognized if the attribute      */
/*value is none or a blank. If a character is specified as the attribute   */
/*value, this character is used as the input escape delimiter. If an       */
/*empty('') or none value is specified, the blank value is used. Refer     */
/*to "Input Translation" on page 80 for more information.                  */
/*                                                                         */
/*gutter The gutter attribute specifies the amount of space between columns*/
/*in a multi-column document, and has no effect in a single column         */
/*document. This attribute accepts any valid horizontal space unit.        */
/*                                                                         */
/*binding The binding attribute accepts any valid horizontal space unit.   */
/*The binding value is added to the current left and right margins of those*/
/*output pages which are odd numbered.                                     */
/***************************************************************************/
 
 
/***************************************************************************/
/*  lay_default                                                            */
/***************************************************************************/
 
void    lay_default( lay_tag tag )
{
    char        *   p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    int             cvterr;
 
    tag = tag;
    p = scan_start;
    cvterr = false;
 
    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_default ) {
        ProcFlags.lay_xxx = el_default;
    }
    cc = get_lay_sub_and_value( &l_args );  // get attr with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = default_att[k]; curr > 0; k++, curr = default_att[k] ) {
 
            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];
 
                switch( curr ) {
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.defaults.spacing );
                    break;
                case   e_columns:
                    cvterr = i_int8( p, curr, &layout_work.defaults.columns );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.defaults.font );
                    if( layout_work.defaults.font >= wgml_font_cnt ) {
                        layout_work.defaults.font = 0;
                    }
                    break;
                case   e_justify:
                    cvterr = i_yes_no( p, curr, &layout_work.defaults.justify );
                    break;
                case   e_input_esc:
                    cvterr = i_char( p, curr, &layout_work.defaults.input_esc );
                    in_esc = layout_work.defaults.input_esc;
                    if( in_esc != ' ' ) {
                        ProcFlags.in_trans = true;
                    }
                    break;
                case   e_gutter:
                    cvterr = i_space_unit( p, curr, &layout_work.defaults.gutter );
                    break;
                case   e_binding:
                    cvterr = i_space_unit( p, curr, &layout_work.defaults.binding );
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
        cc = get_lay_sub_and_value( &l_args );  // get attr with value
    }
    scan_start = scan_stop;
    return;
}
