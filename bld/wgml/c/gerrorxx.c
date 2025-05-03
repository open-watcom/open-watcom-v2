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
* Description:  wgml misc error messages
*
****************************************************************************/


#include "wgml.h"

typedef struct {
    locflags    location;
    char        tagname[TAG_NAME_LENGTH + 1];
} loc_to_name;

#define L2N_ENTRIES 3
static loc_to_name  l2n_names[L2N_ENTRIES] = { { address_tag, "EADDRESS" },
                                               { figcap_tag, "FIGDESC or EFIG" },
                                               { titlep_tag, "ETITLEP" } };

/***************************************************************************/
/*  display offending text line and mark the offending token               */
/***************************************************************************/

static void show_line_error_len( const char * pa, size_t len )
{
    char    *buf = NULL;
    size_t  cnt;

    msg_indent = 0;
    cnt = pa - buff2 + len;
    buf = mem_tokdup( buff2, cnt );
    out_msg( "%s\n", buf );
    // number of characters before the offending input + "*" at start of offending input
    if( pa > buff2 ) {
        cnt = pa - buff2;
    } else {
        cnt = 0;
    }
    memset( buf, ' ', cnt );
    buf[cnt] = '*';         // puts "*" after last memset position; no, really
    buf[cnt + 1] = '\0';
    out_msg( "%s\n", buf );
    mem_free( buf );
    out_msg( "\n" );
}

static void show_line_error( const char * pa )
{
    show_line_error_len( pa, strlen( pa ) );
}

/***************************************************************************/
/*  display lineno of file/macro and include stack                         */
/***************************************************************************/

void file_mac_info( void )
{
    char        linestr[NUM2STR_LENGTH];
    char        linemac[NUM2STR_LENGTH];

    if( input_cbs != NULL ) {
        if( input_cbs->fmflags & II_tag_mac ) {
            sprintf( linestr, "%d", input_cbs->s.m->lineno );
            sprintf( linemac, "%d", input_cbs->s.m->mac->lineno );
            g_info( err_inf_mac_def, linestr, input_cbs->s.m->mac->name,
                    linemac, input_cbs->s.m->mac->mac_file_name);
        } else {
            sprintf( linestr, "%d", input_cbs->s.f->lineno );
            g_info( inf_file_line, linestr, input_cbs->s.f->filename );
        }
    }
    show_include_stack();
    return;
}

/***************************************************************************/
/*  display lineno of file/macro for open nested tags :sl :ol, ...         */
/*                             and hilighting tags :HPx, :SF, ...          */
/*   used if the corresponding end tag is missing                          */
/***************************************************************************/

void file_mac_info_nest( void )
{
    char            linestr[NUM2STR_LENGTH];
    char            linemac[NUM2STR_LENGTH];
    nest_stack  *   nw;

    if( input_cbs != NULL ) {
        if( input_cbs->fmflags & II_tag_mac ) {
            sprintf( linestr, "%d", input_cbs->s.m->lineno );
            sprintf( linemac, "%d", input_cbs->s.m->mac->lineno );
            g_info( err_inf_mac_def, linestr, input_cbs->s.m->mac->name,
                    linemac, input_cbs->s.m->mac->mac_file_name);
        } else {
            sprintf( linestr, "%d", input_cbs->s.f->lineno );
            g_info( inf_file_line, linestr, input_cbs->s.f->filename );
        }

        g_info( err_tag_starting, str_tags[nest_cb->c_tag] );

        nw = nest_cb->p_stack;
        while( nw != NULL ) {
            switch( nw->nest_flag & II_input ) {
            case    II_file:
                sprintf( linestr, "%d", nw->lineno );
                g_info( inf_file_line, linestr, nw->s.filename );
                break;
            case    II_tag :
                g_info( err_inf_tag, nw->s.mt.tag_m->name );
                // fallthrough
            case    II_macro :
                sprintf( linestr, "%d", nw->lineno );
                sprintf( linemac, "%d", nw->s.mt.m->lineno );
                g_info( err_inf_mac_def, linestr, nw->s.mt.m->name,
                        linemac, nw->s.mt.m->mac_file_name);
                break;
            default:
                g_info( err_inc_unknown );
                break;
            }
            nw = nw->prev;
            out_msg( "\n" );
        }
    }
    return;
}

void internal_err( const char * file, int line )
{
    err_count++;
    g_err( err_intern, file, line );
    return;
}

void list_level_err( const char * xl_tag, uint8_t xl_level )
{
    err_count++;
    g_err( err_level_skipped, xl_tag );
    g_info( info_level_skipped, xl_level );
    file_mac_info();
    return;
}

void att_val_err( const char *attname )
{
//****ERROR**** SC--045: Value 'xxx' for the 'yyy' attribute is not defined
    err_count++;
    g_err( err_att_val, token_buf, attname );
    file_mac_info();
    return;
}

void auto_att_err( void )
{
//****ERROR**** SC--041: Cannot specify the automatic attribute 'xxx'
    err_count++;
    g_err( err_auto_att, token_buf );
    file_mac_info();
    return;
}

