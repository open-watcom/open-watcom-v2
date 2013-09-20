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
* Description:  C lexical analyzer. Closely coupled with preprocessor.
*
****************************************************************************/


#include "cvars.h"
#include "scan.h"
#include "escchars.h"
#include "asciiout.h"
#include "i64.h"
#include "kwhash.h"


extern  unsigned char   TokValue[];
extern  unsigned short  UniCode[];

enum scan_class {
    SCAN_NAME = 0,      // identifier
    SCAN_WIDE,          // L"abc" or L'a' or Lname
    SCAN_NUM,           // number that starts with a digit
    SCAN_QUESTION,      // '?'
    SCAN_SLASH,         // '/'
    SCAN_MINUS,         // '-'
    SCAN_EQUAL,         // '='
    SCAN_COLON,         // ':'
    SCAN_STAR,          // '*'
    SCAN_DELIM1,        // single character delimiter
    SCAN_DELIM2,        // one, two, or three byte delimiter
    SCAN_DOT,           // .
    SCAN_STRING,        // "string"
    SCAN_CHARCONST,     // 'a'
    SCAN_CR,            // '\r'
    SCAN_NEWLINE,       // '\n'
    SCAN_WHITESPACE,    // all whitespace
    SCAN_INVALID,       // all other characters
    SCAN_MACRO,         // get next token from macro
    SCAN_EOF            // end-of-file
};

static  FCB             rescan_tmp_file;
static  int             SavedCurrChar;          // used when get tokens from macro
static  unsigned char   ClassTable[ 260 ];

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
    '?',        SCAN_QUESTION,
    '/',        SCAN_SLASH,
    '-',        SCAN_MINUS,
    '=',        SCAN_EQUAL,
    ':',        SCAN_COLON,
    '*',        SCAN_STAR,
    '[',        SCAN_DELIM1,
    ']',        SCAN_DELIM1,
    '{',        SCAN_DELIM1,
    '}',        SCAN_DELIM1,
    '~',        SCAN_DELIM1,
    '.',        SCAN_DOT,
    '!',        SCAN_DELIM2,
    '#',        SCAN_DELIM2,
    '%',        SCAN_DELIM2,
    '&',        SCAN_DELIM2,
    '+',        SCAN_DELIM2,
    '<',        SCAN_DELIM2,
    '>',        SCAN_DELIM2,
    '^',        SCAN_DELIM2,
    '|',        SCAN_DELIM2,
    '_',        SCAN_NAME,
    'L',        SCAN_WIDE,
    '\0',       0
};

static  void    UnGetChar( int c );
static  TOKEN   ScanString( void );
static  TOKEN   CharConst( int char_type );
static  void    ScanComment( void );


void ReScanInit( char *ptr )                            /* 28-oct-92 */
{
    rescan_tmp_file.src_ptr = (unsigned char *)ptr;
}

char *ReScanPos( void )
{
    return( (char *)rescan_tmp_file.src_ptr );
}

static int ReScanBuffer( void )
{
    CurrChar = *SrcFile->src_ptr++;
    if( CurrChar == '\0' ) {
        CompFlags.rescan_buffer_done = 1;
    }
    return( CurrChar );
}

int InReScanMode( void )
{
    return( NextChar == ReScanBuffer );
}

static int SaveNextChar( void )
{
    int         c;

    c = NextChar();
    if( TokenLen >= BufSize - 2 )
        EnlargeBuffer( TokenLen * 2 );
    Buffer[ TokenLen ] = c;
    ++TokenLen;
    return( c );
}

