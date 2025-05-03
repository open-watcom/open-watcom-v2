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
* Description: WGML implement :APPENDIX tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :APPENDIX attributes                                                  */
/***************************************************************************/
const   lay_att     appendix_att[22] =
    { e_indent, e_pre_top_skip, e_pre_skip, e_post_skip, e_spacing,
      e_font, e_number_font, e_number_form, e_page_position, e_number_style,
      e_page_eject, e_line_break, e_display_heading, e_number_reset, e_case,
      e_align, e_header, e_appendix_string, e_page_reset, e_section_eject,
      e_columns, e_dummy_zero };

/*********************************************************************************/
/*Define the characteristics of the appendix section and appendix heading. All   */
/*of the attributes, with the exception of the spacing value, apply to the :h1   */
/*tag while in the appendix section.                                             */
/*                                                                               */
/*:APPENDIX                                                                      */
/*        indent = 0                                                             */
/*        pre_top_skip = 0                                                       */
/*        pre_skip = 0                                                           */
/*        post_skip = 3                                                          */
/*        spacing = 1                                                            */
/*        font = 3                                                               */
/*        number_font = 3                                                        */
/*        number_form = new                                                      */
/*        page_position = left                                                   */
/*        number_style = b                                                       */
/*        page_eject = yes                                                       */
/*        line_break = yes                                                       */
/*        display_heading = yes                                                  */
/*        number_reset = yes                                                     */
/*        case = mixed :                                                         */
/*        align = 0                                                              */
/*        header = yes                                                           */
/*        appendix_string = "APPENDIX "                                          */
/*        page_reset = no                                                        */
/*        section_eject = yes                                                    */
/*        columns = 1                                                            */
/*                                                                               */
/*indent The indent attribute accepts any valid horizontal space unit. The       */
/*indent value is added to the offset determined by the page position            */
/*attribute, giving the starting offset from the left margin for the             */
/*appendix heading.                                                              */
/*                                                                               */
/*pre_top_skip This attribute accepts vertical space units. A zero value means   */
/*that no lines are skipped. If the skip value is a line unit, it is multiplied  */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the appendix heading. The pre-top-skip will be merged with the                 */
/*previous document entity's post-skip value. The specified space is             */
/*still skipped at the beginning of a new page.                                  */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the appendix heading. The post-skip will be merged with the next               */
/*document entity's pre-skip value. If a post-skip occurs at the end of          */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page. If the appendix heading is not displayed, the            */
/*post-skip is ignored.                                                          */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the appendix section.                          */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the appendix heading. The font value is linked to the indent,                  */
/*post_skip and pre_top_skip attributes (see "Font Linkage" on page              */
/*77).                                                                           */
/*                                                                               */
/*number_font This attribute accepts a non-negative integer number. If a font    */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The number font attribute defines                */
/*the font of the appendix number.                                               */
/*                                                                               */
/*number_form This attribute accepts the values none, prop, and new. The         */
/*specified value determines the format of the appendix heading number. The      */
/*value none indicates that no number is to be output. The value prop            */
/*indicates that the number is composed of the number for the current            */
/*level prefixed by the number for the previous level and the number             */
/*delimiter specified in the heading layout tag. The value new                   */
/*indicates that only the number of the current level is to be output.           */
/*                                                                               */
/*page_position This attribute accepts the values left, right, center, and       */
/*centre. The position of the appendix heading between the left and right        */
/*margins is determined by the value selected. If left is the attribute value,   */
/*the text is output at the left margin. If right is the attribute value, the    */
/*text is output next to the right margin. When center or centre is              */
/*specified, the text is centered between the left and right margins.            */
/*number_style This attribute sets the number style of the appendix heading      */
/*number. (See "Number Style" on page 121).                                      */
/*                                                                               */
/*page_eject This attribute accepts the keyword values yes, no, odd, and even.   */
/*If the value no is specified, the heading is one column wide and is not        */
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
/*line_break This attribute accepts the keyword values yes and no. If the value  */
/*yes is specified, the skip value specified by the post_skip attribute          */
/*will be issued. If the value no is specified, the skip value specified         */
/*by the post_skip attribute will be ignored. If a paragraph follows             */
/*the heading, the paragraph text will start on the same line as the             */
/*heading.                                                                       */
/*                                                                               */
/*display_heading This attribute accepts the keyword values yes and no. The      */
/*heading is not produced when the value no is specified. The                    */
/*heading pre and post skips are still generated.                                */
/*                                                                               */
/*number_reset This attribute accepts the keyword values yes and no. When a      */
/*heading is processed, all heading levels after it have their heading           */
/*numbers reset. When the value 'no' is specified, the number of the             */
/*next level of heading is not reset.                                            */
/*                                                                               */
/*case This attribute accepts the keyword values mixed, upper and lower. :       */
/*When a heading is processed, the text is converted to upper or lower           */
/*case when the values UPPER or LOWER are used. The text is left :               */
/*unchanged when the value MIXED is used.                                        */
/*                                                                               */
/*align This attribute accepts any valid horizontal space unit. The align        */
/*value specifies the amount of space reserved for the appendix                  */
/*heading. After the appendix heading is produced, the align value is            */
/*added to the current left margin. The left margin will be reset to its         */
/*previous value after the appendix heading.                                     */
/*                                                                               */
/*header The header attribute accepts the keyword values yes and no. If the      */
/*value yes is specified, the appendix header (specified by the                  */
/*appendix_string attribute) is generated at the beginning of the                */
/*heading text specified by a :h1 tag. If the value no is specified, the         */
/*header text is not generated.                                                  */
/*                                                                               */
/*appendix_string This attribute accepts a character string. If the appendix     */
/*header is generated, the specified string is inserted before the :h1           */
/*heading text.                                                                  */
/*                                                                               */
/*page_reset This attribute accepts the keyword values yes and no. If the value  */
/*yes is specified, the page number is reset to one at the beginning of          */
/*the section.                                                                   */
/*                                                                               */
/*section_eject This attribute accepts the keyword values yes, no, odd, and even.*/
/*If the value no is specified, the section is not forced to a new page.         */
/*The section is always placed on a new page when the value yes is               */
/*specified.                                                                     */
/*The values odd and even will place the section on a new page if the            */
/*parity (odd or even) of the current page number does not match the             */
/*specified value.                                                               */
/*                                                                               */
/*columns The columns attribute accepts a positive integer number. The           */
/*columns value determines how many columns are created for the                  */
/*appendix.                                                                      */
/*********************************************************************************/


