/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  C lexical analyzer. Closely coupled with preprocessor.
*
****************************************************************************/


#include "cvars.h"
#include "scan.h"
#include "escchars.h"
#include "asciiout.h"
#include "i64.h"
#include "kwhash.h"
#include "unicode.h"
#include "cmacadd.h"
#include "cscanbuf.h"


#define diagnose_lex_error()    ((SkipLevel == NestLevel) && (PPControl & PPCTL_NO_LEX_ERRORS) == 0)

#define HEXBIN(c)               ((CharSet[c] & C_HX) ? HEX2BIN((c)) : DEC2BIN((c)))
#define OCTAL(c)                ((c) >= '0' && (c) <= '7')

enum scan_class {
    #define pick(e,p) e,
    #include "_scnclas.h"
    #undef pick
};

extern const unsigned char  TokValue[];

static FCB              rescan_fcb;
#ifdef CHAR_MACRO
static int              SavedCurrChar;      /* used when get tokens from macro */
#endif
static unsigned char    ClassTable[LCHR_MAX];

static struct {
    unsigned short  chr;
    unsigned char   cls;
} InitClassTable[] = {
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
    '/',        SCAN_SLASH,
    '-',        SCAN_DELIM2,        // -, -=, --, ->
    '=',        SCAN_DELIM2,        // =, ==
    ':',        SCAN_DELIM2,        // :, :>
    '*',        SCAN_DELIM2,        // *, *=
    '[',        SCAN_DELIM1,
    ']',        SCAN_DELIM1,
    '{',        SCAN_DELIM1,
    '}',        SCAN_DELIM1,
    '~',        SCAN_DELIM1,
    '.',        SCAN_DOT,
    '!',        SCAN_DELIM2,        // !, !=
    '#',        SCAN_DELIM2,        // #, ##
    '%',        SCAN_DELIM2,        // %, %=
    '&',        SCAN_DELIM2,        // &, &=, &&
    '+',        SCAN_DELIM2,        // +, +=, ++
    '<',        SCAN_DELIM2,        // <, <=, <<, <<=, <:
    '>',        SCAN_DELIM2,        // >, >=, >>, >>=
    '^',        SCAN_DELIM2,        // ^, ^=
    '|',        SCAN_DELIM2,        // |, |=, ||
    '_',        SCAN_NAME,
    'L',        SCAN_WIDE,
    LCHR_EOF,   SCAN_EOF,
#ifdef CHAR_MACRO
    LCHR_MACRO, SCAN_MACRO,
#endif
    '\0',       0
};


void NewLineStartPos( FCB *srcfile )
/**********************************/
{
    srcfile->src_line_cnt++;
    srcfile->src_loc.line++;
    srcfile->src_loc.column = 0;
}

void ReScanInit( const char *ptr )
/********************************/
{
    rescan_fcb.src_ptr = (const unsigned char *)ptr;
}

const char *ReScanPos( void )
/***************************/
{
    return( (const char *)rescan_fcb.src_ptr );
}

static int reScanGetNextChar( void )
/**********************************/
{
    CurrChar = *rescan_fcb.src_ptr++;
    if( CurrChar == '\0' ) {
        CompFlags.rescan_buffer_done = true;
    }
    return( CurrChar );
}

static int reScanGetCharCheck( int c )
/************************************/
{
    if( c == '\0' ) {
        CompFlags.rescan_buffer_done = true;
    }
    return( c );
}

static void reScanGetNextCharUndo( int c )
/****************************************/
{
    /* unused parameters */ (void)c;

    rescan_fcb.src_ptr--;
    CompFlags.rescan_buffer_done = false;
}

static unsigned hashpjw( const char *s, size_t len )
/**************************************************/
{
    unsigned        h;
    size_t          i;

    h = *(const unsigned char *)s++;
    if( len > 1 ) {
        h = ( ( h << 4 ) + *(const unsigned char *)s++ ) & 0x0fff;
        for( i = 2; i < len; i++) {
            h = ( h << 4 ) + *(const unsigned char *)s++;
            h = ( h ^ ( h >> 12 ) ) & 0x0fff;
        }
    }
    return( h );
}

id_hash_idx CalcHashID( const char *id )
/**************************************/
{
    unsigned    hash;

    if( *id == '\0' )
        return( 0 );
    hash = hashpjw( id, strlen( id ) );
    return( (id_hash_idx)( hash % ID_HASH_SIZE ) );
}

mac_hash_idx CalcHashMacro( const char *id )
/******************************************/
{
    unsigned    hash;

    if( *id == '\0' )
        return( 0 );
    hash = hashpjw( id, strlen( id ) );
#if ( MACRO_HASH_SIZE > 0x0ff0 ) && ( MACRO_HASH_SIZE < 0x0fff )
    if( hash >= MACRO_HASH_SIZE ) {
        hash -= MACRO_HASH_SIZE;
    }
#else
    hash = hash % MACRO_HASH_SIZE;
#endif
    return( (mac_hash_idx)hash );
}

str_hash_idx CalcStringHash( STR_HANDLE lit )
/*******************************************/
{
    if( lit->length == 0 )
        return( 0 );
    return( (str_hash_idx)( hashpjw( lit->literal, lit->length ) % STRING_HASH_SIZE ) );
}

