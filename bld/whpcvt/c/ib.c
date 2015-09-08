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
* Description:  This file defines the InfoBench specific functions.
*
****************************************************************************/

#include "whpcvt.h"
#include <stdio.h>
#include <assert.h>

#include "clibext.h"


#define MAX_LISTS       20

enum {
    LPOSTFIX_NONE,
    LPOSTFIX_TERM,
};
static int              Line_postfix=LPOSTFIX_NONE;

// We use the escape char a lot...
#define STR_ESCAPE              "\x1B"
#define CHR_ESCAPE              0x1B

// these are for the map_char_ib function
#define MAP_REMOVE              -1
#define MAP_NONE                -2

// internal hyperlink symbol. Gets filtered out at output time
#define CHR_TEMP_HLINK          0x7F

// this symbol separates the hyper-link label and topic. Other hyper-link
// related symbols are in whpcvt.h
#define CHR_HLINK               0xE0
#define CHR_HLINK_BREAK         0xE8

// Some characters we use for graphics
#define CHR_BULLET              0x07
#define BOX_VBAR                0xB3
#define BOX_HBAR                0xC4
#define BOX_CORNER_TOP_LEFT     0xDA
#define BOX_CORNER_TOP_RIGHT    0xBF
#define BOX_CORNER_BOTOM_LEFT   0xC0
#define BOX_CORNER_BOTOM_RIGHT  0xD9

// InfoBench style codes
#define STR_BOLD_ON             STR_ESCAPE "b"
#define STR_UNDERLINE_ON        STR_ESCAPE "u"
#define STR_BOLD_OFF            STR_ESCAPE "p"
#define STR_UNDERLINE_OFF       STR_ESCAPE "w"

// labels for speacial header buttons
#define HB_UP                   " Up "
#define HB_PREV                 " <<<< "
#define HB_NEXT                 " >>>> "
#define HB_CONTENTS             " Contents "
#define HB_INDEX                " Index "
#define HB_KEYWORDS             " Keywords "

// Some stuff for tab examples:
#define MAX_TABS                100     // up to 100 tab stops
static int Tab_list[MAX_TABS];

// turns off bold and underline
static char Reset_Style[] = STR_BOLD_OFF STR_UNDERLINE_OFF;

enum {
    LIST_SPACE_COMPACT,
    LIST_SPACE_STANDARD,
};

enum {
    LIST_TYPE_NONE,
    LIST_TYPE_UNORDERED,
    LIST_TYPE_ORDERED,
    LIST_TYPE_SIMPLE,
    LIST_TYPE_DEFN
};

typedef struct {
    int                 type;
    int                 number;
    int                 prev_indent;
    int                 compact;
} list_def;

static list_def         Lists[MAX_LISTS]={
    { LIST_TYPE_NONE,   0,      0 },            // list base
};
static int              List_level=0;
static list_def         *Curr_list=&Lists[0];

static bool             Blank_line = FALSE;
static int              Curr_indent = 0;
static bool             Eat_blanks = FALSE;

// The following are for word-wrapping and indentation support
static int              Cursor_X = 0;   // column number
static int              R_Chars = 0;    // visible chars since Wrap_Safe
static int              Wrap_Safe = 0;  // index of break candidate
static int              NL_Group = 0;   // Number of contiguous newlines

static bool             Box_Mode = FALSE;

static void warning( char *msg, unsigned int line )
/*************************************************/
{
    printf( "*** WARNING: %s on line %d.\n", msg, line );
}

static void set_compact( char *line )
/***********************************/
{
    ++line;
    if( *line == 'c' ) {
        /* compact list */
        Curr_list->compact = LIST_SPACE_COMPACT;
    } else {
        Curr_list->compact = LIST_SPACE_STANDARD;
    }
}

// this function will change all of the spaces in a string into non-breaking
// spaces (character 0xFF ). It's currently only used for the labels on
// hyper-links to ensure that they do not get broken across lines as
// this is not allowed by the InfoBench grammar.
static void to_nobreak( char *str )
/*********************************/
{
    if( str != NULL ) {
        for( ; *str != '\0'; ++str ) {
            if( *str == ' ' ) {
                *str = '\xFF';
            }
        }
    }
}

