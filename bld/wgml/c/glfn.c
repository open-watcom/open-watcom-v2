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
* Description: WGML implement :FN and :FNREF tags for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :FN    attributes                                                     */
/***************************************************************************/
const   lay_att     fn_att[11] =
    { e_line_indent, e_align, e_pre_lines, e_skip, e_spacing, e_font,
      e_number_font, e_number_style, e_frame, e_dummy_zero };


/***************************************************************************/
/*   :FNREF attributes                                                     */
/***************************************************************************/
const   lay_att     fnref_att[3] = { e_font, e_number_style, e_dummy_zero };


/*********************************************************************************/
/*Define the characteristics of the footnote entity.                             */
/*                                                                               */
/*:FN                                                                            */
/*        line_indent = 0                                                        */
/*        align = '0.4i'                                                         */
/*        pre_lines = 2                                                          */
/*        skip = 2                                                               */
/*        spacing = 1                                                            */
/*        font = 0                                                               */
/*        number_font = 0                                                        */
/*        number_style = h                                                       */
/*        frame = none                                                           */
/*                                                                               */
/*line_indent The line_indent attribute accepts any valid horizontal space unit. */
/*This attribute specifies the amount of indentation for the first output        */
/*line of the footnote.                                                          */
/*                                                                               */
/*align This attribute accepts any valid horizontal space unit. The align        */
/*value specifies the amount of space reserved for the footnote                  */
/*number. After the footnote number is produced, the align value is              */
/*added to the current left margin. The left margin will be reset to its         */
/*previous value after the footnote.                                             */
/*                                                                               */
/*pre_lines This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting number of lines are skipped                   */
/*before the footnotes are output. If the document entity starts a new           */
/*page, the specified number of lines are still skipped. The pre-lines           */
/*value is not merged with the previous document entity's post-skip              */
/*value.                                                                         */
/*                                                                               */
/*skip This attribute accepts vertical space units. A zero value means that      */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped                    */
/*between the footnotes.                                                         */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the footnote.                                  */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the footnote text. The font value is linked to the line_indent,                */
/*pre_lines, skip and align attributes (see "Font Linkage" on page 77).          */
/*                                                                               */
/*number_font This attribute accepts a non-negative integer number. If a font    */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The number font attribute defines                */
/*the font of the footnote number.                                               */
/*                                                                               */
/*number_style This attribute sets the number style of the footnote number.      */
/*(See "Number Style" on page 121).                                              */
/*                                                                               */
/*frame This attribute accepts the values rule or none. If the value rule is     */
/*specified, a rule line is placed between the main body of text and the         */
/*footnotes at the bottom of the output page. If the footnote is placed          */
/*across the entire page, the width of the rule line is half the width of        */
/*the page. If the footnote is one column wide, the rule line width is           */
/*the width of a column minus twenty percent.                                    */
/*********************************************************************************/


/******************************************************************************/
/*Define the characteristics of the footnote reference entity.                */
/*                                                                            */
/*:FNREF                                                                      */
/*        font = 0                                                            */
/*        number_style = hp                                                   */
/*                                                                            */
/*font This attribute accepts a non-negative integer number. If a font        */
/*number is used for which no font has been defined, WATCOM                   */
/*Script/GML will use font zero. The font numbers from zero to three          */
/*correspond directly to the highlighting levels specified by the             */
/*highlighting phrase GML tags. The font attribute defines the font of        */
/*the footnote reference text.                                                */
/*                                                                            */
/*number_style This attribute sets the number style of the footnote reference */
/*number. (See "Number Style" on page 121).                                   */
/******************************************************************************/


/***************************************************************************/
/*  lay_fn                                                                 */
/***************************************************************************/

void    lay_fn( lay_tag tag )
{
    char        *   p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    bool            cvterr;

    tag = tag;
    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_fn ) {
        ProcFlags.lay_xxx = el_fn;
    }
    cc = get_lay_sub_and_value( &l_args );  // get one with value
    while( cc == pos ) {
        cvterr = true;
        for( k = 0, curr = fn_att[k]; curr > 0; k++, curr = fn_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_line_indent:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.fn.line_indent );
                    break;
                case   e_align:
                    cvterr = i_space_unit( p, curr, &layout_work.fn.align );
                    break;
                case   e_pre_lines:
                    cvterr = i_space_unit( p, curr, &layout_work.fn.pre_lines );
                    break;
                case   e_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.fn.skip );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.fn.spacing );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.fn.font );
                    if( layout_work.fn.font >= wgml_font_cnt ) {
                        layout_work.fn.font = 0;
                    }
                    break;
                case   e_number_font:
                    cvterr = i_font_number( p, curr, &layout_work.fn.number_font );
                    if( layout_work.fn.number_font >= wgml_font_cnt ) {
                        layout_work.fn.number_font = 0;
                    }
                    break;
                case   e_number_style:
                    cvterr = i_number_style( p, curr,
                                             &layout_work.fn.number_style );
                    break;
                case   e_frame:
                    cvterr = i_frame( p, curr, &layout_work.fn.frame );
                    break;
                default:
                    out_msg( "WGML logic error.\n");
                    cvterr = true;
                    break;
                }
                if( cvterr ) {                  // there was an error
                    err_count++;
                    g_err( err_att_val_inv );
                    file_mac_info();
                }
                break;                  // break out of for loop
            }
        }
        cc = get_lay_sub_and_value( &l_args );  // get one with value
    }
    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  lay_fnref                                                              */
/***************************************************************************/
void    lay_fnref( lay_tag tag )
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
    if( ProcFlags.lay_xxx != el_fnref ) {
        ProcFlags.lay_xxx = el_fnref;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = fnref_att[k]; curr > 0; k++, curr = fnref_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.fnref.font );
                    if( layout_work.fnref.font >= wgml_font_cnt ) {
                        layout_work.fnref.font = 0;
                    }
                    break;
                case   e_number_style:
                    cvterr = i_number_style( p, curr,
                                             &layout_work.fnref.number_style );
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

