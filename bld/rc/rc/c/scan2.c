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
* Description:  Lexical scanner, OS/2 RC version.
*
****************************************************************************/


#include <ctype.h>
#include <string.h>
#include "global.h"
#include "yydrivr2.h"
#include "rctypes.h"
#include "errors.h"
#include "depend.h"
#include "errprt.h"
#include "rcrtns.h"

#ifdef SCANDEBUG

#define DEBUGPUTS(s)    PutScanString(s);

static void PutScanString( const char *string )
{
    if( CmdLineParms.DebugScanner && string != NULL ) {
        RcMsgFprintf( stdout, NULL, "%s\n", string );
    }
} /* PutScanString */

#else
    #define DEBUGPUTS(s)
#endif

/*** Macros to implement the parts of a finite state machine ***/
/* change_state changes states without reading a character */

static int      _next;
static int      LookAhead;
static int      longString;
static int      newLineInString = 0;

static YTOKEN   scanDFA( ScanValue *value );

static void GetNextChar( void )
{
    for( ;; ) {
        LookAhead = _next;
        if( LookAhead != RC_EOF ) {
            _next = RcIoGetChar();
            if( LookAhead == '\\' && _next == '\n' ) {
                _next = RcIoGetChar();
                continue;
            }
        }
        break;
    }
} /* GetNextChar */

static void CharInit( void )
{
    _next = RcIoGetChar();
    GetNextChar();
} /* CharInit */


#define state(s)            s
#define do_transition(s)    GetNextChar(); goto s
#define change_state(s)     goto s
#define enter_start_state() CharInit()

static void     AddDigitToInt( long * value, int base, int newchar )
{
    int     newdigit;

    if( isdigit( newchar ) ) {
        newdigit = newchar - '0';
    } else {            /* assume it is a hex digit */
        newdigit = toupper( newchar ) - 'A' + 10;
    }

    *value = *value * base + newdigit;
} /* AddDigitToInt */

static YTOKEN   scanCPPDirective( ScanValue *value )
/**************************************************/
/* This function takes the correct action for the #line directive and returns */
/* the token following the preprocessor stuff. It uses Scan to do it's */
/* scanning. DON'T call this function from within Scan or the functions it */
/* calls unless you are very careful about recurtion. */
{
    YTOKEN  token;
    int     linenum;

    if( StopInvoked ) {
        RcFatalError( ERR_STOP_REQUESTED );
    }
    /* get the "line" or "pragma" directive */
    token = scanDFA( value );
    if( token != Y_NAME ) {
        RcFatalError( ERR_INVALID_CPP );
    }

    if( stricmp( value->string.string, "line" ) == 0 ) {
        RCFREE( value->string.string );

        /* get the line number */
        token = scanDFA( value );
        if( token != Y_INTEGER ) {
            RcFatalError( ERR_INVALID_CPP_LINE );
        }
        RCFREE( value->intinfo.str );
        value->intinfo.str = NULL;
        linenum = value->intinfo.val;

        /* get the filename if there is one */
        token = scanDFA( value );
        if( token == Y_STRING ) {
            RcIoSetLogicalFileInfo( linenum, value->string.string );
            if( AddDependency( value->string.string ) ) {
                ErrorHasOccured = TRUE;
            }
            RCFREE( value->string.string );
            token = scanDFA( value );
        } else {
            RcIoSetLogicalFileInfo( linenum, NULL );
        }
    } else if( stricmp( value->string.string, "pragma" ) == 0 ) {
        RCFREE( value->string.string );
        token = Y_POUND_PRAGMA;
    } else if( stricmp( value->string.string, "error" ) == 0 ) {
        char            buf[80];
        unsigned        i;

        i = 0;
        while( LookAhead != '\n' && LookAhead != RC_EOF ) {
            buf[i] = LookAhead;
            i ++;
            GetNextChar();
        }
        buf[i] = '\0';
        RcFatalError( ERR_TEXT_FROM_CPP, buf );
    } else {
        RcFatalError( ERR_INVALID_CPP );
    }

    return( token );
} /* scanCPPDirective */

void    ScanInitOS2( void )
/*************************/
{
    enter_start_state();
}

