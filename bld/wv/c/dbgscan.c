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
* Description:  Debugger lexical scanner.
*
****************************************************************************/


#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgtoggl.h"
#include "dbglit.h"
#include "ldsupp.h"
#include "mad.h"
#include "madcli.h"
#include "dui.h"
#include "i64.h"


typedef struct rad_str {
    struct   rad_str *next;
    unsigned char    radval;
    char             radstr[1];         /* first byte is length */
} rad_str;

extern unsigned         Lookup( char *, char *, unsigned );
extern unsigned int     ReqExpr( void );
extern void             ConfigLine( char * );
extern char             *Format( char *, char *, ... );
extern char             *CnvULongDec( unsigned long, char * );
extern unsigned         GetMADTypeNameForCmd( mad_type_handle th, unsigned max, char *p );
extern void             DbgUpdate( update_list );
extern char             *ReScan( char *point );

extern tokens         CurrToken;
extern unsigned char  DefRadix;
extern unsigned char  CurrRadix;
extern char           *TxtBuff;

extern void Scan( void );

static char CmdLnDelimTab[] = { "<>*/(),{}!?;[]~#\0" };

typedef union {
        unsigned_64     int_val;
        xreal           real_val;
} token_value;


static  rad_str     *RadStrs;
static  char        *ScanPtr;
static  token_value  TokenVal;
static  char        *TokenStart;
static  token_table *ExprTokens;
        bool         scan_string = FALSE;
        char        *StringStart = NULL;
        unsigned     StringLength = 0;
        unsigned     ScanCCharNum = TRUE;


static void SetRadixSpec( char *str, unsigned len, unsigned radix, bool clear )
{

    rad_str   *pref;
    rad_str  **owner;

    owner = &RadStrs;
    pref = RadStrs;
    while( pref != NULL ) {
        if( pref->radstr[0] == len
         && memicmp( &pref->radstr[1], str, pref->radstr[0] ) == 0 ) break;
        if( pref->radstr[0] < len ) break;
        owner = &pref->next;
        pref = pref->next;
    }
    if( pref == NULL || pref->radstr[0] != len ) {
        if( clear ) return;
        pref = DbgMustAlloc( sizeof( rad_str ) + len );
        memcpy( &pref->radstr[1], str, len );
        pref->radstr[0] = len;
        pref->next = *owner;
        *owner = pref;
    } else if( clear ) {
        *owner = pref->next;
        _Free( pref );
        return;
    }
    pref->radval = radix;
}


/*
 * InitScan -- initialize scanner
 */

void InitScan( void )
{
    ScanPtr = LIT( Empty );
    TokenStart = ScanPtr;
    CurrToken = T_LINE_SEPARATOR;
    RadStrs = NULL;
    SetRadixSpec( "0x", 2, 16, FALSE );
    SetRadixSpec( "0n", 2, 10, FALSE );
}


void FiniScan( void )
{
    rad_str     *old;

    while( RadStrs != NULL ) {
        old = RadStrs;
        RadStrs = RadStrs->next;
        _Free( old );
    }
}


/*
 * ScanPos -- return the current scan position
 */

char *ScanPos( void )
{
    return( TokenStart );
}



/*
 * ScanLen -- return the length of current token
 */

unsigned ScanLen( void )
{
    return( ScanPtr - TokenStart );
}



/*
 * ScanCmd -- scan a command start of current token, looking up in given table
 */

unsigned ScanCmd( char *cmd_table )
{
    unsigned    ind;
    char        *saveptr;

    saveptr = ScanPtr;
    ScanPtr = TokenStart;
    while( isalpha( *ScanPtr ) || *ScanPtr == '_' ) {
        ++ScanPtr;
    }
    ind = Lookup( cmd_table, TokenStart, ScanPtr - TokenStart );
    if( ind != 0 ) {
        Scan();
    } else {
        ScanPtr = saveptr;
    }
    return( ind );
}

struct type_name {
    char                *start;
    unsigned            len;
    mad_type_handle     th;
};

static walk_result      FindTypeName( mad_type_handle th, void *d )
{
    struct type_name    *nd = d;
    char                *p;
    char                *q;
    unsigned            len;

    GetMADTypeNameForCmd( th, TXT_LEN, TxtBuff );
    p = nd->start;
    q = TxtBuff;
    for( ;; ) {
        if( tolower( *p ) != tolower( *q ) ) break;
        if( *q == '\0' ) break;
        ++p;
        ++q;
    }
    if( isalnum( *p ) ) return( WR_CONTINUE );
    len = q - TxtBuff;
    if( *q == '\0' ) {
        /* an exact match */
        nd->len = len;
        nd->th = th;
        return( WR_STOP );
    }
    if( len > nd->len ) {
        nd->len = len;
        nd->th = th;
    }
    return( WR_CONTINUE );
}

