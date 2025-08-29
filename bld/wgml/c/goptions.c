/************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  wgml cmdline option processing
*               several options are still ignored                 TBD
****************************************************************************/


#include <fcntl.h>
#include "wgml.h"
#include "wio.h"

#include "clibext.h"


typedef struct option {
    char            *option;            // the option
    unsigned        optionLen;          // length of option
    unsigned        minLength;          // minimum abbreviation
    int             value;              // sometimes value to set option to
    void            (*function)( struct option *optentry );
    unsigned        parmcount;          // expected number of parms
} option;

typedef struct cmd_tok {
    struct cmd_tok  *nxt;
    unsigned        toklen;
    bool            bol;
    char            token[1];           // variable length
} cmd_tok;

typedef struct level_data {
    char            *file_name;
    cmd_tok         *cmd_tokens;
    cmd_tok         *sav_tokens;
} level_data;

static bool         is_option( void );  // used before defined
static level_data   cmd_data[MAX_NESTING];  // option file include level: cmd_data[0] == cmdline
static level_data   *cmd_data_lvl;      // pointer to current level option file data
static char         *opt_parm;
static char         *opt_scan_ptr;
static cmd_tok      *tokennext;
static int          opt_value;

/***************************************************************************/
/*  free storage for tokens at specified include level                     */
/***************************************************************************/

static void free_tokens( void )
{
    cmd_tok         *tok;

    while( (tok = cmd_data_lvl->cmd_tokens) != NULL ) {
        cmd_data_lvl->cmd_tokens = tok->nxt;
        mem_free( tok );
    }
}

/***************************************************************************/
/*  split a line into blank delimited words                                */
/***************************************************************************/

static int split_tokens( char *str )
{
    bool            linestart;
    cmd_tok     *   last_tok;
    cmd_tok     *   new;
    char            quote;
    char        *   tokstart;
    int             cnt;
    unsigned        toklen;

    linestart = true;                   // assume start of line
    cnt = 0;                            // found tokens

    last_tok = cmd_data_lvl->cmd_tokens;    // first token at this level
    if( last_tok != NULL ) {
        while( last_tok->nxt != NULL ) {
            last_tok = last_tok->nxt;   // last token at this level
        }
    }

    for( ;; ) {
        SkipSpacesTabs( str );          // skip blanks / tabs
        if( *str == '\n' ) {
            linestart =  true;          // detect start of line
            str++;
            continue;
        }
        if( *str == '\0' ) {
            break;
        }
        if( *str == '"'
          || *str == '\'' ) {
            quote = *str++;
        } else {
            quote = '\0';
        }
        tokstart = str;
        while( *str != '\0' ) {
            if( quote == '\0'
              && is_space_tab_char( *str )
              || *str == '\n' ) {
                break;
            }
            if( *str == quote ) {
                break;
            }
            str++;
        }
        cnt++;
        toklen = str - tokstart;
        if( quote ) {
            str++;
        }
        if( toklen == 0 ) {
            continue;
        }

        new = mem_alloc( sizeof( *new ) + toklen );
        new->nxt = NULL;
        new->bol = linestart;
        linestart = false;
        new->toklen = toklen;
        strncpy( new->token, tokstart, toklen );
        new->token[toklen] = '\0';

        if( last_tok == NULL ) {
            cmd_data_lvl->cmd_tokens = new;
        } else {
            last_tok->nxt = new;
        }
        last_tok = new;
    }
    return( cnt );
}


static bool CmdScanSwitchChar( char c )
{
#ifdef __UNIX__
    return( c == '-' );
#else
    return( c == '-' || c == '/' );
#endif
}

/***************************************************************************/
/*  Format error in cmdline                                                */
/***************************************************************************/
NO_RETURN( static void bad_cmd_line_err_exit( msg_ids msg, const char *str, char n ) );

static void bad_cmd_line_err_exit( msg_ids msg, const char *str, char n )
{
    char    *   p;
    char    *   pbuff;

    pbuff = mem_alloc( strlen( str ) + 1 );
    p = pbuff;

    for( ; ; ) {
        if( *str == '\0' )
            break;
        if( *str == '\n' )
            break;
        *p++ = *str++;
        if( CmdScanSwitchChar( *str ) )
            break;
        if( *str == n ) {
            break;         // for additional stop char '(' or ' '
        }
    }
    *p = '\0';
    g_banner();
    xx_simple_err_exit_c( msg, pbuff );
    /* never return */
}

/***************************************************************************/
/*  read an option file into memory                                        */
/***************************************************************************/

static void read_indirect_file( FILE *fp )
{
    char        *buf;
    char        ch;
    char        *str;
    unsigned    len;
    unsigned    blk_len;

    buf = mem_alloc( 1024 );
    len = 0;
    while( (blk_len = fread( buf, 1, 1024, fp )) == 1024 ) {
        len += blk_len;
    }
    len += blk_len;
    buf = mem_realloc( buf, len + 1 );
    rewind( fp );
    fread( buf, 1, len, fp );
    buf[len] = '\0';
    // zip through characters changing \r into ' '
    str = buf;
    while( *str ) {
        ch = *str;
        if( ch == '\r' ) {
            *str = ' ';
        } else if( ch == 0x1A ) {   // if end of file
            *str = '\0';            // - mark end of str
            break;
        }
        ++str;
    }
    split_tokens( buf );
    mem_free( buf );
}

/***************************************************************************/
/*  convert string to integer                                              */
/***************************************************************************/
static int get_num_value( const char *p )
{
    int     value;

    value = 0;
    for( ; my_isdigit( *p ); p++ ) {
        value = value * 10 + *p - '0';
    }
    return( value );
}

/***************************************************************************/
/*  ignore option consuming option parms if neccessary                     */
/*  Note: if any other options that don't need implementations are found   */
/*        beyond "wscript", using a different handler might be more        */
/*        efficient than checking for each in turn                         */
/***************************************************************************/

static void ign_option( option * opt )
{
    if( strcmp( "wscript", opt->option ) != 0 ) {   // ignore wscript without msg
        xx_warn_c( WNG_IGN_OPTION, opt->option );
    }
    if( opt->parmcount > 0 ) {
        int     k;

        for( k = 0; k < opt->parmcount; k++ ) {
            if( tokennext == NULL )
                break;
            if( tokennext->bol )
                break;
            if( tokennext->token[0] == '(' )
                break;
            if( is_option() )
                break;
            tokennext = tokennext->nxt;
        }
    }
    return;
}

/***************************************************************************/
/*  warning about using (no)script option                                  */
/***************************************************************************/

