/****************************************************************************
*
*                            Open Watcom Project
*
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

static int Curr_head_level = 0;
static int Curr_head_skip = 0;

#define BOX_LINE_SIZE   200

#define FONT_STYLE_BOLD         1
#define FONT_STYLE_ITALIC       2
#define FONT_STYLE_UNDERLINE    4

static char     *Font_match[]={
    ":hp1.:ehp1",               // 0: PLAIN
    ":hp2.",                    // 1: BOLD
    ":hp1.",                    // 2: ITALIC
    ":hp3.",                    // 3: BOLD + ITALIC
    ":hp5.",                    // 4: UNDERLINE
    ":hp7.",                    // 5: BOLD + UNDERLINE
    ":hp6.",                    // 6: ITALIC + UNDERLINE
    ":hp7.",                    // 7: BOLD + ITALIC + UNDERLINE (can't do it)
};

static char     *Font_end[]={
    "",                         // 0: PLAIN
    ":ehp2.",                   // 1: BOLD
    ":ehp1.",                   // 2: ITALIC
    ":ehp3.",                   // 3: BOLD + ITALIC
    ":ehp5.",                   // 4: UNDERLINE
    ":ehp7.",                   // 5: BOLD + UNDERLINE
    ":ehp6.",                   // 6: ITALIC + UNDERLINE
    ":ehp7.",                   // 7: BOLD + ITALIC + UNDERLINE (can't do it)
};

static int Font_list[100];      // up to 100 nested fonts
static int Font_list_curr= 0;

static bool Blank_line_pfx=FALSE;
static bool Blank_line_sfx=TRUE;

#define IPF_TRANS_LEN           50

static char *Trans_str=NULL;
static int Trans_len=0;

#define MAX_TABS                100     // up to 100 tab stops
static int Tab_list[MAX_TABS];

static void draw_line(
/********************/

    section_def         *section,
    int                 *alloc_size
) {
    int                 i;

    trans_add_str( ":cgraphic.\n", section, alloc_size );
    for( i = BOX_LINE_SIZE; i > 0; --i ) {
        trans_add_char( 196, section, alloc_size );
    }
    trans_add_str( "\n:ecgraphic.\n", section, alloc_size );
}

