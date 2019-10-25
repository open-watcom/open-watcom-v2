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
* Description:  Support for Perl/AWK syntax.
*
****************************************************************************/


#include "vi.h"
#include "sstyle.h"
#include "sstyle_p.h"


/*----- LOCALS -----*/

static  ss_flags_p  flags;
static  char        *firstNonWS;


enum getFloatCommands {
    AFTER_ZERO,
    AFTER_DOT,
    AFTER_EXP,
    BADTEXT,
};

static int issymbol( int c )
{
    /* symbol list taken from Watcom C Lang Ref. pg 11
    */
    if( c == ';' ||
        c == '(' || c == ')' ||
        c == '{' || c == '}' ||
        c == '=' ||
        c == '+' || c == '-' ||
        c == '/' || c == '*' || c == '%' || c == '^' ||
        c == '|' || c == '&' || c == '!' ||
        c == '[' || c == ']' ||
        c == '<' || c == '>' ||
        c == '.' || c == '\\' ||
        c == '?' || c == ':' ||
        c == ',' ||
        c == '~' ) {
        return( 1 );
    } else {
        return( 0 );
    }
}

static int isspecvar( int c )
{
    switch( c ) {
    case '|':
    case '%':
    case '=':
    case '-':
    case '~':
    case '^':
    case '&':
    case '`':
    case '\'':
    case '+':
    case '_':
    case '/':
    case '\\':
    case ',':
    case '\"':
    case '$':
    case '?':
    case '*':
    case '[':
    case ']':
    case ';':
    case '!':
    case '@':
        return( 1 );
    default:
        return( 0 );
    }
}

void InitPerlLine( char *text )
{
    SKIP_SPACES( text );
    firstNonWS = text;
}

static void getHex( ss_block *ss_new, char *start )
{
    int     lastc;
    char    *end = start + 2;
    bool    nodigits = true;

    flags.beforeRegExp = false;
    ss_new->type = SE_HEX;
    for( end = start + 2; isxdigit( *end ); ++end ) {
        nodigits = false;
    }
    if( nodigits ) {
        ss_new->type = SE_INVALIDTEXT;
    }
    lastc = tolower( *end );
    if( lastc == 'u' ) {
        end++;
        if( tolower( *end ) == 'l' ) {
            end++;
        }
        if( tolower( *end ) == 'l' ) {
            end++;
        }
    } else if( lastc == 'l' ) {
        end++;
        if( tolower( *end ) == 'l' ) {
            end++;
        }
        if( tolower( *end ) == 'u' ) {
            end++;
        }
    }
    ss_new->len = end - start;
}

static void getFloat( ss_block *ss_new, char *start, int skip, int command )
{
    char    *end = start + skip;
    char    lastc;

    ss_new->type = SE_FLOAT;
    flags.beforeRegExp = false;
    if( command == AFTER_ZERO ) {
        SKIP_DIGITS( end );
        if( *end == 'E' || *end == 'e' ) {
            command = AFTER_EXP;
            end++;
        } else if( *end == '.' ) {
            command = AFTER_DOT;
            end++;
        } else {
            // simply a bad octal value (eg 09)
            ss_new->type = SE_INVALIDTEXT;
            command = BADTEXT;
        }
    }

    switch( command ) {
        case AFTER_DOT:
            if( !isdigit( *end ) ) {
                if( *end == 'e' || *end == 'E' ) {
                    getFloat( ss_new, start, end - start + 1, AFTER_EXP );
                    return;
                }
                if( *end == 'f' || *end == 'F' || *end == 'l' || *end == 'L' ) {
                    break;
                }
                if( *end != '\0' && !isspace( *end ) && !issymbol( *end ) ) {
                    if( *end != '\0' ) {
                        end++;
                    }
                    ss_new->type = SE_INVALIDTEXT;
                }
                break;
            }
            end++;
            SKIP_DIGITS( end );
            if( *end != 'E' && *end != 'e' ) {
                break;
            }
            end++;
            // fall through
        case AFTER_EXP:
            if( *end == '+' || *end == '-' ) {
                end++;
            }
            if( !isdigit( *end ) ) {
                if( *end != '\0' ) {
                    end++;
                }
                ss_new->type = SE_INVALIDTEXT;
                break;
            }
            end++;
            SKIP_DIGITS( end );
    }

    // get float/long spec
    lastc = tolower( *end );
    if( lastc == 'f' || lastc == 'l' ) {
        end++;
    } else if( *end != '\0' && !isspace( *end ) && !issymbol( *end ) ) {
        ss_new->type = SE_INVALIDTEXT;
        end++;
    }
    ss_new->len = end - start;
}

