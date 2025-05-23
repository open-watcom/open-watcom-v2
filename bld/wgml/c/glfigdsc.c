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
* Description: WGML implement :FIGDESC tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :FIGDESC attributes                                                   */
/***************************************************************************/
const   lay_att     figdesc_att[3] =
    { e_pre_lines, e_font, e_dummy_zero };

/*********************************************************************************/
/*Define the characteristics of the figure description entity.                   */
/*:FIGDESC                                                                       */
/*        pre_lines = 1                                                          */
/*        font = 0                                                               */
/*                                                                               */
/*pre_lines This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting number of lines are skipped                   */
/*before the figure description. If the document entity starts a new             */
/*page, the specified number of lines are still skipped. The pre-lines           */
/*value is not merged with the previous document entity's post-skip              */
/*value. If the previous tag was :figcap, this value is ignored.                 */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the figure description. The font value is linked to the pre_lines              */
/*attribute (see "Font Linkage" on page 77).                                     */
/*********************************************************************************/


/***************************************************************************/
/*  lay_figdesc                                                            */
/***************************************************************************/

void    lay_figdesc( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;

    (void)entry;

    p = scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != TL_FIGDESC ) {
        ProcFlags.lay_xxx = TL_FIGDESC;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0, curr = figdesc_att[k]; curr > 0; k++, curr = figdesc_att[k] ) {
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_pre_lines:
                    if( AttrFlags.pre_lines ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.figdesc.pre_lines );
                    AttrFlags.pre_lines = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.figdesc.font );
                    if( layout_work.figdesc.font >= wgml_font_cnt ) {
                        layout_work.figdesc.font = 0;
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

