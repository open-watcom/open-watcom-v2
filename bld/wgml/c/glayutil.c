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
* Description: WGML implement utility functions for :LAYOUT processing
*                   eat_lay_sub_tag()
*                   free_layout()
*                   i_xxxx               input routines
*                   o_xxxx               output routines
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


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
    /* The "d" forms must be first or they will never be found */
    { "pgnumad",   7, pgnumad_content   },
    { "pgnuma",    6, pgnuma_content    },
    { "pgnumrd",   7, pgnumrd_content   },
    { "pgnumr",    6, pgnumr_content    },
    { "pgnumcd",   7, pgnumcd_content   },
    { "pgnumc",    6, pgnumc_content    },
    { "rule",      4, rule_content      },
    { "sec",       3, sec_content       },
    { "stitle",    6, stitle_content    },
    { "title",     5, title_content     },
    { "time",      4, time_content      },
    { "tophead",   7, tophead_content   },
    /* Must be last: will match any following entries */
    { "",          0, string_content    },  // special
};

/***************************************************************************/
/*  document sections for banner definition                                */
/***************************************************************************/

const   ban_sections    doc_sections[max_ban] = {
    { "???",      3, no_ban        },
    { "abstract", 8, abstract_ban  },
    { "appendix", 8, appendix_ban  },
    { "backm",    5, backm_ban     },
    { "body",     4, body_ban      },
    { "figlist",  7, figlist_ban   },
    { "index",    5, index_ban     },
    { "preface",  7, preface_ban   },
    { "toc",      3, toc_ban       },
    { "head0",    5, head0_ban     },
    { "head1",    5, head1_ban     },
    { "head2",    5, head2_ban     },
    { "head3",    5, head3_ban     },
    { "head4",    5, head4_ban     },
    { "head5",    5, head5_ban     },
    { "head6",    5, head6_ban     },
    { "letfirst", 8, letfirst_ban  },
    { "letlast",  7, letlast_ban   },
    { "letter",   6, letter_ban    },
};


/***************************************************************************/
/*  place names for fig and banner definition                              */
/***************************************************************************/

