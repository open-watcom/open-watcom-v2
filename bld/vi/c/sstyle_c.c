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
* Description:  Support for C syntax.
*
****************************************************************************/


#include "vi.h"
#include "sstyle.h"
#include "sstyle_c.h"


/*----- LOCALS -----*/

static  ss_flags_c  flags;
static  long        lenCComment = 0;
static  char        *firstNonWS;

#define DIRECTIVE_ERROR     "error"
#define DIRECTIVE_IF        "if"
#define DIRECTIVE_ELIF      "elif"
#define DIRECTIVE_PRAGMA    "pragma"
#define KEYWORD_DECLSPEC    "__declspec"
#define KEYWORD_DEFINED     "defined"

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

static bool isdirective( char *text, char *directive )
{
    int len = strlen( directive );
    return( strncmp( text, directive, len ) == 0 && !isalnum( *(text + len) ) &&
            *(text + len) != '_' );
}

void InitCLine( char *text )
{
    while( *text != '\0' && isspace( *text ) ) {
        text++;
    }
    firstNonWS = text;
}

static void getHex( ss_block *ss_new, char *start )
{
    int     lastc;
    char    *text;
    bool    nodigits = true;

    ss_new->type = SE_HEX;
    for( text = start + 2; *text != '\0' && isxdigit( *text ); ++text ) {
        nodigits = false;
    }
    if( nodigits ) {
        ss_new->type = SE_INVALIDTEXT;
    }
    lastc = tolower ( *text );
    if( lastc == 'u' ) {
        text++;
        if( tolower( *text ) == 'l' ) {
            text++;
        }
        if( tolower( *text ) == 'l' ) {
            text++;
        }
    } else if( lastc == 'l' ) {
        text++;
        if( tolower( *text ) == 'l' ) {
            text++;
        }
        if( tolower( *text ) == 'u' ) {
            text++;
        }
    }
    ss_new->len = text - start;
    flags.inDeclspec = false;
    flags.inDeclspec2 = false;
}

static void getFloat( ss_block *ss_new, char *start, int skip, int command )
{
    char    *text = start + skip;
    char    lastc;

    ss_new->type = SE_FLOAT;

    if( command == AFTER_ZERO ) {
        while( isdigit( *text ) ) {
            text++;
        }
        if( *text == 'E' || *text == 'e' ) {
            command = AFTER_EXP;
            text++;
        } else if( *text == '.' ) {
            command = AFTER_DOT;
            text++;
        } else {
            // simply a bad octal value (eg 09)
            ss_new->type = SE_INVALIDTEXT;
            command = BADTEXT;
        }
    }

    switch( command ) {
    case AFTER_DOT:
        if( !isdigit( *text ) ) {
            if( *text == 'e' || *text == 'E' ) {
                getFloat( ss_new, start, text - start + 1, AFTER_EXP );
                return;
            }
            if( *text == 'f' || *text == 'F' || *text == 'l' || *text == 'L' ) {
                break;
            }
            if( *text && !isspace( *text ) && !issymbol( *text ) ) {
                if( *text ) {
                    text++;
                }
                ss_new->type = SE_INVALIDTEXT;
            }
            break;
        }
        text++;
        while( isdigit( *text ) ) {
            text++;
        }
        if( *text != 'E' && *text != 'e' ) {
            break;
        }
        text++;
        // fall through
    case AFTER_EXP:
        if( *text == '+' || *text == '-' ) {
            text++;
        }
        if( !isdigit( *text ) ) {
            if( *text ) {
                text++;
            }
            ss_new->type = SE_INVALIDTEXT;
            break;
        }
        text++;
        while( isdigit( *text ) ) {
            text++;
        }
    }

    // get float/long spec
    lastc = tolower( *text );
    if( lastc == 'f' || lastc == 'l' ) {
        text++;
    } else if( *text && !isspace( *text ) && !issymbol( *text ) ) {
        ss_new->type = SE_INVALIDTEXT;
        text++;
    }
    ss_new->len = text - start;
    flags.inDeclspec = false;
    flags.inDeclspec2 = false;
}

