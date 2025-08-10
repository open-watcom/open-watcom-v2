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
* Description:  WGML tags :HDREF :FIGREF :FNREF processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


static  bool    ref_page        = false;
static  bool    page_found      = false;
static  bool    refid_found     = false;
static  char    refid[REFID_LEN + 1];

/***************************************************************************/
/* Get attribute values for FIGREF, FNREF, and HDREF                       */
/***************************************************************************/

static char * get_ref_attributes( void )
{
    char            *p;
    char            *pa;
    att_name_type   attr_name;
    att_val_type    attr_val;

    g_scan_err = false;
    p = g_scandata.s;

    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "page", attr_name.attname.t ) == 0 ) {
                page_found = true;
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( strcmp( "yes", attr_val.specval ) == 0 ) {
                    ref_page = true;
                } else if( strcmp( "no", attr_val.specval ) == 0 ) {
                    ref_page = false;
                } else {
                    xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val.tok.s );
                    /* never return */
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "refid", attr_name.attname.t ) == 0 ) {
                p = get_refid_value( p, &attr_val, refid );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                refid_found = true;
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {    // no match = end-of-tag in wgml 4.0
                p = pa; // restore any spaces before non-attribute value
                break;
            }
        }
    }
    if( !refid_found ) {            // detect missing required attribute
        xx_err_exit( ERR_ATT_MISSING );
        /* never return */
    }
    return( p );
}

/***************************************************************************/
/*      :FIGREF refid='id-name'                                            */
/*              [page=yes                                                  */
/*                    no].                                                 */
/* This tag causes a figure reference to be generated. The text "Figure"   */
/* followed by the figure number will be generated at the point where the  */
/* :figref tag is specified. The figure reference tag is a paragraph       */
/* element, and is used with text to create the content of a basic         */
/* document element. The figure being referenced must have a figure        */
/* caption specified.                                                      */
/***************************************************************************/

void gml_figref( const gmltag * entry )
{
    bool            do_page     =   false;  // default for fwd refs w/no "page" attribute
    char            buffer[NUM2STR_LENGTH + 1];
    char        *   p;
    char        *   ref_text;
    ref_entry   *   cur_re;
    unsigned        bu_len;
    unsigned        len;

    static char     def_page[]  = " on page XXX";
    static char     def_ref[]   = "Figure XX";
    static char     on_page[]  = " on page ";
    static unsigned dp_len;
    static unsigned dr_len;
    static unsigned op_len;

    (void)entry;

    p = get_ref_attributes();

    cur_re = find_refid( fig_ref_dict, refid );

    if( page_found ) {
        do_page = ref_page;
        page_found = false;
        ref_page = false;
    } else if( cur_re != NULL ) {
        do_page = ((g_page + 1) != cur_re->u.ffh.entry->pageno);
    }

    dp_len = strlen( def_page );
    dr_len = strlen( def_ref );
    op_len = strlen( on_page );
    if( cur_re == NULL ) {              // undefined refid
        if( do_page ) {
            ref_text = (char *)mem_alloc( dr_len + dp_len + 1 );
            strcpy( ref_text, def_ref );
            strcat( ref_text, def_page );
        } else {
            ref_text = (char *)mem_alloc( dr_len + 1 );
            strcpy( ref_text, def_ref );
        }
    } else {
        len = strlen( cur_re->u.ffh.entry->prefix );
        if( do_page ) {
            format_num( cur_re->u.ffh.entry->pageno, buffer, sizeof( buffer ),
                        cur_re->u.ffh.entry->style );
            bu_len = strlen( buffer );
            ref_text = (char *)mem_alloc( len + op_len + bu_len  + 1 );
            strcpy( ref_text, cur_re->u.ffh.entry->prefix );
            ref_text[len - 1] = '\0';       // remove delim
            len += (dr_len + op_len + bu_len );
            strcat( ref_text, on_page );
            strcat( ref_text, buffer );
        } else {
            ref_text = (char *)mem_alloc( len + 1 );
            strcpy( ref_text, cur_re->u.ffh.entry->prefix );
            ref_text[len - 1] = '\0';       // remove delim
        }
    }
    process_text( ref_text, g_curr_font );
    mem_free( ref_text );
    ref_text = NULL;

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {                  // only if text follows
            post_space = 0;                 // cancel space after ref_text
            process_text( p, g_curr_font );
        }
    }

    if( GlobalFlags.lastpass ) {
        if( cur_re == NULL ) {
            if( passes == 1 ) {
                fig_fwd_refs = init_fwd_ref( fig_fwd_refs, refid );
            } else {
                xx_warn_c_info_c( WNG_ID_XXX, refid, INF_ID_UNKNOWN, "figure" );
            }
        }
    }

    g_scandata.s = g_scandata.e;

    return;
}

/***************************************************************************/
/*         :HDREF refid='id-name'                                          */
/*                [page=yes                                                */
/*                      no].                                               */
/*                                                                         */
/* This tag causes a heading reference to be generated.  The heading       */
/* reference tag is a paragraph element, and is used with text to create   */
/* the content of a basic document element.  The heading text from the     */
/* referenced heading is enclosed in double quotation marks and inserted   */
/* into the formatted document.                                            */
/*                                                                         */
/* Forward references always produce:                                      */
/*     "Undefined Heading" on page XXX                                     */
/* unless attribute page is explicitly given the value "no"                */
/***************************************************************************/

