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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "vi.h"
#include "colors.h"
#include "sstyle.h"
#include "lang.h"
#include "sstyle_c.h"
#include "sstyle_f.h"


/*----- EXPORTS -----*/
type_style      SEType[ SE_NUMTYPES ];


static void getEOFText( ss_block *ss_new, char *text )
{
    if( *text ) {
        ss_new->type = SE_EOFTEXT;
        ss_new->len = strlen( text ) - 1;
        ss_new->end = ss_new->len;
    } else {
        ss_new->type = SE_WHITESPACE;
        ss_new->len = BEYOND_TEXT;
        ss_new->end = ss_new->len;
    }
}

static void getText( ss_block *ss_new, char *text )
{
    char    *start = text;

    if( *text == '\0' ) {
        ss_new->type = SE_WHITESPACE;
        ss_new->len = BEYOND_TEXT;
        return;
    }

    while( *text ) {
        text++;
    }
    ss_new->type = SE_TEXT;
    ss_new->len = text - start;
}

static void getNextBlock( ss_block *ss_new, char *text, int text_col,
                          line *line, linenum line_no )
{
    line_no = line_no;

    if( line == NULL ) {
        getEOFText( ss_new, text + text_col );
        return;
    }

    if( CurrentInfo == NULL ) return;

    switch( CurrentInfo->Language ) {
        case LANG_NONE:
            getText( ss_new, text + text_col );
            break;
        case LANG_FORTRAN:
            GetFORTRANBlock( ss_new, text + text_col, text_col );
            break;
        case LANG_JAVA:
        case LANG_C:
        case LANG_CPP:
        case LANG_SQL:
        case LANG_BAT:
        case LANG_BASIC:
        case LANG_PERL:
        default:
            GetCBlock( ss_new, text + text_col, line, line_no );
            break;
    }

    /* GetCBlock, GetFORTRANBlock and getText fill in only ss->len.
       use this to get an index into the text, stored in ss->end.
    */
    ss_new->end = ss_new->len;
    if( ss_new->len != BEYOND_TEXT ) {
        ss_new->end = ss_new->len + text_col - 1;
    }
}

void addSelection( ss_block *ss_start, linenum line_no )
{
    int         sel_start_col, sel_end_col;
    int         sel_start_line, sel_end_line;
    int         tmp, i;
    bool        swap_cols;
    ss_block    *ss, *ss2;
    static ss_block ss_copy[ MAX_SS_BLOCKS ];
    ss_block    ss_save;

    // don't call me unless something selected
    assert( SelRgn.selected );
    i = 0;

    // get nicely ordered values from SelRgn
    sel_end_col = VirtualCursorPosition2( SelRgn.end_col ) - 1;
    sel_start_col = SelRgn.start_col_v - 1;
    #ifdef __WIN__
    if( EditFlags.RealTabs ){
        sel_end_col = SelRgn.end_col - 1;
        sel_start_col = SelRgn.start_col- 1;
    }
    #endif
    swap_cols = FALSE;
    if( SelRgn.start_line > SelRgn.end_line ) {
        sel_start_line = SelRgn.end_line;
        sel_end_line = SelRgn.start_line;
        swap_cols = TRUE;
    } else {
        sel_start_line = SelRgn.start_line;
        sel_end_line = SelRgn.end_line;
    }
    if( SelRgn.start_line == SelRgn.end_line &&
        sel_start_col > sel_end_col ) {
        swap_cols = TRUE;
    }
    if( swap_cols ) {
        tmp = sel_start_col;
        sel_start_col = sel_end_col;
        sel_end_col = tmp;
    }

    // select entire line
    if( ( sel_start_line < line_no && sel_end_line > line_no ) ||
        ( SelRgn.lines &&
            ( sel_start_line == line_no || sel_end_line == line_no ) ) ) {
        ss_start->type = SE_SELECTION;
        ss_start->end = ss_start->len = BEYOND_TEXT;
        return;
    }

    // select subset within current line
    if( sel_start_line == sel_end_line && sel_start_line == line_no ) {
        if( sel_start_col == sel_end_col ) {
            return;
        }
        ss2 = ss_copy;
        ss = ss_start;
        while( ss->end < sel_start_col ) {
            *ss2 = *ss;
            ss2++;
            ss++;
        }
        ss_save = *ss;
        if( ( ss == ss_start && sel_start_col > 0 ) ||
            ( ss != ss_start && ( ss - 1 )->end + 1 != sel_start_col ) ) {
            ss2->type = ss->type;
            ss2->len = ss->len - ( ss->end - sel_start_col + 1 );
            if( ss2->len != 0 ) {
                ss2->end = sel_start_col - 1;
                ss2++;
            }
        }
        ss2->type = SE_SELECTION;
        ss2->len = sel_end_col - sel_start_col;
        ss2->end = sel_end_col - 1;
        ss2++;
        if( ss_save.end >= sel_end_col ) {
            ss2->type = ss_save.type;
            ss2->len = ss_save.end - sel_end_col + 1;
            if( ss2->len != 0 ) {
                ss2->end = ss_save.end;
                ss2++;
            }
        } else {
            while( ss->end < sel_end_col ) {
                ss++;
            }
            if( ss->end >= sel_end_col ) {
                ss2->type = ss->type;
                ss2->len = ss->end - sel_end_col + 1;
                if( ss2->len != 0 ) {
                    ss2->end = ss->end;
                    ss2++;
                }
            }
        }
        if( ss->end != BEYOND_TEXT ) {
            do {
                ss++;
                *ss2 = *ss;
                ss2++;
            } while( ss->end != BEYOND_TEXT );
        }
        memcpy( ss_start, ss_copy, MAX_SS_BLOCKS * sizeof( ss_block ) );
        return;
    }

    // select from start of selection to far right
    if( sel_start_line == line_no ) {
        ss = ss_start;
        while( ss->end < sel_start_col ) {
            ss++;
        }
        if( ( ss == ss_start && sel_start_col > 0 ) ||
            ( ss != ss_start && ( ss - 1 )->end + 1 != sel_start_col ) ) {
            // split block just to the left
            ss->len -= ( ss->end - sel_start_col + 1 );
            if( ss->len != 0 ) {
                ss->end = sel_start_col - 1;
                ss++;
            }
        }
        ss->type = SE_SELECTION;
        ss->end = ss->len = BEYOND_TEXT;
        return;
    }

    // select from far left to end of selection
    if( sel_end_line == line_no && sel_end_col != 0 ) {
        ss = ss_start;
        while( ss->end < sel_end_col ) {
            ss++;
            i++;
        }
        ss2 = ss_start + 1;
        if( ss->end >= sel_end_col ) {
            ss_save.type = ss->type;
            ss_save.len = ss->end - sel_end_col + 1;
            if( ss_save.len != 0 ) {
                ss_save.end = ss->end;
                ss2++;
                i++;
            }
        }
        memmove( ss2, ss + 1, ( MAX_SS_BLOCKS - i - 1 ) * sizeof( ss_block ) );
        ss_start->type = SE_SELECTION;
        ss_start->end = sel_end_col - 1;
        ss_start->len = sel_end_col;
        if( ss2 == ss_start + 2 ) {
            *( ss_start + 1 ) = ss_save;
        }
        return;
    }

    // otherwise nothing is selected on this line
}