const   ban_places    bf_places[max_place] = {
    { "???",      3, no_place      },
    { "inline",   6, inline_place  },
    { "bottom",   6, bottom_place  },
    { "botodd",   6, botodd_place  },
    { "boteven",  7, boteven_place },
    { "topodd",   6, topodd_place  },
    { "topeven",  7, topeven_place },
    { "top",      3, top_place     },   // must follow or topodd/topeven are never found
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
/*          and store result in g_att_val                                  */
/*  rc = pos if all ok                                                     */
/*  rc = no  in case of error                                              */
/*  rc = omit if nothing found                                             */
/***************************************************************************/

condcode    get_attr_and_value( void )
{
    char        *   p;
    char        *   pa;
    char            quote;
    condcode        rc;

    p = scan_start;
    pa = p;
    rc = no;

    SkipSpacesTabs( p );                    // over WS to start of name

    g_att_val.att_name = p;
    g_att_val.att_len = -1;                 // switch for scanning error
    g_att_val.val_len = -1;                 // switch for scanning error

    for(;;) {                               // loop until attribute/value pair or rescan line found
        while( is_att_char( *p ) ) {
            p++;
        }
        if( *p == '\0' ) {                  // end of line: get new line
            if( !(input_cbs->fmflags & II_eof) ) {
                if( get_line( true ) ) {    // next line for missing attribute

                    process_line();
                    scan_start = buff2;
                    scan_stop  = buff2 + buff2_lg;
                    if( (*scan_start == SCR_char) ||    // cw found: end-of-tag
                        (*scan_start == GML_char) ) {   // tag found: end-of-tag
                        ProcFlags.reprocess_line = true;
                        break;
                    } else {
                        p = scan_start;                 // new line is part of current tag
                        SkipSpacesTabs( p );            // over WS to start of alleged attribute
                        g_att_val.att_name = p;         // set for new line
                        continue;
                    }
                }
            }
        }
        g_att_val.att_len = p - g_att_val.att_name;
        if( *p == '.' ) {                   // end of tag
            ProcFlags.tag_end_found = true;
            return( omit );
        }
        if( g_att_val.att_len < 4 ) {       // attribute name length
            xx_line_err_c( err_att_name_inv, pa );
        }
        SkipSpacesTabs( p );                // over WS to =
        if( *p == '=' ) {
            p++;
            SkipSpacesTabs( p );            // over WS to attribute value
            if( *p == '.' ) {               // final "." is end of tag
                xx_line_err_c( err_att_val_missing, p );
            }
        } else {                            // equals sign is required
            xx_line_err_c( err_eq_missing, p );
        }

        g_att_val.val_name = p;             // delimiters must be included for error checking
        pa = p;

        if( is_quote_char( *p ) ) {
            quote = *p;
            ++p;
            g_att_val.val_quoted = true;
        } else {
            quote = ' ';
            g_att_val.val_quoted = false;
        }

        while( *p != '\0' && *p != quote ) {
            ++p;
        }

        if( g_att_val.val_quoted && is_quote_char( *p ) ) {
            p++;                            // over terminating quote
        }

        g_att_val.val_len = p - g_att_val.val_name;

        if( g_att_val.val_len < 1 ) {       // attribute value length
            xx_line_err_c( err_att_val_missing, pa );
        } else {
            rc = pos;
        }

        if( *(p - 1) == '.' ) {             // final "." is end of tag
            ProcFlags.tag_end_found = true;
            g_att_val.val_len--;            // remove final "." from value
        }

        val_start = g_att_val.val_name;
        val_len = g_att_val.val_len;
        if( g_att_val.val_quoted) {         // delimiters must be omitted for these externs
            val_start++;
            val_len -= 2;
        }
        break;                              // values found
    }

    scan_start = p;
    return( rc );
}


/***************************************************************************/
/* free_layout  free list levels, banners and banregions                   */
/***************************************************************************/
void    free_layout( void )
{
    banner_lay_tag  * ban;
    banner_lay_tag  * ban1;
    ban_reg_group   * br_gp;
    dl_lay_level    * dl_layout;
    gl_lay_level    * gl_layout;
    ol_lay_level    * ol_layout;
    region_lay_tag  * reg;
    sl_lay_level    * sl_layout;
    ul_lay_level    * ul_layout;

    while( layout_work.dl.first != NULL ) {
        dl_layout = layout_work.dl.first;
        layout_work.dl.first = layout_work.dl.first->next;
        mem_free( dl_layout );
    }

    while( layout_work.gl.first != NULL ) {
        gl_layout = layout_work.gl.first;
        layout_work.gl.first = layout_work.gl.first->next;
        mem_free( gl_layout );
    }

    while( layout_work.ol.first != NULL ) {
        ol_layout = layout_work.ol.first;
        layout_work.ol.first = layout_work.ol.first->next;
        mem_free( ol_layout );
    }

    while( layout_work.sl.first != NULL ) {
        sl_layout = layout_work.sl.first;
        layout_work.sl.first = layout_work.sl.first->next;
        mem_free( sl_layout );
    }

    while( layout_work.ul.first != NULL ) {
        ul_layout = layout_work.ul.first;
        layout_work.ul.first = layout_work.ul.first->next;
        mem_free( ul_layout );
    }

    ban = layout_work.banner;
    while( ban != NULL ) {
        reg = ban->region;
        while( reg != NULL ) {
            ban->region = reg->next;
            mem_free( reg );
            reg = ban->region;
        }
        br_gp = ban->by_line;
        while( br_gp != NULL ) {
            reg = br_gp->first;
            while( reg != NULL ) {
                br_gp->first = reg->next;
                if( reg->final_content[0].string != NULL ) {
                    mem_free( reg->final_content[0].string );
                }
                if( reg->final_content[1].string != NULL ) {
                    mem_free( reg->final_content[1].string );
                }
                if( reg->final_content[2].string != NULL ) {
                    mem_free( reg->final_content[2].string );
                }
                mem_free( reg );
                reg = br_gp->first;
            }
            ban->by_line = br_gp->next;
            mem_free( br_gp );
            br_gp = ban->by_line;
        }
        ban1 = ban->next;
        mem_free( ban );
        ban = ban1;
    }
}


/***************************************************************************/
/*  case                                                                   */
/***************************************************************************/
bool    i_case( char * p, lay_att curr, case_t * tm )
{
    bool        cvterr;

    (void)curr;

    cvterr = false;
    if( strnicmp( "mixed", p, 5 ) == 0 ) {
        *tm = case_mixed;
    } else if( strnicmp( "lower", p, 5 ) == 0 ) {
        *tm = case_lower;
    } else if( strnicmp( "upper", p, 5 ) == 0 ) {
        *tm = case_upper;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_case( FILE *fp, lay_att curr, const case_t * tm )
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
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  single character                                                       */
/*  UL bullet entered as '' is treated as ' ' by wgml 4.0, so all may be   */
/***************************************************************************/
bool    i_char( char * p, lay_att curr, char * tm )
{
    (void)curr;

    if( is_quote_char( *p ) && (*p == *(p + 2)) ) {
        *tm = *(p + 1);                 // 2nd char if quoted
    } else if( is_quote_char( *p ) && (*p == *(p + 1)) ) {
        *tm = ' ';                      // space if '' or ""
    } else {
        *tm = *p;                       // else 1st char
    }
    return( false );
}

void    o_char( FILE *fp, lay_att curr, const char * tm )
{
    fprintf( fp, "        %s = '%c'\n", att_names[curr], *tm );
    return;
}


/***************************************************************************/
/*  contents for banregion    only unquoted                                */
/***************************************************************************/
bool    i_content( char * p, lay_att curr, content * tm )
{
    bool        cvterr;
    char    *   pa;
    int         k;
    size_t      len;

    (void)curr;

    cvterr = false;
    tm->content_type = no_content;
    for( k = no_content; k < max_content; ++k ) {
        if( strnicmp( content_text[k].name, p, content_text[k].len ) == 0 ) {
            tm->content_type = content_text[k].type;
            strcpy( tm->string, content_text[k].name );
            break;
        }
    }
    if( tm->content_type == string_content ) {  // unquoted single word
        pa = p;
        SkipNonSpaces( pa );
        len = pa - p;
        if( len > STRBLK_SIZE )
            len = STRBLK_SIZE;                     // truncate to allowed length
        strncpy( tm->string, p, len );
        tm->string[len] = '\0';
    }
    return( cvterr );
}

void    o_content( FILE *fp, lay_att curr, const content * tm )
{
    const   char    * p;
    char              c;

    if( tm->content_type >= no_content && tm->content_type < max_content) {
        p = tm->string;
        if( tm->content_type == string_content ) { // user string with quotes
            fprintf( fp, "        %s = '", att_names[curr] );
            while( (c = *p++) != '\0' ) {
                if( c == '&' ) {
                    fprintf( fp, "&$amp." );
                } else {
                    fputc( c, fp );
                }
            }
            fputc( '\'', fp );
            fputc( '\n', fp );
            return;
        }
    } else {
        p = "???";
    }
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  default frame                                                          */
/***************************************************************************/
bool    i_default_frame( char * p, lay_att curr, def_frame * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strnicmp( "none", p, 4 ) == 0 ) {
        tm->type = none;
    } else if( strnicmp( "rule", p, 4 ) == 0 ) {
        tm->type = rule_frame;
    } else if( strnicmp( "box", p, 3 ) == 0 ) {
        tm->type = box_frame;
    } else if( !is_quote_char( *p ) ) {
        cvterr = true;
    } else {
        if( val_len == 0 ) {        // empty string entered
            tm->type = none;        // should work for both FIG and IXHEAD
        } else {                    // string value entered
            i_xx_string( p, curr, tm->string );
            tm->type = char_frame;
        }
    }
    if( cvterr ) {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );

}

void    o_default_frame( FILE *fp, lay_att curr, const def_frame * tm )
{

    switch( tm->type ) {
    case   none:
        fprintf( fp, "        %s = none\n", att_names[curr] );
        break;
    case   rule_frame:
        fprintf( fp, "        %s = rule\n", att_names[curr] );
        break;
    case   box_frame:
        fprintf( fp, "        %s = box\n", att_names[curr] );
        break;
    case   char_frame:
        fprintf( fp, "        %s = '%s'\n", att_names[curr], tm->string );
        break;
    default:
        fprintf( fp, "        %s = ???\n", att_names[curr] );
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

    (void)curr;

    cvterr = false;
    *tm = no_ban;
    for( k = no_ban; k < max_ban; ++k ) {
        if( strnicmp( doc_sections[k].name, p, doc_sections[k].len ) == 0 ) {
            *tm = doc_sections[k].type;
            break;
        }
    }
    if( *tm == no_ban ) {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_docsect( FILE *fp, lay_att curr, const ban_docsect * tm )
{
    const   char    * p;

    if( *tm >= no_ban && *tm < max_ban) {
        p = doc_sections[*tm].name;
    } else {
        p = "???";
    }
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  frame  rule or none                                                    */
/***************************************************************************/
bool    i_frame( char * p, lay_att curr, bool * tm )
{
    bool        cvterr;

    (void)curr;

    cvterr = false;
    if( strnicmp( "none", p, 4 ) == 0 ) {
        *tm = false;
    } else if( strnicmp( "rule", p, 4 ) == 0 ) {
        *tm = true;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );

}

void    o_frame( FILE *fp, lay_att curr, const bool * tm )
{
    char    * p;

    if( *tm ) {
        p = "rule";
    } else {
        p = "none";
    }
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  integer routines                                                       */
/***************************************************************************/
bool    i_int32( char * p, lay_att curr, int32_t * tm )
{
    long    wk;

    (void)curr;

    wk = strtol( p, NULL, 10 );
    *tm = wk;
    return( false );
}

void    o_int32( FILE *fp, lay_att curr, const int32_t * tm )
{

    fprintf( fp, "        %s = %d\n", att_names[curr], *tm );
    return;
}

bool    i_int8( char * p, lay_att curr, int8_t * tm )
{
    long    wk;

    (void)curr;

    wk = strtol( p, NULL, 10 );
    if( abs( wk ) > 255 ) {
        xx_line_err_c( err_i_8, p );
    }
    *tm = wk;
    return( false );
}

void    o_int8( FILE *fp, lay_att curr, const int8_t * tm )
{
    int     wk = *tm;

    fprintf( fp, "        %s = %d\n", att_names[curr], wk );
    return;
}


/***************************************************************************/
/*  font number                                                            */
/***************************************************************************/
bool    i_font_number( char *p, lay_att curr, font_number *tm )
{
    char    *   pb;
    size_t      len;

    (void)curr;

    pb = p;
    len = 0;
    while( *pb != '\0' && !is_space_tab_char( *pb ) && *pb != '.' ) {   // get length
        len++;
        pb++;
    }
    *tm = get_font_number( p, len );
    return( false );
}

void    o_font_number( FILE *fp, lay_att curr, const font_number *tm )
{
    fprintf( fp, "        %s = %u\n", att_names[curr], (unsigned)*tm );
}


/***************************************************************************/
/*  number form                                                            */
/***************************************************************************/
bool    i_number_form( char * p, lay_att curr, num_form * tm )
{
    bool        cvterr;

    (void)curr;

    cvterr = false;
    if( strnicmp( "none", p, 4 ) == 0 ) {
        *tm = num_none;
    } else if( strnicmp( "prop", p, 4 ) == 0 ) {
        *tm = num_prop;
    } else if( strnicmp( "new", p, 3 ) == 0 ) {
        *tm = num_new;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_number_form( FILE *fp, lay_att curr, const num_form * tm )
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
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
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

    (void)curr;

    cvterr = false;
    c = my_tolower( *p );
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
    if( !cvterr && *p != '\0' && (*p != ' ') ) {    // second letter
        c = my_tolower( *p );
        switch( c ) {
        case   'd':
            wk |= xd_style;
            break;
        case   'p':
            p++;
            if( *p != '\0' && (*p != ' ') ) {   // third letter
                c = my_tolower( *p );
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
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_number_style( FILE *fp, lay_att curr, const num_style * tm )
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
    fprintf( fp, "        %s = %s\n", att_names[curr], str );
    return;
}


/***************************************************************************/
/*  page eject                                                             */
/***************************************************************************/
bool    i_page_eject( char * p, lay_att curr, page_ej * tm )
{
    bool        cvterr;

    (void)curr;

    cvterr = false;
    if( strnicmp( "no", p, 2 ) == 0 ) {
        *tm = ej_no;
    } else if( strnicmp( "yes", p, 3 ) == 0 ) {
        *tm = ej_yes;
    } else if( strnicmp( "odd", p, 3 ) == 0 ) {
        *tm = ej_odd;
    } else if( strnicmp( "even", p, 4 ) == 0 ) {
        *tm = ej_even;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_page_eject( FILE *fp, lay_att curr, const page_ej * tm )
{
    const   char    *   p;

    if( *tm == ej_no ) {
        p = "no";
    } else if( *tm == ej_yes ) {
        p = "yes";
    } else if( *tm == ej_odd ) {
        p = "odd";
    } else if( *tm == ej_even ) {
        p = "even";
    } else {
        p = "???";
    }
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  page position                                                          */
/***************************************************************************/
bool    i_page_position( char * p, lay_att curr, page_pos * tm )
{
    bool        cvterr;

    (void)curr;

    cvterr = false;
    if( strnicmp( "left", p, 4 ) == 0 ) {
        *tm = pos_left;
    } else if( strnicmp( "right", p, 5 ) == 0 ) {
        *tm = pos_right;
    } else if( (strnicmp( "centre", p, 6 ) == 0 || strnicmp( "center", p, 6 ) == 0 ) ) {
        *tm = pos_center;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_page_position( FILE *fp, lay_att curr, const page_pos * tm )
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
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  place                                                                  */
/***************************************************************************/
bool    i_place( char * p, lay_att curr, bf_place * tm )
{
    bool        cvterr;
    int         k;

    (void)curr;

    cvterr = false;
    *tm = no_place;
    for( k = no_place; k < max_place; ++k ) {
        if( strnicmp( bf_places[k].name, p, bf_places[k].len ) == 0 ) {
            *tm = bf_places[k].type;
            break;
        }
    }
    if( *tm == no_place ) {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_place( FILE *fp, lay_att curr, const bf_place * tm )
{
    const   char    *   p;
            int         k;

    p = "???";          // desperation value
    for( k = no_place; k < max_place; ++k ) {
        if( bf_places[k].type == *tm ) {
            p = bf_places[k].name;
        }
    }
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  pouring                                                                */
/***************************************************************************/
bool    i_pouring( char * p, lay_att curr, reg_pour * tm )
{
    bool        cvterr;

    (void)curr;

    cvterr = false;
    if( strnicmp( "none", p, 4 ) == 0 ) {
        *tm = no_pour;
    } else if( strnicmp( "last", p, 4 ) == 0 ) {
        *tm = last_pour;
    } else if( strnicmp( "head0", p, 5 ) == 0 ) {
        *tm = head0_pour;
    } else if( strnicmp( "head1", p, 5 ) == 0 ) {
        *tm = head1_pour;
    } else if( strnicmp( "head2", p, 5 ) == 0 ) {
        *tm = head2_pour;
    } else if( strnicmp( "head3", p, 5 ) == 0 ) {
        *tm = head3_pour;
    } else if( strnicmp( "head4", p, 5 ) == 0 ) {
        *tm = head4_pour;
    } else if( strnicmp( "head5", p, 5 ) == 0 ) {
        *tm = head5_pour;
    } else if( strnicmp( "head6", p, 5 ) == 0 ) {
        *tm = head6_pour;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_pouring( FILE *fp, lay_att curr, const reg_pour * tm )
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
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}


/***************************************************************************/
/*  space unit                                                             */
/***************************************************************************/
bool    i_space_unit( char * p, lay_att curr, su * tm )
{
    (void)p;
    (void)curr;

    return( att_val_to_su( tm, true ) );    // no negative values allowed TBD
}

void    o_space_unit( FILE *fp, lay_att curr, const su * tm )
{

    if( tm->su_u == SU_chars_lines || tm->su_u == SU_undefined ||
        tm->su_u >= SU_lay_left ) {
        fprintf( fp, "        %s = %s\n", att_names[curr], tm->su_txt );
    } else {
        fprintf( fp, "        %s = '%s'\n", att_names[curr], tm->su_txt );
    }
    return;
}


/***************************************************************************/
/*  spacing                                                                */
/***************************************************************************/
bool    i_spacing( char *p, lay_att curr, text_space *tm )
{
    long wk;

    curr = curr;
    wk = strtol( p, NULL, 10 );
    if( wk < 0 || wk > 255 ) {
        xx_line_err_c( err_ui_8, p );
    }
    *tm = wk;
    return( false );
}

void    o_spacing( FILE *fp, lay_att curr, const text_space *tm )
{
    unsigned wk = *tm;

    fprintf( fp, "        %s = %u\n", att_names[curr], wk );
    return;
}


/***************************************************************************/
/*  threshold                                                              */
/***************************************************************************/
bool    i_threshold( char * p, lay_att curr, uint16_t * tm )
{
    char    *   pa;
    long        wk;

    curr = curr;
    wk = strtol( p, NULL, 10 );

    for( pa = p; isdigit( *pa ); pa++ );
    if( *pa ) {
        xx_line_err_c( err_num_too_large, p );
    }
    if( wk == 0 ) {
        xx_line_err_c( err_num_zero, p );
    }
    if( wk > 0x7fff ) {
        xx_line_err_c( err_num_s16_neg, p );
    }
    *tm = wk;
    return( false );
}

void    o_threshold( FILE *fp, lay_att curr, const uint16_t * tm )
{
    unsigned wk = *tm;

    fprintf( fp, "        %s = %d\n", att_names[curr], wk );
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

    (void)curr;

    cvterr = false;
    if( (val_start == NULL) || (val_len > STRBLK_SIZE) ) {
        xx_line_err_c( err_xx_string, p );
    } else {
        strncpy( tm, val_start, val_len );
        tm[val_len] = '\0';
    }
    return( cvterr );
}

void    o_xx_string( FILE *fp, lay_att curr, const xx_str * tm )
{

    fprintf( fp, "        %s = \"%s\"\n", att_names[curr], tm );
    return;
}

/***************************************************************************/
/*  date_form      stored as string perhaps better other type    TBD       */
/***************************************************************************/
bool    i_date_form( char * p, lay_att curr, xx_str * tm )
{
    return( i_xx_string( p, curr, tm ) );
}

void    o_date_form( FILE *fp, lay_att curr, const xx_str * tm )
{
    o_xx_string( fp, curr, tm );
}

/***************************************************************************/
/*  yes or No  as bool result                                              */
/***************************************************************************/
bool    i_yes_no( char * p, lay_att curr, bool * tm )
{
    bool        cvterr;

    (void)curr;

    cvterr = false;
    if( strnicmp( "no", p, 2 ) == 0 ) {
        *tm = false;
    } else if( strnicmp( "yes", p, 3 ) == 0 ) {
        *tm = true;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_yes_no( FILE *fp, lay_att curr, const bool * tm )
{
    char    const   *   p;

    if( *tm == 0 ) {
        p = "no";
    } else {
        p = "yes";
    }
    fprintf( fp, "        %s = %s\n", att_names[curr], p );
    return;
}