/* This function will return CHR_REMOVE if the character is not allowed in
 * InfoBench, null if the character is okay. Any other values are prefixes
 * used to escape the character.
 */
static int map_char_ib( int ch )
/******************************/
{
    int         res;

    switch( ch ) {
    // The following characters should be preceded by an ESC character
    // Some could also be preceded by themselves, but this leads to
    // ambiguity. ie: what does <<< mean?
    case '}':
    case '{':
    case '<':
    case '>':
    case '"':
        res = CHR_ESCAPE;
        break;

    // The following characters are special to InfoBench, and there is no
    // way to represent them with the current grammar
    case CHR_HLINK:
    case CHR_HLINK_BREAK:
    case CHR_ESCAPE:
        res = MAP_REMOVE;
        break;

    default:
        res = MAP_NONE;
    }
    return( res );
}

/* This function will do proper escaping or character substitution for
 * characters special to InfoBench and add them to the section text.
 */
static int trans_add_char_ib( int ch, section_def *section, int *alloc_size )
/***************************************************************************/
{
    int                 len=0;
    int                 esc;

    esc = map_char_ib( ch );
    if( esc == MAP_REMOVE ) {
        return( 0 );
    } else if( esc != MAP_NONE ) {
        len += trans_add_char( esc, section, alloc_size );
    }

    len += trans_add_char( ch, section, alloc_size );

    return( len );
}

/* This function will do proper escaping or character substitution for
 * characters special to InfoBench and send them to the output stream.
 */
static void str_out_ib( FILE *f, char *str )
/******************************************/
{
    int         esc;

    if( str != NULL ) {
        for( ; *str != '\0'; ++str ) {
            esc = map_char_ib( *(unsigned char *)str );
            if( esc != MAP_REMOVE ) {
                if( esc != MAP_NONE ) {
                    whp_fprintf( f, "%c", esc );
                }
                whp_fwrite( str, 1, 1, f );
            }
        }
    }
}

