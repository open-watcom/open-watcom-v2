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
* Description:  WGML tags :CIT :eCIT :HP0 :eHP0 :HP1, :eHP1 :HP2, :eHP2
*                         :HP3, :eHP3, :Q, :eQ, :SF, and :eSF processing
*
* These tags all begin/end some form of inline phrase
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


static  font_number     tt_font     = FONT0;    // font number to be replaced with "0"
static  unsigned        quote_lvl   = 0;        // nesting level of Q phrases

/***************************************************************************/
/*  :CIT :HPx :Q :SF common processing                                        */
/***************************************************************************/

static void gml_inline_common( const gmltag *entry, font_number font )
{
    bool            sav_sbl = ProcFlags.skip_blank_line;
    char            *p;
    font_number     o_c_font;

    if( ProcFlags.overprint
      && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    // update block_font, if appropriate

    if( (t_doc_el_group != NULL)
      && (t_doc_el_group->owner == GRT_co)
      && (t_doc_el_group->first == NULL) ) {
        t_doc_el_group->block_font = font;
    }

    // keep the preceding space, even if followed by CT

    if( ProcFlags.concat
      && !ProcFlags.cont_char ) {
        if( !input_cbs->fm_hh
          && !ProcFlags.ct ) {
            if( post_space == 0 ) {
                post_space = wgml_fonts[g_curr_font].spc_width;
                if( (t_line != NULL)
                  && (t_line->last != NULL) ) {                                  // add second space after stop
                    if( is_stop_char( t_line->last->text[t_line->last->count - 1] ) ) {
                        post_space *= 2;
                    }
                }
            }
        } else if( (entry->u.tagid == T_SF)
          && (input_cbs->fmflags & II_macro) ) {   // may apply more generally
            ProcFlags.utc = true;
            if( (post_space == 0)
              && (input_cbs->sym_space
              || (input_cbs->fm_symbol
              && !ProcFlags.ct)) ) {
                post_space = wgml_fonts[g_curr_font].spc_width;
                if( (t_line != NULL)
                  && (t_line->last != NULL) ) {                                  // add second space after stop
                    if( is_stop_char( t_line->last->text[t_line->last->count - 1] ) ) {
                        post_space *= 2;
                    }
                }
            }
        }
    }

    /* Implements wgml 4.0 behavior */

    if( (entry->u.tagid == T_SF)
      && (input_cbs->fmflags & II_tag)
      && (cur_group_type == GRT_xmp) ) {
        if( ProcFlags.xmp_ut_sf ) {     // matches wgml 4.0
            scr_process_break();
        } else {
            ProcFlags.xmp_ut_sf = true;
            if( (input_cbs->s.m->tag->tagflags & GTFLG_cont) == 0 ) {
                scr_process_break();        // tag not defined with CONT
            }
        }
    }

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();

    g_phrase_font = font;
    CHECK_FONT( font );

    /* if pc is to be used, use it before changing g_curr_font */
    if( ProcFlags.force_pc ) {
        do_force_pc( NULL );
    }

    /* this should be g_curr_font, but making it so causes complications see forWiki notes */
    nest_cb->font = font;
    o_c_font = g_curr_font;
    g_curr_font = font;

    nest_cb->gtag = entry->u.tagid;

    nest_cb->align = nest_cb->prev->align;
    nest_cb->left_indent = nest_cb->prev->left_indent;
    nest_cb->right_indent = nest_cb->prev->right_indent;

    if( nest_cb->prev != NULL ) {               // at least one prior entry
        if( nest_cb->prev->prev == NULL ) {     // but only one
            if( nest_cb->font != nest_cb->prev->font ) {           // font actually changed
                tt_font = font;                 // save current value
            }
        }
    }

    g_scan_err = false;
    p = g_scandata.s;
    SkipDot( p );                       // over '.'

    if( entry->u.tagid == T_Q ) {       // Q/eQ inserts quote char
        if( (quote_lvl % 2) ) {
            token_buf[0] = s_q;
        } else {
            token_buf[0] = d_q;
        }
        if( !ProcFlags.concat
          && ((input_cbs->hidden_head != NULL)
          || (*p != '\0')) ) {
            token_buf[1] = CONT_char;
            token_buf[2] = '\0';
        } else {
            token_buf[1] = '\0';
        }
        process_text( token_buf, g_curr_font );
        if( ProcFlags.concat
          && ((input_cbs->hidden_head != NULL)
          || (*p != '\0')) ) {
            post_space = 0;                 // no space after quote
            ProcFlags.cont_char = true;
        }
        quote_lvl++;
    }

    if( *p != '\0' ) {
        if( (*(p + 1) == '\0')
          && (*p == CONT_char) ) { // text is continuation character only
            /* tbd */
        } else {
            process_text( p, g_curr_font);          // if text follows
        }
    } else if( entry->u.tagid != T_Q ) {
        if( ProcFlags.concat
          && !ProcFlags.cont_char
          && (entry->u.tagid == T_SF) ) {
            post_space = wgml_fonts[o_c_font].spc_width;
        } else{
            post_space = wgml_fonts[g_curr_font].spc_width;
        }
    }

    if( (entry->u.tagid == T_SF)
      && sav_sbl ) {      // reset flag, but only if was set on entry, and only for SF
        ProcFlags.skip_blank_line = sav_sbl;
    }

    if( !ProcFlags.concat
      && !ProcFlags.cont_char
      && (input_cbs->fmflags & (II_file | II_macro)) ) {
        scr_process_break();            // ensure line is output
    }
    g_scandata.s = g_scandata.e;
}


/***************************************************************************/
/*  :HP0  :HP1  :HP2  :HP3                                                 */
/*                                                                         */
/* Format: :HPn. (n=0,1,2,3)                                               */
/* These tags start the highlighting of phrases at one of the four levels  */
/* provided by GML. The actual highlighting to be performed is determined  */
/* by the type of device for which the document is being formatted.        */
/* Examples of highlighting include underlining, displaying in bold face,  */
/* or using a different character shape (such as italics).                 */
/* Highlighting may not be used when the GML layout explicitly determines  */
/* the emphasis to be used, such as in the text of a heading.              */
/* The highlighting tags are paragraph elements. They are used with text   */
/* to create the content of a basic document element, such as a paragraph. */
/* A corresponding :EHPn tag must be specified for each :HPn tag.          */
/***************************************************************************/

void gml_hp0( const gmltag *entry )
{
    gml_inline_common( entry, FONT0 );
}

void gml_hp1( const gmltag *entry )
{
    gml_inline_common( entry, FONT1 );
}

void gml_hp2( const gmltag *entry )
{
    gml_inline_common( entry, FONT2 );
}

void gml_hp3( const gmltag *entry )
{
    gml_inline_common( entry, FONT3 );
}


/***************************************************************************/
/*  :eCIT :eHPx :eQ :eSF common processing                                 */
/***************************************************************************/

static void gml_e_inline_common( const gmltag *entry )
{
    char            *p;
    tag_cb          *wk;

    check_close_tag_err_exit( entry->u.tagid );
    /* Mark end of highlighted phrase embedded in a highlighted phrase */
    if( cur_group_type != GRT_xmp
      && ProcFlags.concat ) {
        switch( nest_cb->prev->gtag ) {    // testing showed they all do this, in either phrase
        case T_CIT:
        case T_HP0:
        case T_HP1:
        case T_HP2:
        case T_HP3:
        case T_Q:
        case T_SF:
            ProcFlags.einl_in_inlp = true;  // restrict further as needed for embedded phrase
            break;
        default:
            ProcFlags.einl_in_inlp = false; // cancel when outermost inline phrase closes
        }
    }

    if( ProcFlags.xmp_ut_sf ) {   // matches wgml 4.0
        scr_process_break();
    }
    if( nest_cb->prev->font == tt_font ) {      // returning to second stack entry
        tt_stack = nest_cb->prev;               // set tt_stack
    }
    if( nest_cb == tt_stack ) {                 // closing second stack entry
        tt_stack = NULL;                        // clear tt_stack
    }
    nest_cb->prev->left_indent = nest_cb->left_indent;
    nest_cb->prev->right_indent = nest_cb->right_indent;
    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );

    if( entry->u.tagid != T_EQ ) {                    // Q/eQ does not restore the prior font
        g_curr_font = nest_cb->font;
    }

    /************************************************************************/
    /* when concatenation is on, the continue character is not needed to    */
    /* cause input records to be put onto the same output line              */
    /* but the presence/absence of a space before any text output by an     */
    /* inline end tag must be determined as if a continue char were present */
    /* when an inline end tag (used inside a user-defined tag or not) is    */
    /* at the start of an input record from a file or a macro, then any     */
    /* space must be cancelled unless the prior input record ended with a   */
    /* continue character                                                   */
    /************************************************************************/

    if( ProcFlags.concat ) {
        if( ProcFlags.cont_char ) {
        } else {
            ProcFlags.cont_char = true;
            if( ProcFlags.space_fnd ) {
                if( input_cbs->hh_tag ) {
                    if( (entry->u.tagid != T_ECIT)
                      && (entry->u.tagid != T_EQ) ) {
                        post_space = 0;
                    }
                } else {
                    if( !ProcFlags.fsp ) {  // space not from substitution
                        post_space = 0;
                    }
                }
            } else {
                post_space = 0;
            }
        }
    }

    if( entry->u.tagid == T_EQ ) {          // Q/eQ insert quote character
        quote_lvl--;
        if( (quote_lvl % 2) ) {
            token_buf[0] = s_q;
        } else {
            token_buf[0] = d_q;
        }
        token_buf[1] = CONT_char;
        token_buf[2] = '\0';
        process_text( token_buf, g_curr_font );
    }

    g_scan_err = false;
    p = g_scandata.s;
    SkipDot( p );                           // over '.'
    if( *p != '\0' ) {
        if( (*(p + 1) == '\0')
          && (*p == CONT_char) ) { // text is continuation character only
            /* tbd */
        } else {
            process_text( p, g_curr_font);          // if text follows
        }
    } else {
        if( input_cbs->hidden_head == NULL ) {  // no text, no continue char
            ProcFlags.cont_char = false;
        }
    }
    if( !ProcFlags.concat
      && !ProcFlags.cont_char
      && (input_cbs->fmflags & (II_file | II_macro)) ) {
        scr_process_break();        // ensure line is output
    }
    if( g_script_style_sav.style != SCT_none ) {
        if( entry->u.tagid != T_EQ ) {
            scr_style_copy( &g_script_style_sav, &g_script_style );
            ProcFlags.scr_scope_eip = true;
        }
        g_script_style_sav.style = SCT_none;
    }
    g_scandata.s = g_scandata.e;
}