unsigned hashpjw( const char *s )
{
    unsigned    h;
    char        c;

    h = *s++;
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

int CalcHash( const char *id, int len )
{
    unsigned    hash;

    len = len;
    hash = hashpjw( id );
    HashValue = hash % SYM_HASH_SIZE;
#if ( MACRO_HASH_SIZE > 0x0ff0 ) && ( MACRO_HASH_SIZE < 0x0fff )
    hash &= 0x0fff;
    if( hash >= MACRO_HASH_SIZE ) {
        hash -= MACRO_HASH_SIZE;
    }
#else
    hash = hash % MACRO_HASH_SIZE;
#endif
    MacHashValue = hash;
    return( HashValue );
}

TOKEN KwLookup( const char *buf, int len )
{
    char        *keyword;
    TOKEN       hash;

    hash = keyword_hash( buf, TokValue, len ) + FIRST_KEYWORD;

    /* look up id in keyword table */
    if( !CompFlags.c99_extensions ) {
        switch( hash ) {
        case T_INLINE:
            if( !CompFlags.extensions_enabled )
                hash = T_ID;
            break;
        case T_RESTRICT:
        case T__COMPLEX:
        case T__IMAGINARY:
        case T__BOOL:
        case T___OW_IMAGINARY_UNIT:
            hash = T_ID;
            break;
        }
    }

    keyword = Tokens[ hash ];
    if( *keyword == buf[ 0 ] ) {
        if( strcmp( keyword, buf ) == 0 ) {
            return( hash );
        }
    }

    /* not in keyword table, so must be just an identifier */
    return( T_ID );
}

TOKEN IdLookup( const char *buf, int len )
{
    MEPTR       mentry;

    mentry = MacroLookup( buf );
    if( mentry != NULL ) {      /* if this is a macro */
        NextMacro = mentry;     /* save pointer to it */
        return( T_MACRO );
    }
    return( KwLookup( buf, len ) );
}

static TOKEN doScanName( void )
{
    TOKEN       token;
    int         c;
    char        *p;

    c = CurrChar;
//      we know that NextChar will be pointing to GetNextChar()
//      so it is safe to inline the function here.
//      NextChar could also be pointing to ReScanBuffer().
    p = &Buffer[ TokenLen - 1 ];
    do {
        for( ; (CharSet[ c ] & (C_AL | C_DI)); ) {
            *p++ = c;
            c = *SrcFile->src_ptr++;
            if( p >= &Buffer[ BufSize - 16 ] ) {
                char    *oldbuf = Buffer;

                EnlargeBuffer( BufSize * 2 );
                p += Buffer - oldbuf;
            }
        }
        if( (CharSet[ c ] & C_EX) == 0 )
            break;
        c = GetCharCheck( c );
    } while( (CharSet[ c ] & (C_AL | C_DI)) );
    CurrChar = c;
    if( p >= &Buffer[ BufSize - 18 ] ) {
        char *oldbuf = Buffer;
        EnlargeBuffer( BufSize * 2 );
        p += Buffer - oldbuf;
    }
    *p = '\0';
    TokenLen = p - Buffer;
    CalcHash( Buffer, TokenLen );
    if( CompFlags.doing_macro_expansion )
        return( T_ID );
    if( CompFlags.pre_processing & PPCTL_NO_EXPAND )
        return( T_ID );
    token = IdLookup( Buffer, TokenLen );
    if( token == T_MACRO ) {
        if( NextMacro->macro_defn == 0 ) {
            return( SpecialMacro( NextMacro ) );
        }
        NextMacro->macro_flags |= MFLAG_REFERENCED;
        /* if macro requires parameters and next char is not a '('
        then this is not a macro */
        if( NextMacro->parm_count != 0 ) {
            SkipAhead();
            if( CurrChar != '(' ) {
                if( CompFlags.cpp_output ) {            /* 30-sep-90 */
                    Buffer[ TokenLen++ ] = ' ';
                    Buffer[ TokenLen ] = '\0';
                    return( T_ID );
                }
                return( KwLookup( Buffer, TokenLen ) );
            }
        }
        DoMacroExpansion();             /* start macro expansion */
        GetMacroToken();
        token = CurToken;
#if 0
        if( MacroPtr != NULL ) {
            SavedCurrChar = CurrChar;
            CurrChar = MACRO_CHAR;
        }
#endif
        if( token == T_NULL ) {
            token = T_WHITE_SPACE;
        }
    }
    return( token );
}

static TOKEN ScanName( void )
{
    Buffer[ 0 ] = CurrChar;
    TokenLen = 1;
    return( doScanName() );
}

static TOKEN ScanWide( void )        // scan something that starts with L
{
    int         c;
    TOKEN       token;

    Buffer[ 0 ] = 'L';
    c = NextChar();
    Buffer[ 1 ] = c;
    TokenLen = 2;
    if( c == '"' ) {                    // L"abc"
        token = ScanString();
        CompFlags.wide_char_string = 1;
    } else if( c == '\'' ) {            // L'a'
        token = CharConst( TYPE_WCHAR );
    } else {                            // regular identifier
        token = doScanName();
    }
    return( token );
}

static TOKEN ScanDotSomething( int c )
{
    if( c == '.' ) {
        c = SaveNextChar();
        if( c == '.' ) {
            NextChar();
            return( T_DOT_DOT_DOT );
        }
        CurrChar = '.';
        UnGetChar( c );
    }
    Buffer[ 1 ] = '\0';
    TokenLen = 1;
    return( T_DOT );
}

static TOKEN doScanFloat( void )
{
    int         c;

    BadTokenInfo = 0;
    c = CurrChar;
    if( c == '.' ) {
        while( (c = SaveNextChar()) >= '0' && c <= '9' )
            {}
        if( TokenLen == 2 ) {   /* .? */
            return( ScanDotSomething( c ) );
        }
    }
    CurToken = T_CONSTANT;
    if( c == 'e' || c == 'E' ) {
        c = SaveNextChar();
        if( c == '+' || c == '-' ) {
            c = SaveNextChar();
        }
        if( c < '0' || c > '9' ) {
            CurToken = T_BAD_TOKEN;
            BadTokenInfo = ERR_INVALID_FLOATING_POINT_CONSTANT;
        }
        while( c >= '0' && c <= '9' ) {
            c = SaveNextChar();
        }
    }
    if( c == 'f' || c == 'F' ) {
        NextChar();
        ConstType = TYPE_FLOAT;
    } else if( c == 'l' || c == 'L' ) {
        NextChar();
        if( CompFlags.use_long_double ) {
            ConstType = TYPE_LONG_DOUBLE;
        } else {
            ConstType = TYPE_DOUBLE;
        }
    } else {
        --TokenLen;
        ConstType = TYPE_DOUBLE;
    }
    Buffer[ TokenLen ] = '\0';
    return( CurToken );
}

static void doScanAsmToken( void )
{
    char        *p;
    int         c;

    p = &Buffer[ TokenLen ];
    c = NextChar();
    do {
        for( ; (CharSet[ c ] & (C_AL | C_DI)); ) {
            *p++ = c;
            c = *SrcFile->src_ptr++;
            if( p >= &Buffer[ BufSize - 16 ] ) {
                char    *oldbuf = Buffer;

                EnlargeBuffer( BufSize * 2 );
                p += Buffer - oldbuf;
            }
        }
        if( (CharSet[ c ] & C_EX) == 0 )
            break;
        c = GetCharCheck( c );
    } while( (CharSet[ c ] & (C_AL | C_DI)) );
    CurrChar = c;
    if( p >= &Buffer[ BufSize - 18 ] ) {
        char    *oldbuf = Buffer;

        EnlargeBuffer( BufSize * 2 );
        p += Buffer - oldbuf;
    }
    *p = '\0';
    TokenLen = p - Buffer;
}

static TOKEN doScanAsm( void )
{
    BadTokenInfo = 0;
    TokenLen = 0;
    do {
        Buffer[ TokenLen++ ] = CurrChar;
        doScanAsmToken();
    } while( CurrChar == '.' );
    CurToken = T_ID;
    return( CurToken );
}

static TOKEN ScanDot( void )
{
    if( CompFlags.pre_processing & PPCTL_ASM )
        return( doScanAsm() );

    Buffer[ 0 ] = '.';
    TokenLen = 1;
    return( doScanFloat() );
}

static TOKEN ScanPPNumber( void )
{
    int         c;
    int         prevc;
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
        c = SaveNextChar();
        if( CharSet[ c ] & (C_AL | C_DI) )
            continue;
        if( c == '.' )
            continue;
        if( c == '+' || c == '-' ) {
            if( prevc == 'e' || prevc == 'E' ) {
                if( CompFlags.extensions_enabled ) {
                    /* concession to existing practice...
                        #define A2 0x02
                        #define A3 0xaa0e+A2
                        // users want: 0xaa0e + 0x02
                        // not: 0xaa0e + A2 (but, this is what ISO C requires!)
                    */
                    prevc = c;  //advance to next
                    c = SaveNextChar();
                    if( !(CharSet[ c ] & C_DI) ) {
                        break;  //allow e+<digit>
                    }
                }
                continue;
            }
        }
        break;
    }
    --TokenLen;
    Buffer[ TokenLen ] = '\0';
    return( T_PPNUMBER );
}