void fixSelection( ss_block *ss_start, int start_col )
{
    ss_block    *ss;
    int i = MAX_SS_BLOCKS;

    ss = ss_start;
    while( ss->end < start_col ) {
        ss++;
        i--;
    }
    if( ss == ss_start ) {
        return;
    }
    memmove( ss_start, ss, i * sizeof( ss_block ) );
    ss_start->len -= start_col;
}


// NOTE! for this to work ...
// ss_old must point the the head of a
void SSDifBlock( ss_block *ss_old, char *text, int start_col,
                 line *line, linenum line_no, int *dif )
{
    ss_block    ss_new, *ss_inc;
    int         index;
    int         changed;
    int         anychange;
    int         text_col;

    *dif = 0;
    index = 0;
    anychange = FALSE;
    switch( CurrentInfo->Language ) {
        case LANG_JAVA:
        case LANG_C:
        case LANG_CPP:
        case LANG_SQL:
        case LANG_BAT:
        case LANG_BASIC:
        case LANG_PERL:
            InitCLine( text );
            break;
        case LANG_FORTRAN:
            InitFORTRANLine( text, line_no );
            break;
    }
    ss_inc = ss_old;
    text_col = 0;
    do {
        ss_new.type = SE_UNPARSED;
        do {
            getNextBlock( &ss_new, text, text_col, line, line_no );
            // -3 because as many as 2 extra blocks could be needed
            // when selection is inserted
            if( index == MAX_SS_BLOCKS - 3 && ss_new.end != BEYOND_TEXT ) {
                // out of room - fake rest of line
                ss_new.end = BEYOND_TEXT;
                break;
            }
            index++;
            text_col = ss_new.end + 1;
        } while( ss_new.end < start_col );

        changed = memcmp( ss_inc, &ss_new, sizeof( ss_block ) );
        if( changed ) {
            memcpy( ss_inc, &ss_new, sizeof( ss_block ) );
        }
        anychange |= changed;
        if( !anychange ) {
            *dif = ss_inc->end + 1;
        }
    } while( ( ss_inc++ )->end != BEYOND_TEXT );

    if( SelRgn.selected ) {
        addSelection( ss_old, line_no );
        fixSelection( ss_old, start_col );
    }

    // change origin of ->ends from text[0] to text[start_col]
    ss_inc = ss_old;
    while( ss_inc->end != BEYOND_TEXT ) {
        ss_inc->end -= start_col;
        ss_inc++;
    }
}

