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
* Description: WGML implement :BACKM and :BODY tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :BACKM and :BODY attributes                                           */
/***************************************************************************/
static const lay_att       backbod_att[] = {
    e_post_skip, e_pre_top_skip, e_header, e_backm_string, e_body_string,
    e_page_eject, e_page_reset, e_columns, e_font
};

/*********************************************************************************/
/*Define the characteristics of the back material section.                       */
/*:BACKM                                                                         */
/*        post_skip = 0                                                          */
/*        pre_top_skip = 0                                                       */
/*        header = no                                                            */
/*        backm_string = ""                                                      */
/*        page_eject = yes                                                       */
/*        page_reset = no                                                        */
/*        columns = 1                                                            */
/*        font = 1                                                               */
/*                                                                               */
/*post_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied       */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped after              */
/*the back material. The post-skip will be merged with the next                  */
/*document entitys pre-skip value. If a post-skip occurs at the end of           */
/*an output page, any remaining part of the skip is not carried over to          */
/*the next output page. If the back material heading is not displayed            */
/*(the header attribute has a value of NO), the post-skip value has no           */
/*effect.                                                                        */
/*                                                                               */
/*pre_top_skip This attribute accepts vertical space units. A zero value means   */
/*that no lines are skipped. If the skip value is a line unit, it is multiplied  */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for          */
/*more information). The resulting amount of space is skipped before             */
/*the back material. The pre-top-skip will be merged with the                    */
/*previous document entitys post-skip value. The specified space is              */
/*still skipped at the beginning of a new page.                                  */
/*                                                                               */
/*header The header attribute accepts the keyword values yes and no. If the      */
/*value yes is specified, the back material heading is generated. If the         */
/*value no is specified, the header text is not generated.                       */
/*backm_string This attribute accepts a character string. If the back material   */
/*header is generated, the specified string is used for the heading text.        */
/*                                                                               */
/*page_eject This attribute accepts the keyword values yes, no, odd, and even.   */
/*if the value no is specified, the heading is one column wide and is not        */
/*forced to a new page. The heading is always placed on a new page               */
/*when the value yes is specified. Values other than no cause the                */
/*heading to be treated as a page wide heading in a multi-column                 */
/*document.                                                                      */
/*The values odd and even will place the heading on a new page if the            */
/*parity (odd or even) of the current page number does not match the             */
/*specified value. When two headings appear together, the attribute              */
/*value stop_eject=yes of the :heading layout tag will normally                  */
/*prevent the the second heading from going to the next page. The                */
/*odd and even values act on the heading without regard to the                   */
/*stop_eject value.                                                              */
/*                                                                               */
/*page_reset This attribute accepts the keyword values yes and no. If the value  */
/*yes is specified, the page number is reset to one at the beginning of          */
/*the section.                                                                   */
/*                                                                               */
/*columns The columns attribute accepts a positive integer number. The           */
/*columns value determines how many columns are created for the                  */
/*back material.                                                                 */
/*                                                                               */
/*font This attribute accepts a non-negative integer number. If a font           */
/*number is used for which no font has been defined, WATCOM                      */
/*Script/GML will use font zero. The font numbers from zero to three             */
/*correspond directly to the highlighting levels specified by the                */
/*highlighting phrase GML tags. The font attribute defines the font of           */
/*the header attribute value. The font value is linked to the                    */
/*pre_top_skip and post_skip attributes (see "Font Linkage" on page              */
/*77).                                                                           */
/*********************************************************************************/

/***************************************************************************/
/*  lay_backbod   for :BACKM and :BODY                                     */
/***************************************************************************/

