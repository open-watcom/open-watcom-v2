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
* Description: WGML implement :LP tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :LP    attributes                                                     */
/***************************************************************************/
static const lay_att    lp_att[] = {
    e_left_indent, e_right_indent, e_line_indent, e_pre_skip, e_post_skip, e_spacing
};

/*********************************************************************************/
/*Define the characteristics of the list part entity.                            */
/*:LP                                                                            */
/*        left_indent = 0                                                        */
/*        right_indent = 0                                                       */
/*        line_indent = 0                                                        */
/*        pre_skip = 1                                                           */
/*        post_skip = 1                                                          */
/*        spacing = 1                                                            */
/*                                                                               */
/*left_indent This attribute accepts any valid horizontal space unit. The left   */
/*indent value is added to the current left margin. The left margin will         */
/*be reset to its previous value at the end of the list part.                    */
/*right_indent This attribute accepts any valid horizontal space unit. The right */
/*indent value is subtracted from the current right margin. The right            */
/*margin will be reset to its previous value at the end of the list part.        */
/*                                                                               */
/*line_indent The line_indent attribute accepts any valid horizontal space unit. */
/*This attribute specifies the amount of indentation for the first output        */
/*line of the list part.                                                         */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the list part. The pre-skip will be merged with the previous                   */
/*document entity's post-skip value. If a pre-skip occurs at the                */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the list part. The post-skip will be merged with the next document             */
/*entity's pre-skip value. If a post-skip occurs at the end of an output        */
/*page, any remaining part of the skip is not carried over to the next           */
/*output page.                                                                   */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the list part.                                 */
/*********************************************************************************/

/***************************************************************************/
/*  lay_lp                                                                 */
/***************************************************************************/

void    lay_lp( const gmltag * entry )
{
    char                *p;
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
        for( k = 0; k < TABLE_SIZE( lp_att ); k++ ) {
            curr = lp_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_left_indent:
                    if( AttrFlags.left_indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.lp.left_indent );
                    AttrFlags.left_indent = true;
                    break;
                case e_right_indent:
                    if( AttrFlags.right_indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.lp.right_indent );
                    AttrFlags.right_indent = true;
                    break;
                case e_line_indent:
                    if( AttrFlags.line_indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.lp.line_indent );
                    AttrFlags.line_indent = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.lp.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.lp.post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_spacing( p, &attr_val, &layout_work.lp.spacing );
                    AttrFlags.spacing = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                if( cvterr ) {          // there was an error
                    xx_err_exit( ERR_ATT_VAL_INV );
                    /* never return */
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err_exit( ERR_ATT_NAME_INV );
            /* never return */
        }
    }
    scandata.s = scandata.e;
    return;
}



/***************************************************************************/
/*   :LP        output list part attribute values                          */
/***************************************************************************/
void    put_lay_lp( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":LP\n" );

    for( k = 0; k < TABLE_SIZE( lp_att ); k++ ) {
        curr = lp_att[k];
        switch( curr ) {
        case e_left_indent:
            o_space_unit( fp, curr, &lay->lp.left_indent );
            break;
        case e_right_indent:
            o_space_unit( fp, curr, &lay->lp.right_indent );
            break;
        case e_line_indent:
            o_space_unit( fp, curr, &lay->lp.line_indent );
            break;
        case e_pre_skip:
            o_space_unit( fp, curr, &lay->lp.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( fp, curr, &lay->lp.post_skip );
            break;
        case e_spacing:
            o_spacing( fp, curr, &lay->lp.spacing );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}
