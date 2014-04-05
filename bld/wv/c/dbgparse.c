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
* Description:  Expression parser (using .prs grammar files).
*
****************************************************************************/


#include <string.h>
#include "dbgdefn.h"
#include "dbginfo.h"
#include "dbgstk.h"
#include "dbgtoken.h"
#include "dbgreg.h"
#include "dbgerr.h"
#include "dbgio.h"
#include "dbgmem.h"
#include "dbgtoggl.h"
#include "watcom.h"
#include "i64.h"


extern void             PushInt( int );
extern void             SwapStack( int );
extern void             MakeAddr( void );
extern void             ConvertTo( stack_entry *, type_kind, type_modifier, unsigned );
extern void             PopEntry( void );
extern void             ExprValue( stack_entry * );
extern void             DefAddr( memory_expr, address * );
extern void             ExprResolve( stack_entry * );
extern void             AddrFix( address * );
extern void             AddrFloat( address * );

extern void             ScanExpr( void * );
extern bool             ScanEOC( void );
extern handle           LocalPathOpen( char *, unsigned, char * );
extern int              SSLWalk( char *, unsigned, void **, unsigned int );
extern unsigned         SetCurrRadix( unsigned );

extern tokens           CurrToken;

extern unsigned         SkipCount;
extern unsigned         ExprAddrDepth;

extern stack_entry      *ExprSP;


static char         *ParseTable;
static unsigned     ParseTableSize;
static token_table  ParseTokens;

#define PARSE_STACK_SIZE 128


static void start_expr( void )
{
    void        *stack[ PARSE_STACK_SIZE ];

    ScanExpr( &ParseTokens );
    SSLWalk( ParseTable, 0, stack, PARSE_STACK_SIZE );
    ExprResolve( ExprSP );
    ScanExpr( NULL );
}


void SetUpExpr( unsigned addr_depth )
{
    SkipCount = 0;
    ExprAddrDepth = addr_depth;
}


void EvalLValExpr( unsigned addr_depth )
{
    SetUpExpr( addr_depth );
    start_expr();
}


void EvalExpr( unsigned addr_depth )
{
    EvalLValExpr( addr_depth );
    ExprValue( ExprSP );
}

void NormalExpr( void )
{
    EvalExpr( 1 );
}



/*
 * ChkExpr -- check out expression syntax
 */

void ChkExpr( void )
{
    SkipCount = 1;
    ExprAddrDepth = 1;
    start_expr();
}
/*
 * ReqExpr -- get a required expression
 */

unsigned_64 ReqU64Expr( void )
{
    unsigned_64 rtn;

    NormalExpr();
    ConvertTo( ExprSP, TK_INTEGER, TM_UNSIGNED, sizeof( ExprSP->v.uint ) );
    rtn = ExprSP->v.uint;
    PopEntry();
    return( rtn );
}

long ReqLongExpr( void )
{
    unsigned_64 tmp;

    tmp = ReqU64Expr();
    return( U32FetchTrunc( tmp ) );
}

unsigned ReqExpr( void )
{
    unsigned_64 tmp;

    tmp = ReqU64Expr();
    return( U32FetchTrunc( tmp ) );
}


/*
 * ReqXRealExpr -- get a required floating point expression
 */

#ifdef DEADCODE
xreal ReqXRealExpr( void )
{
    xreal v;

    NormalExpr();
    ConvertTo( ExprSP, TK_REAL, TM_NONE, sizeof( ExprSP->v.real ) );
    v = ExprSP->v.real;
    PopEntry();
    return( v );
}
#endif


/*
 * OptExpr -- get an optional expression
 */

unsigned OptExpr( unsigned def_val )
{
    if( CurrToken == T_COMMA
     || CurrToken == T_LEFT_BRACE
     || ScanEOC() ) return( def_val );
    return( ReqExpr() );
}