static void wng_option( option * opt )
{
    xx_warn_c( WNG_WNG_OPTION, opt->option );
    if( opt->parmcount > 0 ) {
        int     k;

        for( k = 0; k < opt->parmcount; k++ ) {
            if( tokennext == NULL )
                break;
            if( tokennext->bol )
                break;
            if( tokennext->token[0] == '(' )
                break;
            if( is_option() )
                break;
            tokennext = tokennext->nxt;
        }
    }
    return;
}


/***************************************************************************/
/*  ( altext xxx    set alternate extension                                */
/***************************************************************************/

static void set_altext( option * opt )
{
    char    *   pw;
    char    *   p;
    unsigned    len;

    if( tokennext == NULL
      || tokennext->bol
      || is_option() ) {
        bad_cmd_line_err_exit( ERR_MISSING_OPT_VALUE, opt->option, ' ' );
        /* never return */
    }
    len = tokennext->toklen;
    p = tokennext->token;
    if( alt_ext ) {
        mem_free( alt_ext );
    }
    alt_ext = mem_alloc( len + 2 );
    pw = alt_ext;
    if( *p != '.' ) {
        *pw++ = '.';                // make extension start with .
    }
    while( len > 0 ) {
         len--;
         *pw++ = *p++;
    }
    *pw = '\0';
    tokennext = tokennext->nxt;
}

/***************************************************************************/
/*  ( Bind odd [even]   1 or 2 Horizontal Space values                     */
/***************************************************************************/

static void set_bind( option * opt )
{
    bool            scanerr;
    su              bindwork;
    att_val_type    attr_val;

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_cc( ERR_MISS_INV_OPT_VALUE, opt->option, "" );
        /* never return */
    }
    attr_val.quoted = ' ';
    attr_val.tok.s = tokennext->token;
    attr_val.tok.l = tokennext->toklen;
    scanerr = att_val_to_su( &bindwork, true, &attr_val, false ); // must be positive TBD
    if( scanerr ) {
        xx_simple_err_exit_cc( ERR_MISS_INV_OPT_VALUE, opt->option, tokennext->token );
        /* never return */
    }
    memcpy( &bind_odd, &bindwork, sizeof( bind_odd) );

    out_msg_research( "\tbind odd  value %ii (%imm) '%s' %i %i \n",
             bind_odd.su_inch, bind_odd.su_mm, bind_odd.su_txt,
             bind_odd.su_whole, bind_odd.su_dec );

    tokennext = tokennext->nxt; // check for optional bind even val
    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        memcpy( &bind_even, &bind_odd, sizeof( bind_even ) );  // use bind_odd
    } else {
        attr_val.tok.s = tokennext->token;
        attr_val.tok.l = tokennext->toklen;
        scanerr = att_val_to_su( &bindwork, true, &attr_val, false ); // must be positive TBD
        if( scanerr ) {
            xx_simple_err_exit_cc( ERR_MISS_INV_OPT_VALUE, opt->option, tokennext->token );
            /* never return */
        }
        memcpy( &bind_even, &bindwork, sizeof( bindwork ) );
        out_msg_research( "\tbind even value %ii (%imm) '%s' %i %i \n",
                bind_even.su_inch,bind_even.su_mm, bind_even.su_txt,
                bind_even.su_whole, bind_even.su_dec );
    }
    tokennext = tokennext->nxt;
}


/***************************************************************************/
/*  ( cpinch n   set chars per inch                                        */
/*   WGML 4 accepts values up to _I32_MAX ???                              */
/***************************************************************************/

static void set_cpinch( option * opt )
{
    char            *p;
    char            wkstring[NUM2STR_LENGTH + 1];

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_c( ERR_MISSING_OPT_VALUE, opt->option );
        /* never return */
    }
    p = tokennext->token;
    opt_value = get_num_value( p );
    if( opt_value < 1
      || opt_value > MAX_CPI ) {
        xx_simple_err_exit_c( ERR_OUT_RANGE, "cpinch" );
        /* never return */
    }
    CPI = opt_value;
    add_symvar( global_dict, "$cpi", wkstring, sprintf( wkstring, "%d", CPI ), SI_no_subscript, SF_none );
    tokennext = tokennext->nxt;
}


/***************************************************************************/
/*  ( lpinch n   set lines per inch                                        */
/*   WGML 4 accepts values up to _I32_MAX ???                              */
/***************************************************************************/

static void set_lpinch( option * opt )
{
    char            *p;

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_c( ERR_MISSING_OPT_VALUE, opt->option );
        /* never return */
    }
    p = tokennext->token;
    opt_value = get_num_value( p );
    if( opt_value < 1
      || opt_value > MAX_LPI ) {
        xx_simple_err_exit_c( ERR_OUT_RANGE, "lpinch" );
        /* never return */
    }
    LPI = opt_value;
    /*    LPI (in contrast to CPI) is not stored as global symbol
     *  add_symvar( global_dict, "$lpi", wkstring, sprintf( wkstring, "%d", LPI ), SI_no_subscript, SF_none );
     */
    tokennext = tokennext->nxt;
}


/***************************************************************************/
/*  ( delim x     set GML delimiter                                        */
/***************************************************************************/

static void set_delim( option * opt )
{
    if( tokennext == NULL
      || is_option()
      || tokennext->toklen != 1 ) {     // not length 1
        xx_simple_err_exit_cc( ERR_MISS_INV_OPT_VALUE, opt->option, tokennext == NULL ? " " : tokennext->token );
        /* never return */
    }
    GML_char = tokennext->token[0]; // new delimiter
    tokennext = tokennext->nxt;
    return;
}

/***************************************************************************/
/*  ( device      defined_name                                             */
/***************************************************************************/

static void set_device( option * opt )
{

    char    *   p;
    char    *   pw;
    unsigned    len;

    if( tokennext == NULL
      || tokennext->bol
      || is_option() ) {
        bad_cmd_line_err_exit( ERR_MISSING_DEVICE_NAME, opt->option, ' ' );
        /* never return */
    }
    len = tokennext->toklen;
    p = tokennext->token;

    g_info_research( INF_RECOGNIZED_XXX, "device name", p );
    if( g_dev_name != NULL ) {
        mem_free( g_dev_name );
    }
    g_dev_name = mem_alloc( len + 1 );
    pw = g_dev_name;
    while( len > 0 ) {
         len--;
         *pw++ = *p++;
    }
    *pw = '\0';
    tokennext = tokennext->nxt;
    return;
}

#define MAX_CENTIPOINTS 7200   // maximum allowed by gendev per Wiki