static void getNumber( ss_block *ss_new, char *start, char top )
{
    int     lastc;
    char    *text = start + 1;

    while( (*text >= '0') && (*text <= top) ) {
        text++;
    }
    if( *text == '.' ) {
        getFloat( ss_new, start, text - start + 1, AFTER_DOT );
        return;
    } else if( *text == 'e' || *text == 'E' ) {
        getFloat( ss_new, start, text - start + 1, AFTER_EXP );
        return;
    } else if( isdigit( *text ) ) {
        // correctly handle something like 09.3
        getFloat( ss_new, start, text - start + 1, AFTER_ZERO );
        return;
    }

    ss_new->len = text - start;
    /* feature!: we display 0 as an integer (it's really an octal)
     *           as it is so common & is usually thought of as such
     */
    ss_new->type = (top == '7' && ss_new->len > 1) ? SE_OCTAL : SE_INTEGER;
    lastc = tolower( *text );
    if( lastc == 'u' ) {
        ss_new->len++;
        if( tolower( text[1] ) == 'l' ) {
            text++;
            ss_new->len++;
        }
        if( tolower( text[1] ) == 'l' ) {
            ss_new->len++;
        }
    } else if( lastc == 'l' ) {
        ss_new->len++;
        if( tolower( text[1] ) == 'l' ) {
            text++;
            ss_new->len++;
        }
        if( tolower( text[1] ) == 'u' ) {
            ss_new->len++;
        }
    }
    flags.inDeclspec = false;
    flags.inDeclspec2 = false;
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
    char    save_char;
    bool    isKeyword;
    bool    isPragma;
    bool    isDeclspec;

    while( isalnum( *text ) || ( *text == '_' ) ) {
        text++;
    }
    save_char = *text;
    *text = '\0';
    isKeyword = IsKeyword( start, false );
    isPragma = flags.inPragmaDir && IsPragma( start );
    isDeclspec = flags.inDeclspec2 && IsDeclspec( start );
    *text = save_char;

    ss_new->type = SE_IDENTIFIER;
    flags.inDeclspec = flags.inDeclspec2 = false;
    if( isPragma ) {
        ss_new->type = SE_PREPROCESSOR;
    } else if( isKeyword ) {
        ss_new->type = SE_KEYWORD;
        if( isdirective( start, KEYWORD_DECLSPEC ) ) {
            flags.inDeclspec = true;
        }
    } else if( isDeclspec ) {
        ss_new->type = SE_KEYWORD;
    } else if( flags.inIfDir && isdirective( start, KEYWORD_DEFINED ) ) {
        ss_new->type = SE_PREPROCESSOR;
    } else if( text[0] == ':' && firstNonWS == start && text[1] != ':' && text[1] != '>' ) {
        // : and > checked as it may be :: (CPP) operator or :> (base op.)
        text++;
        ss_new->type = SE_JUMPLABEL;
    }
    ss_new->len = text - start;
}

static void getSymbol( ss_block *ss_new, char *start )
{
    flags.inDeclspec2 = flags.inDeclspec && *start == '(';
    flags.inDeclspec = false;
    ss_new->type = SE_SYMBOL;
    ss_new->len = 1;
}

static void getPreprocessor( ss_block *ss_new, char *start )
{
    char    *text = start;
    bool    withinQuotes = flags.inString;

    ss_new->type = SE_PREPROCESSOR;

    if( EditFlags.PPKeywordOnly ) {
        char *directive;

        // just grab the #xxx bit & go

        // skip the #
        text++;
        // take any spaces present
        for( ; *text != '\0'; ++text ) {
            if( !isspace( *text ) ) {
                break;
            }
        }
        // and then the keyword
        directive = text;
        for( ; *text != '\0'; ++text ) {
            if( isspace( *text ) || issymbol( *text ) ) {
                break;
            }
        }
        if( isdirective( directive, DIRECTIVE_ERROR ) ) {
            flags.inErrorDir = true;
        } else if( isdirective( directive, DIRECTIVE_IF ) ) {
            flags.inIfDir = true;
        } else if( isdirective( directive, DIRECTIVE_ELIF ) ) {
            flags.inIfDir = true;
        } else if( isdirective( directive, DIRECTIVE_PRAGMA ) ) {
            flags.inPragmaDir = true;
        }
        ss_new->len = text - start;
        flags.inPreprocessor = false;
        return;
    }

    flags.inPreprocessor = true;
    for( ; *text != '\0'; ++text ) {
        if( text[0] == '"' ) {
            if( !withinQuotes ) {
                withinQuotes = true;
            } else if( text[-1] != '\\' || text[-2] == '\\' ) {
                withinQuotes = false;
            }
            continue;
        }
        if( text[0] == '/' ) {
            if( text[1] == '*' && !withinQuotes ) {
                flags.inCComment = true;
                lenCComment = 0;
                break;
            } else if( text[1] == '/' && !withinQuotes ) {
                flags.inCPPComment = true;
                flags.inPreprocessor = false;
                break;
            }
        }
    }
    flags.inString = withinQuotes;

    if( text[0] == '\0' ) {
        if( text[-1] != '\\' ) {
            flags.inPreprocessor = false;
            if( flags.inString ) {
                ss_new->type = SE_INVALIDTEXT;
                flags.inString = false;
            }
        }
    }
    ss_new->len = text - start;
}

