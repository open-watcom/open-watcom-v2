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
    SKIP_SPACES( text );
    firstNonWS = text;
}

static void getText( ss_block *ss_new, char *start )
{
    char    *end = start + 1;

    // gather up symbol
    SKIP_SYMBOL( end );
    ss_new->type = SE_IDENTIFIER;
    ss_new->len = end - start;
}

static bool isElse( const char *start )
{
    return( ( start[0] == 'e' || start[0] == 'E' )
        && ( start[1] == 'l' || start[1] == 'L' )
        && ( start[2] == 's' || start[2] == 'S' )
        && ( start[3] == 'e' || start[3] == 'E' )
        && ( start[4] == '\0' || isspace( start[4] ) ) );
}

static bool isIf( const char *start )
{
    /* check only for first two characters, that include all "if.." variants */
    return( ( start[0] == 'i' || start[0] == 'I' )
        && ( start[1] == 'f' || start[1] == 'F' ) );
}

static void getPreproc( ss_block *ss_new, char *start )
{
    char    *end = start + 1;
    char    *keyword;
    char    *end2;
    char    *keyword2;

    // whitespace is allowed after '!'
    SKIP_SPACES( end );
    keyword = end;
    // gather up symbol
    SKIP_SYMBOL( end );

    ss_new->type = SE_IDENTIFIER;

    // see if symbol is a keyword
    if( IsKeyword( keyword, end, true ) ) {
        /* check "else .." */
        if( isspace( *end ) && isElse( keyword ) ) {
            end2 = end + 1;
            SKIP_SPACES( end2 );
            /* check "else if.." */
            if( isIf( end2 ) ) {
                keyword2 = end2;
                SKIP_SYMBOL( end2 );
                if( IsKeyword( keyword2, end2, true ) ) {
                    end = end2;
                }
            }
        }
        ss_new->type = SE_PREPROCESSOR;
        flags.inPreproc = false;
    }

    ss_new->len = end - start;
}

static void getDirective( ss_block *ss_new, char *start )
{
    char    *end = start + 1;

    // gather up symbol
    SKIP_SYMBOL( end );
    ss_new->type = SE_IDENTIFIER;

    // see if symbol is a keyword
    if( IsKeyword( start + 1, end, true ) ) {
        ss_new->type = SE_KEYWORD;
    }
    ss_new->len = end - start;
}

static void getSymbol( ss_block *ss_new )
{
    ss_new->type = SE_SYMBOL;
    ss_new->len = 1;
}

static void getComment( ss_block *ss_new, char *start )
{
    char    *end = start + 1;

    // everything is a comment until the end of line
    SKIP_TOEND( end );
    ss_new->type = SE_COMMENT;
    ss_new->len = end - start;
}

static void getMacro( ss_block *ss_new, char *start, int skip )
{
    char    *nstart = start + skip;
    char    *end = nstart;

    ss_new->type = SE_STRING;

    if( *end != '\0' && *end == '(' ) {
        int     nesting = 1;

        // parse a parenthesized macro
        ++end;
        // the '%' char is only allowed at start of macro
        if( *end == '%' ) {
            ++end;
        }

        while( *end != '\0' && nesting && (isalnum( *end ) || (*end == '_')
            || (*end == '(') || (*end == ')') || (*end == '$')
            || (*end == '&') || (*end == '[')) ) {
            if( *end == '(' ) {
                ++nesting;
            }
            if( *end == ')' ) {
                --nesting;
            }
            ++end;
        }
    } else if( isalnum( *end ) ) {
        // parse a non-parenthesized macro
        ++end;
        while( isalnum( *end ) || (*end == '_') ) {
            ++end;
        }
    } else {
        bool    quit = false;

        // let's try a special macro
        while( *end != '\0' && !quit ) {
            switch( *end ) {
            case '^':
            case '[':
            case ']':
                ++end;
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
                ++end;
                break;
            default:
                quit = true;
                break;
            }
        }
    }

    flags.inMacro = false;
    ss_new->len = end - start;
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
    /* unused parameters */ (void)line_no;

    flags.inPreproc = false;
    flags.inInlineFile = false;
    flags.inMacro = false;
}

void GetMkBlock( ss_block *ss_new, char *start, int line )
{
    /* unused parameters */ (void)line;

    if( start[0] == '\0' ) {
        if( firstNonWS == start ) {
            // line is empty -
            // do not flag following line as having anything to do
            // with an unterminated macro
            flags.inMacro = false;
        }
        SSGetBeyondText( ss_new );
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
        SSGetWhiteSpace( ss_new, start );
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
