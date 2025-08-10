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

/***************************************************************************/
/*  tagnames as strings for msg display                                    */
/***************************************************************************/
static const char * const str_tags[] = {
    "NONE",
    #define pick1(n,l,r,g,o,c) #n,
    #define pick2(n1,l1,r1,g1,o1,c1,n2,l2,r2,g2,o2,c2) \
                pick1(n1,l1,r1,g1,o1,c1) pick1(n2,l2,r2,g2,o2,c2)
    #include "gtags.h"
    #undef pick2
    #undef pick1
//    #define pick( name, routine, flags )  extern void routine( void );
//    #include "gscrcws.h" TBD
//    #undef pick
    "MAX"
};

static loc_to_name  l2n_names[L2N_ENTRIES] = { { TLOC_address, "EADDRESS" },
                                               { TLOC_figcap, "FIGDESC or EFIG" },
                                               { TLOC_titlep, "ETITLEP" } };

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

static void g_err( msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_ERROR, args );
    va_end( args );
}

/***************************************************************************/
/*  warning message                                                        */
/***************************************************************************/

static void g_warn( msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_WARNING, args );
    va_end( args );
}

/***************************************************************************/
/*  informational message (general)                                        */
/***************************************************************************/

void g_info( msg_ids num, ... )
{
    va_list args;

    va_start( args, num );
    g_msg_var( num, SEV_INFO, args );
    va_end( args );
}

/***************************************************************************/
/*  informational message forced to left margin                            */
/***************************************************************************/

void g_info_lm( msg_ids num, ... )
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

void g_info_research( msg_ids num, ... )
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
NO_RETURN( static void err_exit( void ) );

