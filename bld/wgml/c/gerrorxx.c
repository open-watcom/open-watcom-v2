/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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

#include "clibext.h"

typedef struct {
    locflags    location;
    char        tagname[TAG_NAME_LENGTH + 1];
} loc_to_name;

#define L2N_ENTRIES 2
static loc_to_name  l2n_names[L2N_ENTRIES] = { { address_tag, "EADDRESS" },
                                                { titlep_tag, "ETITLEP" } };

/***************************************************************************/
/*  display offending text line and mark the offending token               */
/***************************************************************************/

static void show_line_error_len( const char *pa, size_t len )
{
    char    *buf = NULL;
    size_t  cnt;

    msg_indent = 0;
    cnt = pa - buff2 + len;
    buf = mem_alloc( cnt + 1 );
    memcpy( buf, buff2, cnt );
    buf[cnt] = '\0';
    out_msg( "%s\n", buf );
    // number of characters before the offending input + "*" at start of offending input
    cnt = pa - buff2 - 1;
    memset( buf, ' ', cnt );
    buf[cnt] = '*';         // puts "*" after last memset position; no, really
    buf[cnt + 1] = '\0';
    out_msg( "%s\n", buf );
    mem_free( buf );
    out_msg( "\n" );
}


static void show_line_error( const char *pa )
{
    show_line_error_len( pa, strlen( pa ) );
}

/***************************************************************************/
/*  display lineno of file/macro and include stack                         */
/***************************************************************************/

