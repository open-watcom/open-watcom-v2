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


#include <limits.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgtback.h"
#include "dbgmem.h"
#include "dbgchain.h"
#include "dui.h"
#include "mad.h"


extern address          FindLclBlock( address addr );
extern int              AddrComp(address ,address );
extern void             GoToAddr( address addr );
extern bool             DlgBreak(address);
extern char             *StrCopy(char*,char*);
extern char             *CopySourceLine( cue_handle * );
extern unsigned         LineNumLkup(address);
extern void             UnAsm( address addr, char *buff, unsigned buff_len );
extern char             *DupStr(char*);
extern void             SetStackPos( location_context *lc, int pos );
extern int              GetStackPos();
extern char             *CnvNearestAddr( address, char *, unsigned );
extern char             *Format(char *,char *,... );

#define MODEST_CALL_LEVEL       20
#define MODEST_INCREMENT        10

address FindNextIns( address a )
{
    mad_disasm_data     *dd;

    _AllocA( dd, MADDisasmDataSize() );
    MADDisasm( dd, &a, 0 );
    return( a );
}


static void FreeChainInfo( traceback *curr, int start, int end )
{
    int         i;

    for( i = start; i < end; ++i ) {
        DbgFree( curr->chain[ i ].source_line );
        DbgFree( curr->chain[ i ].symbol );
        curr->chain[ i ].source_line = NULL;
        curr->chain[ i ].symbol = NULL;
    }
}


static bool ReAllocChain( traceback *curr, int new_size )
{
    call_chain  *new_chain;
    call_chain  *chain;

    chain = curr->chain;
    if( new_size >= (UINT_MAX / sizeof(call_chain)) ) return( FALSE );
    new_chain = DbgAlloc( new_size * sizeof(call_chain) );
    if( new_chain == NULL ) return( FALSE );
    memset( new_chain, 0, new_size*sizeof( call_chain ) );
    memcpy( new_chain, chain, curr->current_depth * sizeof(call_chain) );
    DbgFree( chain );
    curr->chain = new_chain;
    curr->allocated_size = new_size;
    return( TRUE );
}

static bool EarlyOut( cached_traceback *tb, address execution, address frame )
{
    call_chain  *chain;
    int         new_size;
    int         i;
    traceback   *curr;
    traceback   *prev;

    curr = tb->curr;
    prev = tb->prev;
    for( i = 0; i < prev->total_depth; ++i ) {
        chain = &prev->chain[ i ];
        if( AddrComp( chain->lc.execution, execution ) != 0 ) continue;
        if( AddrComp( chain->lc.frame, frame ) != 0 ) continue;
        new_size = curr->current_depth + prev->total_depth - i;
        if( new_size > curr->allocated_size ) {
            if( !ReAllocChain( curr, new_size ) ) return( FALSE );
        }
        curr->clean_size = prev->total_depth - i;
        memcpy( &curr->chain[ curr->current_depth ], chain,
                curr->clean_size * sizeof( call_chain ) );
        curr->current_depth = new_size;
        while( i < prev->total_depth ) {
            chain->source_line = NULL; // since we copied the ptr
            chain->symbol = NULL; // since we copied the ptr
            ++chain;
            ++i;
        }
        return( TRUE );
    }
    return( FALSE );
}