void MakeMemoryAddr( bool pops, memory_expr def_seg, address *val )
{
    if( ExprSP->flags & SF_LOCATION ) {
        ExprSP->flags &= ~(SF_LOCATION|SF_IMP_ADDR);
        ExprSP->v.addr = ExprSP->v.loc.e[0].u.addr;
        ExprSP->info.kind = TK_ADDRESS;
        ExprSP->info.modifier = TM_FAR;
    }
    switch( ExprSP->info.kind ) {
    case TK_ADDRESS:
    case TK_POINTER:
        if( (ExprSP->info.modifier & TM_MOD_MASK) != TM_NEAR ) break;
        /* fall through */
    default:
        DefAddr( def_seg, val );
        AddrFix( val );
        //NYI: lost address abstraction
        PushInt( val->mach.segment );
        SwapStack( 1 );
        MakeAddr();
    }
    *val = ExprSP->v.addr;
    AddrFloat( val );
    if( pops ) PopEntry();
}


/*
 * ReqMemAddr -- get a required memory address
 */

void ReqMemAddr( memory_expr def_seg, address *out_val )
{
    unsigned    old;

    old = SetCurrRadix( 16 );
    _SwitchOff( SW_EXPR_IS_CALL );
    EvalExpr( 0 );   /* memory expression */
    MakeMemoryAddr( TRUE, def_seg, out_val );
    SetCurrRadix( old );
}


/*
 * CallExpr -- get a call expression
 */
void CallExpr( address *out_val )
{
    unsigned    old;

    old = SetCurrRadix( 16 );
    _SwitchOn( SW_EXPR_IS_CALL );
    EvalExpr( 0 ); /* call expression */
    MakeMemoryAddr( TRUE, EXPR_CODE, out_val );
    SetCurrRadix( old );
}


/*
 * OptMemAddr -- get an optional memory address
 */

void OptMemAddr( memory_expr def_seg, address *def_val )
{
    if( CurrToken == T_COMMA || ScanEOC() ) return;
    ReqMemAddr( def_seg, def_val );
}


void SetTokens( bool parse_tokens )
{
    if( parse_tokens ) ScanExpr( &ParseTokens );
    else               ScanExpr( NULL );
}



#define PARSE_TABLE_INIT (1024*4)

void LangInit( void )
{
    _Alloc( ParseTable, PARSE_TABLE_INIT );
    ParseTableSize = PARSE_TABLE_INIT;
}

void LangFini( void )
{
    _Free( ParseTable );
    ParseTable = NULL;
    ParseTableSize = 0;
}

static unsigned ReadSection( handle filehndl, unsigned off )
{
    unsigned_16 len;
    unsigned    last;
    void        *new;

    if( ReadStream( filehndl, &len, sizeof( len ) ) != sizeof( len ) ) {
        return( 0 );
    }
    CONV_LE_16( len );
    last = off + len;
    if( last > ParseTableSize ) {
        new = ParseTable;
        _Realloc( new, last );
        if( new == NULL ) return( 0 );
        ParseTable = new;
        ParseTableSize = last;
    }
    if( ReadStream( filehndl, &ParseTable[off], len ) != len ) {
        return( 0 );
    }
    return( off + len );
}

static bool ReadAllSections( handle filehndl )
{
    unsigned    key_off;
    unsigned    delim_off;

    /* read rules */
    key_off = ReadSection( filehndl, 0 );
    if( key_off == 0 ) return( FALSE );
    /* read keywords */
    delim_off = ReadSection( filehndl, key_off );
    if( delim_off == 0 ) return( FALSE );
    /* read delimiters */
    if( ReadSection( filehndl, delim_off ) == 0 ) return( FALSE );
    ParseTokens.keywords = &ParseTable[ key_off ];
    ParseTokens.delims = &ParseTable[ delim_off ];
    return( TRUE );
}

bool LangLoad( char *lang, int langlen )
{
    handle      filehndl;
    bool        ret;

    filehndl = LocalPathOpen( lang, langlen, "prs" );
    if( filehndl == NIL_HANDLE ) return( FALSE );
    ret = ReadAllSections( filehndl );
    FileClose( filehndl );
    return( ret );
}
