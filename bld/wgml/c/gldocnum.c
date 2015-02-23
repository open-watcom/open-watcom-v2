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
* Description: WGML implement :DOCNUM  tag for LAYOUT processing
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

#include "clibext.h"

/***************************************************************************/
/*   :DOCNUM   attributes                                                  */
/***************************************************************************/
const   lay_att     docnum_att[7] =
    { e_left_adjust, e_right_adjust, e_page_position, e_font, e_pre_skip,
      e_docnum_string, e_dummy_zero };


/***************************************************************************/
/*  lay_docnum                                                             */
/***************************************************************************/

void    lay_docnum( lay_tag tag )
{
    char                *p;
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
    if( ProcFlags.lay_xxx != el_docnum ) {
        ProcFlags.lay_xxx = el_docnum;
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = -1;
        for( k = 0, curr = docnum_att[k]; curr > 0; k++, curr = docnum_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_left_adjust:
                    cvterr = i_space_unit( p, curr, &layout_work.docnum.left_adjust );
                    break;
                case   e_right_adjust:
                    cvterr = i_space_unit( p, curr, &layout_work.docnum.right_adjust );
                    break;
                case   e_page_position:
                    cvterr = i_page_position( p, curr, &layout_work.docnum.page_position );
                    break;
                case   e_font:
                    cvterr = i_font_number( p, curr, &layout_work.docnum.font );
                    if( layout_work.docnum.font >= wgml_font_cnt ) {
                        layout_work.docnum.font = 0;
                    }
                    break;
                case   e_pre_skip:
                    cvterr = i_space_unit( p, curr, &layout_work.docnum.pre_skip );
                    break;
                case   e_docnum_string:
                    cvterr = i_xx_string( p, curr, layout_work.docnum.string );
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

