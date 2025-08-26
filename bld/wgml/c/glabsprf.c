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
* Description: WGML implement :ABSTRCT and :PREFACE tags  LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :ABSTRACT and :PREFACE attributes                                     */
/***************************************************************************/

static const lay_att       abspref_att[] = {
    e_post_skip, e_pre_top_skip, e_font, e_spacing, e_header, e_abstract_string,
    e_preface_string, e_page_eject, e_page_reset, e_columns
};

/**********************************************************************************/
/*Define the characteristics of the abstract section and the abstract heading.    */
/*:ABSTRACT                      same for :PREFACE                                */
/*        post_skip = 1                                                           */
/*        pre_top_skip = 1                                                        */
/*        font = 1                                                                */
/*        spacing = 1                                                             */
/*        header = yes                                                            */
/*        abstract_string = "ABSTRACT"       preface_string = "PREFACE"           */
/*        page_eject = yes                                                        */
/*        page_reset = yes                                                        */
/*        columns = 1                                                             */
/*                                                                                */
/*post_skip This attribute accepts vertical space units. A zero value means that  */
/*no lines are skipped. If the skip value is a line unit, it is multiplied        */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped after               */
/*the abstract heading. The post-skip will be merged with the next                */
/*document entity's pre-skip value. If a post-skip occurs at the end of           */
/*an output page, any remaining part of the skip is not carried over to           */
/*the next output page. If the abstract heading is not displayed (the             */
/*header attribute has a value of NO), the post-skip value has no                 */
/*effect.                                                                         */
/*                                                                                */
/*pre_top_skip This attribute accepts vertical space units. A zero value means    */
/*that no lines are skipped. If the skip value is a line unit, it is multiplied   */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for           */
/*more information). The resulting amount of space is skipped before              */
/*the abstract heading. The pre-top-skip will be merged with the                  */
/*previous document entity's post-skip value. The specified space is              */
/*still skipped at the beginning of a new page.                                   */
/*                                                                                */
/*font This attribute accepts a non-negative integer number. If a font            */
/*number is used for which no font has been defined, WATCOM                       */
/*Script/GML will use font zero. The font numbers from zero to three              */
/*correspond directly to the highlighting levels specified by the                 */
/*highlighting phrase GML tags. The font attribute defines the font of            */
/*the abstract heading. The font value is linked to the pre_top_skip              */
/*and post_skip attributes (see "Font Linkage" on page 77).                       */
/*                                                                                */
/*spacing This attribute accepts a positive integer number. The spacing           */
/*determines the number of blank lines that are output between text               */
/*lines. If the line spacing is two, each text line will take two lines in        */
/*the output. The number of blank lines between text lines will                   */
/*therefore be the spacing value minus one. The spacing attribute                 */
/*defines the line spacing within the abstract section.                           */
/*                                                                                */
/*header The header attribute accepts the keyword values yes and no. If the       */
/*value yes is specified, the abstract heading is generated. If the value         */
/*no is specified, the header text is not generated.                              */
/*                                                                                */
/*abstract_string This attribute accepts a character string. If the abstract      */
/*header is generated, the specified string is used for the heading text.         */
/*                                                                                */
/*page_eject This attribute accepts the keyword values yes, no, odd, and even. If */
/*the value no is specified, the heading is one column wide and is not            */
/*forced to a new page. The heading is always placed on a new page                */
/*when the value yes is specified. Values other than no cause the                 */
/*heading to be treated as a page wide heading in a multi-column                  */
/*document.                                                                       */
/*The values odd and even will place the heading on a new page if the             */
/*parity (odd or even) of the current page number does not match the              */
/*specified value. When two headings appear together, the attribute               */
/*value stop_eject=yes of the :heading layout tag will normally                   */
/*prevent the the second heading from going to the next page. The                 */
/*odd and even values act on the heading without regard to the                    */
/*stop_eject value.                                                               */
/*                                                                                */
/*page_reset This attribute accepts the keyword values yes and no. If the value   */
/*yes is specified, the page number is reset to one at the beginning of           */
/*the section. With the :ABSTRACT tag only, a value of yes will                   */
/*cause the page number to always be reset after the title page.                  */
/*                                                                                */
/*columns The columns attribute accepts a positive integer number. The            */
/*columns value determines how many columns are created for the                   */
/*abstract.                                                                       */
/**********************************************************************************/


/***************************************************************************/
/*  lay_abspref   for :ABSTRACT and :PREFACE                               */
/***************************************************************************/