/***************************************************************************/
/*  font_points   validates font_space and font_height values              */
/***************************************************************************/

static bool font_points( cmd_tok * in_tok, char buff[5] )
{
    bool        good;
    bool        has_pt;
    char    *   p;
    unsigned    i;
    unsigned    len;
    int         post_pt;
    int         pre_pt;

    good = true;
    has_pt = false;
    p = in_tok->token;
    len = in_tok->toklen;
    post_pt = 0;
    pre_pt = 0;
    for( i = 0; i < len; i++ ) {
        if( p[i] != '.' ) {
            if( !my_isdigit( p[i] ) ) {
                good = false;
                break;
            }
            if( has_pt ) {
                post_pt++;
            } else {
                pre_pt++;
            }
        } else {
            has_pt = true;
        }
    }
    if( !good
      || pre_pt > 2
      || post_pt > 2 )
        return( false );
    i = 0;
    if( pre_pt > 0 ) {
        if( pre_pt == 1 ) {
            buff[0] = '0';
            buff[1] = p[i];
            i++;
        } else {
            buff[0] = p[i];
            i++;
            buff[1] = p[i];
            i++;
        }
    }
    if( has_pt )
        i++;
    if( post_pt > 0 ) {
        if( post_pt == 1) {
            buff[2] = p[i];
            buff[3] = '0';
        } else {
            buff[2] = p[i];
            i++;
            buff[3] = p[i];
        }
    }
    buff[4] = '\0';

    return( true );
}

/***************************************************************************/
/*  ( font        number name style space height                           */
/***************************************************************************/

static void set_font( option * opt )
{

    bool            good;
    char        *   p;
    char            pts[5];
    char        *   pw;
    cmd_tok     *   opts[3];
    int             fn;
    unsigned        i;
    unsigned        len;
    int             old_errs;
    int             opts_cnt;
    opt_font    *   new_font;
    opt_font    *   f;

    old_errs = err_count;
    new_font = mem_alloc( sizeof( opt_font ) );
    new_font->nxt = NULL;
    new_font->font = FONT0;
    new_font->name = NULL;
    new_font->style = NULL;
    new_font->space = 0;
    new_font->height = 0;

    opts_cnt = 0;
    opts[0] = NULL;
    opts[1] = NULL;
    opts[2] = NULL;

    if( tokennext == NULL
      || tokennext->bol
      || is_option() ) {
        bad_cmd_line_err_exit( ERR_MISSING_FONT_NUMBER, opt->option, ' ' );
        /* never return */
    }
    len = tokennext->toklen;
    p = tokennext->token;

    good = true;
    for( i = 0; i < len; i++ ) {
        if( !my_isdigit( p[i] ) ) {
            good = false;
            break;
        }
    }

    if( !good ) {
        bad_cmd_line_err_exit( ERR_INVALID_FONT_NUMBER, p, ' ' );
        /* never return */
    }
    fn = atoi( p );
    if( fn > UINT8_MAX ) {
        bad_cmd_line_err_exit( ERR_INVALID_FONT_NUMBER, p, ' ' );
        /* never return */
    }
    g_info_research( INF_RECOGNIZED_XXX, "font number", p );
    new_font->font = (font_number)fn;
    tokennext = tokennext->nxt;

    if( tokennext == NULL
      || tokennext->bol
      || is_option() ) {
        bad_cmd_line_err_exit( ERR_MISSING_FONT_NAME, opt->option, ' ' );
        /* never return */
    }
    len = tokennext->toklen;
    p = tokennext->token;

    g_info_research( INF_RECOGNIZED_XXX, "font name", p );
    new_font->name = mem_alloc( len + 1 );
    pw = new_font->name;
    while( len > 0 ) {
         len--;
         *pw++ = *p++;
    }
    *pw = '\0';
    tokennext = tokennext->nxt;

    if( tokennext != NULL
      && !tokennext->bol
      && !is_option() ) {
        opts_cnt++;
        opts[0] = tokennext;
        tokennext = tokennext->nxt;

        if( tokennext != NULL
          && !tokennext->bol
          && !is_option() ) {
            opts_cnt++;
            opts[1] = tokennext;
            tokennext = tokennext->nxt;

            if( tokennext != NULL
              && !tokennext->bol
              && !is_option() ) {
                opts_cnt++;
                opts[2] = tokennext;
                tokennext = tokennext->nxt;
            }
        }
    }

    switch( opts_cnt ) {
    case 0:
        break;
    case 1:
        for( i = 0; i < 4; i++ ) pts[i] = '0';
        pts[4] = '\0';
        len = opts[0]->toklen;
        p = opts[0]->token;
        if( font_points( opts[0], pts ) ) {
            fn = atoi( pts );
            if( fn > MAX_CENTIPOINTS ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_SPACE, p, ' ' );
                /* never return */
            }
            g_info_research( INF_RECOGNIZED_XXX, "font space", pts );
            new_font->space = (unsigned) fn;
        } else {
            g_info_research( INF_RECOGNIZED_XXX, "font style", p );
            new_font->style = mem_alloc( len + 1 );
            pw = new_font->style;
            while( len > 0 ) {
                 len--;
                 *pw++ = *p++;
            }
            *pw = '\0';
        }

        break;
    case 2:
        for( i = 0; i < 4; i++ ) pts[i] = '0';
        pts[4] = '\0';
        len = opts[0]->toklen;
        p = opts[0]->token;
        if( strcmp( "''", p ) == 0 ) {
            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[1]->toklen;
            p = opts[1]->token;
            if( !font_points( opts[1], pts ) ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                /* never return */
            }
            fn = atoi( pts );
            if( fn > MAX_CENTIPOINTS ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                /* never return */
            }
            g_info_research( INF_RECOGNIZED_XXX, "font height", pts );
            new_font->height = (unsigned)fn;
        } else {
            if( font_points( opts[0], pts ) ) {
                fn = atoi( pts );
                if( fn > MAX_CENTIPOINTS ) {
                    bad_cmd_line_err_exit( ERR_INVALID_FONT_SPACE, p, ' ' );
                    /* never return */
                }
                g_info_research( INF_RECOGNIZED_XXX, "font space", pts );
                new_font->space = (unsigned)fn;

                for( i = 0; i < 4; i++ ) pts[i] = '0';
                pts[4] = '\0';
                len = opts[1]->toklen;
                p = opts[1]->token;
                if( !font_points( opts[1], pts ) ) {
                    bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                    /* never return */
                }
                fn = atoi( pts );
                if( fn > MAX_CENTIPOINTS ) {
                    bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                    /* never return */
                }
                g_info_research( INF_RECOGNIZED_XXX, "font height", pts );
                new_font->height = (unsigned) fn;
            } else {
                g_info_research( INF_RECOGNIZED_XXX, "font style", p );
                new_font->style = mem_alloc( len + 1 );
                pw = new_font->style;
                while( len > 0 ) {
                     len--;
                     *pw++ = *p++;
                }
                *pw = '\0';

                for( i = 0; i < 4; i++ ) pts[i] = '0';
                pts[4] = '\0';
                len = opts[1]->toklen;
                p = opts[1]->token;
                if( font_points( opts[1], pts ) ) {
                    fn = atoi( pts );
                    if( fn > MAX_CENTIPOINTS ) {
                        bad_cmd_line_err_exit( ERR_INVALID_FONT_SPACE, p, ' ' );
                        /* never return */
                    }
                    g_info_research( INF_RECOGNIZED_XXX, "font space", pts );
                    new_font->space = (unsigned) fn;
                }
            }
        }

        break;
    case 3:
        len = opts[0]->toklen;
        p = opts[0]->token;
        g_info_research( INF_RECOGNIZED_XXX, "font style", p );
        new_font->style = mem_alloc( len + 1 );
        pw = new_font->style;
        while( len > 0 ) {
             len--;
             *pw++ = *p++;
        }
        *pw = '\0';

        if( strcmp( "''", opts[1]->token ) == 0 ) {
            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[2]->toklen;
            p = opts[2]->token;
            if( !font_points( opts[2], pts ) ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                /* never return */
            }
            fn = atoi( pts );
            if( fn > MAX_CENTIPOINTS ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                /* never return */
            }
            g_info_research( INF_RECOGNIZED_XXX, "font height", pts );
            new_font->height = (unsigned)fn;
        } else {
            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[1]->toklen;
            p = opts[1]->token;
            if( !font_points( opts[1], pts ) ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_SPACE, p, ' ' );
                /* never return */
            }
            fn = atoi( pts );
            if( fn > MAX_CENTIPOINTS ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_SPACE, p, ' ' );
                /* never return */
            }
            g_info_research( INF_RECOGNIZED_XXX, "font space", pts );
            new_font->space = (unsigned)fn;

            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[2]->toklen;
            p = opts[2]->token;
            if( !font_points( opts[2], pts ) ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                /* never return */
            }
            fn = atoi( pts );
            if( fn > MAX_CENTIPOINTS ) {
                bad_cmd_line_err_exit( ERR_INVALID_FONT_HEIGHT, p, ' ' );
                /* never return */
            }
            g_info_research( INF_RECOGNIZED_XXX, "font height", pts );
            new_font->height = (unsigned) fn;
        }
        break;
    default:
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( old_errs == err_count ) {
        if( opt_fonts == NULL ) {
            opt_fonts = new_font;
        } else {
            f = opt_fonts;
            while( f->nxt != NULL ) {
                f = f->nxt;
            }
            f->nxt = new_font;
        }
        out_msg_research( "Font: %i %s ", new_font->font, new_font->name );
        if( new_font->style != NULL ) {
            out_msg_research( "%s ", new_font->style ); // can't use NULL here
        }
        out_msg_research( "%i %i\n", new_font->space, new_font->height );
    } else {
        if( new_font->name !=NULL ) {
            mem_free( new_font->name );
            new_font->name = NULL;
        }
        if( new_font->style !=NULL ) {
            mem_free( new_font->style );
            new_font->style = NULL;
        }
        mem_free( new_font );
        new_font = NULL;
    }
    return;
}