static mad_type_handle DoScanType( mad_type_kind tk, char *prefix )
{
    struct type_name    data;
    unsigned            len;

    len = strlen( prefix );
    if( memicmp( TokenStart, prefix, len ) != 0 ) {
        return( MAD_NIL_TYPE_HANDLE );
    }
    data.start = TokenStart + len;
    data.len = 0;
    data.th = MAD_NIL_TYPE_HANDLE;
    MADTypeWalk( tk, FindTypeName, &data );
    if( data.th == MAD_NIL_TYPE_HANDLE ) return( MAD_NIL_TYPE_HANDLE );
    ReScan( data.start + data.len );
    return( data.th );
}

mad_type_handle ScanType( mad_type_kind tk, mad_type_kind *tkr )
{
    mad_type_handle     th;

    if( tk & MAS_MEMORY ) {
        th = DoScanType( tk & ~MAS_IO, LIT( Empty ) );
        if( th != MAD_NIL_TYPE_HANDLE ) {
            if( tkr != NULL ) *tkr = MAS_MEMORY;
            return( th );
        }
    }
    if( th & MAS_IO ) {
        th = DoScanType( tk & ~MAS_MEMORY, LIT( IO ) );
        if( th != MAD_NIL_TYPE_HANDLE ) {
            if( tkr != NULL ) *tkr = MAS_IO;
            return( th );
        }
    }
    return( th );
}

mad_string ScanCall( void )
{
    char                *p;
    char                *q;
    const mad_string    *type;
    char                buff[NAM_LEN];

    type = MADCallTypeList();
    for( ;; ) {
        if( *type == MAD_MSTR_NIL ) return( MAD_MSTR_NIL );
        MADCliString( *type, sizeof( buff ), buff );
        q = buff;
        p = TokenStart;
        for( ;; ) {
            if( !isalnum( *p ) ) {
                if( p == TokenStart ) return( MAD_MSTR_NIL );
                ReScan( p );
                return( *type );
            }
            if( tolower( *p ) != tolower( *q ) ) break;
            ++p;
            ++q;
        }
        ++type;
    }
}

/*
 * ScanEOC -- check if at end of command
 */

bool ScanEOC( void )
{
    return( CurrToken == T_CMD_SEPARATOR || CurrToken == T_LINE_SEPARATOR );
}


static bool FindToken( char *table, unsigned token,
                       char **start, unsigned *len )
{
    unsigned chk;

    while( *table != NULLCHAR ) {
        *start = table;
        for( ; *table != NULLCHAR; ++table )
            ;
        chk = table[1];
        chk |= table[2] << 8;
        if( chk == token ) {
            *len = table - *start;
            return( TRUE );
        }
        table += 3;
    }
    return( FALSE );
}


bool TokenName( unsigned token, char **start, unsigned *len )
{
    switch( token ) {
    case T_LINE_SEPARATOR:
        *start = LIT( End_Of_Line );
        *len = strlen( LIT( End_Of_Line ) ) + 1;
        return( TRUE );
    case T_INT_NUM:
    case T_REAL_NUM:
        *start = LIT( Num_Name );
        *len = strlen( LIT( Num_Name ) ) + 1;
        return( TRUE );
    case T_NAME:
        *start = LIT( Sym_Name_Name );
        *len = strlen( LIT( Sym_Name_Name ) ) + 1;
        return( TRUE );
    }
    if( token < LAST_CMDLN_DELIM ) {
        *start = &CmdLnDelimTab[ token - FIRST_CMDLN_DELIM ];
        *len = sizeof( char );
        return( TRUE );
    }
    if( ExprTokens != NULL ) {
        if( FindToken( ExprTokens->delims, token, start, len ) ) return(TRUE);
        if( FindToken( ExprTokens->keywords, token, start, len ) ) return(TRUE);
    }
    return( FALSE );
}


void Recog( unsigned token )
{
    char        *start;
    unsigned    len;

    if( token != CurrToken ) {
        TokenName( token, &start, &len );
        Error( ERR_LOC, LIT( ERR_WANT_TOKEN ), start, len );
    }
    Scan();
}


/*
 * ScanQuote -- scan a debugger quoted string
 */

