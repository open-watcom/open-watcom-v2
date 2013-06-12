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
* Description:  Lexical scanner for C++ compiler.
*
****************************************************************************/


#include "plusplus.h"

#include <assert.h>

#include "preproc.h"
#include "stats.h"
#include "scan.h"
#include "escchars.h"
#include "memmgr.h"
#include "dbg.h"
#include "name.h"
#include "context.h"
#include "unicode.h"

#if defined(__DOS__) || defined(__OS2__) || defined(__NT__)
    #define SYS_EOF_CHAR    0x1A
#elif defined(__UNIX__)
    #undef SYS_EOF_CHAR
#else
    #error SYS_EOF_CHAR is not set for this system
#endif

#ifndef NDEBUG
#define _BufferOverrun (*((uint_32*)(&Buffer[sizeof(Buffer)-sizeof(uint_32)])))
#define BUFFER_OVERRUN_CHECK    (0x35791113)
#endif

static  void    nextMacroToken( void );
static  void    (*tokenSource)( void ) = nextMacroToken;

static  char    *ReScanPtr;

ExtraRptCtr( nextTokenCalls );
ExtraRptCtr( nextTokenSavedId );
ExtraRptCtr( nextTokenNormal );
ExtraRptCtr( lookPastRewrite );

typedef enum {
    SCAN_NAME = 0,      // identifier
    SCAN_WIDE,          // L"abc" or L'a' or Lname
    SCAN_NUM,           // number that starts with a digit
    SCAN_DELIM1,        // single character delimiter
    SCAN_DELIM12,       // @ or @@ token
    SCAN_DELIM12EQ,     // @, @@, or @= token
    SCAN_DELIM12EQ2EQ,  // @, @@, @=, or @@= token
    SCAN_DELIM1EQ,      // @ or @= token
    SCAN_SLASH,         // /, /=, // comment, or /* comment */
    SCAN_LT,            // <, <=, <<, <<=, <%, <:
    SCAN_PERCENT,       // %, %=, %>, %:, %:%:
    SCAN_COLON,         // :, ::, or :>
    SCAN_MINUS,         // -, -=, --, ->, or ->*
    SCAN_FLOAT,         // .
    SCAN_STRING,        // "string"
    SCAN_STRING2,       // "string" continued
    SCAN_LSTRING2,      // L"string" continued
    SCAN_CHARCONST,     // 'a'
    SCAN_CR,            // '\r'
    SCAN_NEWLINE,       // '\n'
    SCAN_WHITESPACE,    // all whitespace
    SCAN_INVALID,       // all other characters
    SCAN_EOF,           // end-of-file
    SCAN_MAX
} scan_class;

static uint_8 ClassTable[LCHR_MAX];

static uint_8 InitClassTable[] = {
    '\r',       SCAN_CR,
    '\n',       SCAN_NEWLINE,
    ' ',        SCAN_WHITESPACE,
    '\f',       SCAN_WHITESPACE,
    '\t',       SCAN_WHITESPACE,
    '\v',       SCAN_WHITESPACE,
    '\'',       SCAN_CHARCONST,
    '"',        SCAN_STRING,
    '(',        SCAN_DELIM1,
    ')',        SCAN_DELIM1,
    ',',        SCAN_DELIM1,
    ';',        SCAN_DELIM1,
    '?',        SCAN_DELIM1,
    '[',        SCAN_DELIM1,
    ']',        SCAN_DELIM1,
    '{',        SCAN_DELIM1,
    '}',        SCAN_DELIM1,
    '~',        SCAN_DELIM1,
    '.',        SCAN_FLOAT,
    '#',        SCAN_DELIM12,           // #    ##
    '=',        SCAN_DELIM1EQ,          // = ==
    '^',        SCAN_DELIM1EQ,          // ^ ^=
    '!',        SCAN_DELIM1EQ,          // ! !=
    '%',        SCAN_PERCENT,           // % %= %> %: %:%:
    '*',        SCAN_DELIM1EQ,          // * *=
    '&',        SCAN_DELIM12EQ,         // & &= &&
    '|',        SCAN_DELIM12EQ,         // | |= ||
    '+',        SCAN_DELIM12EQ,         // + += ++
    '<',        SCAN_LT,                // < <= << <<= <% <:
    '>',        SCAN_DELIM12EQ2EQ,      // > >= >> >>=
    '-',        SCAN_MINUS,             // - -= -- -> ->*
    '/',        SCAN_SLASH,             // / /=    // /**/
    ':',        SCAN_COLON,             // :    :: :>
    '_',        SCAN_NAME,
    'L',        SCAN_WIDE,
    '\0',       0
};

// #undef static
// #define static

#if TARGET_INT == 2
static unsigned_64 intMax   = I64Val( 0x00000000, 0x00007fff );
static unsigned_64 uintMax  = I64Val( 0x00000000, 0x0000ffff );
#else
static unsigned_64 intMax   = I64Val( 0x00000000, 0x7fffffff );
static unsigned_64 uintMax  = I64Val( 0x00000000, 0xffffffff );
#endif

#define prt_char( x )           if( CompFlags.cpp_output ) { PrtChar( x ); }

#define diagnose_lex_error( e ) \
        (!(e) && ( SkipLevel == NestLevel ) && (PPControl & PPCTL_NO_LEX_ERRORS) == 0 )

void ReScanInit( char *ptr )
/**************************/
{
    ReScanPtr = ptr;
}

static int rescanBuffer( void )
{
    CurrChar = *(unsigned char *)ReScanPtr++;
    if( CurrChar == '\0' ) {
        CompFlags.rescan_buffer_done = 1;
    }
    return( CurrChar );
}

int ReScanToken( void )
/*********************/
{
    int saved_currchar;
    int (*saved_nextchar)(void);
    int saved_line;
    int saved_column;

    saved_line = TokenLine;
    saved_column = TokenColumn;
    saved_currchar = CurrChar;
    saved_nextchar = NextChar;
    CompFlags.rescan_buffer_done = 0;
    NextChar = rescanBuffer;
    NextChar();
    CurToken = ScanToken( 1 );
    --ReScanPtr;
    CurrChar = saved_currchar;
    NextChar = saved_nextchar;
    TokenLine = saved_line;
    TokenColumn = saved_column;
    return( CompFlags.rescan_buffer_done );
}