static int translate_char_ipf(
/****************************/

    int                 ch,
    char                *buf
) {
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

static char *translate_str_ipf(
/*****************************/

    char                *str
) {
    char                *t_str;
    int                 len;
    char                buf[IPF_TRANS_LEN];
    char                *ptr;

    len = 1;
    for( t_str = str; *t_str != '\0'; ++t_str ) {
        len += translate_char_ipf( *(unsigned char *)t_str, buf );
    }
    if( len > Trans_len ) {
        if( Trans_str != NULL ) {
            _free( Trans_str );
        }
        _new( Trans_str, len );
        Trans_len = len;
    }
    ptr = Trans_str;
    for( t_str = str; *t_str != '\0'; ++t_str ) {
        len = translate_char_ipf( *(unsigned char *)t_str, buf );
        strcpy( ptr, buf );
        ptr += len;
    }
    *ptr = '\0';

    return( Trans_str );
}

static int trans_add_char_ipf(
/****************************/

    int                 ch,
    section_def         *section,
    int                 *alloc_size
) {
    char                buf[IPF_TRANS_LEN];

    translate_char_ipf( ch, buf );
    return( trans_add_str( buf, section, alloc_size ) );
}

static int trans_add_str_ipf(
/***************************/

    char                *str,
    section_def         *section,
    int                 *alloc_size
) {
    int                 len;

    len = 0;
    for( ; *str != '\0'; ++str ) {
        len += trans_add_char_ipf( *(unsigned char *)str, section, alloc_size );
    }

    return( len );
}

static int trans_add_list(
/************************/

    char                *list,
    section_def         *section,
    int                 *alloc_size,
    char                *ptr
) {
    int                 len;

    len = trans_add_str( list, section, alloc_size );
    ++ptr;
    if( *ptr == 'c' ) {
        len += trans_add_str( " compact", section, alloc_size );
    }

    len += trans_add_str( ".\n", section, alloc_size );

    return( len );
}

static void read_tabs(
/********************/

    char                *tab_line
) {
    char                *ptr;
    int                 i;
    int                 tabcol;

    Tab_xmp_char = *tab_line;

    ptr = strtok( tab_line + 1, " " );
    for( tabcol = 0, i = 0 ; ptr != NULL; ptr = strtok( NULL, " " ), ++i ) {
        if( *ptr == '+' ) {
            tabcol += atoi( ptr + 1 );
        } else {
            tabcol = atoi( ptr );
        }
        Tab_list[i] = tabcol;
    }
    Tab_list[i] = -1;
}

static int tab_align(
/*******************/

    int                 ch_len,
    section_def         *section,
    int                 *alloc_size
) {
    int                 i;
    int                 len;

    // find the tab we should use
    i = 0;
    while( ch_len >= Tab_list[i]) {
        if( Tab_list[i] == -1 ) break;
        ++i;
    }

    len = 1;
    if( Tab_list[i] != -1 ) {
        len =  Tab_list[i] - ch_len;
    }
    for( i = len; i > 0; --i ) {
        trans_add_char_ipf( ' ', section, alloc_size );
    }

    return( len );
}

void ipf_topic_init(
/******************/

    void
) {
}

int ipf_trans_line(
/*****************/

    section_def         *section,
    int                 alloc_size
) {
    char                *ptr;
    char                *end;
    int                 ch;
    char                *ctx_name;
    char                *ctx_text;
    char                buf[500];
    int                 font_idx;
    int                 line_len;
    bool                term_fix;
    int                 ch_len;
    int                 len;
    char                *file_name;

    /* check for special column 0 stuff first */
    ptr = Line_buf;
    ch = *(unsigned char *)ptr;
    ch_len = 0;
    line_len = 0;

    switch( ch ) {
    case CH_TABXMP:
        if( *skip_blank( ptr + 1 ) == '\0' ) {
            Tab_xmp = FALSE;
            trans_add_str( ":exmp.\n", section, &alloc_size );
            Blank_line_sfx = FALSE;     // remove following blanks
        } else {
            read_tabs( ptr + 1 );
            trans_add_str( ":xmp.\n", section, &alloc_size );
            Tab_xmp = TRUE;
            Blank_line_pfx = FALSE;     // remove preceding blanks
        }
        return( alloc_size );

    case CH_BOX_ON:
        /* Table support is the closest thing to boxing in IPF, but it
           doesn't work well with changing fonts on items in the tables
           (the edges don't line up). So we draw long lines at the
           top and bottom instead */
        draw_line( section, &alloc_size );
        Blank_line_pfx = FALSE;
        return( alloc_size );

    case CH_BOX_OFF:
        draw_line( section, &alloc_size );
        Blank_line_sfx = FALSE;
        return( alloc_size );

    case CH_OLIST_START:
        trans_add_list( ":ol", section, &alloc_size, ptr );
        Blank_line_pfx = FALSE;
        return( alloc_size );

    case CH_LIST_START:
        trans_add_list( ":ul", section, &alloc_size, ptr );
        Blank_line_pfx = FALSE;
        return( alloc_size );

    case CH_DLIST_START:
        trans_add_str( ":dl break=all tsize=5.\n", section, &alloc_size );
        Blank_line_pfx = FALSE;
        return( alloc_size );

    case CH_SLIST_START:
        trans_add_list( ":sl", section, &alloc_size, ptr );
        Blank_line_pfx = FALSE;
        return( alloc_size );

    case CH_SLIST_END:
        trans_add_str( ":esl.\n", section, &alloc_size );
        Blank_line_sfx = FALSE;
        return( alloc_size );

    case CH_OLIST_END:
        trans_add_str( ":eol.\n", section, &alloc_size );
        Blank_line_sfx = FALSE;
        return( alloc_size );

    case CH_LIST_END:
        trans_add_str( ":eul.\n", section, &alloc_size );
        Blank_line_sfx = FALSE;
        return( alloc_size );

    case CH_DLIST_END:
        trans_add_str( ":edl.\n", section, &alloc_size );
        Blank_line_sfx = FALSE;
        return( alloc_size );

    case CH_LIST_ITEM:
    case CH_DLIST_TERM:
        /* eat blank lines before list items and terms */
        Blank_line_pfx = FALSE;
        break;

    case CH_CTX_KW:
        ptr = whole_keyword_line( ptr );
        if( ptr == NULL ) {
            return( alloc_size );
        }
        break;
    }

    if( *skip_blank( ptr ) == '\0' ) {
        /* ignore blanks lines before the topic starts */
        if( !Curr_ctx->empty ) {
            /* the line is completely blank. This tells us to output
               a blank line. BUT, all lists and things automatically
               generate blank lines before they display, so we
               must pend the line */
            Blank_line_pfx = TRUE;
        }
        return( alloc_size );
    }

    /* An explanation of 'Blank_line_pfx': when we hit a blank line,
       we set Blank_line_pfx to TRUE. On the non-tag next line, the
       blank line is generated.
       Some tags automatically generate a blank line, so they
       turn this flag off. This causes the next non-tag line to NOT
       put out the blank line */

    if( Blank_line_pfx ) {
        if( Blank_line_sfx ) {
            line_len += trans_add_str( ".br\n", section, &alloc_size );
        }
        Blank_line_pfx = FALSE;
    }

    /* An explanation of 'Blank_line_sfx': some ending tags automatically
       generate a blank line, so no blank line after them should get
       generated. Normally, this flag is set to TRUE, but ending
       tags and Defn list term tags set this FALSE, so no extra '.br'
       is generated.
       But, this rule only applies if a blank line immediately
       follows the tag, so its reset here regardless */

    Blank_line_sfx = TRUE;

    ch = *(unsigned char *)ptr;
    if( ch != CH_LIST_ITEM && ch != CH_DLIST_TERM && ch != CH_DLIST_DESC && !Tab_xmp ) {
        /* a .br in front of li and dt would generate extra spaces */
        line_len += trans_add_str( ".br\n", section, &alloc_size );
    }

    term_fix = FALSE;
    for( ;; ) {
        ch = *(unsigned char *)ptr;
        if( ch == '\0' ) {
            if( term_fix ) {
                trans_add_str( ":ehp2.", section, &alloc_size );
                term_fix = FALSE;
            }
            trans_add_char( '\n', section, &alloc_size );
            break;
        } else if( ch == CH_HLINK || ch == CH_DFN ) {
            Curr_ctx->empty = FALSE;
            /* there are no popups in IPF, so treat them as links */
            ctx_name = ptr + 1;
            ptr = strchr( ptr + 1, ch );
            if( ptr == NULL ) {
                error( ERR_BAD_LINK_DFN, TRUE );
            }
            *ptr = '\0';
            ctx_text = ptr + 1;
            ptr = strchr( ctx_text + 1, ch );
            if( ptr == NULL ) {
                error( ERR_BAD_LINK_DFN, TRUE );
            }
            *ptr = '\0';
            add_link( ctx_name );
            sprintf( buf, ":link reftype=hd refid=%s.", ctx_name );
            line_len += trans_add_str( buf, section, &alloc_size );
            line_len += trans_add_str_ipf( ctx_text, section, &alloc_size );
            ch_len += strlen( ctx_text );
            line_len += trans_add_str( ":elink.", section, &alloc_size );
            ++ptr;
        } else if( ch == CH_FLINK ) {
            Curr_ctx->empty = FALSE;
            file_name = strchr( ptr + 1, ch );
            if( file_name == NULL ) {
                error( ERR_BAD_LINK_DFN, TRUE );
            }
            ctx_name = strchr( file_name + 1, ch );
            if( ctx_name == NULL ) {
                error( ERR_BAD_LINK_DFN, TRUE );
            }
            ctx_text = strchr( ctx_name + 1, ch );
            if( ctx_text == NULL ) {
                error( ERR_BAD_LINK_DFN, TRUE );
            }
            *ctx_text = '\0';
            ctx_text = ctx_name + 1;
            *ctx_name = '\0';
            ctx_name = file_name + 1;
            *file_name = '\0';
            file_name = ptr + 1;
            sprintf( buf, ":link reftype=launch object='view.exe' "
                          "data='%s %s'.", file_name, ctx_name );
            line_len += trans_add_str( buf, section, &alloc_size );
            line_len += trans_add_str_ipf( ctx_text, section, &alloc_size );
            ch_len += strlen( ctx_text );
            line_len += trans_add_str( ":elink.", section, &alloc_size );
            ptr = ctx_text + strlen( ctx_text ) + 1;
        } else if( ch == CH_LIST_ITEM ) {
            /* list item */
            line_len += trans_add_str( ":li.", section, &alloc_size );
            ptr = skip_blank( ptr + 1 );
        } else if( ch == CH_DLIST_DESC ) {
            trans_add_str( ":dd.", section, &alloc_size );
            ptr = skip_blank( ptr + 1 );
        } else if( ch == CH_DLIST_TERM ) {
            /* definition list term */
            ptr ++;
            while( *ptr == ' ' ) {
                /* kludge fix cuz of GML: GML thinks that keywords are
                   are real words, so it puts a space after them.
                   This should fix that */
                ++ptr;
            }
            if( *ptr == CH_FONTSTYLE_START ) {  /* avoid nesting */
                --ptr;
                line_len += trans_add_str( ":dt.", section, &alloc_size );
	            ptr = skip_blank( ptr + 1 );
                Blank_line_sfx = FALSE;
            } else {
                --ptr;
                line_len += trans_add_str( ":dt.:hp2.", section, &alloc_size );
                term_fix = TRUE;
                ptr = skip_blank( ptr + 1 );
                Blank_line_sfx = FALSE;
            }
        } else if( ch == CH_CTX_KW ) {
            end = strchr( ptr + 1, CH_CTX_KW );
            memcpy( buf, ptr + 1, end - ptr - 1 );
            buf[end - ptr - 1] = '\0';
            add_ctx_keyword( Curr_ctx, buf );
            ptr = end + 1;
            if( *ptr == ' ' ) {
                /* kludge fix cuz of GML: GML thinks that keywords are
                   are real words, so it puts a space after them.
                   This should fix that */
                ++ptr;
            }
        } else if( ch == CH_PAR_RESET ) {
            /* this can be ignored for IPF */
            ++ptr;
        } else if( ch == CH_BMP ) {
            Curr_ctx->empty = FALSE;
            ++ptr;
            ch = *(unsigned char *)ptr;
            ptr += 2;
            end = strchr( ptr, CH_BMP );
            *end = '\0';
            switch( ch ) {
            case 'i':
                sprintf( buf, ":artwork runin name='%s'.", ptr );
                break;

            case 'l':
                sprintf( buf, ":artwork align=left name='%s'.", ptr );
                break;

            case 'r':
                sprintf( buf, ":artwork align=right name='%s'.", ptr );
                break;

            case 'c':
                sprintf( buf, ":artwork align=center name='%s'.", ptr );
                break;
            }
            line_len += trans_add_str( buf, section, &alloc_size );
            ptr = end + 1;
        } else if( ch == CH_FONTSTYLE_START ) {
            ++ptr;
            end = strchr( ptr, CH_FONTSTYLE_START );
            font_idx = 0;
            for( ; ptr != end; ++ptr ) {
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
            line_len += trans_add_str( Font_match[font_idx],
                                                section, &alloc_size );
            Font_list[Font_list_curr] = font_idx;
            ++Font_list_curr;
            ++ptr;
        } else if( ch == CH_FONTSTYLE_END ) {
            --Font_list_curr;
            line_len += trans_add_str( Font_end[Font_list[Font_list_curr]],
                                                section, &alloc_size );
            ++ptr;
        } else if( ch == CH_FONTTYPE ) {
            ++ptr;
            end = strchr( ptr, CH_FONTTYPE );
            *end = '\0';
            strcpy( buf, ":font facename=" );

            if( Real_ipf_font ) {
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
                line_len += trans_add_str( buf, section, &alloc_size );
                ptr = end + 1;
                end = strchr( ptr, CH_FONTTYPE );
                *end = '\0';
                sprintf( buf, " size=%dx10.", atoi( ptr ) );
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
                ptr = end + 1;
                end = strchr( ptr, CH_FONTTYPE );
            }

            line_len += trans_add_str( buf, section, &alloc_size );
            ptr = end + 1;
        } else {
            ++ptr;
            Curr_ctx->empty = FALSE;
            if( Tab_xmp && ch == Tab_xmp_char ) {
                len = tab_align( ch_len, section, &alloc_size );
                ch_len += len;
                line_len += len;
                ptr = skip_blank( ptr );
            } else {
                line_len += trans_add_char_ipf( ch, section, &alloc_size );
                ++ch_len;
            }
            if( line_len > 120 && ch == ' ' && !Tab_xmp ) {
                /* break onto the next line */
                line_len = 0;
                trans_add_char( '\n', section, &alloc_size );
            }
        }
    }

    return( alloc_size );
}

static void output_hdr(
/*********************/

    void
) {
    whp_fprintf( Out_file, ":userdoc.\n" );
    if( Ipf_title != NULL && Ipf_title[0] != '\0' ) {
        whp_fprintf( Out_file, ":title.%s\n", Ipf_title );
    }
    whp_fprintf( Out_file, ":docprof toc=123456.\n" );
}

static void output_ctx_hdr(
/*************************/

    ctx_def                     *ctx
) {
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
        for( keylist = ctx->keylist; keylist != NULL;
                                            keylist = keylist->next ) {
            key = keylist->key;
            ptr = key->keyword;
            if( !key->duplicate ) {
                fputs( ":i1.", Out_file );
            } else {
                if( key->defined_ctx == ctx ) {
                    /* this is the first instance. :i1 and :i2 */
                    fprintf( Out_file, ":i1 id=%d.%s\n",
                                    key->id, translate_str_ipf( ptr ) );
                }

                if( stricmp( ptr, ctx->title ) == 0 ) {
                    /* we are about to out an index subentry whose
                       name is the same as the main index entry!
                       Skip it! */
                    continue;
                }
                fprintf( Out_file, ":i2 refid=%d.", key->id );
                ptr = ctx->title;
            }
            fputs( translate_str_ipf( ptr ), Out_file );
            fputc( '\n', Out_file );
        }
    }
    if( Real_ipf_font ) {
        /* The default font is system, which wouldn't be right */
        whp_fprintf( Out_file, ":font facename=Helv size=10x10.\n" );
    }
    /* browse lists are not used in IPF */
    /* nor does 'Up' topicing have any relevance */
}

static void output_end(
/*********************/

    void
) {
    whp_fprintf( Out_file, "\n:euserdoc.\n" );
}

static void output_ctx_sections(
/******************************/

    ctx_def                     *ctx
) {
    section_def                 *section;

    for( section = ctx->section_list; section != NULL; ) {
        if( section->section_size > 0 ) {
            whp_fwrite( section->section_text, 1,
                                        section->section_size, Out_file );
        }
        section = section->next;
    }
}

void ipf_output_file(
/*******************/

    void
) {
    ctx_def                     *ctx;

    output_hdr();
    for( ctx = Ctx_list; ctx != NULL; ctx = ctx->next ) {
        if( !Remove_empty || !ctx->empty || ctx->req_by_link ) {
            if( !Exclude_special || !is_special_topic( ctx, FALSE ) ) {
                output_ctx_hdr( ctx );
                output_ctx_sections( ctx );
            }
        }
    }
    output_end();
}