TOKEN KwLookup( const char *buf, size_t len )
/*******************************************/
{
    const char  *keyword;
    TOKEN       token;

    token = keyword_hash( buf, TokValue, len ) + FIRST_KEYWORD;
    /*
     * look up id in keyword table
     */
    switch( token ) {
    case T__BOOL:
    case T_INLINE:
        if( CompVars.cstd < STD_C99
          && !CompFlags.extensions_enabled )
            return( T_ID );
        break;
    case T__COMPLEX:
    case T_RESTRICT:
    case T__IMAGINARY:
    case T___OW_IMAGINARY_UNIT:
        if( CompVars.cstd < STD_C99 ) {
            return( T_ID );
        }
        break;
    case T__NORETURN:
        if( CompVars.cstd >= STD_C23
          || CompVars.cstd < STD_C11
          && !CompFlags.extensions_enabled )
            return( T_ID );
        break;
    }
    keyword = Tokens[token];
    if( *keyword == buf[0] ) {
        if( strcmp( keyword, buf ) == 0 ) {
            return( token );
        }
    }
    /*
     * not in keyword table, so must be just an identifier
     */
    return( T_ID );
}

static int getIDName( int c )
/***************************/
{
    while( CharSet[c] & (C_AL | C_DI) ) {
        while( CharSet[c] & (C_AL | C_DI) ) {
            WriteBufferChar( c );
            c = *SrcFiles->src_ptr++;
        }
        if( (CharSet[c] & C_EX) == 0 )
            break;
        c = GetCharCheck( c );
    }
    CurrChar = c;
    return( c );
}

static TOKEN doScanName( void )
/*****************************/
{
    TOKEN       token;
    MEPTR       mentry;

    getIDName( CurrChar );
    WriteBufferNullChar();
    if( CompFlags.doing_macro_expansion
      || (PPControl & PPCTL_NO_EXPAND) ) {
        return( T_ID );
    }
    mentry = MacroLookup( Buffer );
    if( mentry == NULL ) {
        if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
            token = Process_Pragma();
        } else {
            token = KwLookup( Buffer, TokenLen );
        }
    } else {
        /*
         * this is a macro
         */
        if( MacroIsSpecial( mentry ) ) {
            return( SpecialMacro( mentry ) );
        }
        mentry->macro_flags |= MFLAG_REFERENCED;
        /*
         * if macro requires parameters and next char is not a '('
         * then this is not a macro
         */
        if( MacroWithParenthesis( mentry ) ) {
            SkipAhead();
            if( CurrChar != '(' ) {
                if( CompFlags.cpp_mode ) {
                    InsertToken( T_WHITE_SPACE, " " );
                    token = T_ID;
                } else {
                    if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
                        token = Process_Pragma();
                    } else {
                        token = KwLookup( Buffer, TokenLen );
                    }
                }
                return( token );
            }
        }
        /*
         * start macro expansion
         */
        DoMacroExpansion( mentry );
        token = GetMacroToken();
#if 0
        if( MacroPtr != NULL ) {
            SavedCurrChar = CurrChar;
            CurrChar = LCHR_MACRO;
        }
#endif
        if( token == T_NULL ) {
            token = T_WHITE_SPACE;
        }
    }
    return( token );
}

static TOKEN ScanName( void )
/***************************/
{
    Buffer[0] = CurrChar;
    TokenLen = 1;
    NextChar();
    return( doScanName() );
}

