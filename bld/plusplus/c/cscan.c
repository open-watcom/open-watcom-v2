/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "name.h"
#include "context.h"
#include "unicode.h"
#include "cscanbuf.h"
#ifndef NDEBUG
    #include "dbg.h"
#endif


#if defined(__DOS__) || defined(__OS2__) || defined(__NT__)
    #define SYS_EOF_CHAR    0x1A
#elif defined(__UNIX__) || defined(__RDOS__)
    #undef SYS_EOF_CHAR
#else
    #error SYS_EOF_CHAR is not set for this system
#endif

#define prt_char( x )           if( CompFlags.cpp_output ) { PrtChar( x ); }

#define diagnose_lex_error( e ) \
        (!(e) && ( SkipLevel == NestLevel ) && (PPControl & PPCTL_NO_LEX_ERRORS) == 0 )

typedef enum {
    #define pick(e,p) e,
    #include "_scnclas.h"
    #undef pick
    SCAN_MAX
} scan_class;

ExtraRptCtr( nextTokenCalls );
ExtraRptCtr( nextTokenSavedId );
ExtraRptCtr( nextTokenNormal );
ExtraRptCtr( lookPastRewrite );

static token_source_fn  *tokenSource;

static const char       *ReScanPtr;

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
    '.',        SCAN_DOT,
    '#',        SCAN_DELIM2,            // #    ##
    '=',        SCAN_DELIM2,            // = ==
    '^',        SCAN_DELIM2,            // ^ ^=
    '!',        SCAN_DELIM2,            // ! !=
    '%',        SCAN_DELIM2,            // % %= %> %: %:%:
    '*',        SCAN_DELIM2,            // * *=
    '&',        SCAN_DELIM2,            // & &= &&
    '|',        SCAN_DELIM2,            // | |= ||
    '+',        SCAN_DELIM2,            // + += ++
    '<',        SCAN_DELIM2,            // < <= << <<= <% <:
    '>',        SCAN_DELIM2,            // > >= >> >>=
    '-',        SCAN_DELIM2,            // - -= -- -> ->*
    '/',        SCAN_SLASH,             // / /=    // /**/
    ':',        SCAN_DELIM2,            // :    :: :>
    '_',        SCAN_NAME,
    'L',        SCAN_WIDE,
    '\0',       0
};

#if TARGET_INT == 2
static unsigned_64 intMax   = I64Val( 0x00000000, 0x00007fff );
static unsigned_64 uintMax  = I64Val( 0x00000000, 0x0000ffff );
#else
static unsigned_64 intMax   = I64Val( 0x00000000, 0x7fffffff );
static unsigned_64 uintMax  = I64Val( 0x00000000, 0xffffffff );
#endif

void ReScanInit( const char *ptr )
/********************************/
{
    ReScanPtr = ptr;
}

static int rescanBuffer( void )
{
    CurrChar = *(const unsigned char *)ReScanPtr++;
    if( CurrChar == '\0' ) {
        CompFlags.rescan_buffer_done = true;
    }
    return( CurrChar );
}

TOKEN ReScanToken( void )
/***********************/
{
    int saved_currchar;
    int (*saved_nextchar)(void);
    LINE_NO saved_line;
    COLUMN_NO saved_column;
    TOKEN token;

    saved_line = TokenLine;
    saved_column = TokenColumn;
    saved_currchar = CurrChar;
    saved_nextchar = NextChar;
    CompFlags.rescan_buffer_done = false;
    NextChar = rescanBuffer;
    NextChar();
    token = ScanToken( true );
    --ReScanPtr;
    CurrChar = saved_currchar;
    NextChar = saved_nextchar;
    TokenLine = saved_line;
    TokenColumn = saved_column;
    return( token );
}

token_source_fn *SetTokenSource( token_source_fn *source )
/********************************************************/
{
    token_source_fn *last_source;

    last_source = tokenSource;
    tokenSource = source;
    return( last_source );
}

void ResetTokenSource( token_source_fn *source )
/**********************************************/
{
    tokenSource = source;
}

static bool doScanHex( bool expanding )
{
    int c;
    struct {
        unsigned too_big        : 1;
        unsigned at_least_one   : 1;
    } flag;

    flag.too_big = false;
    flag.at_least_one = false;
    c = NextChar();
    while( CharSet[c] & (C_HX | C_DI) ) {
        if( U64Cnv16( &Constant64, hex_dig( c ) ) ) {
            flag.too_big = true;
        }
        flag.at_least_one = true;
        c = WriteBufferCharNextChar( c );
    }
    if( !flag.at_least_one ) {
        return( false );            /* indicate no characters matched */
    }
    if( flag.too_big ) {
        if( diagnose_lex_error( expanding ) ) {
            CErr1( WARN_CONSTANT_TOO_BIG );
        }
    }
    return( true );                 /* indicate characters were matched */
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
        if( CompFlags.cpp_keep_comments ) {
            prt_char( c );
        }
    }
}

