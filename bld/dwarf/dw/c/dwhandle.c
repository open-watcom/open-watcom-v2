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
* Description:  Manage handles.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>

#include "dwpriv.h"
#include "dwmem.h"
#include "dwcarve.h"
#include "dwutils.h"
#include "dwhandle.h"

/* first handle we ever return */
#define FIRST_HANDLE    (DW_FT_MAX+1)

/* number of elts in a block; should be a power of 2 */
#define BLOCK_SIZE      1024

typedef uint_16 dw_index_t;
typedef struct handle_blk handle_blk;
struct handle_blk {
    handle_common       data[ BLOCK_SIZE ];
    dw_index_t          index;  /* data[0] is the (index*BLOCK_SIZE)th elt */
    uint_16             height; /* height of this node */
    handle_blk *        next[ 1 ];
};

#ifndef NDEBUG
#include <stdio.h>

static struct {
    unsigned    dump_handle : 1;
} dbgFlag;

static void initDbgHandle( void ) {
    if( getenv( "DW_DUMPHANDLE" ) ) {
        dbgFlag.dump_handle = 1;
    }
}

static void dbgAllocHandle( dw_handle h ) {
    if( dbgFlag.dump_handle ) {
        printf( __FILE__ ": %09u+\n", h );
        fflush( stdout );
    }
}

static void dbgDefineHandle( dw_handle h ) {
    if( dbgFlag.dump_handle ) {
        printf( __FILE__ ": %09u-\n", h );
        fflush( stdout );
    }
}
#else
#define initDbgHandle()
#define dbgAllocHandle( h )
#define dbgDefineHandle( h )
#endif

void InitHandles(
    dw_client                   cli )
{
    int i;

    cli->handles.num_handles = 0;
    cli->handles.forward = 0;
    cli->handles.max_height = 0;
    for( i = 0; i < MAX_HANDLE_HEIGHT; ++i ) {
        cli->handles.block_head[ i ] = 0;
        cli->handles.block_tail[ i ] = &cli->handles.block_head[ i ];
    }
    cli->handles.extra_carver = CarveCreate( cli, sizeof( handle_extra ), 32 );
    cli->handles.chain_carver = CarveCreate( cli, sizeof( reloc_chain ), 16 );
    initDbgHandle();
}


void FiniHandles(
    dw_client                   cli )
{
    handle_blk *                cur;
    handle_blk *                next;

    _Assert(  cli->handles.forward == 0 );
    cur = cli->handles.block_head[ 0 ];
    while( cur ) {
        next = cur->next[ 0 ];
        CLIFree( cur );
        cur = next;
    }
    CarveDestroy( cli, cli->handles.extra_carver );
    CarveDestroy( cli, cli->handles.chain_carver );
}


static handle_blk *newBlock(
    dw_client                   cli )
{
    handle_blk *                new;
    uint                        height;
    int                         i;

    /*
        FIXME: we use the power of two distribution... should we use a
        different distribution?
    */
    height = 1; /* calculate the height of this node */
    while( rand() > (RAND_MAX / 2) && height < MAX_HANDLE_HEIGHT ) ++height;
    if( height > cli->handles.max_height ) cli->handles.max_height = height;

    /* allocate the new node */
    new = CLIAlloc( sizeof( handle_blk ) + (height-1) * sizeof(handle_blk *) );
    memset( new, 0, sizeof( handle_blk ) );
    new->height = height;
    new->index = cli->handles.num_handles / BLOCK_SIZE;

    /* link the node into all the lists */
    for( i = 0; i < height; ++i ) {
        new->next[ i ] = NULL;
        *cli->handles.block_tail[ i ] = new;
        cli->handles.block_tail[ i ] = &new->next[ i ];
    }
    return( new );
}


dw_handle NewHandle(
    dw_client                   cli )
{
    uint_32                     elm_num;
    dw_handle                   h;

    elm_num = cli->handles.num_handles;
    if( elm_num % BLOCK_SIZE == 0 ) { /* time to allocate a new block */
        if( elm_num >= BLOCK_SIZE * ( 1ul << 8*sizeof( dw_index_t ) ) ) {
            _Abort( ABORT_TOO_MANY_HANDLES );
        }
        newBlock( cli );
    }
    ++cli->handles.num_handles;
    ++cli->handles.forward;
    h = elm_num + FIRST_HANDLE;
    dbgAllocHandle( h );
    return( h );
}


static handle_blk *getIndex(
    dw_client                   cli,
    dw_index_t                  index )
{
    int                         i;
    handle_blk *                blk;
    handle_blk **               left_edge;

    _Assert( index * BLOCK_SIZE <= cli->handles.num_handles );
    _Assert( cli->handles.max_height > 0 );
    /*
        Since the index is already in the list, this loop will always
        execute the return statement.
    */
    left_edge = &cli->handles.block_head[0];
    i = cli->handles.max_height - 1;
    for(;;) {
        for(;;) {
            blk = left_edge[ i ];
            if( blk->index == index ) return( blk );
            if( blk->index > index ) break;
            if( blk->next[ i ] == NULL ) break;
            left_edge = &blk->next[ 0 ];
        }
        --i;
    }
}