static CALL_CHAIN_RTN RecordTraceBackInfo;
static bool RecordTraceBackInfo( call_chain_entry *entry, void *_tb )
{
    address     prev_ins;
    address     execution;
    call_chain  *chain;
    traceback   *curr;
    mad_disasm_data     *dd;
    DIPHDL( cue, ch );
    cached_traceback *tb = _tb;

    execution = entry->lc.execution;
    curr = tb->curr;
    if( curr->current_depth >= curr->allocated_size ) {
        if( !ReAllocChain( curr, curr->allocated_size + MODEST_INCREMENT ) ) {
            return( FALSE );
        }
    }
    if( curr->current_depth != 0 ) {
        _AllocA( dd, MADDisasmDataSize() );
        prev_ins = execution;
        if( MADDisasm( dd, &prev_ins, -1 ) == MS_OK ) {
            execution.mach.offset -= MADDisasmInsSize( dd );
        }
    }
    if( EarlyOut( tb, execution, entry->lc.frame ) ) return( FALSE );
    chain = &curr->chain[ curr->current_depth ];
    chain->lc = entry->lc;
    chain->lc.execution = execution;
    chain->open = FALSE;
    chain->source_line = NULL;
    chain->symbol = NULL;
    if( DeAliasAddrCue( NO_MOD, execution, ch ) != SR_NONE ) {
        chain->source_line = CopySourceLine( ch );
    }
    CnvNearestAddr( chain->lc.execution, TxtBuff, TXT_LEN );
    DbgFree( chain->symbol );
    chain->symbol = DupStr( TxtBuff );
    chain->sym_len = 0;
    curr->current_depth++;
    return( TRUE );
}

static traceback *DoInitTraceBack( traceback *curr )
{
    curr->chain = DbgMustAlloc( MODEST_CALL_LEVEL * sizeof( call_chain ) );
    memset( curr->chain, 0, MODEST_CALL_LEVEL * sizeof( call_chain ) );
    curr->allocated_size = MODEST_CALL_LEVEL;
    curr->total_depth = 0;
    return( curr );
}

void InitTraceBack( cached_traceback *tb )
{
    tb->curr = DoInitTraceBack( &tb->a );
    tb->prev = DoInitTraceBack( &tb->b );
}

void FiniTraceBack( cached_traceback *tb )
{
    FreeChainInfo( tb->curr, 0, tb->curr->total_depth );
    FreeChainInfo( tb->prev, 0, tb->prev->total_depth );
    DbgFree( tb->curr->chain );
    DbgFree( tb->prev->chain );
}

void UpdateTraceBack( cached_traceback *tb )
{
    traceback           *curr;
    traceback           *prev;

    curr = tb->prev;
    prev = tb->curr;
    tb->curr = curr;
    tb->prev = prev;
    curr->current_depth = 0;
    if( UpdateFlags & UP_SYM_CHANGE ) {
        FreeChainInfo( prev, 0, prev->total_depth );
        prev->total_depth = 0;
    }
    curr->clean_size = 0;
    WalkCallChain( RecordTraceBackInfo, tb );
    FreeChainInfo( curr, curr->current_depth, curr->total_depth );
    FreeChainInfo( prev, 0, prev->total_depth );
    curr->total_depth = curr->current_depth;
}

call_chain *GetCallChain( cached_traceback *tb, int row )
{
    traceback   *curr;

    curr = tb->curr;
    if( row >= 0 && row < curr->total_depth ) {
        return( &curr->chain[ curr->total_depth - 1 - row ] );
    } else {
        return( NULL );
    }
}

void ShowCalls()
{
    cached_traceback    tb;
    int                 i;
    char                buff[TXT_LEN];
    char                *source;
    call_chain          *chain;

    InitTraceBack( &tb );
    UpdateTraceBack( &tb );
    i = 0;
    for( ;; ) {
        chain = GetCallChain( &tb, i++ );
        if( chain == NULL ) break;
        if( chain->source_line != NULL ) {
            source = chain->source_line;
        } else {
            UnAsm( chain->lc.execution, buff, TXT_LEN );
            source = buff;
        }
        Format( TxtBuff, "%s: %s", chain->symbol, source );
        DUIDlgTxt( TxtBuff );
    }
    FiniTraceBack( &tb );
}

void UnWindToFrame( call_chain *chain, int row, int rows )
/********************************************************/
{
    address     ip;

    ip = chain->lc.execution;
    if( row != 0 ) {
        chain->lc.execution = FindNextIns( ip );
    }
    SetStackPos( &chain->lc, -( rows - 1 - row ) );
    chain->lc.execution = ip;
}
