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
* Description: WGML implement utility functions for :LAYOUT processing
*                   eat_lay_sub_tag()
*                   get_lay_sub_and_value()
*                   free_layout_banner()
*                   i_xxxx               input routines
*                   o_xxxx               output routines
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

static char  const      stryes[] =  { "yes" };
static char  const      strno[]  =  { "no" };

/***************************************************************************/
/*  document sections for banner definition                                */
/***************************************************************************/

typedef struct  ban_sections {
    char            name[12];
    size_t          len;
    ban_docsect     type;
} ban_sections;

static  const   ban_sections    doc_sections[max_ban] = {
    #define pick(e,t,s,n) { s, n, e },
    #include "bdocsect.h"
    #undef pick
};

typedef struct  content_names {
    char                name[12];
    size_t              len;
    content_enum        type;
} content_names;

static  const   content_names   content_text[max_content] =  {
    { "none",      4, no_content        },
    { "author",    6, author_content    },
    { "bothead",   7, bothead_content   },
    { "date",      4, date_content      },
    { "docnum",    6, docnum_content    },
    { "head0",     5, head0_content     },
    { "head1",     5, head1_content     },
    { "head2",     5, head2_content     },
    { "head3",     5, head3_content     },
    { "head4",     5, head4_content     },
    { "head5",     5, head5_content     },
    { "head6",     5, head6_content     },
    { "headnum0",  8, headnum0_content  },
    { "headnum1",  8, headnum1_content  },
    { "headnum2",  8, headnum2_content  },
    { "headnum3",  8, headnum3_content  },
    { "headnum4",  8, headnum4_content  },
    { "headnum5",  8, headnum5_content  },
    { "headnum6",  8, headnum6_content  },
    { "headtext0", 9, headtext0_content },
    { "headtext1", 9, headtext1_content },
    { "headtext2", 9, headtext2_content },
    { "headtext3", 9, headtext3_content },
    { "headtext4", 9, headtext4_content },
    { "headtext5", 9, headtext5_content },
    { "headtext6", 9, headtext6_content },
    { "pgnuma",    6, pgnuma_content    },
    { "pgnumad",   7, pgnumad_content   },
    { "pgnumr",    6, pgnumr_content    },
    { "pgnumrd",   7, pgnumrd_content   },
    { "pgnumc",    6, pgnumc_content    },
    { "pgnumcd",   7, pgnumcd_content   },
    { "rule",      4, rule_content      },
    { "sec",       3, sec_content       },
    { "stitle",    6, stitle_content    },
    { "title",     5, title_content     },
    { "",          0, string_content    },  // special
    { "time",      4, time_content      },
    { "tophead",   7, tophead_content   }
};


/***************************************************************************/
/*  read and ignore lines until a tag starts in col 1                      */
/*  then set reprocess switch  and return                                  */
/***************************************************************************/

void    eat_lay_sub_tag( void )
{
     while( get_line( false ) ) {
         if( *buff2 == ':' ) {
             ProcFlags.reprocess_line = true;
             break;
         }
     }
}


/***************************************************************************/
/*  parse lines like right_margin = '7i'                                   */
/*              or   right_margin='7i'                                     */
/*          and store result in att_args struct                            */
/*  rc = pos if all ok                                                     */
/*  rc = no  in case of error                                              */
/*  rc = omit if nothing found                                             */
/***************************************************************************/

condcode    get_lay_sub_and_value( att_args * args )
{
    char        *   p;
    char            quote;
    condcode        rc;

    p = scan_start;
    rc = no;

    while( is_space_tab_char( *p ) ) {  // over WS to start of name
        p++;
    }

    args->start[0] = p;
    args->len[0] = -1;                  // switch for scanning error
    args->len[1] = -1;                  // switch for scanning error

    while( *p && is_lay_att_char( *p ) ) {
        p++;
    }
    if( *p == '\0' ) {
        if( p == args->start[0] ) {
            rc = omit;                  // nothing found
        }
        return( rc );                   // or parsing error
    }
    args->len[0] = p - args->start[0];
    if( args->len[0] < 4 ) {            // attribute name length
        err_count++;
        g_err( err_att_name_inv );
        file_mac_info();
        return( rc );
    }

    while( is_space_tab_char( *p ) ) {  // over WS to =
        p++;
    }

    if(*p && *p == '=' ) {
        p++;
        while( is_space_tab_char( *p ) ) {  // over WS to attribute value
            p++;
        }
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        return( no );                   // parsing err '=' missing
    }

    args->start[1] = p;                 // delimiters must be included for error checking

    if( is_quote_char( *p ) ) {
        quote = *p;
        ++p;
        args->quoted = true;
    } else {
        quote = ' ';
        args->quoted = false;
    }

    while( *p && *p != quote ) {
        ++p;
    }

    if( args->quoted && is_quote_char( *p ) ) {
        p++;                            // over terminating quote
    }

    args->len[1] = p - args->start[1];

    if( args->len[1] < 1 ) {            // attribute value length
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
    } else {
        rc = pos;
    }

    if( *p == '.' ) {
        ProcFlags.tag_end_found = true;
        p++;
    }

    val_start = args->start[1];
    val_len = args->len[1];
    if( args->quoted) {         // delimiters must be omitted for these externs
        val_start++;
        val_len -= 2;
    }
    scan_start = p;
    return( rc );
}


