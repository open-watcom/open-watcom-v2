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
* Description:  This file defines the InfoBench specific functions.
*
****************************************************************************/

#include "whpcvt.h"
#include <stdio.h>
#include <assert.h>
#include "wibhelp.h"

#include "clibext.h"


typedef enum {
    LPOSTFIX_NONE,
    LPOSTFIX_TERM,
} line_postfix;

// internal hyperlink symbol. Gets filtered out at output time
#define TEMP_HLINK_BREAK        (char)'\x7F'

// Some characters we use for graphics
#define BOX_VBAR                (char)'\xB3'
#define BOX_HBAR                (char)'\xC4'  // 196
#define BOX_CORNER_TOP_LEFT     (char)'\xDA'
#define BOX_CORNER_TOP_RIGHT    (char)'\xBF'
#define BOX_CORNER_BOTOM_LEFT   (char)'\xC0'
#define BOX_CORNER_BOTOM_RIGHT  (char)'\xD9'

// labels for speacial header buttons
#define HBUTTON_UP              " Up "
#define HBUTTON_PREV            " <<<< "
#define HBUTTON_NEXT            " >>>> "
#define HBUTTON_CONTENTS        " Contents "
#define HBUTTON_INDEX           " Index "
#define HBUTTON_KEYWORDS        " Keywords "

static bool             Blank_line = false;
static int              Curr_indent = 0;
static bool             list_indent = false;
static bool             Eat_blanks = false;

// The following are for word-wrapping and indentation support
static int              Cursor_X = 0;   // column number
static int              R_Chars = 0;    // visible chars since Wrap_Safe
static size_t           Wrap_Safe = 0;  // index of break candidate
static int              NL_Group = 0;   // Number of contiguous newlines

static bool             Box_Mode = false;

static line_postfix     Line_postfix = LPOSTFIX_NONE;

static int currentIndent( void )
{
    if( list_indent )
        return( Curr_indent + Text_Indent );
    return( Curr_indent );
}

// this function will change all of the spaces in a string into non-breaking
// spaces (character 0xFF ). It's currently only used for the labels on
// hyper-links to ensure that they do not get broken across lines as
// this is not allowed by the InfoBench grammar.
static void spaces_to_nobreak( char *str )
/****************************************/
{
    if( str != NULL ) {
        for( ; *str != '\0'; str++ ) {
            if( *str == ' ' ) {
                *str = IB_SPACE_NOBREAK;
            }
        }
    }
}

/* This function will return converted character to output buffer.
 * Return length of converted character for InfoBench.
 */
static int out_char_ib( char *out, char ch )
/******************************************/
{
    switch( ch ) {
    // The following characters should be preceded by an ESC character
    // Some could also be preceded by themselves, but this leads to
    // ambiguity. ie: what does <<< mean?
    case '}':
    case '{':
    case '<':
    case '>':
    case '"':
        *out++ = IB_ESCAPE;
        *out = ch;
        return( 2 );
    // The following characters are special to InfoBench, and there is no
    // way to represent them with the current grammar
    case IB_HLINK:
    case IB_HLINK_BREAK:
    case IB_ESCAPE:
        return( 0 );
    default:
        *out = ch;
        return( 1 );
    }
}

/* This function will do proper escaping or character substitution for
 * characters special to InfoBench and add them to the section text.
 */
static size_t trans_add_char_ib( char ch, section_def *section )
/**************************************************************/
{
    int         len;
    char        buffer[3];

    len = out_char_ib( buffer, ch );
    if( len > 0 ) {
        trans_add_char( buffer[0], section );
        if( len > 1 ) {
            trans_add_char( buffer[1], section );
        }
    }
    return( len );
}

/* This function will do proper escaping or character substitution for
 * characters special to InfoBench and send them to the output stream.
 */
static void str_out_ib( const char *str )
/***************************************/
{
    int         len;
    char        buffer[3];

    if( str != NULL ) {
        for( ; *str != '\0'; str++ ) {
            len = out_char_ib( buffer, *str );
            if( len > 0 ) {
                whp_fwrite( Out_file, buffer, 1, len );
            }
        }
    }
}