// The following two functions handle word-wrapping
int trans_add_char_wrap( int ch, section_def *section, int *alloc_size )
/**********************************************************************/
{
    int                 ctr;            // misc. counter
    int                 wrap_amount;    // amount of wrapped text
    int                 shift;          // amount we need to shift text
    int                 delta;          // amount of whitespace we ignore
    int                 indent;         // actual indent
    int                 len = 0;        // number of chars we just added

    // the "1" is because a character is allowed to appear on the right margin
    // the minus part is so that we leave enough room for the box chars
    #define MY_MARGIN ( Right_Margin + 1 - ( Box_Mode ? 2 : 0 ) )

    // find out the real indentation
    indent = Curr_indent;
    if( indent < 0 )
        indent = 0;

    // add character
    if( ch == '\n' ) {
        NL_Group++;
        if( Box_Mode ) {
            // Add left side if necessary:
            while( Cursor_X < indent ) {
                trans_add_char( ' ', section, alloc_size );
                Cursor_X++;
            }
            if( Cursor_X == indent ) {
                trans_add_char( BOX_VBAR, section, alloc_size );
                Cursor_X++;
            }

            // Now add right side:
            // the "- 1" is for the BOX_VBAR
            while( Cursor_X < Right_Margin - 1 ) {
                trans_add_char( ' ', section, alloc_size );
                Cursor_X++;
            }
            // Now add vertical bar if room (there should be...)
            if( Cursor_X == Right_Margin - 1 ) {
                trans_add_char( BOX_VBAR, section, alloc_size );
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
    len = trans_add_char_ib( ch, section, alloc_size );

    // adjust the nearest safe break point if the char we got was a space and
    // is not preceded by a space
    if( ch == ' ' && section->section_size > 2 &&
                section->section_text[ section->section_size - 2 ] != ' ' ) {
        Wrap_Safe = section->section_size;
        R_Chars = 0;
    }

    // If a bunch of spaces are pushing us over the edge then we'll rip all
    // of them off except one. Only happens in extreme cases. (ie: wdbg.whp)
    if( Cursor_X >= MY_MARGIN && ch==' ' ) {
        while( section->section_size > 2 &&
                section->section_text[ section->section_size - 2 ] == ' ' ) {
            section->section_size--;
            Cursor_X--;
        }
    }

    // if this assertion fails then the wrapping code will break. Hopefully
    // this cannot happen...
    assert( Cursor_X <= MY_MARGIN );

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
        shift = indent          // for the indent spaces
                + 1;            // for the newline char

        // if we're in box mode we need some extra space for the
        // spaces and box drawing chars
        if ( Box_Mode ) {
            shift += R_Chars                 // trailing spaces to add
                     + 3;                    // 2 vertical bars and 1 space
        } else {
            // remove existing trailing spaces when not in box mode
            delta = 0;
            while( 1 + delta < Wrap_Safe && section->section_text[ Wrap_Safe - 1 - delta ] == ' ' ) {
                delta++;
            }
            shift -= delta;
        }

        // figure out which way the text has to move, and tweak the section
        if( shift > 0 ) {
            // add some padding so we can shift the chars over
            for( ctr = 1; ctr <= shift; ctr++ ) {
                trans_add_char( 'X', section, alloc_size );
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
        *(unsigned char *)(section->section_text + ctr) = '\n';

        // if we're in Box_Mode then we also add the vertical bars
        if( Box_Mode ) {
            *(unsigned char *)(section->section_text + ctr - 1) = BOX_VBAR;
            *(unsigned char *)(section->section_text + Wrap_Safe + shift - 2) = BOX_VBAR;
        }

        // reset cursor x position
        Cursor_X = indent + R_Chars + ( Box_Mode ? 2 : 0 );

        // Set next safe wrappable point to beginning of text on this line
        Wrap_Safe += indent + 1;

        if( wrap_amount==len ) {
            R_Chars = 1;
        }
    }
    return( len );
}

int trans_add_str_wrap( char *str, section_def *section, int *alloc_size )
/************************************************************************/
{
    int         len = 0;

    for( ; *str != '\0'; ++str ) {
        len += trans_add_char_wrap( *(unsigned char *)str, section, alloc_size );
    }
    return( len );
}

static void new_list( int type )
/******************************/
{
    ++List_level;
    if( List_level == MAX_LISTS ) {
        error( ERR_MAX_LISTS, TRUE );
    }
    Curr_list = &Lists[ List_level ];
    Curr_list->type = type;
    Curr_list->number = 1;
    Curr_list->prev_indent = Curr_indent;
    Curr_list->compact = LIST_SPACE_STANDARD;
}

static void pop_list( void )
/**************************/
{
    Curr_indent = Curr_list->prev_indent;
    --List_level;
    Curr_list = &Lists[List_level];
}

static void read_tabs(
/********************/

    char        *tab_line
) {
    char        *ptr;
    int         i;
    int         tabcol;

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

    section_def         *section,
    int                 *alloc_size
) {
    int                 i;
    int                 len;
    int                 ch_len;
    int                 indent;

    indent = Curr_indent;
    if( indent < 0 )
        indent = 0;
    // find out how close we are to "col 0" for the current indent
    ch_len = (Cursor_X - indent - (Box_Mode ? 2 : 0 ));

    // find the tab we should use
    i = 0;
    while( ch_len >= Tab_list[i]) {
        if( Tab_list[i] == -1 ) break;
        ++i;
    }

    // figure out how far away we are from our tab
    len = 1;
    if( Tab_list[i] != -1 ) {
        len =  Tab_list[i] - ch_len;
    }

    // output the spaces for our tab
    for( i = len; i > 0; --i ) {
        trans_add_char_wrap( ' ', section, alloc_size );
    }

    return( len );
}

void ib_topic_init( void )
/************************/
{
}

int ib_trans_line(
/*****************/

    section_def         *section,
    int                 alloc_size
) {
    char                *ptr;
    char                *end;
    int                 ch;
    char                *ctx_name;
    char                *ctx_text;
    char                buf[ 100 ];
    int                 indent = 0;
    int                 ctr;
    char                *file_name;

    // check for special pre-processing stuff first
    ptr = Line_buf;
    ch = *(unsigned char *)ptr;

    // We start at a new line...
    Wrap_Safe = section->section_size;
    Cursor_X = 0;
    R_Chars = 0;

    if( Blank_line && ( ch != CH_LIST_ITEM ||
                        Curr_list->compact != LIST_SPACE_COMPACT ) ) {
        Blank_line = FALSE;
    }
    switch( ch ) {
    // Tabbed-example
    case CH_TABXMP:
        if( *skip_blank( ptr + 1 ) == '\0' ) {
            Tab_xmp = FALSE;
        } else {
            read_tabs( ptr + 1 );
            Tab_xmp = TRUE;
        }
        return( alloc_size );

    // Box-mode start
    case CH_BOX_ON:
        ctr = 0;
        // indent properly
        while( ctr < Curr_indent ) {
            ctr++;
            trans_add_char( ' ', section, &alloc_size);
        }
        // draw the top line of the box
        trans_add_char( BOX_CORNER_TOP_LEFT, section, &alloc_size );
        for( ctr = 1; ctr <= Right_Margin - Curr_indent - 2; ctr++ ) {
            trans_add_char( BOX_HBAR, section, &alloc_size );
        }
        trans_add_char( BOX_CORNER_TOP_RIGHT, section, &alloc_size );

        trans_add_char_wrap( '\n', section, &alloc_size);

        Box_Mode = TRUE;
        return( alloc_size );

    case CH_BOX_OFF:
        ctr = 0;
        while( ctr < Curr_indent ) {
            ctr++;
            trans_add_char( ' ', section, &alloc_size);
        }

        trans_add_char( BOX_CORNER_BOTOM_LEFT, section, &alloc_size );
        for( ctr = 1; ctr <= Right_Margin - Curr_indent - 2; ctr++ ) {
            trans_add_char( BOX_HBAR, section, &alloc_size );
        }
        trans_add_char( BOX_CORNER_BOTOM_RIGHT, section, &alloc_size );
        Box_Mode = FALSE;

        trans_add_char_wrap( '\n', section, &alloc_size );

        return( alloc_size );

    case CH_OLIST_START:
        new_list( LIST_TYPE_ORDERED );
        set_compact( ptr );
        Curr_indent += Text_Indent;
        return( alloc_size );

    case CH_LIST_START:
    case CH_DLIST_START:
        new_list( ( ch == CH_LIST_START ) ? LIST_TYPE_UNORDERED :
                                                        LIST_TYPE_DEFN );
        set_compact( ptr );
        Curr_indent += Text_Indent;

        if( ch == CH_DLIST_START ) {
            ptr = skip_blank( ptr + 1 );
            if( *ptr != '\0' ) {
                /* due to a weakness in GML, the definition term must be
                   allowed on the same line as the definition tag. So
                   if its there, continue */
                break;
            }
        }
        return( alloc_size );

    case CH_DLIST_TERM:
        Curr_indent -= Text_Indent;
        break;

    case CH_SLIST_START:
        indent = 0;
        if( Curr_list->type == LIST_TYPE_SIMPLE ) {
            /* nested simple lists, with no pre-indent. Force an
               indent */
            indent = Text_Indent;
        }

        new_list( LIST_TYPE_SIMPLE );
        set_compact( ptr );
        Curr_indent += indent;
        return( alloc_size );

    case CH_SLIST_END:
    case CH_OLIST_END:
    case CH_LIST_END:
    case CH_DLIST_END:
        pop_list();
        return( alloc_size );

    case CH_DLIST_DESC:
        Curr_indent += Text_Indent;
        if( *skip_blank( ptr + 1 ) == '\0' ) {
            /* no description on this line. Ignore it so that no
               blank line gets generated */
            return( alloc_size );
        }
        break;

    case CH_CTX_KW:
        ptr = whole_keyword_line( ptr );
        if( ptr == NULL ) {
            return( alloc_size );
        }
        break;
    }

    // skip preceding blank lines
    if( *skip_blank( ptr ) == '\0' && Curr_ctx->empty ) {
        return( alloc_size );
    }

    // remove '\n' on the end
    if( Blank_line ) {
        --section->section_size;
    }

    // indent properly if the first char is not white-space
    if( ch != '\0' && ch != ' ' && ch != '\t') {
        ctr = ( ch == CH_LIST_ITEM && !Box_Mode &&
                        Curr_list->type != LIST_TYPE_SIMPLE )
                ? Text_Indent
                : 0;
        while( ctr < Curr_indent ) {
            ctr++;
            trans_add_char_wrap( ' ', section, &alloc_size);
        }

        if( Box_Mode ) {
            trans_add_char_wrap( BOX_VBAR, section, &alloc_size);
            trans_add_char_wrap( ' ', section, &alloc_size);
        }
    }

    Blank_line = TRUE;
    for( ;; ) {
        ch = *(unsigned char *)ptr;
        if( ch != '\0' && ( ch != ' ' || ch != '\t' ) ) {
            Blank_line = FALSE;
        }
        if( ch == '\0' ) {
            // this just shuts off bolding after a def. list term
            if( Line_postfix == LPOSTFIX_TERM ) {
                Line_postfix = LPOSTFIX_NONE;
                trans_add_str( STR_BOLD_OFF, section, &alloc_size );
            }
            trans_add_char_wrap( '\n', section, &alloc_size );
            break;
        } else if( ch == CH_HLINK || ch == CH_DFN || ch == CH_FLINK ) {
            Curr_ctx->empty = FALSE;
            if( ch == CH_FLINK ) {
                file_name = strchr( ptr + 1, ch );
                if( file_name == NULL ) {
                    error( ERR_BAD_LINK_DFN, TRUE );
                }
                *file_name = '\0';
            } else {
                file_name = ptr;
            }
            ctx_name = strchr( file_name + 1, ch );
            if( ctx_name == NULL ) {
                error( ERR_BAD_LINK_DFN, TRUE );
            }
            *ctx_name = '\0';

            ctx_text = strchr( ctx_name + 1, ch );
            if( ctx_text == NULL ) {
                error( ERR_BAD_LINK_DFN, TRUE );
            }
            *ctx_text = '\0';

            ctx_text = ctx_name + 1;
            ctx_name = file_name + 1;
            file_name = ptr + 1;
            if( ch != CH_FLINK ) {
                add_link( ctx_name );
            }

            ptr = ctx_text + strlen( ctx_text ) + 1;

            // Definition pop-up's are converted to hyper-links in InfoBench
            trans_add_char( CHR_TEMP_HLINK , section, &alloc_size );

            // Add line number to hyperlink so we can give meaningful errors
            trans_add_str( itoa( Line_num, buf, 10 ), section, &alloc_size );
            trans_add_char( CHR_TEMP_HLINK, section, &alloc_size );

            // We don't want links to break as IB doesn't like this...
            to_nobreak( ctx_text );

            indent = Curr_indent;
            if( indent < 0 )
                indent = 0;
            // find out the maximum allowed length for hyper-link text:
            ctr = Right_Margin - indent - ( ( Hyper_Brace_L == '<' ) ? 2 : 0 );

            // if the link name is too long then we warn & truncate it
            if( strlen( ctx_text ) > ctr ) {
                warning( "Hyperlink name too long", Line_num );
                ctx_text[ ctr ] = '\0';
            }

            /* If hyper-link bracing is on we have to do a kludge to fix
             * the spacing. The "XX" will make the wrap routine happy.
             * They're stripped off when it comes time to write the file.
             */
            if( Hyper_Brace_L == '<' ) {
                trans_add_str_wrap( "XX", section, &alloc_size );
            }
            trans_add_str_wrap( ctx_text, section, &alloc_size );
            trans_add_char( CHR_HLINK_BREAK , section, &alloc_size );
            trans_add_str( ctx_name, section, &alloc_size );
            if( ch == CH_FLINK ) {
                trans_add_char( CHR_HLINK_BREAK, section, &alloc_size );
                trans_add_str( file_name, section, &alloc_size );
            }
            trans_add_char( CHR_TEMP_HLINK , section, &alloc_size );
        } else if( ch == CH_LIST_ITEM ) {
            if( Curr_list->type != LIST_TYPE_SIMPLE ) {
                if( Curr_list->type == LIST_TYPE_UNORDERED ) {
                    // generate a bullet, correctly spaced for tab size
                    buf[0] = '\0';
                    for( ctr = 1; ctr <= Text_Indent; ctr++)
                    {
                        strcat( buf, " " );
                    }
                    buf[ Text_Indent / 2 - 1 ] = CHR_BULLET;
                } else if( Curr_list->type == LIST_TYPE_ORDERED ) {
                    /* ordered list type */
                    sprintf( buf, "%*d. ", Text_Indent - 2,
                                                        Curr_list->number );
                    ++Curr_list->number;
                }
                trans_add_str_wrap( buf, section, &alloc_size );
            }
            Eat_blanks = TRUE;
            ptr = skip_blank( ptr + 1 );
        } else if( ch == CH_DLIST_DESC ) {
            ptr = skip_blank( ptr + 1 );
        } else if( ch == CH_DLIST_TERM ) {
            /* definition list term */
            trans_add_str( STR_BOLD_ON, section, &alloc_size );
            Line_postfix = LPOSTFIX_TERM;
            ptr = skip_blank( ptr + 1 );
            Eat_blanks = TRUE;
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
            // we ignore paragraph resets
            ++ptr;
        } else if( ch == CH_BMP ) {
            // we ignore bitmaps
            ptr = strchr( ptr + 3, CH_BMP ) + 1;
        } else if( ch == CH_FONTSTYLE_START ) {
            ++ptr;
            end = strchr( ptr, CH_FONTSTYLE_START );
            for( ; ptr != end; ++ptr ) {
                switch( *ptr ) {

                // bold and italic map to bold
                case 'b':
                case 'i':
                    trans_add_str( STR_BOLD_ON, section, &alloc_size );
                    break;

                // underline and underscore map to underline
                case 'u':
                case 's':
                    trans_add_str( STR_UNDERLINE_ON, section, &alloc_size );
                    break;
                }
            }
            ++ptr;
        } else if( ch == CH_FONTSTYLE_END ) {
            // reset style (bold off, underline off)
            trans_add_str( Reset_Style, section, &alloc_size );
            ++ptr;
        } else if( ch == CH_FONTTYPE ) {
            // we basically ignore font type changes
            ptr = strchr( strchr( ptr + 1 , CH_FONTTYPE ) + 1,
                            CH_FONTTYPE ) + 1;
        } else {
            ++ptr;
            if( !Eat_blanks || ch != ' ' ) {
                Curr_ctx->empty = FALSE;

                if( Tab_xmp && ch == Tab_xmp_char ) {
                    tab_align( section, &alloc_size );
                    ptr = skip_blank( ptr );
                } else {
                    trans_add_char_wrap( ch, section, &alloc_size );
                }

                Eat_blanks = FALSE;
            }
        }
    }

    return( alloc_size );
}

static void find_browse_pair(
/***************************/

    ctx_def                     *ctx,
    ctx_def                     **prev,
    ctx_def                     **next
) {
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
static void ib_append_line(
/*************************/

    FILE                        *outfile,
    char                        *infnam
) {
    FILE                        *infile;
    int                         inchar;

    if( infnam[0] != '\0' ) {
        infile = fopen( infnam, "rt" );
        if( infile != NULL ) {
            for(;;) {
                inchar = fgetc( infile );

                if( inchar == EOF || inchar == '\n' ) {
                    break;
                }

                whp_fprintf( outfile, "%c", inchar );
            }
            fclose( infile );
        }
    }
}

static void fake_hlink(
/*********************/

    FILE                        *file,
    char                        *label
) {
    if( Hyper_Brace_L == '<' ) {
        whp_fprintf( file, "<<" );
    }
    whp_fprintf( file, "%s%s%s", STR_BOLD_ON, label, STR_BOLD_OFF );
    if( Hyper_Brace_R == '>' ) {
        whp_fprintf( file, ">>" );
    }
    whp_fprintf( file, " " );
}

static void output_ctx_hdr(
/*************************/

    ctx_def                     *ctx
) {
    ctx_def                     *temp_ctx;
    ctx_def                     *prev;      // for << button
    ctx_def                     *next;      // for >> button

    // output topic name
    whp_fprintf( Out_file, "::::\"" );
    str_out_ib( Out_file, ctx->title );
    whp_fprintf( Out_file, "\" 0 0\n" );

    // Header stuff:
    if( Do_tc_button
        || Do_idx_button
        || Do_browse
        || Do_up
        || Header_File[0] != '\0' ) {

        //beginning of header
        whp_fprintf( Out_file, ":h\n" );

        if( Do_tc_button ) {
            if( stricmp( ctx->ctx_name, "table_of_contents" ) != 0 ) {
                whp_fprintf( Out_file,
                                "%c" HB_CONTENTS "%cTable of Contents%c ",
                                Hyper_Brace_L,
                                CHR_HLINK_BREAK,
                                Hyper_Brace_R );
            } else {
                fake_hlink( Out_file, HB_CONTENTS );
            }
        }

        if( Do_kw_button ) {
            if( stricmp( ctx->ctx_name, "keyword_search" ) != 0 ) {
                whp_fprintf( Out_file,
                                "%c" HB_KEYWORDS "%cKeyword Search%c ",
                                Hyper_Brace_L,
                                CHR_HLINK_BREAK,
                                Hyper_Brace_R );
            } else {
                fake_hlink( Out_file, HB_KEYWORDS );
            }
        }

        if( Do_browse ) {
            find_browse_pair( ctx, &prev, &next );

            // << browse button
            if( prev != ctx ) {
                whp_fprintf( Out_file, "%c" HB_PREV "%c", Hyper_Brace_L,
                                CHR_HLINK_BREAK );
                str_out_ib( Out_file, prev->title );
                whp_fprintf( Out_file, "%c ", Hyper_Brace_R );
            } else {
                fake_hlink( Out_file, HB_PREV );
            }

            // >> browse button (relies on the find_browse_pair above)
            if( next != ctx ) {
                whp_fprintf( Out_file, "%c" HB_NEXT "%c", Hyper_Brace_L,
                                CHR_HLINK_BREAK );
                str_out_ib( Out_file, next->title );
                whp_fprintf( Out_file, "%c ", Hyper_Brace_R );
            } else {
                fake_hlink( Out_file, HB_NEXT );
            }
        }

        if( Do_idx_button ) {
            if( stricmp( ctx->ctx_name, "index_of_topics" ) != 0 ) {
                whp_fprintf( Out_file,
                                "%c" HB_INDEX "%cIndex of Topics%c ",
                                Hyper_Brace_L,
                                CHR_HLINK_BREAK,
                                Hyper_Brace_R );
            } else {
                fake_hlink( Out_file, HB_INDEX );
            }
        }

        // "Up" button
        if( Do_up ) {
            // find "parent" context
            for( temp_ctx = ctx->up_ctx; temp_ctx != NULL;
                                        temp_ctx = temp_ctx->up_ctx ) {
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
                whp_fprintf( Out_file, "%c" HB_UP "%c",
                                        Hyper_Brace_L, CHR_HLINK_BREAK );
                str_out_ib( Out_file, temp_ctx->title );
                whp_fprintf( Out_file, "%c ", Hyper_Brace_R );
            } else {
                fake_hlink( Out_file, HB_UP );
            }
        }
        // append user header file
        ib_append_line( Out_file, Header_File );

        // end of header
        whp_fprintf( Out_file, "\n:eh\n" );
    }
    // append user footer file
    if( Footer_File[0] != '\0' ) {
        whp_fprintf( Out_file, ":f\n" );
        ib_append_line( Out_file, Footer_File );
        whp_fprintf( Out_file, "\n:ef\n" );
    }
}

static void output_end(
/*********************/

    void
) {
    whp_fprintf( Out_file, "\n" );
}

static void output_section_ib( section_def *section )
/***************************************************/
{
    int                         len;
    ctx_def                     *ctx;
    unsigned int                line;
    unsigned char               *label;
    int                         label_len;
    int                         ch;
    char                        *file;
    char                        *topic;
    unsigned char               *p;
    unsigned char               *end;

    p = (unsigned char *)section->section_text;
    end = p + section->section_size;
    len = 0;
    while( p + len < end ) {
        // stop when we hit a hyper-link
        if( *(p + len) != CHR_TEMP_HLINK ) {
            len++;
        } else {
            // write out the block of text we've got so far
            whp_fwrite( p, 1, len, Out_file );
            p += len + 1;

            // grab the line number
            for( len = 0; ; ++len ) {
                if( *(p + len) == CHR_TEMP_HLINK ) {
                    break;
                }
            }
            p[ len ] = '\0';
            line = atoi( (char *)p );
            p += len + 1;

            // find the length of the link label (what the user sees)
            for( len = 0; ; ++len ) {
                if( *(p + len) == CHR_HLINK_BREAK ) {
                    break;
                }
            }

            // if we're using the brace mode we strip off the "XX"
            if( Hyper_Brace_L == '<' ) {
                p += 2;
                len -= 2;
            }

            // output the label and the break
            label = p;
            label_len = len + 1;
            p += len + 1;

            // find the length of the link context
            for( len = 0; ; ++len ) {
                ch = *(p + len);
                if( ch == CHR_TEMP_HLINK || ch == CHR_HLINK_BREAK ) {
                    break;
                }
            }
            // null terminate the context name, and find the associated topic
            *(p + len) = '\0';
            topic = (char *)p;
            ctx = find_ctx( topic );

            // output the topic name that belongs to the context
            if( ctx == NULL && ch != CHR_HLINK_BREAK ) {
                warning( "Link to nonexistent context", line );
                printf( "For topic=%s\n", topic );
                whp_fwrite( label, 1, label_len - 1, Out_file );
            } else {
                // now we start writing the hyper-link
                whp_fwrite( &Hyper_Brace_L, 1, 1, Out_file );
                whp_fwrite( label, 1, label_len, Out_file );
                if( ctx != NULL ) {
                    str_out_ib( Out_file, ctx->title );
                } else {
                    str_out_ib( Out_file, topic );
                }
                if( ch == CHR_HLINK_BREAK ) {
                    /* file link. Get the file name */
                    file = (char *)(p + len + 1);
                    for( ;; ) {
                        ++len;
                        if( *(p + len) == CHR_TEMP_HLINK ) {
                            break;
                        }
                    }
                    *(p + len) = '\0';
                    whp_fprintf( Out_file, "%c%s", CHR_HLINK_BREAK, file );
                }
                whp_fwrite( &Hyper_Brace_R, 1, 1, Out_file );
            }

            // adjust the len and ctr counters appropriately
            p += len + 1;
            len = 0;
        }
    }
    // output whatever's left
    if( p < end ) {
        whp_fwrite( p, 1, end - p, Out_file );
    }
}

static void output_ctx_sections(
/******************************/

    ctx_def                     *ctx
) {
    section_def                 *section;

    for( section = ctx->section_list; section != NULL; ) {
        if( section->section_size > 0 ) {
            output_section_ib( section );
        }
        section = section->next;
    }
}

void ib_output_file( void )
/*************************/
{
    ctx_def                     *ctx;

    if( IB_def_topic != NULL ) {
        fprintf( Out_file, "DEFTOPIC::::\"%s\"\n", IB_def_topic );
        free( IB_def_topic );
    }
    if( IB_help_desc != NULL ) {
        fprintf( Out_file, "DESCRIPTION::::\"%s\"\n", IB_help_desc );
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

