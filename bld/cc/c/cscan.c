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

enum scan_class {
    #define pick(e,p) e,
    #include "_scnclas.h"
    #undef pick
};

extern const unsigned char  TokValue[];

static FCB              rescan_tmp_file;
#ifdef CHAR_MACRO
static int              SavedCurrChar;          // used when get tokens from macro
#endif
static unsigned char    ClassTable[LCHR_MAX];

static unsigned char InitClassTable[] = {
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
    rescan_tmp_file.src_ptr = (const unsigned char *)ptr;
}

const char *ReScanPos( void )
/***************************/
{
    return( (const char *)rescan_tmp_file.src_ptr );
}

static int reScanGetNextChar( void )
/**********************************/
{
    CurrChar = *rescan_tmp_file.src_ptr++;
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

    rescan_tmp_file.src_ptr--;
    CompFlags.rescan_buffer_done = false;
}

unsigned hashpjw( const char *s )
/*******************************/
{
    unsigned        h;
    unsigned char   c;

    h = *(const unsigned char *)s++;
    if( h != 0 ) {
        c = *s++;
        if( c != '\0' ) {
            h = ( h << 4 ) + c;
            for( ;; ) {
                h &= 0x0fff;
                c = *s++;
                if( c == '\0' )
                    break;
                h = ( h << 4 ) + c;
                h = ( h ^ (h >> 12) ) & 0x0fff;
                c = *s++;
                if( c == '\0' )
                    break;
                h = ( h << 4 ) + c;
                h = h ^ (h >> 12);
            }
        }
    }
    return( h );
}

id_hash_idx CalcHash( const char *id, size_t len )
/************************************************/
{
    unsigned    hash;

    /* unused parameters */ (void)len;

    hash = hashpjw( id );
    HashValue = hash % ID_HASH_SIZE;
#if ( MACRO_HASH_SIZE > 0x0ff0 ) && ( MACRO_HASH_SIZE < 0x0fff )
    hash &= 0x0fff;
    if( hash >= MACRO_HASH_SIZE ) {
        hash -= MACRO_HASH_SIZE;
    }
#else
    hash = hash % MACRO_HASH_SIZE;
#endif
    MacHashValue = (mac_hash_idx)hash;
    return( HashValue );
}

TOKEN KwLookup( const char *buf, size_t len )
/*******************************************/
{
    const char  *keyword;
    TOKEN       token;

    token = keyword_hash( buf, TokValue, len ) + FIRST_KEYWORD;

    /* look up id in keyword table */
    if( !CompFlags.c99_extensions ) {
        switch( token ) {
        case T_INLINE:
            if( !CompFlags.extensions_enabled )
                return( T_ID );
            break;
        case T_RESTRICT:
        case T__COMPLEX:
        case T__IMAGINARY:
        case T__BOOL:
        case T___OW_IMAGINARY_UNIT:
            return( T_ID );
        }
    }

    keyword = Tokens[token];
    if( *keyword == buf[0] ) {
        if( memcmp( keyword, buf, len + 1 ) == 0 ) {
            return( token );
        }
    }

    /* not in keyword table, so must be just an identifier */
    return( T_ID );
}