bool ScanQuote( char **start, unsigned *len )
{
    int   cnt;

    if( CurrToken != T_LEFT_BRACE ) {
        *start = NULL;
        *len   = 0;
        return( FALSE );
    }
    *start = ScanPtr;
    cnt = 1;
    while( cnt > 0 ) {
        Scan();
        if( CurrToken == T_LEFT_BRACE ) {
            cnt += 1;
        } else if( CurrToken == T_RIGHT_BRACE ) {
            cnt -= 1;
        } else if( CurrToken == T_LINE_SEPARATOR ) {
            Recog( T_RIGHT_BRACE ); /* cause error */
        }
    }
    *len = TokenStart - *start;
    Scan();
    return( TRUE );
}


/*
 * ScanItem - scan to a blank or EOC
 */

bool ScanItem( bool blank_delim, char **start, unsigned *len )
{
    if( ScanEOC() ) {
        *start = NULL;
        *len   = 0;
        return( FALSE );
    }
    if( ScanQuote( start, len ) ) return( TRUE );
    *start = TokenStart;
    for( ;; ) {
        if( blank_delim && isspace( *ScanPtr ) ) break;
        if( *ScanPtr == ';' ) break;
        if( *ScanPtr == NULLCHAR ) break;
        ++ScanPtr;
    }
    *len = ScanPtr - TokenStart;
    Scan();
    return( TRUE );
}


/*
 * ReqEOC -- require end of command
 */

void ReqEOC( void )
{
    if( !ScanEOC() ) Error( ERR_LOC, LIT( ERR_WANT_EOC ) );
}


/*
 * ReqEOC -- require end of command
 */

void FlushEOC( void )
{
    while( !ScanEOC() ) Scan();
}


static bool ScanExprDelim( char *table )
{
    char   *ptr;

    for( ; *table != NULLCHAR ; table += 3 ) {
        for( ptr = ScanPtr ;  ( _IsOn( SW_CASE_SENSITIVE ) ?
                *table == *ptr  :  toupper(*table) == toupper(*ptr) )
                && *table != NULLCHAR ;  ptr++, table++ );
        if( *table == NULLCHAR ) {
            table++;
            CurrToken = *table;
            ScanPtr = ptr;
            return( TRUE );
        }
        for( ; *table != NULLCHAR ; table++ );
    }
    return( FALSE );
}


static bool ScanCmdLnDelim( void )
{
    char    *ptr;

    for( ptr = CmdLnDelimTab; ; ptr++ ) {
        if( *ScanPtr == *ptr ) break;
        if( *ptr == NULLCHAR ) return( FALSE );
    }
    CurrToken = FIRST_CMDLN_DELIM + (ptr - CmdLnDelimTab);
    if( *ScanPtr != NULLCHAR ) {
        ++ScanPtr;
    }
    return( TRUE );
}


/*
 * ScanRealNum -- try to scan a real number
 */

static bool ScanRealNum( void )
{
    char    *curr;

    curr = ScanPtr;
    while( isdigit( *curr ) ) ++curr;
    if( *curr != '.' ) return( FALSE );
    SToLD( ScanPtr, &ScanPtr, &TokenVal.real_val );
    if( curr == ScanPtr ) {    /* it isn't a number, it's just a dot */
        ScanPtr++;
        return( FALSE );
    }
    CurrToken = T_REAL_NUM;
    return( TRUE );
}


/*
 * GetDig -- get value of a digit
 */

static int GetDig( unsigned base )
{
    char     chr;

    chr = *ScanPtr;
    chr = toupper( chr );
    if( ( (chr < '0') || (chr > '9') )
     && ( (chr < 'A') || (chr > 'Z') ) ) return( -1 );
    if( chr >= 'A' ) chr -= 'A' - '0' - 10;
    if( chr - '0' >= base ) return( -1 );
    return( chr - '0' );
}



/*
 * GetNum -- evaluate a number
 */

static bool GetNum( unsigned base )
{
    bool        ok;
    unsigned_64 num;
    unsigned_64 big_base;
    unsigned_64 big_dig;
    int         dig;

    ok = FALSE;
    U32ToU64( base, &big_base );
    U64Clear( num );
    for( ; ; ) {
        dig = GetDig( base );
        if( dig < 0 ) break;
        U32ToU64( dig, &big_dig );
        U64Mul( &num, &big_base, &num );
        U64Add( &num, &big_dig, &num );
        ++ScanPtr;
        ok = TRUE;
    }
    TokenVal.int_val = num;
    return( ok );
}



/*
 * ScanNumber -- scan for a number
 */