/***************************************************************************/
/*  ( layout      filename                                                 */
/***************************************************************************/

static void set_layout( option * opt )
{
    char            attrwork[MAX_FILE_ATTR + 1];
    unsigned        len;
    struct laystack *laywk;

    if( tokennext == NULL
      || tokennext->bol
      || is_option() ) {
        xx_simple_err_exit_cc( ERR_MISS_INV_OPT_VALUE, opt->option, "" );
        /* never return */
    }
    len = tokennext->toklen;
    laywk = mem_alloc( sizeof( laystack ) + len );
    strncpy( laywk->layfn, tokennext->token, len );
    laywk->layfn[len] = '\0';
    laywk->next = NULL;

    split_attr_file( laywk->layfn, attrwork, sizeof( attrwork ) - 1 );
    if( attrwork[0] != '\0' ) {
        xx_warn_cc( WNG_FILEATTR_IGNORED, attrwork, laywk->layfn );
    }
    if( lay_files == NULL ) {       // first file
        lay_files = laywk;
    } else {                        // stack, not queue
        laywk->next = lay_files;
        lay_files = laywk;
    }
    tokennext = tokennext->nxt;
}

/***************************************************************************/
/*  ( output      filename or (T:1234)filename                             */
/***************************************************************************/

static void set_outfile( option * opt )
{
    char    attrwork[MAX_FILE_ATTR + 1];

    if( tokennext == NULL
      || tokennext->bol
      || is_option() ) {
        xx_simple_err_exit_cc( ERR_MISS_INV_OPT_VALUE, opt->option, "" );
        /* never return */
    }
    out_file = mem_tokdup( tokennext->token, tokennext->toklen );

    split_attr_file( out_file, attrwork, sizeof( attrwork ) - 1 );
    if( attrwork[0] != '\0' ) {
        out_file_attr = mem_strdup( attrwork );
    } else {
        out_file_attr = NULL;
    }
    tokennext = tokennext->nxt;
}


/***************************************************************************/
/*  ( passes n                                                             */
/***************************************************************************/

static void set_passes( option * opt )
{
    char        linestr[NUM2STR_LENGTH + 1];
    char        linestr2[NUM2STR_LENGTH + 1];
    char    *   p;

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_c( ERR_MISSING_OPT_VALUE, opt->option );
        /* never return */
    }
    p = tokennext->token;
    opt_value = get_num_value( p );
    if( opt_value < 1
      || opt_value > MAX_PASSES ) {
        sprintf( linestr, "%d", MAX_PASSES );
        sprintf( linestr2, "%d", opt_value );
        xx_simple_err_exit_cc( ERR_PASSES_VALUE, linestr, linestr2 );
        /* never return */
    }
    passes = opt_value;
    tokennext = tokennext->nxt;
}

/***************************************************************************/
/*  ( from n   start printing at pageno n                                  */
/***************************************************************************/