static void getChar( ss_block *ss_new, char *start, int skip )
{
    char    *text;

    ss_new->type = SE_CHAR;
    for( text = start + skip; *text != '\0'; ++text ) {
        if( *text == '\'' ) {
            break;
        }
        if( text[0] == '\\' && ( text[1] == '\\' || text[1] == '\'' ) ) {
            ++text;
        }
    }
    if( *text == '\0' ) {
        ss_new->type = SE_INVALIDTEXT;
    } else {
        text++;
    }
    ss_new->len = text - start;
    if( ss_new->len == 2 ) {
#if 0
        /* multibyte character constants are legal in the C standard */
        (ss_new->len == 2) || ((ss_new->len > 3) && (start[skip] != '\\'))
        // 2 or more length char constants not allowed if first char not '\'
#endif
        // 0 length char constants not allowed
        ss_new->type = SE_INVALIDTEXT;
    }
    flags.inDeclspec = false;
    flags.inDeclspec2 = false;
}

static void getBeyondText( ss_block *ss_new )
{
    ss_new->type = SE_WHITESPACE;
    ss_new->len = BEYOND_TEXT;
}

static void getInvalidChar( ss_block *ss_new )
{
    ss_new->type = SE_INVALIDTEXT;
    ss_new->len = 1;
    flags.inDeclspec = false;
    flags.inDeclspec2 = false;
}

static void getCComment( ss_block *ss_new, char *start, int skip )
{
    char    *text;

    lenCComment += skip;
    flags.inCComment = true;
    for( text = start + skip; *text != '\0'; ++text ) {
        if( text[0] == '*' && text[1] == '/' && lenCComment > 1  ) {
            text += 2;
            lenCComment += 2;
            flags.inCComment = false;
            break;
        }
        lenCComment++;
    }
    ss_new->type = SE_COMMENT;
    ss_new->len = text - start;
}

static void getCPPComment( ss_block *ss_new, char *start )
{
    char    *text;

    text = start;
    while( *text != '\0' ) {
        text++;
    }
    flags.inCPPComment = true;
    if( *start == '\0' || text[-1] != '\\' ) {
        flags.inCPPComment = false;
    }
    ss_new->type = SE_COMMENT;
    ss_new->len = text - start;
}

static void getString( ss_block *ss_new, char *start, int skip )
{
    char    *text;

    ss_new->type = SE_STRING;
    for( text = start + skip; *text != '\0'; ++text ) {
        if( text[0] == '"' ) {
            break;
        }
        if( text[0] == '\\' && ( text[1] == '\\' || text[1] == '"' ) ) {
            ++text;
        }
    }
    if( text[0] == '\0' ) {
        if( text[-1] != '\\' ) {
            // unterminated string
            ss_new->type = SE_INVALIDTEXT;

            // don't try to trash rest of file
            flags.inString = false;
        } else {
            // string continued on next line
            flags.inString = true;
        }
    } else {
        text++;
        // definitely finished string
        flags.inString = false;
    }
    ss_new->len = text - start;
    flags.inDeclspec = false;
    flags.inDeclspec2 = false;
}

static void getErrorMsg( ss_block *ss_new, char *start )
{
    char    *text = start;

    ss_new->type = SE_IDENTIFIER;
    while( *text != '\0' ) {
        text++;
    }
    ss_new->len = text - start;
    flags.inErrorDir = false;
}

void InitCFlagsGivenValues( ss_flags_c *newFlags )
{
    flags = *newFlags;
    lenCComment = 2; // this is a new line, if we are in a comment then
                     // we should be able to end it
}

void GetCFlags( ss_flags_c *storeFlags )
{
    *storeFlags = flags;
}