/***************************************************************************/
/*  lay_appendix                                                           */
/***************************************************************************/

void    lay_appendix( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 k;
    int                 cvterr;
    lay_att             curr;

    (void)entry;

    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_appendix ) {
        ProcFlags.lay_xxx = el_appendix;
    }
    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = appendix_att[k]; curr > 0; k++, curr = appendix_att[k] ) {

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                switch( curr ) {
                case   e_indent:
                    if( AttrFlags.indent ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                    &layout_work.hx.hx_head[hds_appendix].indent );
                    AttrFlags.indent = true;
                    break;
                case   e_pre_top_skip:
                    if( AttrFlags.pre_top_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                &layout_work.hx.hx_sect[hds_appendix].pre_top_skip );
                    AttrFlags.pre_top_skip = true;
                    break;
                case   e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                    &layout_work.hx.hx_head[hds_appendix].pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case   e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                    &layout_work.hx.hx_sect[hds_appendix].post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case   e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_spacing( p, curr, &layout_work.hx.hx_sect[hds_appendix].spacing );
                    AttrFlags.spacing = true;
                    break;
                case   e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_font_number( p, curr,
                                        &layout_work.hx.hx_sect[hds_appendix].text_font );
                    if( layout_work.hx.hx_sect[hds_appendix].text_font >= wgml_font_cnt ) {
                        layout_work.hx.hx_sect[hds_appendix].text_font = 0;
                    }
                    AttrFlags.font = true;
                    break;
                case   e_number_font:
                    if( AttrFlags.number_font ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_font_number( p, curr,
                                &layout_work.hx.hx_head[hds_appendix].number_font );
                    if( layout_work.hx.hx_head[hds_appendix].number_font >= wgml_font_cnt ) {
                        layout_work.hx.hx_head[hds_appendix].number_font = 0;
                    }
                    AttrFlags.number_font = true;
                    break;
                case   e_number_form:
                    if( AttrFlags.number_form ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_number_form( p, curr,
                                &layout_work.hx.hx_head[hds_appendix].number_form );
                    AttrFlags.number_form = true;
                    break;
                case   e_page_position:
                    if( AttrFlags.page_position ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_page_position( p, curr,
                                &layout_work.hx.hx_head[hds_appendix].line_position );
                    AttrFlags.page_position = true;
                    break;
                case   e_number_style:
                    if( AttrFlags.number_style ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_number_style( p, curr,
                                &layout_work.hx.hx_head[hds_appendix].number_style );
                    AttrFlags.number_style = true;
                    break;
                case   e_page_eject:
                    if( AttrFlags.page_eject ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_page_eject( p, curr,
                                &layout_work.hx.hx_head[hds_appendix].page_eject );
                    AttrFlags.page_eject = true;
                    break;
                case   e_line_break:
                    if( AttrFlags.line_break ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr,
                                &layout_work.hx.hx_head[hds_appendix].line_break );
                    AttrFlags.line_break = true;
                    break;
                case   e_display_heading:
                    if( AttrFlags.display_heading ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr,
                            &layout_work.hx.hx_head[hds_appendix].display_heading );
                    AttrFlags.display_heading = true;
                    break;
                case   e_number_reset:
                    if( AttrFlags.number_reset ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr,
                                &layout_work.hx.hx_head[hds_appendix].number_reset );
                    AttrFlags.number_reset = true;
                    break;
                case   e_case:
                    if( AttrFlags.case_a ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_case( p, curr,
                                     &layout_work.hx.hx_head[hds_appendix].hd_case );
                    AttrFlags.case_a = true;
                    break;
                case   e_align:
                    if( AttrFlags.align ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_space_unit( p, curr,
                                        &layout_work.hx.hx_head[hds_appendix].align );
                    AttrFlags.align = true;
                    break;
                case   e_header:
                    if( AttrFlags.header ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr,
                                       &layout_work.hx.hx_sect[hds_appendix].header );
                    AttrFlags.header = true;
                    break;
                case   e_appendix_string:
                    if( AttrFlags.appendix_string ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_xx_string( p, curr, layout_work.appendix.string );
                    AttrFlags.appendix_string = true;
                    break;
                case   e_page_reset:
                    if( AttrFlags.page_reset ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_yes_no( p, curr, &layout_work.appendix.page_reset );
                    AttrFlags.page_reset = true;
                    break;
                case   e_section_eject:
                    if( AttrFlags.section_eject ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_page_eject( p, curr,
                                                &layout_work.appendix.section_eject );
                    AttrFlags.section_eject = true;
                    break;
                case   e_columns:
                    if( AttrFlags.columns ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_name,
                            g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                    }
                    cvterr = i_int8( p, curr, &layout_work.appendix.columns );
                    AttrFlags.columns = true;
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