/***************************************************************************/
/*                                                                         */
/* EHP0, EHP1, EHP2, EHP3                                                  */
/*                                                                         */
/* Format: :eHPn. (n=0,1,2,3)                                              */
/* These tags end the highlighting of phrases at one of the four levels    */
/* provided by GML.                                                        */
/* Each :ehpn tag must be preceded by a corresponding :hpn tag.            */
/***************************************************************************/

void gml_ehp0( const gmltag *entry )
{
    gml_e_inline_common( entry );
}

void gml_ehp1( const gmltag *entry )
{
    gml_e_inline_common( entry );
}

void gml_ehp2( const gmltag *entry )
{
    gml_e_inline_common( entry );
}

void gml_ehp3( const gmltag *entry )
{
    gml_e_inline_common( entry );
}

/***************************************************************************/
/*  :esf tag processing                                                    */
/*                                                                         */
/*   Format: :eSF.                                                         */
/*                                                                         */
/*   This tag ends the highlighting of phrases started by the last :sf tag */
/***************************************************************************/

void gml_esf( const gmltag *entry )
{
    gml_e_inline_common( entry );
}


/***************************************************************************/
/*  :SF  processing                                                        */
/*                                                                         */
/*  Format: :SF font=number.                                               */
/*  The set font tag starts the highlighting of phrases at the level       */
/*  specified by the required attribute font. The actual highlighting      */
/*  to be performed is determined by the type of device for which the      */
/*  document is being formatted. Examples of highlighting include          */
/*  underlining, displaying in bold face, or using a different             */
/*  character shape (such as italics).                                     */
/*  The value of the font attribute is a non-negative integer number.      */
/*  If the specified number is larger than the last defined font for the   */
/*  document, font for zero is used. Highlighting may not be used when     */
/*  the GML layout explicitly determines the emphasis to be used, such     */
/*  as in the text of a heading. The set font tag is a paragraph element.  */
/*  It is used with text to create the content of a basic document element,*/
/*  such as a paragraph. A corresponding :ESF tag must be specified for    */
/*  each :SF tag.                                                          */
/*                                                                         */
/*  NOTE:                                                                  */
/*    A font number greater than the maximum font number will indeed be    */
/*    replaced by "0", but it cannot be done here because subsequent       */
/*    processing needs to know what the actual font was                    */
/***************************************************************************/