static int getIDName( int c )
/***************************/
{
    while( CharSet[c] & (C_AL | C_DI) ) {
        while( CharSet[c] & (C_AL | C_DI) ) {
            WriteBufferChar( c );
            c = *SrcFile->src_ptr++;
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
    CalcHash( Buffer, TokenLen );
    if( CompFlags.doing_macro_expansion )
        return( T_ID );
    if( PPControl & PPCTL_NO_EXPAND )
        return( T_ID );
    mentry = MacroLookup( Buffer );
    if( mentry == NULL ) {
        if( IS_PPOPERATOR_PRAGMA( Buffer, TokenLen ) ) {
            token = Process_Pragma();
        } else {
            token = KwLookup( Buffer, TokenLen );
        }
    } else {
        /* this is a macro */
        if( MacroIsSpecial( mentry ) ) {
            return( SpecialMacro( mentry ) );
        }
        mentry->macro_flags |= MFLAG_REFERENCED;
        /* if macro requires parameters and next char is not a '('
        then this is not a macro */
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
        DoMacroExpansion( mentry );             /* start macro expansion */
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
    if( c == 'e' || c == 'E' || ( hex && ( c == 'p' || c == 'P' ) ) ) {
        c = WriteBufferCharNextChar( c );
        if( c == '+' || c == '-' ) {
            c = WriteBufferCharNextChar( c );
        }
        if( c < '0' || c > '9' ) {
            token = T_BAD_TOKEN;
            BadTokenInfo = ERR_INVALID_FLOATING_POINT_CONSTANT;
        }
        while( c >= '0' && c <= '9' ) {
            c = WriteBufferCharNextChar( c );
        }
    }
    if( c == 'f' || c == 'F' ) {
        WriteBufferCharNextChar( c );
        ConstType = TYP_FLOAT;
    } else if( c == 'l' || c == 'L' ) {
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
        if( (CharSet[c] & (C_AL | C_DI)) || c == '.' ) {
            WriteBufferChar( c );
        } else if( ( prevc == 'e' || prevc == 'E'
          || CompFlags.c99_extensions && ( prevc == 'p' || prevc == 'P' ) )
          && ( c == '+' || c == '-' ) ) {
            WriteBufferChar( c );
            if( CompFlags.extensions_enabled ) {
                /* concession to existing practice...
                    #define A2 0x02
                    #define A3 0xaa0e+A2
                    // users want: 0xaa0e + 0x02
                    // not: 0xaa0e + A2 (but, this is what ISO C requires!)
                */
                prevc = c;  //advance to next
                c = NextChar();
                if( (CharSet[c] & C_DI) == 0 ) {
                    break;  //allow e+<digit>
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
    if( c >= '0' && c <= '9' ) {
        Buffer[TokenLen++] = c;
        return( doScanPPNumber() );
    } else {
        return( doScanDotSomething( c ) );
    }
}

typedef enum { CNV_32, CNV_64, CNV_OVR } cnv_cc;

static cnv_cc Cnv8( void )
/************************/
{
    char        *curr;
    char        c;
    size_t      len;
    int         value;
    uint64      value64;
    cnv_cc      ret;

    curr = Buffer;
    len = TokenLen;
    value = 0;
    while( len-- > 0 ) {
        c = *curr;
        if( value & 0xE0000000 )
            goto is64; /* 64 bit */
        value = value * 8 + c - '0';
        ++curr;
    }
    Constant = value;
    return( CNV_32 );
is64:
    ret = CNV_64;
    U32ToU64( value, &value64 );
    do {
        c = *curr;
        if( U64Cnv8( &value64, c-'0' ) ) {
            ret = CNV_OVR;
        }
        ++curr;
    } while( len-- > 0 );
    Constant64 = value64;
    return( ret );
}

static cnv_cc Cnv16( void )
/*************************/
{
    const char      *curr;
    unsigned char   c;
    size_t          len;
    unsigned        value;
    uint64          value64;
    cnv_cc          ret;

    curr = Buffer + 2;      // skip 0x thing
    len = TokenLen - 2;
    value = 0;
    while( len-- > 0 ) {
        c = *curr;
        if( value & 0xF0000000 )
            goto is64; /* 64 bit */
        if( CharSet[c] & C_HX ) {
            c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        }
        value = value * 16 + c - '0';
        ++curr;
    }
    Constant = value;
    return( CNV_32 );
is64:
    ret = CNV_64;
    U32ToU64( value, &value64 );
    do {
        c = *curr;
        if( CharSet[c] & C_HX ) {
            c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        }
        if( U64Cnv16( &value64, c-'0' ) ) {
            ret = CNV_OVR;
        }
        ++curr;
    } while( len-- > 0 );
    Constant64 = value64;
    return( ret );
}

static cnv_cc Cnv10( void )
/*************************/
{
    const char      *curr;
    unsigned char   c;
    size_t          len;
    unsigned        value;
    uint64          value64;
    cnv_cc          ret;

    curr = Buffer;
    len = TokenLen;
    value = 0;
    while( len-- > 0 ) {
        c = *curr;
        if( value >= 429496729 ) {
            if( value == 429496729 ) {
                if( c > '5' ) {
                    goto is64;
                }
            } else {
                goto is64;
            }
        }
        value = value * 10 + c - '0';
        ++curr;
    }
    Constant = value;
    return( CNV_32 );
is64:
    ret = CNV_64;
    U32ToU64( value, &value64 );
    do {
        c = *curr;
        if( U64Cnv10( &value64, c-'0') ) {
            ret = CNV_OVR;
        }
        ++curr;
    } while( len-- > 0 );
    Constant64 = value64;
    return( ret );
}

static TOKEN doScanNum( void )
/****************************/
{
    int         c;
    msg_codes   bad_token_type;
    cnv_cc      ov;
    TOKEN       token;

    struct {
        enum { CON_DEC, CON_HEX, CON_OCT, CON_ERR } form;
        enum { SUFF_NONE,SUFF_U, SUFF_L,SUFF_UL,  SUFF_I, SUFF_UI,
               SUFF_LL,SUFF_ULL } suffix;
    } con;

    BadTokenInfo = ERR_NONE;
    ov = CNV_32;
    Constant = 0;
    if( CurrChar == '0' ) {
        c = NextChar();
        if( c == 'x' || c == 'X' ) {
            bad_token_type = ERR_INVALID_HEX_CONSTANT;
            con.form = CON_HEX;
            c = WriteBufferCharNextChar( c );
            while( CharSet[c] & (C_HX | C_DI) ) {
                c = WriteBufferCharNextChar( c );
            }

            if (CompFlags.c99_extensions) {
                if ( c == '.' || c == 'p' || c == 'P' ) {
                    return( doScanFloat( true ) );
                }
            }

            if( TokenLen == 2 ) {   /* just collected a 0x */
                BadTokenInfo = ERR_INVALID_HEX_CONSTANT;
                con.form = CON_ERR;
                if( diagnose_lex_error() ) {
                    CErr1( ERR_INVALID_HEX_CONSTANT );
                }
            }
        } else {    /* scan octal number */
            unsigned char   digit_mask;

            bad_token_type = ERR_INVALID_OCTAL_CONSTANT;
            con.form = CON_OCT;
            digit_mask = 0;
            // if collecting tokens for macro preprocessor, allow 8 and 9
            // since the argument may be used in with # or ##.
            while( c >= '0' && c <= '9' ) {
                digit_mask |= c;
                c = WriteBufferCharNextChar( c );
            }
            if( c == '.' || c == 'e' || c == 'E' ) {
                return( doScanFloat( false ) );
            }
            if( digit_mask & 0x08 ) {   /* if digit 8 or 9 somewhere */
                BadTokenInfo = ERR_INVALID_OCTAL_CONSTANT;
                con.form = CON_ERR;
                if( diagnose_lex_error() ) {
                    CErr1( ERR_INVALID_OCTAL_CONSTANT );
                }
            }
        }
    } else {    /* scan decimal number */
        bad_token_type = ERR_INVALID_CONSTANT;
        con.form = CON_DEC;
        c = NextChar();
        while( c >= '0' && c <= '9' ) {
            c = WriteBufferCharNextChar( c );
        }
        if( c == '.' || c == 'e' || c == 'E' ) {
            return( doScanFloat( false ) );
        }
    }
    switch( con.form ) {
    case CON_OCT:
        ov = Cnv8();
        break;
    case CON_HEX:
        ov = Cnv16();
        break;
    case CON_DEC:
        ov = Cnv10();
        break;
    case CON_ERR:
        ov = CNV_32;
    }
    con.suffix = SUFF_NONE;
    if( c == 'l' || c == 'L' ) {   // collect suffix
        c = WriteBufferCharNextChar( c );
        if( c == 'u' || c == 'U' ) {
            c = WriteBufferCharNextChar( c );
            con.suffix = SUFF_UL;
        } else if( c == 'l' || c == 'L' ) {
            c = WriteBufferCharNextChar( c );
            if( c == 'u' || c == 'U' ) {
                c = WriteBufferCharNextChar( c );
                con.suffix = SUFF_ULL;
            } else {
                con.suffix = SUFF_LL;
            }
        } else {
            con.suffix = SUFF_L;
        }
    } else if( c == 'u' || c == 'U' ) {
        c = WriteBufferCharNextChar( c );
        if( c == 'l' || c == 'L' ) {
            c = WriteBufferCharNextChar( c );
            if( c == 'l' || c == 'L' ) {
                c = WriteBufferCharNextChar( c );
                con.suffix = SUFF_ULL;
            } else {
                con.suffix = SUFF_UL;
            }
        } else if( c == 'i' || c == 'I' ) {
            c = WriteBufferCharNextChar( c );
            con.suffix = SUFF_UI;
        } else {
            con.suffix = SUFF_U;
        }
    } else if( c == 'i' || c == 'I' ) {
        c = WriteBufferCharNextChar( c );
        con.suffix = SUFF_I;
    }
    if( con.suffix == SUFF_UI || con.suffix == SUFF_I ) {
        unsigned_32 value;

        value = 0;
        while( c >= '0' && c <= '9' ) {
            value = value * 10 + c - '0';
            c = WriteBufferCharNextChar( c );
        }
        if( value == 64 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYP_LONG64;
            } else {
                ConstType = TYP_ULONG64;
            }
            if( ov == CNV_32 ) {
                U32ToU64( Constant, &Constant64 );
            }
        } else if( value == 32 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYP_LONG;
            } else {
                ConstType = TYP_ULONG;
            }
        } else if( value == 16 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYP_SHORT;
            } else {
                ConstType = TYP_USHORT;
            }
        } else if( value == 8 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYP_CHAR;
            } else {
                ConstType = TYP_UCHAR;
            }
        } else {
            if( diagnose_lex_error() ) {
                CErr1( ERR_INVALID_CONSTANT );
            }
        }
        if( ov == CNV_64 && value < 64 ) {
            BadTokenInfo = ERR_CONSTANT_TOO_BIG;
            Constant =  Constant64.u._32[I64LO32];
            if( diagnose_lex_error() ) {
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
        }
    } else if( ov == CNV_32 && con.suffix != SUFF_LL && con.suffix != SUFF_ULL ) {
        switch( con.suffix ) {
        case SUFF_NONE:
            if( Constant <= TARGET_INT_MAX ) {
                ConstType = TYP_INT;
#if TARGET_INT < TARGET_LONG
            } else if( Constant <= TARGET_UINT_MAX && con.form != CON_DEC ) {
                ConstType = TYP_UINT;
            } else if( Constant <= 0x7fffffffU ) {
                ConstType = TYP_LONG;
            } else {
                ConstType = TYP_ULONG;
            }
#else
            } else if( con.form != CON_DEC ) {
                ConstType = TYP_UINT;
            } else {
                ConstType = TYP_ULONG;
            }
#endif
            break;
        case SUFF_L:
            if( Constant <= 0x7FFFFFFFU ) {
                ConstType = TYP_LONG;
            } else {
                ConstType = TYP_ULONG;
            }
            break;
        case SUFF_U:
            ConstType = TYP_UINT;
#if TARGET_INT < TARGET_LONG
            if( Constant > TARGET_UINT_MAX ) {
                ConstType = TYP_ULONG;
            }
#endif
            break;
        case SUFF_UL:
            ConstType = TYP_ULONG;
            break;
        default:
            break;
        }
    } else {
        switch( con.suffix ) {
        case SUFF_NONE:
            ConstType = TYP_LONG64;
            if( Constant64.u._32[I64HI32] & 0x80000000 ) {
                ConstType = TYP_ULONG64;
            }
            break;
        case SUFF_L:
        case SUFF_LL:
            if( ov == CNV_32 ) {
                U32ToU64( Constant, &Constant64 );
            }
            if( Constant64.u._32[I64HI32] & 0x80000000 ) {
                ConstType = TYP_ULONG64;
            } else {
                ConstType = TYP_LONG64;
            }
            break;
        case SUFF_U:
        case SUFF_UL:
        case SUFF_ULL:
            if( ov == CNV_32 ) {
                U32ToU64( Constant, &Constant64 );
            }
            ConstType = TYP_ULONG64;
            break;
        default:
            break;
        }
    }
    token = T_CONSTANT;
    if( CharSet[c] & (C_AL | C_DI) ) {
        token = T_BAD_TOKEN;
        while( CharSet[c] & (C_AL | C_DI) ) {
            c = WriteBufferCharNextChar( c );
        }
    }
    WriteBufferNullChar();
    if( token == T_BAD_TOKEN ) {
        BadTokenInfo = bad_token_type;
    } else if( ov == CNV_OVR ) {
        BadTokenInfo = ERR_CONSTANT_TOO_BIG;
        if( diagnose_lex_error() ) {
            CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
        }
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
        if( last == T_COLON ) {        /* <: */
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
            // TODO: according to the standard, ":>" should be an
            // alternative token (digraph) for "]"
            // *token = T_RIGHT_BRACKET;   /* -> ] */
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
    if( (CharSet[NextChar()] & C_DE) && checkDelim2( &token, TokValue[CurrChar] ) ) {
        Buffer[TokenLen++] = CurrChar;
        if( (CharSet[NextChar()] & C_DE) && checkDelim2( &token, TokValue[CurrChar] ) ) {
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
                continue; //could be **/
            }
            if( c == LCHR_EOF ) {
                break;
            }
            if( c == '\n' ) {
                CppPrtChar( c );
                NewLineStartPos( SrcFile );
            } else if( c != '\r' && CompFlags.cpp_keep_comments ) {
                CppPrtChar( c );
            }
            c = NextChar();
        }
        CppComment( '\0' );
    } else {
        // make '/' anf '\n' a special characters so that we only have
        // to do one test for each character inside the main loop
        CharSet['/'] |= C_EX;           // make '/' special character
        CharSet['\n'] |= C_EX;          // make '\n' special character
        c = '\0';
        for( ; c != LCHR_EOF; ) {
            if( c == '\n' ) {
                NewLineStartPos( SrcFile );
                TokenLoc = SrcFileLoc = SrcFile->src_loc;
            }
            do {
                do {
                    prev_char = c;
                    c = *SrcFile->src_ptr++;
                } while( (CharSet[c] & C_EX) == 0 );
                c = GetCharCheck( c );
                if( c == LCHR_EOF ) {
                    break;
                }
            } while( (CharSet[c] & C_EX) == 0 );
            if( c == '/' ) {
                if( prev_char == '*' )
                    break;
                // get next character and see if it is '*' for nested comment
                c = NextChar();
                if( c == '*' ) {
                    c = NextChar();
                    if( c == '/' )
                        break;
                    CWarn2( WARN_NESTED_COMMENT, ERR_NESTED_COMMENT, CommentLoc.line );
                }
            }
            // NextChar might not be pointing to GetNextChar at this point
            while( c != LCHR_EOF && NextChar != GetNextChar ) {
                c = NextChar();
            }
        }
        CharSet['\n'] &= ~C_EX;         // undo '\n' special character
        CharSet['/'] &= ~C_EX;          // undo '/' special character
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
    NextChar();             // can't inline this copy of NextChar
    if( CurrChar == '=' ) {        /* if second char is an = */
        Buffer[TokenLen++] = '=';
        NextChar();
        token = T_DIV_EQUAL;
    } else if( CurrChar == '/' && !CompFlags.strict_ANSI ) {   /* if C++ // style comment */
        if( CompFlags.cpp_mode ) {
            CppComment( '/' );
        }
        CompFlags.scanning_cpp_comment = true;
        for( ;; ) {
            if( CurrChar == '\r' ) {
                /* some editors don't put linefeeds on end of lines */
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
            if( CompFlags.cpp_mode && CompFlags.cpp_keep_comments && CurrChar != '\r' ) {
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

#define OUTC(x)     if(ofn != NULL) ofn(x)

static msg_codes doScanHex( int max, escinp_fn ifn, escout_fn ofn )
/******************************************************************
 * Warning! this function is also used from cstring.c
 * cannot use Buffer array or NextChar function in any way
 * input and output is done using ifn or ofn functions
 */
{
    int             c;
    int             count;
    char            too_big;
    unsigned        value;

    too_big = 0;
    count = max;
    value = 0;
    for( ;; ) {
        c = ifn();
        if( max == 0 )
            break;
        if( ( CharSet[c] & (C_HX | C_DI) ) == 0 )
            break;
        OUTC( c );
        if( CharSet[c] & C_HX )
            c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        if( value & 0xF0000000 )
            too_big = 1;
        value = value * 16 + c - '0';
        --max;
    }
    Constant = value;
    if( count == max ) {                    /* no characters matched */
        return( ERR_INVALID_HEX_CONSTANT ); /* indicate no characters matched */
    }
    if( too_big ) {
        return( ERR_CONSTANT_TOO_BIG );
    }
    return( ERR_NONE );                     /* indicate characters were matched */
}

int ESCChar( int c, escinp_fn ifn, msg_codes *perr_msg, escout_fn ofn )
/**********************************************************************
 * Warning! this function is also used from cstring.c
 * cannot use Buffer array or NextChar function in any way
 * input and output is done using ifn or ofn functions
 */
{
    int         n;
    int         i;
    msg_codes   err_msg;

    if( c >= '0' && c <= '7' ) {    /* get octal escape sequence */
        n = 0;
        i = 3;
        while( i-- > 0 && c >= '0' && c <= '7' ) {
            OUTC( c );
            n = n * 8 + c - '0';
            c = ifn();
        }
    } else if( c == 'x' ) {         /* get hex escape sequence */
        OUTC( c );
        err_msg = doScanHex( 127, ifn, ofn );
        if( err_msg != ERR_NONE )
            *perr_msg = err_msg;
        n = Constant;
    } else {
        OUTC( c );
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
            /* for lazy QNX programmers */
            if( CompFlags.extensions_enabled ) {
                c = ESCAPE_n;
            }
            break;
#endif
        }
#if _CPU == 370
        _ASCIIOUT( c );
#endif
        n = c;
        ifn();
    }
    return( n );
}

static TOKEN doScanCharConst( DATA_TYPE char_type )
/**************************************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    int         c;
    int         i;
    int         n;
    TOKEN       token;
    int         value;

    value = 0;
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
            if( c == '\r' || c == '\n' ) {
                token = T_BAD_TOKEN;
                break;
            }
            if( c == '\\' ) {
                Buffer[TokenLen++] = '\\';
                c = NextChar();
                if( c >= '0' && c <= '7' ) {
                    n = c - '0';
                    Buffer[TokenLen++] = c;
                    c = NextChar();
                    if( c >= '0' && c <= '7' ) {
                        n = n * 8 + c - '0';
                        Buffer[TokenLen++] = c;
                        c = NextChar();
                        if( c >= '0' && c <= '7' ) {
                            n = n * 8 + c - '0';
                            Buffer[TokenLen++] = c;
                            NextChar();
                            if( n > 0377 && char_type != TYP_WCHAR ) {
                                BadTokenInfo = ERR_CONSTANT_TOO_BIG;
                                if( diagnose_lex_error() ) {
                                    CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                                }
                                n &= 0377;          // mask off high bits
                            }
                        }
                    }
                    c = n;
                } else {
                    c = ESCChar( c, NextChar, &BadTokenInfo, WriteBufferChar );
                    if( BadTokenInfo == ERR_CONSTANT_TOO_BIG ) {
                        if( diagnose_lex_error() ) {
                            CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                        }
                    }
                }
                if( char_type == TYP_WCHAR ) {
                    ++i;
                    value = (value << 8) + ((c & 0xFF00) >> 8);
                    c &= 0x00FF;
                }
            } else {
                Buffer[TokenLen++] = c;
                NextChar();
                if( CharSet[c] & C_DB ) {   /* if double-byte char */
                    c = (c << 8) + (CurrChar & 0x00FF);
                    if( char_type == TYP_WCHAR ) {
                        if( CompFlags.jis_to_unicode ) {
                            c = JIS2Unicode( c );
                        }
                    }
                    ++i;
                    value = (value << 8) + ((c & 0xFF00) >> 8);
                    c &= 0x00FF;
                    Buffer[TokenLen++] = CurrChar;
                    NextChar();
                } else if( char_type == TYP_WCHAR ) {
                    if( CompFlags.use_unicode ) {
                        c = UniCode[c];
                    } else if( CompFlags.jis_to_unicode ) {
                        c = JIS2Unicode( c );
                    }
                    ++i;
                    value = (value << 8) + ((c & 0xFF00) >> 8);
                    c &= 0x00FF;
#if _CPU == 370
                } else {
                    _ASCIIOUT( c );
#endif
                }
            }
            ++i;
            value = (value << 8) + c;
            /* handle case where user wants a \ but doesn't escape it */
            if( c == '\'' && CurrChar != '\'' ) {
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
    if( char_type == TYP_CHAR && CompFlags.signed_char ) {
        if( value < 256 && value > 127 ) {
            value -= 256;
        }
    }
    Constant = value;
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
                    CWarn1( WARN_MISSING_QUOTE, ERR_MISSING_QUOTE );
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
            c = WriteBufferCharNextChar( c );
            if( (CharSet[c] & C_WS) == 0 ) {
                ESCChar( c, NextChar, &BadTokenInfo, WriteBufferChar );
                if( diagnose_lex_error() ) {
                    if( BadTokenInfo == ERR_CONSTANT_TOO_BIG ) {
                        CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                    } else if( BadTokenInfo == ERR_CONSTANT_TOO_BIG ) {
                        CErr1( ERR_INVALID_HEX_CONSTANT );
                    }
                }
                c = CurrChar;
            }
        } else {
            /* if first character of a double-byte character, then
               save it and get the next one. */
            if( CharSet[c] & C_DB ) {
                c = WriteBufferCharNextChar( c );
            }
            c = WriteBufferCharNextChar( c );
        }
    }
    WriteBufferNullChar();
    if( CompFlags.trigraph_alert ) {
        CWarn1( WARN_LEVEL_1, ERR_EXPANDED_TRIGRAPH );
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

static TOKEN ScanWide( void )           // scan something that starts with L
/***************************/
{
    int         c;
    TOKEN       token;

    c = NextChar();
    if( c == '"' ) {                    // L"abc"
        token = doScanString( true );
    } else {                            // regular identifier
        Buffer[0] = 'L';
        TokenLen = 1;
        if( c == '\'' ) {               // L'a'
            Buffer[TokenLen++] = '\'';
            token = doScanCharConst( TYP_WCHAR );
        } else {                        // regular identifier
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
                c = *SrcFile->src_ptr++;
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
            if( c != '\r' && IS_PPCTL_NORMAL() ) {
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
            if( CompFlags.cpp_mode && IS_PPCTL_NORMAL() ) {
                CppPrtChar( '\n' );
            }
            NewLineStartPos( SrcFile );
            SrcFileLoc = SrcFile->src_loc;
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
    NewLineStartPos( SrcFile );
    SrcFileLoc = SrcFile->src_loc;
    if( PPControl & PPCTL_EOL )
        return( T_NULL );
    return( ChkControl() );
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

#if defined(__DOS__) || defined(__OS2__) || defined(__NT__)
    #define     SYS_EOF_CHAR 0x1A
#elif defined(__UNIX__) || defined(__RDOS__)
    #undef      SYS_EOF_CHAR
#else
    #error System end of file character not configured.
#endif

static TOKEN ScanInvalid( void )
/*******************************
 * TokenLen is alway lower then BUF_SIZE that
 * direct access to Buffer array is used
 */
{
    TOKEN   token;

    token = T_BAD_CHAR;
    Buffer[0] = CurrChar;
    TokenLen = 1;
#ifdef SYS_EOF_CHAR
    if( CurrChar == SYS_EOF_CHAR ) {
        CloseSrcFile( SrcFile );
        token = T_WHITE_SPACE;
    }
#endif
    Buffer[TokenLen] = '\0';
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
    TokenLoc = SrcFileLoc;         /* remember line token starts on */
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

TOKEN PPNextToken( void )                     // called from macro pre-processor
/***********************/
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

    /* save current status */
    saved_currchar = CurrChar;
    saved_nextchar = NextChar;
    saved_ungetchar = UnGetChar;
    saved_getcharcheck = GetCharCheck;
    oldSrcFile = SrcFile;

    SrcFile = &rescan_tmp_file;
    NextChar = reScanGetNextChar;
    UnGetChar = reScanGetNextCharUndo;
    GetCharCheck = reScanGetCharCheck;
    CompFlags.rescan_buffer_done = false;

    CurrChar = NextChar();
    CompFlags.doing_macro_expansion = true;     // return macros as ID's
    token = ScanToken();
    CompFlags.doing_macro_expansion = false;
    if( token == T_STRING && CompFlags.wide_char_string ) {
        token = T_LSTRING;
    }
    SrcFile->src_ptr--;

    SrcFile = oldSrcFile;
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
    ClassTable[LCHR_EOF] = SCAN_EOF;
#ifdef CHAR_MACRO
    ClassTable[LCHR_MACRO] = SCAN_MACRO;
#endif
    for( i = 0; (c = InitClassTable[i]) != '\0'; i += 2 ) {
        ClassTable[c] = InitClassTable[i + 1];
    }
    CurrChar = '\n';
    PPControl = PPCTL_NORMAL;
    CompFlags.scanning_comment = false;
    SizeOfCount = 0;
    NextChar = GetNextChar;
    UnGetChar = GetNextCharUndo;
    GetCharCheck = GetCharCheckFile;
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
    if( ScanFunc[SCAN_NUM] == ScanNum ) {
        ScanFunc[SCAN_NUM] = ScanPPDigit;
        ScanFunc[SCAN_DOT] = ScanPPDot;
        return( true );         // indicate changed to PP mode
    }
    return( false );            // indicate already in PP mode
}

void FiniPPScan( bool ppscan_mode )
/**********************************
 * called when CollectParms() and
 * CDefine() are finished gathering tokens
 */
{
    if( ppscan_mode ) {     // if InitPPScan() changed into PP mode
        ScanFunc[SCAN_NUM] = ScanNum; // reset back to normal mode
        ScanFunc[SCAN_DOT] = ScanDot;
    }
}