static char ScanNumber( void )
{
    rad_str *pref;
    bool    ret;
    char    *hold_scan;

    ret = FALSE;
    hold_scan = ScanPtr;
    if( ScanCCharNum && (*ScanPtr == '\'') ) {
        if( ScanPtr[ 1 ] != NULLCHAR && ScanPtr[ 2 ] == '\'' ) {
            U32ToU64( ScanPtr[1], &TokenVal.int_val );
            ScanPtr += 3;
            CurrToken = T_INT_NUM;
            return( TRUE );
        }
    } else {
        CurrToken = T_BAD_NUM; /* assume we'll find a bad number */
        pref = RadStrs;
        while( pref != NULL ) {
            if( memicmp( ScanPtr, &pref->radstr[1], pref->radstr[0] ) == 0 ) {
                ret = TRUE;
                ScanPtr += pref->radstr[0];
                hold_scan = ScanPtr;
                if( GetNum( pref->radval ) ) {
                    CurrToken = T_INT_NUM;
                    return( TRUE );
                }
                ScanPtr -= pref->radstr[0];
            }
            pref = pref->next;
        }
        if( isdigit( *ScanPtr ) && GetNum( CurrRadix ) ) {
            CurrToken = T_INT_NUM;
            return( TRUE );
        }
    }
    ScanPtr = hold_scan;
    return( ret );
}

#define NAME_ESC        '`'

char *NamePos( void )
{
    if( *TokenStart == NAME_ESC ) return( TokenStart + 1 );
    return( TokenStart );
}

unsigned NameLen( void )
{
    char        *end;
    char        *start;

    end = ScanPtr;
    start = TokenStart;
    if( *start == NAME_ESC ) {
        ++start;
        if( end[-1] == NAME_ESC ) {
            --end;
        }
    }
    if( start >= end)
        return( 0 );
    else
        return( end - start );
}

/*
 * ScanId -- scan for an identifier
 */

static bool ScanId( void )
{
    char        c;

    c = *ScanPtr;
    if( c == NAME_ESC ) {
        for( ;; ) {
            c = *++ScanPtr;
            if( c == NULLCHAR ) break;
            if( c == NAME_ESC ) {
                ++ScanPtr;
                break;
            }
        }
    } else {
        while ( c == '_'  || c == '$' || isalnum( c ) ) {
             c = *++ScanPtr;
        }
    }
    if( NameLen() == 0 ) return( FALSE );
    CurrToken = T_NAME;
    return( TRUE );
}


static bool ScanKeyword( char *table )
{
    int   namelen;
    int   keylen;

    namelen = ScanPtr - TokenStart;
    for( ; *table != NULLCHAR ; table += (keylen + 3) ) {
         keylen = strlen( table );
         if( keylen == namelen &&  ( _IsOn( SW_CASE_SENSITIVE )  ?
                !memcmp( table, TokenStart, namelen )  :
                !memicmp( table, TokenStart, namelen ) ) ) {
             table += (namelen + 1);
             CurrToken = *table;
             return( TRUE );
         }
    }
    return( FALSE );
}


char *ReScan( char *point )
{
    char        *old;

    old = TokenStart;
    ScanPtr = point;
    if( point != NULL ) Scan();
    return( old );
}


void ScanExpr( void *tbl )
{
    ExprTokens = tbl;
    ReScan( TokenStart );
}


void AddActualChar( char data )
{
    char    *hold, *walk1, *walk2;
    unsigned len;

    len = ++StringLength;
    _Alloc( hold, len );
    if( hold == NULL ) {
        _Free( StringStart );
        Error( ERR_NONE, LIT( ERR_NO_MEMORY_FOR_EXPR ) );
    }
    walk1 = hold;
    walk2 = StringStart;
    for( --len; len > 0; --len )  *walk1++ = *walk2++;
    *walk1 = data;
    _Free( StringStart );
    StringStart = hold;
}


void AddChar( void )
{
    AddActualChar( *ScanPtr );
}


void AddCEscapeChar( void )
{
    static char escape_seq[] = "\n\t\v\b\r\f\a\\\?\'\"\0";
                                /* the order above must match with SSL file */

    AddActualChar( escape_seq[(int)CurrToken & 0x7f] );
}


static void RawScan( void )
{
    if( ScanPtr[-1] == NULLCHAR ) {
        /* missing end quote; scanned past eol -- error */
        _Free( StringStart );
        StringStart = NULL;   /* this is necessary */
        StringLength = 0;
        scan_string = FALSE;  /* this is essential */
        Error( ERR_NONE, LIT( ERR_WANT_END_STRING ) );
    }
    if( *ScanPtr != NULLCHAR ) {
        TokenStart = ScanPtr;
        CurrToken = T_UNKNOWN;
        if( ExprTokens != NULL ) {
            ScanExprDelim( ExprTokens->delims );
        }
        ScanPtr = TokenStart;
    } else {
        TokenStart = ScanPtr;   /* this is necessary */
        CurrToken = T_LINE_SEPARATOR;
    }
}