static TOKEN doScanDotSomething( int c )
/***************************************
 * on entry TokenLen = 1
 *
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    TOKEN   token;

    token = T_DOT;
    if( c == '.' ) {
        c = NextChar();
        if( c == '.' ) {
            Buffer[TokenLen++] = '.';
            Buffer[TokenLen++] = '.';
            NextChar();
            token = T_DOT_DOT_DOT;
        } else {
            CurrChar = '.';
            UnGetChar( c );
        }
    }
    Buffer[TokenLen] = '\0';
    return( token );
}

static TOKEN doScanFloat( bool hex )
/**********************************/
{
    int             c;
    TOKEN           token;
    charset_flags   flags;

    BadTokenInfo = ERR_NONE;
    c = CurrChar;
    if( c == '.' ) {
        flags = ( hex ) ? C_HX | C_DI : C_DI;
        c = WriteBufferCharNextChar( c );
        while( CharSet[c] & flags ) {
            c = WriteBufferCharNextChar( c );
        }
        if( TokenLen == 1 ) {   /* .? */
            return( doScanDotSomething( c ) );
        }
    }
    token = T_CONSTANT;
    if( ONE_CASE_EQUAL( c, 'E' )
      || hex && ONE_CASE_EQUAL( c, 'P' ) ) {
        c = WriteBufferCharNextChar( c );
        if( c == '+'
          || c == '-' ) {
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
        WriteBufferCharNextChar( c );
        ConstType = TYP_FLOAT;
    } else if( ONE_CASE_EQUAL( c, 'L' ) ) {
        WriteBufferCharNextChar( c );
        if( CompFlags.use_long_double ) {
            ConstType = TYP_LONG_DOUBLE;
        } else {
            ConstType = TYP_DOUBLE;
        }
    } else {
        ConstType = TYP_DOUBLE;
    }
    WriteBufferNullChar();
    return( token );
}

static TOKEN doScanAsm( void )
/****************************/
{
    BadTokenInfo = ERR_NONE;
    NextChar();
    while( getIDName( CurrChar ) == '.' ) {
        WriteBufferCharNextChar( '.' );
    }
    WriteBufferNullChar();
    return( T_ID );
}

static TOKEN ScanDot( void )
/**************************/
{
    if( PPControl & PPCTL_ASM ) {
        Buffer[0] = '.';
        TokenLen = 1;
        return( doScanAsm() );
    } else {
        TokenLen = 0;
        return( doScanFloat( false ) );
    }
}

static TOKEN doScanPPNumber( void )
/**********************************
 *
 * 3.1.8 pp-number (C99 §6.4.8 adds 'p'/'P')
 * pp-number:
 *          digit           (checked by caller)
 *          . digit         (checked by caller)
 *          pp-number digit
 *          pp-number identifier-nondigit
 *          pp-number e sign
 *          pp-number E sign
 *          pp-number p sign
 *          pp-number P sign
 *          pp-number .
 */
{
    int         c;
    int         prevc;

    c = 0;
    for( ;; ) {
        prevc = c;
        c = NextChar();
        if( (CharSet[c] & (C_AL | C_DI))
          || c == '.' ) {
            WriteBufferChar( c );
        } else if( ( ONE_CASE_EQUAL( prevc, 'E' )
          || ( CompVars.cstd > STD_C89 )
          && ONE_CASE_EQUAL( prevc, 'P' ) )
          && ( c == '+' || c == '-' ) ) {
            WriteBufferChar( c );
            if( CompFlags.extensions_enabled ) {
                /*
                 * concession to existing practice...
                 *  #define A2 0x02
                 *  #define A3 0xaa0e+A2
                 *  // users want: 0xaa0e + 0x02
                 *  // not: 0xaa0e + A2 (but, this is what ISO C requires!)
                 */
                /*
                 * advance to next
                 */
                prevc = c;
                c = NextChar();
                if( (CharSet[c] & C_DI) == 0 ) {
                    /*
                     * allow e+<digit>
                     */
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

static TOKEN ScanPPDigit( void )
/******************************/
{
    Buffer[0] = CurrChar;
    TokenLen = 1;
    return( doScanPPNumber() );
}

static TOKEN ScanPPDot( void )
/****************************/
{
    int         c;

    Buffer[0] = '.';
    TokenLen = 1;
    c = NextChar();
    if( CharSet[c] & C_DI ) {
        Buffer[TokenLen++] = c;
        return( doScanPPNumber() );
    } else {
        return( doScanDotSomething( c ) );
    }
}

static TOKEN doScanNum( void )
/****************************/
{
    int             c;
    msg_codes       bad_token_type;
    TOKEN           token;
    const char      *curr;
    size_t          len;
    bool            overflow;
    bool            ssuffix;
    bool            usuffix;
    enum {
        SUFF_NONE,
        SUFF_8    = 0x01, /* 8-bit */
        SUFF_16   = 0x02, /* 16-bit */
        SUFF_L    = 0x03, /* 32-bit */
        SUFF_LL   = 0x04, /* 64-bit */
        SUFF_U    = 0x08, /* unsigned */
        SUFF_MS   = 0x10, /* MS ixx */
        SUFF_MASK = SUFF_8 | SUFF_16 | SUFF_L | SUFF_LL,
    } suffix;

    ConstType = TYP_UNDEFINED;
    BadTokenInfo = ERR_NONE;
    overflow = false;
    usuffix = true;
    Set64ValZero( Constant64 );
    if( CurrChar == '0' ) {
        c = NextChar();
        if( ONE_CASE_EQUAL( c, 'X' ) ) {
            bad_token_type = ERR_INVALID_HEX_CONSTANT;
            c = WriteBufferCharNextChar( c );
            while( CharSet[c] & (C_HX | C_DI) ) {
                c = WriteBufferCharNextChar( c );
            }

            if( CompVars.cstd > STD_C89 ) {
                if( c == '.'
                  || ONE_CASE_EQUAL( c, 'P' ) ) {
                    return( doScanFloat( true ) );
                }
            }

            if( TokenLen == 2 ) {
                /*
                 * just collected a 0x
                 */
                BadTokenInfo = ERR_INVALID_HEX_CONSTANT;
                if( diagnose_lex_error() ) {
                    CErr1( ERR_INVALID_HEX_CONSTANT );
                }
            } else {
                /*
                 * skip 0x thing
                 */
                curr = Buffer + 2;
                len = TokenLen - 2;
                while( len-- > 0 ) {
                    unsigned char   ch;

                    ch = *(unsigned char *)curr++;
                    if( U64Cnv16( &Constant64, HEXBIN( ch ) ) ) {
                        overflow = true;
                    }
                }
            }
        } else if( ONE_CASE_EQUAL( c, 'B' )
          && ( CompFlags.extensions_enabled
          || ( CompVars.cstd >= STD_C23 ) ) ) {
            bad_token_type = ERR_INVALID_BINARY_CONSTANT;
            c = WriteBufferCharNextChar( c );
            while( c == '0' || c == '1' ) {
                c = WriteBufferCharNextChar( c );
            }

            if( TokenLen == 2 ) {
                /*
                 * just collected a 0b
                 */
                BadTokenInfo = ERR_INVALID_BINARY_CONSTANT;
                if( diagnose_lex_error() ) {
                    CErr1( ERR_INVALID_BINARY_CONSTANT );
                }
            } else {
                /*
                 * skip the 0b start of the binary number
                 */
                curr = Buffer + 2;
                len = TokenLen - 2;
                while( len-- > 0 ) {
                    unsigned char   ch;

                    ch = *(unsigned char *)curr++;
                    if( U64Cnv2( &Constant64, DEC2BIN( ch ) ) ) {
                        overflow = true;
                    }
                }
            }
        } else {
            /*
             * scan octal number
             */
            bool    digit89;

            bad_token_type = ERR_INVALID_OCTAL_CONSTANT;
            /*
             * if collecting tokens for macro preprocessor, allow 8 and 9
             * since the argument may be used in with # or ##.
             */
            digit89 = false;
            while( CharSet[c] & C_DI ) {
                digit89 |= ( c > '7' );
                c = WriteBufferCharNextChar( c );
            }
            if( c == '.'
              || ONE_CASE_EQUAL( c, 'E' ) ) {
                return( doScanFloat( false ) );
            }
            if( digit89 ) {
                /*
                 * if digit 8 or 9 somewhere
                 */
                BadTokenInfo = ERR_INVALID_OCTAL_CONSTANT;
                if( diagnose_lex_error() ) {
                    CErr1( ERR_INVALID_OCTAL_CONSTANT );
                }
            } else {
                curr = Buffer;
                len = TokenLen;
                while( len-- > 0 ) {
                    unsigned char   ch;

                    ch = *(unsigned char *)curr++;
                    if( U64Cnv8( &Constant64, DEC2BIN( ch ) ) ) {
                        overflow = true;
                    }
                }
            }
        }
    } else {
        /*
         * scan decimal number
         */
        bad_token_type = ERR_INVALID_CONSTANT;
        usuffix = false;
        c = NextChar();
        while( CharSet[c] & C_DI ) {
            c = WriteBufferCharNextChar( c );
        }
        if( c == '.'
          || ONE_CASE_EQUAL( c, 'E' ) ) {
            return( doScanFloat( false ) );
        }
        curr = Buffer;
        len = TokenLen;
        while( len-- > 0 ) {
            unsigned char   ch;

            ch = *(unsigned char *)curr++;
            if( U64Cnv10( &Constant64, DEC2BIN( ch ) ) ) {
                overflow = true;
            }
        }
    }
    /*
     * collect suffix
     */
    suffix = SUFF_NONE;
    if( ONE_CASE_EQUAL( c, 'U' ) ) {
        suffix |= SUFF_U;
        c = WriteBufferCharNextChar( c );
    }
    if( ONE_CASE_EQUAL( c, 'L' ) ) {
        c = WriteBufferCharNextChar( c );
        if( ONE_CASE_EQUAL( c, 'L' ) ) {
            suffix |= SUFF_LL;
            c = WriteBufferCharNextChar( c );
        } else {
            suffix |= SUFF_L;
        }
        if( ONE_CASE_EQUAL( c, 'U' ) ) {
            suffix |= SUFF_U;
            c = WriteBufferCharNextChar( c );
        }
    } else if( ONE_CASE_EQUAL( c, 'I' ) ) {
        c = WriteBufferCharNextChar( c );
        if( c == '6' ) {
            c = WriteBufferCharNextChar( c );
            if( c == '4' ) {
                suffix |= SUFF_LL | SUFF_MS;
                c = WriteBufferCharNextChar( c );
            } else if( diagnose_lex_error() ) {
                CErr1( ERR_INVALID_CONSTANT );
            }
        } else if( c == '3' ) {
            c = WriteBufferCharNextChar( c );
            if( c == '2' ) {
                suffix |= SUFF_L | SUFF_MS;
                c = WriteBufferCharNextChar( c );
            } else if( diagnose_lex_error() ) {
                CErr1( ERR_INVALID_CONSTANT );
            }
        } else if( c == '1' ) {
            c = WriteBufferCharNextChar( c );
            if( c == '6' ) {
                suffix |= SUFF_16 | SUFF_MS;
                c = WriteBufferCharNextChar( c );
            } else if( diagnose_lex_error() ) {
                CErr1( ERR_INVALID_CONSTANT );
            }
        } else if( c == '8' ) {
            suffix |= SUFF_8 | SUFF_MS;
            c = WriteBufferCharNextChar( c );
        } else if( diagnose_lex_error() ) {
            CErr1( ERR_INVALID_CONSTANT );
        }
    }
    /*
     * process constant size
     */
    ssuffix = ( (suffix & SUFF_U) == 0 );
    usuffix = ( !ssuffix || usuffix );
    switch( suffix & SUFF_MASK ) {
    case SUFF_NONE:
    case SUFF_8:
    case SUFF_16:
        if( ssuffix ) {
            if( U64CmpU32( Constant64, TARGET_INT_MAX ) <= 0 ) {
                ConstType = TYP_INT;
                break;
            }
        }
        if( usuffix ) {
            if( U64CmpU32( Constant64, TARGET_UINT_MAX ) <= 0 ) {
                ConstType = TYP_UINT;
                break;
            }
        }
        /* fall through */
    case SUFF_L:
        if( ssuffix ) {
            if( U64CmpU32( Constant64, TARGET_LONG_MAX ) <= 0 ) {
                ConstType = TYP_LONG;
                break;
            }
        }
        if( usuffix ) {
            if( U64CmpU32( Constant64, TARGET_ULONG_MAX ) <= 0 ) {
                ConstType = TYP_ULONG;
                break;
            }
        }
        /* fall through */
    case SUFF_LL:
        if( ssuffix ) {
            if( Constant64.u.sign.v == 0 ) {
                ConstType = TYP_LONG64;
                break;
            }
        }
        if( usuffix ) {
            ConstType = TYP_ULONG64;
        } else if( ConstType == TYP_UNDEFINED ) {
            /*
             * C99 say that signed long long decimal number is undefined type
             * if it cannot be represented by any standard or extended signed type
             * signed integer over LLONG_MAX cannot be represented by OW signed type
             * that it should be undefined type.
             * We report this issue at Warning level 1 and setup it as
             * unsigned long long type
             */
            ConstType = TYP_ULONG64;
            if( diagnose_lex_error() ) {
                CWarn1( ERR_INT_IS_UNSIGNED );
            }
        }
        break;
    }

    token = T_CONSTANT;
    if( CharSet[c] & (C_AL | C_DI) ) {
        token = T_BAD_TOKEN;
        while( CharSet[c] & (C_AL | C_DI) ) {
            c = WriteBufferCharNextChar( c );
        }
    }
    WriteBufferNullChar();
    if( overflow ) {
        BadTokenInfo = ERR_CONSTANT_TOO_BIG;
        if( diagnose_lex_error() ) {
            CWarn1( ERR_CONSTANT_TOO_BIG );
        }
    }
    if( token == T_BAD_TOKEN ) {
        BadTokenInfo = bad_token_type;
    }
    return( token );
}

static TOKEN ScanNum( void )
/**************************/
{
    Buffer[0] = CurrChar;
    TokenLen = 1;
    if( PPControl & PPCTL_ASM ) {
        return( doScanAsm() );
    } else {
        return( doScanNum() );
    }
}

static TOKEN ScanDelim1( void )
/******************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    TOKEN       token;

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
        if( last == T_COLON ) {         /* <: */
            *token = T_LEFT_BRACKET;
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
        if( last == T_GT ) {            /* :> */
            /*
             * TODO: according to the standard, ":>" should be an
             * alternative token (digraph) for "]"
             * *token = T_RIGHT_BRACKET;   -> ]
             */
            *token = T_SEG_OP;
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
    default:
        return( false );
    }
    return( true );
}

static TOKEN ScanDelim2( void )
/******************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    TOKEN           token;

    token = TokValue[CurrChar];
    Buffer[0] = CurrChar;
    TokenLen = 1;
    if( (CharSet[NextChar()] & C_DE)
      && checkDelim2( &token, TokValue[CurrChar] ) ) {
        Buffer[TokenLen++] = CurrChar;
        if( (CharSet[NextChar()] & C_DE)
          && checkDelim2( &token, TokValue[CurrChar] ) ) {
            Buffer[TokenLen++] = CurrChar;
            NextChar();
        }
    }
    Buffer[TokenLen] = '\0';
    return( token );
}

static void doScanComment( void )
/*******************************/
{
    int         c;
    int         prev_char;

    CommentLoc = TokenLoc;
    CompFlags.scanning_comment = true;
    if( CompFlags.cpp_mode ) {
        CppComment( '*' );
        c = NextChar();
        for( ;; ) {
            if( c == '*' ) {
                c = NextChar();
                if( c == '/' )
                    break;
                if( CompFlags.cpp_keep_comments ) {
                    CppPrtChar( '*' );
                }
                continue; // could be **/
            }
            if( c == LCHR_EOF ) {
                break;
            }
            if( c == '\n' ) {
                CppPrtChar( c );
                NewLineStartPos( SrcFiles );
            } else if( c != '\r'
              && CompFlags.cpp_keep_comments ) {
                CppPrtChar( c );
            }
            c = NextChar();
        }
        CppComment( '\0' );
    } else {
        /*
         * make '/' anf '\n' a special characters so that we only have
         * to do one test for each character inside the main loop
         */
        CharSet['/'] |= C_EX;   /* make '/' special character */
        CharSet['\n'] |= C_EX;  /* make '\n' special character */
        c = '\0';
        for( ; c != LCHR_EOF; ) {
            if( c == '\n' ) {
                NewLineStartPos( SrcFiles );
                TokenLoc = SrcFileLoc = SrcFiles->src_loc;
            }
            do {
                do {
                    prev_char = c;
                    c = *SrcFiles->src_ptr++;
                } while( (CharSet[c] & C_EX) == 0 );
                c = GetCharCheck( c );
                if( c == LCHR_EOF ) {
                    break;
                }
            } while( (CharSet[c] & C_EX) == 0 );
            if( c == '/' ) {
                if( prev_char == '*' )
                    break;
                /*
                 * get next character and see if it is '*' for nested comment
                 */
                c = NextChar();
                if( c == '*' ) {
                    c = NextChar();
                    if( c == '/' )
                        break;
                    CWarn2( ERR_NESTED_COMMENT, CommentLoc.line );
                }
            }
            /*
             * NextChar might not be pointing to GetNextChar at this point
             */
            while( c != LCHR_EOF && NextChar != GetNextChar ) {
                c = NextChar();
            }
        }
        CharSet['\n'] &= ~C_EX;     /* undo '\n' special character */
        CharSet['/'] &= ~C_EX;      /* undo '/' special character */
    }
    if( c != LCHR_EOF ) {
        CompFlags.scanning_comment = false;
        NextChar();
    }
}

static TOKEN ScanSlash( void )
/*****************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    TOKEN   token;

    token = T_DIV;
    Buffer[0] = '/';
    TokenLen = 1;
    NextChar();             /* can't inline this copy of NextChar */
    if( CurrChar == '=' ) { /* if second char is an = */
        Buffer[TokenLen++] = '=';
        NextChar();
        token = T_DIV_EQUAL;
    } else if( CurrChar == '/'
      && !CompFlags.strict_ANSI ) {   /* if C++ // style comment */
        if( CompFlags.cpp_mode ) {
            CppComment( '/' );
        }
        CompFlags.scanning_cpp_comment = true;
        for( ;; ) {
            if( CurrChar == '\r' ) {
                /*
                 * some editors don't put linefeeds on end of lines
                 */
                NextChar();
                break;
            }
            NextChar();
            if( CurrChar == '\n' )
                break;
            if( CurrChar == LCHR_EOF )
                break;
            if( CurrChar == '\0' )
                break;
            if( CompFlags.cpp_mode
              && CompFlags.cpp_keep_comments
              && CurrChar != '\r' ) {
                CppPrtChar( CurrChar );
            }
        }
        if( CompFlags.cpp_mode ) {
            CppComment( '\0' );
        }
        CompFlags.scanning_cpp_comment = false;
        Buffer[0] = ' ';
        token = T_WHITE_SPACE;
    } else if( CurrChar == '*' ) {
        doScanComment();
        Buffer[0] = ' ';
        token = T_WHITE_SPACE;
    }
    Buffer[TokenLen] = '\0';
    return( token );
}

static msg_codes doScanHex( int max, unsigned_64 *pval64, escinp_fn ifn, escout_fn ofn )
/***************************************************************************************
 * Warning! this function is also used from cstring.c
 * cannot use Buffer array or NextChar function in any way
 * input and output is done using ifn or ofn functions
 */
{
    int             c;
    int             count;
    bool            too_big;

    too_big = false;
    count = max;
    Set64ValZero( *pval64 );
    for( ;; ) {
        c = ifn();
        if( max == 0 )
            break;
        if( (CharSet[c] & (C_HX | C_DI)) == 0 )
            break;
        if( ofn != NULL )
            ofn( c );
        if( U64Cnv16( pval64, HEXBIN( c ) ) ) {
            too_big = true;
        }
        --max;
    }
    if( count == max ) {
        /*
         * indicate no characters matched
         */
        return( ERR_INVALID_HEX_CONSTANT );
    }
    if( too_big ) {
        return( ERR_CONSTANT_TOO_BIG );
    }
    /*
     * indicate characters were matched
     */
    return( ERR_NONE );
}

int ESCChar( escinp_fn ifn, escout_fn ofn, msg_codes *perr_msg )
/***************************************************************
 * Warning! this function is also used from cstring.c
 * cannot use Buffer array or NextChar function in any way
 * input and output is done using ifn or ofn functions
 */
{
    int         n;
    int         i;
    msg_codes   err_msg;
    int         c;

    c = ifn();
    if( OCTAL( c ) ) {
        /*
         * get octal escape sequence
         */
        n = 0;
        i = 3;
        while( i-- > 0 && OCTAL( c ) ) {
            if( ofn != NULL )
                ofn( c );
            n = n * 8 + DEC2BIN( c );
            c = ifn();
        }
    } else if( c == 'x' ) {
        unsigned_64 val64;

        /*
         * get hex escape sequence
         */
        if( ofn != NULL )
            ofn( c );
        err_msg = doScanHex( 127, &val64, ifn, ofn );
        if( err_msg != ERR_NONE )
            *perr_msg = err_msg;
        n = U32FetchTrunc( val64 );
    } else {
        if( ofn != NULL )
            ofn( c );
        switch( c ) {
        case 'a':
            c = ESCAPE_a;
            break;
        case 'b':
            c = ESCAPE_b;
            break;
        case 'f':
            c = ESCAPE_f;
            break;
        case 'n':
            c = ESCAPE_n;
            break;
        case 'r':
            c = ESCAPE_r;
            break;
        case 't':
            c = ESCAPE_t;
            break;
        case 'v':
            c = ESCAPE_v;
            break;
#ifdef __QNX__
        case 'l':
            /*
             * for lazy QNX programmers
             */
            if( CompFlags.extensions_enabled ) {
                c = ESCAPE_n;
            }
            break;
#endif
        case '\\':
        case '\"':
        case '\'':
        case '?':
            break;
        default:
            *perr_msg = ERR_INV_CHAR_CONSTANT;
            break;
        }
#if _CPU == 370
        _ASCIIOUT( c );
#endif
        n = c;
        ifn();
    }
    return( n );
}

int EncodeWchar( int c )
/**********************/
{
    if( CompFlags.use_double_byte ) {
        if( CompFlags.jis_to_unicode ) {
            c = JIS2Unicode( c );
        }
    } else {
        c = UniCode[c];
    }
    return( c );
}

static int read_inp( void )
{
    return( NextChar() );
}

static TOKEN doScanCharConst( DATA_TYPE char_type )
/**************************************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    int         c;
    int         i;
    TOKEN       token;

    Set64ValZero( Constant64 );
    c = NextChar();
    if( c == '\'' ) {
        Buffer[TokenLen++] = '\'';
        NextChar();
        token = T_BAD_TOKEN;
    } else {
        BadTokenInfo = ERR_NONE;
        token = T_CONSTANT;
        i = 0;
        for( ;; ) {
            if( c == '\r'
              || c == '\n' ) {
                token = T_BAD_TOKEN;
                break;
            }
            if( c == '\\' ) {
                Buffer[TokenLen++] = '\\';
                c = ESCChar( read_inp, WriteBufferChar, &BadTokenInfo );
                if( BadTokenInfo == ERR_INVALID_HEX_CONSTANT ) {
                    if( diagnose_lex_error() ) {
                        CErr1( ERR_INVALID_HEX_CONSTANT );
                    }
                } else if( BadTokenInfo == ERR_CONSTANT_TOO_BIG
                  || BadTokenInfo == ERR_INV_CHAR_CONSTANT ) {
                    if( diagnose_lex_error() ) {
                        CWarn1( BadTokenInfo );
                    }
                }
                if( char_type == TYP_WCHAR ) {
                    ++i;
                    /* value = (value << 8) + ((c & 0xFF00) >> 8); */
                    U64ShiftLEq( &Constant64, 8 );
                    U64AddI32( &Constant64, ((c & 0xFF00) >> 8) );
                    c &= 0x00FF;
                }
            } else {
                Buffer[TokenLen++] = c;
                NextChar();
                if( CharSet[c] & C_DB ) {
                    /*
                     * if double-byte char
                     */
                    c = (c << 8) + (CurrChar & 0x00FF);
                    if( char_type == TYP_WCHAR ) {
                        if( CompFlags.jis_to_unicode ) {
                            c = JIS2Unicode( c );
                        }
                    }
                    ++i;
                    /* value = (value << 8) + ((c & 0xFF00) >> 8); */
                    U64ShiftLEq( &Constant64, 8 );
                    U64AddI32( &Constant64, ((c & 0xFF00) >> 8) );
                    c &= 0x00FF;
                    Buffer[TokenLen++] = CurrChar;
                    NextChar();
                } else if( char_type == TYP_WCHAR ) {
                    c = EncodeWchar( c );
                    ++i;
                    /* value = (value << 8) + ((c & 0xFF00) >> 8); */
                    U64ShiftLEq( &Constant64, 8 );
                    U64AddI32( &Constant64, ((c & 0xFF00) >> 8) );
                    c &= 0x00FF;
#if _CPU == 370
                } else {
                    _ASCIIOUT( c );
#endif
                }
            }
            ++i;
            /* value = (value << 8) + c; */
            U64ShiftLEq( &Constant64, 8 );
            U64AddI32( &Constant64, c );
            /*
             * handle case where user wants a \ but doesn't escape it
             */
            if( c == '\''
              && CurrChar != '\'' ) {
                token = T_BAD_TOKEN;
                break;
            }
            c = CurrChar;
            if( c == '\'' )
                break;
            if( i >= 4 ) {
                token = T_BAD_TOKEN;
                break;
            }
        }
    }
    if( token == T_BAD_TOKEN ) {
        BadTokenInfo = ERR_INV_CHAR_CONSTANT;
    } else {
        Buffer[TokenLen++] = c;
        NextChar();
        if( BadTokenInfo == ERR_INVALID_HEX_CONSTANT ) {
            token = T_BAD_TOKEN;
        }
    }
    Buffer[TokenLen] = '\0';
    ConstType = char_type;
    if( char_type == TYP_CHAR ) {
        /*
         * character constant has plain character type
         * ConstType must be setup to appropriate character type
         */
        if( CompFlags.signed_char ) {
            /*
             * check if it is single character constant
             * it means value in range 128 ... 255
             * then convert into signed char range -128 ... 127
             */
            if( U64CmpU32( Constant64, 127 ) > 0
              && U64CmpU32( Constant64, 256 ) < 0 ) {
                U64AddI32( &Constant64, -256 );
            }
        } else {
            ConstType = TYP_UCHAR;
        }
    }
    return( token );
}