void InitCFlags( linenum line_no )
{
    fcb     *fcb;
    char    *text;
    char    *starttext;
    line    *thisline;
    line    *topline;
    char    topChar;
    vi_rc   rc;
    bool    withinQuotes = false;
    line    *line;
    bool    inBlock = false;

    flags.inCComment = false;
    flags.inCPPComment = false;
    flags.inString = false;
    flags.inPreprocessor = false;
    flags.inErrorDir = false;
    flags.inIfDir = false;
    flags.inPragmaDir = false;
    flags.inDeclspec = false;
    flags.inDeclspec2 = false;

    CGimmeLinePtr( line_no, &fcb, &thisline );
    line = thisline;
    while( (rc = GimmePrevLinePtr( &fcb, &line )) == ERR_NO_ERR ) {
        if( line->data[line->len - 1] != '\\' ) {
            break;
        }
        inBlock = true;
    }

    if( rc == ERR_NO_ERR ) {
        topline = line;
        if( inBlock ) {
            CGimmeNextLinePtr( &fcb, &line );
        }
    } else {
        topline = NULL;
        if( inBlock ) {
            CGimmeLinePtr( 1, &fcb, &line );
        } else {
            return;
        }
    }

    if( inBlock ) {
        // jot down whether it started with #
        for( text = line->data; *text != '\0'; ++text ) {
            if( !isspace( *text ) ) {
                break;
            }
        }
        topChar = *text;

        // parse down through lines, noticing /*, */ and "
        while( line != thisline ) {
            for( text = line->data; ; ++text ) {
                for( ; *text != '\0'; ++text ) {
                    if( text[0] == '/' )
                        break;
                    if( text[0] == '"' ) {
                        if( !withinQuotes ) {
                            withinQuotes = true;
                        } else if( text[-1] != '\\' || text[-2] == '\\' ) {
                            withinQuotes = false;
                        }
                    }
                }
                if( *text == '\0' ) {
                    break;
                }
                if( !withinQuotes ) {
                    if( text[-1] == '/' ) {
                        flags.inCPPComment = true;
                    } else if( text[1] == '*' ) {
                        flags.inCComment = true;
                        lenCComment = 100;
                    }
                }
                if( text[-1] == '*' && !withinQuotes ) {
                    flags.inCComment = false;
                }
            }
            rc = CGimmeNextLinePtr( &fcb, &line );
        }

        // if not in a comment (and none above), we may be string or pp
        if( !flags.inCComment ) {
            if( topChar == '#' && !EditFlags.PPKeywordOnly ) {
                flags.inPreprocessor = true;
            }
            if( withinQuotes ) {
                flags.inString = true;
            }
        }
    }

    if( topline == NULL ) {
        return;
    }

    if( !flags.inCComment ) {
        // keep going above multi-line thing till hit /* or */
        line = topline;
        do {
            starttext = line->data;
            for( text = starttext + line->len; ; --text ) {
                while( text != starttext && *text != '/' ) {
                    text--;
                }
                if( text[1] == '*' && text[0] == '/' && text[-1] != '/' ) {
                    if( text == starttext ) {
                        flags.inCComment = true;
                        lenCComment = 100;
                        return;
                    }
                    withinQuotes = false;
                    do {
                        text--;
                        if( text[0] == '"' ) {
                            if( !withinQuotes ) {
                                withinQuotes = true;
                            } else if( text[-1] != '\\' || text[-2] == '\\' ) {
                                withinQuotes = false;
                            }
                        } else if( text[0] == '/' && text[-1] == '/' && !withinQuotes ) {
                            flags.inCPPComment = true;
                        }
                    } while( text != starttext );
                    if( withinQuotes ) {
                        flags.inString = true;
                    } else if( !flags.inCPPComment ) {
                        flags.inCComment = true;
                        lenCComment = 100;
                    } else {
                        flags.inCPPComment = false;
                    }
                    return;
                }
                if( text == starttext ) {
                    break;
                }
                if( text[-1] == '*' ) {
                    // we may actually be in a string, but that's extreme
                    // (if this becomes a problem, count the "s to beginning
                    // of line, check if multiline, etc. etc.)
                    return;
                }
            }
            rc = GimmePrevLinePtr( &fcb, &line );
        } while( rc == ERR_NO_ERR );
    }
}

void GetCBlock( ss_block *ss_new, char *start, line *line, linenum line_no )
{
    line = line;
    line_no = line_no;

    if( start[0] == '\0' ) {
        flags.inIfDir = false;
        flags.inPragmaDir = false;
        if( firstNonWS == start ) {
            // line is empty -
            // do not flag following line as having anything to do
            // with an unterminated " or # or // from previous line
            flags.inString = false;
            flags.inPreprocessor = false;
            flags.inCPPComment = false;
        }
        getBeyondText( ss_new );
        return;
    }

    if( flags.inCComment ) {
        getCComment( ss_new, start, 0 );
        return;
    }
    if( flags.inCPPComment ) {
        getCPPComment( ss_new, start );
        return;
    }
    if( flags.inPreprocessor ) {
        getPreprocessor( ss_new, start );
        return;
    }
    if( flags.inString ) {
        getString( ss_new, start, 0 );
        return;
    }
    if( flags.inErrorDir ) {
        getErrorMsg( ss_new, start );
        return;
    }

    if( isspace( start[0] ) ) {
        getWhiteSpace( ss_new, start );
        return;
    }

    if( *firstNonWS == '#' &&
        (!EditFlags.PPKeywordOnly || firstNonWS == start) ) {
        getPreprocessor( ss_new, start );
        return;
    }

    switch( start[0] ) {
    case '"':
        getString( ss_new, start, 1 );
        return;
    case '/':
        if( start[1] == '*' ) {
            getCComment( ss_new, start, 2 );
            return;
        } else if( start[1] == '/' ) {
            getCPPComment( ss_new, start );
            return;
        }
        break;
    case '\'':
        getChar( ss_new, start, 1 );
        return;
    case 'L':
        if( start[1] == '\'' ) {
            // wide char constant
            getChar( ss_new, start, 2 );
            return;
        }
        break;
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