void gml_sf( const gmltag *entry )
{
    bool            font_seen   =   false;
    char            *p;
    char            *pa;
    font_number     font;
    att_name_type   attr_name;
    att_val_type    attr_val;

    p = g_scandata.s;
    SkipSpaces( p );
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        p = get_tag_att_name( p, &pa, &attr_name );
        if( !ProcFlags.reprocess_line
          && !ProcFlags.tag_end_found ) {
            if( strcmp( "font", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s != NULL ) {
                    font = get_font_number( attr_val.tok.s, attr_val.tok.l );
                    font_seen = true;
                    g_scandata.s = p;
                    gml_inline_common( entry, font );
                }
            }
        }
    }
    if( !font_seen ) {          // font is a required attribute
        xx_line_err_exit_c( ERR_ATT_MISSING, g_scandata.s );
        /* never return */
    }

    g_scandata.s = g_scandata.e;
}


/********************************************************************************************/
/* Format: :CIT.                                                                            */
/*                                                                                          */
/* This tag starts the highlighting of a citation (e.g., the title of a book). The actual   */
/* highlighting to be performed is determined by the layout and the type of output device   */
/* the document is processed for. Examples of highlighting include underlining,             */
/* displaying in bold face, or using a different character shape (such as italics).         */
/*                                                                                          */
/* A citation may not be used where the GML layout explicitly determines the emphasis to    */
/* be used, such as in the text of a heading.                                               */
/*                                                                                          */
/* The citation tag is a paragraph element. It is used with text to create the content of a */
/* basic document element, such as a paragraph. A corresponding :ecit tag must be           */
/* specified for each :cit tag.                                                             */
/*                                                                                          */
/********************************************************************************************/

