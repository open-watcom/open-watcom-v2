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
* Description: WGML implement :TOC tag for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*   :TOC    attributes                                                      */
/***************************************************************************/
const   lay_att     toc_att[7] =
    { e_left_adjust, e_right_adjust, e_spacing, e_columns, e_toc_levels,
      e_fill_string, e_dummy_zero };


/***********************************************************************************/
/*Define the characteristics of the table of contents.                             */
/*                                                                                 */
/*:TOC                                                                             */
/*        left_adjust = 0                                                          */
/*        right_adjust = 0                                                         */
/*        spacing = 1                                                              */
/*        columns = 1                                                              */
/*        toc_levels = 4                                                           */
/*        fill_string = "."                                                        */
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
/*defines the line spacing within the table of contents.                           */
/*                                                                                 */
/*columns The columns attribute accepts a positive integer number. The             */
/*columns value determines how many columns are created for the                    */
/*table of contents.                                                               */
/*                                                                                 */
/*toc_levels This attribute accepts as its value a positive integer value. The     */
/*attribute value specifies the maximum level of the entries that                  */
/*appear in the table of contents. For example, if the attribute value is          */
/*four, heading levels zero through three will appear in the table of              */
/*contents.                                                                        */
/*                                                                                 */
/*fill_string This attribute accepts a string value which is used to 'fill' the    */
/*line between the text and the page number.                                       */
/***********************************************************************************/



/***************************************************************************/
/*  lay_toc                                                                */
/***************************************************************************/

void    lay_toc( lay_tag tag )
{
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_args            l_args;
    int                 cvterr;

    tag = tag;
    p = scan_start;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_toc ) {
        ProcFlags.lay_xxx = el_toc;
    }


    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = toc_att[k]; curr > 0; k++, curr = toc_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_left_adjust:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.toc.left_adjust );
                    break;
                case   e_right_adjust:
                    cvterr = i_space_unit( p, curr,
                                           &layout_work.toc.right_adjust );
                    break;
                case   e_spacing:
                    cvterr = i_int8( p, curr, &layout_work.toc.spacing );
                    break;
                case   e_columns:
                    cvterr = i_int8( p, curr, &layout_work.toc.columns );
                    break;
                case   e_toc_levels:
                    cvterr = i_int8( p, curr, &layout_work.toc.toc_levels );
                    break;
                case   e_fill_string:
                    cvterr = i_xx_string( p, curr, layout_work.toc.fill_string );
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