static void set_from( option * opt )
{
    char    *   p;

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_c( ERR_MISSING_OPT_VALUE, opt->option );
        /* never return */
    }
    p = tokennext->token;
    opt_value = get_num_value( p );
    if( opt_value < 1
      || opt_value >= INT_MAX ) {
        xx_simple_err_exit_c( ERR_OUT_RANGE, "from" );
        /* never return */
    }
    print_from = opt_value;
    tokennext = tokennext->nxt;
}


/***************************************************************************/
/*  ( setsymbol  x y     set variable x to y                               */
/***************************************************************************/

static void set_symbol( option *opt )
{
    char            *name;
    int         rc;

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_c( ERR_MISSING_NAME, opt->option );
        /* never return */
    }
    name = tokennext->token;

    tokennext = tokennext->nxt;

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_c( ERR_MISSING_VALUE, opt->option );
        /* never return */
    }
    rc = add_symvar( global_dict, name, tokennext->token, tokennext->toklen, SI_no_subscript, SF_none );
    tokennext = tokennext->nxt;
}

/***************************************************************************/
/*  ( to n     stop  printing at pageno n                                  */
/***************************************************************************/

static void set_to( option * opt )
{
    char    *   p;

    if( tokennext == NULL
      || tokennext->bol
      || tokennext->token[0] == '('
      || is_option() ) {
        xx_simple_err_exit_c( ERR_MISSING_VALUE, opt->option );
        /* never return */
    }
    p = tokennext->token;
    opt_value = get_num_value( p );
    if( opt_value < 1
      || opt_value >= INT_MAX ) {
        xx_simple_err_exit_c( ERR_OUT_RANGE, "to" );
        /* never return */
    }
    print_to = opt_value;
    tokennext = tokennext->nxt;
}

/***************************************************************************/
/*  ( inclist   or   ( noinclist                                           */
/***************************************************************************/

static void set_inclist( option * opt )
{
    GlobalFlags.inclist = opt->value;
}

/***************************************************************************/
/*  ( index     or   ( noindex                                             */
/***************************************************************************/

static void set_index( option * opt )
{
    GlobalFlags.index = opt->value;
}

/***************************************************************************/
/*  ( statistics or  ( nostatistics                                        */
/***************************************************************************/

static void set_stats( option * opt )
{
    GlobalFlags.statistics = opt->value;
}


/***************************************************************************/
/*  ( warning or ( nowarning                                               */
/***************************************************************************/

static void set_warning( option * opt )
{
    GlobalFlags.warning = opt->value;
}

/***************************************************************************/
/*  ( file xxx    command option file                                      */
/***************************************************************************/

static void set_optfile( option *opt )
{
    char            attrwork[MAX_FILE_ATTR + 1];
    char            *str;
    unsigned        len;
    FILE            *fp;

    if( tokennext == NULL
      || tokennext->bol
      || is_option()
      /* || tokennext->token[0] == '('  allow (t:123)file.opt construct */
      ) {
        xx_simple_err_exit_c( ERR_MISSING_VALUE, opt->option );
        /* never return */
    }
    if( cmd_data_lvl - cmd_data == MAX_NESTING - 1 ) {
        xx_simple_err_exit_c( ERR_MAX_NESTING_OPT, token_buf );
        /* never return */
    }

    len = tokennext->toklen;
    str = tokennext->token;

    g_info_research( INF_RECOGNIZED_XXX, "option file", str );
    strcpy( token_buf, str );
    split_attr_file( token_buf, attrwork, sizeof( attrwork ) - 1 );
    if( attrwork[0] != '\0' ) {
        xx_warn_cc( WNG_FILEATTR_IGNORED, attrwork, token_buf );
    }
    fp = search_file_in_dirs( token_buf, OPT_EXT, "", DSEQ_opt_file );
    if( fp == NULL ) {
        xx_simple_err_exit_c( ERR_FILE_NOT_FOUND, token_buf );
        /* never return */
    }
    /*
     * cmd_data[0] is command line
     */
    if( cmd_data_lvl - cmd_data > 0 ) {
        level_data  *data;

        for( data = cmd_data_lvl; data != cmd_data; data-- ) {
            if( stricmp( try_file_name, data->file_name ) == 0 ) {
                fclose( fp );
                xx_simple_err_exit_c( ERR_RECURSIVE_OPTION, try_file_name );
                /* never return */
            }
        }
    }

    cmd_data_lvl++;
    cmd_data_lvl->sav_tokens = tokennext->nxt;
    cmd_data_lvl->file_name = mem_strdup( try_file_name );
    read_indirect_file( fp );
    fclose( fp );
    tokennext = cmd_data_lvl->cmd_tokens;
}

/***************************************************************************/
/*  Processing routines for 'new format' options                           */
/***************************************************************************/

/***************************************************************************/
/*  -i <GMLINC path>                                                       */
/*    overrides the GMLINC environment variable if present                 */
/***************************************************************************/

static void set_incpath( option * opt )
{
    char        str[_MAX_PATH];
    unsigned    len;

    if( tokennext == NULL
      || tokennext->bol
      || is_option()
      || tokennext->token[0] == '(' ) {
        str[0] = '\0';
        xx_simple_err_exit_c( ERR_MISSING_VALUE, opt->option );
        /* never return */
    }
    len = tokennext->toklen;
    if( len < sizeof( str ) ) {
        strcpy( str, tokennext->token );
    } else {
        strcpy( str, "GML include path too long " );
    }

    ff_set_incpath( str );

    tokennext = tokennext->nxt;

    g_info_research( INF_RECOGNIZED_XXX, "-i", str );
}


/***************************************************************************/
/*  -l <GMLLIB path>                                                       */
/*    overrides the GMLLIB environment variable if present                 */
/***************************************************************************/

static void set_libpath( option * opt )
{
    char        str[256];
    unsigned    len;

    if( tokennext == NULL
      || tokennext->bol
      || is_option()
      || tokennext->token[0] == '(' ) {
        str[0] = '\0';
        xx_simple_err_exit_c( ERR_MISSING_VALUE, opt->option );
        /* never return */
    }
    len = tokennext->toklen;
    if( len < sizeof( str ) ) {
        strcpy( str, tokennext->token );
    } else {
        strcpy( str, "GML library path too long" );
    }

    ff_set_libpath( str );

    tokennext = tokennext->nxt;

    g_info_research( INF_RECOGNIZED_XXX, "-l", str );
}


static void set_quiet( option * opt )
{
    GlobalFlags.quiet = opt->value;
    if( opt->value ) {
        add_symvar( global_dict, "$quiet", "ON", 2, SI_no_subscript, SF_predefined );
    } else {
        add_symvar( global_dict, "$quiet", "OFF", 3, SI_no_subscript, SF_predefined );
    }
}