// The following two functions handle word-wrapping
static size_t trans_add_char_wrap( char ch, section_def *section )
/****************************************************************/
{
    int                 ctr;            // misc. counter
    size_t              wrap_amount;    // amount of wrapped text
    int                 shift;          // amount we need to shift text
    int                 delta;          // amount of whitespace we ignore
    int                 indent;         // actual indent
    size_t              len = 0;        // number of chars we just added

    // the "1" is because a character is allowed to appear on the right margin
    // the minus part is so that we leave enough room for the box chars
    #define MY_MARGIN ( Right_Margin + 1 - ( Box_Mode ? 2 : 0 ) )

    // find out the real indentation
    indent = currentIndent();

    // add character
    if( ch == '\n' ) {
        NL_Group++;
        if( Box_Mode ) {
            // Add left side if necessary:
            for( ; Cursor_X < indent; Cursor_X++ ) {
                trans_add_char( ' ', section );
            }
            if( Cursor_X == indent ) {
                trans_add_char( BOX_VBAR, section );
                Cursor_X++;
            }

            // Now add right side:
            // the "- 1" is for the BOX_VBAR
            for( ; Cursor_X < Right_Margin - 1; Cursor_X++ ) {
                trans_add_char( ' ', section );
            }
            // Now add vertical bar if room (there should be...)
            if( Cursor_X == Right_Margin - 1 ) {
                trans_add_char( BOX_VBAR, section );
            }
        }
        R_Chars = 0;
        Cursor_X = 0;
        Wrap_Safe = section->section_size;
    } else {
        // We assume all other characters will advance one char by default
        R_Chars++;
        Cursor_X++;
    }

    // now we actually add the character to our buffer
    len = trans_add_char_ib( ch, section );

    // adjust the nearest safe break point if the char we got was a space and
    // is not preceded by a space
    if( ch == ' ' && section->section_size > 2
      && section->section_text[section->section_size - 2] != ' ' ) {
        Wrap_Safe = section->section_size;
        R_Chars = 0;
    }

    // If a bunch of spaces are pushing us over the edge then we'll rip all
    // of them off except one. Only happens in extreme cases. (ie: wdbg.whp)
    if( Cursor_X >= MY_MARGIN && ch == ' ' ) {
        while( section->section_size > 2 && section->section_text[section->section_size - 2] == ' ' ) {
            section->section_size--;
            Cursor_X--;
        }
    }

    // if this assertion fails then the wrapping code will break. Hopefully
    // this cannot happen...
//    assert( Cursor_X <= MY_MARGIN );

    // if we need to wrap...
    if( Cursor_X == MY_MARGIN ) {
        // find out how many characters we need to move to next line
        if( R_Chars == MY_MARGIN - indent ) {
            // if the current word won't even fit on the next line then we
            // break it at the margin, so just wrap the current char
            R_Chars = 1;
            wrap_amount = len;
            Wrap_Safe = section->section_size - wrap_amount;
        } else {
            wrap_amount = section->section_size - Wrap_Safe;
        }

        // By this point we know exactly how many chars we need to move to
        // the next line. Now to figure out how to move them:
        // calculate shift
        shift = indent + 1;         // for the indent spaces and for the newline char

        // if we're in box mode we need some extra space for the
        // spaces and box drawing chars
        if( Box_Mode ) {
            shift += R_Chars + 3;   // trailing spaces to add plus 2 vertical bars and 1 space
        } else {
            // remove existing trailing spaces when not in box mode
            delta = 0;
            while( 1 + delta < Wrap_Safe && section->section_text[Wrap_Safe - 1 - delta] == ' ' ) {
                delta++;
            }
            shift -= delta;
        }

        // figure out which way the text has to move, and tweak the section
        if( shift > 0 ) {
            // add some padding so we can shift the chars over
            for( ctr = 0; ctr < shift; ctr++ ) {
                trans_add_char( 'X', section );
            }
        } else {
            // we have to decrease the section size accordingly
            section->section_size += shift;
        }

        // shift the chars over
        memmove( section->section_text + Wrap_Safe + shift, section->section_text + Wrap_Safe, wrap_amount );

        // fill "vacated" region with spaces (if there was one)
        if( shift > 0 ) {
            memset( section->section_text + Wrap_Safe, ' ', shift );
            // if shift is negative then the region we'll be working with
            // should already be filled with spaces.
        }

        // add newline before text we just shifted to break the line
        ctr = Wrap_Safe + shift - indent - 1;
        if( Box_Mode ) {
            // when in box mode we've added two chars for the vertical bar
            // and a leading space. Need to make sure the newline goes
            // before them...
            ctr -= 2;
        }
        section->section_text[ctr] = '\n';

        // if we're in Box_Mode then we also add the vertical bars
        if( Box_Mode ) {
            section->section_text[ctr - 1] = BOX_VBAR;
            section->section_text[Wrap_Safe + shift - 2] = BOX_VBAR;
        }

        // reset cursor x position
        Cursor_X = indent + R_Chars + ( Box_Mode ? 2 : 0 );

        // Set next safe wrappable point to beginning of text on this line
        Wrap_Safe += indent + 1;

        if( wrap_amount == len ) {
            R_Chars = 1;
        }
    }
    return( len );
}