void (*SetTokenSource( void (*source)( void ) ))( void )
/******************************************************/
{
    void (*last_source)( void );

    last_source = tokenSource;
    tokenSource = source;
    return( last_source );
}

void ResetTokenSource( void (*source)( void ) )
/*********************************************/
{
    tokenSource = source;
}

TOKEN NextToken( void )
/*********************/
{
#ifdef XTRA_RPT
    ExtraRptIncrementCtr( nextTokenCalls );
    if( tokenSource == nextMacroToken ) {
        ExtraRptIncrementCtr( nextTokenNormal );
    }
#endif
    if( CurToken == T_SAVED_ID ) {
        ExtraRptIncrementCtr( nextTokenSavedId );
        CurToken = LAToken;
        return( CurToken );
    }
    (*tokenSource)();
#ifndef NDEBUG
    CtxScanToken();
    DumpToken();
#endif
    return( CurToken );
}

static int saveNextChar( void )
{
    int c;

    c = NextChar();
    if( TokenLen < BUF_SIZE - 2 ) {
        Buffer[TokenLen] = c;
        ++TokenLen;
    } else if( TokenLen == BUF_SIZE - 2 ) {
        if( NestLevel == SkipLevel ) {
            CErr1( ERR_TOKEN_TRUNCATED );
        }
        ++TokenLen;
    }
    return( c );
}

static int scanHex( int expanding )
{
    int c;
    struct {
        unsigned too_big : 1;
        unsigned at_least_one : 1;
    } flag;

    flag.too_big = FALSE;
    flag.at_least_one = FALSE;
    for(;;) {
        c = saveNextChar();
        if(( CharSet[ c ] & (C_HX|C_DI) ) == 0 )
            break;
        if( CharSet[ c ] & C_HX ) {
            c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        }
        if( U64Cnv16( &Constant64, c - '0' ) ) {
            flag.too_big = TRUE;
        }
        flag.at_least_one = TRUE;
    }
    if( ! flag.at_least_one ) {
        return( 0 );            /* indicate no characters matched after "0x" */
    }
    if( flag.too_big ) {
        if( diagnose_lex_error( expanding ) ) {
            CErr1( WARN_CONSTANT_TOO_BIG );
        }
    }
    return( 1 );                    /* indicate characters were matched */
}

static TOKEN idLookup( unsigned len, MEPTR *pmeptr )
{
    MEPTR fmentry;

    fmentry = MacroLookup( Buffer, len );
    if( fmentry != NULL ) {
        *pmeptr = fmentry;
        return( T_MACRO );
    }
    return( KwLookup( len ) );
}

static void prt_comment_char( int c )
{
    switch( c ) {
    case '\n':
        prt_char( c );
        break;
    case '\r':
    case LCHR_EOF:
        break;
    default:
        if( CompFlags.keep_comments  ) {
            prt_char( c );
        }
    }
}

static void scanCComment( void )
{
    int c;
    int prev_char;
    unsigned start_line;

    SrcFileCurrentLocation();
    start_line = TokenLine;
    CompFlags.scanning_c_comment = 1;
    if( CompFlags.cpp_output ) {
        prt_comment_char( '/' );
        prt_comment_char( '*' );
        c = '\0';
        for(;;) {
            for(;;) {
                prev_char = c;
                c = NextChar();
                prt_comment_char( c );
                if( c == '/' ) break;
                if( c == LCHR_EOF ) {
                    /* unterminated comment already detected in NextChar() */
                    CompFlags.scanning_c_comment = 0;
                    return;
                }
            }
            if( prev_char == '*' ) break;
            // get next character and see if it is '*' for nested comment
            c = NextChar();
            prt_comment_char( c );
            if( c == '*' ) {
                /* '*' may be just about to close this comment */
                c = NextChar();
                prt_comment_char( c );
                if( c == '/' ) break;
                if( c == LCHR_EOF ) {
                    /* unterminated comment already detected in NextChar() */
                    CompFlags.scanning_c_comment = 0;
                    return;
                }
                SrcFileSetErrLoc();
                CErr2( WARN_NESTED_COMMENT, start_line );
            }
        }
    } else {
        c = '\0';
        for(;;) {
            for(;;) {
                prev_char = c;
                c = NextChar();
                if( c == '/' ) break;
                if( c == LCHR_EOF ) {
                    /* unterminated comment already detected in NextChar() */
                    CompFlags.scanning_c_comment = 0;
                    return;
                }
            }
            if( prev_char == '*' ) break;
            // get next character and see if it is '*' for nested comment
            c = NextChar();
            if( c == '*' ) {
                /* '*' may be just about to close this comment */
                c = NextChar();
                if( c == '/' ) break;
                if( c == LCHR_EOF ) {
                    /* unterminated comment already detected in NextChar() */
                    CompFlags.scanning_c_comment = 0;
                    return;
                }
                SrcFileSetErrLoc();
                CErr2( WARN_NESTED_COMMENT, start_line );
            }
        }
    }
    CompFlags.scanning_c_comment = 0;
    NextChar();
}

static void scanCppComment( void )
{
    int c;

    CompFlags.scanning_cpp_comment = 1;
    if( CompFlags.cpp_output ) {
        prt_comment_char( '/' );
        prt_comment_char( '/' );
        for(;;) {
            c = NextChar();
            if( c == LCHR_EOF ) break;
            if( c == '\n' ) break;
            prt_comment_char( c );
        }
    } else {
        SrcFileScanCppComment();
    }
    CompFlags.scanning_cpp_comment = 0;
}