/***************************************************************************/
/*  -r [filename] [from [to]]                                              */
/*    if only from and to specified the filename will become the           */
/*    masterfilename later (when it is known)                              */
/***************************************************************************/

static void set_research( option * opt )
{
    char        str[256];
    unsigned    len;

    GlobalFlags.research = opt->value;

    if( tokennext == NULL
      || tokennext->bol
      || is_option()
      || tokennext->token[0] == '(' ) {
        str[0] = '\0';
    } else {
        len = tokennext->toklen;
        if( len < sizeof( str ) ) {
            strcpy( str, tokennext->token );
        } else {
            strcpy( str, "too long " );
        }
        ProcFlags.researchfile = true;  // only one file
        research_from = FIRST_LINE;
        research_to = LAST_LINE;

        research_file_name[0] = '\0';   // no filename
        if( my_isalpha( *str ) ) {      // filename ?
            if( len < sizeof( research_file_name ) ) {
                strcpy( research_file_name, str );
            }
            tokennext = tokennext->nxt;
            if( tokennext == NULL
              || tokennext->bol
              || is_option()
              || tokennext->token[0] == '(' ) {
                /* nothing to do */
            } else {                    // get from and to values
                research_from = get_num_value( tokennext->token );
                strcat( str, " " );
                strcat( str, tokennext->token );
                tokennext = tokennext->nxt;
                if( tokennext == NULL
                  || tokennext->bol
                  || is_option()
                  || tokennext->token[0] == '(' ) {
                    /* nothing to do */
                } else {
                    research_to = get_num_value( tokennext->token );
                    strcat( str, " " );
                    strcat( str, tokennext->token );
                    tokennext = tokennext->nxt;
                }
            }
        } else {
            if( tokennext == NULL
              || tokennext->bol
              || is_option()
              || tokennext->token[0] == '(' ) {
                /* nothing to do */
            } else {                    // get from and to values
                research_from = get_num_value( tokennext->token );
                tokennext = tokennext->nxt;
                if( tokennext == NULL
                  || tokennext->bol
                  || is_option()
                  || tokennext->token[0] == '(' ) {
                    /* nothing to do */
                } else {
                    research_to = get_num_value( tokennext->token );
                    strcat( str, " " );
                    strcat( str, tokennext->token );
                    tokennext = tokennext->nxt;
                }
            }

        }
    }
    g_info_research( INF_RECOGNIZED_XXX, "-r", str );
}

#if 0                  // always set (w)script option, don't allow to disable
static void set_wscript( option * opt )
{
    GlobalFlags.wscript = opt->value;
}
#endif


/* struct option and logic adapted from bld\cc\c\coptions.c */

/*options in format of WGML i.e.,   ( option xxx     */

static option gml_old_options[] =
{
/*                 length  minimum  default  Processing             */
/*     optionname          Abbrev   value    routine      Parmcount */
    { "altextension",  12, 6,       0,       set_altext,     1 },
    { "bind",          4,  1,       0,       set_bind,       1 },
    { "cpinch",        6,  3,       10,      set_cpinch,     1 },
    { "delim",         5,  3,       0,       set_delim,      1 },
    { "device",        6,  3,       0,       set_device,     1 },
    { "description",   11, 4,       0,       ign_option,     1 },
    { "duplex",        6,  3,       0,       ign_option,     0 },
    { "file",          4,  4,       0,       set_optfile,    1 },
    { "font",          4,  4,       0,       set_font,       5 },
    { "fontfamily",    10, 5,       0,       ign_option,     0 },
    { "format",        6,  4,       0,       ign_option,     1 },
    { "from",          4,  4,       1,       set_from,       1 },
    { "inclist",       7,  4,       1,       set_inclist,    0 },
    { "index",         5,  3,       1,       set_index,      0 },
    { "layout",        6,  3,       0,       set_layout,     1 },
    { "linemode",      8,  4,       0,       ign_option,     0 },
    { "llength",       7,  2,       130,     ign_option,     1 },
    { "logfile",       7,  3,       0,       ign_option,     1 },
    { "lpinch",        6,  3,       6,       set_lpinch,     1 },
    { "mailmerge",     9,  4,       0,       ign_option,     1 },
    { "noduplex",      8,  5,       0,       ign_option,     0 },
    { "noinclist",     9,  6,       0,       set_inclist,    0 },
    { "noindex",       7,  5,       0,       set_index,      0 },
    { "nopause",       7,  3,       0,       ign_option,     0 },
    { "noquiet",       7,  3,       0,       set_quiet,      0 },
    { "noscript",      8,  5,       0,       wng_option,     0 },
    { "nostatistics",  12, 6,       0,       set_stats,      0 },
    { "nowait",        6,  6,       0,       ign_option,     0 },
    { "nowarning",     9,  6,       0,       set_warning,    0 },
    { "output",        6,  3,       0,       set_outfile,    1 },
    { "passes",        6,  4,       1,       set_passes,     1 },
    { "pause",         5,  5,       1,       ign_option,     0 },
    { "process",       7,  4,       0,       ign_option,     1 },
    { "quiet",         5,  5,       1,       set_quiet,      0 },
    { "resetscreen",   11, 5,       1,       ign_option,     0 },
    { "script",        6,  3,       1,       wng_option,     0 },
    { "setsymbol",     9,  3,       0,       set_symbol,     2 },
    { "statistics",    10, 4,       1,       set_stats,      0 },
    { "terse",         5,  5,       1,       ign_option,     0 },
    { "to",            2,  2,       INT_MAX, set_to,         1 },
    { "valueset",      8,  6,       0,       ign_option,     1 },
    { "verbose",       7,  4,       1,       ign_option,     0 },
    { "wait",          4,  4,       1,       ign_option,     0 },
    { "warning",       7,  4,       1,       set_warning,    0 },
    { "wscript",       7,  4,       1,       ign_option,     0 },   // always set
    { NULL,            0,  0,       0,       ign_option,     0 }    // end marker
};

/* options in 'new' format   -o                        */
static option gml_new_options[] =
{
    { "i",             1,  1,       0,        set_incpath,   1 },
    { "l",             1,  1,       0,        set_libpath,   1 },
    { "q",             1,  1,       1,        set_quiet,     0 },
    { "r",             1,  1,       1,        set_research,  3 },
    { NULL,            0,  0,       0,        ign_option,    0 }    // end marker
};


/***************************************************************************/
/*  split (t:200)Filename    into t:200 and Filename                       */
/***************************************************************************/

