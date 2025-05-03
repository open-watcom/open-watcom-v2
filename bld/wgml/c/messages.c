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
#include <stdarg.h>

#define L2N_ENTRIES     3
#define MAX_ERR_LEN     1020
#define MAX_LINE_LEN    75

static  char    err_buf[MAX_ERR_LEN + 2];   // +2 for \n and \0
static  char    str_buf[MAX_ERR_LEN + 2];

typedef struct {
    locflags    location;
    char        tagname[TAG_NAME_LENGTH + 1];
} loc_to_name;

static loc_to_name  l2n_names[L2N_ENTRIES] = { { address_tag, "EADDRESS" },
                                               { figcap_tag, "FIGDESC or EFIG" },
                                               { titlep_tag, "ETITLEP" } };

/* basic utility function (also used for debugging) */

/***************************************************************************/
/* output message string with a variable number of parameters              */
/***************************************************************************/

void out_msg( const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    vprintf( msg, args );
    va_end( args );
}

/* core message output functions */

/***************************************************************************/
/* actual message output                                                   */
/***************************************************************************/

static void g_msg_var( msg_ids errornum, int sev, va_list arglist )
{
    bool                supp_line = false;
    int                 len;
    const char      *   prefix;
    char            *   save;
    char            *   start;
    char            *   end;

    if( errornum == ERR_DUMMY ) {
        /* dont print anything */
        return;
    }

    switch( sev ) {
#if 0
    case SEV_INFO:
        prefix = "Info:";
        break;
#endif
    case SEV_WARNING:
        prefix = "Warning!";
        msg_indent = 0;
        break;
    case SEV_ERROR:
        prefix = "Error!";
        msg_indent = 0;
        break;
    case SEV_FATAL_ERR:
        prefix = "Fatal Error!";
        msg_indent = 0;
        break;
    default:
        prefix = "";
        supp_line = true;
        break;
    }

    if( errornum == ERR_STR_NOT_FOUND ) {
        /* this message means the error strings cannot be obtained from
         * the exe so its text is hard coded */
        strcpy( err_buf, "Resource strings not found" );
    } else {
        get_msg( errornum, err_buf, sizeof( err_buf ) );
    }
    vsprintf( str_buf, err_buf, arglist );
    len = 0;
    err_buf[0] = '\0';
    if( *prefix != '\0' ) {
        len = sprintf( err_buf, "%s %d: ", prefix, errornum );
        if( len < 0 ) {
            len = 0;
        }
    }
    strcat( err_buf + len, str_buf );

    if( !supp_line ) {    // save points to the ":" or is NULL
        save = strchr( err_buf, ':' );
    }

    start = err_buf;
    if( supp_line ) {
        if( (msg_indent > 0) && (start[0] == '\t') ) {
            start++;    // skip initial tab in favor of msg_indent
        }
        out_msg( "%*s%s\n", msg_indent, "", start );
    } else {
        while( strlen( start ) > MAX_LINE_LEN - msg_indent ) {
            end = start + MAX_LINE_LEN - msg_indent;
            while( !my_isspace( *end ) && end > start )
                end--;
            if( end != start )  {
                *end = '\0';
            } else {
                break;
            }
            out_msg( "%*s%s\n", msg_indent, "", start );
            start = end + 1;
            msg_indent = len;
        }
        out_msg( "%*s%s\n", msg_indent, "", start );
        if( save != NULL ) {    // set msg_indent for follow-on line
            save++;             // step over the ":"
            while( my_isspace( *save ) )    // step over any spaces
                save++;
            msg_indent = save - err_buf;
        }
    }
}

/***************************************************************************/
/*  error message                                                          */
/***************************************************************************/

static void g_err( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_ERROR, args );
    va_end( args );
}

/***************************************************************************/
/*  warning message                                                        */
/***************************************************************************/

static void g_warn( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_WARNING, args );
    va_end( args );
}

/***************************************************************************/
/*  informational message (general)                                        */
/***************************************************************************/

void g_info( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_INFO, args );
    va_end( args );
}

/***************************************************************************/
/*  informational message forced to left margin                            */
/***************************************************************************/

void g_info_lm( const msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    msg_indent = 0;
    g_msg_var( num, SEV_INFO, args );
    va_end( args );
}

/***************************************************************************/
/*  these functions do output that is controlled by GlobalFlags.research   */
/***************************************************************************/

void g_info_research( const msg_ids num, ... )
{
    va_list args;

    if( GlobalFlags.research ) {
        va_start( args, num );
        msg_indent = 0;
        g_msg_var( num, SEV_INFO, args );
        va_end( args );
    }
    return;
}

void out_msg_research( const char *msg, ... )
{
    va_list args;

    if( GlobalFlags.research ) {
        va_start( args, msg );
        vprintf( msg, args );
        va_end( args );
    }
    return;
}

/* extended message support functions (stack, error location in line) */

