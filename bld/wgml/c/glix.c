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
* Description: WGML implement :I1 - :I3 tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :I1 - :I3 attributes                                                  */
/***************************************************************************/
static const lay_att    ix_att[] = {
    e_pre_skip, e_post_skip, e_skip, e_font, e_indent, e_wrap_indent,
    e_index_delim, e_string_font
};

/**********************************************************************************/
/*Define the characteristics of an index entry level, where n is 1, 2, or 3.      */
/*The string_font attribute is only valid with index entry levels one and two.    */
/*                                                                                */
/*:I1                                                                             */
/*        pre_skip = 1                                                            */
/*        post_skip = 1                                                           */
/*        skip = 1                                                                */
/*        font = 0                                                                */
/*        indent = 0                                                              */
/*        wrap_indent = '0.4i'                                                    */
/*        index_delim = " "                                                       */
/*        string_font = 0                                                         */
/*                                                                                */
/*pre_skip This attribute accepts vertical space units. A zero value means that   */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped before              */
/*the index entry. The pre-skip will be merged with the previous                  */
/*document entity's post-skip value. If a pre-skip occurs at the                  */
/*beginning of an output page, the pre-skip value has no effect.                  */
/*                                                                                */
/*post_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped after               */
/*the index entry. The post-skip will be merged with the next                     */
/*document entity's pre-skip value. If a post-skip occurs at the end of           */
/*an output page, any remaining part of the skip is not carried over to           */
/*the next output page.                                                           */
/*                                                                                */
/*skip This attribute accepts vertical space units. A zero value means that       */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped                     */
/*between each entry in an index level.                                           */
/*                                                                                */
/*font This attribute accepts a non-negative integer number. If a font            */
/*number is used for which no font has been defined, WATCOM                       */
/*Script/GML will use font zero. The font numbers from zero to three              */
/*correspond directly to the highlighting levels specified by the                 */
/*highlighting phrase GML tags. The font attribute defines the font of            */
/*the index entry. The font value is linked to the wrap_indent, skip,             */
/*pre_skip and post_skip attributes (see "Font Linkage" on page 77).              */
/*                                                                                */
/*indent The indent attribute accepts any valid horizontal space unit. The        */
/*attribute space value is added to the current left margin before the            */
/*index entry is produced in the index. After the index entries under             */
/*the current entry are produced, the left margin is reset to its previous        */
/*value.                                                                          */
/*                                                                                */
/*wrap_indent This attribute accepts as a value any valid horizontal space unit.  */
/*if the list of references for an index entry in the index does not fit on       */
/*one output line, the specified attribute value indicates the                    */
/*indentation that is to occur on the following output lines.                     */
/*                                                                                */
/*index_delim This attribute accepts a string value which is placed between the   */
/*index text and the index page number(s). If the text, page number(s)            */
/*and delimiter does not fit on one output line, the delimiter text is not        */
/*used.                                                                           */
/*                                                                                */
/*string_font This attribute accepts a positive integer number, and is valid with */
/*the :i1 and :i2 layout tags. If a font number is used for which no              */
/*font has been defined, WATCOM Script/GML will use font zero.                    */
/*The font numbers from zero to three correspond directly to                      */
/*highlighting levels specified by the highlighting phrase GML tags.              */
/*The string_font attribute defines the font of the the see and see_also          */
/*attribute strings defined by the :INDEX layout tag.                             */
/**********************************************************************************/


/***************************************************************************/
/*  lay_ix          for :I1 - :I3                                          */
/***************************************************************************/

void    lay_ix( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 ix_l;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    pre_skip        :1;
        unsigned    post_skip       :1;
        unsigned    skip            :1;
        unsigned    font            :1;
        unsigned    indent          :1;
        unsigned    wrap_indent     :1;
        unsigned    index_delim     :1;
        unsigned    string_font     :1;
    } AttrFlags;

    p = g_scandata.s;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }

    ix_l = entry->tagname[1] - '1';     // construct Ix level  0 - 2
    if( ix_l > 2 ) {
        ix_l = 2;
    }

    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( ix_att ); k++ ) {
            curr = ix_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.ix[ix_l].pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.ix[ix_l].post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.ix[ix_l].skip );
                    AttrFlags.skip = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.ix[ix_l].font );
                    AttrFlags.font = true;
                    break;
                case e_indent:
                    if( AttrFlags.indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.ix[ix_l].indent );
                    AttrFlags.indent = true;
                    break;
                case e_wrap_indent:
                    if( AttrFlags.wrap_indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.ix[ix_l].wrap_indent );
                    AttrFlags.wrap_indent = true;
                    break;
                case e_index_delim:
                    if( AttrFlags.index_delim ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_xx_string( p, &attr_val,
                                          layout_work.ix[ix_l].index_delim );
                    AttrFlags.index_delim = true;
                    break;
                case e_string_font:
                    if( AttrFlags.string_font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    if( ix_l < 2 ) {
                        cvterr = i_font_number( p, &attr_val, &layout_work.ix[ix_l].string_font );
                    }
                    AttrFlags.string_font = true;
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
/*   :Ix        output index  attribute values for :I1 - :I3               */
/***************************************************************************/
void    put_lay_ix( FILE *fp, layout_data * lay )
{
    int                 k;
    int                 lvl;
    lay_att             curr;

    for( lvl = 0; lvl < 3; ++lvl ) {

        fprintf( fp, ":I%c\n", '1' + lvl );

        for( k = 0; k < TABLE_SIZE( ix_att ); k++ ) {
            curr = ix_att[k];
            switch( curr ) {
            case e_pre_skip:
                o_space_unit( fp, curr, &lay->ix[lvl].pre_skip );
                break;
            case e_post_skip:
                o_space_unit( fp, curr, &lay->ix[lvl].post_skip );
                break;
            case e_skip:
                o_space_unit( fp, curr, &lay->ix[lvl].skip );
                break;
            case e_font:
                o_font_number( fp, curr, &lay->ix[lvl].font );
                break;
            case e_indent:
                o_space_unit( fp, curr, &lay->ix[lvl].indent );
                break;
            case e_wrap_indent:
                o_space_unit( fp, curr, &lay->ix[lvl].wrap_indent );
                break;
            case e_index_delim:
                o_xx_string( fp, curr, lay->ix[lvl].index_delim );
                break;
            case e_string_font:
                if( lvl < 2 ) {         // :I3 has no string font
                    o_font_number( fp, curr, &lay->ix[lvl].string_font );
                }
                break;
            default:
                internal_err_exit( __FILE__, __LINE__ );
                /* never return */
            }
        }
    }
}