static YTOKEN scanDFA( ScanValue *value )
/***************************************/
{
    long                newint;     /* these are used to accumulate parts of */
    VarString           *newstring; /* a new value */
    YTOKEN              token;
#ifdef SCANDEBUG
    char                debugstring[10];
#endif
    char                *stringFromFile;
    int                 i;

    value->intinfo.type  = SCAN_INT_TYPE_DEFAULT;
    value->string.string = NULL;
    longString = FALSE;

    state( S_START ):
        if( isspace( LookAhead ) ) {
            do_transition( S_START );
        } else if( isdigit( LookAhead ) ) {
            newint = LookAhead - '0';
            newstring = VarStringStart();
            VarStringAddChar( newstring, LookAhead );
            if( LookAhead == '0' ) {
                do_transition( S_HEXSTART );
            } else {
                do_transition( S_DECIMAL );
            }
        } else if( isalpha( LookAhead ) || LookAhead == '_' ) {
            newstring = VarStringStart();
            VarStringAddChar( newstring, LookAhead );
            if( LookAhead == 'l' || LookAhead == 'L' ) {
                do_transition( S_L_STRING );
            }
            do_transition( S_NAME );
        } else switch( LookAhead ) {
            case '"':
                newstring = VarStringStart();  /* don't include the " */
                newLineInString = 0; /* reset newline in string status */
                do_transition( S_STRING );
            case '.':
                newstring = VarStringStart();
                VarStringAddChar( newstring, LookAhead );
                do_transition( S_DOS_FILENAME );
            case RC_EOF:
                DEBUGPUTS( "RC_EOF" )
                return( 0 );                /* yacc wants 0 on EOF */
            case '#':           do_transition( S_POUND_SIGN );
            case '(':           do_transition( S_LPAREN );
            case ')':           do_transition( S_RPAREN );
            case '[':           do_transition( S_LSQ_BRACKET );
            case ']':           do_transition( S_RSQ_BRACKET );
            case '{':           do_transition( S_LBRACE );
            case '}':           do_transition( S_RBRACE );
            case '+':           do_transition( S_PLUS );
            case '-':           do_transition( S_MINUS );
            case '~':           do_transition( S_BITNOT );
            case '!':           do_transition( S_NOT );
            case '*':           do_transition( S_TIMES );
            case '/':           do_transition( S_DIVIDE );
            case '%':           do_transition( S_MOD );
            case '>':           do_transition( S_GT );
            case '<':           do_transition( S_LT );
            case '=':           do_transition( S_EQ );
            case '&':           do_transition( S_BITAND );
            case '^':           do_transition( S_BITXOR );
            case '|':           do_transition( S_BITOR );
            case '?':           do_transition( S_QUESTION );
            case ':':           do_transition( S_COLON );
            case ',':           do_transition( S_COMMA );
            case ';':           do_transition( S_COMMENT );
            case '\\':
                newstring = VarStringStart();
                VarStringAddChar( newstring, LookAhead );
                do_transition( S_DOS_FILENAME );
            default:
                value->UnknownChar = LookAhead;
                do_transition( S_ERROR );
        }
    state( S_L_STRING ):
        if( LookAhead =='"' ) {
            longString = TRUE;
            RCFREE( VarStringEnd( newstring, NULL ) );
            change_state( S_START );
        } else {
            change_state( S_NAME );
        }
    state( S_ERROR ):
        ErrorHasOccured = TRUE;
        return( Y_SCAN_ERROR );

    state( S_COMMENT ):
        if( LookAhead == '\n' || LookAhead == RC_EOF ) {
            do_transition( S_START );
        } else {
            do_transition( S_COMMENT );
        }

    state( S_POUND_SIGN ):
        DEBUGPUTS( "#" )
        return( Y_POUND_SIGN );

    state( S_LPAREN ):
        DEBUGPUTS( "(" )
        return( Y_LPAREN );

    state( S_RPAREN ):
        DEBUGPUTS( ")" )
        return( Y_RPAREN );

    state( S_LSQ_BRACKET ):
        DEBUGPUTS( "[" )
        return( Y_LSQ_BRACKET );

    state( S_RSQ_BRACKET ):
        DEBUGPUTS( "]" )
        return( Y_RSQ_BRACKET );

    state( S_LBRACE ):
        DEBUGPUTS( "{" )
        return( Y_LBRACE );

    state( S_RBRACE ):
        DEBUGPUTS( "}" )
        return( Y_RBRACE );

    state( S_PLUS ):
        DEBUGPUTS( "+" )
        return( Y_PLUS );

    state( S_MINUS ):
        DEBUGPUTS( "-" )
        return( Y_MINUS );

    state( S_BITNOT ):
        DEBUGPUTS( "~" )
        return( Y_BITNOT );

    state( S_NOT ):
        if( LookAhead == '=' ) {
            do_transition( S_NE );
        } else {
            DEBUGPUTS( "!" )
            return( Y_NOT );
        }

    state( S_TIMES ):
        DEBUGPUTS( "*" )
        return( Y_TIMES );

    state( S_DIVIDE ):
        DEBUGPUTS( "/" )
        return( Y_DIVIDE );

    state( S_MOD ):
        DEBUGPUTS( "%" )
        return( Y_MOD );

    state( S_GT ):
        switch( LookAhead ) {
        case '>':       do_transition( S_SHIFTR );
        case '=':       do_transition( S_GE );
        default:
            DEBUGPUTS( ">" )
            return( Y_GT );
        }

    state( S_LT ):
        switch( LookAhead ) {
        case '<':       do_transition( S_SHIFTL );
        case '=':       do_transition( S_LE );
        default:
            DEBUGPUTS( "<" )
            return( Y_LT );
        }

    state( S_EQ ):
        if( LookAhead == '=' ) {
            do_transition( S_ENDEQ );
        } else {
            DEBUGPUTS( "=" )
            return( Y_SINGLE_EQ );
        }

    state( S_BITAND ):
        if( LookAhead == '&' ) {
            do_transition( S_AND );
        } else {
            DEBUGPUTS( "&" )
            return( Y_BITAND );
        }

    state( S_BITXOR ):
        DEBUGPUTS( "^" )
        return( Y_BITXOR );

    state( S_BITOR ):
        if( LookAhead == '|' ) {
            do_transition( S_OR );
        } else {
            DEBUGPUTS( "|" )
            return( Y_BITOR );
        }

    state( S_QUESTION ):
        DEBUGPUTS( "?" )
        return( Y_QUESTION );

    state( S_COLON ):
        DEBUGPUTS( ":" )
        return( Y_COLON );

    state( S_COMMA ):
        DEBUGPUTS( "," )
        return( Y_COMMA );

    state( S_NE ):
        DEBUGPUTS( "!=" )
        return( Y_NE );

    state( S_SHIFTR ):
        DEBUGPUTS( ">>" )
        return( Y_SHIFTR );

    state( S_GE ):
        DEBUGPUTS( ">=" )
        return( Y_GE );

    state( S_SHIFTL ):
        DEBUGPUTS( "<<" )
        return( Y_SHIFTL );

    state( S_LE ):
        DEBUGPUTS( "<=" )
        return( Y_LE );

    state( S_ENDEQ ):
        DEBUGPUTS( "==" )
        return( Y_EQ );

    state( S_AND ):
        DEBUGPUTS( "&&" )
        return( Y_AND );

    state( S_OR ):
        DEBUGPUTS( "||" )
        return( Y_OR );

    state( S_STRING ):
        /* handle double-byte characters */
        i = CharSetLen[LookAhead];
        if( i ) {
            VarStringAddChar( newstring, LookAhead );
            for( ; i > 0; --i ) {
                GetNextChar();
                VarStringAddChar( newstring, LookAhead );
            }
            do_transition( S_STRING );
        }

        // if newline in string was detected, remove all whitespace from
        // begining of the next line
        if( newLineInString ) {
            if( isspace( LookAhead ) ) {
                do_transition( S_STRING );
            } else {
                // non whitespace was detected, reset newline flag, so whitespaces are treated normally
                newLineInString = 0;
            }
        }

        switch( LookAhead ) {
        case '"':           do_transition( S_STRINGEND );
        case '\\':          do_transition( S_ESCAPE_CHAR );
        case '\n':
            if( RcIoIsCOrHFile() ) {
                value->string.string = VarStringEnd( newstring, &(value->string.length) );
                DEBUGPUTS( "STRING" )
                return( Y_STRING );
            } else {
                // MSVC's RC uses this obscure way of handling newline in strings and we follow.
                // First store <space> and then <newline character>. Then on next line, all white
                // spaces from begining of line is removed
                VarStringAddChar( newstring, ' ' );
                VarStringAddChar( newstring, LookAhead );
                newLineInString = 1;
                do_transition( S_STRING );
            }
        default:
            VarStringAddChar( newstring, LookAhead );
            do_transition( S_STRING );
        }

    state( S_ESCAPE_CHAR ):
        if( isdigit( LookAhead ) && LookAhead != '8' && LookAhead != '9' ) {
            newint = LookAhead - '0';
            do_transition( S_OCTAL_ESCAPE_1 );
        } else switch( LookAhead ) {
        case 'x':
            newint = 0;
            do_transition( S_HEX_ESCAPE_1 );
            break;
        case 'a':
            /* this is what Microsoft's RC adds for a \a */
            VarStringAddChar( newstring, '\x8' );
            do_transition( S_STRING );
            break;
        case 'b':
            VarStringAddChar( newstring, '\b' );
            do_transition( S_STRING );
            break;
        case 'f':
            VarStringAddChar( newstring, '\f' );
            do_transition( S_STRING );
            break;
        case 'n':
            VarStringAddChar( newstring, '\n' );
            do_transition( S_STRING );
            break;
        case 'r':
            VarStringAddChar( newstring, '\r' );
            do_transition( S_STRING );
            break;
        case 't':
            VarStringAddChar( newstring, '\t' );
            do_transition( S_STRING );
            break;
        case 'v':
            VarStringAddChar( newstring, '\v' );
            do_transition( S_STRING );
            break;
        default:
            VarStringAddChar( newstring, LookAhead );
            do_transition( S_STRING );
            break;
        }

    state( S_HEX_ESCAPE_1 ):
        if( isxdigit( LookAhead ) ) {
            AddDigitToInt( &newint, 16, LookAhead );
            do_transition( S_HEX_ESCAPE_2 );
        } else {
            change_state( S_STRING );
        }

    state( S_HEX_ESCAPE_2 ):
        if( isxdigit( LookAhead ) ) {
            AddDigitToInt( &newint, 16, LookAhead );
            VarStringAddChar( newstring, newint );
            do_transition( S_STRING );
        } else {
            VarStringAddChar( newstring, newint );
            change_state( S_STRING );
        }

    state( S_OCTAL_ESCAPE_1 ):
        if( isdigit( LookAhead ) && LookAhead != '8' && LookAhead != '9' ) {
            AddDigitToInt( &newint, 8, LookAhead );
            do_transition( S_OCTAL_ESCAPE_2 );
        } else {
            VarStringAddChar( newstring, newint );
            change_state( S_STRING );
        }

    state( S_OCTAL_ESCAPE_2 ):
        if( isdigit( LookAhead ) && LookAhead != '8' && LookAhead != '9' ) {
            AddDigitToInt( &newint, 8, LookAhead );
            do_transition( S_OCTAL_ESCAPE_3 );
        } else {
            VarStringAddChar( newstring, newint );
            change_state( S_STRING );
        }

    state( S_OCTAL_ESCAPE_3 ):
        VarStringAddChar( newstring, newint );
        change_state( S_STRING );

    state( S_STRINGEND ):
        if( LookAhead == '"' ) {   /* a "" in a string means include one " */
            VarStringAddChar( newstring, LookAhead );
            do_transition( S_STRING );
        } else {
            stringFromFile = VarStringEnd( newstring, &(value->string.length) );
            value->string.string = stringFromFile;
#if 0
            //DRW - this code truncates trailing null chars in resources
            //          like user data.  It is commented until I fix it.
            if( CmdLineParms.FindAndReplace == TRUE ) {
                char            *temp;
                temp = FindAndReplace( stringFromFile,
                                       CmdLineParms.FindReplaceStrings );
                // PrependToString prepends a string if that option
                // is specified. As a string from the rc file is only scanned
                // once and the string might have been changed by find and
                // replace, this is needed here
                prependToString( value, temp );
            } else if( CmdLineParms.Prepend == TRUE ) {
                prependToString( value, stringFromFile );
            }
#endif
            value->string.lstring = longString;
            if( longString && CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
                RcWarning( ERR_LSTRING_IGNORED_FOR_WINDOWS );
                value->string.lstring = FALSE;
            }
            DEBUGPUTS( value->string.string )
            return( Y_STRING );
        }

    state( S_DECIMAL ):
        VarStringAddChar( newstring, LookAhead );
        if( isdigit( LookAhead ) ) {
            AddDigitToInt( &newint, 10, LookAhead );
            do_transition( S_DECIMAL );
        } else if( toupper( LookAhead ) == 'L' ) {
            do_transition( S_LONGSUFFIX );
        } else if( toupper( LookAhead ) == 'U' ) {
            do_transition( S_UNSIGNEDSUFFIX );
        } else if( isalpha( LookAhead ) || LookAhead == '.'
                   || LookAhead == '\\' || LookAhead == '_' ) {
            do_transition( S_DOS_FILENAME );
        } else {
            value->intinfo.val = newint;
            value->intinfo.str = VarStringEnd( newstring, NULL );
            DEBUGPUTS( ltoa( newint, debugstring, 10 ) )
            return( Y_INTEGER );
        }

    state( S_LONGSUFFIX ):
        VarStringAddChar( newstring, LookAhead );
        value->intinfo.type |= SCAN_INT_TYPE_LONG;
        if( toupper( LookAhead ) == 'U' ) {
            value->intinfo.type |= SCAN_INT_TYPE_UNSIGNED;
            do_transition( S_ENDINT );
        } else if( isalpha( LookAhead ) || LookAhead == '.'
                   || LookAhead == '\\' || LookAhead == '_' ) {
            do_transition( S_DOS_FILENAME );
        } else {
            value->intinfo.val = newint;
            value->intinfo.str = VarStringEnd( newstring, NULL );
            DEBUGPUTS( ltoa( newint, debugstring, 10 ) )
            return( Y_INTEGER );
        }

    state( S_UNSIGNEDSUFFIX ):
        VarStringAddChar( newstring, LookAhead );
        value->intinfo.type |= SCAN_INT_TYPE_UNSIGNED;
        if( toupper( LookAhead ) == 'L' ) {
            value->intinfo.type |= SCAN_INT_TYPE_LONG;
            do_transition( S_ENDINT );
        } else if( isalpha( LookAhead ) || LookAhead == '.'
                   || LookAhead == '\\' || LookAhead == '_' ) {
            do_transition( S_DOS_FILENAME );
        } else {
            value->intinfo.val = newint;
            value->intinfo.str = VarStringEnd( newstring, NULL );
            DEBUGPUTS( ltoa( newint, debugstring, 10 ) )
            return( Y_INTEGER );
        }

    state( S_ENDINT ):
        if( isalpha( LookAhead ) || LookAhead == '.'
               || LookAhead == '\\' || LookAhead == '_' ) {
            VarStringAddChar( newstring, LookAhead );
            do_transition( S_DOS_FILENAME );
        } else {
            value->intinfo.val = newint;
            value->intinfo.str = VarStringEnd( newstring, NULL );
            DEBUGPUTS( ltoa( newint, debugstring, 10 ) )
            return( Y_INTEGER );
        }

    state( S_HEXSTART ):
        VarStringAddChar( newstring, LookAhead );
        if( isdigit( LookAhead ) ) {
            if( LookAhead == '8' || LookAhead == '9' ) {
                do_transition( S_DOS_FILENAME );
            } else {
                AddDigitToInt( &newint, 8, LookAhead );
                do_transition( S_OCT );
            }
        } else if( toupper( LookAhead ) == 'X' ) {
            do_transition( S_HEX );
        } else if( toupper( LookAhead ) == 'L' ) {
            do_transition( S_LONGSUFFIX );
        } else if( toupper( LookAhead ) == 'U' ) {
            do_transition( S_UNSIGNEDSUFFIX );
        } else if( isalpha( LookAhead ) || LookAhead == '.'
                   || LookAhead == '\\' || LookAhead == '_' ) {
            do_transition( S_DOS_FILENAME );
        } else {
            value->intinfo.val = newint;
            DEBUGPUTS( ltoa( newint, debugstring, 10 ) )
            value->intinfo.str = VarStringEnd( newstring, NULL );
            return( Y_INTEGER );
        }

    state( S_OCT ):
        VarStringAddChar( newstring, LookAhead );
        if( isdigit( LookAhead ) ) {
            if( LookAhead == '8' || LookAhead == '9' ) {
                do_transition( S_DOS_FILENAME );
            } else {
                AddDigitToInt( &newint, 8, LookAhead );
                do_transition( S_OCT );
            }
        } else if( toupper( LookAhead ) == 'L' ) {
            do_transition( S_LONGSUFFIX );
        } else if( toupper( LookAhead ) == 'U' ) {
            do_transition( S_UNSIGNEDSUFFIX );
        } else if( isalpha( LookAhead ) || LookAhead == '.'
                   || LookAhead == '\\' || LookAhead == '_' ) {
            do_transition( S_DOS_FILENAME );
        } else {
            value->intinfo.val = newint;
            value->intinfo.str = VarStringEnd( newstring, NULL );
            DEBUGPUTS( ltoa( newint, debugstring, 10 ) )
            return( Y_INTEGER );
        }

    state(S_HEX):
        VarStringAddChar( newstring, LookAhead );
        if( isxdigit( LookAhead ) ) {
            AddDigitToInt( &newint, 16, LookAhead );
            do_transition( S_HEX );
        } else if( toupper( LookAhead ) == 'L' ) {
            do_transition( S_LONGSUFFIX );
        } else if( toupper( LookAhead ) == 'U' ) {
            do_transition( S_UNSIGNEDSUFFIX );
        } else if( isalpha( LookAhead ) || LookAhead == '.'
                   || LookAhead == '\\' || LookAhead == '_' ) {
            do_transition( S_DOS_FILENAME );
        } else {
            value->intinfo.val = newint;
            value->intinfo.str = VarStringEnd( newstring, NULL );
            DEBUGPUTS( ltoa( newint, debugstring, 10 ) )
            return( Y_INTEGER );
        }

    state(S_NAME):
        if( isalnum( LookAhead ) || LookAhead == '_' ) {
            VarStringAddChar( newstring, LookAhead );
            do_transition( S_NAME );
        } else if( LookAhead == ':' || LookAhead == '\\' || LookAhead == '.' ) {
            VarStringAddChar( newstring, LookAhead );
            do_transition( S_DOS_FILENAME );
        } else {
            value->string.string = VarStringEnd( newstring,
                        &(value->string.length) );
            DEBUGPUTS( value->string.string )
            token = LookupKeywordOS2( value->string );
            if( token != Y_NAME ) {
                /* release the string if it is a keyword */
                RCFREE( value->string.string );
            }
            if( token == Y_RCINCLUDE ) {
                /* when inline preprocessing is in place take steps here */
                /* to make  rcinclude's  look line  #include's  */
                RcFatalError( ERR_NO_RCINCLUDES );
            }
            return( token );
        }

    state(S_DOS_FILENAME):
        if( isalnum( LookAhead ) || LookAhead == ':' || LookAhead == '\\'
                || LookAhead == '.' || LookAhead == '_' ) {
            VarStringAddChar( newstring, LookAhead );
            do_transition( S_DOS_FILENAME );
        } else {
            value->string.string = VarStringEnd( newstring,
                        &(value->string.length) );
            DEBUGPUTS( value->string.string )
            return( Y_DOS_FILENAME );
        }
} /* scanDFA */

YTOKEN ScanOS2( ScanValue *value )
/********************************/
{
    YTOKEN  token;

    token = scanDFA( value );
    while( token == Y_POUND_SIGN ) {
        token = scanCPPDirective( value );
    }

    return( token );
}

void ScanInitStaticsOS2( void )
/*****************************/
{
    _next = 0;
    LookAhead = 0;
    longString = 0;
}