void ban_reg_err( msg_ids num, banner_lay_tag * in_ban1, banner_lay_tag * in_ban2,
                  region_lay_tag * in_reg1, region_lay_tag * in_reg2 )
// various vertical and horizontal spacing errors of banners and regions
{
    if( in_ban1 != NULL ) {
        g_err( inf_ban_id, doc_sections[in_ban1->docsect].name, bf_places[in_ban1->place].name );
        if( in_ban2 != NULL ) {
            g_info( inf_ban_id, doc_sections[in_ban2->docsect].name, bf_places[in_ban2->place].name );
        }
    } else if( in_ban2 != NULL ) {
        g_err( inf_ban_id, doc_sections[in_ban2->docsect].name, bf_places[in_ban2->place].name );
    } else {
        internal_err( __FILE__, __LINE__ );
    }

    if( in_reg1 != NULL ) {
        g_info( inf_reg_id, in_reg1->hoffset.su_txt, in_reg1->voffset.su_txt,
                in_reg1->indent.su_txt );
    }

    if( in_reg2 != NULL ) {
        g_info( inf_reg_id, in_reg2->hoffset.su_txt, in_reg2->voffset.su_txt,
                in_reg2->indent.su_txt );
    }

    g_info( num );
    file_mac_info();
    return;
}

void cw_err( void )
{
// SC--006: Unrecognized control word
    err_count++;
    g_err( err_cw_unrecognized, token_buf );
    file_mac_info();
    return;
}

void dc_opt_warn( const char * pa )
{
    wng_count++;
    g_warn( err_dc_opt, pa );
    file_mac_info();
    show_line_error( pa );
    return;
}

void dc_opt_warn_len( const char * pa, size_t len )
{
    wng_count++;
    g_warn( err_dc_opt, pa );
    file_mac_info();
    show_line_error_len( pa, len );
    return;
}

void parm_miss_err( const char * cw, const char * pa )
{
    err_count++;
    g_err( err_parm_missing, cw );
    file_mac_info();
    show_line_error( pa );
    return;
}

void parm_extra_err( const char * cw, const char * pa )
{
    err_count++;
    g_err( err_extra_ignored, cw, pa );
    file_mac_info();
    return;
}

void numb_err( void )
{
    char    linestr[NUM2STR_LENGTH];

    err_count++;
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_err( ERR_PU_NUM, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_err( ERR_PU_NUM, linestr, "file", input_cbs->s.f->filename );
    }
    show_include_stack();
    return;
}

void nottag_err( void )
{
    err_count++;
    g_err( err_user_tag, g_tagname );
    file_mac_info();
    return;
}

void tag_name_missing_err( void )
{
    err_count++;
    g_err( err_missing_name, "" );
    file_mac_info();
    return;
}

void tag_text_err( const char *tagname )
{
//****ERROR**** SC--038: Tag text may not be specified for the 'xxx' tag
    err_count++;
    g_err( err_att_text, tagname );
    file_mac_info();
    return;
}

void tag_text_req_err( const char *tagname )
{
//****ERROR**** SC--039: Tag text must be specified with the 'xxx' tag
    err_count++;
    g_err( err_att_text_req, tagname );
    file_mac_info();
    return;
}

void val_parse_err( const char * pa, bool tag )
{
    err_count++;
    if( tag ) {
        g_err( err_inv_att_val );
    } else {
        g_err( err_inv_cw_op_val );
    }
    file_mac_info();
    show_line_error( pa );
    return;
}

void xx_tag_err( const msg_ids errid, char const * cw )
{
    err_count++;
    g_err( errid, cw );
    file_mac_info();
    return;
}

void xx_val_line_err( const msg_ids errid, char const * cw, const char * pa )
{
    err_count++;
    g_err( errid, cw );
    file_mac_info();
    show_line_error( pa );
    return;
}

void xx_nest_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info_nest();
    return;
}

void xx_nest_err_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    file_mac_info_nest();
    return;
}

void xx_opt_err( const char * cw, const char * pa )
{
    err_count++;
    g_err( err_xx_opt, cw, pa );
    file_mac_info();
    show_line_error( pa );
    return;
}

void xx_opt_err_len( const char * cw, const char * pa, size_t len )
{
    err_count++;
    g_err( err_xx_opt, cw, pa );
    file_mac_info();
    show_line_error_len( pa, len );
    return;
}

void xx_line_err( const msg_ids errid, const char * pa )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error( pa );
    return;
}

void xx_line_err_len( const msg_ids errid, const char * pa, size_t len )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error_len( pa, len );
    return;
}

void xx_simple_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    return;
}

void xx_simple_err_c( const msg_ids errid, const char * arg )
{
    err_count++;
    g_err( errid, arg );
    return;
}

void xx_simple_err_i( const msg_ids errid, int arg )
{
    err_count++;
    g_err( errid, arg );
    return;
}

void xx_simple_err_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    return;
}

void xx_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    return;
}

void xx_err_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    file_mac_info();
    return;
}

void xx_warn( const msg_ids errid )
{
    wng_count++;
    g_warn( errid );
    file_mac_info();
    return;
}

