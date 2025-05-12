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
* Description: WGML implement :TOCPGNUM tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :TOCPGNUM attributes                                                  */
/***************************************************************************/
const   lay_att     tocpgnum_att[3] =
    { e_size, e_font, e_dummy_zero };


/***************************************************************************/
/*  lay_tocpgnum                                                           */
/***************************************************************************/

void    lay_tocpgnum( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    lay_att_val         lay_attr;

    (void)entry;

    p = scandata.s;
    cvterr = false;

    if( !GlobalFlags.firstpass ) {
        scandata.s = scandata.e;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_tocpgnum ) {
        ProcFlags.lay_xxx = el_tocpgnum;
    }
    while( (cc = lay_attr_and_value( &lay_attr )) == pos ) {   // get att with value
        cvterr = -1;
        for( k = 0, curr = tocpgnum_att[k]; curr > 0; k++, curr = tocpgnum_att[k] ) {
            if( strcmp( lay_att_names[curr], lay_attr.attname ) == 0 ) {
                p = lay_attr.val.name;
                switch( curr ) {
                case e_size:
                    if( AttrFlags.size ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_space_unit( p, &lay_attr,
                                           &layout_work.tocpgnum.size );
                    AttrFlags.size = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_ci( err_att_dup, lay_attr.att_name,
                            lay_attr.val.name - lay_attr.att_name + lay_attr.val.len);
                    }
                    cvterr = i_font_number( p, &lay_attr, &layout_work.tocpgnum.font );
                    if( layout_work.tocpgnum.font >= wgml_font_cnt ) {
                        layout_work.tocpgnum.font = 0;
                    }
                    AttrFlags.font = true;
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
        if( ProcFlags.tag_end_found ) {
            break;
        }
    }
    scandata.s = scandata.e;
    return;
}