static int doESCChar( int c, int expanding, int char_type )
{
    unsigned    n;
    unsigned    i;
    int         classification;

    classification = classify_escape_char( c );
    if( classification == ESCAPE_OCTAL ) {
        n = 0;
        for( i = 3; i > 0; --i ) {
            c = octal_dig( c );
            if( c == 8 ) break;
            n = n * 8 + c;
            c = saveNextChar();
        }
        if( n > 0377 && char_type != TYP_WCHAR ) {
            if( diagnose_lex_error( expanding ) ) {
                SrcFileSetErrLoc();
                CErr1( ANSI_INVALID_OCTAL_ESCAPE );
            }
            n &= 0377;
        }
    } else if( classification == ESCAPE_HEX ) {
        U64Clear( Constant64 );
        if( scanHex( expanding ) ) {
            n = U32FetchTrunc( Constant64 );
            if( n > 0x0ff && char_type != TYP_WCHAR ) {
                if( diagnose_lex_error( expanding ) ) {
                    SrcFileSetErrLoc();
                    CErr1( ANSI_INVALID_HEX_ESCAPE );
                }
                n &= 0x0ff;
            }
        } else {                    /*  '\xz' where z is not a hex char */
            n = 'x';
        }
    } else if( classification == ESCAPE_NONE ) {
        n = c;
        saveNextChar();
    } else {
        n = classification;
        saveNextChar();
    }
    return( n );
}

static TOKEN charConst( int char_type, int expanding )
{
    int c;
    int i;
    TOKEN token;
    long value;
    struct {
        unsigned double_byte_char : 1;
    } flag;

    flag.double_byte_char = FALSE;
    token = T_CONSTANT;
    BadTokenInfo = ERR_INV_CHAR_CONSTANT;   /* in case of error */
    i = 0;
    c = saveNextChar();
    if( c == '\'' ) {
        NextChar();
        Buffer[TokenLen] = '\0';
        return( T_BAD_TOKEN );
    }
    value = 0;
    for( ;; ) {
        if( c == '\r' || c == '\n' ) {
            DbgAssert( Buffer[TokenLen-1] == c );
            --TokenLen;
            token = T_BAD_TOKEN;
            break;
        }
        if( c == '\\' ) {
            c = doESCChar( saveNextChar(), expanding, char_type );
            if( char_type == TYP_WCHAR ) {
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
            }
        } else {
            if( CharSet[c] & C_DB ) {   /* if double-byte char */
                c = (c << 8) + (saveNextChar() & 0x00FF);
                if( char_type == TYP_WCHAR ) {
                    if( CompFlags.jis_to_unicode ) {
                        c = JIS2Unicode( c );
                    }
                }
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
                flag.double_byte_char = TRUE;
            } else if( char_type == TYP_WCHAR ) {
                if( CompFlags.use_unicode ) {
                    c = UniCode[ c ];
                } else if( CompFlags.jis_to_unicode ) {
                    c = JIS2Unicode( c );
                }
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
            }
            saveNextChar();
        }
        ++i;
        value = (value << 8) + c;
        /* handle case where user wants a \ but doesn't escape it */
        if( c == '\'' && CurrChar != '\'' ) {
            if( ! CompFlags.cpp_output ) {
                token = T_BAD_TOKEN;
                break;
            }
        }
        c = CurrChar;
        if( c == '\'' ) break;
        if( i >= 4 ) {
            if( ! CompFlags.cpp_output ) {
                token = T_BAD_TOKEN;
                break;
            }
        }
    }
    if( token != T_BAD_TOKEN ) {
        NextChar();
    }
    Buffer[TokenLen] = '\0';
    ConstType = char_type;
    if( char_type == TYP_CHAR ) {
        if( value >= 0 && value < 256 ) {
            if( CompFlags.signed_char ) {
                if( value & 0x80 ) {            /* if sign bit is on */
                    value |= 0xFFFFFF00;        /* - sign extend it */
                }
            }
        } else {
            // value has more than 8 bits
            if( char_type == TYP_CHAR && ! flag.double_byte_char ) {
                if( diagnose_lex_error( expanding ) ) {
                    CErr2( WARN_CHAR_VALUE_LARGE, value );
                }
            }
        }
    }
    I32ToI64( value, &Constant64 );
    return( token );
}

static int printWhiteSpace( int c )
{
    for(;;) {
        if(( CharSet[ c ] & C_WS) == 0 ) break;
        if( c != '\r' ) {
            PrtChar( c );
        }
        c = NextChar();
    }
    return( c );
}

static TOKEN scanWhiteSpace( int expanding )
{
    SrcFileScanWhiteSpace( expanding );
    return( T_WHITE_SPACE );
}

static int skipWhiteSpace( int c )
{
    if( CompFlags.cpp_output && (PPControl & PPCTL_EOL) == 0 ) {
        c = printWhiteSpace( c );
    } else {
        SrcFileScanWhiteSpace( 0 );
        c = CurrChar;
    }
    return( c );
}

static void unGetChar( int c )
{
    if( NextChar == rescanBuffer ) {
        --ReScanPtr;
        CompFlags.rescan_buffer_done = 0;
    } else {
        GetNextCharUndo( c );
    }
}

boolean ScanOptionalComment( void )
/*********************************/
{
    boolean retn;
    int c;

    retn = FALSE;
    for(;;) {
        c = CurrChar;
        if( c != '/' ) break;
        c = NextChar();
        if( c == '*' ) {
            scanCComment();
            retn = TRUE;
        } else if( c == '/' ) {
            scanCppComment();
            retn = TRUE;
        } else {
            unGetChar( c );
            CurrChar = '/';
            break;
        }
    }
    return retn;
}

void SkipAhead( void )
/********************/
{
    int c;

    for(;;) {
        c = CurrChar;
        for(;;) {
            if( CharSet[c] & C_WS ) {
                c = skipWhiteSpace( c );
            }
            if( c != '\n' ) break;
            if( (PPControl & PPCTL_EOL) == 0 ) {
                prt_char( '\n' );
            }
            c = NextChar();
        }
        if( c != '/' ) break;
        if( ! ScanOptionalComment() ) break;
    }
}

static TOKEN scanDotSomething( int c )
{
    if( c == '*' ) {
        NextChar();
        return( T_DOT_STAR );
    }
    if( c == '.' ) {
        c = saveNextChar();
        if( c == '.' ) {
            NextChar();
            return( T_DOT_DOT_DOT );
        }
        unGetChar( c );
        CurrChar = '.';
    }
    Buffer[1] = '\0';
    TokenLen = 1;
    return( T_DOT );
}

