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
* Description:  This file defines the IPF specific functions.
*
****************************************************************************/

#include "whpcvt.h"

#include "clibext.h"


#define BOX_LINE_SIZE           200

#define BOX_HBAR                (char)'\xC4'  // 196

#define FONT_STYLE_BOLD         1
#define FONT_STYLE_ITALIC       2
#define FONT_STYLE_UNDERLINE    4

#define IPF_TRANS_LEN           50

static int          Curr_head_level = 0;
static int          Curr_head_skip = 0;

static const char   *Font_match[] = {
    ":hp1.:ehp1",               // 0: PLAIN
    ":hp2.",                    // 1: BOLD
    ":hp1.",                    // 2: ITALIC
    ":hp3.",                    // 3: BOLD + ITALIC
    ":hp5.",                    // 4: UNDERLINE
    ":hp7.",                    // 5: BOLD + UNDERLINE
    ":hp6.",                    // 6: ITALIC + UNDERLINE
    ":hp7.",                    // 7: BOLD + ITALIC + UNDERLINE (can't do it)
};

static const char   *Font_end[] = {
    "",                         // 0: PLAIN
    ":ehp2.",                   // 1: BOLD
    ":ehp1.",                   // 2: ITALIC
    ":ehp3.",                   // 3: BOLD + ITALIC
    ":ehp5.",                   // 4: UNDERLINE
    ":ehp7.",                   // 5: BOLD + UNDERLINE
    ":ehp6.",                   // 6: ITALIC + UNDERLINE
    ":ehp7.",                   // 7: BOLD + ITALIC + UNDERLINE (can't do it)
};

static int          Font_list[100];      // up to 100 nested fonts
static int          Font_list_curr = 0;

static bool         Blank_line_pfx = false;
static bool         Blank_line_sfx = true;

static char         *Trans_str = NULL;
static size_t       Trans_len = 0;

static void draw_line( section_def *section )
/*******************************************/
{
    int         i;

    trans_add_str_nl( ":cgraphic.", section );
    for( i = BOX_LINE_SIZE; i > 0; i-- ) {
        trans_add_char( BOX_HBAR, section );
    }
    trans_add_str_nl( "\n:ecgraphic.", section );
}

static size_t translate_char_ipf( char ch, char *buf )
/****************************************************/
{
    switch( ch ) {
    case ':':
        strcpy( buf,  "&colon." );
        break;
    case '=':
        strcpy( buf, "&eq." );
        break;
    case '&':
        strcpy( buf, "&amp." );
        break;
    case '.':
        strcpy( buf, "&per." );
        break;
    default:
        buf[0] = ch;
        buf[1] = '\0';
        break;
    }
    return( strlen( buf ) );
}

static char *translate_str_ipf( const char *str )
/***********************************************/
{
    const char          *t_str;
    size_t              len;
    char                buf[IPF_TRANS_LEN];
    char                *ptr;

    len = 1;
    for( t_str = str; *t_str != '\0'; t_str++ ) {
        len += translate_char_ipf( *t_str, buf );
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
        len = translate_char_ipf( *t_str, buf );
        strcpy( ptr, buf );
        ptr += len;
    }
    *ptr = '\0';

    return( Trans_str );
}

static size_t trans_add_char_ipf( char ch, section_def *section )
/***************************************************************/
{
    char        buf[IPF_TRANS_LEN];

    translate_char_ipf( ch, buf );
    return( trans_add_str( buf, section ) );
}

static size_t trans_add_str_ipf( const char *str, section_def *section )
/**********************************************************************/
{
    size_t      len;

    len = 0;
    for( ; *str != '\0'; str++ ) {
        len += trans_add_char_ipf( *str, section );
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
        trans_add_char_ipf( ' ', section );
    }
    return( len );
}

void ipf_topic_init( void )
/*************************/
{
}