handle_common *GetCommon(
    dw_client                   cli,
    dw_handle                   hdl )
{
    uint_32                     elm_num;

    elm_num = ( hdl & HANDLE_MASK ) - FIRST_HANDLE;
    _Assert( elm_num < cli->handles.num_handles );
    return( getIndex( cli, elm_num/BLOCK_SIZE )->data + elm_num%BLOCK_SIZE );
}


handle_extra *CreateExtra(
    dw_client                   cli,
    dw_handle                   hdl )
{
    handle_extra *              new;

    new = CarveAlloc( cli, cli->handles.extra_carver );
    new->base.handle = hdl;
    new->base.next = cli->handles.extra_list;
    cli->handles.extra_list = new;
    return( new );
}


void DestroyExtra(
    dw_client                   cli,
    dw_handle                   hdl )
{
    handle_extra **             walk;
    handle_extra *              delete;

    walk = &cli->handles.extra_list;
    while( *walk ) {
        if( (*walk)->base.handle == hdl ) {
            delete = *walk;
            *walk = delete->base.next;
            CarveFree( cli->handles.extra_carver, delete );
            return;
        }
        walk = &(*walk)->base.next;
    }
}


handle_extra *GetExtra(
    dw_client                   cli,
    dw_handle                   hdl )
{
    handle_extra *              walk;

    walk = cli->handles.extra_list;
    while( walk ) {
        if( walk->base.handle == hdl ) break;
        walk = walk->base.next;
    }
    return( walk );
}


void HandleReference(
    dw_client                   cli,
    dw_handle                   hdl,
    uint                        section )
{
    if( ( cli->compiler_options & DW_CM_BROWSER) && section == DW_DEBUG_INFO ) {
        DWReference( cli, cli->decl.line, cli->decl.column, hdl );
    }
    HandleWriteOffset( cli, hdl, section );
}

void HandleWriteOffset(
    dw_client                   cli,
    dw_handle                   hdl,
    uint                        section )
{
// always do a write so I know if the
// handle got updated
    handle_common *             c;
    reloc_chain *               chain;
    debug_ref                   offset;

    c = GetCommon( cli, hdl );
    if( c->reloc.offset & RELOC_OFFSET ) {
        offset = c->reloc.offset >> 1;
    } else {
        offset = 0;
        chain = CarveAlloc( cli, cli->handles.chain_carver );
        chain->section = section;
        chain->offset = CLITell( section );
        chain->next = c->reloc.chain;
        c->reloc.chain = chain;
     // CLISeek( section, sizeof( debug_ref ), DW_SEEK_CUR );
    }
    CLIWrite( section, &offset, sizeof( debug_ref ) );
}


void SetHandleLocation(
    dw_client                   cli,
    dw_handle                   hdl )
{
    handle_common *             c;
    reloc_chain *               cur;
    char                        used[ DW_DEBUG_MAX ];
    uint                        u;
    debug_ref                   offset;

    c = GetCommon( cli, hdl );
    cur = c->reloc.chain;
    dbgDefineHandle( hdl );
    --cli->handles.forward;
    offset = CLITell( DW_DEBUG_INFO ) - cli->section_base[ DW_DEBUG_INFO ];
    WriteRef( &offset, offset );
    c->reloc.offset = RELOC_OFFSET | ( offset << RELOC_OFFSET_SHIFT );
    memset( used, 0, sizeof( used ) );
    while( cur != NULL ) {
        used[ cur->section ] = 1;
        CLISeek( cur->section, cur->offset, DW_SEEK_SET );
        CLIWrite( cur->section, &offset, sizeof( debug_ref ) );
        cur = CarveFreeLink( cli->handles.chain_carver, cur );
    }
    for( u = 0; u < DW_DEBUG_MAX; ++u ) {
        if( used[ u ] ) CLISeek( u, 0, DW_SEEK_END );
    }
}

uint_32 DWDebugRefOffset(
    dw_client                   cli,
    dw_handle                   hdl )
{
    handle_common *             c;
    debug_ref                   offset;

    c = GetCommon( cli, hdl );
    if( c->reloc.offset & RELOC_OFFSET ) {
        offset = c->reloc.offset >> 1;
    }else{
        offset = 0;
    }
    return( offset );
}

dw_handle LabelNewHandle(
    dw_client                   cli )
{
    dw_handle                   new;
    handle_common *             c;
    debug_ref                   offset;

    new = NewHandle( cli );
    c = GetCommon( cli, new );
    offset = CLITell( DW_DEBUG_INFO ) - cli->section_base[ DW_DEBUG_INFO ];
    WriteRef( &offset, offset );
    c->reloc.offset = RELOC_OFFSET | ( offset << RELOC_OFFSET_SHIFT );
    dbgDefineHandle( new );
    --cli->handles.forward;
    return( new );
}

dw_handle GetHandle( dw_client cli ){
    dw_handle   new_hdl;

    if( cli->defset == 0 ){
        new_hdl = LabelNewHandle( cli );
    }else{
        new_hdl = cli->defset;
        cli->defset = 0;
        SetHandleLocation( cli, new_hdl );
    }
    return( new_hdl );
}

