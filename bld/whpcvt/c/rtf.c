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
* Description:  This file defines the RTF specific functions.
*
****************************************************************************/

#include "whpcvt.h"

#include "clibext.h"


#define RTF_CHAR_SIZE   180

#define RTF_TRANS_LEN   50

typedef enum {
    LPREFIX_NONE        = 0,
    LPREFIX_S_LIST      = 0x01,
    LPREFIX_FIX_FI      = 0x02,
    LPREFIX_E_LIST      = 0x04,
    LPREFIX_PAR_RESET   = 0x08,
    LPREFIX_LEFTALIGN   = 0x10,
    LPREFIX_BOX_ON      = 0x20,
} line_prefix;

typedef enum {
    LPOSTFIX_NONE,
    LPOSTFIX_TERM,
} line_postfix;

static line_prefix      Line_prefix = LPREFIX_NONE;
static line_postfix     Line_postfix = LPOSTFIX_NONE;

static char             Reset_font_str[] = "\\plain\\f2\\fs20";
static char             Rtf_ctl_prefix[] = "{\\footnote \\pard\\plain \\sl240 \\fs20 ";

static bool             Blank_line = false;
static int              Curr_indent = 0;
static bool             Eat_blanks = false;

static char             *Trans_str = NULL;
static size_t           Trans_len = 0;

static void trans_add_tabs( section_def *section )
/************************************************/
{
    int                 tab;
    char                buf[30];

    trans_add_str( "\\pard ", section );
    for( tab = 1; tab <= NUM_TAB_STOPS; tab++ ) {
        sprintf( buf, "\\tx%d ", INDENT_INC * tab );
        trans_add_str( buf, section );
    }
    sprintf( buf, "\\li%d ", Curr_indent );
    trans_add_str( buf, section );
}