static void doScanCComment( void )
{
    int c;
    int prev_char;
    LINE_NO start_line;

    SrcFileCurrentLocation();
    start_line = TokenLine;
    CompFlags.scanning_c_comment = true;
    if( CompFlags.cpp_output ) {
        prt_comment_char( '/' );
        prt_comment_char( '*' );
    }
    c = '\0';
    for(;;) {
        for(;;) {
            prev_char = c;
            c = NextChar();
            if( c == LCHR_EOF )
                break;
            if( CompFlags.cpp_output ) {
                prt_comment_char( c );
            }
            if( c == '/' ) {
                break;
            }
        }
        if( c == LCHR_EOF )
            break;
        if( prev_char == '*' )
            break;
        // get next character and see if it is '*' for nested comment
        c = NextChar();
        if( c == LCHR_EOF )
            break;
        if( CompFlags.cpp_output ) {
            prt_comment_char( c );
        }
        if( c == '*' ) {
            /* '*' may be just about to close this comment */
            c = NextChar();
            if( c == LCHR_EOF )
                break;
            if( CompFlags.cpp_output ) {
                prt_comment_char( c );
            }
            if( c == '/' )
                break;
            SrcFileSetErrLoc();
            CErr2( WARN_NESTED_COMMENT, start_line );
        }
    }
    CompFlags.scanning_c_comment = false;
    if( c != LCHR_EOF ) {
        NextChar();
    }
}

static void doScanCppComment( void )
{
    int c;

    CompFlags.scanning_cpp_comment = true;
    if( CompFlags.cpp_output ) {
        prt_comment_char( '/' );
        prt_comment_char( '/' );
    }
    for(;;) {
        c = NextChar();
        if( c == LCHR_EOF )
            break;
        if( c == '\n' )
            break;
        if( CompFlags.cpp_output ) {
            prt_comment_char( c );
        }
    }
    CompFlags.scanning_cpp_comment = false;
}