static TOKEN ScanPPDigit( void )
{
    Buffer[ 0 ] = CurrChar;
    TokenLen = 1;
    return( ScanPPNumber() );
}

static TOKEN ScanPPDot( void )
{
    int         c;

    Buffer[ 0 ] = '.';
    TokenLen = 1;
    c = SaveNextChar();
    if( c >= '0' && c <= '9' ) {
        return( ScanPPNumber() );
    } else {
        return( ScanDotSomething( c ) );
    }
}

static int ScanHex( int max, const unsigned char **pbuf )
{
    int                 c;
    int                 count;
    char                too_big;
    unsigned long       value;

    too_big = 0;
    count = max;
    value = 0;
    for( ;; ) {
        if( pbuf == NULL ) {
            c = SaveNextChar();
        } else {
            c = *++*pbuf;
        }
        if( max == 0 )
            break;
        if( ( CharSet[ c ] & (C_HX|C_DI) ) == 0 )
            break;
        if( CharSet[ c ] & C_HX )
            c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        if( value & 0xF0000000 )
            too_big = 1;
        value = value * 16 + c - '0';
        --max;
    }
    Constant = value;
    if( count == max ) {                /* no characters matched */
        return( 0 );            /* indicate no characters matched */
/*          CErr1( ERR_INVALID_HEX_CONSTANT );  */
    }
    if( too_big ) {                                     /* 26-mar-91 */
        BadTokenInfo = ERR_CONSTANT_TOO_BIG;
        if( NestLevel == SkipLevel ) {  /* 10-sep-92 */
            CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
        }
    }
    return( 1 );                        /* indicate characters were matched */
}

typedef enum { CNV_32, CNV_64, CNV_OVR } cnv_cc;

static cnv_cc Cnv8( void )
{
    char    *curr;
    char    c;
    int     len;
    long    value;
    uint64  value64;
    cnv_cc  ret;

    curr = Buffer;
    len = TokenLen;
    value = 0;
    while( --len > 0 ) {
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
    } while( --len > 0 );
    Const64 = value64;
    return( ret );
}

