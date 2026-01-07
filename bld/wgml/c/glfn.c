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
* Description: WGML implement :FN and :FNREF tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :FN    attributes                                                     */
/***************************************************************************/
static const lay_att    fn_att[] = {
    e_line_indent, e_align, e_pre_lines, e_skip, e_spacing, e_font,
    e_number_font, e_number_style, e_frame
};


/***************************************************************************/
/*   :FNREF attributes                                                     */
/***************************************************************************/
static const lay_att    fnref_att[] = {
    e_font, e_number_style
};


/*********************************************************************************/
/*Define the characteristics of the footnote entity.                             */
/*                                                                               */
/*:FN                                                                            */
/*        line_indent = 0                                                        */
/*        align = '0.4i'                                                         */
/*        pre_lines = 2                                                          */
/*        skip = 2                                                               */
/*        spacing = 1                                                            */
/*        font = 0                                                               */
/*        number_font = 0                                                        */
/*        number_style = h                                                       */
/*        frame = none                                                           */
/*                                                                               */
/*line_indent The line_indent attribute accepts any valid horizontal space unit. */
/*This attribute specifies the amount of indentation for the first output        */
/*line of the footnote.                                                          */
/*                                                                               */
/*align This attribute accepts any valid horizontal space unit. The align        */
/*value specifies the amount of space reserved for the footnote                  */
/*number. After the footnote number is produced, the align value is              */
/*added to the current left margin. The left margin will be reset to its         */
/*previous value after the footnote.                                             */
/*                                                                               */
/*pre_lines This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting number of lines are skipped                   */
/*before the footnotes are output. If the document entity starts a new           */
/*page, the specified number of lines are still skipped. The pre-lines           */
/*value is not merged with the previous document entity's post-skip              */
/*value.                                                                         */
/*                                                                               */
/*skip This attribute accepts vertical space units. A zero value means that      */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped                    */
/*between the footnotes.                                                         */
/*                                                                               */
/*spacing This attribute accepts a positive integer number. The spacing          */
/*determines the number of blank lines that are output between text              */
/*lines. If the line spacing is two, each text line will take two lines in       */
/*the output. The number of blank lines between text lines will                  */
/*therefore be the spacing value minus one. The spacing attribute                */
/*defines the line spacing within the footnote.                                  */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the footnote text. The font value is linked to the line_indent,                */
/*pre_lines, skip and align attributes (see "Font Linkage" on page 77).          */
/*                                                                               */
/*number_font This attribute accepts a non-negative integer number. If a font    */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The number font attribute defines                */
/*the font of the footnote number.                                               */
/*                                                                               */
/*number_style This attribute sets the number style of the footnote number.      */
/*(See "Number Style" on page 121).                                              */
/*                                                                               */
/*frame This attribute accepts the values rule or none. If the value rule is     */
/*specified, a rule line is placed between the main body of text and the         */
/*footnotes at the bottom of the output page. If the footnote is placed          */
/*across the entire page, the width of the rule line is half the width of        */
/*the page. If the footnote is one column wide, the rule line width is           */
/*the width of a column minus twenty percent.                                    */
/*********************************************************************************/


/******************************************************************************/
/*Define the characteristics of the footnote reference entity.                */
/*                                                                            */
/*:FNREF                                                                      */
/*        font = 0                                                            */
/*        number_style = hp                                                   */
/*                                                                            */
/*font This attribute accepts a non-negative integer number. If a font        */
/*number is used for which no font has been defined, WATCOM                   */
/*Script/GML will use font zero. The font numbers from zero to three          */
/*correspond directly to the highlighting levels specified by the             */
/*highlighting phrase GML tags. The font attribute defines the font of        */
/*the footnote reference text.                                                */
/*                                                                            */
/*number_style This attribute sets the number style of the footnote reference */
/*number. (See "Number Style" on page 121).                                   */
/******************************************************************************/


/***************************************************************************/
/*  lay_fn                                                                 */
/***************************************************************************/