static TOKEN ScanCharConst( void )
/********************************/
{
    Buffer[0] = '\'';
    TokenLen = 1;
    return( doScanCharConst( TYP_CHAR ) );
}

static TOKEN doScanString( bool wide )
/************************************/
{
    int         c;
    bool        ok;

    ok = false;
    BadTokenInfo = ERR_NONE;
    CompFlags.wide_char_string = false;
    CompFlags.trigraph_alert = false;
    TokenLen = 0;
    c = NextChar();
    for( ;; ) {
        if( c == '\n' ) {
            if( SkipLevel != NestLevel ) {
                if( CompFlags.extensions_enabled ) {
                    CWarn1( ERR_MISSING_QUOTE );
                    ok = true;
                }
            }
            break;
        }
        if( c == LCHR_EOF )
            break;
        if( c == '"' ) {
            NextChar();
            ok = true;
            break;
        }

        if( c == '\\' ) {
            WriteBufferChar( c );
            ESCChar( read_inp, WriteBufferChar, &BadTokenInfo );
            if( BadTokenInfo == ERR_INVALID_HEX_CONSTANT ) {
                if( diagnose_lex_error() ) {
                    CErr1( ERR_INVALID_HEX_CONSTANT );
                }
            } else if( BadTokenInfo == ERR_CONSTANT_TOO_BIG
              || BadTokenInfo == ERR_INV_CHAR_CONSTANT ) {
                if( diagnose_lex_error() ) {
                    CWarn1( BadTokenInfo );
                }
            }
            c = CurrChar;
        } else {
            /*
             * if first character of a double-byte character, then
             * save it and get the next one.
             */
            if( CharSet[c] & C_DB ) {
                c = WriteBufferCharNextChar( c );
            }
            c = WriteBufferCharNextChar( c );
        }
    }
    WriteBufferNullChar();
    if( CompFlags.trigraph_alert ) {
        CWarn1( ERR_EXPANDED_TRIGRAPH );
    }
    if( wide )
        CompFlags.wide_char_string = wide;
    if( ok )
        return( T_STRING );
    BadTokenInfo = ERR_MISSING_QUOTE;
    return( T_BAD_TOKEN );
}