static cnv_cc Cnv16( void )
{
    unsigned char   *curr;
    unsigned char   c;
    int             len;
    unsigned long   value;
    uint64          value64;
    cnv_cc          ret;

    curr = (unsigned char *)Buffer + 2;      // skip 0x thing
    len = TokenLen - 2;
    value = 0;
    while( --len > 0 ) {
        c = *curr;
        if( value & 0xF0000000 )
            goto is64; /* 64 bit */
        if( CharSet[ c ] & C_HX ) {
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
        if( CharSet[ c ] & C_HX ) {
            c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        }
        if( U64Cnv16( &value64, c-'0' ) ) {
            ret = CNV_OVR;
        }
        ++curr;
    } while( --len > 0 );
    Const64 = value64;
    return( ret );
}

static cnv_cc Cnv10( void )
{
    char            *curr;
    char            c;
    int             len;
    unsigned long   value;
    uint64          value64;
    cnv_cc          ret;

    curr = Buffer;      // skip 0x thing
    len = TokenLen;
    value = 0;
    while( --len > 0 ) {
        c = *curr;
        if( value >= 429496729 ) {          /* 15-feb-93 */
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
    } while( --len > 0 );
    Const64 = value64;
    return( ret );
}

static TOKEN ScanNum( void )
{
    int         c;
    int         bad_token_type;
    cnv_cc      ov;

    struct {
        enum { CON_DEC, CON_HEX, CON_OCT, CON_ERR } form;
        enum { SUFF_NONE,SUFF_U, SUFF_L,SUFF_UL,  SUFF_I, SUFF_UI,
               SUFF_LL,SUFF_ULL } suffix;
    } con;

    if( CompFlags.pre_processing & PPCTL_ASM )
        return( doScanAsm() );

    BadTokenInfo = 0;
    ov = CNV_32;
    Constant = 0;
    TokenLen = 1;
    c = CurrChar;
    Buffer[ 0 ] = c;
    if( c == '0' ) {
        c = SaveNextChar();
        if( c == 'x' || c == 'X' ) {
            bad_token_type = ERR_INVALID_HEX_CONSTANT;
            con.form = CON_HEX;
            for( ;; ) {
                c = SaveNextChar();
                if( ( CharSet[ c ] & (C_HX|C_DI) ) == 0 ) {
                    break;
                }
            }
            if( TokenLen == 3 ) {   /* just collected a 0x */
                BadTokenInfo = ERR_INVALID_HEX_CONSTANT;
                if( NestLevel == SkipLevel ) {
                    CErr1( ERR_INVALID_HEX_CONSTANT );
                    con.form = CON_ERR;
                }
            }
        } else {    /* scan octal number */
            unsigned char   digit_mask;

            bad_token_type = ERR_INVALID_OCTAL_CONSTANT;
            con.form = CON_OCT;
            digit_mask = 0;
            // if collecting tokens for macro preprocessor, allow 8 and 9
            // since the argument may be used in with # or ##. 28-oct-92
            while( c >= '0' && c <= '9' ) {
                digit_mask |= c;
                c = SaveNextChar();
            }
            if( c == '.' || c == 'e' || c == 'E' ) {
                return( doScanFloat() );
            }
            if( digit_mask & 0x08 ) {   /* if digit 8 or 9 somewhere */
                BadTokenInfo = ERR_INVALID_OCTAL_CONSTANT;
                con.form = CON_ERR;
                if( NestLevel == SkipLevel ) {
                    CErr1( ERR_INVALID_OCTAL_CONSTANT );
                }
            }
        }
    } else {    /* scan decimal number */
        bad_token_type = ERR_INVALID_CONSTANT;
        con.form = CON_DEC;
        do {
            c = SaveNextChar();
        } while( c >= '0' && c <= '9' );
        if( c == '.' || c == 'e' || c == 'E' ) {
            return( doScanFloat() );
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
        c = SaveNextChar();
        if( c == 'u' || c == 'U' ) {
            c = SaveNextChar();
            con.suffix = SUFF_UL;
        } else if( c == 'l' || c == 'L' ) {
            c = SaveNextChar();
            if( c == 'u' || c == 'U' ) {
                c = SaveNextChar();
                con.suffix = SUFF_ULL;
            } else {
                con.suffix = SUFF_LL;
            }
        } else {
            con.suffix = SUFF_L;
        }
    } else if( c == 'u' || c == 'U' ) {
        c = SaveNextChar();
        if( c == 'l' || c == 'L' ) {
            c = SaveNextChar();
            if( c == 'l' || c == 'L' ) {
                c = SaveNextChar();
                con.suffix = SUFF_ULL;
            } else {
                con.suffix = SUFF_UL;
            }
        } else if( c == 'i' || c == 'I' ) {
            c = SaveNextChar();
            con.suffix = SUFF_UI;
        } else {
            con.suffix = SUFF_U;
        }
    } else if( c == 'i' || c == 'I' ) {
        c = SaveNextChar();
        con.suffix = SUFF_I;
    }
    if( con.suffix == SUFF_UI || con.suffix == SUFF_I ) {
        unsigned_32 value;

        value  = 0;
        while( c >= '0' && c <= '9' ) {
            value = value * 10 + c - '0';
            c = SaveNextChar();
        }
        if( value == 64 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYPE_LONG64;
            } else {
                ConstType = TYPE_ULONG64;
            }
            if( ov == CNV_32 ) {
                U32ToU64( Constant, &Const64 );
            }
        } else if( value == 32 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYPE_LONG;
            } else {
                ConstType = TYPE_ULONG;
            }
        } else if( value == 16 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYPE_SHORT;
            } else {
                ConstType = TYPE_USHORT;
            }
        } else if( value == 8 ) {
            if( con.suffix == SUFF_I ) {
                ConstType = TYPE_CHAR;
            } else {
                ConstType = TYPE_UCHAR;
            }
        } else {
            if( NestLevel == SkipLevel ) {
                CErr1( ERR_INVALID_CONSTANT );
            }
        }
        if( ov == CNV_64 && value < 64 ) {
            BadTokenInfo = ERR_CONSTANT_TOO_BIG;
            if( NestLevel == SkipLevel ) {      /* 10-sep-92 */
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
            Constant =  Const64.u._32[ I64LO32 ];
        }
    } else if( ov == CNV_32 && con.suffix != SUFF_LL && con.suffix != SUFF_ULL ) {
        switch( con.suffix ) {
        case SUFF_NONE:
            if( Constant <= TARGET_INT_MAX ) {
                ConstType = TYPE_INT;
#if TARGET_INT < TARGET_LONG
            } else if( Constant <= TARGET_UINT_MAX && con.form != CON_DEC ) {
                ConstType = TYPE_UINT;
            } else if( Constant <= 0x7ffffffful ) {
                ConstType = TYPE_LONG;
            } else {
                ConstType = TYPE_ULONG;
            }
#else
            } else if( con.form != CON_DEC ) {
                ConstType = TYPE_UINT;
            } else {
                ConstType = TYPE_ULONG;
            }
#endif
            break;
        case SUFF_L:
            if( Constant <= 0x7FFFFFFFul ) {     /* 13-sep-89 */
                ConstType = TYPE_LONG;
            } else {
                ConstType = TYPE_ULONG;
            }
            break;
        case SUFF_U:
            ConstType = TYPE_UINT;
#if TARGET_INT < TARGET_LONG
            if( Constant > TARGET_UINT_MAX ) {
                ConstType = TYPE_ULONG;
            }
#endif
            break;
        case SUFF_UL:
            ConstType = TYPE_ULONG;
            break;
        default:
            break;
        }
    } else {
        switch( con.suffix ) {
        case SUFF_NONE:
            ConstType = TYPE_LONG64;
            if( Const64.u._32[ I64HI32 ] & 0x80000000 ) {
                ConstType = TYPE_ULONG64;
            }
            break;
        case SUFF_L:
        case SUFF_LL:
            if( ov == CNV_32 ) {
                U32ToU64( Constant, &Const64 );
            }
            if( Const64.u._32[ I64HI32 ] & 0x80000000 ) {
                ConstType = TYPE_ULONG64;
            } else {
                ConstType = TYPE_LONG64;
            }
            break;
        case SUFF_U:
        case SUFF_UL:
        case SUFF_ULL:
            if( ov == CNV_32 ) {
                U32ToU64( Constant, &Const64 );
            }
            ConstType = TYPE_ULONG64;
            break;
        default:
            break;
        }
    }
    if( CompFlags.pre_processing && (CharSet[ c ] & (C_AL | C_DI)) ) {
        do {
            c = SaveNextChar();
        } while( CharSet[ c ] & (C_AL | C_DI) );
        --TokenLen;
        Buffer[ TokenLen ] = '\0';
        BadTokenInfo = bad_token_type;
        return( T_BAD_TOKEN );
    } else {
        if( ov == CNV_OVR ) {
            BadTokenInfo = ERR_CONSTANT_TOO_BIG;
            if( NestLevel == SkipLevel ) {      /* 10-sep-92 */
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
        }
        --TokenLen;
        Buffer[ TokenLen ] = '\0';
        return( T_CONSTANT );
    }
}

static TOKEN ScanQuestionMark( void )
{
    NextChar();
    Buffer[ 0 ] = '?';
    Buffer[ 1 ] = '\0';
    return( T_QUESTION );
}

static TOKEN ScanSlash( void )
{
    int         c;

    c = NextChar();         // can't inline this copy of NextChar
    if( c == '=' ) {        /* if second char is an = */
        NextChar();
        Buffer[ 0 ] = '/';
        Buffer[ 1 ] = '=';
        Buffer[ 2 ] = '\0';
        return( T_DIV_EQUAL );
    } else if( c == '/' && !CompFlags.strict_ANSI ) {   /* if C++ // style comment */
        if( CompFlags.cpp_output ) {
            CppComment( '/' );
        }
        CompFlags.scanning_cpp_comment = 1;
        for( ;; ) {
            c = CurrChar;
            NextChar();
            if( CurrChar == EOF_CHAR )
                break;
            if( CurrChar == '\0' )
                break;
            /* swallow up the next line if this one ends with \ */
            /* some editors don't put linefeeds on end of lines */
            if( CurrChar == '\n' || c == '\r' )
                break;
            if( CompFlags.cpp_output && CompFlags.keep_comments && CurrChar != '\r' ) {
                CppPrtChar( CurrChar );
            }
        }
        if( CompFlags.cpp_output ) {
            CppComment( '\0' );
        }
        CompFlags.scanning_cpp_comment = 0;
        Buffer[ 0 ] = ' ';
        Buffer[ 1 ] = '\0';
        return( T_WHITE_SPACE );
    } else if( c == '*' ) {
        ScanComment();
        Buffer[ 0 ] = ' ';
        Buffer[ 1 ] = '\0';
        return( T_WHITE_SPACE );
    } else {
        Buffer[ 0 ] = '/';
        Buffer[ 1 ] = '\0';
        return( T_DIV );
    }
}

static TOKEN ScanDelim1( void )
{
    TOKEN       token;

    Buffer[ 0 ] = CurrChar;
    Buffer[ 1 ] = '\0';
    token = TokValue[ CurrChar ];
    NextChar();
    return( token );
}

static TOKEN ScanMinus( void )
{
    int         chr2;

    Buffer[ 0 ] = '-';
    chr2 = NextChar();          // can't inline this copy of NextChar
    Buffer[ 1 ] = chr2;
    if( chr2 == '>' ) {
        Buffer[ 2 ] = '\0';
        NextChar();
        return( T_ARROW );
    } else if( chr2 == '=' ) {
        Buffer[ 2 ] = '\0';
        NextChar();
        return( T_MINUS_EQUAL );
    } else if( chr2 == '-' ) {
        Buffer[ 2 ] = '\0';
        NextChar();
        return( T_MINUS_MINUS );
    } else {
        Buffer[ 1 ] = '\0';
        return( T_MINUS );
    }
}

static TOKEN ScanEqual( void )
{
    Buffer[ 0 ] = '=';
    if( NextChar() == '=' ) {
        NextChar();
        Buffer[ 1 ] = '=';
        Buffer[ 2 ] = '\0';
        return( T_EQ );
    } else {
        Buffer[ 1 ] = '\0';
        return( T_EQUAL );
    }
}

static TOKEN ScanStar( void )
{
    Buffer[ 0 ] = '*';
    if( NextChar() == '=' ) {
        NextChar();
        Buffer[ 1 ] = '=';
        Buffer[ 2 ] = '\0';
        return( T_TIMES_EQUAL );
    } else {
        Buffer[ 1 ] = '\0';
        return( T_TIMES );
    }
}

static TOKEN ScanColon( void )
{
    int         chr2;

    Buffer[ 0 ] = ':';
    chr2 = NextChar();
    if( chr2 == '>' ) {
        NextChar();
        Buffer[ 1 ] = '>';
        Buffer[ 2 ] = '\0';
        return( T_SEG_OP );
#if _CPU == 370
    } else if( chr2 == ')' ) {
        NextChar();
        Buffer[ 1 ] = ')';
        Buffer[ 2 ] = '\0';
        return( T_RIGHT_BRACKET );
#endif
    } else {
        Buffer[ 1 ] = '\0';
        return( T_COLON );
    }
}

static TOKEN ScanDelim2( void )
{
    int             c;
    int             chr2;
    TOKEN           tok;
    unsigned char   chrclass;

    c = CurrChar;
    Buffer[ 0 ] = c;
    Buffer[ 1 ] = '\0';
    chrclass = TokValue[ c ];
    tok = chrclass & C_MASK;
    chr2 = NextChar();          // can't inline this copy of NextChar
    if( chr2 == '=' ) {         /* if second char is an = */
        if( chrclass & EQ ) {   /* and = is valid second char */
            ++tok;
            NextChar();
            Buffer[ 1 ] = '=';
            Buffer[ 2 ] = '\0';
        }
    } else if( chr2 == c ) {    /* if second char is same as first */
        if( chrclass & DUP ) {  /* and duplicate is valid */
            tok += 2;
            Buffer[ 1 ] = c;
            Buffer[ 2 ] = '\0';
            if( NextChar() == '=' ) {
                if( tok == T_LSHIFT || tok == T_RSHIFT ) {
                    ++tok;
                    NextChar();
                    Buffer[ 2 ] = '=';
                    Buffer[ 3 ] = '\0';
                }
            }
        }
#if _CPU == 370
    } else if( c == '(' && chr2 == ':' ) {
        tok = T_LEFT_BRACKET;
        NextChar();
        Buffer[ 1 ] = ':';
        Buffer[ 2 ] = '\0';
#endif
    }
    return( tok );
}

static void ScanComment( void )
{
    int         c;
    int         prev_char;

    CommentLoc = TokenLoc;
    CompFlags.scanning_comment = 1;
    if( CompFlags.cpp_output ) {        // 30-dec-93
        CppComment( '*' );
        c = NextChar();
        for( ;; ) {
            if( c == '*' ) {
                c = NextChar();
                if( c == '/' )
                    break;
                if( CompFlags.keep_comments ) {
                    CppPrtChar( '*' );
                }
                continue; //could be **/
            }
            if( c == EOF_CHAR ) {
                CppComment( '\0' );
                return;
            }
            if( c == '\n' ) {
                CppPrtChar( c );
            } else if( c != '\r' && CompFlags.keep_comments ) {
                CppPrtChar( c );
            }
            c = NextChar();
        }
        CppComment( '\0' );
    } else {
        // make '/' a special character so that we only have to do one test
        // for each character inside the main loop
        CharSet[ '/' ] |= C_EX;           // make '/' special character
        c = '\0';
        for( ;; ) {
            if( c == '\n' ) {
                TokenLoc = SrcFileLoc = SrcFile->src_loc;
            }
            do {
                do {
                prev_char = c;
                    c = *SrcFile->src_ptr++;
                } while( (CharSet[ c ] & C_EX) == 0 );
                c = GetCharCheck( c );
                if( c == EOF_CHAR ) {
                    return;
                }
            } while( (CharSet[ c ] & C_EX) == 0 );
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
            while( NextChar != GetNextChar ) {
                c = NextChar();
                if( c == EOF_CHAR ) {
                    return;
                }
            }
        }
        CharSet[ '/' ] &= ~C_EX;          // undo '/' special character
    }
    CompFlags.scanning_comment = 0;
    NextChar();
}

static TOKEN CharConst( int char_type )
{
    int         c;
    int         i;
    int         n;
    TOKEN       token;
    long        value;
    bool        error;

    c = SaveNextChar();
    if( c == '\'' ) {                           /* 05-jan-95 */
        NextChar();
        Buffer[ TokenLen ] = '\0';
        BadTokenInfo = ERR_INV_CHAR_CONSTANT;
        return( T_BAD_TOKEN );
    }
    BadTokenInfo = 0;
    token = T_CONSTANT;
    i = 0;
    value = 0;
    error = FALSE;
    for( ;; ) {
        if( c == '\r' || c == '\n' ) {
            token = T_BAD_TOKEN;
            break;
        }
        if( c == '\\' ) {
            c = SaveNextChar();
            if( c >= '0' && c <= '7' ) {              /* 09-jan-94 */
                n = c - '0';
                c = SaveNextChar();
                if( c >= '0' && c <= '7' ) {
                    n = n * 8 + c - '0';
                    c = SaveNextChar();
                    if( c >= '0' && c <= '7' ) {
                        n = n * 8 + c - '0';
                        SaveNextChar();
                        if( n > 0377 && char_type != TYPE_WCHAR ) {
                            BadTokenInfo = ERR_CONSTANT_TOO_BIG;
                            if( NestLevel == SkipLevel ) {
                                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                            }
                            n &= 0377;          // mask off high bits
                        }
                    }
                }
                c = n;
            } else {
                c = ESCChar( c, NULL, &error );
            }
            if( char_type == TYPE_WCHAR ) {     /* 02-aug-91 */
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
            }
        } else {
            if( CharSet[ c ] & C_DB ) {   /* if double-byte char */
                c = (c << 8) + (SaveNextChar() & 0x00FF);
                if( char_type == TYPE_WCHAR ) { /* 15-jun-93 */
                    if( CompFlags.jis_to_unicode ) {
                        c = JIS2Unicode( c );
                    }
                }
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
            } else if( char_type == TYPE_WCHAR ) {  /* 02-aug-91 */
                if( CompFlags.use_unicode ) {
                    c = UniCode[ c ];
                } else if( CompFlags.jis_to_unicode ) {
                    c = JIS2Unicode( c );
                }
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
            } else {
                _ASCIIOUT( c );
            }
            SaveNextChar();
        }
        ++i;
        value = (value << 8) + c;
        /* handle case where user wants a \ but doesn't escape it */
        if( c == '\'' && CurrChar != '\'' ) {
            if( !CompFlags.cpp_output ) {       /* 08-feb-93 */
                token = T_BAD_TOKEN;
                break;
            }
        }
        c = CurrChar;
        if( c == '\'' )
            break;
        if( i >= 4 ) {
            if( !CompFlags.cpp_output ) {       /* 08-feb-93 */
                token = T_BAD_TOKEN;
                break;
            }
        }
    }
    if( token == T_BAD_TOKEN ) {
        BadTokenInfo = ERR_INV_CHAR_CONSTANT;
    } else {
        NextChar();
        if( error ) {                      /* 16-nov-94 */
            BadTokenInfo = ERR_INVALID_HEX_CONSTANT;
            token = T_BAD_TOKEN;
        }
    }
    Buffer[ TokenLen ] = '\0';
    ConstType = char_type;
    if( char_type == TYPE_CHAR && CompFlags.signed_char ) {
        if( value < 256 && value > 127 ) {
            value -= 256;
        }
    }
    Constant = value;
    return( token );
}

static TOKEN ScanCharConst( void )
{
    Buffer[ 0 ] = '\'';
    TokenLen = 1;
    return( CharConst( TYPE_CHAR ) );
}

static TOKEN ScanString( void )
{
    int         c;
    int         ok;
    bool        error;

    ok = 0;
    error = FALSE;
    CompFlags.wide_char_string = 0;
    CompFlags.trigraph_alert = 0;
    c = NextChar();
    Buffer[ 0 ] = c;
    TokenLen = 1;
    for( ;; ) {
        if( c == '\n' ) {
            if( NestLevel != SkipLevel ) {
                if ( CompFlags.extensions_enabled ) {
                    CWarn1( WARN_MISSING_QUOTE, ERR_MISSING_QUOTE );
                    ok = 1;
                } else {
                    CErr1( ERR_MISSING_QUOTE );
                }
            }
            break;
        }
        if( c == EOF_CHAR )
            break;
        if( c == '"' ) {
            NextChar();
            ok = 1;
            break;
        }

        if( c == '\\' ) {               /* 23-mar-90 */
            if( TokenLen > BufSize - 32 ) {
                EnlargeBuffer( TokenLen * 2 );
            }
            c = NextChar();
            Buffer[ TokenLen++ ] = c;
            if( (CharSet[ c ] & C_WS) == 0 ) {    /* 04-nov-88 */
                ESCChar( c, NULL, &error );
            }
            c = CurrChar;
        } else {
            /* if first character of a double-byte character, then
               save it and get the next one.    10-nov-89  */
            if( CharSet[ c ] & C_DB ) {
                SaveNextChar();
            }
            if( TokenLen > BufSize - 32 ) {
                EnlargeBuffer( TokenLen * 2 );
            }
            c = NextChar();
            Buffer[ TokenLen++ ] = c;
        }
    }
    Buffer[ TokenLen - 1 ] = '\0';
    CLitLength = TokenLen;
    if( CompFlags.trigraph_alert ) {
        CWarn1( WARN_LEVEL_1, ERR_EXPANDED_TRIGRAPH );
    }
    if( ok )
        return( T_STRING );
    --TokenLen;         /* '\n' or EOF_CHAR don't print nicely */
    Buffer[ TokenLen - 1 ] = '\0';
    BadTokenInfo = ERR_MISSING_QUOTE;
    return( T_BAD_TOKEN );
}

int ESCChar( int c, const unsigned char **pbuf, bool *error )
{
    int         n;
    int         i;

    if( c >= '0' && c <= '7' ) {          /* get octal escape sequence */
        n = 0;
        i = 3;
        while( c >= '0' && c <= '7' ) {
            n = n * 8 + c - '0';
            if( pbuf == NULL ) {
                c = SaveNextChar();
            } else {
                c = *++*pbuf;
            }
            --i;
            if( i == 0 ) {
                break;
            }
        }
    } else if( c == 'x' ) {         /* get hex escape sequence */
        if( ScanHex( 127, pbuf ) ) {
            n = Constant;
        } else {                        /*  '\xz' where z is not a hex char */
            *error = TRUE;
            n = 'x';
        }
    } else {
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
        _ASCIIOUT( c );
        n = c;
        if( pbuf == NULL ) {
            SaveNextChar();
        } else {
            ++*pbuf;
        }
    }
    return( n );
}

static TOKEN ScanWhiteSpace( void )
{
    int         c;

    if( NextChar == getCharAfterBackSlash ) {
        do {
            c = NextChar();
        } while( CharSet[ c ] & C_WS );
    } else {
        do {
            do {
                c = *SrcFile->src_ptr++;
            } while( CharSet[ c ] & C_WS );
            if( (CharSet[ c ] & C_EX) == 0 )
                break;
            c = GetCharCheck( c );
        } while( CharSet[ c ] & C_WS );
        CurrChar = c;
    }
    return( T_WHITE_SPACE );
}

static void SkipWhiteSpace( int c )
{
    if( !CompFlags.cpp_output ) {
        ScanWhiteSpace();
    } else {
        for( ;; ) {
            if( (CharSet[ c ] & C_WS) == 0 )
                break;
            if( c != '\r' && CompFlags.pre_processing == 0 ) {
                CppPrtChar( c );
            }
            c = NextChar();
        }
    }
}


void SkipAhead( void )
{
    for( ;; ) {
        for( ;; ) {
            if( CharSet[ CurrChar ] & C_WS ) {
                SkipWhiteSpace( CurrChar );
            }
            if( CurrChar != '\n' )
                break;
            if( CompFlags.cpp_output && CompFlags.pre_processing == 0 ) {
                CppPrtChar( '\n' );
            }
            SrcFileLoc = SrcFile->src_loc;
            NextChar();
        }
        if( CurrChar != '/' )
            break;
        NextChar();
        if( CurrChar == '*' ) {
            TokenLoc = SrcFileLoc;
            ScanComment();
        } else {
            UnGetChar( CurrChar );
            CurrChar = '/';
            break;
        }
    }
}

static TOKEN ScanNewline( void )
{
    SrcFileLoc = SrcFile->src_loc;
    if( CompFlags.pre_processing & PPCTL_EOL )
        return( T_NULL );
    return( ChkControl() );
}

static TOKEN ScanCarriageReturn( void )
{
    if( NextChar() == '\n' ) {
        return( ScanNewline() );
    } else {
        return( ScanWhiteSpace() );
    }
}

#if defined(__DOS__) || defined(__OS2__) || defined(__NT__) || defined(__OSI__)
    #define     SYS_EOF_CHAR 0x1A
#elif defined(__UNIX__)
    #undef      SYS_EOF_CHAR
#else
    #error System end of file character not configured.
#endif

static TOKEN ScanInvalid( void )
{
    Buffer[ 0 ] = CurrChar;
    Buffer[ 1 ] = '\0';
#ifdef SYS_EOF_CHAR
    if( Buffer[ 0 ] == SYS_EOF_CHAR ) {
        CloseSrcFile( SrcFile );                /* 13-sep-94 */
        return( T_WHITE_SPACE );
    }
#endif
    NextChar();
    return( T_BAD_CHAR );
}

static TOKEN ScanMacroToken( void )
{
    GetMacroToken();
    if( CurToken == T_NULL ) {
        if( CompFlags.cpp_output ) {
            CppPrtChar( ' ' );
        }
        CurrChar = SavedCurrChar;
        CurToken = ScanToken();
    }
    return( CurToken );
}

static TOKEN ScanEof( void )
{
    return( T_EOF );
}

static TOKEN (*ScanFunc[])( void ) = {
    ScanName,
    ScanWide,
    ScanNum,
    ScanQuestionMark,
    ScanSlash,
    ScanMinus,
    ScanEqual,
    ScanColon,
    ScanStar,
    ScanDelim1,
    ScanDelim2,
    ScanDot,
    ScanString,
    ScanCharConst,
    ScanCarriageReturn,
    ScanNewline,
    ScanWhiteSpace,
    ScanInvalid,
    ScanMacroToken,
    ScanEof
};

TOKEN ScanToken( void )
{
    TokenLoc = SrcFileLoc;         /* remember line token starts on */
//    TokenLen = 1;
//    Buffer[ 0 ] = CurrChar;
    return( (*ScanFunc[ ClassTable[ CurrChar ] ])() );
}

TOKEN NextToken( void )
{
    do {
        CurToken = T_NULL;
        if( MacroPtr != NULL ) {
            GetMacroToken();
        }
        if( CurToken == T_NULL ) {
            CurToken = ScanToken();
        }
    } while( CurToken == T_WHITE_SPACE );
#ifdef FDEBUG
    DumpToken();
#endif
    return( CurToken );
}

TOKEN PPNextToken( void )                     // called from macro pre-processor
{
    do {
        if( MacroPtr != NULL ) {
            GetMacroToken();
            if( CurToken == T_NULL ) {
                if( CompFlags.cpp_output ) {
                    CppPrtChar( ' ' );
                }
                CurToken = ScanToken();
            }
        } else {
            CurToken = ScanToken();
        }
    } while( CurToken == T_WHITE_SPACE );
    return( CurToken );
}

int ReScanToken( void )
{
    FCB             *oldSrcFile;
    int             saved_currchar;
    int             (*saved_nextchar)( void );

    saved_currchar = CurrChar;
    saved_nextchar = NextChar;
    oldSrcFile = SrcFile;
    SrcFile = &rescan_tmp_file;
    CompFlags.rescan_buffer_done = 0;
    NextChar = ReScanBuffer;
    CurrChar = ReScanBuffer();
    CompFlags.doing_macro_expansion = 1;        // return macros as ID's
    CurToken = ScanToken();
    CompFlags.doing_macro_expansion = 0;
    if( CurToken == T_STRING && CompFlags.wide_char_string ) {
        CurToken = T_LSTRING;                   /* 12-nov-92 */
    }
    --SrcFile->src_ptr;
    SrcFile = oldSrcFile;
    CurrChar = saved_currchar;
    NextChar = saved_nextchar;
    if( CompFlags.rescan_buffer_done == 0 ) {
        return( 1 );
    }
    return( 0 );
}

static void UnGetChar( int c )
{
    if( NextChar == ReScanBuffer ) {            /* 29-feb-92 */
        --SrcFile->src_ptr;
        CompFlags.rescan_buffer_done = 0;
    } else {
        GetNextCharUndo( c );
    }
}

void ScanInit( void )
{
    int         i;
    int         c;

    memset( &ClassTable[ 0 ],   SCAN_INVALID, 256 );
    memset( &ClassTable[ 'A' ], SCAN_NAME,    26 );
    memset( &ClassTable[ 'a' ], SCAN_NAME,    26 );
    memset( &ClassTable[ '0' ], SCAN_NUM,     10 );
    ClassTable[ EOF_CHAR ] = SCAN_EOF;
    ClassTable[ MACRO_CHAR ] = SCAN_MACRO;
    for( i = 0; ; i += 2 ) {
        c = InitClassTable[ i ];
        if( c == '\0' )
            break;
        ClassTable[ c ] =  InitClassTable[ i + 1 ];
    }
    CurrChar = '\n';
    CompFlags.pre_processing = PPCTL_NORMAL;
    CompFlags.scanning_comment = 0;
    SizeOfCount = 0;
    NextChar = GetNextChar;
}

// called by CollectParms() to gather tokens for macro parms
// and CDefine() to gather tokens for macro definition
// example usage:
//      int ppscan_mode;
//      ppscan_mode = InitPPScan();
//      CollectParms();
//      FiniPPScan( ppscan_mode );
int InitPPScan( void )
{
    if( ScanFunc[ SCAN_NUM ] == ScanNum ) {
        ScanFunc[ SCAN_NUM ] = ScanPPDigit;
        ScanFunc[ SCAN_DOT ] = ScanPPDot;
        return( 1 );            // indicate changed to PP mode
    }
    return( 0 );                // indicate already in PP mode
}

// called when CollectParms() and CDefine() are finished gathering tokens
void FiniPPScan( int ppscan_mode )
{
    if( ppscan_mode == 1 ) {    // if InitPPScan() changed into PP mode
        ScanFunc[ SCAN_NUM ] = ScanNum; // reset back to normal mode
        ScanFunc[ SCAN_DOT ] = ScanDot;
    }
}
