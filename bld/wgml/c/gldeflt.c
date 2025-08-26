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
* Description: WGML implement :DEFAULT LAYOUT tag
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :DEFAULT attributes                                                   */
/***************************************************************************/
static const lay_att    default_att[] = {
    e_spacing, e_columns, e_font, e_justify, e_input_esc, e_gutter, e_binding
};

/***************************************************************************/
/*Define default characteristics for document processing.                  */
/*                                                                         */
/*:DEFAULT                                                                 */
/*        spacing = 1                                                      */
/*        columns = 1                                                      */
/*        font = 0                                                         */
/*        justify = yes                                                    */
/*        input_esc = ' '                                                  */
/*        gutter = '0.5i'                                                  */
/*        binding = 0                                                      */
/*                                                                         */
/*spacing This attribute accepts a positive integer number. The spacing    */
/*determines the number of blank lines that are output between text        */
/*lines. If the line spacing is two, each text line will take two lines in */
/*the output. The number of blank lines between text lines will            */
/*therefore be the spacing value minus one. The spacing attribute          */
/*defines the line spacing within the document when there is no layout     */
/*entry for spacing with a specific document element.                      */
/*                                                                         */
/*columns The columns attribute accepts a positive integer number. The     */
/*columns value determines how many columns are created on each            */
/*output page.                                                             */
/*                                                                         */
/*font This attribute accepts a non-negative integer number. If a font     */
/*number is used for which no font has been defined, WATCOM                */
/*Script/GML will use font zero. The font numbers from zero to three       */
/*correspond directly to the highlighting levels specified by the          */
/*highlighting phrase GML tags. The font attribute defines the font of     */
/*the document when the font is not explicitly determined by the           */
/*document element.                                                        */
/*                                                                         */
/*justify The justify attribute accepts the keyword values yes and no.     */
/*Right justification of text is performed if this attribute has a value   */
/*of yes.                                                                  */
/*If justification is not desired, the value should be no.                 */
/*                                                                         */
/*input_esc The input escape attribute accepts the keyword value none or a */
/*quoted character. Input escapes are not recognized if the attribute      */
/*value is none or a blank. If a character is specified as the attribute   */
/*value, this character is used as the input escape delimiter. If an       */
/*empty('') or none value is specified, the blank value is used. Refer     */
/*to "Input Translation" on page 80 for more information.                  */
/*                                                                         */
/*gutter The gutter attribute specifies the amount of space between columns*/
/*in a multi-column document, and has no effect in a single column         */
/*document. This attribute accepts any valid horizontal space unit.        */
/*                                                                         */
/*binding The binding attribute accepts any valid horizontal space unit.   */
/*The binding value is added to the current left and right margins of those*/
/*output pages which are odd numbered.                                     */
/***************************************************************************/


/***************************************************************************/
/*  lay_default                                                            */
/***************************************************************************/

void    lay_default( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    spacing         :1;
        unsigned    columns         :1;
        unsigned    font            :1;
        unsigned    justify         :1;
        unsigned    input_esc       :1;
        unsigned    gutter          :1;
        unsigned    binding         :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( default_att ); k++ ) {
            curr = default_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_spacing( p, &attr_val, &layout_work.defaults.spacing );
                    AttrFlags.spacing = true;
                    break;
                case e_columns:
                    if( AttrFlags.columns ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_int8( p, &attr_val, &layout_work.defaults.columns );
                    AttrFlags.columns = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.defaults.font );
                    AttrFlags.font = true;
                    break;
                case e_justify:
                    if( AttrFlags.justify ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_yes_no( p, &attr_val, &layout_work.defaults.justify );
                    AttrFlags.justify = true;
                    break;
                case e_input_esc:
                    if( AttrFlags.input_esc ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_char( p, &attr_val, &layout_work.defaults.input_esc );
                    in_esc = layout_work.defaults.input_esc;
                    if( in_esc != ' ' ) {
                        ProcFlags.in_trans = true;
                    }
                    AttrFlags.input_esc = true;
                    break;
                case e_gutter:
                    if( AttrFlags.gutter ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.defaults.gutter );
                    AttrFlags.gutter = true;
                    break;
                case e_binding:
                    if( AttrFlags.binding ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.defaults.binding );
                    AttrFlags.binding = true;
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
/*   :DEFAULT   output default attribute values                            */
/***************************************************************************/
void    put_lay_default( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":DEFAULT\n" );

    for( k = 0; k < TABLE_SIZE( default_att ); k++ ) {
        curr = default_att[k];
        switch( curr ) {
        case e_spacing:
            o_spacing( fp, curr, &lay->defaults.spacing );
            break;
        case e_columns:
            o_int8( fp, curr, &lay->defaults.columns );
            break;
        case e_font:
            o_font_number( fp, curr, &lay->defaults.font );
            break;
        case e_justify:
            o_yes_no( fp, curr, &lay->defaults.justify );
            break;
        case e_input_esc:
            o_char( fp, curr, &lay->defaults.input_esc );
            break;
        case e_gutter:
            o_space_unit( fp, curr, &lay->defaults.gutter );
            break;
        case e_binding:
            o_space_unit( fp, curr, &lay->defaults.binding );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}