static TOKEN doScanFloat( void )
{
    int c;
    int one_case;

    c = CurrChar;
    if( c == '.' ) {
        for(;;) {
            c = saveNextChar();
            if(( CharSet[c] & C_DI ) == 0 ) break;
        }
        if( TokenLen == 2 ) {       /* .? */
            return scanDotSomething( c );
        }
    }
    CurToken = T_CONSTANT;
    if( ONE_CASE( c ) == ONE_CASE( 'E' ) ) {
        c = saveNextChar();
        if( c == '+' || c == '-' ) {
            c = saveNextChar();
        }
        if(( CharSet[c] & C_DI ) == 0 ) {
            CurToken = T_BAD_TOKEN;
            BadTokenInfo = ERR_INVALID_FLOATING_POINT_CONSTANT;
        }
        for(;;) {
            if(( CharSet[c] & C_DI ) == 0 ) break;
            c = saveNextChar();
        }
    }
    one_case = ONE_CASE( c );
    if( one_case == ONE_CASE( 'F' ) ) {
        c = saveNextChar();
        ConstType = TYP_FLOAT;
    } else if( one_case == ONE_CASE( 'L' ) ) {
        c = saveNextChar();
        ConstType = TYP_LONG_DOUBLE;
    } else {
        ConstType = TYP_DOUBLE;
    }
    if( (PPControl & PPCTL_ASM) && (CharSet[c] & (C_AL | C_DI)) ) {
        for(;;) {
            c = saveNextChar();
            if( (CharSet[c] & (C_AL | C_DI)) == 0 ) break;
        }
        --TokenLen;
        Buffer[TokenLen] = '\0';
        return( T_BAD_TOKEN );
    } else {
        --TokenLen;
        Buffer[TokenLen] = '\0';
        return( CurToken );
    }
}

static void willContinueStringLater( int string_type )
{
    if( string_type == TYP_WCHAR ) {
        CurrChar = LCHR_CONTINUE_LSTRING;
    } else {
        CurrChar = LCHR_CONTINUE_STRING;
    }
    ++TokenLen;
}

static TOKEN doScanString( int string_type, int expanding )
{
    int c;
    int ok;

    SrcFileSetSwEnd( TRUE );
    ok = 0;
    c = NextChar();
    Buffer[0] = c;
    TokenLen = 1;
    for(;;) {
        if( c == '\n' ) break;
        if( c == LCHR_EOF ) break;
        if( c == '"' ) {
            ok = 2;
            break;
        }
        if( c == '\\' ) {
            if( TokenLen > BUF_SIZE - 32 ) {
                unGetChar( c );
                // break long strings apart (parser will join them)
                willContinueStringLater( string_type );
                ok = 1;
                break;
            }
            c = NextChar();
            Buffer[TokenLen++] = c;
            if(( CharSet[c] & C_WS ) == 0 ) {
                doESCChar( c, expanding, string_type );
            }
            c = CurrChar;
        } else {
            if( CharSet[c] & C_DB ) {
                // if first char of a double-byte char, grab next one
                c = NextChar();
                Buffer[TokenLen++] = c;
            }
            if( TokenLen > BUF_SIZE - 32 ) {
                // break long strings apart (parser will join them)
                willContinueStringLater( string_type );
                ok = 1;
                break;
            }
            c = NextChar();
            Buffer[TokenLen++] = c;
        }
    }
    SrcFileSetSwEnd( FALSE );
    Buffer[TokenLen-1] = '\0';
    if( ok ) {
        if( ok == 2 ) {
            NextChar();
        }
        if( string_type == TYP_WCHAR ) {
            return( T_LSTRING );
        }
        return( T_STRING );
    }
    // '\n' or LCHR_EOF don't print nicely
    --TokenLen;
    Buffer[TokenLen-1] = '\0';
    BadTokenInfo = ERR_MISSING_QUOTE;
    return( T_BAD_TOKEN );
}

static TOKEN doScanName( int c, int expanding )
{
    MEPTR fmentry = NULL;

    SrcFileScanName( c );
    if( expanding || (PPControl & PPCTL_NO_EXPAND) ) {
        return( T_ID );
    }
    CurToken = idLookup( TokenLen, &fmentry );
    if( CurToken == T_MACRO ) {
        prt_char( ' ' );
        if( fmentry->macro_defn == 0 ) {
            return( SpecialMacro( fmentry ) );
        }
        fmentry->macro_flags |= MFLAG_REFERENCED;
        /* if macro requires parameters and next char is not a '('
        then this is not a macro */
        if( fmentry->parm_count != 0 ) {
            SkipAhead();
            if( CurrChar != '(' ) {
                if( CompFlags.cpp_output ) {
                    Buffer[TokenLen++] = ' ';
                    Buffer[TokenLen] = '\0';
                    return( T_ID );
                }
                return( KwLookup( TokenLen ) );
            }
        }
        DoMacroExpansion( fmentry );
        DbgAssert( _BufferOverrun == BUFFER_OVERRUN_CHECK );
        GetMacroToken( FALSE );
        if( CurToken == T_NULL ) {
            CurToken = T_WHITE_SPACE;
        }
    }
    return( CurToken );
}

static TOKEN scanName( int expanding )
{
    int c;

    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    c = NextChar();
    Buffer[1] = c;
    TokenLen = 2;
    return( doScanName( c, expanding ) );
}

static TOKEN doScanAsmToken( void )
{
    TokenLen = 0;
    do {
        Buffer[TokenLen++] = CurrChar;
        if( CurrChar == '.' ) {
            CurrChar = saveNextChar();
        }
        SrcFileScanName( CurrChar );
    } while( CurrChar == '.' );
    CurToken = T_ID;
    return( CurToken );
}

static TOKEN scanWide( int expanding )  // scan something that starts with L
{
    int c;
    TOKEN token;

    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    c = NextChar();
    Buffer[1] = c;
    TokenLen = 2;
    if( c == '"' ) {                    // L"abc"
        token = doScanString( TYP_WCHAR, expanding );
    } else if( c == '\'' ) {            // L'a'
        token = charConst( TYP_WCHAR, expanding );
    } else {                            // regular identifier
        token = doScanName( c, expanding );
    }
    return( token );
}

