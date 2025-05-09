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
        #define pick(ban,gml,text,len)   { text, len, ban },
        #include "bdocsect.h"
        #undef pick
/*
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
*/
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

static char *get_lay_attname( const char *p, lay_att_val *lay_attr )
{
    int     i;

    lay_attr->att_name = (char *)p;
    i = 0;
    while( is_lay_att_char( *p ) ) {
        if( i < LAY_ATT_NAME_LENGTH ) {
            lay_attr->attname[i++] = my_tolower( *p );
        }
        p++;
    }
    lay_attr->attname[i] = '\0';
    return( (char *)p );
}

condcode    lay_attr_and_value( lay_att_val *lay_attr )
{
    char        *   p;
    char        *   pa;
    condcode        rc;

    p = scandata.s;
    pa = p;
    rc = no;

    SkipSpacesTabs( p );                // over WS to start of name

    memset( lay_attr, 0, sizeof( *lay_attr ) );
    lay_attr->val.quoted = ' ';

    /*
     * loop until attribute/value pair or rescan line found
     */
    while( *(p = get_lay_attname( p, lay_attr )) == '\0' ) {
        if( input_cbs->fmflags & II_eof ) {
            break;
        }
        if( !get_line( true ) ) {       // next line for missing attribute
            break;
        }
        process_line();
        scandata.s = buff2;
        scandata.e = buff2 + buff2_lg;
        if( (*scandata.s == SCR_char)       // cw found: end-of-tag
          || (*scandata.s == GML_char) ) {  // tag found: end-of-tag
            ProcFlags.reprocess_line = true;
            scandata.s = p;
            return( rc );
        }
        p = scandata.s;                 // new line is part of current tag
        SkipSpacesTabs( p );            // over WS to start of alleged attribute
    }
    if( *p == '.' ) {                   // end of tag
        ProcFlags.tag_end_found = true;
        return( omit );
    }
    if( p - lay_attr->att_name < 4 ) {
        xx_line_err_c( err_att_name_inv, pa );
    }

    p = get_lay_value( p, &lay_attr->val );
    if( lay_attr->val.len > 0 ) {
        rc = pos;
    }
    scandata.s = p;
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
bool    i_case( char * p, lay_attr_i lay_attr, case_t * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "mixed", lay_attr->val.specval ) == 0 ) {
        *tm = case_mixed;
    } else if( strcmp( "lower", lay_attr->val.specval ) == 0 ) {
        *tm = case_lower;
    } else if( strcmp( "upper", lay_attr->val.specval ) == 0 ) {
        *tm = case_upper;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_case( FILE *fp, lay_attr_o lay_attr, const case_t * tm )
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
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  single character                                                       */
/*  UL bullet entered as '' is treated as ' ' by wgml 4.0, so all may be   */
/***************************************************************************/
bool    i_char( char * p, lay_attr_i lay_attr, char * tm )
{
    if( lay_attr->val.quoted && *p == '\0' ) {
        *tm = ' ';                      // space if '' or ""
    } else {
        *tm = *p;                       // else 1st char
    }
    return( false );
}

void    o_char( FILE *fp, lay_attr_o lay_attr, const char * tm )
{
    fprintf( fp, "        %s = '%c'\n", lay_att_names[lay_attr], *tm );
    return;
}


/***************************************************************************/
/*  contents for banregion    only unquoted                                */
/***************************************************************************/
bool    i_content( char * p, lay_attr_i lay_attr, content * tm )
{
    bool        cvterr;
    char    *   pa;
    int         k;
    size_t      len;

    (void)lay_attr;

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

void    o_content( FILE *fp, lay_attr_o lay_attr, const content * tm )
{
    const   char    * p;
    char              c;

    if( tm->content_type >= no_content && tm->content_type < max_content) {
        p = tm->string;
        if( tm->content_type == string_content ) { // user string with quotes
            fprintf( fp, "        %s = '", lay_att_names[lay_attr] );
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
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  default frame                                                          */
/***************************************************************************/
bool    i_default_frame( char * p, lay_attr_i lay_attr, def_frame * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "none", lay_attr->val.specval ) == 0 ) {
        tm->type = none_frame;
    } else if( strcmp( "rule", lay_attr->val.specval ) == 0 ) {
        tm->type = rule_frame;
    } else if( strcmp( "box", lay_attr->val.specval ) == 0 ) {
        tm->type = box_frame;
    } else if( !is_quote_char( *p ) ) {
        cvterr = true;
    } else {
        if( lay_attr->val.len == 0 ) {  // empty string entered
            tm->type = none_frame;      // should work for both FIG and IXHEAD
        } else {                        // string value entered
            i_xx_string( p, lay_attr, tm->string );
            tm->type = char_frame;
        }
    }
    if( cvterr ) {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );

}

void    o_default_frame( FILE *fp, lay_attr_o lay_attr, const def_frame * tm )
{

    switch( tm->type ) {
    case none_frame:
        fprintf( fp, "        %s = none\n", lay_att_names[lay_attr] );
        break;
    case rule_frame:
        fprintf( fp, "        %s = rule\n", lay_att_names[lay_attr] );
        break;
    case box_frame:
        fprintf( fp, "        %s = box\n", lay_att_names[lay_attr] );
        break;
    case char_frame:
        fprintf( fp, "        %s = '%s'\n", lay_att_names[lay_attr], tm->string );
        break;
    default:
        fprintf( fp, "        %s = ???\n", lay_att_names[lay_attr] );
        break;
    }
    return;
}


/***************************************************************************/
/*  docsect  refdoc                                                        */
/***************************************************************************/
bool    i_docsect( char * p, lay_attr_i lay_attr, ban_docsect * tm )
{
    bool        cvterr;
    int         k;

    (void)lay_attr;

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

void    o_docsect( FILE *fp, lay_attr_o lay_attr, const ban_docsect * tm )
{
    const   char    * p;

    if( *tm >= no_ban && *tm < max_ban) {
        p = doc_sections[*tm].name;
    } else {
        p = "???";
    }
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  frame  rule or none                                                    */
/***************************************************************************/
bool    i_frame( char * p, lay_attr_i lay_attr, bool * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "none", lay_attr->val.specval ) == 0 ) {
        *tm = false;
    } else if( strcmp( "rule", lay_attr->val.specval ) == 0 ) {
        *tm = true;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );

}

void    o_frame( FILE *fp, lay_attr_o lay_attr, const bool * tm )
{
    char    * p;

    if( *tm ) {
        p = "rule";
    } else {
        p = "none";
    }
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  integer routines                                                       */
/***************************************************************************/
bool    i_int32( char * p, lay_attr_i lay_attr, int32_t * tm )
{
    long    wk;

    (void)lay_attr;

    wk = strtol( p, NULL, 10 );
    *tm = wk;
    return( false );
}

void    o_int32( FILE *fp, lay_attr_o lay_attr, const int32_t * tm )
{

    fprintf( fp, "        %s = %d\n", lay_att_names[lay_attr], *tm );
    return;
}

bool    i_int8( char * p, lay_attr_i lay_attr, int8_t * tm )
{
    long    wk;

    (void)lay_attr;

    wk = strtol( p, NULL, 10 );
    if( abs( wk ) > 255 ) {
        xx_line_err_c( err_i_8, p );
    }
    *tm = wk;
    return( false );
}

void    o_int8( FILE *fp, lay_attr_o lay_attr, const int8_t * tm )
{
    int     wk = *tm;

    fprintf( fp, "        %s = %d\n", lay_att_names[lay_attr], wk );
    return;
}

bool    i_uint8( char * p, lay_attr_i lay_attr, uint8_t * tm )
{
    unsigned long   wk;

    (void)lay_attr;

    wk = strtoul( p, NULL, 10 );
    if( errno == ERANGE || wk > 255 ) {
        xx_line_err_c( err_ui_8, p );
    }
    *tm = wk;
    return( false );
}

void    o_uint8( FILE *fp, lay_attr_o lay_attr, const uint8_t * tm )
{
    unsigned    wk = *tm;

    fprintf( fp, "        %s = %u\n", lay_att_names[lay_attr], wk );
    return;
}


/***************************************************************************/
/*  font number                                                            */
/***************************************************************************/
bool    i_font_number( char *p, lay_attr_i lay_attr, font_number *tm )
{
    char    *   pb;
    size_t      len;

    (void)lay_attr;

    pb = p;
    len = 0;
    while( *pb != '\0' && !is_space_tab_char( *pb ) && *pb != '.' ) {   // get length
        len++;
        pb++;
    }
    *tm = get_font_number( p, len );
    return( false );
}

void    o_font_number( FILE *fp, lay_attr_o lay_attr, const font_number *tm )
{
    fprintf( fp, "        %s = %u\n", lay_att_names[lay_attr], (unsigned)*tm );
}


/***************************************************************************/
/*  number form                                                            */
/***************************************************************************/
bool    i_number_form( char * p, lay_attr_i lay_attr, num_form * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "none", lay_attr->val.specval ) == 0 ) {
        *tm = num_none;
    } else if( strcmp( "prop", lay_attr->val.specval ) == 0 ) {
        *tm = num_prop;
    } else if( strcmp( "new", lay_attr->val.specval ) == 0 ) {
        *tm = num_new;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_number_form( FILE *fp, lay_attr_o lay_attr, const num_form * tm )
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
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  number style                                                           */
/***************************************************************************/
bool    i_number_style( char * p, lay_attr_i lay_attr, num_style * tm )
{
    bool        cvterr;
    num_style   wk = 0;
    char        c;

    cvterr = false;
    c = *lay_attr->val.specval;     // lower cased already
    switch( c ) {                   // first letter
    case 'a':
        wk |= a_style;
        break;
    case 'b':
        wk |= b_style;
        break;
    case 'c':
        wk |= c_style;
        break;
    case 'r':
        wk |= r_style;
        break;
    case 'h':
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
        case 'd':
            wk |= xd_style;
            break;
        case 'p':
            p++;
            if( *p != '\0' && (*p != ' ') ) {   // third letter
                c = my_tolower( *p );
                switch( c ) {
                case 'a':
                    wk |= xpa_style;    // only left parenthesis
                    break;
                case 'b':
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

void    o_number_style( FILE *fp, lay_attr_o lay_attr, const num_style * tm )
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
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], str );
    return;
}


/***************************************************************************/
/*  page eject                                                             */
/***************************************************************************/
bool    i_page_eject( char * p, lay_attr_i lay_attr, page_ej * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "no", lay_attr->val.specval ) == 0 ) {
        *tm = ej_no;
    } else if( strcmp( "yes", lay_attr->val.specval ) == 0 ) {
        *tm = ej_yes;
    } else if( strcmp( "odd", lay_attr->val.specval ) == 0 ) {
        *tm = ej_odd;
    } else if( strcmp( "even", lay_attr->val.specval ) == 0 ) {
        *tm = ej_even;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_page_eject( FILE *fp, lay_attr_o lay_attr, const page_ej * tm )
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
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  page position                                                          */
/***************************************************************************/
bool    i_page_position( char * p, lay_attr_i lay_attr, page_pos * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "left", lay_attr->val.specval ) == 0 ) {
        *tm = pos_left;
    } else if( strcmp( "right", lay_attr->val.specval ) == 0 ) {
        *tm = pos_right;
    } else if( (strcmp( "centre", lay_attr->val.specval ) == 0
      || strcmp( "center", lay_attr->val.specval ) == 0 ) ) {
        *tm = pos_center;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_page_position( FILE *fp, lay_attr_o lay_attr, const page_pos * tm )
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
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  place                                                                  */
/***************************************************************************/
bool    i_place( char * p, lay_attr_i lay_attr, bf_place * tm )
{
    bool        cvterr;
    int         k;

    (void)lay_attr;

    cvterr = false;
    *tm = no_place;
    for( k = no_place; k < max_place; ++k ) {
        if( strcmp( bf_places[k].name, lay_attr->val.specval ) == 0 ) {
            *tm = bf_places[k].type;
            break;
        }
    }
    if( *tm == no_place ) {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_place( FILE *fp, lay_attr_o lay_attr, const bf_place * tm )
{
    const   char    *   p;
            int         k;

    p = "???";          // desperation value
    for( k = no_place; k < max_place; ++k ) {
        if( bf_places[k].type == *tm ) {
            p = bf_places[k].name;
        }
    }
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  pouring                                                                */
/***************************************************************************/
bool    i_pouring( char * p, lay_attr_i lay_attr, reg_pour * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "none", lay_attr->val.specval ) == 0 ) {
        *tm = no_pour;
    } else if( strcmp( "last", lay_attr->val.specval ) == 0 ) {
        *tm = last_pour;
    } else if( strcmp( "head0", lay_attr->val.specval ) == 0 ) {
        *tm = head0_pour;
    } else if( strcmp( "head1", lay_attr->val.specval ) == 0 ) {
        *tm = head1_pour;
    } else if( strcmp( "head2", lay_attr->val.specval ) == 0 ) {
        *tm = head2_pour;
    } else if( strcmp( "head3", lay_attr->val.specval ) == 0 ) {
        *tm = head3_pour;
    } else if( strcmp( "head4", lay_attr->val.specval ) == 0 ) {
        *tm = head4_pour;
    } else if( strcmp( "head5", lay_attr->val.specval ) == 0 ) {
        *tm = head5_pour;
    } else if( strcmp( "head6", lay_attr->val.specval ) == 0 ) {
        *tm = head6_pour;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_pouring( FILE *fp, lay_attr_o lay_attr, const reg_pour * tm )
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
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}


/***************************************************************************/
/*  space unit                                                             */
/***************************************************************************/
bool    i_space_unit( char * p, lay_attr_i lay_attr, su * tm )
{
    (void)p;

    return( att_val_to_su( tm, true, &lay_attr->val, true ) );    // no negative values allowed TBD
}

void    o_space_unit( FILE *fp, lay_attr_o lay_attr, const su * tm )
{

    if( tm->su_u == SU_chars_lines || tm->su_u == SU_undefined ||
        tm->su_u >= SU_lay_left ) {
        fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], tm->su_txt );
    } else {
        fprintf( fp, "        %s = '%s'\n", lay_att_names[lay_attr], tm->su_txt );
    }
    return;
}


/***************************************************************************/
/*  spacing                                                                */
/***************************************************************************/
bool    i_spacing( char *p, lay_attr_i lay_attr, text_space *tm )
{
    long wk;

    (void)lay_attr;

    wk = strtol( p, NULL, 10 );
    if( wk < 0 || wk > 255 ) {
        xx_line_err_c( err_ui_8, p );
    }
    *tm = wk;
    return( false );
}

void    o_spacing( FILE *fp, lay_attr_o lay_attr, const text_space *tm )
{
    unsigned wk = *tm;

    fprintf( fp, "        %s = %u\n", lay_att_names[lay_attr], wk );
    return;
}


/***************************************************************************/
/*  threshold                                                              */
/***************************************************************************/
bool    i_threshold( char * p, lay_attr_i lay_attr, uint16_t * tm )
{
    char    *   pa;
    long        wk;

    (void)lay_attr;

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

void    o_threshold( FILE *fp, lay_attr_o lay_attr, const uint16_t * tm )
{
    unsigned wk = *tm;

    fprintf( fp, "        %s = %d\n", lay_att_names[lay_attr], wk );
    return;
}


/***************************************************************************/
/*  xx_string  for :NOTE and others                                        */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool    i_xx_string( char * p, lay_attr_i lay_attr, xx_str * tm )
{
    bool        cvterr;

    cvterr = false;
    if( (lay_attr->val.name != NULL) && (lay_attr->val.len < STRBLK_SIZE) ) {
        strncpy( tm, lay_attr->val.name, lay_attr->val.len );
        tm[lay_attr->val.len] = '\0';
    } else {
        xx_line_err_c( err_xx_string, p );
    }
    return( cvterr );
}

void    o_xx_string( FILE *fp, lay_attr_o lay_attr, const xx_str * tm )
{

    fprintf( fp, "        %s = \"%s\"\n", lay_att_names[lay_attr], tm );
    return;
}

/***************************************************************************/
/*  date_form      stored as string perhaps better other type    TBD       */
/***************************************************************************/
bool    i_date_form( char * p, lay_attr_i lay_attr, xx_str * tm )
{
    return( i_xx_string( p, lay_attr, tm ) );
}

void    o_date_form( FILE *fp, lay_attr_o lay_attr, const xx_str * tm )
{
    o_xx_string( fp, lay_attr, tm );
}

/***************************************************************************/
/*  yes or No  as bool result                                              */
/***************************************************************************/
bool    i_yes_no( char * p, lay_attr_i lay_attr, bool * tm )
{
    bool        cvterr;

    cvterr = false;
    if( strcmp( "no", lay_attr->val.specval ) == 0 ) {
        *tm = false;
    } else if( strcmp( "yes", lay_attr->val.specval ) == 0 ) {
        *tm = true;
    } else {
        xx_line_err_c( err_inv_att_val, p );
    }
    return( cvterr );
}

void    o_yes_no( FILE *fp, lay_attr_o lay_attr, const bool * tm )
{
    char    const   *   p;

    if( *tm == 0 ) {
        p = "no";
    } else {
        p = "yes";
    }
    fprintf( fp, "        %s = %s\n", lay_att_names[lay_attr], p );
    return;
}