static void getNumber( ss_block *ss_new, char *start, char top )
{
    int     lastc;
    char    *end = start + 1;

    flags.beforeRegExp = false;
    while( (*end >= '0') && (*end <= top) ) {
        end++;
    }
    if( *end == '.' ) {
        getFloat( ss_new, start, end - start + 1, AFTER_DOT );
        return;
    } else if( *end == 'e' || *end == 'E' ) {
        getFloat( ss_new, start, end - start + 1, AFTER_EXP );
        return;
    } else if( isdigit( *end ) ) {
        // correctly handle something like 09.3
        getFloat( ss_new, start, end - start + 1, AFTER_ZERO );
        return;
    }

    ss_new->len = end - start;
    /* feature!: we display 0 as an integer (it's really an octal)
     *           as it is so common & is usually thought of as such
     */
    ss_new->type = (top == '7' && ss_new->len > 1) ? SE_OCTAL : SE_INTEGER;
    lastc = tolower( *end );
    if( lastc == 'u' ) {
        ss_new->len++;
        if( tolower( end[1] ) == 'l' ) {
            end++;
            ss_new->len++;
        }
        if( tolower( end[1] ) == 'l' ) {
            ss_new->len++;
        }
    } else if( lastc == 'l' ) {
        ss_new->len++;
        if( tolower( end[1] ) == 'l' ) {
            end++;
            ss_new->len++;
        }
        if( tolower( end[1] ) == 'u' ) {
            ss_new->len++;
        }
    }
}

static void getText( ss_block *ss_new, char *start )
{
    char    *end = start + 1;
    bool    isKeyword;

    while( isalnum( *end ) || (*end == '_') ) {
        end++;
    }
    isKeyword = IsKeyword( start, end, false );

    // Expect a double regular expression after s, tr, and y.
    if( end - start == 1 && (*start == 's' || *start == 'y') ) {
        flags.doubleRegExp = true;
    } else if( end - start == 2 && *start == 't' && *(start + 1) == 'r' ) {
        flags.doubleRegExp = true;
    } else {
        flags.doubleRegExp = false;
    }
    flags.beforeRegExp = isKeyword;
    ss_new->type = SE_IDENTIFIER;
    if( isKeyword ) {
        ss_new->type = SE_KEYWORD;
    } else if( end[0] == ':' && firstNonWS == start && end[1] != ':' && end[1] != '>' ) {
        // : and > checked as it may be :: (CPP) operator or :> (base op.)
        end++;
        ss_new->type = SE_JUMPLABEL;
    }
    ss_new->len = end - start;
}

static void getVariable( ss_block *ss_new, char *start )
{
    char    *end = start + 1;

    if( *end == '#' ) {
        end++;
    }
    while( isalnum( *end ) || (*end == '_') ) {
        end++;
        if( end[0] == ':' && end[1] == ':' ) {
            // Allow scope resolution operator in variable names.
            end += 2;
        }
    }
    flags.beforeRegExp = false;
    ss_new->type = SE_VARIABLE;
    ss_new->len = end - start;
}

static void getSpecialVariable( ss_block *ss_new, char *start )
{
    char    *end = start + 1;
    if( isdigit( *end ) ) {
        end++;
        SKIP_DIGITS( end );
    } else {
        end++;
    }
    flags.beforeRegExp = false;
    ss_new->type = SE_VARIABLE;
    ss_new->len = end - start;
}

static void getSymbol( ss_block *ss_new, char *start )
{
    /* unused parameters */ (void)start;

    flags.beforeRegExp = true;
    flags.doubleRegExp = false;
    ss_new->type = SE_SYMBOL;
    ss_new->len = 1;
}

static void getChar( ss_block *ss_new, char *start, int skip )
{
    char    *end;

    flags.beforeRegExp = false;
    ss_new->type = SE_CHAR;
    for( end = start + skip; *end != '\0'; ++end ) {
        if( end[0] == '\'' ) {
            break;
        }
        if( end[0] == '\\' && ( end[1] == '\\' || end[1] == '\'' ) ) {
            ++end;
        }
    }
    if( *end == '\0' ) {
        ss_new->type = SE_INVALIDTEXT;
    } else {
        ++end;
    }
    ss_new->len = end - start;
    if(ss_new->len == 2) {
#if 0
        /* multibyte character constants are legal in the C standard */
        (ss_new->len == 2) || ((ss_new->len > 3) && (start[skip] != '\\'))
        // 2 or more length char constants not allowed if first char not '\'
#endif
        // 0 length char constants not allowed
        ss_new->type = SE_INVALIDTEXT;
    }
}

static void getInvalidChar( ss_block *ss_new )
{
    flags.beforeRegExp = true;
    flags.doubleRegExp = false;
    ss_new->type = SE_INVALIDTEXT;
    ss_new->len = 1;
}

static void getPerlComment( ss_block *ss_new, char *start )
{
    char    *end = start;

    SKIP_TOEND( end );
    flags.beforeRegExp = true;
    flags.doubleRegExp = false;
    ss_new->type = SE_COMMENT;
    ss_new->len = end - start;
}

