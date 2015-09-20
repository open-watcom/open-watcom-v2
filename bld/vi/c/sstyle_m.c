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
* Description:  Support for makefile syntax.
*
****************************************************************************/


#include "vi.h"
#include "sstyle.h"
#include "sstyle_m.h"


/* Note: We're cheating and only have a single list of keywords.
 * We don't expect people to try '.ifdef' or '!autodepend'.
 */

/*----- LOCALS -----*/

static  ss_flags_m  flags;
static  char        *firstNonWS;
static  char        *firstChar;

void InitMkLine( char *text )
{
    firstChar = text;
    while( *text != '\0' && isspace( *text ) ) {
        text++;
    }
    firstNonWS = text;
}

static void getWhiteSpace( ss_block *ss_new, char *start )
{
    char    *text = start + 1;

    while( isspace( *text ) ) {
        text++;
    }
    ss_new->type = SE_WHITESPACE;
    ss_new->len = text - start;
}

static void getText( ss_block *ss_new, char *start )
{
    char    *text = start + 1;

    // gather up symbol
    while( isalnum( *text ) ) {
        text++;
    }
    ss_new->type = SE_IDENTIFIER;

    ss_new->len = text - start;
}

static void getPreproc( ss_block *ss_new, char *start )
{
    char    *text = start + 1;
    char    *keyword;
    char    save_char;

    // whitespace is allowed after '!'
    while( isspace( *text ) ) {
        text++;
    }
    keyword = text;
    // gather up symbol
    while( isalpha( *text ) ) {
        text++;
    }

    ss_new->type = SE_IDENTIFIER;

    // see if symbol is a keyword
    save_char = *text;
    *text = '\0';
    if( IsKeyword( keyword, true ) ) {
        ss_new->type = SE_PREPROCESSOR;
        flags.inPreproc = false;
    }
    *text = save_char;

    ss_new->len = text - start;
}

static void getDirective( ss_block *ss_new, char *start )
{
    char    *text = start + 1;
    char    save_char;

    // gather up symbol
    while( isalpha( *text ) ) {
        text++;
    }
    ss_new->type = SE_IDENTIFIER;

    // see if symbol is a keyword
    save_char = *text;
    *text = '\0';
    if( IsKeyword( start + 1, true ) ) {
        ss_new->type = SE_KEYWORD;
    }
    *text = save_char;

    ss_new->len = text - start;
}

static void getSymbol( ss_block *ss_new )
{
    ss_new->type = SE_SYMBOL;
    ss_new->len = 1;
}

static void getBeyondText( ss_block *ss_new )
{
    ss_new->type = SE_WHITESPACE;
    ss_new->len = BEYOND_TEXT;
}

static void getComment( ss_block *ss_new, char *start )
{
    char    *text = start + 1;

    // everything is a comment until the end of line
    while( *text != '\0' ) {
        text++;
    }
    ss_new->type = SE_COMMENT;
    ss_new->len = text - start;
}

static void getMacro( ss_block *ss_new, char *start, int skip )
{
    char    *nstart = start + skip;
    char    *text = nstart;

    ss_new->type = SE_STRING;

    if( *text && *text == '(' ) {
        int     nesting = 1;

        // parse a parenthesized macro
        ++text;
        // the '%' char is only allowed at start of macro
        if( *text == '%' ) {
            ++text;
        }

        while( *text != '\0' && nesting && (isalnum( *text ) || (*text == '_')
            || (*text == '(') || (*text == ')') || (*text == '$')
            || (*text == '&') || (*text == '[')) ) {
            if( *text == '(' ) {
                ++nesting;
            }
            if( *text == ')' ) {
                --nesting;
            }
            ++text;
        }
    } else if( *text != '\0' && isalnum( *text ) ) {
        // parse a non-parenthesized macro
        ++text;
        while( *text != '\0' && (isalnum( *text ) || (*text == '_')) ) {
            ++text;
        }
    } else {
        bool    quit = false;

        // let's try a special macro
        while( *text != '\0' && !quit ) {
            switch( *text ) {
            case '^':
            case '[':
            case ']':
                ++text;
                break;
            case '@':
            case '*':
            case '&':
            case '.':
            case ':':
            case '$':
            case '#':
            case '<':
            case '?':
                quit = true;
                ++text;
                break;
            default:
                quit = true;
                break;
            }
        }
    }

    flags.inMacro = false;
    ss_new->len = text - start;
}

void InitMkFlagsGivenValues( ss_flags_m *newFlags )
{
    flags = *newFlags;
}

void GetMkFlags( ss_flags_m *storeFlags )
{
    *storeFlags = flags;
}

void InitMkFlags( linenum line_no )
{
    line_no = line_no;
    flags.inPreproc = false;
    flags.inInlineFile = false;
    flags.inMacro = false;
}

void GetMkBlock( ss_block *ss_new, char *start, int line )
{
    line = line;

    if( start[0] == '\0' ) {
        if( firstNonWS == start ) {
            // line is empty -
            // do not flag following line as having anything to do
            // with an unterminated macro
            flags.inMacro = false;
        }
        getBeyondText( ss_new );
        return;
    }

    // Preprocessor directives must start at beginning of line
    if( (start[0] == '!') && (firstChar == start) ) {
        getPreproc( ss_new, start );
        return;
    }

    if( flags.inMacro ) {
        getMacro( ss_new, start, 0 );
        return;
    }

    if( isspace( start[0] ) ) {
        getWhiteSpace( ss_new, start );
        return;
    }

    switch( start[0] ) {
    case '$':
        getMacro( ss_new, start, 1 );
        return;
    case '#':
        getComment( ss_new, start );
        return;
    case '.':
        getDirective( ss_new, start );
        return;
    }

    if( isalpha( start[0] ) ) {
        getText( ss_new, start );
    } else {
        getSymbol( ss_new );
    }
}