static void msIntSuffix( uint_32 signed_max, unsigned sid, unsigned uid, unsigned_64 *max_value )
{
    if( U64IsU32( Constant64 ) ) {
        uint_32 v = U32Fetch( Constant64 );
        if( v <= signed_max ) {
            ConstType = sid;
        } else {
            // 127 => 255, 32767 => 65535, 0x7fffffff => 0xffffffff
            uint_32 unsigned_max = (( signed_max + 1 ) << 1 ) - 1;
            if( v <= unsigned_max ) {
                ConstType = uid;
            } else {
                if( U64Cmp( &Constant64, max_value ) > 0 ) {
                    if( U64IsI32( Constant64 ) ) {
                        ConstType = TYP_SLONG;
                    } else {
                        ConstType = TYP_ULONG;
                    }
                } else {
                    if( U64Cmp( &Constant64, &intMax ) > 0 ) {
                        ConstType = TYP_UINT;
                    } else {
                        ConstType = TYP_SINT;
                    }
                }
            }
        }
    } else {
        if( U64IsI64( Constant64 ) ) {
            ConstType = TYP_SLONG64;
        } else {
            ConstType = TYP_ULONG64;
        }
    }
}

static TOKEN scanNum( int expanding )
{
    int c;
    unsigned_64 *max_value;
    unsigned long value;
    char too_big;
    char max_digit;

    SrcFileCurrentLocation();
    if( PPControl & PPCTL_ASM )
        return( doScanAsmToken() );

    U64Clear( Constant64 );
    value = 0;
    too_big = 0;
    TokenLen = 1;
    c = CurrChar;
    Buffer[0] = c;
    if( c == '0' ) {
        c = saveNextChar();
        if( ONE_CASE( c ) == ONE_CASE( 'X' ) ) {
            if( scanHex( expanding ) ) {
                c = CurrChar;       /* get next character */
            } else {
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_HEX_CONSTANT );
                } else {
                    unGetChar( CurrChar );
                    --TokenLen;
                    CurrChar = c;       /* put last character back */
                }
            }
        } else {                    /* scan octal number */
            max_digit = '7';
            if( expanding ) {
                // if collecting tokens for macro preprocessor, allow 8 and 9
                // since the argument may be used in with # or ##.
                max_digit = '9';
            }
            while( c >= '0' && c <= max_digit ) {
                if( U64Cnv8( &Constant64, c - '0' ) ) {
                    too_big = 1;
                }
                c = saveNextChar();
            }
            c = CurrChar;
            switch( c ) {       /* could be front of a float constant */
            case '8':           /* 0128.3 */
            case '9':           /* 0129.3 */
                for(;;) {
                    if(( CharSet[c] & C_DI ) == 0 ) break;
                    c = saveNextChar();
                }
                if( c != '.' && c != 'e' && c != 'E' ) {
                    if( diagnose_lex_error( expanding ) ) {
                        CErr1( ERR_INVALID_OCTAL_CONSTANT );
                    }
                }
                /* fall through */
            case '.':           /* 012.3 */
            case 'e':           /* 012e3 */
            case 'E':           /* 012E9 */
                return( doScanFloat() );
            }
        }
        max_value = &uintMax;
    } else {                /* scan decimal number */
        // we know 'c' is a digit
        U32ToU64( c - '0', &Constant64 );
        c = saveNextChar();
        for(;;) {
            if(( CharSet[c] & C_DI ) == 0 ) break;
            if( U64Cnv10( &Constant64, c - '0' ) ) {
                too_big = 1;
            }
            c = saveNextChar();
        }
        if( c == '.' || ONE_CASE( c ) == ONE_CASE( 'E' ) ) {
            return( doScanFloat() );
        }
        max_value = &intMax;
    }
    switch( ONE_CASE( c ) ) {
    case ONE_CASE( 'i' ):
        ConstType = TYP_SINT;
        c = saveNextChar();
        switch( c ) {
        case '6':
            c = saveNextChar();
            if( c == '4' ) {
                c = saveNextChar();
                if( U64IsI64( Constant64 ) ) {
                    ConstType = TYP_SLONG64;
                } else {
                    ConstType = TYP_ULONG64;
                }
            } else {
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                }
                unGetChar( c );
            }
            break;
        case '1':
            c = saveNextChar();
            if( c == '6' ) {
                c = saveNextChar();
                msIntSuffix( 0x00007fff, TYP_SSHORT, TYP_USHORT, max_value );
            } else {
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                }
            }
            break;
        case '3':
            c = saveNextChar();
            if( c == '2' ) {
                c = saveNextChar();
                msIntSuffix( 0x7fffffff, TYP_SLONG, TYP_ULONG, max_value );
            } else {
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                }
            }
            break;
        case '8':
            c = saveNextChar();
            msIntSuffix( 0x0000007f, TYP_SCHAR, TYP_UCHAR, max_value );
            break;
        default:
            if( diagnose_lex_error( expanding ) ) {
                CErr1( ERR_INVALID_CONSTANT_SUFFIX );
            }
            unGetChar( c );
        }
        break;
    case ONE_CASE( 'L' ):
        ConstType = TYP_SLONG;
        c = ONE_CASE( saveNextChar() );
        if( c == ONE_CASE( 'u' ) ) {
            c = saveNextChar();
            ConstType = TYP_ULONG;
        } else if( c == ONE_CASE( 'L' ) ) {
            c = ONE_CASE( saveNextChar() );
            if( c == ONE_CASE( 'u' ) ) {
                c = saveNextChar();
                ConstType = TYP_ULONG64;
            } else {
                ConstType = TYP_SLONG64;
            }
        } else if( ! U64IsI32( Constant64 ) ) { // Constant > 0x7FFFFFFFul
            ConstType = TYP_ULONG;
        }
        if( ! U64IsU32( Constant64 ) ) {
            if( U64IsI64( Constant64 ) ) {
                ConstType = TYP_SLONG64;
            } else {
                ConstType = TYP_ULONG64;
            }
        }
        break;
    case ONE_CASE( 'u' ):
        c = saveNextChar();
        switch( ONE_CASE( c ) ) {
        case ONE_CASE( 'i' ):
            ConstType = TYP_ULONG64;
            c = saveNextChar();
            switch( c ) {
            case '6':
                c = saveNextChar();
                if( c == '4' ) {
                    c = saveNextChar();
                } else {
                    if( diagnose_lex_error( expanding ) ) {
                        CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                    }
                    unGetChar( c );
                }
                break;
            case '1':
                c = saveNextChar();
                if( c == '6' ) {
                    c = saveNextChar();
                    msIntSuffix( 0x00007fff, TYP_USHORT, TYP_USHORT, &uintMax );
                } else {
                    if( diagnose_lex_error( expanding ) ) {
                        CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                    }
                    unGetChar( c );
                }
                break;
            case '3':
                c = saveNextChar();
                if( c == '2' ) {
                    c = saveNextChar();
                    msIntSuffix( 0x7fffffff, TYP_ULONG, TYP_ULONG, &uintMax );
                } else {
                    if( diagnose_lex_error( expanding ) ) {
                        CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                    }
                    unGetChar( c );
                }
                break;
            case '8':
                c = saveNextChar();
                msIntSuffix( 0x0000007f, TYP_UCHAR, TYP_UCHAR, &uintMax );
                break;
            default:
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                }
                unGetChar( c );
            }
            break;
        case ONE_CASE( 'L' ):
            c = ONE_CASE( saveNextChar() );
            if( c == ONE_CASE( 'L' ) ) {
                c = saveNextChar();
                ConstType = TYP_ULONG64;
            } else {
                ConstType = TYP_ULONG;
            }
            break;
        default:
            ConstType = TYP_UINT;
            if( U64Cmp( &Constant64, &uintMax ) > 0 ) { // Constant > TARGET_UINT_MAX
                ConstType = TYP_ULONG;
            }
        }
        if( ! U64IsU32( Constant64 ) ) {
            ConstType = TYP_ULONG64;
        }
        break;
    default:
        ConstType = TYP_SINT;
        if( U64Cmp( &Constant64, max_value ) > 0 ) {
            if( U64IsU32( Constant64 ) ) {
                if( U64IsI32( Constant64 ) ) {
                    ConstType = TYP_SLONG;
                } else {
                    ConstType = TYP_ULONG;
                }
            } else {
                if( U64IsI64( Constant64 ) ) {
                    ConstType = TYP_SLONG64;
                } else {
                    ConstType = TYP_ULONG64;
                }
            }
        } else {
            DbgAssert( U64IsU32( Constant64 ) );
            if( U64Cmp( &Constant64, &intMax ) > 0 ) { // Constant > TARGET_INT_MAX
                ConstType = TYP_UINT;
            }
        }
    }
    if( too_big ) {
        if( diagnose_lex_error( expanding ) ) {
            CErr1( WARN_CONSTANT_TOO_BIG );
        }
    }
    if( (PPControl & PPCTL_ASM) && (CharSet[c] & (C_AL | C_DI)) ) {
        for(;;) {
            c = saveNextChar();
            if( (CharSet[c] & (C_AL | C_DI)) == 0 ) break;
        }
        --TokenLen;
        Buffer[TokenLen] = '\0';
        return( T_BAD_TOKEN );
    } else {
        --TokenLen;
        Buffer[TokenLen] = '\0';
        return( T_CONSTANT );
    }
}

