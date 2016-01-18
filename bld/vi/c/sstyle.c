/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Syntax highlighting control module.
*
****************************************************************************/


#include "vi.h"
#include "sstyle.h"
#include "sstyle_c.h"
#include "sstyle_f.h"
#include "sstyle_h.h"
#include "sstyle_g.h"
#include "sstyle_m.h"
#include "sstyle_p.h"
#include <assert.h>


/*----- EXPORTS -----*/
type_style      SEType[SE_NUMTYPES];


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

    while( *text != '\0' ) {
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

    if( CurrentInfo == NULL ) {
        return;
    }

    switch( CurrentInfo->fsi.Language ) {
    case LANG_NONE:
        getText( ss_new, text + text_col );
        break;
    case LANG_HTML:
    case LANG_WML:
        GetHTMLBlock( ss_new, text + text_col, text_col );
        break;
    case LANG_GML:
        GetGMLBlock( ss_new, text + text_col, text_col );
        break;
    case LANG_MAKEFILE:
        GetMkBlock( ss_new, text + text_col, text_col );
        break;
    case LANG_FORTRAN:
        GetFORTRANBlock( ss_new, text + text_col, text_col );
        break;
    case LANG_PERL:
    case LANG_AWK:
        GetPerlBlock( ss_new, text + text_col, line, line_no );
        break;
    case LANG_C:
    case LANG_CPP:
    case LANG_JAVA:
    case LANG_SQL:
    case LANG_BAT:
    case LANG_BASIC:
    case LANG_DBTEST:
    case LANG_RC:
    case LANG_USER:
    default:
        GetCBlock( ss_new, text + text_col, line, line_no );
        break;
    }

    /* GetCBlock, GetFORTRANBlock, GetHTMLBlock and getText fill in only ss->len.
       use this to get an index into the text, stored in ss->end.
    */
    ss_new->end = ss_new->len;
    if( ss_new->len != BEYOND_TEXT ) {
        ss_new->end = ss_new->len + text_col - 1;
    }
}