/***************************************************************************/
/*  handle exit for error message functions                                */
/*  may be used to implement allowing multiple errors to be displayed      */
/*  if that is ever desired                                                */
/***************************************************************************/

static void err_exit( void ) {
    if( GlobalFlags.research ) {        // TBD

        print_macro_dict( macro_dict, true );

        if( tag_dict != NULL ) {
            print_tag_dict( tag_dict );
        }
        print_single_funcs_research();
        print_multi_funcs_research();

        if( global_dict != NULL ) {
            print_sym_dict( global_dict );
        }
        print_sym_dict( sys_dict );
    }
    out_msg( "\nWGML has ended prematurely\n" );
    fflush( NULL );                     // TBD
    if( environment ) {
        longjmp( *environment, 1 );
    }
    my_exit( 16 );
}

/***************************************************************************/
/*  display offending text line and mark the offending token               */
/***************************************************************************/

static void show_line_error_len( const char * pa, size_t len )
{
    char    *buf = NULL;
    size_t  cnt;

    msg_indent = 0;
    if( len == 0 ) {
        len = 1;
    }
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

/* specialized error functions */

void att_req_err( const char *tagname, const char *attname )  // for process_tag()
{
    char        one_name[12];   // attname length max plus 2 "'" plus string terminator
    uint8_t     len;

    const char    *   p;
    const char    *   pa;

    err_count++;
    g_err( err_att_req, tagname, attname );
    p = attname;
    SkipSpaces( p );    // start of first attribute name
    while( *p != '\0' ) {
        pa = p;             // preserve start of attribute name
        while( (*p != ' ') && (*p != '\0' ) ) {
            p++;
        }
        len = p - pa;
        strncpy( one_name, pa, len );
        one_name[len] = '\0';
        g_info( info_att_req_name, one_name );
        SkipSpaces( p );    // start of next attribute name
    }
    g_info( info_att_req );
    file_mac_info();
    err_exit();
}

void ban_reg_err( msg_ids num, banner_lay_tag * in_ban1, banner_lay_tag * in_ban2,
                  region_lay_tag * in_reg1, region_lay_tag * in_reg2 )
// various vertical and horizontal spacing errors of banners and regions
// for finish_banners()
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
    err_exit();
}

void internal_err( const char * file, int line )    // utility function
{
    err_count++;
    g_err( err_intern, file, line );
    err_exit();
}

void list_level_err( const char * xl_tag, uint8_t xl_level )    // for finish_lists()
{
    err_count++;
    g_err( err_level_skipped, xl_tag );
    g_info( info_level_skipped, xl_level );
    file_mac_info();
    err_exit();
}

void main_file_err( const char * filename )
{
    g_err( err_input_file_not_found, filename );
    err_count++;
    if( inc_level > 0 ) {
        show_include_stack();
    } else {                // master file included from cmdline
        g_info( inf_included, "cmdline" );
    }
    err_exit();
}

void numb_err( void )                                           // for scr_pu()
{
    char    linestr[NUM2STR_LENGTH];

    err_count++;
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_err( err_pu_num, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_err( err_pu_num, linestr, "file", input_cbs->s.f->filename );
    }
    show_include_stack();
    err_exit();
}

void symbol_name_length_err( const char * symname )
{
    char    linestr[NUM2STR_LENGTH];

    err_count++;
    g_err( err_sym_long, symname );
    g_info( inf_sym_10 );
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
    }
    file_mac_info();
    err_exit();
}
void val_parse_err( const char * pa, bool tag ) // for internal_to_su()
{
    err_count++;
    if( tag ) {
        g_err( err_inv_att_val );
    } else {
        g_err( err_inv_cw_op_val );
    }
    file_mac_info();
    show_line_error( pa );
    err_exit();
}


/***************************************************************************/
/*  message for duplicate figure, footnote, or heading ids                 */
/***************************************************************************/

void dup_id_err( const char * id, const char * context )
{
    g_err( wng_id_xxx, id );
    g_info( inf_id_duplicate, context );
    file_mac_info();
    err_count++;
    err_exit();
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
    err_exit();
}

void g_err_tag_nest( const char * tag )
{
    g_err_tag_common( tag, 1 );         // nested tag stack display
    err_exit();
}

/* Various special-purpose functions */

void g_err_if_int( void )
{
    char    linestr[NUM2STR_LENGTH];

    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_err( err_if_intern, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_err( err_if_intern, linestr, "file", input_cbs->s.f->filename );
    }
    if( inc_level > 1 ) {
        show_include_stack();
    }
    err_count++;
    err_exit();
}

/* SC--037: The macro 'xxxxxx' for the gml tag 'yyyyy' is not defined */

void g_err_tag_mac( gtentry * ge )
{
    char    linestr[NUM2STR_LENGTH];

    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_err( err_tag_macro, ge->macname, ge->name, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_err( err_tag_macro, ge->macname, ge->name, linestr, "file", input_cbs->s.f->filename );
    }
    if( inc_level > 0 ) {
        show_include_stack();
    }
    err_count++;
    err_exit();
}

