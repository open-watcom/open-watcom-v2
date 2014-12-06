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


#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgstk.h"
#include "dbgitem.h"
#include "dbgio.h"
#include "spawn.h"
#include "mad.h"

extern void             PopEntry(void);
extern void             ExprValue(stack_entry *);
extern bool             IsInternalMod( mod_handle mod );
extern void             AddrFloat( address * );
extern void             MakeMemoryAddr( bool, memory_expr def_seg, address *out_val );
extern char             *StrCopy( char *src, char*dest );
extern char             *ReScan(char *);
extern void             EvalLValExpr( int );
extern void             ReqEOC(void);
extern void             NormalExpr(void);
extern void             BreakOnExprSP(char*);
extern void             LValue(stack_entry *);

extern stack_entry      *ExprSP;

extern void BreakOnSelected( char *item )
{
    char        *old;

    old = ReScan( item );
    EvalLValExpr( 1 );
    ReScan( old );
    BreakOnExprSP( item );
}


bool ScanSelectedExpr( char *expr )
{
    char        *old;
    bool        rc;

    old = ReScan( expr );
    _SwitchOn( SW_ERR_IN_TXTBUFF );
    _SwitchOn( SW_NO_DISAMBIGUATOR );
    rc = ( Spawn( NormalExpr ) == 0 );
    _SwitchOff( SW_NO_DISAMBIGUATOR );
    _SwitchOff( SW_ERR_IN_TXTBUFF );
    ReScan( old );
    return( rc );
}

inspect_type WndGetExprSPInspectType( address *paddr )
{
    sym_info    info;
    DIPHDL( sym, sh );

    LValue( ExprSP );
    *paddr = NilAddr;
    if( ExprSP->info.kind == TK_FUNCTION ) {
        ExprValue( ExprSP );
        *paddr = ExprSP->v.addr;
        PopEntry();
        return( INSP_CODE );
    } else if( (ExprSP->flags & SF_LOCATION) && ExprSP->th != NULL && !IsInternalMod( TypeMod( ExprSP->th ) ) ) {
        if( ExprSP->v.loc.num == 1 && ExprSP->v.loc.e[0].type == LT_ADDR ) {
            *paddr = ExprSP->v.loc.e[0].u.addr;
        }
        PopEntry();
        return( INSP_DATA );
    } else {
        if( (ExprSP->flags & SF_LOCATION) && ExprSP->v.loc.e[0].type == LT_ADDR ) {
            *paddr = ExprSP->v.loc.e[0].u.addr;
        } else if( ExprSP->info.kind == TK_ADDRESS
                || ExprSP->info.kind == TK_POINTER ) {
            *paddr = ExprSP->v.addr;
        }
        if( !IS_NIL_ADDR( (*paddr) ) ) {
            AddrFloat( paddr );
            if( DeAliasAddrSym( NO_MOD, *paddr, sh ) != SR_NONE ) {
                SymInfo( sh, ExprSP->lc, &info );
                PopEntry();
                switch( info.kind ) {
                case SK_CODE:
                case SK_PROCEDURE:
                    return( INSP_CODE );
                    break;
                default:
                    return( INSP_RAW_DATA );
                    break;
                }
            }
        }
    }
    ExprValue( ExprSP );
    MakeMemoryAddr( TRUE, EXPR_DATA, paddr );
    return( INSP_RAW_DATA );
}

static void DoLValExpr( void )
/****************************/
{
    EvalLValExpr( 1 );
    ReqEOC();
}

extern  bool    WndEvalInspectExpr( char *item, bool pop )
/********************************************************/
{
    char        *old;
    char        buff[12],*p;
    bool        rc;

    if( ispunct(item[0]) &&
      ( item[1] == '\0' || ( ispunct( item[1] ) && item[2] == '\0' ) ) ) {
        // nyi - pui - use SSL
        p = StrCopy( item, StrCopy( "operator", buff ) );
        if( item[0] == '[' && item[1] == '\0' ) {
            StrCopy( "]", p );
        } else if( item[0] == '(' && item[1] == '\0' ) {
            StrCopy( ")", p );
        }
        old = ReScan( buff );
    } else {
        old = ReScan( item );
    }
    _SwitchOn( SW_CALL_FATAL );
    _SwitchOn( SW_ERR_IN_TXTBUFF );
    _SwitchOn( SW_NO_DISAMBIGUATOR );
    rc = ( Spawn( DoLValExpr ) == 0 );
    _SwitchOff( SW_CALL_FATAL );
    _SwitchOff( SW_NO_DISAMBIGUATOR );
    _SwitchOff( SW_ERR_IN_TXTBUFF );
    ReScan( old );
    if( pop && rc ) PopEntry();
    return( rc );
}