void    file_mac_info( void )
{
    char        linestr[MAX_L_AS_STR];
    char        linemac[MAX_L_AS_STR];

    if( input_cbs != NULL ) {
        if( input_cbs->fmflags & II_tag_mac ) {
            ultoa( input_cbs->s.m->lineno, linestr, 10 );
            ultoa( input_cbs->s.m->mac->lineno, linemac, 10 );
            g_info( err_inf_mac_def, linestr, input_cbs->s.m->mac->name,
                    linemac, input_cbs->s.m->mac->mac_file_name);
        } else {
            ultoa( input_cbs->s.f->lineno, linestr, 10 );
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

void    file_mac_info_nest( void )
{
    char            linestr[MAX_L_AS_STR];
    char            linemac[MAX_L_AS_STR];
    nest_stack  *   nw;

    if( input_cbs != NULL ) {
        if( input_cbs->fmflags & II_tag_mac ) {
            ultoa( input_cbs->s.m->lineno, linestr, 10 );
            ultoa( input_cbs->s.m->mac->lineno, linemac, 10 );
            g_info( err_inf_mac_def, linestr, input_cbs->s.m->mac->name,
                    linemac, input_cbs->s.m->mac->mac_file_name);
        } else {
            ultoa( input_cbs->s.f->lineno, linestr, 10 );
            g_info( inf_file_line, linestr, input_cbs->s.f->filename );
        }

        g_info( err_tag_starting, str_tags[nest_cb->c_tag] );

        nw = nest_cb->p_stack;
        while( nw != NULL ) {
            switch( nw->nest_flag & II_input ) {
            case    II_file:
                ultoa( nw->lineno, linestr, 10 );
                g_info( inf_file_line, linestr, nw->s.filename );
                break;
            case    II_tag :
                g_info( err_inf_tag, nw->s.mt.tag_m->name );
                // fallthrough
            case    II_macro :
                ultoa( nw->lineno, linestr, 10 );
                ultoa( nw->s.mt.m->lineno, linemac, 10 );
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

void    att_val_err( const char * attname )
{
//****ERROR**** SC--045: Value 'xxx' for the 'yyy' attribute is not defined
    err_count++;
    g_err( err_att_val, token_buf, attname );
    file_mac_info();
    return;
}


void    auto_att_err( void )
{
//****ERROR**** SC--041: Cannot specify the automatic attribute 'xxx'
    err_count++;
    g_err( err_auto_att, token_buf );
    file_mac_info();
    return;
}


void    cw_err( void )
{
// SC--006: Unrecognized control word
    err_count++;
    g_err( err_cw_unrecognized, token_buf );
    file_mac_info();
    return;
}


void    dc_opt_warn( const char *pa )
{
    wng_count++;
    g_warn( err_dc_opt, pa );
    file_mac_info();
    show_line_error( pa );
    return;
}


void    dc_opt_warn_len( const char *pa, size_t len )
{
    wng_count++;
    g_warn( err_dc_opt, pa );
    file_mac_info();
    show_line_error_len( pa, len );
    return;
}


void    parm_miss_err( const char *pa )
{
    err_count++;
    g_err( err_parm_missing, pa );
    file_mac_info();
    return;
}


void    parm_extra_err( const char *cw, const char *pa )
{
    err_count++;
    g_err( err_extra_ignored, cw, pa );
    file_mac_info();
    return;
}


void    numb_err( void )
{
    char    linestr[MAX_L_AS_STR];

    err_count++;
    if( input_cbs->fmflags & II_tag_mac ) {
        ultoa( input_cbs->s.m->lineno, linestr, 10 );
        g_err( ERR_PU_NUM, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        ultoa( input_cbs->s.f->lineno, linestr, 10 );
        g_err( ERR_PU_NUM, linestr, "file", input_cbs->s.f->filename );
    }
    show_include_stack();
    return;
}


void    nottag_err( void )
{
    err_count++;
    g_err( err_user_tag, tagname );
    file_mac_info();
    return;
}


void    tag_name_missing_err( void )
{
    err_count++;
    g_err( err_missing_name, "" );
    file_mac_info();
    return;
}


void    tag_text_err( const char *tagname )
{
//****ERROR**** SC--038: Tag text may not be specified for the 'xxx' tag
    err_count++;
    g_err( err_att_text, tagname );
    file_mac_info();
    return;
}


void    tag_text_req_err( const char *tagname )
{
//****ERROR**** SC--039: Tag text must be specified with the 'xxx' tag
    err_count++;
    g_err( err_att_text_req, tagname );
    file_mac_info();
    return;
}


void    val_parse_err( const char *pa, bool tag )
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

void    xx_tag_err( const msg_ids errid, char const * cw )
{
    err_count++;
    g_err( errid, cw );
    file_mac_info();
    return;
}

void    xx_nest_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info_nest();
    return;
}


void    xx_opt_err( const char *cw, const char *pa )
{
    err_count++;
    g_err( err_xx_opt, cw, pa );
    file_mac_info();
    show_line_error( pa );
    return;
}


void    xx_opt_err_len( const char *cw, const char *pa, size_t len )
{
    err_count++;
    g_err( err_xx_opt, cw, pa );
    file_mac_info();
    show_line_error_len( pa, len );
    return;
}


void    xx_line_err( const msg_ids errid, const char *pa )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error( pa );
    return;
}

void    xx_line_err_len( const msg_ids errid, const char *pa, size_t len )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error_len( pa, len );
    return;
}

void    xx_simple_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    return;
}

void    xx_simple_err_c( const msg_ids errid, const char * arg )
{
    err_count++;
    g_err( errid, arg );
    return;
}

void    xx_simple_err_i( const msg_ids errid, int arg )
{
    err_count++;
    g_err( errid, arg );
    return;
}

void    xx_simple_err_ul( const msg_ids errid, unsigned long arg )
{
    err_count++;
    g_err( errid, arg );
    return;
}

void    xx_simple_err_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    return;
}

void    xx_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    return;
}

void    xx_warn( const msg_ids errid )
{
    wng_count++;
    g_warn( errid );
    file_mac_info();
    return;
}

/***************************************************************************/
/*  error msgs for missing or duplicate :XXX :eXXX tags                    */
/***************************************************************************/
static  void    g_err_tag_common( const char *tag, bool nest )
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

void    g_err_tag( const char *tag )
{
    g_err_tag_common( tag, 0 );         // 'normal' stack display
    return;
}

void    g_err_tag_nest( e_tags c_tag )
{
    g_err_tag_common( str_tags[c_tag + 1], 1 );         // nested tag stack display
    return;
}

void    g_err_tag_rsloc( locflags inloc, const char *pa )
{
    const char  *tag_name    = NULL;
    int         i;

    for( i = 0; i < L2N_ENTRIES; i++ ) {
        if( l2n_names[i].location == inloc ) {
            tag_name = l2n_names[i].tagname;
            break;
        }
    }
    if( tag_name == NULL ) {        // should never happen, make internal error?
        tag_name = "unknown";
    }
    g_err_tag_common( tag_name, 1 );
    show_line_error( pa );

    return;
}

void    g_err_tag_no( e_tags c_tag )
{
    char    tagn[TAG_NAME_LENGTH + 1];

    sprintf( tagn, "%c%s", GML_char, str_tags[c_tag + 1] );
    g_err( err_tag_not_expected, tagn );
    file_mac_info_nest();
    err_count++;
    return;
}

void    g_err_tag_prec( const char *tag )
{
    char    tagn[TAG_NAME_LENGTH + 1];

    sprintf( tagn, "%c%s", GML_char, tag );
    g_err( err_tag_preceding, tagn );
    file_mac_info();
    err_count++;
    return;
}

void    g_err_tag_x_in_y( const char *tag1, const char *tag2 )
{
    char    tagn1[TAG_NAME_LENGTH + 1];
    char    tagn2[TAG_NAME_LENGTH + 1];

    sprintf( tagn1, "%c%s", GML_char, tag1 );
    sprintf( tagn2, "%c%s", GML_char, tag2 );
    g_err( err_tag_x_in_y, tagn1, tagn2 );
    file_mac_info();
    err_count++;
    return;
}