/*
 * Scan -- scan a token
 */

void Scan( void )
{
    if( !scan_string ) {
        while( isspace( *ScanPtr ) ) {
            ++ScanPtr;
        }
        TokenStart = ScanPtr;
        if( ExprTokens != NULL ) {
            if( ScanExprDelim( ExprTokens->delims ) )     return;
        }
        if( ScanCmdLnDelim() )      return;   /*sf do this if the others fail */
        if( ScanRealNum() )         return;
        if( ScanNumber() )          return;
        if( ScanId() ) {
            if( ExprTokens != NULL && CurrToken == T_NAME ) {
                ScanKeyword( ExprTokens->keywords );
            }
            return;
        }
        ++ScanPtr;
        CurrToken = T_UNKNOWN;
    } else {
        RawScan();
    }
}


void RawScanInit( void )
{
    ScanPtr = TokenStart;
    CurrToken = T_UNKNOWN;
}

char RawScanChar( void )
{
    return( *ScanPtr );
}

void RawScanAdvance( void )
{
    if( *ScanPtr != NULLCHAR ) ++ScanPtr;
}

void RawScanFini( void )
{
    TokenStart = ScanPtr;
    Scan();
}

/*
 * IntNumVal -- return a integer number's value
 */

unsigned_64 IntNumVal( void )
{
    return( TokenVal.int_val );
}


/*
 * RealNumVal -- return a real number's value
 */

xreal RealNumVal( void )
{
    return( TokenVal.real_val );
}


/*
 * NewCurrRadix - use when you know there's no scanning in progress
 */

unsigned NewCurrRadix( unsigned rad )
{
    unsigned    old;

    old = CurrRadix;
    CurrRadix = rad;
    return( old );
}


/*
 * SetCurrRadix - set the current number radix
 */

unsigned SetCurrRadix( unsigned rad )
{
    unsigned    old;

    old = NewCurrRadix( rad );
    ReScan( TokenStart );
    return( old );
}


void RestoreRadix( void )
{
    SetCurrRadix( DefRadix );
}


void DefaultRadixSet( unsigned radix )
{
    DefRadix = radix;
    CurrRadix = radix;
    DbgUpdate( UP_RADIX_CHANGE );
}

/*
 * RadixSet - set the default numeric radix
 */

void RadixSet( void )
{
    unsigned   radix;
    unsigned   old;

    old = SetCurrRadix( 10 ); /* radix sets are always base 10 */
    radix = ReqExpr();
    ReScan( TokenStart );
    ReqEOC();
    if( radix < 2 || radix > 36 ) {
        Error( ERR_NONE, LIT( ERR_BAD_RADIX ), radix );
    }
    SetCurrRadix( old );
    DefaultRadixSet( radix );
}


void RadixConf( void )
{
    *CnvULongDec( DefRadix, TxtBuff ) = NULLCHAR;
    ConfigLine( TxtBuff );
}


void FindRadixSpec( unsigned char value, char **start, unsigned *len )
{
    rad_str     *rad;

    *start = NULL;
    *len = 0;
    for( rad = RadStrs; rad != NULL; rad = rad->next ) {
        if( rad->radval == value ) {
            *start = &rad->radstr[1];
            *len   = rad->radstr[0];
            return;
        }
    }
}


char *AddHexSpec( char *p )
{
    char        *pref;
    unsigned    len;

    if( CurrRadix == 16 ) return( p );
    FindRadixSpec( 16, &pref, &len );
    memcpy( p, pref, len );
    p += len;
    *p = '\0';
    return( p );
}


/*
 * ForceSym2Num -- try to force an unknown symbol into a number
 */

bool ForceSym2Num( char *start, unsigned len, unsigned_64 *val_ptr )
{
    char        *old_scanptr;
    char        *old_tokenstart;
    token_value old_token_val;
    bool        rtn;

    old_token_val = TokenVal;
    old_scanptr = ScanPtr;
    old_tokenstart = TokenStart;
    ScanPtr = TokenStart = start;
    rtn = ( GetNum( CurrRadix ) && (ScanPtr - TokenStart) == len );
    *val_ptr = TokenVal.int_val;
    ScanPtr = old_scanptr;
    TokenStart = old_tokenstart;
    TokenVal = old_token_val;
    return( rtn );
}
