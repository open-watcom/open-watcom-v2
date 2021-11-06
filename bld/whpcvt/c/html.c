/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  This file defines the HTML specific functions.
*
****************************************************************************/

#include "whpcvt.h"

#include "clibext.h"


#define BOX_LINE_SIZE           200

#define FONT_STYLE_BOLD         1
#define FONT_STYLE_ITALIC       2
#define FONT_STYLE_UNDERLINE    4

#define HTML_SPACE              "&nbsp;"

#define HTML_TRANS_LEN          50

static int          Curr_head_level = 0;
static int          Curr_head_skip = 0;

static char         *Font_match[] = {
    "",             // 0: PLAIN
    "<b>",          // 1: BOLD
    "<i>",          // 2: ITALIC
    "<b><i>",       // 3: BOLD + ITALIC
    "<u>",          // 4: UNDERLINE
    "<b><u>",       // 5: BOLD + UNDERLINE
    "<i><u>",       // 6: ITALIC + UNDERLINE
    "<i><u><b>",    // 7: ITALIC + BOLD + UNDERLINE
};

static char         *Font_end[] = {
    "",             // 0: PLAIN
    "</b>",         // 1: BOLD
    "</i>",         // 2: ITALIC
    "</i></b>",     // 3: BOLD + ITALIC
    "</u>",         // 4: UNDERLINE
    "</u></b>",     // 5: BOLD + UNDERLINE
    "</u></i>",     // 6: ITALIC + UNDERLINE
    "</b></u></i>", // 7: ITALIC + BOLD + UNDERLINE
};

static int          Font_list[100];         // up to 100 nested fonts
static int          Font_list_curr = 0;

static bool         Blank_line_pfx = false;
static bool         Blank_line_sfx = true;

static char         *Trans_str = NULL;
static size_t       Trans_len = 0;

static void draw_line( section_def *section )
/*******************************************/
{
    trans_add_str_nl( "<hr>", section );
}

static size_t translate_char_html( char ch, char next_ch, char *buf )
/*******************************************************************/
{
    switch( ch ) {
    case '<':
        strcpy( buf, "&lt;" );
        break;
    case '>':
        strcpy( buf, "&gt;" );
        break;
    case '[':
        strcpy( buf, "&#91;" );
        break;
    case ']':
        strcpy( buf, "&#93;" );
        break;
    case '&':
        strcpy( buf, "&amp;" );
        break;
    case '"':
        strcpy( buf, "&quot;" );
        break;
    case ' ':
        if( next_ch == ' ' ) {
            strcpy( buf, HTML_SPACE );
            break;
        }
        /* fall through */
    default:
        buf[0] = ch;
        buf[1] = '\0';
        break;
    }
    return( strlen( buf ) );
}

static char *translate_str_html( const char *str )
/************************************************/
{
    const char      *t_str;
    size_t          len;
    char            buf[HTML_TRANS_LEN];
    char            *ptr;

    len = 1;
    for( t_str = str; *t_str != '\0'; t_str++ ) {
        len += translate_char_html( t_str[0], t_str[1], buf );
    }
    if( len > Trans_len ) {
        if( Trans_str != NULL ) {
            _free( Trans_str );
        }
        _new( Trans_str, len );
        Trans_len = len;
    }
    ptr = Trans_str;
    for( t_str = str; *t_str != '\0'; t_str++ ) {
        len = translate_char_html( t_str[0], t_str[1], buf );
        strcpy( ptr, buf );
        ptr += len;
    }
    *ptr = '\0';
    return( Trans_str );
}

static size_t trans_add_char_html( char ch, char next_ch, section_def *section )
/******************************************************************************/
{
    char        buf[HTML_TRANS_LEN];

    translate_char_html( ch, next_ch, buf );
    return( trans_add_str( buf, section ) );
}

static size_t trans_add_str_html( const char *str, section_def *section )
/***********************************************************************/
{
    size_t      len;

    len = 0;
    for( ; *str != '\0'; str++ ) {
        len += trans_add_char_html( str[0], str[1], section );
    }
    return( len );
}

static tab_size tab_align( tab_size ch_len, section_def *section )
/****************************************************************/
{
    tab_size    i;
    tab_size    len;

    // find the tab we should use
    len = Tabs_align( ch_len );
    for( i = len; i > 0; i-- ) {
        trans_add_str_html( HTML_SPACE, section );
    }
    return( len );
}

void html_topic_init( void )
/**************************/
{
}