void ipf_trans_line( char *line_buf, section_def *section )
/*********************************************************/
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
            trans_add_str_nl( ":exmp.", section );
            Blank_line_sfx = false;     // remove following blanks
        } else {
            Tab_xmp_char = *ptr++;
            Tabs_read( ptr );
            trans_add_str_nl( ":xmp.", section );
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
    case WHP_LIST_START:
    case WHP_DLIST_START:
    case WHP_OLIST_START:
    case WHP_SLIST_START:
        NewList( ptr, 0, false );
        switch( ch ) {
        case WHP_OLIST_START:
            trans_add_str( ":ol", section );
            break;
        case WHP_LIST_START:
            trans_add_str( ":ul", section );
            break;
        case WHP_DLIST_START:
            trans_add_str( ":dl break=all tsize=5", section );
            break;
        case WHP_SLIST_START:
            trans_add_str( ":sl", section );
            break;
        }
        if( ptr[1] == WHP_LIST_COMPACT )
            trans_add_str( " compact", section );
        trans_add_str_nl( ".", section );
        Blank_line_pfx = false;
        if( ch == WHP_DLIST_START ) {
            if( ptr[1] == WHP_LIST_COMPACT )
                ptr++;
            ptr = skip_blanks( ptr + 1 );
            if( *ptr != '\0' ) {
                /* due to a weakness in GML, the definition term must be
                   allowed on the same line as the definition tag. So
                   if its there, continue */
                break;
            }
        }
        return;
    case WHP_LIST_END:
    case WHP_DLIST_END:
    case WHP_OLIST_END:
    case WHP_SLIST_END:
        switch( ch ) {
        case WHP_SLIST_END:
            trans_add_str_nl( ":esl.", section );
            break;
        case WHP_OLIST_END:
            trans_add_str_nl( ":eol.", section );
            break;
        case WHP_LIST_END:
            trans_add_str_nl( ":eul.", section );
            break;
        case WHP_DLIST_END:
            trans_add_str_nl( ":edl.", section );
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
            line_len += trans_add_str_nl( ".br", section );
        }
        Blank_line_pfx = false;
    }

    /* An explanation of 'Blank_line_sfx': some ending tags automatically
       generate a blank line, so no blank line after them should get
       generated. Normally, this flag is set to true, but ending
       tags and Defn list term tags set this false, so no extra '.br'
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
            line_len += trans_add_str_nl( ".br", section );
            break;
        }
    }

    term_fix = false;
    for( ;; ) {
        ch = *ptr;
        if( ch == '\0' ) {
            if( term_fix ) {
                trans_add_str( ":ehp2.", section );
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
            if( ch == WHP_FLINK ) {
                sprintf( buf, ":link reftype=launch object='view.exe' "
                          "data='%s %s'.", file_name, ctx_name );
            } else {
                add_link( ctx_name );
                sprintf( buf, ":link reftype=hd refid=%s.", ctx_name );
            }
            line_len += trans_add_str( buf, section );
            line_len += trans_add_str_ipf( ctx_text, section );
            ch_len += strlen( ctx_text );
            line_len += trans_add_str( ":elink.", section );
            break;
        case WHP_LIST_ITEM:
            /* list item */
            Curr_list->number++;
            line_len += trans_add_str( ":li.", section );
            ptr = skip_blanks( ptr + 1 );
            break;
        case WHP_DLIST_DESC:
            Curr_list->number++;
            trans_add_str( ":dd.", section );
            ptr = skip_blanks( ptr + 1 );
            Blank_line_sfx = false;
            break;
        case WHP_DLIST_TERM:
            /* definition list term */
            ptr = skip_blanks( ptr + 1 );
            if( *ptr == WHP_FONTSTYLE_START ) {  /* avoid nesting */
                line_len += trans_add_str( ":dt.", section );
                Blank_line_sfx = false;
            } else {
                line_len += trans_add_str( ":dt.:hp2.", section );
                term_fix = true;
                Blank_line_sfx = false;
            }
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
            switch( ch ) {
            case 'i':
                sprintf( buf, ":artwork runin name='%s'.", file_name );
                break;
            case 'l':
                sprintf( buf, ":artwork align=left name='%s'.", file_name );
                break;
            case 'r':
                sprintf( buf, ":artwork align=right name='%s'.", file_name );
                break;
            case 'c':
                sprintf( buf, ":artwork align=center name='%s'.", file_name );
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
            strcpy( buf, ":font facename=" );
            if( Ipf_or_Html_Real_font ) {
                /* This code supports fonts in the expected
                   manor, but not in the usual IPF way. In IPF, font switching
                   (including sizing) is NEVER done, except to Courier for
                   examples. So, this code is inappropriate */

                if( stricmp( ptr, Fonttype_roman ) == 0 ) {
                    strcat( buf, "'Tms Rmn'" );
                } else if( stricmp( ptr, Fonttype_helv ) == 0 ) {
                    strcat( buf, "Helv" );
                } else {
                    /* Symbol doesn't work,so use courier instead */
                    strcat( buf, "Courier" );
                }
                line_len += trans_add_str( buf, section );
                ptr = strchr( end, WHP_FONTTYPE );
                *ptr++ = '\0';
                sprintf( buf, " size=%dx10.", (int)strtol( end, NULL, 10 ) );
            } else {
                /* this code turns all font changes to the default system
                   font, except for Courier. This is the normal IPF way */

                strcat( buf, "Courier" );
                if( stricmp( ptr, Fonttype_courier ) == 0 ) {
                    strcat( buf, " size=12x10." );
                } else {
                    /* default system font */
                    strcat( buf, " size=0x0." );
                }
                ptr = strchr( end, WHP_FONTTYPE ) + 1;
            }
            line_len += trans_add_str( buf, section );
            break;
        default:
            ptr++;
            Curr_ctx->empty = false;
            if( Tab_xmp && ch == Tab_xmp_char ) {
                len = tab_align( ch_len, section );
                ch_len += len;
                line_len += len;
                ptr = skip_blanks( ptr );
            } else {
                line_len += trans_add_char_ipf( ch, section );
                ch_len++;
            }
            if( line_len > 120 && ch == ' ' && !Tab_xmp ) {
                /* break onto the next line */
                line_len = 0;
                trans_add_char( '\n', section );
            }
            break;
        }
    }
}