static void getString( ss_block *ss_new, char *start, int skip )
{
    char    *end;

    flags.beforeRegExp = false;
    ss_new->type = SE_STRING;
    for( end = start + skip; *end != '\0'; ++end ) {
        if( end[0] == '"' ) {
            break;
        }
        if( end[0] == '\\' && (end[1] == '\\' || end[1] == '"') ) {
            ++end;
        }
    }
    if( end[0] == '\0' ) {
        if( end[-1] != '\\' ) {
            // unterminated string
            ss_new->type = SE_INVALIDTEXT;

            // don't try to trash rest of file
            flags.inString = false;
        } else {
            // string continued on next line
            flags.inString = true;
        }
    } else {
        end++;
        // definitely finished string
        flags.inString = false;
    }
    ss_new->len = end - start;
}

static void getRegExp( ss_block *ss_new, char *start )
{
    char    *end;

    ss_new->type = SE_REGEXP;
    for( ;; ) {
        for( end = start + 1; *end != '\0'; ++end ) {
            if( end[0] != '/' ) {
                break;
            }
            if( end[0] == '\\' && (end[1] == '\\' || end[1] == '/') ) {
                ++end;
            }
        }
        if( end[0] == '\0' )
            break;
        ++end;
        if( !flags.doubleRegExp )
            break;
        flags.doubleRegExp = false;
    }
    SKIP_SYMBOL( end );
    ss_new->len = end - start;
}

void InitPerlFlagsGivenValues( ss_flags_p *newFlags )
{
    flags = *newFlags;
}

void GetPerlFlags( ss_flags_p *storeFlags )
{
    *storeFlags = flags;
}

void InitPerlFlags( linenum line_no )
{
    fcb     *fcb;
    char    *text;
    line    *thisline;
//    line    *topline;
    vi_rc   rc;
    bool    withinQuotes = false;
    line    *line;
    bool    inBlock = false;

    flags.inString = false;
    flags.beforeRegExp = true;
    flags.doubleRegExp = false;

    CGimmeLinePtr( line_no, &fcb, &thisline );
    line = thisline;
    while( (rc = GimmePrevLinePtr( &fcb, &line )) == ERR_NO_ERR ) {
        if( line->data[line->len - 1] != '\\' ) {
            break;
        }
        inBlock = true;
    }

    if( rc == ERR_NO_ERR ) {
//        topline = line;
        if( inBlock ) {
            CGimmeNextLinePtr( &fcb, &line );
        }
    } else {
//        topline = NULL;
        if( inBlock ) {
            CGimmeLinePtr( 1, &fcb, &line );
        } else {
            return;
        }
    }

    if( inBlock ) {
        // parse down through lines, noticing "
        while( line != thisline ) {
            for( text = line->data; *text != '\0'; ++text ) {
                if( text[0] == '"' ) {
                    if( !withinQuotes ) {
                        withinQuotes = true;
                    } else {
                        withinQuotes = false;
                    }
                } else if( text[0] == '\\' && ( text[1] == '\\' || text[1] == '"' ) ) {
                    ++text;
                }
            }
            rc = CGimmeNextLinePtr( &fcb, &line );
        }

        if( withinQuotes ) {
            flags.inString = true;
        }
    }
}

void GetPerlBlock( ss_block *ss_new, char *start, line *line, linenum line_no )
{
    /* unused parameters */ (void)line; (void)line_no;

    if( start[0] == '\0' ) {
        if( firstNonWS == start ) {
            // line is empty -
            // do not flag following line as having anything to do
            // with an unterminated " or # or // from previous line
            flags.inString = false;
        }
        SSGetBeyondText( ss_new );
        return;
    }

    if( flags.inString ) {
        getString( ss_new, start, 0 );
        return;
    }

    if( isspace( start[0] ) ) {
        SSGetWhiteSpace( ss_new, start );
        return;
    }

    switch( start[0] ) {
        case '#':
            getPerlComment( ss_new, start );
            return;
        case '"':
            getString( ss_new, start, 1 );
            return;
        case '/':
            if( flags.beforeRegExp ) {
                getRegExp( ss_new, start );
                return;
            }
            break;
        case '$':
        case '@':
        case '%':
            if( isalpha( start[1] ) || (start[0] == '$' && start[1] == '#') ) {
                getVariable( ss_new, start );
                return;
            } else if( start[0] == '$' &&
                       (isdigit( start[1] ) || isspecvar( start[1] )) ) {
                getSpecialVariable( ss_new, start );
                return;
            }
            break;
        case '\'':
            getChar( ss_new, start, 1 );
            return;
        case '.':
            if( isdigit( start[1] ) ) {
                getFloat( ss_new, start, 1, AFTER_DOT );
                return;
            }
            break;
        case '0':
            if( start[1] == 'x' || start[1] == 'X' ) {
                getHex( ss_new, start );
                return;
            } else {
                getNumber( ss_new, start, '7' );
                return;
            }
            break;
    }

    if( issymbol( start[0] ) ) {
        getSymbol( ss_new, start );
        return;
    }

    if( isdigit( start[0] ) ) {
        getNumber( ss_new, start, '9' );
        return;
    }

    if( isalpha( *start ) || (*start == '_') ) {
        getText( ss_new, start );
        return;
    }

    getInvalidChar( ss_new );
}