void xx_warn_att( const msg_ids errid, const char * arg )
{
    wng_count++;
    g_warn( errid, arg );
    file_mac_info();
    return;
}

void xx_warn_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    wng_count++;
    g_warn( errid, arg1, arg2 );
    file_mac_info();
    return;
}

void xx_line_warn( const msg_ids errid, const char * pa )
{
    wng_count++;
    g_warn( errid );
    file_mac_info();
    show_line_error( pa );
    return;
}

void xx_val_line_warn( const msg_ids errid, const char * cw, const char * pa )
{
    wng_count++;
    g_warn( errid, cw );
    file_mac_info();
    show_line_error( pa );
    return;
}

void xx_simple_warn( const msg_ids errid )
{
    wng_count++;
    g_warn( errid );
    return;
}

/***************************************************************************/
/*  messages for duplicate, forward, or undefined figure, footnote, or     */
/*  heading ids                                                            */
/***************************************************************************/

void dup_id_err( const char * id, const char * context )
{
    g_err( wng_id_xxx, id );
    g_info( inf_id_duplicate, context );
    file_mac_info();
    err_count++;
}

void fwd_id_warn( const char * id, const char * context )
{
    g_warn( wng_id_xxx, id );
    g_info( inf_id_forward, context );
    wng_count++;
}

void undef_id_warn( const char * id, const char * context )
{
    g_warn( wng_id_xxx, id );
    g_info( inf_id_unknown, context );
    wng_count++;
}

void undef_id_warn_info( const char * id, const char * context )
{
    g_warn( wng_id_xxx, id );
    g_info( inf_id_unknown, context );
    file_mac_info();
    wng_count++;
}

/***************************************************************************/
/*  error msgs for missing or duplicate :XXX :eXXX tags                    */
/***************************************************************************/

static void g_err_tag_common( const char * tag, bool nest )
{
    char    tagn[TAG_NAME_LENGTH + 1];

    sprintf( tagn, "%s", tag );
    g_err( err_tag_expected, tagn );
    if( nest ) {
        file_mac_info_nest();
    } else {
        file_mac_info();
    }
    err_count++;
    return;
}

void g_err_tag( const char * tag )
{
    g_err_tag_common( tag, 0 );         // 'normal' stack display
    return;
}

void g_err_tag_nest( const char * tag )
{
    g_err_tag_common( tag, 1 );         // nested tag stack display
    return;
}

/* Various special-purpose functions */

void g_err_tag_rsloc( locflags inloc, const char * pa )
{
    const char  *   tag_name    = NULL;
    int             i;

    for( i = 0; i < L2N_ENTRIES; i++ ) {
        if( l2n_names[i].location == inloc ) {
            tag_name = l2n_names[i].tagname;
            break;
        }
    }
    if( tag_name == NULL ) {
        tag_name = "unknown";
    }
    g_err_tag_common( tag_name, 1 );
    show_line_error( pa );

    return;
}

void g_err_tag_no( const char * tag )
{
    char    tagn[TAG_NAME_LENGTH + 1 + 1];

    sprintf( tagn, "%c%s", GML_char, tag );
    g_err( err_tag_not_expected, tagn );
    file_mac_info_nest();
    err_count++;
    return;
}

void g_err_tag_prec( const char * tag )
{
    char    tagn[TAG_NAME_LENGTH + 1 + 1];

    sprintf( tagn, "%c%s", GML_char, tag );
    g_err( err_tag_preceding, tagn );
    file_mac_info();
    err_count++;
    return;
}

void g_keep_nest( const char * cw_tag ) {
    switch( cur_group_type ) {
    case gt_fb :
        g_err( err_cw_tag_x_in_y, cw_tag, "a floating block" );
        g_info( inf_nested_blocks1 );
        g_info( inf_nested_blocks2 );
        file_mac_info();
        err_count++;
        break;
    case gt_fig :
        g_err( err_cw_tag_x_in_y, cw_tag, "a figure" );
        g_info( inf_nested_blocks1 );
        g_info( inf_nested_blocks2 );
        file_mac_info();
        err_count++;
        break;
    case gt_fk :
        g_err( err_cw_tag_x_in_y, cw_tag, "a floating keep" );
        g_info( inf_nested_blocks1 );
        g_info( inf_nested_blocks2 );
        file_mac_info();
        err_count++;
        break;
    case gt_fn :
        g_err( err_cw_tag_x_in_y, cw_tag, "a footnote" );
        g_info( inf_nested_blocks1 );
        g_info( inf_nested_blocks2 );
        file_mac_info();
        err_count++;
        break;
    case gt_xmp :
        g_err( err_cw_tag_x_in_y, cw_tag, "an example" );
        g_info( inf_nested_blocks1 );
        g_info( inf_nested_blocks2 );
        file_mac_info();
        err_count++;
    }
}

void g_wng_hlevel( hdsrc hd_found, hdsrc hd_expected )
{
        g_warn( wng_heading_level );
        g_info( inf_heading_level, hd_nums[hd_found].tag, hd_nums[hd_expected].tag );
        file_mac_info();
        wng_count++;
}