static void err_exit( void )
{
    if( GlobalFlags.research ) {        // TBD

        print_macro_dict( macro_dict, true );

        if( tags_dict != NULL ) {
            print_tag_dict( tags_dict );
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
    my_exit( 16 );
    /* never return */
}

/***************************************************************************/
/*  display offending text line and mark the offending token               */
/***************************************************************************/

static void show_line_error_len( const char * pa, unsigned len )
{
    char        *buf = NULL;
    unsigned    cnt;

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
    char        linestr[NUM2STR_LENGTH + 1];
    char        linemac[NUM2STR_LENGTH + 1];
    char        linefile[NUM2STR_LENGTH + 1];

    if( input_cbs != NULL ) {
        if( input_cbs->fmflags & II_tag_mac ) {
            sprintf( linestr, "%d", input_cbs->s.m->lineno );
            sprintf( linefile, "%d", input_cbs->s.m->lineno + input_cbs->s.m->mac->lineno );
            sprintf( linemac, "%d", input_cbs->s.m->mac->lineno );
            g_info( ERR_INF_MAC_DEF, linestr, linefile, input_cbs->s.m->mac->name,
                    linemac, input_cbs->s.m->mac->mac_file_name);
        } else {
            sprintf( linestr, "%d", input_cbs->s.f->lineno );
            g_info( INF_FILE_LINE, linestr, input_cbs->s.f->filename );
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
    char            linestr[NUM2STR_LENGTH + 1];
    char            linemac[NUM2STR_LENGTH + 1];
    char            linefile[NUM2STR_LENGTH + 1];
    nest_stack      *nw;

    if( input_cbs != NULL ) {
        if( input_cbs->fmflags & II_tag_mac ) {
            sprintf( linestr, "%d", input_cbs->s.m->lineno );
            sprintf( linefile, "%d", input_cbs->s.m->lineno + input_cbs->s.m->mac->lineno );
            sprintf( linemac, "%d", input_cbs->s.m->mac->lineno );
            g_info( ERR_INF_MAC_DEF, linestr, linefile, input_cbs->s.m->mac->name,
                    linemac, input_cbs->s.m->mac->mac_file_name);
        } else {
            sprintf( linestr, "%d", input_cbs->s.f->lineno );
            g_info( INF_FILE_LINE, linestr, input_cbs->s.f->filename );
        }

        g_info( ERR_TAG_STARTING, str_tags[nest_cb->gtag] );

        nw = nest_cb->p_stack;
        while( nw != NULL ) {
            switch( nw->nest_flag & II_input ) {
            case II_file:
                sprintf( linestr, "%d", nw->lineno );
                g_info( INF_FILE_LINE, linestr, nw->s.filename );
                break;
            case II_tag :
                g_info( ERR_INF_TAG, nw->s.mt.tag_m->tagname );
                // fallthrough
            case II_macro :
                sprintf( linestr, "%d", nw->lineno );
                sprintf( linefile, "%d", nw->lineno + nw->s.mt.m->lineno );
                sprintf( linemac, "%d", nw->s.mt.m->lineno );
                g_info( ERR_INF_MAC_DEF, linestr, linefile, nw->s.mt.m->name,
                        linemac, nw->s.mt.m->mac_file_name);
                break;
            default:
                g_info( ERR_INC_UNKNOWN );
                break;
            }
            nw = nw->prev;
            out_msg( "\n" );
        }
    }
    return;
}

/* specialized error functions */

void att_req_err_exit( const char *tagname, const char *attname )  // for process_tag()
{
    char        one_name[12];   // attname length max plus 2 "'" plus string terminator
    uint8_t     len;

    const char    *   p;
    const char    *   pa;

    err_count++;
    g_err( ERR_ATT_REQ, tagname, attname );
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
        g_info( INFO_ATT_REQ_NAME, one_name );
        SkipSpaces( p );    // start of next attribute name
    }
    g_info( INFO_ATT_REQ );
    file_mac_info();
    err_exit();
    /* never return */
}

void ban_reg_err_exit( msg_ids num, banner_lay_tag * in_ban1, banner_lay_tag * in_ban2,
                  region_lay_tag * in_reg1, region_lay_tag * in_reg2 )
// various vertical and horizontal spacing errors of banners and regions
// for finish_banners()
{
    if( in_ban1 != NULL ) {
        g_err( INF_BAN_ID, doc_sections[in_ban1->docsect].name, ban_places[in_ban1->place] );
        if( in_ban2 != NULL ) {
            g_info( INF_BAN_ID, doc_sections[in_ban2->docsect].name, ban_places[in_ban2->place] );
        }
    } else if( in_ban2 != NULL ) {
        g_err( INF_BAN_ID, doc_sections[in_ban2->docsect].name, ban_places[in_ban2->place] );
    } else {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( in_reg1 != NULL ) {
        g_info( INF_REG_ID, in_reg1->hoffset.su_txt, in_reg1->voffset.su_txt,
                in_reg1->indent.su_txt );
    }

    if( in_reg2 != NULL ) {
        g_info( INF_REG_ID, in_reg2->hoffset.su_txt, in_reg2->voffset.su_txt,
                in_reg2->indent.su_txt );
    }

    g_info( num );
    file_mac_info();
    err_exit();
    /* never return */
}

void internal_err_exit( const char * file, int line )    // utility function
{
    err_count++;
    g_err( ERR_INTERN, file, line );
    err_exit();
    /* never return */
}

void list_level_err_exit( const char * xl_tag, uint8_t xl_level )    // for finish_lists()
{
    err_count++;
    g_err( ERR_LEVEL_SKIPPED, xl_tag );
    g_info( INFO_LEVEL_SKIPPED, xl_level );
    file_mac_info();
    err_exit();
    /* never return */
}

void main_file_err_exit( const char * filename )
{
    g_err( ERR_INPUT_FILE_NOT_FOUND, filename );
    err_count++;
    if( inc_level > 0 ) {
        show_include_stack();
    } else {                // master file included from cmdline
        g_info( INF_INCLUDED, "cmdline" );
    }
    err_exit();
    /* never return */
}

void numb_err_exit( void )                                           // for scr_pu()
{
    char    linestr[NUM2STR_LENGTH + 1];

    err_count++;
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_err( ERR_PU_NUM, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_err( ERR_PU_NUM, linestr, "file", input_cbs->s.f->filename );
    }
    show_include_stack();
    err_exit();
    /* never return */
}

void symbol_name_length_err_exit( const char * symname )
{
    char    linestr[NUM2STR_LENGTH + 1];

    err_count++;
    g_err( ERR_SYM_LONG, symname );
    g_info( INF_SYM_10 );
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_info( INF_MAC_LINE, linestr, input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_info( INF_FILE_LINE, linestr, input_cbs->s.f->filename );
    }
    file_mac_info();
    err_exit();
    /* never return */
}

void val_parse_err_exit( const char * pa, bool tag ) // for internal_to_su()
{
    err_count++;
    if( tag ) {
        g_err( ERR_INV_ATT_VAL );
    } else {
        g_err( ERR_INV_CW_OP_VAL );
    }
    file_mac_info();
    show_line_error( pa );
    err_exit();
    /* never return */
}


/***************************************************************************/
/*  message for duplicate figure, footnote, or heading ids                 */
/***************************************************************************/

void dup_refid_err_exit( const char *refid, const char * context )
{
    g_err( WNG_ID_XXX, refid );
    g_info( INF_ID_DUPLICATE, context );
    file_mac_info();
    err_count++;
    err_exit();
    /* never return */
}

/***************************************************************************/
/*  error msgs for missing or duplicate :XXX :eXXX tags                    */
/***************************************************************************/

static void g_tag_common_err( g_tags gtag, bool nest )
{
    g_err( ERR_TAG_EXPECTED, str_tags[gtag] );
    if( nest ) {
        file_mac_info_nest();
    } else {
        file_mac_info();
    }
    err_count++;
    return;
}

void g_tag_err_exit( g_tags gtag )
{
    g_tag_common_err( gtag, false );         // 'normal' stack display
    err_exit();
    /* never return */
}

void g_tag_nest_err_exit( g_tags gtag )
{
    g_tag_common_err( get_tclo( gtag ), true );         // nested tag stack display
    err_exit();
    /* never return */
}

/* Various special-purpose functions */

void g_if_int_err_exit( void )
{
    char    linestr[NUM2STR_LENGTH + 1];

    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_err( ERR_IF_INTERN, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_err( ERR_IF_INTERN, linestr, "file", input_cbs->s.f->filename );
    }
    if( inc_level > 1 ) {
        show_include_stack();
    }
    err_count++;
    err_exit();
    /* never return */
}

/* SC--037: The macro 'xxxxxx' for the gml tag 'yyyyy' is not defined */

void g_tag_mac_err_exit( const gtentry *ge )
{
    char    linestr[NUM2STR_LENGTH + 1];

    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_err( ERR_TAG_MACRO, ge->macname, ge->tagname, linestr, "macro", input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_err( ERR_TAG_MACRO, ge->macname, ge->tagname, linestr, "file", input_cbs->s.f->filename );
    }
    if( inc_level > 0 ) {
        show_include_stack();
    }
    err_count++;
    err_exit();
    /* never return */
}

void g_tag_no_err_exit( g_tags gtag )
{
    char    tagn[TAG_NAME_LENGTH + 1 + 1];

    sprintf( tagn, "%c%s", GML_char, str_tags[gtag] );
    g_err( ERR_TAG_NOT_EXPECTED, tagn );
    file_mac_info_nest();
    err_count++;
    err_exit();
    /* never return */
}

void check_close_tag_err_exit( g_tags gtag )
{
    if( nest_cb->gtag != get_topn( gtag ) ) {       // unexpected exxx tag
        if( nest_cb->gtag == T_NONE ) {
            g_tag_no_err_exit( gtag );              // no exxx expected
        } else {
            g_tag_nest_err_exit( nest_cb->gtag );   // exxx expected
        }
        /* never return */
    }
}

void g_tag_prec_err_exit( g_tags gtag )
{
    char    tagn[TAG_NAME_LENGTH + 1 + 1];

    sprintf( tagn, "%c%s", GML_char, str_tags[gtag] );
    g_err( ERR_TAG_PRECEDING, tagn );
    file_mac_info();
    err_count++;
    err_exit();
    /* never return */
}

void g_tag_rsloc_err_exit( locflags inloc, const char * pa )
{
    const char  *tag_name;
    int         i;

    tag_name = "unknown";
    for( i = 0; i < L2N_ENTRIES; i++ ) {
        if( l2n_names[i].location == inloc ) {
            tag_name = l2n_names[i].tagname;
            break;
        }
    }
    g_err( ERR_TAG_EXPECTED, tag_name );
    file_mac_info_nest();
    err_count++;

    show_line_error( pa );

    err_exit();
    /* never return */
}

void g_wng_hlevel( hdlvl hd_found, hdlvl hd_expected )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( WNG_HEADING_LEVEL );
        g_info( INF_HEADING_LEVEL, hd_nums[hd_found].tag, hd_nums[hd_expected].tag );
        file_mac_info();
    }
    return;
}

void keep_nest_err_exit( const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( ERR_CW_TAG_X_IN_Y, arg1, arg2 );
    g_info( INF_NESTED_BLOCKS1 );
    g_info( INF_NESTED_BLOCKS2 );
    file_mac_info();
    err_exit();
    /* never return */
}

/* These are generic helper functions */

void xx_err_exit( msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    err_exit();
    /* never return */
}

void xx_err_exit_c( msg_ids errid, char const * arg )
{
    err_count++;
    g_err( errid, arg );
    file_mac_info();
    err_exit();
    /* never return */
}

void xx_err_exit_cc( msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    file_mac_info();
    err_exit();
    /* never return */
}

void xx_line_err_exit_c( msg_ids errid, const char * pa )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error( pa );
    err_exit();
    /* never return */
}

void xx_line_err_exit_ci( msg_ids errid, const char * pa, unsigned len )
{
    err_count++;
    g_err( errid );
    file_mac_info();
    show_line_error_len( pa, len );
    err_exit();
    /* never return */
}

void xx_line_err_exit_cc( msg_ids errid, char const * cw, const char * pa )
{
    err_count++;
    g_err( errid, cw );
    file_mac_info();
    show_line_error( pa );
    err_exit();
    /* never return */
}

void xx_line_err_exit_cci( msg_ids errid, char const * cw, char const * pa, unsigned len )
{
    err_count++;
    g_err( errid, cw, pa );
    file_mac_info();
    show_line_error_len( pa, len );
    err_exit();
    /* never return */
}

void xx_nest_err_exit( msg_ids errid )
{
    err_count++;
    g_err( errid );
    file_mac_info_nest();
    err_exit();
    /* never return */
}

void xx_nest_err_exit_cc( msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    file_mac_info_nest();
    err_exit();
    /* never return */
}

void xx_simple_err_exit( msg_ids errid )
{
    err_count++;
    g_err( errid );
    err_exit();
    /* never return */
}

void xx_simple_err_exit_c( msg_ids errid, const char * arg )
{
    err_count++;
    g_err( errid, arg );
    err_exit();
    /* never return */
}

void xx_simple_err_exit_i( msg_ids errid, int arg )
{
    err_count++;
    g_err( errid, arg );
    err_exit();
    /* never return */
}

void xx_simple_err_exit_cc( msg_ids errid, const char * arg1, const char * arg2 )
{
    err_count++;
    g_err( errid, arg1, arg2 );
    err_exit();
    /* never return */
}

void xx_source_err_exit( msg_ids errid )
{
    char    linestr[NUM2STR_LENGTH + 1];

    g_err( errid );
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_info( INF_MAC_LINE, linestr, input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_info( INF_FILE_LINE, linestr, input_cbs->s.f->filename );
    }
    show_include_stack();
    err_count++;
    err_exit();
    /* never return */
}

void xx_source_err_exit_c( msg_ids errid, const char * arg )
{
    char    linestr[NUM2STR_LENGTH + 1];

    g_err( errid, arg );
    if( input_cbs->fmflags & II_tag_mac ) {
        sprintf( linestr, "%d", input_cbs->s.m->lineno );
        g_info( INF_MAC_LINE, linestr, input_cbs->s.m->mac->name );
    } else {
        sprintf( linestr, "%d", input_cbs->s.f->lineno );
        g_info( INF_FILE_LINE, linestr, input_cbs->s.f->filename );
    }
    show_include_stack();
    err_count++;
    err_exit();
    /* never return */
}

void xx_warn( msg_ids errid )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid );
        file_mac_info();
    }
    return;
}