void split_attr_file( char *filename , char *attr, unsigned attrlen )
{
    char        *fn;
    char        *p;
    unsigned    k;

    fn = filename;
    p = attr;
    if( *fn == '(' ) {                  // attribute infront of filename
        fn++;
        for( k = 0; k < attrlen; k++ ) {
            *p++ = *fn++;               // isolate attribute
            if( *fn == ')' ) {
                fn++;
                break;
            }
        }
        *p = '\0';                      // terminate attr

        p = filename;
        while( *fn != '\0' ) {          // shift filename
            *p++ = *fn++;
        }
    }
    *p = '\0';                          // terminate attr or filename
}


/***************************************************************************/
/*  test for delimiter                                                     */
/***************************************************************************/

static bool option_delimiter( char c )
{
    return( c == ' ' || c == '\t' || c == '(' || CmdScanSwitchChar( c ) || c == '\n' );
}


/***************************************************************************/
/*                                                                         */
/***************************************************************************/

static void strip_quotes( char * fname )
{
    char    *   s;
    char    *   d;

    if( *fname == '"' ) {
    /* string will shrink so we can reduce in place */
        d = fname;
        for( s = d + 1; *s && *s != '"'; ++s ) {
        /* collapse double backslashes, only then look for escaped quotes */
             if( s[0] == '\\'
               && s[1] == '\\' ) {
                 ++s;
             } else if( s[0] == '\\'
               && s[1] == '"' ) {
                 ++s;
             }
             *d++ = *s;
        }
        *d = '\0';
    }
}


/***************************************************************************/
/*  process 'new' option -x  /x  via option table                          */
/***************************************************************************/

static cmd_tok * process_option( option * op_table, cmd_tok * tok )
{
    bool        opt_delim_start;
    char        first_c;
    char    *   opt;
    char    *   option_start;
    char    *   p;
    char    *   pa;
    int         i;


    option_start = tok->token;
    p = option_start;
    opt_delim_start = option_delimiter( *p );
    if( opt_delim_start ) {
        p++;
    }
    first_c = my_tolower( *p );
    tokennext = tok->nxt;
    for( i = 0; (opt = op_table[i].option) != NULL; i++ ) {
        if( first_c == *opt ) {               // match for first char
            opt_value = op_table[i].value;
            for( opt++, pa = p + 1; *opt != '\0'; opt++, pa++ ) {
                if( *opt != (char)my_tolower( *pa ) ) {
                    if( *opt < 'A'
                      || *opt > 'Z' )
                        break;
                    if( *opt != *pa ) {
                        break;
                    }
                }
            }
            if( *opt == '\0' ) {
                if( !opt_delim_start
                  || *pa == '\0'
                  || option_delimiter( *pa ) ) {
                    break;
                }
            }
        }
    }
    if( opt == NULL ) {
        bad_cmd_line_err_exit( ERR_INVALID_OPTION, option_start, ' ' );
        /* never return */
    }
    opt_scan_ptr = pa;
    g_info_research( INF_RECOGNIZED_XXX, "n1", option_start );
    op_table[i].function( &op_table[i]);
    return( tokennext );
}


/***************************************************************************/
/*  process 'old' option ( xxx                                             */
/***************************************************************************/

static cmd_tok * process_option_old( option * op_table, cmd_tok * tok )
{
    bool        opt_delim_start;
    char        c;
    char        first_c;
    char    *   opt;
    char    *   option_start;
    char    *   p;
    char    *   pa;
    int         i;
    unsigned    len;

    p = tok->token;
    option_start = p;
    len = tok->toklen;
    tokennext = tok->nxt;
    opt_delim_start = ( *p == '(' );
    if( opt_delim_start ) {
        if( len == 1 ) {
            return( tokennext );        // skip single (
        }
        p++;
        len--;
    }
    first_c = my_tolower( *p );
    for( i = 0; (opt = op_table[i].option) != NULL; i++ ) {
        if( first_c != *opt )
            continue;
        if( len < op_table[i].minLength )
            continue;                   // cannot be this option
        if( strnicmp( opt, p, len ) ) {
            continue;
        }
        /* '=' indicates optional '=' */
        /* '#' indicates a decimal numeric value */
        /* '$' indicates identifier */
        /* '@' indicates filename */
        /* '*' indicates additional characters will be scanned by option routine */
        /* if a capital letter appears in the option, then input must match exactly */
        /* otherwise all input characters are changed to lower case before matching */
        opt_value = op_table[i].value;
        for( pa = p + len, opt += op_table[i].optionLen; *opt != '\0' && *opt != '*'; opt++ ) {
            if( *opt == '#' ) {         // collect a number
                SkipSpaces( pa );       // skip blanks
                if( my_isdigit( *pa ) ) {
                    opt_value = 0;
                    for( ; my_isdigit( *pa ); pa++ ) {
                        opt_value = opt_value * 10 + *pa - '0';
                    }
                    opt_scan_ptr = pa;
                }
                g_info_research( INF_RECOGNIZED_XXX, "num", option_start );
            } else if( *opt == '$' ) {  // collect an identifer
                if( *pa == ' ' )        // skip 1 blank
                    pa++;
                opt_parm = pa;
                for( ; (c = *pa) != '\0'; pa++ ) {
                    if( c == '(' )
                        break;
                    if( c == ' ' )
                        break;
                    if( CmdScanSwitchChar( c ) )
                        break;
                    if( c == '\n' ) {
                        *pa = ' ';
                        break;
                    }
                }
                opt_scan_ptr = pa;
                g_info_research( INF_RECOGNIZED_XXX, "id", option_start );
            } else if( *opt == '@' ) {  // collect a filename
                opt_parm = pa;
                c = *pa;
                if( c == '"' ){         // "filename"
                    for( pa++; (c = *pa) != '\0'; pa++ ) {
                        if( c == '"' ){
                            pa++;
                            break;
                        }
                        if( c == '\\' ){
                            pa++;
                        }
                    }
                } else {
                    for( ; (c = *pa) != '\0'; pa++ ) {
                        if( is_space_tab_char( c ) )
                            break;
                        if( CmdScanSwitchChar( c ) )
                            break;
                        if( c == '\n' ) {
                            *pa = ' ';
                            break;
                        }
                    }
                }
                g_info_research( INF_RECOGNIZED_XXX, "fn", option_start );
            } else if( *opt == '=' ) {  // collect an optional '='
                if( *pa == '='
                  || *pa == '#' ) {
                    pa++;
                }
            } else {
                if( *opt != (char)my_tolower( *pa ) ) {
                    if( *opt < 'A'
                      || *opt > 'Z' )
                        break;
                    if( *opt != *pa ) {
                        break;
                    }
                }
                pa++;
                opt_scan_ptr = pa;
            }
        }
        if( *opt == '*' ) {
            break;
        }
        if( *opt == '\0' ) {
            if( !opt_delim_start
              || *pa == '\0'
              || option_delimiter( *pa ) ) {
                break;
            }
        }
        g_info_research( INF_RECOGNIZED_XXX, "5", option_start );
    }
    if( opt == NULL ) {
        bad_cmd_line_err_exit( ERR_INVALID_OPTION, option_start, '(' );
        /* never return */
    }
    opt_scan_ptr = pa;
    g_info_research( INF_RECOGNIZED_XXX, "1", option_start );
    op_table[i].function( &op_table[i] );
    return( tokennext );
}