static int doESCChar( int c, bool expanding, type_id char_type )
{
    unsigned    n;
    unsigned    i;
    int         classification;
    int         c1;

    classification = classify_escape_char( c );
    if( classification == ESCAPE_OCTAL ) {
        n = 0;
        for( i = 3; i > 0; --i ) {
            c1 = octal_dig( c );
            if( c1 == 8 )
                break;
            n = n * 8 + c1;
            c = WriteBufferCharNextChar( c );
        }
        if( n > 0377 && char_type != TYP_WCHAR ) {
            if( diagnose_lex_error( expanding ) ) {
                SrcFileSetErrLoc();
                CErr1( ANSI_INVALID_OCTAL_ESCAPE );
            }
            n &= 0377;
        }
    } else if( classification == ESCAPE_HEX ) {
        WriteBufferChar( c );
        U64Clear( Constant64 );
        if( doScanHex( expanding ) ) {
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
        WriteBufferCharNextChar( c );
    } else {
        n = classification;
        WriteBufferCharNextChar( c );
    }
    return( n );
}

static TOKEN doScanCharConst( type_id char_type, bool expanding )
{
    int c;
    int i;
    TOKEN token;
    int value;
    struct {
        unsigned double_byte_char   : 1;
    } flag;
    bool ok;

    ok = false;
    flag.double_byte_char = false;
    value = 0;
    i = 0;
    c = NextChar();
    if( c == '\'' ) {
        Buffer[TokenLen++] = c;
        NextChar();
    } else {
        for( ;; ) {
            if( c == '\r' || c == '\n' || c == LCHR_EOF ) {
                break;
            }
            Buffer[TokenLen++] = c;
            if( c == '\\' ) {
                NextChar();
                c = doESCChar( CurrChar, expanding, char_type );
                if( char_type == TYP_WCHAR ) {
                    ++i;
                    value = (value << 8) + ((c & 0xFF00) >> 8);
                    c &= 0x00FF;
                }
            } else {
                if( CharSet[c] & C_DB ) {   /* if double-byte char */
                    NextChar();
                    Buffer[TokenLen++] = CurrChar;
                    c = (c << 8) + (CurrChar & 0x00FF);
                    if( char_type == TYP_WCHAR ) {
                        if( CompFlags.jis_to_unicode ) {
                            c = JIS2Unicode( c );
                        }
                    }
                    ++i;
                    value = (value << 8) + ((c & 0xFF00) >> 8);
                    c &= 0x00FF;
                    flag.double_byte_char = true;
                } else if( char_type == TYP_WCHAR ) {
                    if( CompFlags.use_unicode ) {
                        c = UniCode[c];
                    } else if( CompFlags.jis_to_unicode ) {
                        c = JIS2Unicode( c );
                    }
                    ++i;
                    value = (value << 8) + ((c & 0xFF00) >> 8);
                    c &= 0x00FF;
                }
                NextChar();
            }
            ++i;
            value = (value << 8) + c;
            /* handle case where user wants a \ but doesn't escape it */
            if( c == '\'' && CurrChar != '\'' ) {
                if( !CompFlags.cpp_output ) {
                    break;
                }
            }
            c = CurrChar;
            if( c == '\'' ) {
                ok = true;
                break;
            }
            if( i >= 4 ) {
                if( !CompFlags.cpp_output ) {
                    break;
                }
            }
        }
    }
    if( ok ) {
        Buffer[TokenLen++] = c;
        NextChar();
        token = T_CONSTANT;
    } else {
        BadTokenInfo = ERR_INV_CHAR_CONSTANT;   /* in case of error */
        token = T_BAD_TOKEN;
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
        if(( CharSet[c] & C_WS) == 0 )
            break;
        if( c != '\r' ) {
            PrtChar( c );
        }
        c = NextChar();
    }
    return( c );
}

static TOKEN scanWhiteSpace( bool expanding )
{
    SrcFileScanWhiteSpace( expanding );
    return( T_WHITE_SPACE );
}

static int skipWhiteSpace( int c )
{
    if( CompFlags.cpp_output && (PPControl & PPCTL_EOL) == 0 ) {
        c = printWhiteSpace( c );
    } else {
        SrcFileScanWhiteSpace( false );
        c = CurrChar;
    }
    return( c );
}

static void unGetChar( int c )
{
    if( NextChar == rescanBuffer ) {
        --ReScanPtr;
        CompFlags.rescan_buffer_done = false;
    } else {
        GetNextCharUndo( c );
    }
}

bool DoScanOptionalComment( void )
/********************************/
{
    bool ok;
    int c;

    ok = false;
    for(;;) {
        c = CurrChar;
        if( c != '/' )
            break;
        c = NextChar();
        if( c == '*' ) {
            doScanCComment();
            ok = true;
        } else if( c == '/' ) {
            doScanCppComment();
            ok = true;
        } else {
            unGetChar( c );
            CurrChar = '/';
            break;
        }
    }
    return( ok );
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
            if( c != '\n' )
                break;
            if( (PPControl & PPCTL_EOL) == 0 ) {
                prt_char( '\n' );
            }
            c = NextChar();
        }
        if( c != '/' )
            break;
        if( !DoScanOptionalComment() ) {
            break;
        }
    }
}

static TOKEN doScanDotSomething( int c )
{
    TOKEN   token;

    if( c == '.' ) {
        c = NextChar();
        if( c == '.' ) {
            Buffer[TokenLen++] = c;
            Buffer[TokenLen++] = c;
            token = T_DOT_DOT_DOT;
        } else {
            unGetChar( c );
            CurrChar = '.';
            token = T_DOT;
        }
    } else if( c == '*' ) {
        Buffer[TokenLen++] = c;
        token = T_DOT_STAR;
    } else {
        token = T_DOT;
    }
    if( token != T_DOT )
        NextChar();
    Buffer[TokenLen] = '\0';
    return( token );
}

static TOKEN doScanFloat( void )
{
    int c;
    TOKEN token;

    c = CurrChar;
    if( c == '.' ) {
        c = WriteBufferCharNextChar( c );
        while( CharSet[c] & C_DI ) {
            c = WriteBufferCharNextChar( c );
        }
        if( TokenLen == 1 ) {       /* .? */
            return( doScanDotSomething( c ) );
        }
    }
    token = T_CONSTANT;
    if( ONE_CASE_EQUAL( c, 'E' ) ) {
        c = WriteBufferCharNextChar( c );
        if( c == '+' || c == '-' ) {
            c = WriteBufferCharNextChar( c );
        }
        if( (CharSet[c] & C_DI) == 0 ) {
            token = T_BAD_TOKEN;
            BadTokenInfo = ERR_INVALID_FLOATING_POINT_CONSTANT;
        }
        while( CharSet[c] & C_DI ) {
            c = WriteBufferCharNextChar( c );
        }
    }
    if( ONE_CASE_EQUAL( c, 'F' ) ) {
        c = WriteBufferCharNextChar( c );
        ConstType = TYP_FLOAT;
    } else if( ONE_CASE_EQUAL( c, 'L' ) ) {
        c = WriteBufferCharNextChar( c );
        ConstType = TYP_LONG_DOUBLE;
    } else {
        ConstType = TYP_DOUBLE;
    }
    if( (PPControl & PPCTL_ASM) && (CharSet[c] & (C_AL | C_DI)) ) {
        while( CharSet[c] & (C_AL | C_DI) ) {
            c = WriteBufferCharNextChar( c );
        }
        BadTokenInfo = ERR_INVALID_FLOATING_POINT_CONSTANT;
        token = T_BAD_TOKEN;
    }
    WriteBufferNullChar();
    return( token );
}