void xx_warn_c( msg_ids errid, const char * arg )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg );
        file_mac_info();
    }
    return;
}

void xx_warn_c_info( msg_ids errid, const char * arg, msg_ids warnid )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg );
        g_info( warnid );
        file_mac_info();
    }
    return;
}

void xx_warn_c_info_c( msg_ids errid, const char * arg1, msg_ids warnid,
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

void xx_warn_cc( msg_ids errid, const char * arg1, const char * arg2 )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg1, arg2 );
        file_mac_info();
    }
    return;
}

void xx_warn_info_cc( msg_ids errid, msg_ids warnid, const char * arg1,
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

void xx_line_warn_c( msg_ids errid, const char * pa )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid );
        file_mac_info();
        show_line_error( pa );
    }
    return;
}

void xx_line_warn_cc( msg_ids errid, const char * cw, const char * pa )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, cw );
        file_mac_info();
        show_line_error( pa );
    }
    return;
}

void xx_simple_warn( msg_ids errid )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid );
    }
    return;
}

void xx_simple_warn_info_cc( msg_ids errid, const char * arg1, msg_ids warnid,
                             const char * arg2 )
{
    wng_count++;
    if( GlobalFlags.warning ) {
        g_warn( errid, arg1 );
        g_info( warnid, arg2 );
    }
    return;
}