static void output_hdr( void )
/****************************/
{
    whp_fprintf( Out_file, ":userdoc.\n" );
    if( Ipf_or_Html_title != NULL && Ipf_or_Html_title[0] != '\0' ) {
        whp_fprintf( Out_file, ":title.%s\n", Ipf_or_Html_title );
    }
    whp_fprintf( Out_file, ":docprof toc=123456.\n" );
}

static void output_ctx_hdr( ctx_def *ctx )
/****************************************/
{
    int                         head_level;
    char                        *ptr;
    keyword_def                 *key;
    keylist_def                 *keylist;
    int                         p_skip;

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

    whp_fprintf( Out_file, "\n:h%d res=%d id=%s.%s\n",
                                head_level, ctx->ctx_id, ctx->ctx_name,
                                translate_str_ipf( ctx->title ) );

    if( ctx->keylist != NULL ) {
        for( keylist = ctx->keylist; keylist != NULL; keylist = keylist->next ) {
            key = keylist->key;
            ptr = key->keyword;
            if( !key->duplicate ) {
                whp_fprintf( Out_file, ":i1." );
            } else {
                if( key->defined_ctx == ctx ) {
                    /* this is the first instance. :i1 and :i2 */
                    whp_fprintf( Out_file, ":i1 id=%d.%s\n",
                                    key->id, translate_str_ipf( ptr ) );
                }

                if( stricmp( ptr, ctx->title ) == 0 ) {
                    /* we are about to out an index subentry whose
                       name is the same as the main index entry!
                       Skip it! */
                    continue;
                }
                whp_fprintf( Out_file, ":i2 refid=%d.", key->id );
                ptr = ctx->title;
            }
            whp_fprintf( Out_file, "%s\n", translate_str_ipf( ptr ) );
        }
    }
    if( Ipf_or_Html_Real_font ) {
        /* The default font is system, which wouldn't be right */
        whp_fprintf( Out_file, ":font facename=Helv size=10x10.\n" );
    }
    /* browse lists are not used in IPF */
    /* nor does 'Up' topicing have any relevance */
}

static void output_end( void )
/****************************/
{
    whp_fprintf( Out_file, "\n:euserdoc.\n" );
}

static void output_ctx_sections( ctx_def *ctx )
/*********************************************/
{
    section_def                 *section;

    for( section = ctx->section_list; section != NULL; section = section->next ) {
        if( section->section_size > 0 ) {
            whp_fwrite( Out_file, section->section_text, 1, section->section_size );
        }
    }
}

void ipf_output_file( void )
/**************************/
{
    ctx_def                     *ctx;

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

void ipf_init_whp( void )
{
}