static void addSelection( ss_block *ss_start, linenum line_no )
{
    int             sel_start_col, sel_end_col;
    int             sel_start_line, sel_end_line;
    int             tmp, i;
    bool            swap_cols;
    ss_block        *ss, *ss2;
    static ss_block ss_copy[MAX_SS_BLOCKS];
    ss_block        ss_save;

    // don't call me unless something selected
    assert( SelRgn.selected );
    i = 0;

    // get nicely ordered values from SelRgn
    sel_end_col = VirtualColumnOnCurrentLine( SelRgn.end.column ) - 1;
    sel_start_col = SelRgn.start_col_v - 1;
#ifdef __WIN__
    if( EditFlags.RealTabs ) {
        sel_end_col = SelRgn.end.column - 1;
        sel_start_col = SelRgn.start.column - 1;
    }
#endif
    swap_cols = false;
    if( SelRgn.start.line > SelRgn.end.line ) {
        sel_start_line = SelRgn.end.line;
        sel_end_line = SelRgn.start.line;
        swap_cols = true;
    } else {
        sel_start_line = SelRgn.start.line;
        sel_end_line = SelRgn.end.line;
    }
    if( SelRgn.start.line == SelRgn.end.line && sel_start_col > sel_end_col ) {
        swap_cols = true;
    }
    if( swap_cols ) {
        tmp = sel_start_col;
        sel_start_col = sel_end_col;
        sel_end_col = tmp;
    }

    // select entire line
    if( (sel_start_line < line_no && sel_end_line > line_no) ||
        (SelRgn.lines && (sel_start_line == line_no || sel_end_line == line_no)) ) {
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
        for( ss = ss_start; ss->end < sel_start_col; ++ss ) {
            *ss2 = *ss;
            ss2++;
        }
        ss_save = *ss;
        if( (ss == ss_start && sel_start_col > 0) ||
            (ss != ss_start && (ss - 1)->end + 1 != sel_start_col) ) {
            ss2->type = ss->type;
            ss2->len = ss->len - (ss->end - sel_start_col + 1);
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
        if( (ss == ss_start && sel_start_col > 0) ||
            (ss != ss_start && (ss - 1)->end + 1 != sel_start_col) ) {
            // split block just to the left
            ss->len -= ss->end - sel_start_col + 1;
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
        for( ss = ss_start; ss->end < sel_end_col; ++ss ) {
            i++;
        }
        ss2 = ss_start + 1;
        ss_save = *ss2;
        if( ss->end >= sel_end_col ) {
            ss_save.type = ss->type;
            ss_save.len = ss->end - sel_end_col + 1;
            if( ss_save.len != 0 ) {
                ss_save.end = ss->end;
                ss2++;
                i++;
            }
        }
        memmove( ss2, ss + 1, (MAX_SS_BLOCKS - i - 1) * sizeof( ss_block ) );
        ss_start->type = SE_SELECTION;
        ss_start->end = sel_end_col - 1;
        ss_start->len = sel_end_col;
        if( ss2 == ss_start + 2 ) {
            *(ss_start + 1) = ss_save;
        }
        return;
    }

    // otherwise nothing is selected on this line
}

static void fixSelection( ss_block *ss_start, int start_col )
{
    ss_block    *ss;
    int         i = MAX_SS_BLOCKS;

    for( ss = ss_start; ss->end < start_col; ++ss ) {
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
    bool        changed;
    bool        anychange;
    int         text_col;

    *dif = 0;
    index = 0;
    anychange = false;
    switch( CurrentInfo->fsi.Language ) {
    case LANG_C:
    case LANG_CPP:
    case LANG_JAVA:
    case LANG_SQL:
    case LANG_BAT:
    case LANG_BASIC:
    case LANG_DBTEST:
    case LANG_RC:
    case LANG_USER:
        InitCLine( text );
        break;
    case LANG_FORTRAN:
        InitFORTRANLine( text, line_no );
        break;
    case LANG_HTML:
    case LANG_WML:
        InitHTMLLine( text );
        break;
    case LANG_GML:
        InitGMLLine( text );
        break;
    case LANG_MAKEFILE:
        InitMkLine( text );
        break;
    case LANG_PERL:
    case LANG_AWK:
        InitPerlLine( text );
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

        changed = ( memcmp( ss_inc, &ss_new, sizeof( ss_block ) ) != 0 );
        if( changed ) {
            memcpy( ss_inc, &ss_new, sizeof( ss_block ) );
        }
        anychange |= changed;
        if( !anychange ) {
            *dif = ss_inc->end + 1;
        }
    } while( (ss_inc++)->end != BEYOND_TEXT );

    if( SelRgn.selected ) {
        addSelection( ss_old, line_no );
        fixSelection( ss_old, start_col );
    }

    // change origin of ->ends from text[0] to text[start_col]
    for( ss_inc = ss_old; ss_inc->end != BEYOND_TEXT; ++ss_inc ) {
        ss_inc->end -= start_col;
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

    if( CurrentInfo != NULL ) {
        switch( CurrentInfo->fsi.Language ) {
        case LANG_C:
        case LANG_CPP:
        case LANG_JAVA:
        case LANG_SQL:
        case LANG_BAT:
        case LANG_BASIC:
        case LANG_PERL:
        case LANG_DBTEST:
        case LANG_RC:
        case LANG_AWK:
        case LANG_USER:
            if( ch == '#' || ch == '"' || ch == '/' || ch == '*' ) {
                return( true );
            }
            break;
        case LANG_FORTRAN:
            if( ch == '\'' ) {
                return( true );
            }
            break;
        case LANG_HTML:
        case LANG_WML:
            if( ch == '<' || ch == '>' ) {
                return( true );
            }
            break;
        case LANG_GML:
            if( ch == ':' || ch == '.' ) {
                return( true );
            }
            break;
        case LANG_MAKEFILE:
            if( ch == '#' ) {
                return( true );
            }
            break;
        }
    }
    return( false );
}

void SSInitLanguageFlags( linenum line_no )
{
    if( CurrentInfo != NULL ) {
        switch( CurrentInfo->fsi.Language ) {
        case LANG_C:
        case LANG_CPP:
        case LANG_JAVA:
        case LANG_SQL:
        case LANG_BAT:
        case LANG_BASIC:
        case LANG_DBTEST:
        case LANG_RC:
        case LANG_USER:
            InitCFlags( line_no );
            break;
        case LANG_FORTRAN:
            InitFORTRANFlags( line_no );
            break;
        case LANG_HTML:
        case LANG_WML:
            InitHTMLFlags( line_no );
            break;
        case LANG_GML:
            InitGMLFlags( line_no );
            break;
        case LANG_MAKEFILE:
            InitMkFlags( line_no );
            break;
        case LANG_PERL:
        case LANG_AWK:
            InitPerlFlags( line_no );
            break;
        }
    }
}

void SSInitLanguageFlagsGivenValues( ss_flags *flags )
{
    if( CurrentInfo != NULL ) {
        switch( CurrentInfo->fsi.Language ) {
        case LANG_C:
        case LANG_CPP:
        case LANG_JAVA:
        case LANG_SQL:
        case LANG_BAT:
        case LANG_BASIC:
        case LANG_DBTEST:
        case LANG_RC:
        case LANG_USER:
            InitCFlagsGivenValues( &flags->c );
            break;
        case LANG_FORTRAN:
            InitFORTRANFlagsGivenValues( &flags->f );
            break;
        case LANG_HTML:
        case LANG_WML:
            InitHTMLFlagsGivenValues( &flags->h );
            break;
        case LANG_GML:
            InitGMLFlagsGivenValues( &flags->g );
            break;
        case LANG_MAKEFILE:
            InitMkFlagsGivenValues( &flags->m );
            break;
        case LANG_PERL:
        case LANG_AWK:
            InitPerlFlagsGivenValues( &flags->p );
            break;
        }
    }
}

void SSGetLanguageFlags( ss_flags *flags )
{
    if( CurrentInfo != NULL ) {
        switch( CurrentInfo->fsi.Language ) {
        case LANG_C:
        case LANG_CPP:
        case LANG_JAVA:
        case LANG_SQL:
        case LANG_BAT:
        case LANG_BASIC:
        case LANG_DBTEST:
        case LANG_RC:
        case LANG_USER:
            GetCFlags( &flags->c );
            break;
        case LANG_FORTRAN:
            GetFORTRANFlags( &flags->f );
            break;
        case LANG_HTML:
        case LANG_WML:
            GetHTMLFlags( &flags->h );
            break;
        case LANG_GML:
            GetGMLFlags( &flags->g );
            break;
        case LANG_MAKEFILE:
            GetMkFlags( &flags->m );
            break;
        case LANG_PERL:
        case LANG_AWK:
            GetPerlFlags( &flags->p );
            break;
        }
    }
}

#ifdef __WIN__
syntax_element SSGetStyle( int row, int col )
{
    dc_line     *c_line;
    ss_block    *ss;

    c_line = DCFindLine( row - 1, CurrentWindow );
    assert( c_line->valid );
    if( c_line->start_col != LeftTopPos.column ) {
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
    syntax_element  i;

    for( i = 0; i < SE_NUMTYPES; i++ ) {
        SEType[i].foreground = -1;
        SEType[i].background = -1;
        SEType[i].font = FONT_DEFAULT;
    }
}

/*
 * SSInitAfterConfig - init that happens after reading config file
 */
void SSInitAfterConfig( void )
{
    syntax_element  i;

    // text must have some color
    if( SEType[SE_TEXT].foreground == -1 ) {
        SEType[SE_TEXT].foreground = WHITE;
        SEType[SE_TEXT].background = BLACK;
    }

    // selection should default to inverse of text
    if( SEType[SE_SELECTION].foreground == -1 ) {
        SEType[SE_SELECTION].foreground = SEType[SE_TEXT].background;
        SEType[SE_SELECTION].background = SEType[SE_TEXT].foreground;
    }

    // any syntax style not specified defaults to SE_TEXT style
    for( i = 0; i < SE_NUMTYPES; i++ ) {
        if( SEType[i].foreground == -1 ) {
            SEType[i].foreground = SEType[SE_TEXT].foreground;
            SEType[i].background = SEType[SE_TEXT].background;
        }
    }
}

/*
 * SSFini - deinitialize this module
 */
void SSFini( void )
{
}