/***************************************************************************/
/*  determine if tokennext is an option                                    */
/***************************************************************************/

static bool is_option( void )
{
    int         i;
    unsigned    len;
    char    *   opt;
    char        c;
    char    *   p;
    char    *   option_start;

    if( tokennext == NULL )
        return( false );
    p = tokennext->token;
    option_start = p;
    len = tokennext->toklen;
    c = my_tolower( *p );
    if( c == '(' ) {
        if( len == 1 ) {            // skip single (
            tokennext = tokennext->nxt;
            p = tokennext->token;
            option_start = p;
            len = tokennext->toklen;
            c = my_tolower( *p );
        } else {
            p++;
            len--;
            c = my_tolower( *p );
        }
    }
    for( i = 0; ; i++ ) {
        opt = gml_old_options[i].option;
        if( opt == NULL )
            break;    // end of table
        if( c != *opt )
            continue;  // easy disqualifiers: first char & length
        if( len < gml_old_options[i].minLength )
            continue;
        if( len > gml_old_options[i].optionLen )
            continue;
        if( strnicmp( opt, p, len ) )
            continue; // no match
        return( true );                         // match found
    }

    p = tokennext->token;
    option_start = p;
    len = tokennext->toklen;
    c = my_tolower( *p );
    if( option_delimiter( c ) ) {
        p++;
        c = my_tolower( *p );
        --len;

        for( i = 0; ; i++ ) {
            opt = gml_new_options[i].option;
            if( opt == NULL )
                break;    // end of table
            if( c != *opt )
                continue;  // easy disqualifiers: first char & length
            if( len < gml_new_options[i].minLength )
                continue;
            if( len > gml_new_options[i].optionLen )
                continue;
            if( strnicmp( opt, p, len ) )
                continue; // no match
            return( true );                         // match found
        }
    }
    return( false );
}


/***************************************************************************/
/*  get Document master input file name                                    */
/***************************************************************************/

static cmd_tok * process_master_filename( cmd_tok * tok )
{
    char        attrwork[MAX_FILE_ATTR + 1];
    char    *   p;

    p = mem_tokdup( tok->token, tok->toklen );
    g_info_research( INF_RECOGNIZED_XXX, "document source file", p );
    strip_quotes( p );
    if( master_fname != NULL ) {         // more than one master file ?
        g_banner();
        bad_cmd_line_err_exit( ERR_DOC_DUPLICATE, tok->token, ' ' );
        /* never return */
    }
    split_attr_file( p , attrwork, sizeof( attrwork ) - 1 );
    if( attrwork[0] != '\0' ) {
        xx_warn_cc( WNG_FILEATTR_IGNORED, attrwork, p );
        master_fname_attr = mem_strdup( attrwork );
    } else {
        master_fname_attr = NULL;
    }
    master_fname = p;
    return( tok->nxt );
}


/***************************************************************************/
/*  process command line and option files                                  */
/*                                                                         */
/*  for a cmdline like wgml "file.gml ( dev ps" where the quotes are part  */
/*  of the cmdline, special processing, the OW doc build needs it.         */
/***************************************************************************/

int proc_options( char * string )
{
    bool            sol;                    // start of line switch
    char            c;
    char            linestr[NUM2STR_LENGTH + 1];
    char            linestr2[NUM2STR_LENGTH + 1];
    char            *p;
    char            *s_after_dq;
    cmd_tok         *tok;
    int             tokcount;

    cmd_data_lvl = cmd_data;

    SkipSpaces( string );
    s_after_dq = string;                // assume no starting quote
    if( *string == d_q ) {              // take care of possible quotes
        for( p = string + 1; *p != '\0'; p++ )
            /* empty */ ;
        p--;
        while( *p == ' ' ) {            // ignore trailing spaces
            p--;
        }
        if( *p == d_q ) {               // ending quote
            *p = '\0';                  // remove
            s_after_dq = string + 1;    // start after leading quote
        }
    }

    cmd_data_lvl->cmd_tokens = NULL;
    tokcount = split_tokens( s_after_dq );
    sprintf( linestr, "%d", tokcount );
    g_info_research( INF_CMDLINE_TOK_CNT, linestr );
    tok = cmd_data_lvl->cmd_tokens;
    for( ;; ) {
        while( tok != NULL ) {
            sol = tok->bol;
            c = tok->token[0];

            if( CmdScanSwitchChar( c ) ) {
                /***************************************************************/
                /*  process 'new' options -x or /x                             */
                /***************************************************************/
                tok = process_option( gml_new_options, tok );

                if( tok != NULL
                  && !tok->bol ) {
                    /*******************************************************/
                    /*  allow master filename if following 'new' option    */
                    /*******************************************************/
                    tok->bol = sol;
                }
            } else {
                if( c == '('
                  || !sol ) {
                    /*******************************************************/
                    /*  process 'old' options  ( xxx                       */
                    /*******************************************************/
                    tok = process_option_old( gml_old_options, tok );
                } else {

                    /*******************************************************/
                    /*  collect Document source file name                  */
                    /*******************************************************/
                    tok = process_master_filename( tok );
                }
            }
        }
        if( cmd_data_lvl->cmd_tokens != NULL ) {
            free_tokens();
        }
        if( cmd_data_lvl == cmd_data ) {
            break;
        }
        if( cmd_data_lvl->file_name != NULL ) {
            mem_free( cmd_data_lvl->file_name );
            cmd_data_lvl->file_name = NULL;
        }
        tok = cmd_data_lvl->sav_tokens;
        cmd_data_lvl--;
    }
    if( print_to < print_from ) {
        g_banner();
        sprintf( linestr, "%d", print_from );
        sprintf( linestr2, "%d", print_to );
        xx_simple_err_exit_cc( ERR_INV_PAGE_RANGE, linestr, linestr2 );
        /* never return */
    }
    return( tokcount );
}