static TOKEN ScanString( void )
/*****************************/
{
    return( doScanString( false ) );
}

static TOKEN ScanWide( void )
/****************************
 * scan something that starts with L
 */
{
    int         c;
    TOKEN       token;

    c = NextChar();
    if( c == '"' ) {
        /*
         * L"abc"
         */
        token = doScanString( true );
    } else {
        /*
         * regular identifier
         */
        Buffer[0] = 'L';
        TokenLen = 1;
        if( c == '\'' ) {
            /*
             * L'a'
             */
            Buffer[TokenLen++] = '\'';
            token = doScanCharConst( TYP_WCHAR );
        } else {
            /*
             * regular identifier
             */
            token = doScanName();
        }
    }
    return( token );
}

static TOKEN ScanWhiteSpace( void )
/*********************************/
{
    int         c;

    if( NextChar == getCharAfterBackSlash ) {
        do {
            c = NextChar();
        } while( CharSet[c] & C_WS );
    } else {
        do {
            do {
                c = *SrcFiles->src_ptr++;
            } while( CharSet[c] & C_WS );
            if( (CharSet[c] & C_EX) == 0 )
                break;
            c = GetCharCheck( c );
        } while( CharSet[c] & C_WS );
        CurrChar = c;
    }
    return( T_WHITE_SPACE );
}