/***************************************************************************/
/*  case                                                                   */
/***************************************************************************/
bool    i_case( char * p, lay_att curr, case_t * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( "mixed", p, 5 ) ) {
        *tm = case_mixed;
    } else if( !strnicmp( "lower", p, 5 ) ) {
        *tm = case_lower;
    } else if( !strnicmp( "upper", p, 5 ) ) {
        *tm = case_upper;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_case( FILE * f, lay_att curr, case_t * tm )
{
    char    * p;

    if( *tm == case_mixed ) {
        p = "mixed";
    } else if( *tm == case_lower ) {
        p = "lower";
    } else if( *tm == case_upper ) {
        p = "upper";
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  single character                                                       */
/***************************************************************************/
bool    i_char( char * p, lay_att curr, char * tm )
{
    curr = curr;
    if( is_quote_char( *p ) && (*p == *(p + 2)) ) {
        *tm = *(p + 1);                 // 2. char if quoted
    } else {
        *tm = *p;                       // else 1.
    }
    return( false );
}

void    o_char( FILE * f, lay_att curr, char * tm )
{
    fprintf( f, "        %s = '%c'\n", att_names[curr], *tm );
    return;
}


/***************************************************************************/
/*  contents for banregion    only unquoted                                */
/***************************************************************************/
bool    i_content( char * p, lay_att curr, content * tm )
{
    bool        cvterr;
    int         k;

    curr = curr;
    cvterr = false;
    tm->content_type = no_content;
    for( k = no_content; k < max_content; ++k ) {
        if( !strnicmp( content_text[k].name, p, content_text[k].len ) ) {
            tm->content_type = k;
            strcpy( tm->string, content_text[k].name );
            break;
        }
    }
    if( k >= max_content ) {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_content( FILE * f, lay_att curr, content * tm )
{
    const   char    * p;
    char            c   = '\0';

    if( tm->content_type >= no_content && tm->content_type < max_content) {
        p = tm->string;
        if( tm->content_type == string_content ) { // user string with quotes
            fprintf( f, "        %s = '", att_names[curr] );
            while( (c = *p++) != '\0' ) {
                if( c == '&' ) {
                    fprintf( f, "&$amp." );
                } else {
                    fputc( c, f );
                }
            }
            fputc( '\'', f );
            fputc( '\n', f );
            return;
        }
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  default frame                                                          */
/***************************************************************************/
bool    i_default_frame( char * p, lay_att curr, def_frame * tm )
{
    bool        cvterr;
    int         len;

    curr = curr;
    cvterr = false;
    if( !strnicmp( "none", p, 4 ) ) {
        tm->type = none;
    } else if( !strnicmp( "rule", p, 4 ) ) {
        tm->type = rule_frame;
    } else if( !strnicmp( "box", p, 3 ) ) {
        tm->type = box_frame;
    } else if( !is_quote_char( *p ) ) {
        cvterr = true;
    } else {
        len = strlen( p );
        if( *p != *(p + len - 1) ) {
            cvterr = true;  // string not terminated
        } else {
            if( sizeof( tm->string ) > len - 2 ) {
                *(p + len - 1 ) = '\0';
                strcpy( tm->string, p + 1 );
                tm->type = char_frame;
            } else {
                cvterr = true; // string too long;
            }
        }
    }
    if( cvterr ) {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
    }
    return( cvterr );

}

void    o_default_frame( FILE * f, lay_att curr, def_frame * tm )
{

    switch( tm->type ) {
    case   none:
        fprintf( f, "        %s = none\n", att_names[curr] );
        break;
    case   rule_frame:
        fprintf( f, "        %s = rule\n", att_names[curr] );
        break;
    case   box_frame:
        fprintf( f, "        %s = box\n", att_names[curr] );
        break;
    case   char_frame:
        fprintf( f, "        %s = '%s'\n", att_names[curr], tm->string );
        break;
    default:
        fprintf( f, "        %s = ???\n", att_names[curr] );
        break;
    }
    return;
}




/***************************************************************************/
/*  docsect  refdoc                                                        */
/***************************************************************************/
bool    i_docsect( char * p, lay_att curr, ban_docsect * tm )
{
    bool        cvterr;
    int         k;

    curr = curr;
    cvterr = false;
    *tm = no_ban;
    for( k = no_ban; k < max_ban; ++k ) {
        if( !strnicmp( doc_sections[k].name, p, doc_sections[k].len ) ) {
            *tm = doc_sections[k].type;
            break;
        }
    }
    if( *tm == no_ban ) {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_docsect( FILE * f, lay_att curr, ban_docsect * tm )
{
    const   char    * p;

    if( *tm >= no_ban && *tm < max_ban) {
        p = doc_sections[*tm].name;
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  frame  rule or none                                                    */
/***************************************************************************/
bool    i_frame( char * p, lay_att curr, bool * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( "none", p, 4 ) ) {
        *tm = false;
    } else if( !strnicmp( "rule", p, 4 ) ) {
        *tm = true;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );

}

void    o_frame( FILE * f, lay_att curr, bool * tm )
{
    char    * p;

    if( *tm ) {
        p = "rule";
    } else {
        p = "none";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  integer routines                                                       */
/***************************************************************************/
bool    i_int32( char * p, lay_att curr, int32_t * tm )
{
    curr = curr;

    *tm = strtol( p, NULL, 10 );
    return( false );
}

void    o_int32( FILE * f, lay_att curr, int32_t * tm )
{

    fprintf( f, "        %s = %ld\n", att_names[curr], *tm );
    return;
}

bool    i_int8( char * p, lay_att curr, int8_t * tm )
{
    int32_t     wk;

    curr = curr;
    wk = strtol( p, NULL, 10 );
    if( abs( wk ) > 255 ) {
        return( true );
    }
    *tm = wk;
    return( false );
}

void    o_int8( FILE * f, lay_att curr, int8_t * tm )
{
    int     wk = *tm;

    fprintf( f, "        %s = %d\n", att_names[curr], wk );
    return;
}


bool    i_uint8( char *p, lay_att curr, uint8_t *tm )
{
    int wk;

    curr = curr;
    wk = strtol( p, NULL, 10 );
    if( wk < 0 || wk > 255 ) {
        return( true );
    }
    *tm = wk;
    return( false );
}

void    o_uint8( FILE * f, lay_att curr, uint8_t *tm )
{
    unsigned wk = *tm;

    fprintf( f, "        %s = %u\n", att_names[curr], wk );
    return;
}


/***************************************************************************/
/*  font number                                                            */
/***************************************************************************/
bool    i_font_number( char *p, lay_att curr, font_number *tm )
{
    return( i_uint8( p, curr, tm ) );
}

void    o_font_number( FILE * f, lay_att curr, font_number *tm )
{
    o_uint8( f, curr, tm );
}


/***************************************************************************/
/*  number form                                                            */
/***************************************************************************/
bool    i_number_form( char * p, lay_att curr, num_form * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( "none", p, 4 ) ) {
        *tm = num_none;
    } else if( !strnicmp( "prop", p, 4 ) ) {
        *tm = num_prop;
    } else if( !strnicmp( "new", p, 3 ) ) {
        *tm = num_new;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_number_form( FILE * f, lay_att curr, num_form * tm )
{
    char    * p;

    if( *tm == num_none ) {
        p = "none";
    } else if( *tm == num_prop ) {
        p = "prop";
    } else if( *tm == num_new ) {
        p = "new";
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  number style                                                           */
/***************************************************************************/
bool    i_number_style( char * p, lay_att curr, num_style * tm )
{
    bool        cvterr;
    num_style   wk = 0;
    char        c;

    curr = curr;
    cvterr = false;
    c = tolower( *p );
    switch( c ) {                       // first letter
    case   'a':
        wk |= a_style;
        break;
    case   'b':
        wk |= b_style;
        break;
    case   'c':
        wk |= c_style;
        break;
    case   'r':
        wk |= r_style;
        break;
    case   'h':
        wk |= h_style;
        break;
    default:
        cvterr = true;
        break;
    }

    p++;
    if( !cvterr && *p ) {               // second letter
        c = tolower( *p );
        switch( c ) {
        case   'd':
            wk |= xd_style;
            break;
        case   'p':
            p++;
            if( *p ) {                  // third letter
                c = tolower( *p );
                switch( c ) {
                case   'a':
                    wk |= xpa_style;    // only left parenthesis
                    break;
                case   'b':
                    wk |= xpb_style;    // only right parenthesis
                    break;
                default:
                    cvterr = true;
                    break;
                }
            } else {
                wk |= xp_style;         // left and right parenthesis
            }
            break;
        default:
            cvterr = true;
            break;
        }
    }
    if( !cvterr ) {
        *tm = wk;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
    }
    return( cvterr );
}

void    o_number_style( FILE * f, lay_att curr, num_style * tm )
{
    char        str[4];
    char    *    p;

    p = str;
    if( *tm & h_style ) {
        *p = 'h';
        p++;
    } else if( *tm & a_style ) {
        *p = 'a';
        p++;
    } else if( *tm & b_style ) {
        *p = 'b';
        p++;
    } else if( *tm & c_style ) {
        *p = 'c';
        p++;
    } else if( *tm & r_style ) {
        *p = 'r';
        p++;
    }
    *p = '\0';

    if( *tm & xd_style ) {
        *p = 'd';
        p++;
    } else if( (*tm & xp_style) == xp_style) {
        *p = 'p';
        p++;
    } else if( *tm & xpa_style ) {
        *p = 'p';
        p++;
        *p = 'a';
        p++;
    } else if( *tm & xpb_style ) {
        *p = 'p';
        p++;
        *p = 'b';
        p++;
    }
    *p = '\0';
    fprintf( f, "        %s = %s\n", att_names[curr], str );
    return;
}


/***************************************************************************/
/*  page eject                                                             */
/***************************************************************************/
bool    i_page_eject( char * p, lay_att curr, page_ej * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( strno, p, 2 ) ) {
        *tm = ej_no;
    } else if( !strnicmp( stryes, p, 3 ) ) {
        *tm = ej_yes;
    } else if( !strnicmp( "odd", p, 3 ) ) {
        *tm = ej_odd;
    } else if( !strnicmp( "even", p, 4 ) ) {
        *tm = ej_even;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_page_eject( FILE * f, lay_att curr, page_ej * tm )
{
    const   char    *   p;

    if( *tm == ej_no ) {
        p = strno;
    } else if( *tm == ej_yes ) {
        p = stryes;
    } else if( *tm == ej_odd ) {
        p = "odd";
    } else if( *tm == ej_even ) {
        p = "even";
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  page position                                                          */
/***************************************************************************/
bool    i_page_position( char * p, lay_att curr, page_pos * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( "left", p, 4 ) ) {
        *tm = pos_left;
    } else if( !strnicmp( "right", p, 5 ) ) {
        *tm = pos_right;
    } else if( !(strnicmp( "centre", p, 6 ) && strnicmp( "center", p, 6 )) ) {
        *tm = pos_center;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_page_position( FILE * f, lay_att curr, page_pos * tm )
{
    char    * p;

    if( *tm == pos_left ) {
        p = "left";
    } else if( *tm == pos_right ) {
        p = "right";
    } else if( *tm == pos_centre ) {
        p = "centre";
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  place                                                                  */
/***************************************************************************/
bool    i_place( char * p, lay_att curr, bf_place * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( "topeven", p, 7 ) ) {
        *tm = topeven_place;
    } else if( !strnicmp( "bottom", p, 6 ) ) {
        *tm = bottom_place;
    } else if( !strnicmp( "inline", p, 6 ) ) {
        *tm = inline_place;
    } else if( !strnicmp( "topodd", p, 6 ) ) {
        *tm = topodd_place;
    } else if( !strnicmp( "top", p, 3 ) ) {// check for top later than topXXX
        *tm = top_place;
    } else if( !strnicmp( "botodd", p, 6 ) ) {
        *tm = botodd_place;
    } else if( !strnicmp( "boteven", p, 7 ) ) {
        *tm = boteven_place;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_place( FILE * f, lay_att curr, bf_place * tm )
{
    char    * p;

    if( *tm == top_place ) {
        p = "top";
    } else if( *tm == bottom_place ) {
        p = "bottom";
    } else if( *tm == inline_place ) {
        p = "inline";
    } else if( *tm == topodd_place ) {
        p = "topodd";
    } else if( *tm == topeven_place ) {
        p = "topeven";
    } else if( *tm == botodd_place ) {
        p = "botodd";
    } else if( *tm == boteven_place ) {
        p = "boteven";
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  pouring                                                                */
/***************************************************************************/
bool    i_pouring( char * p, lay_att curr, reg_pour * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( "none", p, 4 ) ) {
        *tm = no_pour;
    } else if( !strnicmp( "last", p, 4 ) ) {
        *tm = last_pour;
    } else if( !strnicmp( "head0", p, 5 ) ) {
        *tm = head0_pour;
    } else if( !strnicmp( "head1", p, 5 ) ) {
        *tm = head1_pour;
    } else if( !strnicmp( "head2", p, 5 ) ) {
        *tm = head2_pour;
    } else if( !strnicmp( "head3", p, 5 ) ) {
        *tm = head3_pour;
    } else if( !strnicmp( "head4", p, 5 ) ) {
        *tm = head4_pour;
    } else if( !strnicmp( "head5", p, 5 ) ) {
        *tm = head5_pour;
    } else if( !strnicmp( "head6", p, 5 ) ) {
        *tm = head6_pour;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_pouring( FILE * f, lay_att curr, reg_pour * tm )
{
    char    * p;

    if( *tm == no_pour ) {
        p = "none";
    } else if( *tm == last_pour ) {
        p = "last";
    } else if( *tm == head0_pour) {
        p = "head0";
    } else if( *tm == head1_pour) {
        p = "head1";
    } else if( *tm == head2_pour) {
        p = "head2";
    } else if( *tm == head3_pour) {
        p = "head3";
    } else if( *tm == head4_pour) {
        p = "head4";
    } else if( *tm == head5_pour) {
        p = "head5";
    } else if( *tm == head6_pour) {
        p = "head6";
    } else {
        p = "???";
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  Space unit                                                             */
/***************************************************************************/
bool    i_space_unit( char * p, lay_att curr, su * tm )
{
    p = p; curr = curr;
    return( att_val_to_su( tm, true ) );    // no negative values allowed TBD
}

void    o_space_unit( FILE * f, lay_att curr, su * tm )
{

    if( tm->su_u == SU_chars_lines || tm->su_u == SU_undefined ||
        tm->su_u >= SU_lay_left ) {
        fprintf( f, "        %s = %s\n", att_names[curr], tm->su_txt );
    } else {
        fprintf( f, "        %s = '%s'\n", att_names[curr], tm->su_txt );
    }
    return;
}


/***************************************************************************/
/*  xx_string  for :NOTE and others                                        */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool    i_xx_string( char * p, lay_att curr, xx_str * tm )
{
    bool        cvterr;

    p = p; curr = curr;
    cvterr = false;
    if( (val_start != NULL) && (val_len < str_size) ) {
        memcpy( tm, val_start, val_len );
        tm[val_len] = '\0';
    } else {
        cvterr = true;
    }
    return( cvterr );
}

void    o_xx_string( FILE * f, lay_att curr, xx_str * tm )
{

    fprintf( f, "        %s = \"%s\"\n", att_names[curr], tm );
    return;
}

/***************************************************************************/
/*  date_form      stored as string perhaps better other type    TBD       */
/***************************************************************************/
bool    i_date_form( char * p, lay_att curr, xx_str * tm )
{
    return( i_xx_string( p, curr, tm ) );
}

void    o_date_form( FILE * f, lay_att curr, xx_str * tm )
{
    o_xx_string( f, curr, tm );
}

/***************************************************************************/
/*  Yes or No  as bool result                                              */
/***************************************************************************/
bool    i_yes_no( char * p, lay_att curr, bool * tm )
{
    bool        cvterr;

    curr = curr;
    cvterr = false;
    if( !strnicmp( strno, p, 2 ) ) {
        *tm = false;
    } else if( !strnicmp( stryes, p, 3 ) ) {
        *tm = true;
    } else {
        err_count++;
        g_err( err_att_val_inv );
        file_mac_info();
        cvterr = true;
    }
    return( cvterr );
}

void    o_yes_no( FILE * f, lay_att curr, bool * tm )
{
    char    const   *   p;

    if( *tm == 0 ) {
        p = strno;
    } else {
        p = stryes;
    }
    fprintf( f, "        %s = %s\n", att_names[curr], p );
    return;
}