static TOKEN scanDelim1( int expanding )
{
    TOKEN token;

    SrcFileCurrentLocation();
    expanding = expanding;
    token = TokValue[ CurrChar ];
    Buffer[0] = CurrChar;
    Buffer[1] = '\0';
    TokenLen = 1;
    NextChar();
    return( token );
}

static TOKEN scanDelim12( int expanding )       // @ or @@ token
{
    int c;
    int chr2;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    c = CurrChar;
    Buffer[0] = c;
    tok = TokValue[ c ];
    token_len = 1;
    chr2 = NextChar();
    Buffer[1] = chr2;
    if( chr2 == c ) {
        tok += 2;
        ++token_len;
        NextChar();
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanDelim12EQ( int expanding )     // @, @@, or @= token
{
    int c;
    int chr2;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    c = CurrChar;
    Buffer[0] = c;
    tok = TokValue[ c ];
    token_len = 1;
    chr2 = NextChar();
    Buffer[1] = chr2;
    if( chr2 == '=' ) {
        ++tok;
        ++token_len;
        NextChar();
    } else if( chr2 == c ) {
        tok += 2;
        ++token_len;
        NextChar();
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanDelim12EQ2EQ( int expanding )  // @, @@, @=, or @@= token
{
    int c;
    int chr2;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    c = CurrChar;
    Buffer[0] = c;
    tok = TokValue[ c ];
    token_len = 1;
    chr2 = NextChar();
    Buffer[1] = chr2;
    if( chr2 == '=' ) {
        ++tok;
        ++token_len;
        NextChar();
    } else if( chr2 == c ) {
        tok += 2;
        ++token_len;
        if( NextChar() == '=' ) {
            ++tok;
            ++token_len;
            Buffer[2] = '=';
            NextChar();
        }
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanDelim1EQ( int expanding )      // @ or @= token
{
    int c;
    int chr2;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    c = CurrChar;
    Buffer[0] = c;
    tok = TokValue[ c ];
    token_len = 1;
    chr2 = NextChar();
    Buffer[1] = chr2;
    if( chr2 == '=' ) {
        ++tok;
        ++token_len;
        NextChar();
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanSlash( int expanding ) // /, /=, // comment, or /*comment*/
{
    int nc;
    int tok;
    int token_len;

    SrcFileCurrentLocation();
    Buffer[0] = '/';
    nc = NextChar();
    Buffer[1] = nc;
    tok = T_DIVIDE;
    token_len = 1;
    if( nc == '=' ) {
        ++tok;
        ++token_len;
        NextChar();
    } else if( ! expanding ) {
        if( nc == '/' ) {
            // C++ comment
            scanCppComment();
            Buffer[0] = ' ';
            tok = T_WHITE_SPACE;
        } else if( nc == '*' ) {
            // C comment
            scanCComment();
            Buffer[0] = ' ';
            tok = T_WHITE_SPACE;
        }
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanLT( int expanding )    // <, <=, <<, <<=, <%, <:
{
    int nc;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    Buffer[0] = '<';
    tok = T_LT;
    token_len = 1;
    nc = NextChar();
    Buffer[1] = nc;
    if( nc == '=' ) {
        ++tok;
        ++token_len;
        NextChar();
    } else if( nc == '<' ) {
        tok += 2;
        ++token_len;
        if( NextChar() == '=' ) {
            ++tok;
            ++token_len;
            Buffer[2] = '=';
            NextChar();
        }
    } else if( nc == '%' ) {
        tok = T_ALT_LEFT_BRACE;
        ++token_len;
        NextChar();
    } else if( nc == ':' ) {
        tok = T_ALT_LEFT_BRACKET;
        ++token_len;
        NextChar();
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanPercent( int expanding )   // %, %=, %>, %:, %:%:
{
    int nc;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    Buffer[0] = '%';
    tok = T_PERCENT;
    token_len = 1;
    nc = NextChar();
    Buffer[1] = nc;
    if( nc == '=' ) {
        ++tok;
        ++token_len;
        NextChar();
    } else if( nc == '>' ) {
        tok = T_ALT_RIGHT_BRACE;
        ++token_len;
        NextChar();
    } else if( nc == ':' ) {
        ++token_len;
        tok = T_ALT_SHARP;
        if( NextChar() == '%' ) {
            Buffer[2] = '%';
            ++token_len;
            if( NextChar() == ':' ) {
                ++token_len;
                tok = T_ALT_SHARP_SHARP;
                Buffer[3] = ':';
                NextChar();
            } else {
                unGetChar( CurrChar );
                CurrChar = '%';
            }
        }
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanColon( int expanding ) // :, ::, or :>
{
    int nc;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    Buffer[0] = ':';
    nc = NextChar();
    Buffer[1] = nc;
    tok = T_COLON;
    token_len = 1;
    if( nc == ':' ) {
        tok += 2;
        NextChar();
        ++token_len;
    } else if( nc == '>' ) {
        // TODO: according to the standard, ":>" should be an
        // alternative token (digraph) for "]" (T_RIGHT_BRACKET)...
        tok = T_SEG_OP;
        NextChar();
        ++token_len;
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanMinus( int expanding ) // -, -=, --, ->, or ->*
{
    int nc;
    int nnc;
    TOKEN tok;
    int token_len;

    expanding = expanding;
    SrcFileCurrentLocation();
    Buffer[0] = '-';
    nc = NextChar();
    Buffer[1] = nc;
    tok = T_MINUS;
    token_len = 1;
    if( nc == '=' ) {
        ++tok;
        NextChar();
        ++token_len;
    } else if( nc == '-' ) {
        tok += 2;
        NextChar();
        ++token_len;
    } else if( nc == '>' ) {
        ++token_len;
        nnc = NextChar();
        if( nnc == '*' ) {
            Buffer[2] = nnc;
            tok = T_ARROW_STAR;
            NextChar();
            ++token_len;
        } else {
            tok = T_ARROW;
        }
    }
    Buffer[ token_len ] = '\0';
    TokenLen = token_len;
    return( tok );
}

static TOKEN scanFloat( int expanding )
{
    expanding = expanding;
    SrcFileCurrentLocation();
    if( PPControl & PPCTL_ASM )
        return( doScanAsmToken() );

    Buffer[0] = CurrChar;
    TokenLen = 1;
    return( doScanFloat() );
}

static TOKEN scanPPNumber( void )
{
    int c;
    int prevc;

    // 3.1.8 pp-number
    // pp-number:
    //          digit           (checked by caller)
    //          . digit         (checked by caller)
    //          pp-number digit
    //          pp-number non-digit
    //          pp-number e sign
    //          pp-number E sign
    //          pp-number .
    //
    c = 0;
    for(;;) {
        prevc = c;
        c = saveNextChar();
        if( CharSet[c] & (C_AL|C_DI) ) {
            continue;
        }
        if( c == '.' ) {
            continue;
        }
        if( ONE_CASE( prevc ) == ONE_CASE( 'e' ) ) {
            if( c == '+' || c == '-' ) {
                if( CompFlags.extensions_enabled ) {
                    /* concession to existing practice...
                        #define A2 0x02
                        #define A3 0xaa0e+A2
                        // users want: 0xaa0e + 0x02
                        // not: 0xaa0e + A2 (but, this is what ISO C requires!)

                        after the 'sign', we bail out if we don't find a digit
                    */
                    prevc = c;
                    c = saveNextChar();
                    if(( CharSet[c] & C_DI ) == 0 ) {
                        // this check will kick out on a '+' and '-' also
                        break;
                    }
                }
                continue;
            }
        }
        break;
    }
    --TokenLen;
    Buffer[TokenLen] = '\0';
    return( T_PPNUMBER );
}

static TOKEN scanPPDigit( int expanding )
{
    expanding = expanding;
    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    TokenLen = 1;
    return scanPPNumber();
}

static TOKEN scanPPDot( int expanding )
{
    int         c;

    expanding = expanding;
    SrcFileCurrentLocation();
    Buffer[0] = '.';
    TokenLen = 1;
    c = saveNextChar();
    if( c >= '0' && c <= '9' ) {
        return scanPPNumber();
    } else {
        return scanDotSomething( c );
    }
}

static TOKEN scanString( int expanding )
{
    SrcFileCurrentLocation();
    return( doScanString( TYP_CHAR, expanding ) );
}

static TOKEN scanStringContinue( int expanding )
{
    SrcFileCurrentLocation();
    return( doScanString( TYP_CHAR, expanding ) );
}

static TOKEN scanLStringContinue( int expanding )
{
    SrcFileCurrentLocation();
    return( doScanString( TYP_WCHAR, expanding ) );
}

static TOKEN scanCharConst( int expanding )
{
    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    TokenLen = 1;
    return( charConst( TYP_CHAR, expanding ) );
}

static TOKEN scanNewline( int expanding )
{
    DbgAssert( _BufferOverrun == BUFFER_OVERRUN_CHECK );
    if( PPControl & PPCTL_EOL ) {
        return( T_NULL );
    }
    return( ChkControl( expanding ) );
}

static TOKEN scanCarriageReturn( int expanding )
{
    int         c;

    DbgAssert( _BufferOverrun == BUFFER_OVERRUN_CHECK );
    // under DOS-like systems, '\r' is often followed by '\n'
    // so we perform a quick check and perform the '\n' code right away
    c = NextChar();
    if( c == '\n' ) {
        return( scanNewline( expanding ) );
    }
    return( scanWhiteSpace( expanding ) );
}

static TOKEN scanInvalid( int expanding )
{
    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    Buffer[1] = '\0';
    TokenLen = 1;
#if defined( SYS_EOF_CHAR )
    if( CurrChar == SYS_EOF_CHAR ) {
        if( SrcFileClose( FALSE ) ) {
            return( ScanToken( expanding ) );
        }
        return( T_EOF );
    }
#else
    expanding = expanding;
#endif
    if( diagnose_lex_error( expanding ) ) {
        CErr2( WARN_WEIRD_CHARACTER, CurrChar );
    }
    NextChar();
    return( T_BAD_CHAR );
}

static TOKEN scanEof( int expanding )
{
    DbgAssert( _BufferOverrun == BUFFER_OVERRUN_CHECK );
    expanding = expanding;
    return( T_EOF );
}

static TOKEN (*scanFunc[])( int ) = {
    scanName,
    scanWide,
    scanNum,
    scanDelim1,
    scanDelim12,
    scanDelim12EQ,
    scanDelim12EQ2EQ,
    scanDelim1EQ,
    scanSlash,
    scanLT,
    scanPercent,
    scanColon,
    scanMinus,
    scanFloat,
    scanString,
    scanStringContinue,
    scanLStringContinue,
    scanCharConst,
    scanCarriageReturn,
    scanNewline,
    scanWhiteSpace,
    scanInvalid,
    scanEof,
};

#define dispatchFunc( _ex ) (scanFunc[ClassTable[CurrChar]]( (_ex) ))

TOKEN ScanToken( int expanding )
/******************************/
{
    return( dispatchFunc( expanding ) );
}

static void nextMacroToken( void )
{
    do {
        CurToken = T_NULL;
        if( CompFlags.use_macro_tokens ) {
            GetMacroToken( FALSE );
            if( CurToken == T_NULL ) {
                CurToken = dispatchFunc( 0 );
            }
        } else {
            CurToken = dispatchFunc( 0 );
        }
    } while( CurToken == T_WHITE_SPACE );
}


void ScanInit( void )
/*******************/
{
    uint_8  *p;

#ifndef NDEBUG
    _BufferOverrun = BUFFER_OVERRUN_CHECK;
#endif
    tokenSource = nextMacroToken;
    ReScanPtr = NULL;
    PPControl = PPCTL_NORMAL;
    CompFlags.scanning_c_comment = 0;
    memset( ClassTable, SCAN_INVALID, sizeof( ClassTable ) );
    memset( &ClassTable['A'], SCAN_NAME, 26 );
    memset( &ClassTable['a'], SCAN_NAME, 26 );
    memset( &ClassTable['0'], SCAN_NUM, 10 );
    ClassTable[LCHR_EOF] = SCAN_EOF;
    ClassTable[LCHR_CONTINUE_STRING] = SCAN_STRING2;
    ClassTable[LCHR_CONTINUE_LSTRING] = SCAN_LSTRING2;
    for( p = InitClassTable; *p != '\0'; p += 2 ) {
        ClassTable[ p[0] ] = p[1];
    }
    ExtraRptRegisterCtr( &nextTokenCalls, "NextToken calls" );
    ExtraRptRegisterCtr( &nextTokenSavedId, "NextToken T_SAVED_ID" );
    ExtraRptRegisterCtr( &nextTokenNormal, "NextToken nextMacroToken" );
    ExtraRptRegisterCtr( &lookPastRewrite, "LookPastName calls with different tokenSource setting" );
}

// called by CollectParms() to gather tokens for macro parms
// and CDefine() to gather tokens for macro definition
// example usage:
//      int ppscan_mode;
//      ppscan_mode = InitPPScan();
//      CollectParms();
//      FiniPPScan( ppscan_mode );
int InitPPScan( void )
/********************/
{
    if( scanFunc[ SCAN_NUM ] == scanNum ) {
        scanFunc[ SCAN_NUM ] = scanPPDigit;
        scanFunc[ SCAN_FLOAT ] = scanPPDot;
        return( 1 );            // indicate changed to PP mode
    }
    return( 0 );                // indicate already in PP mode
}

// called when CollectParms() and CDefine() are finished gathering tokens
void FiniPPScan( int ppscan_mode )
/********************************/
{
    if( ppscan_mode == 1 ) {    // if InitPPScan() changed into PP mode
        scanFunc[ SCAN_NUM ] = scanNum; // reset back to normal mode
        scanFunc[ SCAN_FLOAT ] = scanFloat;
    }
}

void LookPastName( void )
/***********************/
{
#ifdef XTRA_RPT
    if( tokenSource != nextMacroToken ) {
        ExtraRptIncrementCtr( lookPastRewrite );
    }
#endif
    SavedId = NameCreateLen( Buffer, TokenLen );
    NextToken();
    LAToken = CurToken;
    CurToken = T_SAVED_ID;
}

void UndoNextToken( void )
/************************/
{
    SavedId = NULL;
    LAToken = CurToken;
    CurToken = T_SAVED_ID;
}


void GetNextToken( void )       // used ONLY if generating pre-processed output
/***********************/
{
    if( CompFlags.use_macro_tokens ) {
        CurToken = T_NULL;
        GetMacroToken( FALSE );
        if( CurToken == T_NULL ) {
            // prevents macro expansion from merging with trailing text
            // to form new tokens in pre-processed output
            CurToken = T_WHITE_SPACE;
        }
        DbgAssert( CurToken != T_NULL );
    } else {
        printWhiteSpace( CurrChar );
        CurToken = ScanToken( 0 );
    }
}

boolean TokenUsesBuffer( TOKEN token )
/************************************/
{
    switch( token ) {
    case T_ID:
    case T_BAD_TOKEN:
    case T_BAD_CHAR:
    case T_STRING:
    case T_LSTRING:
    case T_CONSTANT:
        return( TRUE );
    }
    return( FALSE );
}