static void SkipWhiteSpace( int c )
/*********************************/
{
    if( !CompFlags.cpp_mode ) {
        ScanWhiteSpace();
    } else {
        while( CharSet[c] & C_WS ) {
            if( c != '\r'
              && IS_PPCTL_NORMAL() ) {
                CppPrtChar( c );
            }
            c = NextChar();
        }
    }
}


void SkipAhead( void )
/********************/
{
    for( ;; ) {
        for( ;; ) {
            if( CharSet[CurrChar] & C_WS ) {
                SkipWhiteSpace( CurrChar );
            }
            if( CurrChar != '\n' )
                break;
            if( CompFlags.cpp_mode
              && IS_PPCTL_NORMAL() ) {
                CppPrtChar( '\n' );
            }
            NewLineStartPos( SrcFiles );
            SrcFileLoc = SrcFiles->src_loc;
            NextChar();
        }
        if( CurrChar != '/' )
            break;
        NextChar();
        if( CurrChar == '*' ) {
            TokenLoc = SrcFileLoc;
            doScanComment();
        } else {
            UnGetChar( CurrChar );
            CurrChar = '/';
            break;
        }
    }
}

static TOKEN ScanNewline( void )
/******************************/
{
    NewLineStartPos( SrcFiles );
    SrcFileLoc = SrcFiles->src_loc;
    if( PPControl & PPCTL_EOL )
        return( T_NULL );
    return( CheckControl() );
}

