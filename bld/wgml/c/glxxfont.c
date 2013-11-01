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

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*                     several  attributes  with only font as value        */
/*                                                                         */
/* :DT :GT :DTHD :CIT :GD :DDHD :IXPGNUM :IXMAJOR                          */
/*                                                                         */
/***************************************************************************/
const   lay_att     xx_att[2] =
    { e_font, e_dummy_zero };



void    lay_xx( const gmltag * entry )
{
    char            *p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    int             cvterr;
    lay_sub         x_tag;
    font_number     *fntptr;

    p = scan_start;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( !strcmp( "CIT", entry->tagname ) ) {
        x_tag = el_cit;
        fntptr = &layout_work.cit.font;
    } else if( !strcmp( "DTHD", entry->tagname ) ) {
        x_tag = el_dthd;
        fntptr = &layout_work.dthd.font;
    } else if( !strcmp( "DT", entry->tagname ) ) {
        x_tag = el_dt;
        fntptr = &layout_work.dt.font;
    } else if( !strcmp( "GT", entry->tagname ) ) {
        x_tag = el_gt;
        fntptr = &layout_work.gt.font;
    } else if( !strcmp( "GD", entry->tagname ) ) {
        x_tag = el_gd;
        fntptr = &layout_work.gd.font;
    } else if( !strcmp( "DDHD", entry->tagname ) ) {
        x_tag = el_ddhd;
        fntptr = &layout_work.ddhd.font;
    } else if( !strcmp( "IXPGNUM", entry->tagname ) ) {
        x_tag = el_ixpgnum;
        fntptr = &layout_work.ixpgnum.font;
    } else if( !strcmp( "IXMAJOR", entry->tagname ) ) {
        x_tag = el_ixmajor;
        fntptr = &layout_work.ixmajor.font;
    } else {
         out_msg( "WGML logic error glxxfont.c.\n");
         file_mac_info();
         err_count++;
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
                    cvterr = i_font_number( p, curr, fntptr );
                    if( *fntptr >= wgml_font_cnt ) {
                        *fntptr = 0;
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
    scan_start = scan_stop + 1;
    return;
}