void gml_cit( const gmltag *entry )
{
    gml_inline_common( entry, layout_work.cit.font );
}


/************************************************************************************/
/* Format: :eCIT.                                                                   */
/*                                                                                  */
/* This tag ends the highlighting of a citation. A corresponding :cit tag must be   */
/* previously specified for each :ecit tag.                                         */
/*                                                                                  */
/************************************************************************************/

void gml_ecit( const gmltag *entry )
{
    gml_e_inline_common( entry );
}


/***********************************************************************************************/
/* Format: :Q.                                                                                 */
/*                                                                                             */
/* This tag starts a quote. The quote is enclosed in double quotation marks. When quotes       */
/* are specified within other quotes, they are alternately enclosed by single and double       */
/* quotation marks.                                                                            */
/* The quote tag is a paragraph element. It is used with text to create the content of a basic */
/* document element, such as a paragraph. A corresponding :eq tag must be specified for        */
/* each :q tag.                                                                                */
/*                                                                                             */
/* NOTE: Q always uses font 0, even when the value of attribute font of the LAYOUT tag         */
/*       DEFAULT has a different value.                                                        */
/*                                                                                             */
/***********************************************************************************************/

void gml_q( const gmltag *entry )
{
    gml_inline_common( entry, FONT0 );
}


/************************************************************************************/
/* Format: :eQ.                                                                     */
/* This tag signals the end of a quote. A corresponding :q tag must be previously   */
/* specified for each :eq tag.                                                      */
/*                                                                                  */
/* NOTE: eQ does not restore the prior font in wgml 4.0, and so also here.         */
/*                                                                                  */
/************************************************************************************/

void gml_eq( const gmltag *entry )
{
    gml_e_inline_common( entry );
}