static TOKEN doScanString( type_id string_type, bool expanding )
{
    int c;
    bool ok;

    SrcFileSetSwEnd( true );
    ok = false;
    TokenLen = 0;
    c = NextChar();
    for(;;) {
        if( c == '\n' )
            break;
        if( c == LCHR_EOF )
            break;
        if( c == '"' ) {
            ok = true;
            break;
        }
        if( c == '\\' ) {
            c = WriteBufferCharNextChar( c );
            if(( CharSet[c] & C_WS ) == 0 ) {
                doESCChar( c, expanding, string_type );
                c = CurrChar;
            }
        } else {
            if( CharSet[c] & C_DB ) {
                // if first char of a double-byte char, grab next one
                c = WriteBufferCharNextChar( c );
            }
            c = WriteBufferCharNextChar( c );
        }
    }
    SrcFileSetSwEnd( false );
    WriteBufferNullChar();
    if( ok ) {
        NextChar();
        if( string_type == TYP_WCHAR ) {
            return( T_LSTRING );
        }
        return( T_STRING );
    }
    BadTokenInfo = ERR_MISSING_QUOTE;
    return( T_BAD_TOKEN );
}

static TOKEN doScanName( int c, bool expanding )
{
    MEPTR   mentry;
    TOKEN   token;

    SrcFileScanName( c );
    if( expanding || (PPControl & PPCTL_NO_EXPAND) )
        return( T_ID );

    mentry = MacroLookup( Buffer, TokenLen );
    if( mentry == NULL ) {
        if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
            token = Process_Pragma( false );
        } else {
            token = KwLookup( TokenLen );
        }
        return( token );
    }
    prt_char( ' ' );
    if( MacroIsSpecial( mentry ) )
        return( SpecialMacro( mentry ) );
    mentry->macro_flags |= MFLAG_REFERENCED;
    /* if macro requires parameters and next char is not a '('
    then this is not a macro */
    if( MacroWithParenthesis( mentry ) ) {
        SkipAhead();
        if( CurrChar != '(' ) {
            if( CompFlags.cpp_output ) {
                InsertToken( T_WHITE_SPACE, " ", false );
                return( T_ID );
            }
            if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
                token = Process_Pragma( false );
            } else {
                token = KwLookup( TokenLen );
            }
            return( token );
        }
    }
    DoMacroExpansion( mentry );
    token = GetMacroToken( false );
    if( token == T_NULL ) {
        token = T_WHITE_SPACE;
    }
    return( token );
}

static TOKEN scanName( bool expanding )
{
    int c;

    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    TokenLen = 1;
    c = NextChar();
    return( doScanName( c, expanding ) );
}

static TOKEN doScanAsmToken( void )
{
    NextChar();
    while( SrcFileScanName( CurrChar ) == '.' ) {
        WriteBufferCharNextChar( '.' );
    }
    WriteBufferNullChar();
    return( T_ID );
}

static TOKEN scanWide( bool expanding )  // scan something that starts with L
{
    int c;
    TOKEN token;

    SrcFileCurrentLocation();
    c = NextChar();
    if( c == '"' ) {                    // L"abc"
        token = doScanString( TYP_WCHAR, expanding );
    } else {
        Buffer[0] = 'L';
        if( c == '\'' ) {               // L'a'
            Buffer[1] = '\'';
            TokenLen = 2;
            token = doScanCharConst( TYP_WCHAR, expanding );
        } else {                        // regular identifier
            TokenLen = 1;
            token = doScanName( c, expanding );
        }
    }
    return( token );
}

static void msIntSuffix( uint_32 signed_max, type_id sid, type_id uid, unsigned_64 *max_value )
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