void    lay_fn( const gmltag * entry )
{
    bool                cvterr;
    char                *p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    line_indent     :1;
        unsigned    align           :1;
        unsigned    pre_lines       :1;
        unsigned    skip            :1;
        unsigned    spacing         :1;
        unsigned    font            :1;
        unsigned    number_font     :1;
        unsigned    number_style    :1;
        unsigned    frame           :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = true;
        for( k = 0; k < TABLE_SIZE( fn_att ); k++ ) {
            curr = fn_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_line_indent:
                    if( AttrFlags.line_indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.fn.line_indent );
                    AttrFlags.line_indent = true;
                    break;
                case e_align:
                    if( AttrFlags.align ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.fn.align );
                    AttrFlags.align = true;
                    break;
                case e_pre_lines:
                    if( AttrFlags.pre_lines ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.fn.pre_lines );
                    AttrFlags.pre_lines = true;
                    break;
                case e_skip:
                    if( AttrFlags.skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &layout_work.fn.skip );
                    AttrFlags.skip = true;
                    break;
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_spacing( p, &attr_val, &layout_work.fn.spacing );
                    AttrFlags.spacing = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.fn.font );
                    AttrFlags.font = true;
                    break;
                case e_number_font:
                    if( AttrFlags.number_font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.fn.number_font );
                    AttrFlags.number_font = true;
                    break;
                case e_number_style:
                    if( AttrFlags.number_style ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_number_style( p, &attr_val,
                                             &layout_work.fn.number_style );
                    AttrFlags.number_style = true;
                    break;
                case e_frame:
                    if( AttrFlags.frame ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_frame( p, &attr_val, &layout_work.fn.frame );
                    AttrFlags.frame = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                if( cvterr ) {                  // there was an error
                    xx_err_exit( ERR_ATT_VAL_INV );
                    /* never return */
                }
                break;                  // break out of for loop
            }
        }
    }
    g_scandata.s = g_scandata.e;
    return;
}

/***************************************************************************/
/*  lay_fnref                                                              */
/***************************************************************************/
void    lay_fnref( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    font            :1;
        unsigned    number_style    :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( fnref_att ); k++ ) {
            curr = fnref_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.fnref.font );
                    AttrFlags.font = true;
                    break;
                case e_number_style:
                    if( AttrFlags.number_style ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_number_style( p, &attr_val, &layout_work.fnref.number_style );
                    AttrFlags.number_style = true;
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
/*   :FN        output footnote attribute values                            */
/***************************************************************************/
void    put_lay_fn( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":FN\n" );

    for( k = 0; k < TABLE_SIZE( fn_att ); k++ ) {
        curr = fn_att[k];
        switch( curr ) {
        case e_line_indent:
            o_space_unit( fp, curr, &lay->fn.line_indent );
            break;
        case e_align:
            o_space_unit( fp, curr, &lay->fn.align );
            break;
        case e_pre_lines:
            o_space_unit( fp, curr, &lay->fn.pre_lines );
            break;
        case e_skip:
            o_space_unit( fp, curr, &lay->fn.skip );
            break;
        case e_spacing:
            o_spacing( fp, curr, &lay->fn.spacing );
            break;
        case e_font:
            o_font_number( fp, curr, &lay->fn.font );
            break;
        case e_number_font:
            o_font_number( fp, curr, &lay->fn.number_font );
            break;
        case e_number_style:
            o_number_style( fp, curr, &lay->fn.number_style );
            break;
        case e_frame:
            o_frame( fp, curr, &lay->fn.frame );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}


/***************************************************************************/
/*   :FNREF     output footnote attribute values                            */
/***************************************************************************/
void    put_lay_fnref( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":FNREF\n" );

    for( k = 0; k < TABLE_SIZE( fnref_att ); k++ ) {
        curr = fnref_att[k];
        switch( curr ) {
        case e_font:
            o_font_number( fp, curr, &lay->fnref.font );
            break;
        case e_number_style:
            o_number_style( fp, curr, &lay->fnref.number_style );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}