static TOKEN ScanCarriageReturn( void )
/*************************************/
{
    if( NextChar() == '\n' ) {
        return( ScanNewline() );
    } else {
        return( ScanWhiteSpace() );
    }
}

static TOKEN ScanInvalid( void )
/*******************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    TOKEN   token;

    token = T_BAD_CHAR;
    Buffer[0] = CurrChar;
    Buffer[1] = '\0';
    TokenLen = 1;
    NextChar();
    return( token );
}

#ifdef CHAR_MACRO
static TOKEN ScanMacroToken( void )
/*********************************/
{
    TOKEN   token;

    token = GetMacroToken();
    if( token == T_NULL ) {
        if( CompFlags.cpp_mode ) {
            CppPrtChar( ' ' );
        }
        CurrChar = SavedCurrChar;
        token = ScanToken();
    }
    return( token );
}
#endif

static TOKEN ScanEof( void )
/**************************/
{
    return( T_EOF );
}

static TOKEN (*ScanFunc[])( void ) = {
    #define pick(e,p) p,
    #include "_scnclas.h"
    #undef pick
};

TOKEN ScanToken( void )
/*********************/
{
    /*
     * remember line token starts on
     */
    TokenLoc = SrcFileLoc;
//    TokenLen = 1;
//    Buffer[0] = CurrChar;
    return( (*ScanFunc[ClassTable[CurrChar]])() );
}

