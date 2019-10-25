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
* Description:  Support for REX syntax.
*
****************************************************************************/


#include "vi.h"
#include "sstyle.h"


/*----- LOCALS -----*/

static  ss_flags_c  flags;
static  int         lenCComment = 0;
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

void InitRexxLine( char *text )
{
    SKIP_SPACES( text );
    firstNonWS = text;
}

static void getHex( ss_block *ss_new, char *start )
{
    int     lastc;
    char    *end;
    bool    nodigits = true;

    ss_new->type = SE_HEX;
    for( end = start + 2; isxdigit( *end ); end++ ) {
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
    } else if( lastc == 'l' ) {
        end++;
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

    while( *end >= '0' && *end <= top ) {
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
            ss_new->len++;
        }
    } else if( lastc == 'l' ) {
        ss_new->len++;
        if( tolower( end[1] ) == 'u' ) {
            ss_new->len++;
        }
    }
}

static void getText( ss_block *ss_new, char *start )
{
    char    *end = start + 1;
    char    save_char;

    while( isalnum( *end ) || *end == '_' || *end == '.' ) {
        end++;
    }
    ss_new->type = SE_IDENTIFIER;
    if( IsKeyword( start, end, false ) ) {
        ss_new->type = SE_KEYWORD;
    } else if( end[0] == ':' && firstNonWS == start && end[1] != ':' && end[1] != '>' ) {
        // : and > checked as it may be :: (CPP) operator or :> (base op.)
        end++;
        ss_new->type = SE_JUMPLABEL;
    }
    ss_new->len = end - start;
}

static void getSymbol( ss_block *ss_new )
{
    ss_new->type = SE_SYMBOL;
    ss_new->len = 1;
}

static void getPreprocessor( ss_block *ss_new, char *start )
{
    char    *end = start;
    bool    withinQuotes = flags.inString;

    ss_new->type = SE_PREPROCESSOR;

    if( EditFlags.PPKeywordOnly ) {
        // just grab the #xxx bit & go

        // skip the #
        end++;
        // take any spaces present
        SKIP_SPACES( end );
        // and then the keyword
        SKIP_TOSPACE( end );
        ss_new->len = end - start;
        flags.inPreprocessor = false;
        return;
    }

    flags.inPreprocessor = true;
    for( ; *end != '\0'; ++end ) {
        if( end[0] == '"' ) {
            if( !withinQuotes ) {
                withinQuotes = true;
            } else if( end[-1] != '\\' || end[-2] == '\\' ) {
                withinQuotes = false;
            }
        }
        if( end[0] == '/' ) {
            if( end[1] == '*' && !withinQuotes ) {
                flags.inCComment = true;
                lenCComment = 0;
                break;
            } else if( end[1] == '/' && !withinQuotes ) {
                flags.inCPPComment = true;
                flags.inPreprocessor = false;
                break;
            }
        }
    }
    flags.inString = withinQuotes;

    if( end[0] == '\0' ) {
        if( end[-1] != '\\' ) {
            flags.inPreprocessor = false;
            if( flags.inString ) {
                ss_new->type = SE_INVALIDTEXT;
                flags.inString = false;
            }
        }
    }
    ss_new->len = end - start;
}

static void getChar( ss_block *ss_new, char *start, int skip )
{
    char    *end;

    ss_new->type = SE_CHAR;
    for( end = start + skip; *end != '\0'; ++end ) {
        if( end[0] == '\'' )
            break;
        if( end[0] == '\\' && ( end[1] == '\\' || end[1] == '\'' ) ) {
            ++end;
        }
    }
    if( end[0] == '\0' ) {
        ss_new->type = SE_INVALIDTEXT;
    } else {
        end++;
    }
    ss_new->len = end - start;
}

static void getInvalidChar( ss_block *ss_new )
{
    ss_new->type = SE_INVALIDTEXT;
    ss_new->len = 1;
}

static void getCComment( ss_block *ss_new, char *start, int skip )
{
    char    *end = start + skip;

    lenCComment += skip;
    for( ;; ) {
        while( *end != '\0' && *end != '*' ) {
            lenCComment++;
            end++;
        }
        if( end[0] == '\0' ) {
            // comment extends to next line
            flags.inCComment = true;
            break;
        }
        end++;
        lenCComment++;
        if( end[0] == '/' && lenCComment > 2 ) {
            end++;
            // comment definitely done
            flags.inCComment = false;
            break;
        }
    }
    ss_new->type = SE_COMMENT;
    ss_new->len = end - start;
}

static void getCPPComment( ss_block *ss_new, char *start )
{
    char    *end = start;

    SKIP_TOEND( end );
    flags.inCPPComment = true;
    if( *start == '\0' || end[-1] != '\\' ) {
        flags.inCPPComment = false;
    }
    ss_new->type = SE_COMMENT;
    ss_new->len = end - start;
}

static void getString( ss_block *ss_new, char *start, int skip )
{
    char    *nstart = start + skip;
    char    *end = nstart;

    ss_new->type = SE_STRING;
again:
    while( *end != '\0' && *end != '"' ) {
        end++;
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

void InitRexxFlagsGivenValues( ss_flags_c *newFlags )
{
    // Arrrrrrrrrggggg ! you have got to be kidding !!!!!!!!!!

    flags = *newFlags;
}

void GetRexxFlags( ss_flags_c *storeFlags )
{
    *storeFlags = flags;
}

void InitRexxFlags( linenum line_no )
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
        text = line->data;
        SKIP_SPACES( text );
        topChar = *text;

        // parse down through lines, noticing /*, */ and "
        while( line != thisline ) {
            for( text = line->data; ; ++text ) {
                for( ; *text != '\0' && *text != '/'; ++text ) {
                    if( text[0] == '"' ) {
                        if( !withinQuotes ) {
                            withinQuotes = true;
                        } else if( text[-1] != '\\' || text[-2] == '\\' ) {
                            withinQuotes = false;
                        }
                    }
                }
                if( text[0] == '\0' ) {
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
                        }
                    } while( text != starttext );
                    if( withinQuotes ) {
                        flags.inString = true;
                    } else {
                        flags.inCComment = true;
                        lenCComment = 100;
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

void GetRexxBlock( ss_block *ss_new, char *start, line *line, linenum line_no )
{
    line = line;
    line_no = line_no;

    if( start[0] == '\0' ) {
        if( firstNonWS == start ) {
            // line is empty -
            // do not flag following line as having anything to do
            // with an unterminated " or # or // from previous line
            flags.inString = flags.inPreprocessor = flags.inCPPComment = false;
        }
        SSGetBeyondText( ss_new );
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

    if( isspace( start[0] ) ) {
        SSGetWhiteSpace( ss_new, start );
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
        getSymbol( ss_new );
        return;
    }

    if( isdigit( start[0] ) ) {
        getNumber( ss_new, start, '9' );
        return;
    }

    if( isalpha( *start ) || ( *start == '_' ) ) {
        getText( ss_new, start );
        return;
    }

    getInvalidChar( ss_new );
}
