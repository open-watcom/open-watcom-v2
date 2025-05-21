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


void    lay_xx( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    font_number         *fontptr;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    lay_sub             x_tag;
    att_name_type       attr_name;
    att_val_type        attr_val;

    p = scandata.s;
    cvterr = false;

    if( strcmp( "CIT", entry->tagname ) == 0 ) {
        x_tag = el_cit;
        fontptr = &layout_work.cit.font;
    } else if( strcmp( "DTHD", entry->tagname ) == 0 ) {
        x_tag = el_dthd;
        fontptr = &layout_work.dthd.font;
    } else if( strcmp( "DT", entry->tagname ) == 0 ) {
        x_tag = el_dt;
        fontptr = &layout_work.dt.font;
    } else if( strcmp( "GT", entry->tagname ) == 0 ) {
        x_tag = el_gt;
        fontptr = &layout_work.gt.font;
    } else if( strcmp( "GD", entry->tagname ) == 0 ) {
        x_tag = el_gd;
        fontptr = &layout_work.gd.font;
    } else if( strcmp( "DDHD", entry->tagname ) == 0 ) {
        x_tag = el_ddhd;
        fontptr = &layout_work.ddhd.font;
    } else if( strcmp( "IXPGNUM", entry->tagname ) == 0 ) {
        x_tag = el_ixpgnum;
        fontptr = &layout_work.ixpgnum.font;
    } else if( strcmp( "IXMAJOR", entry->tagname ) == 0 ) {
        x_tag = el_ixmajor;
        fontptr = &layout_work.ixmajor.font;
    } else {
        internal_err_exit( __FILE__, __LINE__ );
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != x_tag ) {
        ProcFlags.lay_xxx = x_tag;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == pos ) {   // get att with value
        cvterr = -1;
        for( k = 0, curr = xx_att[k]; curr > 0; k++, curr = xx_att[k] ) {
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_font_number( p, &attr_val, fontptr );
                    if( *fontptr >= wgml_font_cnt ) {
                        *fontptr = 0;
                    }
                    AttrFlags.font = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                }
                if( cvterr ) {          // there was an error
                    xx_err_exit( err_att_val_inv );
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err_exit( err_att_name_inv );
        }
    }
    scandata.s = scandata.e;
    return;
}

