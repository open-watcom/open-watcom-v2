/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description: WGML implement :DATE  tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :DATE     attributes                                                  */
/***************************************************************************/
static const lay_att     date_att[] = {
    e_date_form, e_left_adjust, e_right_adjust,
    e_page_position, e_font, e_pre_skip
};

/**********************************************************************************/
/*Defines the characteristics of the date entity in the standard tag format.      */
/*The :letdate layout tag defines the characteristics of the date entity in       */
/*the letter tag format.                                                          */
/*                                                                                */
/*:DATE                                                                           */
/*        date_form = "$ml $dsn, $yl"                                             */
/*        left_adjust = 0                                                         */
/*        right_adjust = '1i'                                                     */
/*        page_position = right                                                   */
/*        font = 0                                                                */
/*        pre_skip = 2                                                            */
/*                                                                                */
/*date_form The date_form attribute accepts a character string value which        */
/*defines the format of the date string. The year, month and day may              */
/*be specified separately and in any order by special date sequences.             */
/*These date sequences are started with a dollar($) sign and followed             */
/*by one to three characters. Text which is not recognized as a date              */
/*sequence can be entered to tailor the format of the resulting date.             */
/*The first character in a date sequence is a Y for the year, an M for            */
/*the month, or a D for the day. The next character is the L or S                 */
/*character to specify the long or short form of the date sequence. If            */
/*neither of these characters are present, the long form is used. When            */
/*the length specifier is present, the N character is used to format the          */
/*month or the day as a number. If the length specified is not present,           */
/*the month and day values are created in character form. The year is             */
/*always formatted as a number.                                                   */
/*                                                                                */
/*left_adjust The left_adjust attribute accepts any valid horizontal space unit.  */
/*The left margin is set to the page left margin plus the specified left          */
/*adjustment.                                                                     */
/*                                                                                */
/*right_adjust The right_adjust attribute accepts any valid horizontal space unit.*/
/*The right margin is set to the page right margin minus the specified            */
/*right adjustment.                                                               */
/*                                                                                */
/*page_position This attribute accepts the values left, right, center, and centre.*/
/*The position of the date between the left and right margins is determined       */
/*by the value selected. If left is the attribute value, the text is output       */
/*at the left margin. If right is the attribute value, the text is output         */
/*next to the right margin. When center or centre is specified, the text          */
/*is centered between the left and right margins.                                 */
/*                                                                                */
/*font This attribute accepts a non-negative integer number. If a font            */
/*number is used for which no font has been defined, WATCOM                       */
/*Script/GML will use font zero. The font numbers from zero to three              */
/*correspond directly to the highlighting levels specified by the                 */
/*highlighting phrase GML tags. The font attribute defines the font of            */
/*the date text. The font value is linked to the left_adjust, right_adjust        */
/*and pre_skip attributes (see "Font Linkage" on page 77).                        */
/*                                                                                */
/*pre_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped before              */
/*the date. The pre-skip will be merged with the previous document                */
/*entity's post-skip value. If a pre-skip occurs at the beginning of an           */
/*output page, the pre-skip value has no effect.                                  */
/**********************************************************************************/


/***************************************************************************/
/*  lay_date                                                               */
/***************************************************************************/

void    lay_date( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    date_form       :1;
        unsigned    left_adjust     :1;
        unsigned    right_adjust    :1;
        unsigned    page_position   :1;
        unsigned    font            :1;
        unsigned    pre_skip        :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( date_att ); k++ ) {
            curr = date_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_date_form:
                    if( AttrFlags.date_form ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_date_form( p, &attr_val, layout_work.date.date_form );
                    AttrFlags.date_form = true;
                    break;
                case e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.date.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.date.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case e_page_position:
                    if( AttrFlags.page_position ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_page_position( p, &attr_val,
                                          &layout_work.date.page_position );
                    AttrFlags.page_position = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.date.font );
                    AttrFlags.font = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.date.pre_skip );
                    AttrFlags.pre_skip = true;
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
    g_scandata.s = g_scandata.e;
    return;
}



/***************************************************************************/
/*   :DATE     output date attribute values                                */
/***************************************************************************/
void    put_lay_date( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":DATE\n" );

    for( k = 0; k < TABLE_SIZE( date_att ); k++ ) {
        curr = date_att[k];
        switch( curr ) {
        case e_date_form:
            o_date_form( fp, curr, lay->date.date_form );
            break;
        case e_left_adjust:
            o_space_unit( fp, curr, &lay->date.left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( fp, curr, &lay->date.right_adjust );
            break;
        case e_page_position:
            o_page_position( fp, curr, &lay->date.page_position );
            break;
        case e_font:
            o_font_number( fp, curr, &lay->date.font );
            break;
        case e_pre_skip:
            o_space_unit( fp, curr, &lay->date.pre_skip );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}
