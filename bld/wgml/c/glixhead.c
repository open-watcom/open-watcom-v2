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
* Description: WGML implement :IXHEAD tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :IXHEAD   attributes                                                    */
/***************************************************************************/
static const lay_att    ixhead_att[] = {
    e_pre_skip, e_post_skip, e_font, e_indent, e_ixhead_frame, e_header
};


/*********************************************************************************/
/*Define the characteristics of the index headings. In most cases, the index     */
/*heading is the letter which starts the index terms following it.               */
/*:IXHEAD                                                                        */
/*        pre_skip = 2                                                           */
/*        post_skip = 0                                                          */
/*        font = 2                                                               */
/*        indent = 0                                                             */
/*        frame = box                                                            */
/*        header = yes                                                           */
/*                                                                               */
/*pre_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the index heading. The pre-skip will be merged with the previous               */
/*document entity's post-skip value. If a pre-skip occurs at the                 */
/*beginning of an output page, the pre-skip value has no effect.                 */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the index heading. The post-skip will be merged with the next                  */
/*document entity's pre-skip value. If a post-skip occurs at the end of          */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page.                                                          */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the index heading. The font value is linked to the indent, pre_skip            */
/*and post_skip attributes (see "Font Linkage" on page 77).                      */
/*                                                                               */
/*indent The indent attribute accepts any valid horizontal space unit. The       */
/*attribute space value is added to the current left margin before the           */
/*index heading is generated in the index. The left margin is reset to           */
/*its previous value after the heading is generated.                             */
/*                                                                               */
/*frame This attribute accepts the values rule, box, none, and 'character        */
/*string'. The specified attribute value determines the type of framing          */
/*around the index heading. See the discussion of the frame attribute            */
/*under "FIG" on page 92 for an explanation of the attribute values.             */
/*                                                                               */
/*header This attribute accepts the keyword values yes and no. If 'no' is        */
/*specified, the index heading is not displayed. The font and frame              */
/*attributes are ignored, and the pre and post skip values are merged.           */
/*********************************************************************************/

/***************************************************************************/
/*  lay_ixhead                                                             */
/***************************************************************************/

void    lay_ixhead( const gmltag * entry )
{
    char            *   p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    pre_skip        :1;
        unsigned    post_skip       :1;
        unsigned    font            :1;
        unsigned    indent          :1;
        unsigned    frame           :1;
        unsigned    header          :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( ixhead_att ); k++ ) {
            curr = ixhead_att[k];
            if( curr == e_ixhead_frame ) {
                curr = e_frame;         // use correct externalname
            }
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
                                           &layout_work.ixhead.pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.ixhead.post_skip );
                    AttrFlags.post_skip = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.ixhead.font );
                    AttrFlags.font = true;
                    break;
                case e_indent:
                    if( AttrFlags.indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.ixhead.indent );
                    AttrFlags.indent = true;
                    break;
                case e_frame:
                    if( AttrFlags.frame ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_default_frame( p, &attr_val,
                                           &layout_work.ixhead.frame );
                    AttrFlags.frame = true;
                    break;
                case e_header:
                    if( AttrFlags.header ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_yes_no( p, &attr_val,
                                           &layout_work.ixhead.header );
                    AttrFlags.header = true;
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
/*   :IXHEAD    output index header attribute values                       */
/***************************************************************************/
void    put_lay_ixhead( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":IXHEAD\n" );

    for( k = 0; k < TABLE_SIZE( ixhead_att ); k++ ) {
        curr = ixhead_att[k];
        switch( curr ) {
        case e_pre_skip:
            o_space_unit( fp, curr, &lay->ixhead.pre_skip );
            break;
        case e_post_skip:
            o_space_unit( fp, curr, &lay->ixhead.post_skip );
            break;
        case e_font:
            o_font_number( fp, curr, &lay->ixhead.font );
            break;
        case e_indent:
            o_space_unit( fp, curr, &lay->ixhead.indent );
            break;
        case e_ixhead_frame:
            curr = e_frame;             // frame = instead of ixhead_frame =
            o_default_frame( fp, curr, &lay->ixhead.frame );
            break;
        case e_header:
            o_yes_no( fp, curr, &lay->ixhead.header );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}