void    lay_backbod( const gmltag * entry )
{
    backbod_lay_tag *   bb;
    char            *   p;
    condcode            cc;
    hx_sect_lay_tag *   bbsect;
    int                 k;
    int                 cvterr;
    lay_att             curr;
    l_tags              ltag;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    post_skip       :1;
        unsigned    pre_top_skip    :1;
        unsigned    header          :1;
        unsigned    body_string     :1;
        unsigned    backm_string    :1;
        unsigned    page_eject      :1;
        unsigned    left_adjust     :1;
        unsigned    page_reset      :1;
        unsigned    font            :1;
        unsigned    columns         :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;
    ltag = entry->u.layid;
    if( ltag == TL_BACKM ) {
        bb  = &layout_work.backm;
        bbsect = &layout_work.hx.hx_sect[HDS_backm];
    } else if( ltag == TL_BODY ) {
        bb  = &layout_work.body;
        bbsect = &layout_work.hx.hx_sect[HDS_body];
    } else {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != ltag ) {
        ProcFlags.lay_xxx = ltag;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( backbod_att ); k++ ) {
            curr = backbod_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &(bbsect->post_skip) );
                    AttrFlags.post_skip = true;
                    break;
                case e_pre_top_skip:
                    if( AttrFlags.pre_top_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &(bbsect->pre_top_skip) );
                    AttrFlags.pre_top_skip = true;
                    break;
                case e_header:
                    if( AttrFlags.header ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_yes_no( p, &attr_val, &(bbsect->header) );
                    AttrFlags.header = true;
                    break;
                case e_body_string:
                    if( AttrFlags.body_string ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    if( ltag == TL_BODY ) {
                        cvterr = i_xx_string( p, &attr_val, bb->string );
                    }
                    AttrFlags.body_string = true;
                    break;
                case e_backm_string:
                    if( AttrFlags.backm_string ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    if( ltag == TL_BACKM ) {
                        cvterr = i_xx_string( p, &attr_val, bb->string );
                    }
                    AttrFlags.backm_string = true;
                    break;
                case e_page_eject:
                    if( AttrFlags.page_eject ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_page_eject( p, &attr_val, &(bb->page_eject) );
                    AttrFlags.page_eject = true;
                    break;
                case e_page_reset:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_yes_no( p, &attr_val, &(bb->page_reset) );
                    AttrFlags.page_reset = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &(bbsect->text_font) );
                    AttrFlags.font = true;
                    break;
                case e_columns:
                    if( AttrFlags.columns ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    if( ltag == TL_BACKM ) {
                        cvterr = i_int8( p, &attr_val, &(bb->columns) );
                    }
                    AttrFlags.columns = true;
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
/*   output for :BACKM or :BODY values                                     */
/***************************************************************************/
static  void    put_lay_backbod( FILE *fp, backbod_lay_tag * bb,
                                 hx_sect_lay_tag * bbsect, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":%s\n", name );

    for( k = 0; k < TABLE_SIZE( backbod_att ); k++ ) {
        curr = backbod_att[k];
        switch( curr ) {
        case e_post_skip:
            o_space_unit( fp, curr, &bbsect->post_skip );
            break;
        case e_pre_top_skip:
            o_space_unit( fp, curr, &bbsect->pre_top_skip );
            break;
        case e_header:
            o_yes_no( fp, curr, &bbsect->header );
            break;
        case e_body_string:
            if( *(name + 1) == 'O') {   // BODY tag
                o_xx_string( fp, curr, bb->string );
            }
            break;
        case e_backm_string:
            if( *(name + 1) == 'A') {   // BACKM tag
                o_xx_string( fp, curr, bb->string );
            }
            break;
        case e_page_eject:
            o_page_eject( fp, curr, &bb->page_eject );
            break;
        case e_page_reset:
            o_yes_no( fp, curr, &bb->page_reset );
            break;
        case e_columns:
            if( *(name + 1) == 'A') {   // BACKM tag
                o_int8( fp, curr, &bb->columns );
            }
            break;
        case e_font:
            o_font_number( fp, curr, &bbsect->text_font );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}

void    put_lay_backm( FILE *fp, layout_data * lay )
{
    put_lay_backbod( fp, &(lay->backm), &(lay->hx.hx_sect[HDS_backm]), "BACKM" );
}

void    put_lay_body( FILE *fp, layout_data * lay )
{
    put_lay_backbod( fp, &(lay->body), &(lay->hx.hx_sect[HDS_body]), "BODY" );
}