void    lay_abspref( const gmltag * entry )
{
    abspref_lay_tag *   ap;
    hx_sect_lay_tag *   apsect;
    char            *   p;
    condcode            cc;
    int                 k;
    int                 cvterr;
    lay_att             curr;
    l_tags              ltag;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    post_skip       :1;
        unsigned    pre_top_skip    :1;
        unsigned    font            :1;
        unsigned    spacing         :1;
        unsigned    header          :1;
        unsigned    abstract_string :1;
        unsigned    preface_string  :1;
        unsigned    page_eject      :1;
        unsigned    page_reset      :1;
        unsigned    columns         :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;
    ltag = entry->u.layid;
    if( ltag == TL_ABSTRACT ) {
        ap = &layout_work.abstract;
        apsect = &layout_work.hx.hx_sect[HDS_abstract];
    } else if( ltag == TL_PREFACE ) {
        ap = &layout_work.preface;
        apsect = &layout_work.hx.hx_sect[HDS_preface];
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
        for( k = 0; k < TABLE_SIZE( abspref_att ); k++ ) {
            curr = abspref_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &(apsect->post_skip) );
                    AttrFlags.post_skip = true;
                    break;
                case e_pre_top_skip:
                    if( AttrFlags.pre_top_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &(apsect->pre_top_skip) );
                    AttrFlags.pre_top_skip = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &(apsect->text_font) );
                    AttrFlags.font = true;
                    break;
                case e_spacing:
                    if( AttrFlags.spacing ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_spacing( p, &attr_val, &(apsect->spacing) );
                    AttrFlags.spacing = true;
                    break;
                case e_header:
                    if( AttrFlags.header ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_yes_no( p, &attr_val, &(apsect->header) );
                    AttrFlags.header = true;
                    break;
                case e_abstract_string:
                    if( ltag == TL_ABSTRACT ) {
                        if( AttrFlags.abstract_string ) {
                            xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                                attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                            /* never return */
                        }
                        cvterr = i_xx_string( p, &attr_val, ap->string );
                        AttrFlags.abstract_string = true;
                    }
                    break;
                case e_preface_string:
                    if( ltag == TL_PREFACE ) {
                        if( AttrFlags.preface_string ) {
                            xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                                attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                            /* never return */
                        }
                        cvterr = i_xx_string( p, &attr_val, ap->string );
                        AttrFlags.preface_string = true;
                    }
                    break;
                case e_page_eject:
                    if( AttrFlags.page_eject ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_page_eject( p, &attr_val, &(ap->page_eject) );
                    AttrFlags.page_eject = true;
                    break;
                case e_page_reset:
                    if( AttrFlags.page_reset ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_yes_no( p, &attr_val, &(ap->page_reset) );
                    AttrFlags.page_reset = true;
                    break;
                case e_columns:
                    if( AttrFlags.columns ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_int8( p, &attr_val, &(ap->columns) );
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
/*   output for :ABSTRACT or :PREFACE values                               */
/***************************************************************************/
static  void    put_lay_abspref( FILE *fp, abspref_lay_tag * ap,
                                 hx_sect_lay_tag * apsect, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":%s\n", name );

    for( k = 0; k < TABLE_SIZE( abspref_att ); k++ ) {
        curr = abspref_att[k];
        switch( curr ) {
        case e_post_skip:
            o_space_unit( fp, curr, &apsect->post_skip );
            break;
        case e_pre_top_skip:
            o_space_unit( fp, curr, &apsect->pre_top_skip );
            break;
        case e_font:
            o_font_number( fp, curr, &apsect->text_font );
            break;
        case e_spacing:
            o_spacing( fp, curr, &apsect->spacing );
            break;
        case e_header:
            o_yes_no( fp, curr, &apsect->header );
            break;
        case e_abstract_string:
            if( *name == 'A' ) {        // :Abstract output
                o_xx_string( fp, curr, ap->string );
            }
            break;
        case e_preface_string:
            if( *name == 'P' ) {        // :Preface output
                o_xx_string( fp, curr, ap->string );
            }
            break;
        case e_page_eject:
            o_page_eject( fp, curr, &ap->page_eject );
            break;
        case e_page_reset:
            o_yes_no( fp, curr, &ap->page_reset );
            break;
        case e_columns:
            o_int8( fp, curr, &ap->columns );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}

void    put_lay_abstract( FILE *fp, layout_data * lay )
{
    put_lay_abspref( fp, &(lay->abstract), &(lay->hx.hx_sect[HDS_abstract]), "ABSTRACT" );
}

void    put_lay_preface( FILE *fp, layout_data * lay )
{
    put_lay_abspref( fp, &(lay->preface), &(lay->hx.hx_sect[HDS_preface]), "PREFACE" );
}