void gml_hdref( const gmltag * entry )
{
    bool            do_page     =   false;
    char            buffer[NUM2STR_LENGTH + 1];
    char        *   p;
    char        *   ref_text;
    ref_entry   *   cur_re;
    unsigned        bu_len;
    unsigned        len;
    unsigned        t_depth;

    static char     def_page[]  = " on page XXX";
    static char     def_ref[]   = "\"Undefined Heading\"";
    static char     on_page[]   = " on page ";
    static unsigned dp_len;
    static unsigned dr_len;
    static unsigned op_len;

    (void)entry;

    p = get_ref_attributes();

    cur_re = find_refid( hd_ref_dict, refid );

    if( page_found ) {
        do_page = ref_page;
        page_found = false;
        ref_page = false;
    } else if( cur_re != NULL ) {
        t_depth = t_page.cur_depth;
        if( t_element == NULL ) {
            t_depth += g_subs_skip;
        } else {
            t_depth += t_element->blank_lines + t_element->subs_skip + t_element->depth;
        }
        if( t_line != NULL ) {
            t_depth += t_line->line_height;
        } else {
            t_depth += wgml_fonts[g_curr_font].line_height;
        }
        if( t_depth > t_page.max_depth ) {
            do_page = ((g_page + 2 ) != cur_re->u.ffh.entry->pageno);
        } else {
            do_page = ((g_page + 1 ) != cur_re->u.ffh.entry->pageno);
        }
    } else {
        do_page = true;
    }

    dp_len = strlen( def_page );
    dr_len = strlen( def_ref );
    op_len = strlen( on_page );
    if( cur_re == NULL ) {              // undefined refid
        ref_text = (char *)mem_alloc( dp_len + dr_len + 1 );
        strcpy( ref_text, def_ref );
        if( do_page ) {
          strcat( ref_text, def_page );
        }
    } else {
        len = strlen( cur_re->u.ffh.entry->text ) + 2;        // allow for quote chars
        if( do_page ) {
            format_num( cur_re->u.ffh.entry->pageno, buffer, sizeof( buffer ),
                        cur_re->u.ffh.entry->style );
            bu_len = strlen( buffer );
            len += (op_len + bu_len );
            ref_text = (char *)mem_alloc( len + 1 );
            strcpy( ref_text, "\"" );
            strcat( ref_text, cur_re->u.ffh.entry->text );
            strcat( ref_text, "\"" );
            strcat( ref_text, on_page );
            strcat( ref_text, buffer );
        } else {
            ref_text = (char *)mem_alloc( len + 1 );
            strcpy( ref_text, "\"" );
            strcat( ref_text, cur_re->u.ffh.entry->text );
            strcat( ref_text, "\"" );
        }
    }

    if( ProcFlags.dd_macro ) {      // special processing for DT/DD in macro followed by HDREF
        add_dt_space();
        ProcFlags.dd_macro = false;
    }

    if( post_space == 0 ) {
        post_space = wgml_fonts[g_curr_font].spc_width;
        if( !ProcFlags.dd_space && !ProcFlags.as_text_line &&
                is_stop_char( t_line->last->text[t_line->last->count - 1] ) ) {
            post_space += wgml_fonts[g_curr_font].spc_width;
        }
    }

    process_text( ref_text, g_curr_font );
    mem_free( ref_text );
    ref_text = NULL;

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {                  // only if text follows
            ProcFlags.ct = true;
            post_space = 0;                 // cancel space after ref_text
            process_text( p, g_curr_font );
        }
    }

    if( GlobalFlags.lastpass ) {
        if( cur_re == NULL ) {
            if( passes == 1 ) {
                hd_fwd_refs = init_fwd_ref( hd_fwd_refs, refid );
            } else {
                xx_warn_c_info_c( WNG_ID_XXX, refid, INF_ID_UNKNOWN, "heading" );
            }
        }
    }

    g_scandata.s = g_scandata.e;

    return;
}

/***************************************************************************/
/*      :FNREF refid='id-name'                                             */
/* This tag causes a footnote reference to be generated. The number of the */
/* referenced footnote will be generated at the point where the :fnref tag */
/* is specified. The footnote reference tag is a paragraph element, and is */
/* used with text to create the content of a basic document element.       */
/***************************************************************************/

void gml_fnref( const gmltag * entry )
{
    char            buffer[NUM2STR_LENGTH + 1];
    char        *   p;
    ref_entry   *   cur_re;

    (void)entry;

    p = get_ref_attributes();

    /***********************************************************************/
    /* wgml 4.0: FNREF changes the font to "0" and does not change it back */
    /* reactivate the next line and change the font used with the first    */
    /* process_text() to g_curr_font to restore this behavior              */
    /* our wgml does something a bit more sensible                         */
    /***********************************************************************/

//    g_curr_font = FONT0;                // layout attribute "font" is ignored

    cur_re = find_refid( fn_ref_dict, refid );
    if( cur_re == NULL ) {              // undefined refid
        process_text( "(XX)", g_curr_font );
    } else {
        format_num( cur_re->u.ffh.entry->number, buffer, sizeof( buffer ),
                    layout_work.fnref.number_style );
        process_text( buffer, layout_work.fnref.font );
    }

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {                  // only if text follows
            post_space = 0;                 // cancel space after ref_text
            process_text( p, g_curr_font );
        }
    }

    if( GlobalFlags.lastpass ) {
        if( cur_re == NULL ) {
            if( passes == 1 ) {
                fn_fwd_refs = init_fwd_ref( fn_fwd_refs, refid );
            } else {
                xx_warn_c_info_c( WNG_ID_XXX, refid, INF_ID_UNKNOWN, "footnote" );
            }
        }
    }

    g_scandata.s = g_scandata.e;
    return;
}