void html_trans_line( char *line_buf, section_def *section )
/**********************************************************/
{
    char                *ptr;
    char                *end;
    char                ch;
    char                *ctx_name;
    char                *ctx_text;
    char                buf[500];
    int                 font_idx;
    size_t              line_len;
    bool                term_fix;
    size_t              ch_len;
    size_t              len;
    char                *file_name;

    /* check for special column 0 pre-processing stuff first */
    ptr = line_buf;
    ch = *ptr;
    ch_len = 0;
    line_len = 0;

    switch( ch ) {
    case WHP_TABXMP:
        ptr = skip_blanks( ptr + 1 );
        if( *ptr == '\0' ) {
            Tab_xmp = false;
            trans_add_str_nl( "</xmp>", section );
            Blank_line_sfx = false;     // remove following blanks
        } else {
            Tab_xmp_char = *ptr++;
            Tabs_read( ptr );
            trans_add_str_nl( "<xmp>", section );
            Tab_xmp = true;
            Blank_line_pfx = false;     // remove preceding blanks
        }
        return;
    case WHP_BOX_ON:
        /* Table support is the closest thing to boxing in IPF, but it
           doesn't work well with changing fonts on items in the tables
           (the edges don't line up). So we draw long lines at the
           top and bottom instead */
        draw_line( section );
        Blank_line_pfx = false;
        return;
    case WHP_BOX_OFF:
        draw_line( section );
        Blank_line_sfx = false;
        return;
    case WHP_OLIST_START:
    case WHP_LIST_START:
    case WHP_DLIST_START:
    case WHP_SLIST_START:
        NewList( ptr, 0, false );
        switch( ch ) {
        case WHP_OLIST_START:
            trans_add_str( "<ol", section );
            break;
        case WHP_LIST_START:
            trans_add_str( "<ul", section );
            break;
        case WHP_DLIST_START:
            trans_add_str( "<dl", section );
            break;
        case WHP_SLIST_START:
            trans_add_str( "<ul style=\"list-style-type:none;\"", section );
            break;
        }
        trans_add_str_nl( ">", section );
        Blank_line_pfx = false;
        return;
    case WHP_OLIST_END:
    case WHP_LIST_END:
    case WHP_DLIST_END:
    case WHP_SLIST_END:
        switch( ch ) {
        case WHP_OLIST_END:
            trans_add_str_nl( "</ol>", section );
            break;
        case WHP_LIST_END:
        case WHP_SLIST_END:
            trans_add_str_nl( "</ul>", section );
            break;
        case WHP_DLIST_END:
            trans_add_str_nl( "</dl>", section );
            break;
        }
        Blank_line_sfx = false;
        PopList();
        return;
    case WHP_LIST_ITEM:
    case WHP_DLIST_TERM:
    case WHP_DLIST_DESC:
        /* eat blank lines before list items and terms */
        Blank_line_pfx = false;
        break;
    case WHP_CTX_KW:
        ptr = whole_keyword_line( ptr );
        if( ptr == NULL ) {
            return;
        }
        break;
    }

    if( *skip_blanks( ptr ) == '\0' ) {
        /* ignore blanks lines before the topic starts */
        if( !Curr_ctx->empty ) {
            /* the line is completely blank. This tells us to output
               a blank line. BUT, all lists and things automatically
               generate blank lines before they display, so we
               must pend the line */
            Blank_line_pfx = true;
        }
        return;
    }

    /* An explanation of 'Blank_line_pfx': when we hit a blank line,
       we set Blank_line_pfx to true. On the non-tag next line, the
       blank line is generated.
       Some tags automatically generate a blank line, so they
       turn this flag off. This causes the next non-tag line to NOT
       put out the blank line */

    if( Blank_line_pfx ) {
        if( Blank_line_sfx ) {
            line_len += trans_add_str( "<br>", section );
        }
        Blank_line_pfx = false;
    }

    /* An explanation of 'Blank_line_sfx': some ending tags automatically
       generate a blank line, so no blank line after them should get
       generated. Normally, this flag is set to true, but ending
       tags and Defn list term tags set this false, so no extra '<BR>'
       is generated.
       But, this rule only applies if a blank line immediately
       follows the tag, so its reset here regardless */

    Blank_line_sfx = true;

    if( !Tab_xmp ) {
        ch = *ptr;
        switch( ch ) {
        case WHP_LIST_ITEM:
        case WHP_DLIST_TERM:
        case WHP_DLIST_DESC:
            break;
        default:
            /* a .br in front of li and dt would generate extra spaces */
            line_len += trans_add_str( "<br>", section );
            break;
        }
    }

    term_fix = false;
    for( ;; ) {
        ch = *ptr;
        if( ch == '\0' ) {
            if( term_fix ) {
//              trans_add_str( "</hp2>", section, &size );
                term_fix = false;
            }
            trans_add_char( '\n', section );
            break;
        }
        switch( ch ) {
        case WHP_FLINK:
            ptr++;
            file_name = ptr;
            ptr = strchr( file_name, WHP_FLINK );
            if( ptr == NULL ) {
                error( ERR_BAD_LINK_DFN );
            }
            *ptr = '\0';
            /* fall through */
        case WHP_HLINK:
        case WHP_DFN:
            Curr_ctx->empty = false;
            /* there are no popups in IPF, so treat them as links */
            ptr++;
            ctx_name = ptr;
            ptr = strchr( ctx_name, ch );
            if( ptr == NULL ) {
                error( ERR_BAD_LINK_DFN );
            }
            *ptr++ = '\0';
            ctx_text = ptr;
            ptr = strchr( ctx_text, ch );
            if( ptr == NULL ) {
                error( ERR_BAD_LINK_DFN );
            }
            *ptr++ = '\0';
            if( ch != WHP_FLINK ) {
                add_link( ctx_name );
            }
            sprintf( buf, "<a href=\"#%s\">", ctx_name );
            line_len += trans_add_str( buf, section );
            line_len += trans_add_str_html( ctx_text, section );
            ch_len += strlen( ctx_text );
            line_len += trans_add_str( "</a>", section );
            break;
        case WHP_LIST_ITEM:
            /* list item */
            Curr_list->number++;
            line_len += trans_add_str( "<li>", section );
            ptr = skip_blanks( ptr + 1 );
            break;
        case WHP_DLIST_DESC:
            Curr_list->number++;
            trans_add_str( "<dd>", section );
            ptr = skip_blanks( ptr + 1 );
            Blank_line_sfx = false;
            break;
        case WHP_DLIST_TERM:
            /* definition list term */
            if( Curr_list->number ) {
                line_len += trans_add_str( "<dt><br>", section );
            } else {
                line_len += trans_add_str( "<dt>", section );
            }
            term_fix = true;
            ptr = skip_blanks( ptr + 1 );
            Blank_line_sfx = false;
            break;
        case WHP_CTX_KW:
            ptr++;
            end = strchr( ptr, WHP_CTX_KW );
            len = end - ptr;
            memcpy( buf, ptr, len );
            buf[len] = '\0';
            add_ctx_keyword( Curr_ctx, buf );
            ptr = end + 1;
            if( *ptr == ' ' ) {
                /* kludge fix cuz of GML: GML thinks that keywords are
                   are real words, so it puts a space after them.
                   This should fix that */
                ptr++;
            }
            break;
        case WHP_PAR_RESET:
            /* this can be ignored for IPF */
            ptr++;
            break;
        case WHP_BMP:
            Curr_ctx->empty = false;
            ptr++;
            ch = *ptr;
            file_name = ptr + 2;
            ptr = strchr( file_name, WHP_BMP );
            *ptr++ = '\0';
            // convert filenames to lower case
            strlwr( file_name );
            switch( ch ) {
            case 'i':
                sprintf( buf, "<img src=\"%s\">", file_name );
                break;
            case 'l':
                sprintf( buf, "<img src=\"%s\" style=\"vertical-align:top\">", file_name );
                break;
            case 'r':
                sprintf( buf, "<img src=\"%s\" style=\"vertical-align:bottom\">", file_name );
                break;
            case 'c':
                sprintf( buf, "<img src=\"%s\" style=\"vertical-align:middle\">", file_name );
                break;
            default:
                *buf = '\0';
                break;
            }
            line_len += trans_add_str( buf, section );
            break;
        case WHP_FONTSTYLE_START:
            ptr++;
            end = strchr( ptr, WHP_FONTSTYLE_START );
            font_idx = 0;
            for( ; ptr != end; ptr++ ) {
                switch( *ptr ) {
                case 'b':
                    font_idx |= FONT_STYLE_BOLD;
                    break;
                case 'i':
                    font_idx |= FONT_STYLE_ITALIC;
                    break;
                case 'u':
                case 's':
                    font_idx |= FONT_STYLE_UNDERLINE;
                    break;
                }
            }
            line_len += trans_add_str( Font_match[font_idx], section );
            Font_list[Font_list_curr] = font_idx;
            Font_list_curr++;
            ptr++;
            break;
        case WHP_FONTSTYLE_END:
            Font_list_curr--;
            line_len += trans_add_str( Font_end[Font_list[Font_list_curr]], section );
            ptr++;
            break;
        case WHP_FONTTYPE:
            ptr++;
            end = strchr( ptr, WHP_FONTTYPE );
            *end++ = '\0';
            if( stricmp( ptr, Fonttype_courier ) == 0 ) {
                strcpy( buf, "<tt>" );
            } else {
                /* default system font */
                strcpy( buf, "</tt>" );
            }
            ptr = strchr( end, WHP_FONTTYPE ) + 1;
            line_len += trans_add_str( buf, section );
            break;
        default:
            ptr++;
            Curr_ctx->empty = false;
            if( Tab_xmp && ch == Tab_xmp_char ) {
                len = tab_align( ch_len, section );
                ch_len += len;
                line_len += len * sizeof( HTML_SPACE );
                ptr = skip_blanks( ptr );
            }
            if( line_len > 120 && ch == ' ' && !Tab_xmp ) {
                /* break onto the next line */
                trans_add_char( '\n', section );
                line_len = 0;
            }
            line_len += trans_add_char_html( ch, *ptr, section );
            ch_len++;
            break;
        }
    }
}