void g_err_tag_no( const char * tag )
{
    char    tagn[TAG_NAME_LENGTH + 1 + 1];

    sprintf( tagn, "%c%s", GML_char, tag );
    g_err( err_tag_not_expected, tagn );
    file_mac_info_nest();
    err_count++;
    err_exit();
}

void g_err_tag_prec( const char * tag )
{
    char    tagn[TAG_NAME_LENGTH + 1 + 1];

    sprintf( tagn, "%c%s", GML_char, tag );
    g_err( err_tag_preceding, tagn );
    file_mac_info();
    err_count++;
    err_exit();
}

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

    err_exit();
}

void g_wng_hlevel( hdsrc hd_found, hdsrc hd_expected )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( wng_heading_level );
        g_info( inf_heading_level, hd_nums[hd_found].tag, hd_nums[hd_expected].tag );
        file_mac_info();
    }
    return;
}

void keep_nest_err( const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( err_cw_tag_x_in_y, arg1, arg2 );
    g_info( inf_nested_blocks1 );
    g_info( inf_nested_blocks2 );
    file_mac_info();
    err_exit();
}

/* These are generic helper functions */

void xx_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    err_exit();
}

void xx_err_c( const msg_ids errid, char const * arg )
{
    err_count++;
    g_err( errid, arg );
    file_mac_info();
    err_exit();
}

void xx_err_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    file_mac_info();
    err_exit();
}

void xx_line_err_c( const msg_ids errid, const char * pa )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error( pa );
    err_exit();
}

void xx_line_err_ci( const msg_ids errid, const char * pa, size_t len )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error_len( pa, len );
    err_exit();
}

void xx_line_err_cc( const msg_ids errid, char const * cw, const char * pa )
{
    err_count++;
    g_err( errid, cw );
    file_mac_info();
    show_line_error( pa );
    err_exit();
}

void xx_line_err_cci( const msg_ids errid, char const * cw, char const * pa, size_t len )
{
    err_count++;
    g_err( errid, cw, pa );
    file_mac_info();
    show_line_error_len( pa, len );
    err_exit();
}

void xx_nest_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info_nest();
    err_exit();
}

void xx_nest_err_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    file_mac_info_nest();
    err_exit();
}

void xx_simple_err( const msg_ids errid )
{
    err_count++;
    g_err( errid );
    err_exit();
}

void xx_simple_err_c( const msg_ids errid, const char * arg )
{
    err_count++;
    g_err( errid, arg );
    err_exit();
}

void xx_simple_err_i( const msg_ids errid, int arg )
{
    err_count++;
    g_err( errid, arg );
    err_exit();
}

void xx_simple_err_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    err_exit();
}

void xx_source_err( const msg_ids errid )
{
    char    linestr[NUM2STR_LENGTH];

    g_err( errid );
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
    }
    show_include_stack();
    err_count++;
    err_exit();
}

void xx_source_err_c( const msg_ids errid, const char * arg )
{
    char    linestr[NUM2STR_LENGTH];

    g_err( errid, arg );
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
    }
    show_include_stack();
    err_count++;
    err_exit();
}

void xx_warn( const msg_ids errid )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid );
        file_mac_info();
    }
    return;
}

void xx_warn_c( const msg_ids errid, const char * arg )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg );
        file_mac_info();
    }
    return;
}

void xx_warn_c_info( const msg_ids errid, const char * arg, const msg_ids warnid )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg );
        g_info( warnid );
        file_mac_info();
    }
    return;
}

void xx_warn_c_info_c( const msg_ids errid, const char * arg1, const msg_ids warnid,
                       const char * arg2 )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg1 );
        g_info( warnid, arg2 );
        file_mac_info();
    }
    return;
}

void xx_warn_cc( const msg_ids errid, const char * arg1, const char * arg2 )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg1, arg2 );
        file_mac_info();
    }
    return;
}

void xx_warn_info_cc( const msg_ids errid, const msg_ids warnid, const char * arg1,
                      const char * arg2 )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg1 );
        g_info( warnid, arg2 );
        file_mac_info();
    }
    return;
}

void xx_line_warn_c( const msg_ids errid, const char * pa )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid );
        file_mac_info();
        show_line_error( pa );
    }
    return;
}

void xx_line_warn_cc( const msg_ids errid, const char * cw, const char * pa )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, cw );
        file_mac_info();
        show_line_error( pa );
    }
    return;
}

void xx_simple_warn( const msg_ids errid )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid );
    }
    return;
}

void xx_simple_warn_info_cc( const msg_ids errid, const char * arg1, const msg_ids warnid,
                             const char * arg2 )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg1 );
        g_info( warnid, arg2 );
    }
    return;
}


