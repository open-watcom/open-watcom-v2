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
* Description: WGML implement :FIGLIST tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :FIGLIST attributes                                                   */
/***************************************************************************/
const   lay_att     figlist_att[7] =
    { e_left_adjust, e_right_adjust, e_skip, e_spacing, e_columns,
      e_fill_string, e_dummy_zero };

/***********************************************************************************/
/*Define the characteristics of the figure list.                                   */
/*:FIGLIST                                                                         */
/*        left_adjust = 0                                                          */
/*        right_adjust = 0                                                         */
/*        skip = 0                                                                 */
/*        spacing = 1                                                              */
/*        columns = 1                                                              */
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
/*skip This attribute accepts vertical space units. A zero value means that        */
/*no lines are skipped. If the skip value is a line unit, it is multiplied         */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for            */
/*more information). The resulting amount of space is skipped                      */
/*between figure list items.                                                       */
/*                                                                                 */
/*spacing This attribute accepts a positive integer number. The spacing            */
/*determines the number of blank lines that are output between text                */
/*lines. If the line spacing is two, each text line will take two lines in         */
/*the output. The number of blank lines between text lines will                    */
/*therefore be the spacing value minus one. The spacing attribute                  */
/*defines the line spacing within the figure list.                                 */
/*                                                                                 */
/*columns The columns attribute accepts a positive integer number. The             */
/*columns value determines how many columns are created for the                    */
/*figure list.                                                                     */
/*                                                                                 */
/*fill_string This attribute accepts a string value which is used to 'fill'        */
/*the line between the text and the page number.                                   */
/***********************************************************************************/


/***************************************************************************/
/*  lay_figlist                                                            */
/***************************************************************************/

void    lay_figlist( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;

    p = scandata.s;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0, curr = figlist_att[k]; curr > 0; k++, curr = figlist_att[k] ) {
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.figlist.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.figlist.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.figlist.skip );
                    AttrFlags.skip = true;
                    break;
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_spacing( p, &attr_val, &layout_work.figlist.spacing );
                    AttrFlags.spacing = true;
                    break;
                case e_columns:
                    if( AttrFlags.columns ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_int8( p, &attr_val, &layout_work.figlist.columns );
                    AttrFlags.columns = true;
                    break;
                case e_fill_string:
                    if( AttrFlags.fill_string ) {
                        xx_line_err_exit_ci( err_att_dup, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    }
                    cvterr = i_xx_string( p, &attr_val, layout_work.figlist.fill_string );
                    AttrFlags.fill_string = true;
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

