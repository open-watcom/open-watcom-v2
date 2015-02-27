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
* Description: WGML implement several sub tags for :LAYOUT processing
*                   all those with only font attribute
****************************************************************************/

#include "wgml.h"

#include "clibext.h"

/***************************************************************************/
/*                     several  attributes  with only font as value        */
/*                                                                         */
/* :DT :GT :DTHD :CIT :GD :DDHD :IXPGNUM :IXMAJOR                          */
/*                                                                         */
/***************************************************************************/
const   lay_att     xx_att[2] =
    { e_font, e_dummy_zero };



void    lay_xx( lay_tag tag )
{
    char            *p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    int             cvterr;
    lay_sub         x_tag;
    font_number     *fontptr;

    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    switch( tag ) {
    case LAY_TAG_CIT:
        x_tag = el_cit;
        fontptr = &layout_work.cit.font;
        break;
    case LAY_TAG_DTHD:
        x_tag = el_dthd;
        fontptr = &layout_work.dthd.font;
        break;
    case LAY_TAG_DT:
        x_tag = el_dt;
        fontptr = &layout_work.dt.font;
        break;
    case LAY_TAG_GT:
        x_tag = el_gt;
        fontptr = &layout_work.gt.font;
        break;
    case LAY_TAG_GD:
        x_tag = el_gd;
        fontptr = &layout_work.gd.font;
        break;
    case LAY_TAG_DDHD:
        x_tag = el_ddhd;
        fontptr = &layout_work.ddhd.font;
        break;
    case LAY_TAG_IXPGNUM:
        x_tag = el_ixpgnum;
        fontptr = &layout_work.ixpgnum.font;
        break;
    case LAY_TAG_IXMAJOR:
        x_tag = el_ixmajor;
        fontptr = &layout_work.ixmajor.font;
        break;
    default:
         out_msg( "WGML logic error glxxfont.c.\n");
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
        for( k = 0, curr = xx_att[k]; curr > 0; k++, curr = xx_att[k] ) {

            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];

                switch( curr ) {
                case   e_font:
                    cvterr = i_font_number( p, curr, fontptr );
                    if( *fontptr >= wgml_font_cnt ) {
                        *fontptr = 0;
                    }
                    break;
                default:
                    out_msg( "WGML logic error.\n" );
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