static size_t translate_char_rtf( char ch, char *buf, bool do_quotes )
/*******************************************************************/
{
    switch( ch ) {
    case '}':
        strcpy( buf,  "\\}" );
        break;
    case '{':
        strcpy( buf, "\\{" );
        break;
    case '\\':
        strcpy( buf, "\\\\" );
        break;
    case '"':
        if( do_quotes ) {
            strcpy( buf, "\\\"" );
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

static char *translate_str_rtf( const char *str, bool do_quotes )
/***************************************************************/
{
    const char          *t_str;
    size_t              len;
    char                buf[RTF_TRANS_LEN];
    char                *ptr;

    len = 1;
    for( t_str = str; *t_str != '\0'; t_str++ ) {
        len += translate_char_rtf( *t_str, buf, do_quotes );
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
        len = translate_char_rtf( *t_str, buf, do_quotes );
        strcpy( ptr, buf );
        ptr += len;
    }
    *ptr = '\0';

    return( Trans_str );
}

static size_t trans_add_char_rtf( char ch, section_def *section )
/***************************************************************/
{
    char        buf[RTF_TRANS_LEN];

    translate_char_rtf( ch, buf, false );
    return( trans_add_str( buf, section ) );
}

static void add_tabxmp( section_def *section )
/********************************************/
{
    char            buf[50];
    tab_size        tabcol;
    int             i;

    trans_add_str( "\\pard ", section );
    for( i = 0; (tabcol = Tabs_get( i )) != TAB_UNDEF; i++ ) {
        sprintf( buf, "\\tx%u ", RTF_CHAR_SIZE * tabcol );
        trans_add_str( buf, section );
    }
    trans_add_char( '\n', section );
}

void rtf_topic_init( void )
/*************************/
{
    Line_prefix = LPREFIX_NONE;
}

static size_t trans_add_str_nobreak( const char *str, section_def *section )
/**************************************************************************/
{
    size_t      len;

    len = 0;
    for( ; *str != '\0'; str++ ) {
        if( *str != ' ' || Break_link ) {
            len = trans_add_char( *str, section );
        } else {
            /* non-breaking space */
            len = trans_add_char( '\\', section );
            len += trans_add_char( '~', section );
        }
    }

    return( len );
}

void rtf_trans_line( char *line_buf, section_def *section )
/*********************************************************/
{
    char                *ptr;
    char                *end;
    char                ch;
    char                *ctx_name;
    char                *ctx_text;
    char                buf[100];
    int                 indent;
    char                *file_name;
    size_t              len;

    /* check for special column 0 pre-processing stuff first */
    ptr = line_buf;
    ch = *ptr;
    indent = 0;

    if( Blank_line && ( ch != WHP_LIST_ITEM || !Curr_list->compact ) ) {
        Blank_line = false;
    }
    switch( ch ) {
    case WHP_TABXMP:
        ptr = skip_blanks( ptr + 1 );
        if( *ptr == '\0' ) {
            Line_prefix |= LPREFIX_PAR_RESET;
            Tab_xmp = false;
        } else {
            Tab_xmp_char = *ptr++;
            Tabs_read( ptr );
            add_tabxmp( section );
            Tab_xmp = true;
        }
        return;
    case WHP_BOX_ON:
        Line_prefix |= LPREFIX_BOX_ON;
        return;
    case WHP_BOX_OFF:
        Line_prefix |= LPREFIX_PAR_RESET;
        return;
    case WHP_LIST_START:
    case WHP_DLIST_START:
    case WHP_OLIST_START:
    case WHP_SLIST_START:
        switch( ch ) {
        case WHP_LIST_START:
            indent = INDENT_INC + Start_inc_ul;
            break;
        case WHP_DLIST_START:
            indent = INDENT_INC + Start_inc_dl;
            break;
        case WHP_OLIST_START:
            indent = INDENT_INC + Start_inc_ol;
            break;
        case WHP_SLIST_START:
            indent = Start_inc_sl;
            if( indent == 0 && Curr_list->type == LIST_TYPE_SIMPLE ) {
                /* nested simple lists, with no pre-indent. Force an indent */
                indent = INDENT_INC;
            }
            break;
        }
        NewList( ptr, Curr_indent, false );
        Curr_indent += indent;
        if( ch != WHP_SLIST_START || indent != 0 ) {
            Line_prefix |= LPREFIX_S_LIST;
        }
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
        if( ch != WHP_SLIST_END || Curr_list->prev_indent != Curr_indent ) {
            Line_prefix |= LPREFIX_E_LIST;
        }
        Curr_indent = Curr_list->prev_indent;
        PopList();
        return;
    case WHP_DLIST_DESC:
        if( *skip_blanks( ptr + 1 ) == '\0' ) {
            /* no description on this line. Ignore it so that no
               blank line gets generated */
            return;
        }
        break;
    case WHP_CTX_KW:
        ptr = whole_keyword_line( ptr );
        if( ptr == NULL ) {
            return;
        }
        break;
    }

    if( *skip_blanks( ptr ) == '\0' && Curr_ctx->empty ) {
        /* skip preceding blank lines */
        return;
    }

    if( Blank_line ) {
        /* remove '\n' on the end */
        section->section_size--;
    } else {
        trans_add_str( "\\par ", section );
    }
    if( Line_prefix & LPREFIX_S_LIST ) {
        Line_prefix &= ~LPREFIX_S_LIST;
        sprintf( buf, "\\li%d ", Curr_indent );
        trans_add_str( buf, section );
    }
    if( Line_prefix & LPREFIX_FIX_FI ) {
        Line_prefix &= ~LPREFIX_FIX_FI;
        trans_add_str( "\\fi0 ", section );
    }
    if( Line_prefix & LPREFIX_E_LIST ) {
        Line_prefix &= ~LPREFIX_E_LIST;
        sprintf( buf, "\\li%d\\fi0 ", Curr_indent );
        trans_add_str( buf, section );
    }
    if( Line_prefix & LPREFIX_PAR_RESET ) {
        Line_prefix &= ~LPREFIX_PAR_RESET;
        trans_add_tabs( section );
    }
    if( Line_prefix & LPREFIX_LEFTALIGN ) {
        Line_prefix &= ~LPREFIX_LEFTALIGN;
        trans_add_str( "\\ql ", section );
    }
    if( Line_prefix & LPREFIX_BOX_ON ) {
        Line_prefix &= ~LPREFIX_BOX_ON;
        trans_add_str( "\\box ", section );
    }

    Blank_line = true;
    for( ;; ) {
        ch = *ptr;
        if( _is_nonblank( ch ) ) {
            Blank_line = false;
        }
        if( ch == '\0' ) {
            if( Line_postfix == LPOSTFIX_TERM ) {
                Line_postfix = LPOSTFIX_NONE;
                /* this must go on the end of the current line, otherwise
                   the bolding used on the term screws up the spacing
                   on the next line */
                trans_add_str( Reset_font_str, section );
                /* we still need to do the indent fixing on the next
                   line, though */
                Line_prefix |= LPREFIX_FIX_FI;
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
            switch( ch ) {
            case WHP_HLINK:
                add_link( ctx_name );
                trans_add_str( "{\\f2\\uldb ", section );
                trans_add_str_nobreak( translate_str_rtf( ctx_text, false ), section );
                trans_add_str( "\\v\\f2\\uldb ", section );
                trans_add_str( ctx_name, section );
                break;
            case WHP_DFN:
                add_link( ctx_name );
                trans_add_str( "{\\f2\\ul ", section );
                trans_add_str_nobreak( translate_str_rtf( ctx_text, false ), section );
                trans_add_str( "\\v\\f2\\ul ", section );
                trans_add_str( ctx_name, section );
                break;
            case WHP_FLINK:
                trans_add_str( "{\\f2\\uldb ", section );
                trans_add_str_nobreak( translate_str_rtf( ctx_text, false ), section );
                trans_add_str( "\\v\\f2\\uldb !JumpKeyword( \"", section );
                trans_add_str( file_name, section );
                trans_add_str( ".hlp\", \"", section );
                trans_add_str( translate_str_rtf( ctx_name, true ), section );
                trans_add_str( "\" );", section );
                break;
            }
            trans_add_str( "\\v0 ", section );
            trans_add_str( Reset_font_str, section );
            trans_add_str( "}", section );
            break;
        case WHP_LIST_ITEM:
            Curr_list->number++;
            if( Curr_list->type != LIST_TYPE_SIMPLE ) {
                if( Curr_list->type == LIST_TYPE_UNORDERED ) {
                    sprintf( buf, "\\fi-%d\\f1 \\'b7\\f2\\tab ", INDENT_INC );
                } else if( Curr_list->type == LIST_TYPE_ORDERED ) {
                    /* ordered list type */
                    sprintf( buf, "\\f2\\fi-%d\\b %d.\\plain\\f2\\fs20\\tab ", INDENT_INC, Curr_list->number );
                }
                trans_add_str( buf, section );
                Line_prefix |= LPREFIX_FIX_FI;
            }
            ptr = skip_blanks( ptr + 1 );
            Eat_blanks = true;
            break;
        case WHP_DLIST_DESC:
            /* we don't have to do anything with this for RTF. Ignore it */
            Curr_list->number++;
            ptr = skip_blanks( ptr + 1 );
            break;
        case WHP_DLIST_TERM:
            /* definition list term */
            sprintf( buf, "\\f2\\fi-%d \\b ", INDENT_INC );
            trans_add_str( buf, section );
            Line_postfix = LPOSTFIX_TERM;
            ptr = skip_blanks( ptr + 1 );
            Eat_blanks = true;
            break;
        case WHP_CTX_KW:
            end = strchr( ptr + 1, WHP_CTX_KW );
            len = end - ptr - 1;
            memcpy( buf, ptr + 1, len );
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
            Line_prefix |= LPREFIX_PAR_RESET;
            ptr++;
            break;
        case WHP_BMP:
            Curr_ctx->empty = false;
            ptr++;
            ch = *ptr;
            file_name = ptr + 2;
            ptr = strchr( file_name, WHP_BMP );
            *ptr++ = '\0';
            if( ch != 'c' ) {
                switch( ch ) {
                case 'i':
                    sprintf( buf, "\\{bmc %s\\}", file_name );
                    break;
                case 'l':
                    sprintf( buf, "\\{bml %s\\}", file_name );
                    break;
                case 'r':
                    sprintf( buf, "\\{bmr %s\\}", file_name );
                    break;
                default:
                    *buf = '\0';
                    break;
                }
            } else {
                /* centered bitmap are not directly supported in RTF */
                sprintf( buf, "\\qc \\{bmc %s\\}", file_name );
                Line_prefix |= LPREFIX_LEFTALIGN;
            }
            trans_add_str( buf, section );
            break;
        case WHP_FONTSTYLE_START:
            ptr++;
            end = strchr( ptr, WHP_FONTSTYLE_START );
            for( ; ptr != end; ptr++ ) {
                switch( *ptr ) {
                case 'b':
                    trans_add_str( "\\b ", section );
                    break;
                case 'i':
                    trans_add_str( "\\i ", section );
                    break;
                case 'u':
                    trans_add_str( "\\ul ", section );
                    break;
                case 's':
                    trans_add_str( "\\ulw ", section );
                    break;
                }
            }
            ptr++;
            break;
        case WHP_FONTSTYLE_END:
            trans_add_str( "\\b0 \\i0 \\ul0 \\ulw0 ", section );
            ptr++;
            break;
        case WHP_FONTTYPE:
            ptr++;
            end = strchr( ptr, WHP_FONTTYPE );
            *end++ = '\0';
            if( stricmp( ptr, Fonttype_roman ) == 0 ) {
                strcpy( buf, "\\f0 " );
            } else if( stricmp( ptr, Fonttype_symbol ) == 0 ) {
                strcpy( buf, "\\f1 " );
            } else if( stricmp( ptr, Fonttype_courier ) == 0 ) {
                strcpy( buf, "\\f4 " );
            } else {
                strcpy( buf, "\\f2 " );
            }
            trans_add_str( buf, section );
            ptr = strchr( end, WHP_FONTTYPE );
            *ptr++ = '\0';
            sprintf( buf, "\\fs%d ", (int)( strtol( end, NULL, 10 ) * 2 ) );
            trans_add_str( buf, section );
            break;
        default:
            ptr++;
            if( !Eat_blanks || ch != ' ' ) {
                Curr_ctx->empty = false;
                if( Tab_xmp && ch == Tab_xmp_char ) {
                    trans_add_str( "\\tab ", section );
                    /* skip blanks after a tab, so that hyperlinks line up */
                    ptr = skip_blanks( ptr );
                } else {
                    trans_add_char_rtf( ch, section );
                }
                Eat_blanks = false;
            }
            break;
        }
    }
}

static void print_tab_stops( void )
/*********************************/
{
    int                 tab;

    for( tab = 1; tab <= NUM_TAB_STOPS; tab++ ) {
        whp_fprintf( Out_file, "\\tx%d ", INDENT_INC * tab );
    }
}

static void output_hdr( void )
/****************************/
{
    /* the header is printed inline so that the -zc compiler option
       will place these strings into the code segment.

       This header is originally derived from some WinWord document
       saved in RTF format. The copy used here comes from the
       file 'header.rtf' in the database WINHELP directory */

    whp_fprintf( Out_file, "{\\rtf1\\ansi \\deff2{\\fonttbl{\\f0\\froman Tms Rmn;}{\\f1\\fdecor Symbol;}{\\f2\\fswiss Helv;}{\\f3\\fmodern pica;}{\\f4\\fmodern Courier;}{\\f5\\fmodern elite;}{\\f6\\fmodern prestige;}{\\f7\\fmodern lettergothic;}{\\f8\\fmodern gothicPS;}\n" );
    whp_fprintf( Out_file, "{\\f9\\fmodern cubicPS;}{\\f10\\fmodern lineprinter;}{\\f11\\fswiss Helvetica;}{\\f12\\fmodern avantegarde;}{\\f13\\fmodern spartan;}{\\f14\\fmodern metro;}{\\f15\\fmodern presentation;}{\\f16\\fmodern APL;}{\\f17\\fmodern OCRA;}{\\f18\\fmodern OCRB;}{\\f19\\froman boldPS;}\n" );
    whp_fprintf( Out_file, "{\\f20\\froman emperorPS;}{\\f21\\froman madaleine;}{\\f22\\froman zapf humanist;}{\\f23\\froman classic;}{\\f24\\froman roman f;}{\\f25\\froman roman g;}{\\f26\\froman roman h;}{\\f27\\froman timesroman;}{\\f28\\froman century;}{\\f29\\froman palantino;}\n" );
    whp_fprintf( Out_file, "{\\f30\\froman souvenir;}{\\f31\\froman garamond;}{\\f32\\froman caledonia;}{\\f33\\froman bodini;}{\\f34\\froman university;}{\\f35\\fscript Script;}{\\f36\\fscript scriptPS;}{\\f37\\fscript script c;}{\\f38\\fscript script d;}{\\f39\\fscript commercial script;}\n" );
    whp_fprintf( Out_file, "{\\f40\\fscript park avenue;}{\\f41\\fscript coronet;}{\\f42\\fscript script h;}{\\f43\\fscript greek;}{\\f44\\froman kana;}{\\f45\\froman hebrew;}{\\f46\\froman roman s;}{\\f47\\froman russian;}{\\f48\\froman roman u;}{\\f49\\froman roman v;}{\\f50\\froman roman w;}\n" );
    whp_fprintf( Out_file, "{\\f51\\fdecor narrator;}{\\f52\\fdecor emphasis;}{\\f53\\fdecor zapf chancery;}{\\f54\\fdecor decor d;}{\\f55\\fdecor old english;}{\\f56\\fdecor decor f;}{\\f57\\fdecor decor g;}{\\f58\\fdecor cooper black;}{\\f59\\fnil linedraw;}{\\f60\\fnil math7;}{\\f61\\fnil math8;}\n" );
    whp_fprintf( Out_file, "{\\f62\\fnil bar3of9;}{\\f63\\fnil EAN;}{\\f64\\fnil pcline;}{\\f65\\fnil tech h;}{\\f66\\fswiss Helvetica-Narrow;}{\\f67\\fmodern Modern;}{\\f68\\froman Roman;}}{\\colortbl;\\red0\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green255\\blue255;\\red0\\green255\\blue0;\n" );
    whp_fprintf( Out_file, "\\red255\\green0\\blue255;\\red255\\green0\\blue0;\\red255\\green255\\blue0;\\red255\\green255\\blue255;}{\\stylesheet{\\s244 \\fs16\\up6 \\sbasedon0\\snext0 footnote reference;}{\\s245 \\fs20 \\sbasedon0\\snext245 footnote text;}{\\s246\\li720 \\i\\fs20 \n" );
    whp_fprintf( Out_file, "\\sbasedon0\\snext255 heading 9;}{\\s247\\li720 \\i\\fs20 \\sbasedon0\\snext255 heading 8;}{\\s248\\li720 \\i\\fs20 \\sbasedon0\\snext255 heading 7;}{\\s249\\li720 \\fs20\\ul \\sbasedon0\\snext255 heading 6;}{\\s250\\li720 \\b\\fs20 \\sbasedon0\\snext255 heading 5;}{\\s251\\li360 \n" );
    whp_fprintf( Out_file, "\\ul \\sbasedon0\\snext255 heading 4;}{\\s252\\li360 \\b \\sbasedon0\\snext255 heading 3;}{\\s253\\sb120 \\b\\f2 \\sbasedon0\\snext0 heading 2;}{\\s254\\sb240 \\b\\f2\\ul \\sbasedon0\\snext0 heading 1;}{\\s255\\li720 \\fs20 \\sbasedon0\\snext255 Normal Indent;}{\\fs20 \n" );
    whp_fprintf( Out_file, "\\snext0 Normal;}{\\s2\\fi-240\\li480\\sb80\\tx480 \\f11 \\sbasedon0\\snext2 nscba;}{\\s3\\fi-240\\li240\\sa20 \\f11 \\sbasedon0\\snext3 j;}{\\s4\\li480\\sa20 \\f11 \\sbasedon0\\snext4 ij;}{\\s5\\sb80\\sa20 \\f11 \\sbasedon0\\snext5 btb;}{\\s6\\fi-240\\li2400\\sb20\\sa20 \\f11\\fs20 \n" );
    whp_fprintf( Out_file, "\\sbasedon0\\snext6 ctcb;}{\\s7\\fi-240\\li480\\sa40\\tx480 \\f11 \\sbasedon0\\snext7 ns;}{\\s8\\sa120 \\f11\\fs28 \\sbasedon0\\snext8 TT;}{\\s9\\fi-240\\li2400\\sa20 \\f11 \\sbasedon0\\snext9 crtj;}{\\s10\\fi-240\\li480\\tx480 \\f11 \\sbasedon0\\snext10 nsca;}{\\s11\\sa20 \\f11 \n" );
    whp_fprintf( Out_file, "\\sbasedon0\\snext11 bt;}{\\s12\\li240\\sb120\\sa40 \\f11 \\sbasedon0\\snext12 Hf;}{\\s13\\li240\\sb120\\sa40 \\f11 \\sbasedon0\\snext13 Hs;}{\\s14\\li480\\sb120\\sa40 \\f11 \\sbasedon0\\snext14 RT;}{\\s15\\fi-2160\\li2160\\sb240\\sa80\\tx2160 \\f11 \\sbasedon0\\snext15 c;}{\n" );
    whp_fprintf( Out_file, "\\s16\\li2160\\sa20 \\f11 \\sbasedon0\\snext16 ct;}{\\s17\\li240\\sa20 \\f11 \\sbasedon0\\snext17 it;}{\\s18\\li480 \\f11\\fs20 \\sbasedon0\\snext18 nsct;}{\\s19\\fi-160\\li400\\sb80\\sa40 \\f11 \\sbasedon0\\snext19 nscb;}{\\s20\\fi-2640\\li2880\\sb120\\sa40\\brdrb\\brdrs \\brdrbtw\\brdrs \n" );
    whp_fprintf( Out_file, "\\tx2880 \\f11 \\sbasedon0\\snext20 HC2;}{\\s21\\fi-2640\\li2880\\sb120\\sa20\\tx2880 \\f11 \\sbasedon0\\snext21 C2;}{\\s22\\fi-240\\li2400\\sa20 \\f11\\fs20 \\sbasedon0\\snext22 ctc;}{\\s23\\li2160\\sb160 \\f11 \\sbasedon0\\snext23 crt;}{\\s24\\li480\\sb20\\sa40 \\f11 \n" );
    whp_fprintf( Out_file, "\\sbasedon0\\snext24 or;}}{\\info{\\author WATCOM}{\\creatim\\yr2000\\mo1\\dy1}{\\version1}{\\edmins1}{\\nofpages0}{\\nofwords0}{\\nofchars0}{\\vern8310}}\\ftnbj \\sectd \\linex576\\endnhere\n" );

    whp_fprintf( Out_file, "\\pard\\plain \\sl240 \\fs20 \\f2 " );

    print_tab_stops();
    whp_fprintf( Out_file, "\n" );
}

static void output_ctx_hdr( ctx_def *ctx )
/****************************************/
{
    ctx_def                     *up_ctx;
    keylist_def                 *keylist;

    whp_fprintf( Out_file, "\\par \\pard \\page " );

    /* context id */
    whp_fprintf( Out_file, "#%s# %s}", Rtf_ctl_prefix, ctx->ctx_name );
    whp_fprintf( Out_file, "$%s$ %s}", Rtf_ctl_prefix, translate_str_rtf( ctx->title, false ) );
    if( Do_up ) {
        /* spit out up button stuff */
        whp_fprintf( Out_file, "!{\\footnote ! ChangeButtonBinding( \"btn_up\", " );
        for( up_ctx = ctx->up_ctx; up_ctx != NULL; up_ctx = up_ctx->up_ctx ) {
            if( !Remove_empty || !up_ctx->empty ) {
                break;
            }
        }
        if( up_ctx == NULL ) {
            whp_fprintf( Out_file, "\"Contents()\") }" );
        } else {
            whp_fprintf( Out_file, "\"JumpId( `%s\', `%s\')\") }", Help_File, up_ctx->ctx_name );
        }
    }

    if( ctx->keylist != NULL ) {
        whp_fprintf( Out_file, "K%sK ", Rtf_ctl_prefix );
        for( keylist = ctx->keylist; keylist != NULL;
                                            keylist = keylist->next ) {
            whp_fprintf( Out_file, ";%s",
                            translate_str_rtf( keylist->key->keyword, false ) );
        }
        whp_fprintf( Out_file, "}" );
    }
    if( ctx->browse != NULL ) {
        whp_fprintf( Out_file, "+%s+ %s:%.4d}",
                Rtf_ctl_prefix, translate_str_rtf( ctx->browse->browse_name, false ), ctx->browse_num );
    }
    if( ctx->title_fmt == TITLE_FMT_NOLINE || ( !Keep_titles && ctx->title_fmt != TITLE_FMT_LINE ) ) {
        whp_fprintf( Out_file, "{\\f2 \\fs28 %s }\\f2 \\fs20\n",
                                    translate_str_rtf( ctx->title, false ) );
    } else {
        whp_fprintf( Out_file, "{\\keepn \\f2 \\fs28 %s"
                " \\f2 \\fs20 \\par \\pard \\f2 \\fs20 ",
                                    translate_str_rtf( ctx->title, false ) );
        print_tab_stops();
        whp_fprintf( Out_file, "}\n" );
    }
}

static void output_end( void )
/****************************/
{
    whp_fprintf( Out_file, "\\par }\n" );
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

void rtf_output_file( void )
/**************************/
{
    ctx_def         *ctx;

    output_hdr();
    for( ctx = Ctx_list; ctx != NULL; ctx = ctx->next ) {
        if( !Remove_empty || !ctx->empty || ctx->req_by_link ) {
            output_ctx_hdr( ctx );
            output_ctx_sections( ctx );
        }
    }
    output_end();
}

void rtf_init_whp( void )
{
}