TOKEN NextToken( void )
/*********************/
{
    do {
        if( MacroPtr == NULL ) {
            CurToken = ScanToken();
        } else {
            CurToken = GetMacroToken();
            if( CurToken == T_NULL ) {
                CurToken = ScanToken();
            }
        }
    } while( CurToken == T_WHITE_SPACE );
#ifdef FDEBUG
    DumpToken();
#endif
    return( CurToken );
}

TOKEN PPNextToken( void )
/************************
 * called from macro pre-processor
 */
{
    do {
        if( MacroPtr == NULL ) {
            CurToken = ScanToken();
        } else {
            CurToken = GetMacroToken();
            if( CurToken == T_NULL ) {
                if( CompFlags.cpp_mode ) {
                    CppPrtChar( ' ' );
                }
                CurToken = ScanToken();
            }
        }
    } while( CurToken == T_WHITE_SPACE );
    return( CurToken );
}

TOKEN ReScanToken( void )
/***********************/
{
    FCB             *oldSrcFile;
    int             saved_currchar;
    int             (*saved_nextchar)( void );
    void            (*saved_ungetchar)( int );
    int             (*saved_getcharcheck)( int );
    TOKEN           token;

    /*
     * save current status
     */
    saved_currchar = CurrChar;
    saved_nextchar = NextChar;
    saved_ungetchar = UnGetChar;
    saved_getcharcheck = GetCharCheck;
    oldSrcFile = SrcFiles;

    SrcFiles = &rescan_fcb;
    NextChar = reScanGetNextChar;
    UnGetChar = reScanGetNextCharUndo;
    GetCharCheck = reScanGetCharCheck;
    CompFlags.rescan_buffer_done = false;

    CurrChar = NextChar();
    CompFlags.doing_macro_expansion = true; /* return macros as ID's */
    token = ScanToken();
    CompFlags.doing_macro_expansion = false;
    if( token == T_STRING
      && CompFlags.wide_char_string ) {
        token = T_LSTRING;
    }
    SrcFiles->src_ptr--;

    SrcFiles = oldSrcFile;
    CurrChar = saved_currchar;
    NextChar = saved_nextchar;
    UnGetChar = saved_ungetchar;
    GetCharCheck = saved_getcharcheck;

    return( token );
}

void ScanInit( void )
/*******************/
{
    int         i;
    int         c;

    memset( &ClassTable[0],   SCAN_INVALID, 256 );
    memset( &ClassTable['A'], SCAN_NAME,    26 );
    memset( &ClassTable['a'], SCAN_NAME,    26 );
    memset( &ClassTable['0'], SCAN_NUM,     10 );
    for( i = 0; (c = InitClassTable[i].chr) != '\0'; i++ ) {
        ClassTable[c] = InitClassTable[i].cls;
    }
    CurrChar = '\n';
    PPControl = PPCTL_NORMAL;
    CompFlags.scanning_comment = false;
    SizeOfCount = 0;
    NextChar = GetNextChar;
    UnGetChar = GetNextCharUndo;
    GetCharCheck = GetCharCheckFile;
}

bool InitPPScan( void )
/**********************
 * called by CollectParms() to gather tokens for macro parms
 * and CDefine() to gather tokens for macro definition
 * example usage:
 *      bool ppscan_mode;
 *      ppscan_mode = InitPPScan();
 *      CollectParms();
 *      FiniPPScan( ppscan_mode );
 */
{
    if( ScanFunc[SCAN_NUM] == ScanNum ) {
        ScanFunc[SCAN_NUM] = ScanPPDigit;
        ScanFunc[SCAN_DOT] = ScanPPDot;
        /*
         * indicate changed to PP mode
         */
        return( true );
    }
    /*
     * indicate already in PP mode
     */
    return( false );
}

void FiniPPScan( bool ppscan_mode )
/**********************************
 * called when CollectParms() and
 * CDefine() are finished gathering tokens
 * if InitPPScan() changed into PP mode
 * reset back to normal mode
 */
{
    if( ppscan_mode ) {
        /*
         * reset back to normal mode
         */
        ScanFunc[SCAN_NUM] = ScanNum;
        ScanFunc[SCAN_DOT] = ScanDot;
    }
}