static void output_hdr( void )
/****************************/
{
    whp_fprintf( Out_file, "<head>\n" );
    if( Ipf_or_Html_title != NULL && Ipf_or_Html_title[0] != '\0' ) {
        whp_fprintf( Out_file, "<title> %s </title>\n", Ipf_or_Html_title );
    }
    whp_fprintf( Out_file, "</head>\n" );
    whp_fprintf( Out_file, "<body>\n" );
}

static void output_ctx_hdr( ctx_def *ctx )
/****************************************/
{
    int         head_level;
    int         p_skip;

    head_level = ctx->head_level;
    if( head_level == 0 ) {
        /* OS/2 can't handle heading level 0 */
        head_level = 1;
    }
    head_level -= Curr_head_skip;
    if( head_level > Curr_head_level + 1 ) {
        /* you can't skip heading levels upwards in IPF. To handle this,
           you go up to the next level, and keep track of the
           skip for future heading levels. */
        p_skip = head_level - Curr_head_level - 1 ;
        head_level -= p_skip;
        Curr_head_skip += p_skip;
    } else if( head_level < Curr_head_level ) {
        head_level += Curr_head_skip;
        if( head_level > Curr_head_level ) {
            /* we moved down levels, but we're still too high! */
            Curr_head_skip = head_level - Curr_head_level;
            head_level -= Curr_head_skip;
        } else {
            Curr_head_skip = 0;
        }
    }

    Curr_head_level = head_level;

    whp_fprintf( Out_file, "<h%d", head_level );
    whp_fprintf( Out_file, " id=\"%s\">", translate_str_html( ctx->ctx_name ) );
    whp_fprintf( Out_file, " %s </h%d>\n", translate_str_html( ctx->title ), head_level );

#if 0
    if( ctx->keylist != NULL ) {
        for( keylist = ctx->keylist; keylist != NULL; keylist = keylist->next ) {
            key = keylist->key;
            ptr = key->keyword;
            if( !key->duplicate ) {
                whp_fprintf( Out_file, "<i1>" );
            } else {
                if( key->defined_ctx == ctx ) {
                    /* this is the first instance. :i1 and :i2 */
                    whp_fprintf( Out_file, "<i1 id=%d>%s\n", key->id, translate_str_html( ptr ) );
                }
                if( stricmp( ptr, ctx->title ) == 0 ) {
                    /* we are about to out an index subentry whose
                       name is the same as the main index entry!
                       Skip it! */
                    continue;
                }
                whp_fprintf( Out_file, "<i2 refid=%d>", key->id );
                ptr = ctx->title;
            }
            whp_fprintf( Out_file, "%s\n", translate_str_html( ptr ) );
        }
    }
#endif

    if( Ipf_or_Html_Real_font ) {
        /* The default font is system, which wouldn't be right */
        whp_fprintf( Out_file, "<font facename=Helv size=10x10>\n" );
    }
    /* browse lists are not used in IPF */
    /* nor does 'Up' topicing have any relevance */
}

static void output_end( void )
/****************************/
{
    whp_fprintf( Out_file, "</body>\n" );
}

static void output_ctx_sections( ctx_def *ctx )
/*********************************************/
{
    section_def     *section;

    for( section = ctx->section_list; section != NULL; section = section->next ) {
        if( section->section_size > 0 ) {
            whp_fwrite( Out_file, section->section_text, 1, section->section_size );
        }
    }
}

void html_output_file( void )
/***************************/
{
    ctx_def     *ctx;

    output_hdr();
    for( ctx = Ctx_list; ctx != NULL; ctx = ctx->next ) {
        if( !Remove_empty || !ctx->empty || ctx->req_by_link ) {
            if( !Exclude_special_topics || !is_special_topic( ctx, false ) ) {
                output_ctx_hdr( ctx );
                output_ctx_sections( ctx );
            }
        }
    }
    output_end();
}

void html_init_whp( void )
{
}
