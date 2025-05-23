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
* Description: WGML implement :FLPGNUM tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :FLPGNUM  attributes                                                  */
/***************************************************************************/
const   lay_att     flpgnum_att[3] =
    { e_size, e_font, e_dummy_zero };


/***************************************************************************/
/*  lay_flpgnum                                                            */
/***************************************************************************/

void    lay_flpgnum( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;

    p = scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0, curr = flpgnum_att[k]; curr > 0; k++, curr = flpgnum_att[k] ) {
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_size:
                    if( AttrFlags.size ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.flpgnum.size );
                    AttrFlags.size = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.flpgnum.font );
                    if( layout_work.flpgnum.font >= wgml_font_cnt ) {
                        layout_work.flpgnum.font = 0;
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