static TOKEN doScanNum( bool expanding )
{
    int c;
    unsigned_64 *max_value;
    char too_big;
    char max_digit;

    U64Clear( Constant64 );
    too_big = 0;
    if( CurrChar == '0' ) {
        c = NextChar();
        if( ONE_CASE_EQUAL( c, 'X' ) ) {
            WriteBufferChar( c );
            if( doScanHex( expanding ) ) {
                c = CurrChar;       /* get next character */
            } else {
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_HEX_CONSTANT );
                } else {
                    unGetChar( CurrChar );      /* put character after 'x' back */
                    CurrChar = c;               /* set current character to 'x' */
                    --TokenLen;                 /* remove character 'x' from Buffer */
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
                c = WriteBufferCharNextChar( c );
            }
            switch( c ) {       /* could be front of a float constant */
            case '8':           /* 0128.3 */
            case '9':           /* 0129.3 */
                while( CharSet[c] & C_DI ) {
                    c = WriteBufferCharNextChar( c );
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
        // we know 'CurrChar' is a digit
        U32ToU64( CurrChar - '0', &Constant64 );
        c = NextChar();
        while( CharSet[c] & C_DI ) {
            if( U64Cnv10( &Constant64, c - '0' ) ) {
                too_big = 1;
            }
            c = WriteBufferCharNextChar( c );
        }
        if( c == '.' || ONE_CASE_EQUAL( c, 'E' ) ) {
            return( doScanFloat() );
        }
        max_value = &intMax;
    }
    switch( ONE_CASE( c ) ) {
    case ONE_CASE( 'i' ):
        ConstType = TYP_SINT;
        c = WriteBufferCharNextChar( c );
        switch( c ) {
        case '6':
            c = WriteBufferCharNextChar( c );
            if( c == '4' ) {
                c = WriteBufferCharNextChar( c );
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
            c = WriteBufferCharNextChar( c );
            if( c == '6' ) {
                c = WriteBufferCharNextChar( c );
                msIntSuffix( 0x00007fff, TYP_SSHORT, TYP_USHORT, max_value );
            } else {
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                }
            }
            break;
        case '3':
            c = WriteBufferCharNextChar( c );
            if( c == '2' ) {
                c = WriteBufferCharNextChar( c );
                msIntSuffix( 0x7fffffff, TYP_SLONG, TYP_ULONG, max_value );
            } else {
                if( diagnose_lex_error( expanding ) ) {
                    CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                }
            }
            break;
        case '8':
            c = WriteBufferCharNextChar( c );
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
        c = WriteBufferCharNextChar( c );
        if( ONE_CASE_EQUAL( c, 'u' ) ) {
            ConstType = TYP_ULONG;
            c = WriteBufferCharNextChar( c );
        } else if( ONE_CASE_EQUAL( c, 'L' ) ) {
            c = WriteBufferCharNextChar( c );
            if( ONE_CASE_EQUAL( c, 'u' ) ) {
                ConstType = TYP_ULONG64;
                c = WriteBufferCharNextChar( c );
            } else {
                ConstType = TYP_SLONG64;
            }
        } else if( !U64IsI32( Constant64 ) ) { // Constant > 0x7FFFFFFFul
            ConstType = TYP_ULONG;
        }
        if( !U64IsU32( Constant64 ) ) {
            if( U64IsI64( Constant64 ) ) {
                ConstType = TYP_SLONG64;
            } else {
                ConstType = TYP_ULONG64;
            }
        }
        break;
    case ONE_CASE( 'u' ):
        c = WriteBufferCharNextChar( c );
        switch( ONE_CASE( c ) ) {
        case ONE_CASE( 'i' ):
            ConstType = TYP_ULONG64;
            c = WriteBufferCharNextChar( c );
            switch( c ) {
            case '6':
                c = WriteBufferCharNextChar( c );
                if( c == '4' ) {
                    c = WriteBufferCharNextChar( c );
                } else {
                    if( diagnose_lex_error( expanding ) ) {
                        CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                    }
                    unGetChar( c );
                }
                break;
            case '1':
                c = WriteBufferCharNextChar( c );
                if( c == '6' ) {
                    c = WriteBufferCharNextChar( c );
                    msIntSuffix( 0x00007fff, TYP_USHORT, TYP_USHORT, &uintMax );
                } else {
                    if( diagnose_lex_error( expanding ) ) {
                        CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                    }
                    unGetChar( c );
                }
                break;
            case '3':
                c = WriteBufferCharNextChar( c );
                if( c == '2' ) {
                    c = WriteBufferCharNextChar( c );
                    msIntSuffix( 0x7fffffff, TYP_ULONG, TYP_ULONG, &uintMax );
                } else {
                    if( diagnose_lex_error( expanding ) ) {
                        CErr1( ERR_INVALID_CONSTANT_SUFFIX );
                    }
                    unGetChar( c );
                }
                break;
            case '8':
                c = WriteBufferCharNextChar( c );
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
            c = WriteBufferCharNextChar( c );
            if( ONE_CASE_EQUAL( c, 'L' ) ) {
                c = WriteBufferCharNextChar( c );
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
        while( CharSet[c] & (C_AL | C_DI) ) {
            c = WriteBufferCharNextChar( c );
        }
        WriteBufferNullChar();
        return( T_BAD_TOKEN );
    }
    WriteBufferNullChar();
    return( T_CONSTANT );
}

static TOKEN scanNum( bool expanding )
{
    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    TokenLen = 1;
    if( PPControl & PPCTL_ASM ) {
        return( doScanAsmToken() );
    } else {
        return( doScanNum( expanding ) );
    }
}

static TOKEN scanDelim1( bool expanding )
{
    TOKEN token;

    /* unused parameters */ (void)expanding;

    SrcFileCurrentLocation();
    token = TokValue[CurrChar];
    Buffer[0] = CurrChar;
    Buffer[1] = '\0';
    TokenLen = 1;
    NextChar();
    return( token );
}

static bool checkDelim2( TOKEN *token, TOKEN last )
/*************************************************/
{
    switch( *token ) {
    case T_AND:
        if( last == T_AND ) {           /* && */
            *token = T_AND_AND;
            break;
        }
        if( last == T_EQUAL ) {         /* &= */
            *token = T_AND_EQUAL;
            break;
        }
        return( false );
    case T_PLUS:
        if( last == T_PLUS ) {          /* ++ */
            *token = T_PLUS_PLUS;
            break;
        }
        if( last == T_EQUAL ) {         /* += */
            *token = T_PLUS_EQUAL;
            break;
        }
        return( false );
    case T_MINUS:
        if( last == T_MINUS ) {         /* -- */
            *token = T_MINUS_MINUS;
            break;
        }
        if( last == T_EQUAL ) {         /* -= */
            *token = T_MINUS_EQUAL;
            break;
        }
        if( last == T_GT ) {            /* -> */
            *token = T_ARROW;
            break;
        }
        return( false );
    case T_OR:
        if( last == T_OR ) {            /* || */
            *token = T_OR_OR;
            break;
        }
        if( last == T_EQUAL ) {         /* |= */
            *token = T_OR_EQUAL;
            break;
        }
        return( false );
    case T_LT:
        if( last == T_LT ) {            /* << */
            *token = T_LSHIFT;
            break;
        }
        if( last == T_EQUAL ) {         /* <= */
            *token = T_LE;
            break;
        }
        if( last == T_PERCENT ) {       /* <% */
            *token = T_ALT_LEFT_BRACE;
            break;
        }
        if( last == T_COLON ) {         /* <: */
            *token = T_ALT_LEFT_BRACKET;
            break;
        }
        return( false );
    case T_GT:
        if( last == T_GT ) {            /* >> */
            *token = T_RSHIFT;
            break;
        }
        if( last == T_EQUAL ) {         /* >= */
            *token = T_GE;
            break;
        }
        return( false );
    case T_SHARP:
        if( last == T_SHARP ) {         /* ## */
            *token = T_SHARP_SHARP;
            break;
        }
        return( false );
    case T_EQUAL:
        if( last == T_EQUAL ) {         /* == */
            *token = T_EQ;
            break;
        }
        return( false );
    case T_EXCLAMATION:
        if( last == T_EQUAL ) {         /* != */
            *token = T_NE;
            break;
        }
        return( false );
    case T_PERCENT:
        if( last == T_EQUAL ) {         /* %= */
            *token = T_PERCENT_EQUAL;
            break;
        }
        if( last == T_GT ) {            /* %> */
            *token = T_ALT_RIGHT_BRACE;
            break;
        }
        if( last == T_COLON ) {         /* %: */
            *token = T_ALT_SHARP;
            break;
        }
        return( false );
    case T_TIMES:
        if( last == T_EQUAL ) {         /* *= */
            *token = T_TIMES_EQUAL;
            break;
        }
        return( false );
    case T_DIV:
        if( last == T_EQUAL ) {         /* /= */
            *token = T_DIV_EQUAL;
            break;
        }
        return( false );
    case T_XOR:
        if( last == T_EQUAL ) {         /* ^= */
            *token = T_XOR_EQUAL;
            break;
        }
        return( false );
    case T_COLON:
        if( last == T_COLON ) {         /* :: */
            *token = T_COLON_COLON;
            break;
        }
        if( last == T_GT ) {            /* :> */
            // TODO: according to the standard, ":>" should be an
            // alternative token (digraph) for "]"
            // *token = T_RIGHT_BRACKET;   /* -> ] */
            *token = T_SEG_OP;
            break;
        }
        return( false );
    case T_ARROW:
        if( last == T_TIMES ) {         /* ->* */
            *token = T_ARROW_STAR;
            break;
        }
        return( false );
    case T_LSHIFT:
        if( last == T_EQUAL ) {         /* <<= */
            *token = T_LSHIFT_EQUAL;
            break;
        }
        return( false );
    case T_RSHIFT:
        if( last == T_EQUAL ) {         /* >>= */
            *token = T_RSHIFT_EQUAL;
            break;
        }
        return( false );
    case T_ALT_SHARP:
        if( last == T_PERCENT ) {       /* %:% */
            break;
        }
        return( false );
    default:
        return( false );
    }
    return( true );
}

static TOKEN scanDelim2( bool expanding )
/****************************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    TOKEN           token;
    int             chr3;

    /* unused parameters */ (void)expanding;

    SrcFileCurrentLocation();
    token = TokValue[CurrChar];
    Buffer[0] = CurrChar;
    TokenLen = 1;
    if( (CharSet[NextChar()] & C_DE) && checkDelim2( &token, TokValue[CurrChar] ) ) {
        Buffer[TokenLen++] = CurrChar;
        if( (CharSet[NextChar()] & C_DE) && checkDelim2( &token, TokValue[CurrChar] ) ) {
            if( token == T_ALT_SHARP ) {
                /* only '%:%:' is possible for 4 characters delimiter
                 * if it matches only '%:%' (3 characters)
                 * then push back character 3 and 4
                 */
                chr3 = CurrChar;
                if( (CharSet[NextChar()] & C_DE) && TokValue[CurrChar] == T_COLON ) {
                    Buffer[TokenLen++] = chr3;
                    Buffer[TokenLen++] = CurrChar;
                    NextChar();
                } else {
                    unGetChar( CurrChar );
                    CurrChar = chr3;
                }
            } else {
                Buffer[TokenLen++] = CurrChar;
                NextChar();
            }
        }
    }
    Buffer[TokenLen] = '\0';
    return( token );
}

static TOKEN scanSlash( bool expanding ) // /, /=, // comment, or /*comment*/
{
    int c;
    TOKEN token;

    SrcFileCurrentLocation();
    token = T_DIV;
    Buffer[0] = '/';
    TokenLen = 1;
    c = NextChar();
    if( c == '=' ) {
        Buffer[TokenLen++] = '=';
        NextChar();
        token = T_DIV_EQUAL;
    } else if( !expanding ) {
        if( c == '/' ) {
            // C++ comment
            doScanCppComment();
            Buffer[0] = ' ';
            token = T_WHITE_SPACE;
        } else if( c == '*' ) {
            // C comment
            doScanCComment();
            Buffer[0] = ' ';
            token = T_WHITE_SPACE;
        }
    }
    Buffer[TokenLen] = '\0';
    return( token );
}

static TOKEN scanDot( bool expanding )
{
    /* unused parameters */ (void)expanding;

    SrcFileCurrentLocation();
    if( PPControl & PPCTL_ASM ) {
        Buffer[0] = '.';
        TokenLen = 1;
        return( doScanAsmToken() );
    } else {
        TokenLen = 0;
        return( doScanFloat() );
    }
}

static TOKEN doScanPPNumber( void )
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
    for( ;; ) {
        prevc = c;
        c = NextChar();
        if( c == '.' || (CharSet[c] & (C_AL | C_DI)) ) {
            WriteBufferChar( c );
        } else if( ONE_CASE_EQUAL( prevc, 'e' ) && ( c == '+' || c == '-' ) ) {
            WriteBufferChar( c );
            if( CompFlags.extensions_enabled ) {
                /* concession to existing practice...
                    #define A2 0x02
                    #define A3 0xaa0e+A2
                    // users want: 0xaa0e + 0x02
                    // not: 0xaa0e + A2 (but, this is what ISO C requires!)

                    after the 'sign', we bail out if we don't find a digit
                 */
                prevc = c;
                c = NextChar();
                if(( CharSet[c] & C_DI ) == 0 ) {
                    // this check will kick out on a '+' and '-' also
                    break;
                }
                WriteBufferChar( c );
            }
        } else {
            break;
        }
    }
    WriteBufferNullChar();
    return( T_PPNUMBER );
}

static TOKEN scanPPDigit( bool expanding )
{
    /* unused parameters */ (void)expanding;

    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    TokenLen = 1;
    return( doScanPPNumber() );
}

static TOKEN scanPPDot( bool expanding )
{
    int         c;

    /* unused parameters */ (void)expanding;

    SrcFileCurrentLocation();
    Buffer[0] = '.';
    TokenLen = 1;
    c = NextChar();
    if( c >= '0' && c <= '9' ) {
        WriteBufferChar( c );
        return( doScanPPNumber() );
    } else {
        return( doScanDotSomething( c ) );
    }
}

static TOKEN scanString( bool expanding )
{
    SrcFileCurrentLocation();
    return( doScanString( TYP_CHAR, expanding ) );
}

static TOKEN scanCharConst( bool expanding )
{
    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    TokenLen = 1;
    return( doScanCharConst( TYP_CHAR, expanding ) );
}

static TOKEN scanNewline( bool expanding )
{
    if( PPControl & PPCTL_EOL ) {
        return( T_NULL );
    }
    return( ChkControl( expanding ) );
}

static TOKEN scanCarriageReturn( bool expanding )
{
    int         c;

    // under DOS-like systems, '\r' is often followed by '\n'
    // so we perform a quick check and perform the '\n' code right away
    c = NextChar();
    if( c == '\n' ) {
        return( scanNewline( expanding ) );
    }
    return( scanWhiteSpace( expanding ) );
}

static TOKEN scanInvalid( bool expanding )
{
    SrcFileCurrentLocation();
    Buffer[0] = CurrChar;
    Buffer[1] = '\0';
    TokenLen = 1;
#if defined( SYS_EOF_CHAR )
    if( CurrChar == SYS_EOF_CHAR ) {
        if( SrcFileClose( false ) ) {
            return( ScanToken( expanding ) );
        }
        return( T_EOF );
    }
#endif
    if( diagnose_lex_error( expanding ) ) {
        CErr2( WARN_WEIRD_CHARACTER, CurrChar );
    }
    NextChar();
    return( T_BAD_CHAR );
}

static TOKEN scanEof( bool expanding )
{
    /* unused parameters */ (void)expanding;

    return( T_EOF );
}

static TOKEN (*scanFunc[])( bool ) = {
    #define pick(e,p) p,
    #include "_scnclas.h"
    #undef pick
};

#define dispatchFunc( _ex ) (scanFunc[ClassTable[CurrChar]]( (_ex) ))

TOKEN ScanToken( bool expanding )
/*******************************/
{
    return( dispatchFunc( expanding ) );
}

static void nextMacroToken( void )
{
    do {
        if( CompFlags.use_macro_tokens ) {
            CurToken = GetMacroToken( false );
            if( CurToken == T_NULL ) {
                CurToken = dispatchFunc( false );
            }
        } else {
            CurToken = dispatchFunc( false );
        }
    } while( CurToken == T_WHITE_SPACE );
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

void ScanInit( void )
/*******************/
{
    uint_8  *p;

    tokenSource = nextMacroToken;
    ReScanPtr = NULL;
    PPControl = PPCTL_NORMAL;
    CompFlags.scanning_c_comment = false;
    memset( ClassTable, SCAN_INVALID, sizeof( ClassTable ) );
    memset( &ClassTable['A'], SCAN_NAME, 26 );
    memset( &ClassTable['a'], SCAN_NAME, 26 );
    memset( &ClassTable['0'], SCAN_NUM, 10 );
    ClassTable[LCHR_EOF] = SCAN_EOF;
    for( p = InitClassTable; *p != '\0'; p += 2 ) {
        ClassTable[p[0]] = p[1];
    }
    ExtraRptRegisterCtr( &nextTokenCalls, "NextToken calls" );
    ExtraRptRegisterCtr( &nextTokenSavedId, "NextToken T_SAVED_ID" );
    ExtraRptRegisterCtr( &nextTokenNormal, "NextToken nextMacroToken" );
    ExtraRptRegisterCtr( &lookPastRewrite, "LookPastName calls with different tokenSource setting" );
}

// called by CollectParms() to gather tokens for macro parms
// and CDefine() to gather tokens for macro definition
// example usage:
//      bool ppscan_mode;
//      ppscan_mode = InitPPScan();
//      CollectParms();
//      FiniPPScan( ppscan_mode );
bool InitPPScan( void )
/*********************/
{
    if( scanFunc[SCAN_NUM] == scanNum ) {
        scanFunc[SCAN_NUM] = scanPPDigit;
        scanFunc[SCAN_DOT] = scanPPDot;
        return( true );         // indicate changed to PP mode
    }
    return( false );            // indicate already in PP mode
}

// called when CollectParms() and CDefine() are finished gathering tokens
void FiniPPScan( bool ppscan_mode )
/*********************************/
{
    if( ppscan_mode ) {                 // if InitPPScan() changed into PP mode
        scanFunc[SCAN_NUM] = scanNum; // reset back to normal mode
        scanFunc[SCAN_DOT] = scanDot;
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
        CurToken = GetMacroToken( false );
        if( CurToken == T_NULL ) {
            // prevents macro expansion from merging with trailing text
            // to form new tokens in pre-processed output
            CurToken = T_WHITE_SPACE;
        }
        DbgAssert( CurToken != T_NULL );
    } else {
        printWhiteSpace( CurrChar );
        CurToken = ScanToken( false );
    }
}

bool TokenUsesBuffer( TOKEN token )
/*********************************/
{
    switch( token ) {
    case T_ID:
    case T_BAD_TOKEN:
    case T_BAD_CHAR:
    case T_STRING:
    case T_LSTRING:
    case T_CONSTANT:
        return( true );
    }
    return( false );
}