static size_t trans_add_str_wrap( const char *str, section_def *section )
/***********************************************************************/
{
    size_t      len = 0;

    for( ; *str != '\0'; str++ ) {
        len += trans_add_char_wrap( *str, section );
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
        trans_add_char_wrap( ' ', section );
    }
    return( len );
}

void ib_topic_init( void )
/************************/
{
}

static void draw_line( section_def *section )
/*******************************************/
{
    int     i;

    for( i = 0; i < Right_Margin - currentIndent() - 2; i++ ) {
        trans_add_char( BOX_HBAR, section );
    }
}

void ib_trans_line( char *line_buf, section_def *section )
/********************************************************/
{
    char                *ptr;
    char                *end;
    char                ch;
    char                *ctx_name;
    char                *ctx_text;
    char                buf[100];
    int                 indent = 0;
    int                 ctr;
    char                *file_name;
    size_t              len;

    /* check for special column 0 pre-processing stuff first */
    ptr = line_buf;
    ch = *ptr;

    // We start at a new line...
    Wrap_Safe = section->section_size;
    Cursor_X = 0;
    R_Chars = 0;

    if( Blank_line && ( ch != WHP_LIST_ITEM || !Curr_list->compact ) ) {
        Blank_line = false;
    }
    switch( ch ) {
    case WHP_TABXMP:     // Tabbed-example
        ptr = skip_blanks( ptr + 1 );
        if( *ptr == '\0' ) {
            Tab_xmp = false;
        } else {
            Tab_xmp_char = *ptr++;
            Tabs_read( ptr );
            Tab_xmp = true;
        }
        return;
    case WHP_BOX_ON:     // Box-mode start
        // indent properly
        for( ctr = 0; ctr < currentIndent(); ctr++ ) {
            trans_add_char( ' ', section );
        }
        // draw the top line of the box
        trans_add_char( BOX_CORNER_TOP_LEFT, section );
        draw_line( section );
        trans_add_char( BOX_CORNER_TOP_RIGHT, section );
        trans_add_char_wrap( '\n', section );
        Box_Mode = true;
        return;
    case WHP_BOX_OFF:    // Box-mode end
        for( ctr = 0; ctr < currentIndent(); ctr++ ) {
            trans_add_char( ' ', section );
        }
        trans_add_char( BOX_CORNER_BOTOM_LEFT, section );
        draw_line( section );
        trans_add_char( BOX_CORNER_BOTOM_RIGHT, section );
        Box_Mode = false;
        trans_add_char_wrap( '\n', section );
        return;
    case WHP_LIST_START:
    case WHP_DLIST_START:
    case WHP_OLIST_START:
    case WHP_SLIST_START:
        NewList( ptr, Curr_indent, list_indent );
#if 0
        indent = Text_Indent;
        if( ch == WHP_SLIST_START ) {
            /* nested simple lists, with no pre-indent. Force an indent */
            if( Curr_list->type != LIST_TYPE_SIMPLE ) {
                indent = 0;
            }
        }
#endif
        if( list_indent )
            Curr_indent += Text_Indent;
        list_indent = false;
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
        Curr_indent = Curr_list->prev_indent;
        list_indent = Curr_list->list_indent;
        PopList();
        return;
    case WHP_DLIST_TERM:
        list_indent = false;
        break;
    case WHP_DLIST_DESC:
        list_indent = true;
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

    // skip preceding blank lines
    if( *skip_blanks( ptr ) == '\0' && Curr_ctx->empty ) {
        return;
    }

    // remove '\n' on the end
    if( Blank_line ) {
        section->section_size--;
    }

    // indent properly if the first char is not white-space
    if( _is_nonblank( ch ) ) {
        ctr = ( ch == WHP_LIST_ITEM && !Box_Mode && Curr_list->type != LIST_TYPE_SIMPLE ) ? Text_Indent : 0;
        for( ; ctr < currentIndent(); ctr++ ) {
            trans_add_char_wrap( ' ', section );
        }

        if( Box_Mode ) {
            trans_add_char_wrap( BOX_VBAR, section );
            trans_add_char_wrap( ' ', section );
        }
    }

    Blank_line = true;
    for( ;; ) {
        ch = *ptr;
        if( _is_nonblank( ch ) ) {
            Blank_line = false;
        }
        if( ch == '\0' ) {
            // this just shuts off bolding after a def. list term
            if( Line_postfix == LPOSTFIX_TERM ) {
                Line_postfix = LPOSTFIX_NONE;
                trans_add_str( IB_BOLD_OFF_STR, section );
            }
            trans_add_char_wrap( '\n', section );
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
            if( ch != WHP_FLINK ) {
                add_link( ctx_name );
            }

            // Definition pop-up's are converted to hyper-links in InfoBench
            trans_add_char( TEMP_HLINK_BREAK, section );
            // Add line number to hyperlink so we can give meaningful errors
            trans_add_str( itoa( Line_num, buf, 10 ), section );
            trans_add_char( TEMP_HLINK_BREAK, section );

            // We don't want links to break as IB doesn't like this...
            spaces_to_nobreak( ctx_text );

            indent = currentIndent();
            // find out the maximum allowed length for hyper-link text:
            ctr = Right_Margin - indent - ( ( Hyperlink_Braces ) ? 2 : 0 );

            // if the link name is too long then we warn & truncate it
            if( strlen( ctx_text ) > ctr ) {
                warning_msg( "Hyperlink name too long", Line_num );
                ctx_text[ctr] = '\0';
            }

            /* If hyper-link bracing is on we have to do a kludge to fix
             * the spacing. The "XX" will make the wrap routine happy.
             * They're stripped off when it comes time to write the file.
             */
            if( Hyperlink_Braces ) {
                trans_add_str_wrap( "XX", section );
            }
            trans_add_str_wrap( ctx_text, section );
            trans_add_char( IB_HLINK_BREAK , section );
            trans_add_str( ctx_name, section );
            if( ch == WHP_FLINK ) {
                trans_add_char( IB_HLINK_BREAK, section );
                trans_add_str( file_name, section );
            }
            trans_add_char( TEMP_HLINK_BREAK, section );
            break;
        case WHP_LIST_ITEM:
            Curr_list->number++;
            if( Curr_list->type != LIST_TYPE_SIMPLE ) {
                buf[0] = '\0';
                if( Curr_list->type == LIST_TYPE_UNORDERED ) {
                    // generate a bullet, correctly spaced for tab size
                    for( ctr = 0; ctr < Text_Indent; ctr++ ) {
                        strcat( buf, " " );
                    }
                    buf[Text_Indent / 2 - 1] = IB_BULLET;
                } else if( Curr_list->type == LIST_TYPE_ORDERED ) {
                    /* ordered list type */
                    sprintf( buf, "%*d. ", Text_Indent - 2, Curr_list->number );
                }
                trans_add_str_wrap( buf, section );
            }
            Eat_blanks = true;
            ptr = skip_blanks( ptr + 1 );
            break;
        case WHP_DLIST_DESC:
            Curr_list->number++;
            ptr = skip_blanks( ptr + 1 );
            break;
        case WHP_DLIST_TERM:
            /* definition list term */
            trans_add_str( IB_BOLD_ON_STR, section );
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
            // we ignore paragraph resets
            ptr++;
            break;
        case WHP_BMP:
            // we ignore bitmaps
            ptr = strchr( ptr + 3, WHP_BMP ) + 1;
            break;
        case WHP_FONTSTYLE_START:
            ptr++;
            end = strchr( ptr, WHP_FONTSTYLE_START );
            for( ; ptr != end; ptr++ ) {
                switch( *ptr ) {
                // bold and italic map to bold
                case 'b':
                case 'i':
                    trans_add_str( IB_BOLD_ON_STR, section );
                    break;
                // underline and underscore map to underline
                case 'u':
                case 's':
                    trans_add_str( IB_UNDERLINE_ON_STR, section );
                    break;
                }
            }
            ptr++;
            break;
        case WHP_FONTSTYLE_END:
            // reset style (bold off, underline off)
            trans_add_str( IB_BOLD_OFF_STR IB_UNDERLINE_OFF_STR, section );
            ptr++;
            break;
        case WHP_FONTTYPE:
            // we basically ignore font type changes
            ptr = strchr( strchr( ptr + 1 , WHP_FONTTYPE ) + 1, WHP_FONTTYPE ) + 1;
            break;
        default:
            ptr++;
            if( !Eat_blanks || ch != ' ' ) {
                Curr_ctx->empty = false;
                if( Tab_xmp && ch == Tab_xmp_char ) {
                    size_t  ch_len;

                    // find out how close we are to "col 0" for the current indent
                    ch_len = Cursor_X - currentIndent() - ( Box_Mode ? 2 : 0 );
                    tab_align( ch_len, section );
                    ptr = skip_blanks( ptr );
                } else {
                    trans_add_char_wrap( ch, section );
                }
                Eat_blanks = false;
            }
            break;
        }
    }
}

static void find_browse_pair( ctx_def *ctx, ctx_def **prev, ctx_def **next )
/**************************************************************************/
{
    browse_ctx                  *curr_ctx;
    browse_ctx                  *prev_ctx;
    browse_ctx                  *next_ctx;
    browse_def                  *browse_list;

    for( browse_list = Browse_list; browse_list != NULL; browse_list = browse_list->next ) {
        prev_ctx = NULL;
        for( curr_ctx = browse_list->ctx_list; curr_ctx != NULL; curr_ctx = curr_ctx->next ) {
            if( curr_ctx->ctx == ctx ) {
                if( prev_ctx == NULL ) {
                    *prev = ctx;
                } else {
                    *prev = prev_ctx->ctx;
                }

                next_ctx = curr_ctx;
                for(;;) {
                    next_ctx = next_ctx->next;

                    if( next_ctx == NULL ) {
                        next_ctx = curr_ctx;
                        break;
                    }

                    if( !Remove_empty || !next_ctx->ctx->empty ) {
                        break;
                    }
                }

                *next = next_ctx->ctx;
                return;
            }

            if( !Remove_empty || !curr_ctx->ctx->empty ) {
                prev_ctx = curr_ctx;
            }
        }
        /* if we've reached this point we know this browse list is no good
         * so we go onto the next one
         */
    }
    /* and if we get here we know there are no next and prev, so we point
     * back at the same context
     */
    *prev = ctx;
    *next = ctx;
    return;
}

static void ib_append_line( char *infnam )
/****************************************/
{
    FILE            *infile;
    int             inchar;

    if( infnam != NULL && infnam[0] != '\0' ) {
        infile = fopen( infnam, "rt" );
        if( infile != NULL ) {
            for(;;) {
                inchar = fgetc( infile );

                if( inchar == EOF || inchar == '\n' ) {
                    break;
                }

                whp_fprintf( Out_file, "%c", inchar );
            }
            fclose( infile );
        }
    }
}

static void fake_hlink( char *label )
/***********************************/
{
    if( Hyperlink_Braces ) {
        whp_fprintf( Out_file, "<<" IB_BOLD_ON_STR "%s" IB_BOLD_OFF_STR ">> ", label );
    } else {
        whp_fprintf( Out_file, IB_BOLD_ON_STR "%s" IB_BOLD_OFF_STR " ", label );
    }
}

static void output_button1( const char *button, const char *label )
/*****************************************************************/
{
    if( Hyperlink_Braces ) {
        whp_fprintf( Out_file, IB_PLAIN_LINK_BEG_STR "%s" IB_HLINK_BREAK_STR "%s" IB_PLAIN_LINK_END_STR " ", button, label );
    } else {
        whp_fprintf( Out_file, IB_HLINK_STR "%s" IB_HLINK_BREAK_STR "%s" IB_HLINK_STR " ", button, label );
    }
}

static void output_button2( const char *button, const char *label )
/*****************************************************************/
{
    if( Hyperlink_Braces ) {
        whp_fprintf( Out_file, IB_PLAIN_LINK_BEG_STR "%s" IB_HLINK_BREAK_STR, button );
    } else {
        whp_fprintf( Out_file, IB_HLINK_STR "%s" IB_HLINK_BREAK_STR, button );
    }
    str_out_ib( label );
    if( Hyperlink_Braces ) {
        whp_fprintf( Out_file, IB_PLAIN_LINK_END_STR " " );
    } else {
        whp_fprintf( Out_file, IB_HLINK_STR " " );
    }
}

static void output_ctx_hdr( ctx_def *ctx )
/****************************************/
{
    ctx_def             *temp_ctx;
    ctx_def             *prev;      // for << button
    ctx_def             *next;      // for >> button

    // output topic name
    whp_fprintf( Out_file, IB_TOPIC_NAME "\"" );
    str_out_ib( ctx->title );
    whp_fprintf( Out_file, "\" 0 0\n" );

    // Header stuff:
    if( Do_tc_button
        || Do_idx_button
        || Do_browse
        || Do_up
        || Header_File != NULL && Header_File[0] != '\0' ) {

        //beginning of header
        whp_fprintf( Out_file, IB_HEADER_BEG "\n" );

        if( Do_tc_button ) {
            if( stricmp( ctx->ctx_name, "table_of_contents" ) != 0 ) {
                output_button1( HBUTTON_CONTENTS, "Table of Contents" );
            } else {
                fake_hlink( HBUTTON_CONTENTS );
            }
        }

        if( Do_kw_button ) {
            if( stricmp( ctx->ctx_name, "keyword_search" ) != 0 ) {
                output_button1( HBUTTON_KEYWORDS, "Keyword Search" );
            } else {
                fake_hlink( HBUTTON_KEYWORDS );
            }
        }

        if( Do_browse ) {
            find_browse_pair( ctx, &prev, &next );

            // << browse button
            if( prev != ctx ) {
                output_button2( HBUTTON_PREV, prev->title );
            } else {
                fake_hlink( HBUTTON_PREV );
            }

            // >> browse button (relies on the find_browse_pair above)
            if( next != ctx ) {
                output_button2( HBUTTON_NEXT, next->title );
            } else {
                fake_hlink( HBUTTON_NEXT );
            }
        }

        if( Do_idx_button ) {
            if( stricmp( ctx->ctx_name, "index_of_topics" ) != 0 ) {
                output_button1( HBUTTON_INDEX, "Index of Topics" );
            } else {
                fake_hlink( HBUTTON_INDEX );
            }
        }

        // "Up" button
        if( Do_up ) {
            // find "parent" context
            for( temp_ctx = ctx->up_ctx; temp_ctx != NULL; temp_ctx = temp_ctx->up_ctx ) {
                if( !Remove_empty || !temp_ctx->empty ) {
                    break;
                }
            }

            // use table_of_contents if no "parent" context
            if( temp_ctx == NULL ) {
                temp_ctx = find_ctx( "table_of_contents" );
                if( temp_ctx == ctx) {
                    temp_ctx = NULL;
                }
            }

            // spit out up button stuff
            if( temp_ctx != NULL ) {
                output_button2( HBUTTON_UP, temp_ctx->title );
            } else {
                fake_hlink( HBUTTON_UP );
            }
        }
        // append user header file
        ib_append_line( Header_File );

        // end of header
        whp_fprintf( Out_file, "\n" IB_HEADER_END "\n" );
    }
    // append user footer file
    if( Footer_File != NULL && Footer_File[0] != '\0' ) {
        whp_fprintf( Out_file, IB_TRAILER_BEG "\n" );
        ib_append_line( Footer_File );
        whp_fprintf( Out_file, "\n" IB_TRAILER_END "\n" );
    }
}

static void output_end( void )
/****************************/
{
    whp_fprintf( Out_file, "\n" );
}

static void output_section_ib( section_def *section )
/***************************************************/
{
    size_t              len;
    ctx_def             *ctx;
    int                 line;
    char                *label;
    size_t              label_len;
    char                ch;
    char                *file;
    char                *topic;
    char                *p;
    char                *end;

    p = section->section_text;
    end = p + section->section_size;
    len = 0;
    while( p + len < end ) {
        // stop when we hit a hyper-link
        if( *(p + len) != TEMP_HLINK_BREAK ) {
            len++;
        } else {
            // write out the block of text we've got so far
            whp_fwrite( Out_file, p, 1, len );
            p += len + 1;

            // grab the line number
            for( len = 0; ; len++ ) {
                if( *(p + len) == TEMP_HLINK_BREAK ) {
                    break;
                }
            }
            p[len] = '\0';
            line = strtol( p, NULL, 10 );
            p += len + 1;

            // find the length of the link label (what the user sees)
            for( len = 0; ; len++ ) {
                if( *(p + len) == IB_HLINK_BREAK ) {
                    break;
                }
            }

            // if we're using the brace mode we strip off the "XX"
            if( Hyperlink_Braces ) {
                p += 2;
                len -= 2;
            }

            // output the label and the break
            label = p;
            label_len = len + 1;
            p += len + 1;

            // find the length of the link context
            for( len = 0; ; len++ ) {
                ch = *(p + len);
                if( ch == TEMP_HLINK_BREAK || ch == IB_HLINK_BREAK ) {
                    break;
                }
            }
            // null terminate the context name, and find the associated topic
            *(p + len) = '\0';
            topic = p;
            ctx = find_ctx( topic );

            // output the topic name that belongs to the context
            if( ctx == NULL && ch != IB_HLINK_BREAK ) {
                warning_msg( "Link to nonexistent context", line );
                printf( "For topic=%s\n", topic );
                whp_fwrite( Out_file, label, 1, label_len - 1 );
            } else {
                // now we start writing the hyper-link
                if( Hyperlink_Braces ) {
                    whp_fprintf( Out_file, IB_PLAIN_LINK_BEG_STR );
                } else {
                    whp_fprintf( Out_file, IB_HLINK_STR );
                }
                whp_fwrite( Out_file, label, 1, label_len );
                if( ctx != NULL ) {
                    str_out_ib( ctx->title );
                } else {
                    str_out_ib( topic );
                }
                if( ch == IB_HLINK_BREAK ) {
                    /* file link. Get the file name */
                    file = p + len + 1;
                    for( ;; ) {
                        len++;
                        if( *(p + len) == TEMP_HLINK_BREAK ) {
                            break;
                        }
                    }
                    *(p + len) = '\0';
                    whp_fprintf( Out_file, IB_HLINK_BREAK_STR "%s", file );
                }
                if( Hyperlink_Braces ) {
                    whp_fprintf( Out_file, IB_PLAIN_LINK_END_STR );
                } else {
                    whp_fprintf( Out_file, IB_HLINK_STR );
                }
            }

            // adjust the len and ctr counters appropriately
            p += len + 1;
            len = 0;
        }
    }
    // output whatever's left
    if( p < end ) {
        whp_fwrite( Out_file, p, 1, end - p );
    }
}

static void output_ctx_sections( ctx_def *ctx )
/*********************************************/
{
    section_def         *section;

    for( section = ctx->section_list; section != NULL; section = section->next ) {
        if( section->section_size > 0 ) {
            output_section_ib( section );
        }
    }
}

void ib_output_file( void )
/*************************/
{
    ctx_def         *ctx;

    if( IB_def_topic != NULL ) {
        whp_fprintf( Out_file, IB_DEFAULT_TOPIC "\"%s\"\n", IB_def_topic );
        free( IB_def_topic );
    }
    if( IB_help_desc != NULL ) {
        whp_fprintf( Out_file, IB_DESCRIPTION "\"%s\"\n", IB_help_desc );
        free( IB_help_desc );
    }
    for( ctx = Ctx_list; ctx != NULL; ctx = ctx->next ) {
        if( !Remove_empty || !ctx->empty || ctx->req_by_link ) {
            output_ctx_hdr( ctx );
            output_ctx_sections( ctx );
        }
    }
    output_end();
}

void ib_init_whp( void )
{
}
