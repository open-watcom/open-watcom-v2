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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "i64.h"
#include "cvars.h"
#include "scan.h"
#include "escchars.h"
#include "asciiout.h"

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
    SCAN_STRING2,       // "string" continued
    SCAN_CHARCONST,     // 'a'
    SCAN_CR,            // '\r'
    SCAN_NEWLINE,       // '\n'
    SCAN_WHITESPACE,    // all whitespace
    SCAN_INVALID,       // all other characters
    SCAN_MACRO,         // get next token from macro
    SCAN_EOF            // end-of-file
};

static  char    *ReScanPtr;
static  int     SavedCurrChar;          // used when get tokens from macro
unsigned char   ClassTable[260];

extern  char    TokValue[];
extern  unsigned short __FAR UniCode[];


unsigned char InitClassTable[] = {
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

void ReScanInit( char *ptr )                            /* 28-oct-92 */
{
    ReScanPtr = ptr;
}

char *ReScanPos()
{
    return( ReScanPtr );
}

/*
 * Needs to be global so that it works with the 9.0 overlay manager
 */
int ReScanBuffer()
{
    CurrChar = *ScanCharPtr++;
    if( CurrChar == '\0' ) {
        CompFlags.rescan_buffer_done = 1;
    }
    return( CurrChar );
}


int SaveNextChar()
{
    int         c;

    c = NextChar();
    if( TokenLen < BUF_SIZE - 2 ) {
        Buffer[TokenLen] = c;
        ++TokenLen;
    } else if( TokenLen == BUF_SIZE - 2 ) { /* 10-aug-88 */
        if( NestLevel == SkipLevel ) {  /* 07-jun-92 */
            CErr1( ERR_TOKEN_TRUNCATED );
        }
        ++TokenLen;
    }
    return( c );
}

unsigned hashpjw( char *s )
{
    unsigned h;
    char     c;

    h = *s++;
    if( h != 0 ) {
        c = *s++;
        if( c != '\0' ) {
            h = ( h << 4 ) + c;
            for(;;) {
                h &= 0x0fff;
                c = *s++;
                if( c == '\0' ) break;
                h = ( h << 4 ) + c;
                h = ( h ^ (h >> 12) ) & 0x0fff;
                c = *s++;
                if( c == '\0' ) break;
                h = ( h << 4 ) + c;
                h = h ^ (h >> 12);
            }
        }
    }
    return( h );
}

int CalcHash( char *id, int len )
{
    unsigned    hash;

    hash = len + TokValue[ id[ FIRST_INDEX ] - ' ' ] * FIRST_SCALE;
#if LAST_INDEX > 0
    if( len >= LAST_INDEX+1 ) {
        hash += TokValue[ id[len-(LAST_INDEX+1)] - ' ' ] * LAST_SCALE;
    }
#else
    hash += TokValue[ id[len-(LAST_INDEX+1)] - ' ' ] * LAST_SCALE;
#endif
    hash &= KEYWORD_HASH_MASK;
#ifdef KEYWORD_HASH_EXTRA
    if( hash >= KEYWORD_HASH ) {
        hash -= KEYWORD_HASH;
    }
#endif
    KwHashValue = hash;
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


int KwLookup()
{
    int         hash;
    char        *keyword;
    /*  lookup id in keyword table */

    hash = KwHashValue + FIRST_KEYWORD;
    keyword = Tokens[ hash ];
    if( *keyword == Buffer[0] ) {
        if( strcmp( keyword, Buffer ) == 0 )  return( hash );
    }

    /* not in keyword table, so must be just an identifier */
    return( T_ID );
}

int IdLookup()
{
    MEPTR       mentry;

    mentry = MacroLookup();
    if( mentry != NULL ) {      /* if this is a macro */
        NextMacro = mentry;     /* save pointer to it */
        return( T_MACRO );
    }
    return( KwLookup() );
}

int doScanName()
{
    int         token;
    union {
    int         c;
    unsigned char uc;
    } u;
    char        *scanptr;

    u.c = CurrChar;
    token = TokenLen - 1;
//      we know that NextChar will be pointing to GetNextChar()
//      so it is safe to inline the function here.
//      NextChar could also be pointing to ReScanBuffer().
    for(;;) {
        scanptr = ScanCharPtr;
        for(;;) {
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
            Buffer[token] = u.uc;
            ++token;
            u.uc = *scanptr++;
            if( token >= BUF_SIZE )  token = BUF_SIZE;
        }
        ScanCharPtr = scanptr;
        if( (CharSet[u.c] & C_EX) == 0 ) break;
        u.c = GetCharCheck( u.c );
        if( (CharSet[u.c] & (C_AL | C_DI)) == 0 ) break;
    }
    CurrChar = u.c;
    if( token >= BUF_SIZE - 2 ) {
        if( NestLevel == SkipLevel ) {          /* 07-jun-92 */
            CErr1( ERR_TOKEN_TRUNCATED );
        }
        token = BUF_SIZE - 2;
    }
    Buffer[token] = '\0';
    TokenLen = token;
    CalcHash( Buffer, token );
    if( CompFlags.doing_macro_expansion ) return( T_ID );
    if( CompFlags.pre_processing == 2 ) return( T_ID );
    token = IdLookup();
    if( token == T_MACRO ) {
        if( CompFlags.cpp_output ) {
            PrtChar( ' ' );     /* put white space in front */
        }
        if( NextMacro->macro_defn == 0 ) {
            return( SpecialMacro( NextMacro ) );
        }
        NextMacro->macro_flags |= MACRO_REFERENCED;     /* 04-apr-94 */
        /* if macro requires parameters and next char is not a '('
        then this is not a macro */
        if( NextMacro->parm_count != 0 ) {
            SkipAhead();
            if( CurrChar != '(' ) {
                if( CompFlags.cpp_output ) {            /* 30-sep-90 */
                    Buffer[TokenLen++] = ' ';
                    Buffer[TokenLen] = '\0';
                    return( T_ID );
                }
                return( KwLookup() );
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
        if( token == T_NULL )  token = T_WHITE_SPACE;
    }
    return( token );
}

int ScanName()
{
    Buffer[0] = CurrChar;
    TokenLen = 1;
    return( doScanName() );
}

int ScanWide()          // scan something that starts with L
{
    int         c;
    int         token;

    Buffer[0] = 'L';
    c = NextChar();
    Buffer[1] = c;
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

int ScanDotSomething( int c )
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
    Buffer[1] = '\0';
    TokenLen = 1;
    return( T_DOT );
}

int doScanFloat()
{
    int         c;

    BadTokenInfo = 0;
    c = CurrChar;
    if( c == '.' ) {
        while( (c = SaveNextChar()) >= '0' && c <= '9' ) ;
        if( TokenLen == 2 ) {   /* .? */
            return ScanDotSomething( c );
        }
    }
    CurToken = T_CONSTANT;
    if( c == 'e'  ||  c == 'E' ) {
        c = SaveNextChar();
        if( c == '+'  ||  c == '-' ) {
            c = SaveNextChar();
        }
        if( c < '0'  ||  c > '9' ) {
            CurToken = T_BAD_TOKEN;
            BadTokenInfo = ERR_INVALID_FLOATING_POINT_CONSTANT;
        }
        while( c >= '0'  &&  c <= '9' ) {
            c = SaveNextChar();
        }
    }
    if( c == 'f' || c == 'F' ) {
        NextChar();
        ConstType = TYPE_FLOAT;
    } else if( c == 'l' || c == 'L' ) {
        NextChar();
        ConstType = TYPE_LONG_DOUBLE;
    } else {
        --TokenLen;
        ConstType = TYPE_DOUBLE;
    }
    Buffer[TokenLen] = '\0';
    return( CurToken );
}

int ScanDot()
{
    Buffer[0] = '.';
    TokenLen = 1;
    return( doScanFloat() );
}

static int ScanPPNumber()
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
    for(;;) {
        prevc = c;
        c = SaveNextChar();
        if( CharSet[c] & (C_AL | C_DI) )continue;
        if( c == '.' )continue;
        if( c == '+' || c == '-' ){
            if( prevc == 'e' || prevc == 'E' ){
                if( CompFlags.extensions_enabled ){
                    /* concession to existing practice...
                        #define A2 0x02
                        #define A3 0xaa0e+A2
                        // users want: 0xaa0e + 0x02
                        // not: 0xaa0e + A2 (but, this is what ISO C requires!)
                    */
                    prevc = c;  //advance to next
                    c = SaveNextChar();
                    if( !(CharSet[c] &  C_DI) )break; //allow e+<digit>
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

int ScanPPDigit()
{
    Buffer[0] = CurrChar;
    TokenLen = 1;
    return ScanPPNumber();
}

int ScanPPDot()
{
    int         c;

    Buffer[0] = '.';
    TokenLen = 1;
    c = SaveNextChar();
    if( c >= '0' && c <= '9' ) {
        return ScanPPNumber();
    } else {
        return ScanDotSomething( c );
    }
}

int ScanHex( int max, int rtn )
{
    int                 c;
    int                 count;
    unsigned char       chrclass;
    char                too_big;
    unsigned long       value;

    too_big = 0;
    count = max;
    value = 0;
    for( ;; ) {
        if( rtn == RTN_SAVE_NEXT_CHAR ) {
            c = SaveNextChar();
        } else {
            c = Buffer[ ++CLitLength ];
        }
        if( max == 0 ) break;
        chrclass = CharSet[ c ];
        if( ( chrclass & (C_HX|C_DI) ) == 0 ) break;
        if( chrclass & C_HX )  c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        if( value & 0xF0000000 )  too_big = 1;          /* 26-mar-91 */
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
static cnv_cc Cnv8( void ){
    char *curr;
    char c;
    int len;
    long value;
    uint64   value64;
    cnv_cc   ret;

    curr = Buffer;
    len = TokenLen;
    value = 0;
    while( --len > 0 ){
        c = *curr;
        if( value & 0xE0000000 ) goto is64; /* 64 bit */
        value = value * 8 + c - '0';
        ++curr;
    }
    Constant = value;
    return( CNV_32 );
is64:
    ret = CNV_64;
    U32ToU64( value, &value64 );
    do{
        c = *curr;
        if( U64Cnv8( &value64, c-'0' ) ){
            ret = CNV_OVR;
        }
        ++curr;
    }while( --len > 0 );
    Const64 = value64;
    return( ret );
}

static cnv_cc Cnv16( void ){
    char *curr;
    char c;
    int len;
    unsigned long value;
    uint64   value64;
    cnv_cc   ret;

    curr = Buffer+2;    //skip 0x thing
    len = TokenLen-2;
    value = 0;
    while( --len > 0 ){
        c = *curr;
        if( value & 0xF0000000 )goto is64; /* 64 bit */
        if( CharSet[ c ] & C_HX ){
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
    do{
        c = *curr;
        if( CharSet[ c ] & C_HX ){
            c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
        }
        if( U64Cnv16( &value64, c-'0' ) ){
            ret = CNV_OVR;
        }
        ++curr;
    }while( --len > 0 );
    Const64 = value64;
    return( ret );
}

static cnv_cc Cnv10( void ){
    char *curr;
    char c;
    int len;
    unsigned long value;
    uint64   value64;
    cnv_cc   ret;

    curr = Buffer;    //skip 0x thing
    len = TokenLen;
    value = 0;
    while( --len > 0 ){
        c = *curr;
        if( value >= 429496729 ) {          /* 15-feb-93 */
            if( value == 429496729 ) {
                if( c > '5' )  goto is64;
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
    do{
        c = *curr;
        if( U64Cnv10( &value64, c-'0') ){
            ret = CNV_OVR;
        }
        ++curr;
    }while( --len > 0 );
    Const64 = value64;
    return( ret );
}

int ScanNum()
{
    int                 c;
    int                 bad_token_type;
    cnv_cc              ov;
    struct {
        enum{ CON_DEC, CON_HEX, CON_OCT, CON_ERR }form;
        enum { SUFF_NONE,SUFF_U, SUFF_L,SUFF_UL,  SUFF_I, SUFF_UI } suffix;
    }con;

    BadTokenInfo = 0;
    Constant = 0;
    TokenLen = 1;
    c = CurrChar;
    Buffer[0] = c;
    if( c == '0' ) {
        c = SaveNextChar();
        if( c == 'x'  ||  c == 'X' ) {
            unsigned char       chrclass;

            bad_token_type = ERR_INVALID_HEX_CONSTANT;
            con.form = CON_HEX;
            for( ;; ) {
                c = SaveNextChar();
                chrclass = CharSet[ c ];
                if( ( chrclass & (C_HX|C_DI) ) == 0 ) break;
            }
            if( TokenLen == 3 ){ /* just collected a 0x */
                BadTokenInfo = ERR_INVALID_HEX_CONSTANT;
                if( NestLevel == SkipLevel ) {
                    CErr1( ERR_INVALID_HEX_CONSTANT );
                    con.form = CON_ERR;
                }
            }
        } else {    /* scan octal number */
            unsigned char digit_mask;

            bad_token_type = ERR_INVALID_OCTAL_CONSTANT;
            con.form = CON_OCT;
            digit_mask = 0;
            // if collecting tokens for macro preprocessor, allow 8 and 9
            // since the argument may be used in with # or ##. 28-oct-92
            while( c >= '0'  &&  c <= '9' ) {
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
        for(;;) {
            c = SaveNextChar();
            if( c < '0'  ||  c > '9' ) break;
        }
        if( c == '.' || c == 'e' || c == 'E' ) {
            return( doScanFloat() );
        }
    }
    switch( con.form ){
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
    if( c == 'l'  ||  c == 'L' ) {   // collect suffix
        c = SaveNextChar();
        if( c == 'u' || c == 'U' ) {
            c = SaveNextChar();
            con.suffix = SUFF_UL;
        }else{
            con.suffix = SUFF_L;
        }
    } else if( c == 'u' || c == 'U' ) {
        c = SaveNextChar();
        if( c == 'l' || c == 'L' ) {
            c = SaveNextChar();
            con.suffix = SUFF_UL;
        } else if( c == 'i' || c == 'I' ) {
            c = SaveNextChar();
            con.suffix = SUFF_UI;
        }else{
            con.suffix = SUFF_U;
        }
    } else if( c == 'i' || c == 'I' ) {
        c = SaveNextChar();
        con.suffix = SUFF_I;
    }
    if( con.suffix == SUFF_UI || con.suffix == SUFF_I ){
        unsigned_32 value;
        value  = 0;
        while( c >= '0'  &&  c <= '9' ) {
            value = value * 10 + c - '0';
            c = SaveNextChar();
        }
        if( value == 64 ){
            if( con.suffix == SUFF_I ){
                ConstType = TYPE_LONG64;
            }else{
                ConstType = TYPE_ULONG64;
            }
            if(  ov == CNV_32 ){
                U32ToU64( Constant, &Const64 );
            }
        }else if( value == 32 ){
            if( con.suffix == SUFF_I ){
                ConstType = TYPE_LONG;
            }else{
                ConstType = TYPE_ULONG;
            }
        }else if( value == 16 ){
            if( con.suffix == SUFF_I ){
                ConstType = TYPE_SHORT;
            }else{
                ConstType = TYPE_USHORT;
            }
        }else if( value == 8 ){
            if( con.suffix == SUFF_I ){
                ConstType = TYPE_CHAR;
            }else{
                ConstType = TYPE_UCHAR;
            }
        }else{
            if( NestLevel == SkipLevel ) {
                CErr1( ERR_INVALID_CONSTANT );
            }
        }
        if( ov == CNV_64 && value < 64 ){
            BadTokenInfo = ERR_CONSTANT_TOO_BIG;
            if( NestLevel == SkipLevel ) {      /* 10-sep-92 */
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
            Constant =  Const64.u._32[I64LO32];
        }
    }else if( ov == CNV_32 ){
        switch( con.suffix ){
        case SUFF_NONE:
            if( Constant <= TARGET_INT_MAX ) {
                ConstType = TYPE_INT;
         #if TARGET_INT < TARGET_LONG
            }else if( Constant <= TARGET_UINT_MAX && con.form != CON_DEC ){
                ConstType = TYPE_UINT;
            }else if( Constant <= 0x7ffffffful ) {
                ConstType = TYPE_LONG;
            }else{
                ConstType = TYPE_ULONG;
            }
         #else
            }else if( con.form != CON_DEC ){
                ConstType = TYPE_UINT;
            }else{
                ConstType = TYPE_ULONG;
            }
         #endif
            break;
        case SUFF_L:
            if( Constant <= 0x7FFFFFFFul ) {     /* 13-sep-89 */
                ConstType = TYPE_LONG;
            }else{
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
        }
    }else{
        switch( con.suffix ){
        case SUFF_NONE:
            ConstType = TYPE_LONG64;
            if( Const64.u._32[I64HI32] & 0x80000000 ){
                ConstType = TYPE_ULONG64;
            }
            break;
        case SUFF_L:
            if( Const64.u._32[I64HI32] & 0x80000000 ){
                ConstType = TYPE_ULONG64;
            }else{
                ConstType = TYPE_LONG64;
            }
            break;
        case SUFF_U:
        case SUFF_UL:
            ConstType = TYPE_ULONG64;
            break;
        }
    }
    if( CompFlags.pre_processing && (CharSet[c] & (C_AL | C_DI)) ) {
        for(;;) {
            c = SaveNextChar();
            if( (CharSet[c] & (C_AL | C_DI)) == 0 ) break;
        }
        --TokenLen;
        Buffer[TokenLen] = '\0';
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
        Buffer[TokenLen] = '\0';
        return( T_CONSTANT );
    }
}

int ScanQuestionMark()
{
    NextChar();
    Buffer[0] = '?';
    Buffer[1] = '\0';
    return( T_QUESTION );
}

int ScanSlash()
{
    int         c;
    int         tok;

    c = NextChar();         // can't inline this copy of NextChar
    if( c == '=' ) {         /* if second char is an = */
        NextChar();
        Buffer[0] = '/';
        Buffer[1] = '=';
        Buffer[2] = '\0';
        tok = T_DIVIDE_EQUAL;
    } else if( c == '/' &&   /* if C++ // style comment */
                ! CompFlags.strict_ANSI ) {     /* 13-nov-94 */
        CppComment( '/' );
        CompFlags.scanning_cpp_comment = 1;
        for(;;) {
            c = CurrChar;
            NextChar();
            if( CurrChar == EOF_CHAR ) break;   /* 06-mar-91 AFS */
            if( CurrChar == '\0' ) break;   /* 06-mar-91 AFS */
            /* swallow up the next line if this one ends with \ */
            /* some editors don't put linefeeds on end of lines */
            if( CurrChar == '\n'  ||  c == '\r' )break;
            if( CompFlags.cpp_output && CompFlags.keep_comments ) {
                if( CurrChar != '\r' ) PrtChar( CurrChar );
            }
        }
        CppComment( 0 );
        CompFlags.scanning_cpp_comment = 0;
        Buffer[0] = ' ';
        Buffer[1] = '\0';
        tok = T_WHITE_SPACE;
    } else if( c == '*' ) {
        ScanComment();
        Buffer[0] = ' ';
        Buffer[1] = '\0';
        tok = T_WHITE_SPACE;
    } else {
        Buffer[0] = '/';
        Buffer[1] = '\0';
        tok = T_DIVIDE;
    }
    return( tok );
}

int ScanDelim1()
{
    int         token;
    int         c;

    Buffer[0] = CurrChar;
    Buffer[1] = '\0';
    token = TokValue[ CurrChar - ' ' ];
    c = *ScanCharPtr++;
    if( CharSet[c] & C_EX ) {
        c = GetCharCheck( c );
    }
    CurrChar = c;
    return( token );
}

int ScanMinus()
{
    int         chr2;
    int         tok;

    Buffer[0] = '-';
    chr2 = NextChar();          // can't inline this copy of NextChar
    Buffer[1] = chr2;
    if( chr2 == '>' ) {
        tok = T_ARROW;
        Buffer[2] = '\0';
        NextChar();
    } else if( chr2 == '=' ) {
        tok = T_MINUS_EQUAL;
        Buffer[2] = '\0';
        NextChar();
    } else if( chr2 == '-' ) {
        tok = T_MINUS_MINUS;
        Buffer[2] = '\0';
        NextChar();
    } else {
        Buffer[1] = '\0';
        tok = T_MINUS;
    }
    return( tok );
}

int ScanEqual()
{
    Buffer[0] = '=';
    if( NextChar() == '=' ) {
        NextChar();
        Buffer[1] = '=';
        Buffer[2] = '\0';
        return( T_EQ );
    } else {
        Buffer[1] = '\0';
        return( T_EQUAL );
    }
}

int ScanStar()
{
    Buffer[0] = '*';
    if( NextChar() == '=' ) {
        NextChar();
        Buffer[1] = '=';
        Buffer[2] = '\0';
        return( T_TIMES_EQUAL );
    } else {
        Buffer[1] = '\0';
        return( T_TIMES );
    }
}

int ScanColon()
{
    int         chr2;

    Buffer[0] = ':';
    chr2 = NextChar();
    if( chr2 == '>' ) {
        NextChar();
        Buffer[1] = '>';
        Buffer[2] = '\0';
        return( T_SEG_OP );
#if _CPU == 370
    } else if( chr2 == ')' ) {
        NextChar();
        Buffer[1] = ')';
        Buffer[2] = '\0';
        return( T_RIGHT_BRACKET );
#endif
    } else {
        Buffer[1] = '\0';
        return( T_COLON );
    }
}

int ScanDelim2()
{
    int         c;
    int         chr2;
    int         tok;
    unsigned char chrclass;

    c = CurrChar;
    Buffer[0] = c;
    Buffer[1] = '\0';
    chrclass = TokValue[ c - ' ' ];
    tok = chrclass & C_MASK;
    chr2 = NextChar();          // can't inline this copy of NextChar
    if( chr2 == '=' ) {         /* if second char is an = */
        if( chrclass & EQ ) {   /* and = is valid second char */
            ++tok;
            NextChar();
            Buffer[1] = '=';
            Buffer[2] = '\0';
        }
    } else if( chr2 == c ) {    /* if second char is same as first */
        if( chrclass & DUP ) {  /* and duplicate is valid */
            tok += 2;
            Buffer[1] = c;
            Buffer[2] = '\0';
            if( NextChar() == '=' ) {
                if( tok == T_LSHIFT  ||  tok == T_RSHIFT ) {
                    ++tok;
                    NextChar();
                    Buffer[2] = '=';
                    Buffer[3] = '\0';
                }
            }
        }
#if _CPU == 370
    } else if( c == '('  &&  chr2 == ':' ) {
        tok = T_LEFT_BRACKET;
        NextChar();
        Buffer[1] = ':';
        Buffer[2] = '\0';
#endif
    }
    return( tok );
}

void ScanComment()
{
    int         c;
    int         prev_char;
    unsigned    start_line;
    char        *scanptr;

    CompFlags.scanning_comment = 1;
    if( CompFlags.cpp_output ) {        // 30-dec-93
        CppComment( '*' );
        c = NextChar();
        for( ;; ) {
            if( c == '*' ){
                c = NextChar();
                if( c == '/' )break;
                if( CompFlags.keep_comments ) {
                    PrtChar( '*' );
                }
                continue; //could be **/
            }
            if( c == EOF_CHAR ) {
                CppComment( 0 );
                return;
            }
            if( c == '\n' ) {
                PrtChar( c );
            } else if( c != '\r' && CompFlags.keep_comments ) {
                PrtChar( c );
            }
            c = NextChar();
        }
        CppComment( 0 );
    } else {
        start_line = TokenLine;
        // make '/' a special character so that we only have to do one test
        // for each character inside the main loop
        CharSet['/'] |= C_EX;           // make '/' special character
        c = '\0';
        for(;;) {
            scanptr = ScanCharPtr;
            for(;;) {
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
                prev_char = c;
                c = *scanptr++;
                if( CharSet[c] & C_EX ) break;
            }
            ScanCharPtr = scanptr;
            if( c != '/' ) {
                c = GetCharCheck( c );
                if( c == EOF_CHAR )  return;
            }
            if( c == '/' ) {
                if( prev_char == '*' )  break;
                // get next character and see if it is '*' for nested comment
                c = NextChar();
                if( c == '*' ) {
                    c = NextChar();
                    if( c == '/' ) break;       /* 19-oct-94 */
                    TokenLine = SrcFileLineNum;
                    CWarn2( WARN_NESTED_COMMENT,
                            ERR_NESTED_COMMENT, start_line );
                }
            }
            // NextChar might not be pointing to GetNextChar at this point
            while( NextChar != GetNextChar ) {
                c = NextChar();
                if( c == EOF_CHAR )  return;
            }
        }
        CharSet['/'] &= ~C_EX;          // undo '/' special character
    }
    CompFlags.scanning_comment = 0;
    NextChar();
}

int CharConst( int char_type )
{
    int         c;
    int         i;
    int         n;
    int         token;
    long        value;
    char        error;

    c = SaveNextChar();
    if( c == '\'' ) {                           /* 05-jan-95 */
        NextChar();
        Buffer[TokenLen] = '\0';
        BadTokenInfo = ERR_INV_CHAR_CONSTANT;
        return( T_BAD_TOKEN );
    }
    BadTokenInfo = 0;
    token = T_CONSTANT;
    i = 0;
    value = 0;
    error = 0;
    for( ;; ) {
        if( c == '\r'  ||  c == '\n' ) {
            token = T_BAD_TOKEN;
            break;
        }
        if( c == '\\' ) {
            c = SaveNextChar();
            if( c >= '0'  &&  c <= '7' ) {              /* 09-jan-94 */
                n = c - '0';
                c = SaveNextChar();
                if( c >= '0'  &&  c <= '7' ) {
                    n = n * 8 + c - '0';
                    c = SaveNextChar();
                    if( c >= '0'  &&  c <= '7' ) {
                        n = n * 8 + c - '0';
                        SaveNextChar();
                        if( n > 0377  &&  char_type != TYPE_WCHAR ) {
                            BadTokenInfo = ERR_CONSTANT_TOO_BIG;
                            if( NestLevel == SkipLevel ) {
                                CWarn1( WARN_CONSTANT_TOO_BIG,
                                        ERR_CONSTANT_TOO_BIG );
                            }
                            n &= 0377;          // mask off high bits
                        }
                    }
                }
                c = n;
            } else {
                c = ESCChar( c, RTN_SAVE_NEXT_CHAR, &error );
            }
            if( char_type == TYPE_WCHAR ) {     /* 02-aug-91 */
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
            }
        } else {
            if( CharSet[c] & C_DB ) {   /* if double-byte char */
                c = (c << 8) + (SaveNextChar() & 0x00FF);
                if( char_type == TYPE_WCHAR ) { /* 15-jun-93 */
                    if( CompFlags.jis_to_unicode ) {
                        c = JIS2Unicode( c );
                    }
                }
                ++i;
                value = (value << 8) + ((c & 0xFF00) >> 8);
                c &= 0x00FF;
            } else if( char_type == TYPE_WCHAR ) {/* 02-aug-91 */
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
            if( ! CompFlags.cpp_output ) {      /* 08-feb-93 */
                token = T_BAD_TOKEN;
                break;
            }
        }
        c = CurrChar;
        if( c == '\'' ) break;
        if( i >= 4 ) {
            if( ! CompFlags.cpp_output ) {      /* 08-feb-93 */
                token = T_BAD_TOKEN;
                break;
            }
        }
    }
    if( token == T_BAD_TOKEN ) {
        BadTokenInfo = ERR_INV_CHAR_CONSTANT;
    } else {
        NextChar();
        if( error != 0 ) {                      /* 16-nov-94 */
            BadTokenInfo = ERR_INVALID_HEX_CONSTANT;
            token = T_BAD_TOKEN;
        }
    }
    Buffer[TokenLen] = '\0';
    ConstType = char_type;
    if( CompFlags.signed_char ) {
        if( (value & 0xFFFFFF80) == 0x80 ) {    /* if sign bit is on */
            value |= 0xFFFFFF00;                /* - sign extend it */
        }
    }
    Constant = value;
    return( token );
}

int ScanCharConst()
{
    Buffer[0] = '\'';
    TokenLen = 1;
    return( CharConst( TYPE_CHAR ) );
}

int ScanString()
{
    int         c;
    int         ok;
    char        error;
    ok = 0;
    error = 0;
    CompFlags.wide_char_string = 0;
    CompFlags.trigraph_alert = 0;
    c = NextChar();
    Buffer[0] = c;
    TokenLen = 1;
    for( ;; ) {
        if( c == '\n' ){
            TokenLine = SrcFileLineNum-1; /* place error at site */
            if( NestLevel == SkipLevel ) {  /* 10-sep-92 */
                CErr1( ERR_MISSING_LINE_CONTINUE );
            }else{
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_MISSING_LINE_CONTINUE  );
            }
        }
        if( c == EOF_CHAR ) break;
        if( c == '"' ) {
            NextChar();
            ok = 1;
            break;
        }

        if( c == '\\' ) {               /* 23-mar-90 */
            if( TokenLen > BUF_SIZE - 32 ) {
                /*
                    allow infinite length string tokens by faking up
                    a string concatenation
                */
                // break long strings apart (parser will join them)
                UnGetChar( '\\' );
                CurrChar = CONTINUE_CHAR_STRING;
                ++TokenLen;
                ok = 1;
                break;
            }
            c = NextChar();
            Buffer[TokenLen++] = c;
            if( (CharSet[c] & C_WS) == 0 ) {    /* 04-nov-88 */
                ESCChar( c, RTN_SAVE_NEXT_CHAR, &error );
            }
            c = CurrChar;
        } else {
            /* if first character of a double-byte character, then
               save it and get the next one.    10-nov-89  */
            if( CharSet[c] & C_DB ) SaveNextChar();
            if( TokenLen > BUF_SIZE - 32 ) {
                /*
                    allow infinite length string tokens by faking up
                    a string concatenation
                */
                CurrChar = CONTINUE_CHAR_STRING;
                ++TokenLen;
                ok = 1;
                break;
            }
            c = NextChar();
            Buffer[TokenLen++] = c;
        }
    }
    Buffer[TokenLen-1] = '\0';
    CLitLength = TokenLen;
    if( CompFlags.trigraph_alert ){
        CWarn1( WARN_LEVEL_1, ERR_EXPANDED_TRIGRAPH );
    }
    if( ok ) return( T_STRING );
    --TokenLen;         /* '\n' or EOF_CHAR don't print nicely */
    Buffer[TokenLen-1] = '\0';
    BadTokenInfo = ERR_MISSING_QUOTE;
    return( T_BAD_TOKEN );
}

int ScanStringContinue()
{
    int         token;
    int         was_wide;

    was_wide = CompFlags.wide_char_string;
    token = ScanString();
    CompFlags.wide_char_string = was_wide;
    return( token );
}

int ESCChar( int c, int rtn, char *error )
{
    int n, i;

    if( c >= '0'  &&  c <= '7' ) {          /* get octal escape sequence */
        n = 0;
        i = 3;
        while( c >= '0'  &&  c <= '7' ) {
            n = n * 8 + c - '0';
            if( rtn == RTN_SAVE_NEXT_CHAR ) {
                c = SaveNextChar();
            } else {
                c = Buffer[ ++CLitLength ];
            }
            --i;
            if( i == 0 ) break;
        }
    } else if( c == 'x' ) {         /* get hex escape sequence */
        if( ScanHex( 127, rtn ) ) {
            n = Constant;
        } else {                        /*  '\xz' where z is not a hex char */
            *error = 1;
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
#if _OS == _QNX
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
        if( rtn == RTN_SAVE_NEXT_CHAR ) {
            SaveNextChar();
        } else {
            ++CLitLength;
        }
    }
    return( n );
}

int ScanWhiteSpace()
{
    char        *scanptr;
    union {
    int         c;
    unsigned char uc;
    } u;

    if( NextChar == getCharAfterBackSlash ) {
        for(;;) {
            u.c = NextChar();
            if( (CharSet[u.c] & C_WS) == 0 ) break;
        }
    } else {
        u.c = 0;
        for(;;) {
            scanptr = ScanCharPtr;
            for(;;) {
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
                u.uc = *scanptr++;
                if( (CharSet[u.c] & C_WS) == 0 ) break;
            }
            ScanCharPtr = scanptr;
            if( (CharSet[u.c] & C_EX) == 0 ) break;
            u.c = GetCharCheck( u.c );
            if( (CharSet[u.c] & C_WS) == 0 ) break;
        }
        CurrChar = u.c;
    }
    return( T_WHITE_SPACE );
}

static void SkipWhiteSpace( int c )
{
    if( !CompFlags.cpp_output ) {
        ScanWhiteSpace();
    } else {
        for(;;) {
            if( (CharSet[c] & C_WS) == 0 ) break;
            if( c != '\r' ) {
                if( !CompFlags.pre_processing )  PrtChar( c );
            }
            c = NextChar();
        }
    }
}


void SkipAhead()
{
    for(;;) {
        for(;;) {
            if( CharSet[CurrChar] & C_WS ) {
                SkipWhiteSpace( CurrChar );
            }
            if( CurrChar != '\n' ) break;
            if( CompFlags.cpp_output ) {
                if( !CompFlags.pre_processing )  PrtChar( '\n' );
            }
            NextChar();
        }
        if( CurrChar != '/' )  break;
        NextChar();
        if( CurrChar == '*' ) {
            ScanComment();
        } else {
            UnGetChar( CurrChar );
            CurrChar = '/';
            break;
        }
    }
}

int ScanNewline()
{
    if( CompFlags.pre_processing ) return( T_NULL );
    return( ChkControl() );
}

int ScanCarriageReturn()
{
    int         c;

    c = NextChar();
    if( c == '\n' ) {
        if( CompFlags.pre_processing ) return( T_NULL );
        return( ChkControl() );
    }
    return( ScanWhiteSpace() );
}

#if defined(__DOS__) || defined(__OS2__) || defined(__NT__) || defined(__OSI__)
    #define     SYS_EOF_CHAR 0x1A
#elif defined(__QNX__)
    #undef      SYS_EOF_CHAR
#else
    #error System end of file character not configured.
#endif

int ScanInvalid()
{
    Buffer[0] = CurrChar;
    Buffer[1] = '\0';
#ifdef SYS_EOF_CHAR
    if( Buffer[0] == SYS_EOF_CHAR ) {
        CloseSrcFile( SrcFile );                /* 13-sep-94 */
        return( T_WHITE_SPACE );
    }
#endif
    NextChar();
    return( T_BAD_CHAR );
}

int ScanMacroToken()
{
    GetMacroToken();
    if( CurToken == T_NULL ) {
        if( CompFlags.cpp_output ) {
            PrtChar( ' ' );
        }
        CurrChar = SavedCurrChar;
        CurToken = ScanToken();
    }
    return( CurToken );
}

int ScanEof()
{
    return( T_EOF );
}

int (*ScanFunc[])() = {
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
    ScanStringContinue,
    ScanCharConst,
    ScanCarriageReturn,
    ScanNewline,
    ScanWhiteSpace,
    ScanInvalid,
    ScanMacroToken,
    ScanEof
};

int ScanToken()
{
    TokenLine = SrcFileLineNum;         /* remember line token starts on */
//    TokenLen = 1;
//    Buffer[0] = CurrChar;
    return( (*ScanFunc[ClassTable[CurrChar]])() );
}

int NextToken()
{
    do {
        CurToken = T_NULL;
        if( MacroPtr != NULL ) {
            GetMacroToken();
        }
        if( CurToken == T_NULL ) {
//          CurToken = ScanToken();
            TokenLine = SrcFileLineNum;
//          TokenLen = 1;
//          Buffer[0] = CurrChar;
            CurToken = (*ScanFunc[ClassTable[CurrChar]])();
        }
    } while( CurToken == T_WHITE_SPACE );
    #ifdef FDEBUG
        DumpToken();
    #endif
    return( CurToken );
}

int PPNextToken()                       // called from macro pre-processor
{
    do {
        if( MacroPtr != NULL ) {
            GetMacroToken();
            if( CurToken == T_NULL ) {
                if( CompFlags.cpp_output ) {
                    PrtChar( ' ' );
                }
                CurToken = ScanToken();
            }
        } else {
            CurToken = ScanToken();
        }
    } while( CurToken == T_WHITE_SPACE );
    return( CurToken );
}

int ReScanToken()
{
    int         saved_currchar;
    char        *saved_ScanCharPtr;
    int         (*saved_nextchar)();

    saved_currchar = CurrChar;
    saved_nextchar = NextChar;
    saved_ScanCharPtr = ScanCharPtr;
    ScanCharPtr = ReScanPtr;
    CompFlags.rescan_buffer_done = 0;
    NextChar = ReScanBuffer;
    CurrChar = ReScanBuffer();
    CompFlags.doing_macro_expansion = 1;        // return macros as ID's
    CurToken = ScanToken();
    CompFlags.doing_macro_expansion = 0;
    if( CurToken == T_STRING  &&  CompFlags.wide_char_string ) {
        CurToken = T_LSTRING;                   /* 12-nov-92 */
    }
    --ScanCharPtr;
    ReScanPtr = ScanCharPtr;
    ScanCharPtr = saved_ScanCharPtr;
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
        --ScanCharPtr;
        CompFlags.rescan_buffer_done = 0;
    } else {
        GetNextCharUndo( c );
    }
}

void ScanInit()
{
    int         i;
    int         c;

    memset( &ClassTable[0],   SCAN_INVALID, 256 );
    memset( &ClassTable['A'], SCAN_NAME,    26 );
    memset( &ClassTable['a'], SCAN_NAME,    26 );
    memset( &ClassTable['0'], SCAN_NUM,     10 );
    ClassTable[ EOF_CHAR ] = SCAN_EOF;
    ClassTable[ CONTINUE_CHAR_STRING ] = SCAN_STRING2;
    ClassTable[ MACRO_CHAR ] = SCAN_MACRO;
    for( i = 0; ; i += 2 ) {
        c = InitClassTable[i];
        if( c == '\0' ) break;
        ClassTable[c] =  InitClassTable[i+1];
    }
    CurrChar = '\n';
    CompFlags.pre_processing = 0;
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
int InitPPScan()
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