ss_block *SSNewBlock( void )
{
    return( MemAlloc( MAX_SS_BLOCKS * sizeof( ss_block ) ) );
}

void SSKillBlock( ss_block *ss )
{
    MemFree( ss );
}

bool SSKillsFlags( char ch )
{
    // Warning!  contains language-specific information
    int lang;
    if( CurrentInfo == NULL ) return( FALSE );

    lang = CurrentInfo->Language;

    if( ( lang == LANG_C ||
          lang == LANG_CPP ||
          lang == LANG_JAVA ||
          lang == LANG_SQL ||
          lang == LANG_BAT ||
          lang == LANG_BASIC ||
          lang == LANG_PERL )
        && ( ch == '#' || ch == '"' || ch == '/' || ch == '*' ) ) {
        return( TRUE );
    }
    if( lang == LANG_FORTRAN && ch == '\'' ) {
        return( TRUE );
    }
    return( FALSE );
}

void SSInitLanguageFlags( linenum line_no )
{
    if( CurrentInfo != NULL ) {
        switch( CurrentInfo->Language ) {
            case LANG_JAVA:
            case LANG_C:
            case LANG_CPP:
            case LANG_SQL:
            case LANG_BAT:
            case LANG_BASIC:
            case LANG_PERL:
                InitCFlags( line_no );
                break;
            case LANG_FORTRAN:
                InitFORTRANFlags( line_no );
                break;
        }
    }
}

void SSInitLanguageFlagsGivenValues( ss_flags *flags )
{
    if( CurrentInfo != NULL ) {
        switch( CurrentInfo->Language ) {
            case LANG_JAVA:
            case LANG_C:
            case LANG_CPP:
            case LANG_SQL:
            case LANG_BAT:
            case LANG_BASIC:
            case LANG_PERL:
                InitCFlagsGivenValues( &( flags->c ) );
                break;
            case LANG_FORTRAN:
                InitFORTRANFlagsGivenValues( &( flags->f ) );
                break;
        }
    }
}

void SSGetLanguageFlags( ss_flags *flags )
{
    if( CurrentInfo != NULL ) {
        switch( CurrentInfo->Language ) {
            case LANG_JAVA:
            case LANG_C:
            case LANG_CPP:
            case LANG_SQL:
            case LANG_BAT:
            case LANG_BASIC:
            case LANG_PERL:
                GetCFlags( &( flags->c ) );
                break;
            case LANG_FORTRAN:
                GetFORTRANFlags( &( flags->f ) );
                break;
        }
    }
}

#ifdef __WIN__
int SSGetStyle( int row, int col )
{
    dc          c_line;
    ss_block    *ss;

    c_line = DCFindLine( row - 1, CurrentWindow );
    assert( c_line->valid );
    if( c_line->start_col != LeftColumn ) {
        // text is scrolled off screen - ws remains
        return( SE_WHITESPACE );
    }
    // count across to column
    ss = c_line->ss;
    while( ss->end < col ) {
        ss++;
    }
    return( ss->type );
}
#endif

/*
 * SSInitBeforeConfig - init that happens before reading config file
 */
void SSInitBeforeConfig( void )
{
    int i;

    for( i = 0; i < SE_NUMTYPES; i++ ) {
        SEType[ i ].foreground = -1;
        SEType[ i ].background = -1;
        SEType[ i ].font = 0;
    }
}

/*
 * SSInitAfterConfig - init that happens after reading config file
 */
void SSInitAfterConfig( void )
{
    int i;

    // text must have some color
    if( SEType[ SE_TEXT ].foreground == -1 ) {
        SEType[ SE_TEXT ].foreground = WHITE;
        SEType[ SE_TEXT ].background = BLACK;
    }

    // selection should default to inverse of text
    if( SEType[ SE_SELECTION ].foreground == -1 ) {
        SEType[ SE_SELECTION ].foreground = SEType[ SE_TEXT ].background;
        SEType[ SE_SELECTION ].background = SEType[ SE_TEXT ].foreground;
    }

    // any syntax style not specified defaults to SE_TEXT style
    for( i = 0; i < SE_NUMTYPES; i++ ) {
        if( SEType[ i ].foreground == -1 ) {
            SEType[ i ].foreground = SEType[ SE_TEXT ].foreground;
            SEType[ i ].background = SEType[ SE_TEXT ].background;
        }
    }
}

/*
 * SSFini - deinitialize this module
 */
void SSFini( void )
{
}
