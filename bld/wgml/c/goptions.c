/************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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

#if defined( __UNIX__ ) || defined( __WATCOMC__ )
#include <unistd.h>
#else
#include <io.h>
#endif
#include <fcntl.h>
#include "wgml.h"
#include "findfile.h"
#include "gvars.h"

#define str( a ) # a

typedef struct  option {
    char        *   option;             // the option
    short           optionLenM1;        // length of option - 1
    short           minLength;          // minimum abbreviation
    long            value;              // sometimes value to set option to
    void            (*function)( struct option *optentry );
    int             parmcount;          // expected number of parms
} option;


typedef struct cmd_tok {
    struct cmd_tok  *   nxt;
    size_t              toklen;
    bool                bol;
    char                token[1];       // variable length

} cmd_tok;

static unsigned     level;              // include level 0 = cmdline
static char     *   buffers[MAX_NESTING];
static cmd_tok  *   cmd_tokens[MAX_NESTING];
static char     *   file_names[MAX_NESTING];
static cmd_tok  *   sav_tokens[MAX_NESTING];
static char     *   save[MAX_NESTING];
static char     *   opt_parm;
static char     *   opt_scan_ptr;
static long         opt_value;
static cmd_tok  *   tokennext;

static bool         is_option( void ); // used before defined


/***************************************************************************/
/*  free storage for tokens at specified include level                     */
/***************************************************************************/

static  void    free_tokens( int lvl )
{
    cmd_tok     *   tok;
    cmd_tok     *   wk;

    tok = cmd_tokens[lvl];
    cmd_tokens[lvl] = NULL;

    while( tok != NULL ) {
        wk = tok->nxt;
        mem_free( tok );
        tok = wk;
    }
}

/***************************************************************************/
/*  split a line into blank delimited words                                */
/***************************************************************************/

static  int     split_tokens( char *str )
{
    cmd_tok     *   tok;
    cmd_tok     *   new;
    int             cnt;
    char        *   tokstart;
    bool            linestart;
    size_t          tokl;
    char            quote;

    linestart = true;                   // assume start of line
    cnt = 0;                            // found tokens

    tok = cmd_tokens[level];          // first token at this level
    if( tok != NULL ) {
        while( tok->nxt != NULL ) {
            tok = tok->nxt;             // last token at this level
        }
    }

    for( ;; ) {
        while( (*str == ' ') || ( *str == '\t') )  str++;// skip blanks / tabs
        if( *str == '\0' ) {
            break;
        }
        if( *str == '"' || *str == '\'' ) {
            quote = *str++;
        } else {
           quote = '\0';
        }
        tokstart = str;
        while( *str ) {
            if( (quote == '\0' && ((*str == ' ') || (*str == '\t')))
                || *str == '\n' ) {
                break;
            }
            if( *str == quote ) {
                break;
            }
            str++;
        }
        cnt++;
        tokl = str - tokstart;
        if( *str == '\n' ) {
            linestart =  true;
            str++;
        }
        if( quote ) {
            str++;
        }
        if( tokl == 0 ) {
            continue;
        }

        new = mem_alloc( sizeof( *new ) + tokl );
        new->nxt = NULL;
        new->bol = linestart;
        linestart = false;
        new->toklen = tokl;
        strncpy_s(new->token, new->toklen + 1, tokstart, tokl );

        if( tok == NULL ) {
            cmd_tokens[level] = new;
        } else {
            tok->nxt = new;
        }
        tok = new;
    }
    return( cnt );
}



/***************************************************************************/
/*  Format error in cmdline                                                */
/***************************************************************************/

static  char    *bad_cmd_line( msg_ids msg, char *str, char n )
{
    char    *   p;
    char    *   pbuff;

    pbuff = mem_alloc( strlen( str ) + 1 );
    p = pbuff;

    for( ; ; ) {
        if( *str == '\0' ) break;
        if( *str == '\n' ) break;
        *p++ = *str++;
        if( *str == '-' ) break;
        if( *str == switch_char ) break;

        if( *str == n ) break;         // for additional stop char '(' or ' '
    }
    *p = '\0';
    g_banner();
    g_err( msg, pbuff );
    mem_free( pbuff );
    err_count++;
    return( str );
}

/***************************************************************************/
/*  read an option file into memory                                        */
/***************************************************************************/

static  char    *read_indirect_file( const char * filename )
{
    char    *   buf;
    char    *   str;
    int         handle;
    int         len;
    char        ch;

    buf = NULL;
    handle = open( filename, O_RDONLY | O_BINARY );
    if( handle != -1 ) {
        len = filelength( handle );
        buf = mem_alloc( len + 1 );
        read( handle, buf, len );
        buf[len] = '\0';
        close( handle );
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
    }
    return( buf );
}


/***************************************************************************/
/*  convert string to integer                                              */
/***************************************************************************/
static  long    get_num_value( char * p )
{
    char    c;
    int     j;
    long    value;

    value = 0;
    j = 0;
    for( ;; ) {
        c = p[j];
        if( c < '0' || c > '9' ) break;
        value = value * 10 + c - '0';
        ++j;
    }
    return( value );
}


/***************************************************************************/
/*  ignore option consuming option parms if neccessary                     */
/***************************************************************************/

static void ign_option( option * opt )
{

    if( strcmp( opt->option, "wscript" ) ) {// ignore wscript without msg
        g_warn( wng_ign_option, opt->option );
    }
    wng_count++;
    if( opt->parmcount > 0 ) {
        int     k;

        for( k = 0; k < opt->parmcount; k++ ) {
            if( tokennext == NULL )  break;
            if( tokennext->bol ) break;
            if( tokennext->token[0] == '(' ) break;
            if( is_option() == true ) break;
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

    g_warn( wng_wng_option, opt->option );
    wng_count++;
    if( opt->parmcount > 0 ) {
        int     k;

        for( k = 0; k < opt->parmcount; k++ ) {
            if( tokennext == NULL )  break;
            if( tokennext->bol ) break;
            if( tokennext->token[0] == '(' ) break;
            if( is_option() == true ) break;
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
    int         len;

    if( tokennext == NULL || tokennext->bol || is_option() == true ) {
        bad_cmd_line( err_missing_opt_value, opt->option, ' ' );
    } else {
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
}

/***************************************************************************/
/*  ( Bind odd [even]   1 or 2 Horizontal Space values                     */
/***************************************************************************/

static void set_bind( option * opt )
{
    bool        scanerr;
    char    *   p;
    su          bindwork;

    if( tokennext == NULL || tokennext->bol || tokennext->token[0] == '(' \
                                            || is_option() == true ) {

        g_err( err_miss_inv_opt_value, opt->option, "" );
        err_count++;

    } else {
        p = tokennext->token;
        scanerr = to_internal_SU( &p, &bindwork );
        if( scanerr ) {
            g_err( err_miss_inv_opt_value, opt->option,
                   tokennext->token );
            err_count++;
            tokennext = tokennext->nxt;
        } else {
            memcpy_s( &bind_odd, sizeof( bind_odd), &bindwork, sizeof( bindwork ) );

            out_msg( "\tbind odd  value %lii (%limm) '%s' %li %li \n",
                     bind_odd.su_inch, bind_odd.su_mm, bind_odd.su_txt,
                     bind_odd.su_whole, bind_odd.su_dec );

            tokennext = tokennext->nxt; // check for optional bind even val
            if( tokennext == NULL || tokennext->bol ||
                tokennext->token[0] == '(' || is_option() == true ) {

                memcpy_s( &bind_even, sizeof( bind_even), &bind_odd,
                          sizeof( bind_odd ) );  // use bind_odd
            } else {
                p = tokennext->token;
                scanerr = to_internal_SU( &p, &bindwork );
                if( scanerr ) {
                    g_err( err_miss_inv_opt_value, opt->option,
                          tokennext->token );
                    err_count++;
                } else {
                    memcpy( &bind_even, &bindwork, sizeof( bindwork ) );
                    out_msg( "\tbind even value %lii (%limm) '%s' %li %li \n",
                             bind_even.su_inch,bind_even.su_mm, bind_even.su_txt,
                             bind_even.su_whole, bind_even.su_dec );
                }
                tokennext = tokennext->nxt;
            }
        }
    }
}


/***************************************************************************/
/*  ( cpinch n   set chars per inch                                        */
/*   WGML 4 accepts values up to _I32_MAX ???                              */
/***************************************************************************/

static void set_cpinch( option * opt )
{
    char    *   p;
    char        wkstring[MAX_L_AS_STR];

    if( tokennext == NULL || tokennext->bol ||
        tokennext->token[0] == '(' || is_option() == true ) {

        g_err( err_missing_opt_value, opt->option );
        err_count++;
        CPI = opt->value;               // set default value
    } else {
        p = tokennext->token;
        opt_value = get_num_value( p );
        if( opt_value < 1 || opt_value > MAX_CPI ) {
            g_err( err_out_range, "cpinch" );
            err_count++;
            CPI = opt->value;           // set default value
        } else {
            CPI = opt_value;
        }
        ltoa( CPI, wkstring, 10 );
        add_symvar( &global_dict, "$cpi", wkstring, no_subscript, 0 );
        tokennext = tokennext->nxt;
    }
}


/***************************************************************************/
/*  ( lpinch n   set lines per inch                                        */
/*   WGML 4 accepts values up to _I32_MAX ???                              */
/***************************************************************************/

static void set_lpinch( option * opt )
{
    char    *   p;

    if( tokennext == NULL || tokennext->bol ||
        tokennext->token[0] == '(' || is_option() == true ) {

        g_err( err_missing_opt_value, opt->option );
        err_count++;
        LPI = opt->value;               // set default value
    } else {
        p = tokennext->token;
        opt_value = get_num_value( p );
        if( opt_value < 1 || opt_value > MAX_LPI ) {
            g_err( err_out_range, "lpinch" );
            err_count++;
            LPI = opt->value;           // set default value
        } else {
            LPI = opt_value;
        }
    /*    LPI (in contrast to CPI) is not stored as global symbol
     *  ltoa( LPI, wkstring, 10 );
     *  add_symvar( &global_dict, "$lpi", wkstring, no_subscript, 0 );
     */
        tokennext = tokennext->nxt;
    }
}


/***************************************************************************/
/*  ( delim x     set GML delimiter                                        */
/***************************************************************************/

static void set_delim( option * opt )
{
    if( tokennext == NULL || is_option() == true \
                          || tokennext->toklen != 1 ) {       // not length 1
        g_err( err_miss_inv_opt_value, opt->option,
                tokennext == NULL ? " " : tokennext->token );
        err_count++;
        GML_char = GML_CHAR_DEFAULT;    // set default :
    } else {
        GML_char = tokennext->token[0]; // new delimiter
        tokennext = tokennext->nxt;
    }
    return;
}

/***************************************************************************/
/*  ( device      defined_name                                             */
/***************************************************************************/

static void set_device( option * opt )
{

    char    *   pw;
    char    *   p;
    int         len;

    if( tokennext == NULL || tokennext->bol || is_option() == true ) {
        bad_cmd_line( err_missing_device_name, opt->option, ' ' );
    } else {
        len = tokennext->toklen;
        p = tokennext->token;

        g_info_lm( inf_recognized_xxx, "device name", p );
        if( dev_name ) {
            mem_free( dev_name );
        }
        dev_name = mem_alloc( len + 1 );
        pw = dev_name;
        while( len > 0 ) {
             len--;
             *pw++ = *p++;
        }
        *pw = '\0';
        tokennext = tokennext->nxt;
    }
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
    int         i;
    int         len;
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
            if( p[i] < '0' || p[i] > '9' ) {
                good = false;
                break;
            }
            if( has_pt == true ) post_pt++;
            else pre_pt++;
        }
        else has_pt = true;
    }
    if( good == false || pre_pt > 2 || post_pt > 2 ) return( false );
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
    if( has_pt == true ) i++;
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
    char            pts[5];
    char        *   pw;
    char        *   p;
    int             i;
    int             fn;
    int             len;
    int             old_errs;
    int             opts_cnt;
    opt_font    *   new_font;
    opt_font    *   f;
    cmd_tok     *   opts[3];

    old_errs = err_count;
    new_font = mem_alloc( sizeof( opt_font ) );
    new_font->nxt = NULL;
    new_font->font = 0;
    new_font->name = NULL;
    new_font->style = NULL;
    new_font->space = 0;
    new_font->height = 0;

    opts_cnt = 0;
    opts[0] = NULL;
    opts[1] = NULL;
    opts[2] = NULL;

    if( tokennext == NULL || tokennext->bol || is_option() == true ) {
        bad_cmd_line( err_missing_font_number, opt->option, ' ' );
        mem_free( new_font );
        new_font = NULL;
        return;
    } else {
        len = tokennext->toklen;
        p = tokennext->token;

        good = true;
        for( i = 0; i < len; i++ ) {
            if( p[i] < '0' || p[i] > '9' ) {
                good = false;
                break;
            }
        }

        if( good == false ) {
            bad_cmd_line( err_invalid_font_number, p, ' ' );
            tokennext = tokennext->nxt;
        } else {
            fn = atoi( p );
            if( fn < 0 || fn > UINT8_MAX ) {
                bad_cmd_line( err_invalid_font_number, p, ' ' );
                tokennext = tokennext->nxt;
            } else {
                g_info_lm( inf_recognized_xxx, "font number", p );
                new_font->font = (font_number)fn;
                tokennext = tokennext->nxt;
            }
        }
    }

    if( tokennext == NULL || tokennext->bol || is_option() == true ) {
        bad_cmd_line( err_missing_font_name, opt->option, ' ' );
        mem_free( new_font );
        new_font = NULL;
        return;
    } else {
        len = tokennext->toklen;
        p = tokennext->token;

        g_info_lm( inf_recognized_xxx, "font name", p );
        new_font->name = mem_alloc( len + 1 );
        pw = new_font->name;
        while( len > 0 ) {
             len--;
             *pw++ = *p++;
        }
        *pw = '\0';
        tokennext = tokennext->nxt;
    }

    if( tokennext != NULL && !tokennext->bol && is_option() == false ) {
        opts_cnt++;
        opts[0] = tokennext;
        tokennext = tokennext->nxt;

        if( tokennext != NULL && !tokennext->bol && is_option() == false ) {
            opts_cnt++;
            opts[1] = tokennext;
            tokennext = tokennext->nxt;

            if( tokennext != NULL && !tokennext->bol && is_option() == false ) {
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
        if( font_points( opts[0], pts ) == true ) {
            fn = atoi( pts );
            if( fn > MAX_CENTIPOINTS ) {
                bad_cmd_line( err_invalid_font_space, p, ' ' );
            } else {
                g_info_lm( inf_recognized_xxx, "font space", pts );
                new_font->space = (uint32_t) fn;
            }
        } else {
            g_info_lm( inf_recognized_xxx, "font style", p );
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
        if( !strcmp( p, "''" ) ) {
            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[1]->toklen;
            p = opts[1]->token;
            if( font_points( opts[1], pts ) == false ) {
                bad_cmd_line( err_invalid_font_height, p, ' ' );
            } else {
                fn = atoi( pts );
                if( fn > MAX_CENTIPOINTS ) {
                    bad_cmd_line( err_invalid_font_height, p, ' ' );
                } else {
                    g_info_lm( inf_recognized_xxx, "font height", pts );
                    new_font->height = (uint32_t) fn;
                }
            }
        } else {
            if( font_points( opts[0], pts ) == true ) {
                fn = atoi( pts );
                if( fn > MAX_CENTIPOINTS ) {
                    bad_cmd_line( err_invalid_font_space, p, ' ' );
                } else {
                    g_info_lm( inf_recognized_xxx, "font space", pts );
                    new_font->space = (uint32_t) fn;
                }

                for( i = 0; i < 4; i++ ) pts[i] = '0';
                pts[4] = '\0';
                len = opts[1]->toklen;
                p = opts[1]->token;
                if( font_points( opts[1], pts ) == false ) {
                    bad_cmd_line( err_invalid_font_height, p, ' ' );
                } else {
                    fn = atoi( pts );
                    if( fn > MAX_CENTIPOINTS ) {
                        bad_cmd_line( err_invalid_font_height, p, ' ' );
                    } else {
                        g_info_lm( inf_recognized_xxx, "font height", pts );
                        new_font->height = (uint32_t) fn;
                    }
                }
            } else {
                g_info_lm( inf_recognized_xxx, "font style", p );
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
                if( font_points( opts[1], pts ) == true ) {
                    fn = atoi( pts );
                    if( fn > MAX_CENTIPOINTS ) {
                        bad_cmd_line( err_invalid_font_space, p, ' ' );
                    } else {
                        g_info_lm( inf_recognized_xxx, "font space", pts );
                        new_font->space = (uint32_t) fn;
                    }
                }
            }
        }

        break;
    case 3:
        len = opts[0]->toklen;
        p = opts[0]->token;
        g_info_lm( inf_recognized_xxx, "font style", p );
        new_font->style = mem_alloc( len + 1 );
        pw = new_font->style;
        while( len > 0 ) {
             len--;
             *pw++ = *p++;
        }
        *pw = '\0';

        if( !strcmp( opts[1]->token, "''" ) ) {
            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[2]->toklen;
            p = opts[2]->token;
            if( font_points( opts[2], pts ) == false ) {
                bad_cmd_line( err_invalid_font_height, p, ' ' );
            } else {
                fn = atoi( pts );
                if( fn > MAX_CENTIPOINTS ) {
                    bad_cmd_line( err_invalid_font_height, p, ' ' );
                } else {
                    g_info_lm( inf_recognized_xxx, "font height", pts );
                    new_font->height = (uint32_t) fn;
                }
            }
        } else {
            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[1]->toklen;
            p = opts[1]->token;
            if( font_points( opts[1], pts ) == false ) {
                bad_cmd_line( err_invalid_font_space, p, ' ' );
            } else {
                fn = atoi( pts );
                if( fn > MAX_CENTIPOINTS ) {
                    bad_cmd_line( err_invalid_font_space, p, ' ' );
                } else {
                    g_info_lm( inf_recognized_xxx, "font space", pts );
                    new_font->space = (uint32_t) fn;
                }
            }

            for( i = 0; i < 4; i++ ) pts[i] = '0';
            pts[4] = '\0';
            len = opts[2]->toklen;
            p = opts[2]->token;
            if( font_points( opts[2], pts ) == false ) {
                bad_cmd_line( err_invalid_font_height, p, ' ' );
            } else {
                fn = atoi( pts );
                if( fn > MAX_CENTIPOINTS ) {
                    bad_cmd_line( err_invalid_font_height, p, ' ' );
                } else {
                    g_info_lm( inf_recognized_xxx, "font height", pts );
                    new_font->height = (uint32_t) fn;
                }
            }
        }
        break;
    default:
        g_err( err_intern, __FILE__, __LINE__ );
        g_suicide();
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
        out_msg( "Font: %i %s ", new_font->font, new_font->name );
        if( new_font->style != NULL ) {
            out_msg( "%s ", new_font->style ); // can't use NULL here
        }
        out_msg( "%i %i\n", new_font->space, new_font->height );
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
    int     len;
    char    attrwork[MAX_FILE_ATTR];
    struct  laystack    * laywk;
    struct  laystack    * laywork;


    if( tokennext == NULL || tokennext->bol || is_option() == true ) {
        g_err( err_miss_inv_opt_value, opt->option, "" );
        err_count++;
    } else {
        len = tokennext->toklen;
        laywk = mem_alloc( sizeof( laystack ) + len );

        memcpy_s( laywk->layfn, len + 1, tokennext->token, len );
        *(laywk->layfn + len) = '\0';
        laywk->next = NULL;

        split_attr_file( laywk->layfn, attrwork, sizeof( attrwork ) );
        if( attrwork[0] ) {
            g_warn( wng_fileattr_ignored, attrwork, laywk->layfn );
            wng_count++;
        }
        if( lay_files == NULL ) {       // first file
            lay_files = laywk;
        } else {
            for( laywork = lay_files; laywork->next != NULL;
                 laywork = laywork->next ) {
                /*empty */;
            }
            laywork->next = laywk;
        }
        tokennext = tokennext->nxt;
    }
}

/***************************************************************************/
/*  ( output      filename or (T:1234)filename                             */
/***************************************************************************/

static void set_outfile( option * opt )
{
    int     len;
    char    attrwork[MAX_FILE_ATTR];

    if( tokennext == NULL || tokennext->bol || is_option() == true ) {
        g_err( err_miss_inv_opt_value, opt->option, "" );
        err_count++;
        out_file = NULL;
        out_file_attr = NULL;
    } else {
        len = tokennext->toklen;
        out_file = mem_alloc( len + 1 );

        memcpy_s( out_file, len + 1, tokennext->token, len );
        *(out_file + len) = '\0';

        split_attr_file( out_file, attrwork, sizeof( attrwork ) );
        if( attrwork[0] ) {
            len = 1 + strlen( attrwork );
            out_file_attr = mem_alloc( len );
            strcpy_s( out_file_attr, len, attrwork );
        } else {
            out_file_attr = NULL;
        }
        tokennext = tokennext->nxt;
    }
}


/***************************************************************************/
/*  ( passes n                                                             */
/***************************************************************************/

static void set_passes( option * opt )
{
    char    *   p;

    if( tokennext == NULL || tokennext->bol ||
        tokennext->token[0] == '(' || is_option() == true ) {

        g_err( err_missing_opt_value, opt->option );
        err_count++;
        passes = opt->value;            // set default value
    } else {
        p = tokennext->token;
        opt_value = get_num_value( p );

        if( opt_value < 1 || opt_value > MAX_PASSES ) {
            g_err( err_passes_value, str( MAX_PASSES ), opt_value );
            err_count++;
            passes = opt->value;        // set default value
        } else {
            passes = opt_value;
        }
        tokennext = tokennext->nxt;
    }
}

/***************************************************************************/
/*  ( from n   start printing at pageno n                                  */
/***************************************************************************/

static void set_from( option * opt )
{
    char    *   p;

    if( tokennext == NULL || tokennext->bol ||
        tokennext->token[0] == '(' || is_option() == true ) {

        g_err( err_missing_opt_value, opt->option );
        err_count++;
        print_from = opt->value;        // set default value
    } else {
        p = tokennext->token;
        opt_value = get_num_value( p );
        if( opt_value < 1 || opt_value >= LONG_MAX ) {
            g_err( err_out_range, "from" );
            err_count++;
            print_from = opt->value;    // set default value
        } else {
            print_from = opt_value;
        }
        tokennext = tokennext->nxt;
    }
}


/***************************************************************************/
/*  ( setsymbol  x y     set variable x to y                               */
/***************************************************************************/

static void set_symbol( option * opt )
{
    char    *   name;
    char    *   value;
    int32_t     rc;

    if( tokennext == NULL || tokennext->bol ||
        tokennext->token[0] == '(' || is_option() == true ) {

        g_err( err_missing_name, opt->option );
        err_count++;
    } else {
        name = tokennext->token;

        tokennext = tokennext->nxt;

        if( tokennext == NULL || tokennext->bol ||
            tokennext->token[0] == '(' || is_option() == true ) {

            g_err( ERR_MISSING_VALUE, opt->option );
            err_count++;
        } else {
            value = tokennext->token;
            rc = add_symvar( &global_dict, name, value, no_subscript, 0 );
            tokennext = tokennext->nxt;
        }

    }
}

/***************************************************************************/
/*  ( to n     stop  printing at pageno n                                  */
/***************************************************************************/

static void set_to( option * opt )
{
    char    *   p;

    if( tokennext == NULL || tokennext->bol ||
        tokennext->token[0] == '(' || is_option() == true ) {

        g_err( err_missing_value, opt->option );
        err_count++;
        print_to = opt->value;          // set default value
    } else {
        p = tokennext->token;
        opt_value = get_num_value( p );
        if( opt_value < 1 || opt_value >= LONG_MAX ) {
            g_err( err_out_range, "to" );
            err_count++;
            print_to = opt->value;      // set default value
        } else {
            print_to = opt_value;
        }
        tokennext = tokennext->nxt;
    }
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
/*  ( file xxx    command option file                                      */
/***************************************************************************/

static void set_OPTFile( option * opt )
{
    int         len;
    char        attrwork[MAX_FILE_ATTR];
    char    *   str;


    if( tokennext == NULL || tokennext->bol || is_option() == true
        /* || tokennext->token[0] == '('  allow (t:123)file.opt construct */
                                         ) {
        g_err( err_missing_value, opt->option );
        err_count++;
    } else {
        len = tokennext->toklen;

        str = tokennext->token;

        g_info_lm( inf_recognized_xxx, "option file", str );
        strcpy_s( token_buf, buf_size, str );
        if( try_file_name != NULL ) {
            mem_free( try_file_name );
            try_file_name = NULL;
        }
        split_attr_file( token_buf, attrwork, sizeof( attrwork ) );
        if( attrwork[0]  ) {
            g_warn( wng_fileattr_ignored, attrwork, token_buf );
            wng_count++;
        }
        if( level < MAX_NESTING ) {
            sav_tokens[level] = tokennext->nxt;

            buffers[level + 1] = NULL;
            file_names[level + 1] = NULL;
            if( search_file_in_dirs( token_buf, OPT_EXT, "", ds_opt_file ) ) {
                bool  skip = false;

                fclose( try_fp );
                try_fp = NULL;
                if( level > 0 ) {
                    int     k;

                    for( k = level; k > 0; k-- ) {
                        if( stricmp( try_file_name, file_names[k]) == 0 ) {
                            g_err( err_recursive_option, try_file_name );
                            err_count++;
                            skip = true;
                            break;
                        }
                    }
                }
                if( !skip ) {
                    file_names[++level] = try_file_name;

                    str = read_indirect_file( try_file_name );
                    split_tokens( str );
                    mem_free( str );
                    try_file_name = NULL;// free will be done via file_names[level]
                    tokennext = cmd_tokens[level];
                    return;
                }
            } else {
                g_err( err_file_not_found, token_buf );
                err_count++;
            }
            if( str == NULL )  {
                if( try_file_name != NULL ) mem_free( try_file_name );
                if( file_names[level] != NULL ) mem_free( file_names[level] );
                str = save[--level];
                tokennext = sav_tokens[level];
            }
        } else {                        // max nesting level exceeded
            if( try_file_name != NULL ) mem_free( try_file_name );
            g_err( err_max_nesting_opt, token_buf );
            err_count++;
        }
        tokennext = tokennext->nxt;
    }
};

/***************************************************************************/
/*  Processing routines for 'new format' options                           */
/***************************************************************************/

static void set_quiet( option * opt )
{
    GlobalFlags.quiet = opt->value;
    add_symvar( &global_dict, "$quiet", opt->value ? "ON" : "OFF", no_subscript,
                predefined );

};


/***************************************************************************/
/*  -r [filename] [from [to]]                                              */
/*    if only from and to specified the filename will become the           */
/*    masterfilename later (when it is known)                              */
/***************************************************************************/

static void set_research( option * opt )
{
    int         len;
    char        str[256];


    GlobalFlags.research = opt->value;

    if( tokennext == NULL || tokennext->bol || is_option() == true
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
        research_from = 1;
        research_to = ULONG_MAX - 1;

        research_file_name[0] = '\0';   // no filename
        if( isalpha( *str ) ) {         // filename ?
            if( len < sizeof( research_file_name ) ) {
                strcpy_s( research_file_name, sizeof( research_file_name ), str );
            }
            tokennext = tokennext->nxt;
            if( tokennext == NULL || tokennext->bol || is_option() == true
                    || tokennext->token[0] == '(' ) {
                /* nothing to do */
            } else {                    // get from and to values
                research_from = get_num_value( tokennext->token );
                strcat( str, " " );
                strcat( str, tokennext->token );
                tokennext = tokennext->nxt;
                if( tokennext == NULL || tokennext->bol || is_option() == true
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
            if( tokennext == NULL || tokennext->bol || is_option() == true
                    || tokennext->token[0] == '(' ) {
                /* nothing to do */
            } else {                    // get from and to values
                research_from = get_num_value( tokennext->token );
                tokennext = tokennext->nxt;
                if( tokennext == NULL || tokennext->bol || is_option() == true
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
    g_info_lm( inf_recognized_xxx, "-r", str );
}

#if 0                  // always set (w)script option, don't allow to disable
static void set_wscript( option * opt )
{
    GlobalFlags.wscript = opt->value;
};
#endif


/* struct option and logic adapted from bld\cc\c\coptions.c */

/*options in format of WGML i.e.,   ( option xxx     */

static option GML_old_Options[] =
{
/*               length-1  minimum  default  Processing             */
/*     optionname          Abbrev   value    routine      Parmcount */
    { "altextension",  11, 6,       0,       set_altext,     1 },
    { "bind",          3,  1,       0,       set_bind,       1 },
    { "cpinch",        5,  3,       10,      set_cpinch,     1 },
    { "delim",         4,  3,       0,       set_delim,      1 },
    { "device",        5,  3,       0,       set_device,     1 },
    { "description",   10, 4,       0,       ign_option,     1 },
    { "duplex",        5,  3,       0,       ign_option,     0 },
    { "file",          3,  4,       0,       set_OPTFile,    1 },
    { "font",          3,  4,       0,       set_font,       5 },
    { "fontfamily",    9,  5,       0,       ign_option,     0 },
    { "format",        5,  4,       0,       ign_option,     1 },
    { "from",          3,  4,       1,       set_from,       1 },
    { "inclist",       6,  4,       1,       set_inclist,    0 },
    { "index",         4,  3,       1,       set_index,      0 },
    { "layout",        5,  3,       0,       set_layout,     1 },
    { "linemode",      7,  4,       0,       ign_option,     0 },
    { "llength",       6,  2,       130,     ign_option,     1 },
    { "logfile",       6,  3,       0,       ign_option,     1 },
    { "lpinch",        5,  3,       6,       set_lpinch,     1 },
    { "mailmerge",     8,  4,       0,       ign_option,     1 },
    { "noduplex",      7,  5,       0,       ign_option,     0 },
    { "noinclist",     8,  6,       0,       set_inclist,    0 },
    { "noindex",       6,  5,       0,       set_index,      0 },
    { "nopause",       6,  3,       0,       ign_option,     0 },
    { "noquiet",       6,  3,       0,       set_quiet,      0 },
    { "noscript",      7,  5,       0,       wng_option,     0 },
    { "nostatistics",  11, 6,       0,       set_stats,      0 },
    { "nowait",        5,  6,       0,       ign_option,     0 },
    { "nowarning",     8,  6,       0,       ign_option,     0 },
    { "output",        5,  3,       0,       set_outfile,    1 },
    { "passes",        5,  4,       1,       set_passes,     1 },
    { "pause",         4,  5,       1,       ign_option,     0 },
    { "process",       6,  4,       0,       ign_option,     1 },
    { "quiet",         4,  5,       1,       set_quiet,      0 },
    { "resetscreen",   10, 5,       1,       ign_option,     0 },
    { "script",        5,  3,       1,       wng_option,     0 },
    { "setsymbol",     8,  3,       0,       set_symbol,     2 },
    { "statistics",    9,  4,       1,       set_stats,      0 },
    { "terse",         4,  5,       1,       ign_option,     0 },
    { "to",            1,  2,       INT_MAX, set_to,         1 },
    { "valueset",      7,  6,       0,       ign_option,     1 },
    { "verbose",       6,  4,       1,       ign_option,     0 },
    { "wait",          3,  4,       1,       ign_option,     0 },
    { "warning",       6,  4,       1,       ign_option,     0 },
    { "wscript",       6,  4,       1,       ign_option,     0 },// always set
    { NULL, 0, 0, 0, ign_option, 0 }    // end marker
};

/* options in 'new' format   -o                        */
static option GML_new_Options[] =
{
    { "q",            0,   1,       1,        set_quiet,     0 },
    { "r",            0,   1,       1,        set_research,  3 },
    { NULL, 0, 0, 0, ign_option, 0 }    // end marker
};


/***************************************************************************/
/*  split (t:200)Filename    into t:200 and Filename                       */
/***************************************************************************/

void split_attr_file( char * filename , char * attr, size_t attrlen )
{
    char    *   fn;
    char    *   p;
    size_t      k;

    fn = filename;
    p = attr;
    if( *fn == '(' ) {                  // attribute infront of filename
        k = 0;
        while( ++k < attrlen ) {
            *p++ = *++fn;               // isolate attribute
            if( *fn == ')' ) {
                break;
            }
        }
        *--p = '\0';                    // terminate attr

        p = filename;
        while( *fn != '\0' ) {          // shift filename
            *p++ = *++fn;
        }
    } else {
        *p = '\0';                      // no attr
    }
}


/***************************************************************************/
/*  test for delimiter                                                     */
/***************************************************************************/

static int option_delimiter( char c )
{
    if( c == ' ' || c == '-' || c == '\0' || c == '\t' || c == '(' ||
        c == switch_char || c == '\n' ) {
        return( 1 );
    }
    return( 0 );
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
             if( s[0] == '\\' && s[1] == '\\' ) {
                 ++s;
             } else if( s[0] == '\\' && s[1] == '"' ) {
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

static cmd_tok  *process_option( option * op_table, cmd_tok * tok )
{
    int         i;
    int         j;
    char    *   opt;
    char        c;
    char    *   option_start;
    char    *   p;


    option_start = tok->token;
    p = option_start;
    c = tolower( *p );
    if( option_delimiter( c ) ) {
        p++;
        c = tolower( *p );
    }
    tokennext = tok->nxt;
    for( i = 0; ; i++ ) {
        opt = op_table[i].option;
        j = 1;
        if( opt == NULL ) break;        // not found

        if( c == *opt ) {               // match for first char

            opt_value = op_table[i].value;
            j = 1;
            for(;;) {
                ++opt;
                if( *opt == '\0' ) {
                    if( p - option_start == 1 ) {
                                        // make sure end of option
                        if( !option_delimiter( p[j] ) ) break;
                    }
                    opt_scan_ptr = p + j;
                    g_info_lm( inf_recognized_xxx, "n1", option_start );
                    op_table[i].function( &op_table[i]);
                    return( tokennext );
                }
                c = tolower( p[j] );
                if( *opt != c ) {
                    if( *opt < 'A' || *opt > 'Z' ) break;
                    if( *opt != p[j] ) break;
                }
                ++j;
            }
        }
    }
    p = bad_cmd_line( err_invalid_option, option_start, ' ' );
    return( tokennext );
}


/***************************************************************************/
/*  process 'old' option ( xxx                                             */
/***************************************************************************/

static cmd_tok  *process_option_old( option * op_table, cmd_tok * tok )
{
    int         i;
    int         j;
    int         len;
    char    *   opt;
    char        c;
    char    *   p;
    char    *   option_start;

    p = tok->token;
    option_start = p;
    len = tok->toklen;
    tokennext = tok->nxt;
    c = tolower( *p );
    if(  c == '(' ) {
        if( len == 1 ) {
            return( tokennext );        // skip single (
        }
        p++;
        c = tolower( *p );
    }
    for( i = 0; ; i++ ) {
        opt = op_table[i].option;
        j = 1;
        if( opt == NULL ) break;
        if( c != *opt )  continue;
        if( len < op_table[i].minLength ) {
            continue;                   // cannot be this option
        }
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
        j = len;
        opt += op_table[i].optionLenM1;
        for(;;) {
            ++opt;
            if( *opt == '\0' || *opt == '*' ) {
                if( *opt == '\0' ) {
                    if( p - option_start == 1 ) {
                                        // make sure end of option
                        if( !option_delimiter( p[j] ) ) break;
                    }
                }
                opt_scan_ptr = p + j;
                g_info_lm( inf_recognized_xxx, "1", option_start );
                op_table[i].function( &op_table[i]);
                return( tokennext );
            }
            if( *opt == '#' ) {         // collect a number
                while( p[j] == ' ' ) {// skip blanks
                    ++j;
                }
                if( p[j] >= '0' && p[j] <= '9' ) {
                    opt_value = 0;
                    for(;;) {
                        c = p[j];
                        if( c < '0' || c > '9' ) break;
                        opt_value = opt_value * 10 + c - '0';
                        ++j;
                    }
                    opt_scan_ptr = p + j;
                }
                g_info_lm( inf_recognized_xxx, "num", option_start );
            } else if( *opt == '$' ) {  // collect an identifer
                if( p[j] == ' ' ) j++;// skip 1 blank

                opt_parm = &p[j];
                for(;;) {
                    c = p[j];
                    if( c == '-' ) break;
                    if( c == '(' ) break;
                    if( c == ' ' ) break;
                    if( c == switch_char ) break;
                    if( c == '\n' ) {
                        p[j] = ' ';
                        break;
                    }
                    if( c == '\0' ) break;
                    ++j;
                }
                opt_scan_ptr = p + j;
                g_info_lm( inf_recognized_xxx, "id", option_start );
            } else if( *opt == '@' ) {  // collect a filename
                opt_parm = &p[j];
                c = p[j];
                if( c == '"' ){         // "filename"
                    for(;;){
                        c = p[++j];
                        if( c == '"' ){
                            ++j;
                            break;
                        }
                        if( c == '\0' )break;
                        if( c == '\\' ){
                            ++j;
                        }
                    }
                }else{
                    for(;;) {
                        c = p[j];
                        if( c == ' ' ) break;
                        if( c == '\t' ) break;
                        if( c == switch_char ) break;
                        if( c == '\n' ) {
                            p[j] = ' ';
                            break;
                        }
                        if( c == '\0' ) break;
                        ++j;
                    }
                }
                g_info_lm( inf_recognized_xxx, "fn", option_start );
            } else if( *opt == '=' ) {  // collect an optional '='
                if( p[j] == '=' || p[j] == '#' ) ++j;
            } else {
                c = tolower( p[j] );
                if( *opt != c ) {
                    if( *opt < 'A' || *opt > 'Z' ) break;
                    if( *opt != p[j] ) break;
                }
                ++j;
                opt_scan_ptr = p + j;
            }
        }
        g_info_lm( inf_recognized_xxx, "5", option_start );
    }
    p = bad_cmd_line( err_invalid_option, option_start, '(' );
    return( tokennext );
}


/***************************************************************************/
/*  determine if tokennext is an option                                    */
/***************************************************************************/

static bool is_option( void )
{
    int         i;
    int         len;
    char    *   opt;
    char        c;
    char    *   p;
    char    *   option_start;

    if( tokennext == NULL ) return( false );
    p = tokennext->token;
    option_start = p;
    len = tokennext->toklen;
    c = tolower( *p );
    if(  c == '(' ) {
        if( len == 1 ) {            // skip single (
            tokennext = tokennext->nxt;
            p = tokennext->token;
            option_start = p;
            len = tokennext->toklen;
            c = tolower( *p );
        } else {
            p++;
            len--;
            c = tolower( *p );
        }
    }
    for( i = 0; ; i++ ) {
        opt = GML_old_Options[i].option;
        if( opt == NULL ) break;    // end of table
        if( c != *opt )  continue;  // easy disqualifiers: first char & length
        if( len < GML_old_Options[i].minLength ) continue;
        if( len > GML_old_Options[i].optionLenM1 + 1 ) continue;
        if( strnicmp( opt, p, len ) ) continue; // no match
        return( true );                         // match found
    }

    p = tokennext->token;
    option_start = p;
    len = tokennext->toklen;
    c = tolower( *p );
    if( option_delimiter( c ) ) {
        p++;
        c = tolower( *p );
    }
    for( i = 0; ; i++ ) {
        opt = GML_new_Options[i].option;
        if( opt == NULL ) break;    // end of table
        if( c != *opt )  continue;  // easy disqualifiers: first char & length
        if( len < GML_old_Options[i].minLength ) continue;
        if( len > GML_old_Options[i].optionLenM1 + 1 ) continue;
        if( strnicmp( opt, p, len ) ) continue; // no match
        return( true );                         // match found

    }
    return( false );
}


/***************************************************************************/
/*  get Document master input file name                                    */
/***************************************************************************/

static cmd_tok  *process_master_filename( cmd_tok * tok )
{
    char        attrwork[MAX_FILE_ATTR];
    char    *   p;
    char    *   str;
    int         len;

    len = tok->toklen;
    p = mem_alloc( len + 1 );
    memcpy_s( p, len + 1, tok->token, len );
    p[len] = '\0';
    g_info_lm( inf_recognized_xxx, "document source file", p );
    strip_quotes( p );
    if( master_fname != NULL ) {         // more than one master file ?
        g_banner();
        str = bad_cmd_line( err_doc_duplicate, tok->token, ' ' );
        mem_free( p );
    } else {
        split_attr_file( p , attrwork, sizeof( attrwork ) );
        if( attrwork[0]  ) {
            g_warn( wng_fileattr_ignored, attrwork, p );
            wng_count++;
            master_fname_attr = mem_alloc( 1 + strlen( attrwork ) );
            strcpy( master_fname_attr, attrwork );
        } else {
            master_fname_attr = NULL;
        }
        master_fname = p;
    }
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
    int         tokcount;
    cmd_tok *   tok;
    char    *   s_after_dq;
    char    *   p;
    bool        sol;                    // start of line switch
    char        c;
    char        linestr[MAX_L_AS_STR];
    char        linestr2[MAX_L_AS_STR];

    level = 0;                     // option file include level: 0 == cmdline
    buffers[0] = NULL;
    cmd_tokens[0] = NULL;

    while( *string == ' ' ) {
        string++;
    }
    s_after_dq = string;                // assume no starting quote
    if( *string == d_q ) {              // take care of possible quotes
        for( p = string + 1; *p; p++ )  /* empty */ ;
        p--;
        while( *p == ' ' ) {            // ignore trailing spaces
            p--;
        }
        if( *p == d_q ) {               // ending quote
            *p = '\0';                  // remove
            s_after_dq = string + 1;    // start after leading quote
        }
    }
    tokcount = split_tokens( s_after_dq );
    utoa( tokcount, linestr, 10 );
    g_info_lm( inf_cmdline_tok_cnt, linestr );

    tok = cmd_tokens[level];
    for( ; ; ) {
        while( tok != NULL ) {
            sol = tok->bol;
            c = tok->token[0];

            if( c == '-' || c == switch_char ) {

            /***************************************************************/
            /*  process 'new' options -x or /x                             */
            /***************************************************************/
                tok = process_option( GML_new_Options, tok );

                if( tok != NULL && tok->bol == false ) {

                    /*******************************************************/
                    /*  allow master filename if following 'new' option    */
                    /*******************************************************/
                    tok->bol = sol;
                }
            } else {
                if( c == '(' || !sol ) {

                    /*******************************************************/
                    /*  process 'old' options  ( xxx                       */
                    /*******************************************************/
                    tok = process_option_old( GML_old_Options, tok );
                } else {

                    /*******************************************************/
                    /*  collect Document source file name                  */
                    /*******************************************************/
                    tok = process_master_filename( tok );
                }
            }
        }
        if( buffers[level] != NULL ) {
            mem_free( buffers[level] );
            buffers[level] = NULL;
        }
        if( cmd_tokens[level] != NULL ) {
            free_tokens( level );
        }
        if( file_names[level] != NULL ) {
            mem_free( file_names[level] );
            file_names[level] = NULL;
        }
        if( level == 0 ) break;
        tok = sav_tokens[--level];
    }
    if( print_to < print_from  ) {
        g_banner();
        err_count++;
        utoa( print_from, linestr, 10 );
        utoa( print_to, linestr2, 10 );
        g_err( err_inv_page_range, linestr, linestr2 );
    }
    return( tokcount );
}
