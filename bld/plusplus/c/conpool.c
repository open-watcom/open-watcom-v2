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


#include <string.h>

#include "plusplus.h"
#include "ring.h"
#include "carve.h"
#include "ptree.h"
#include "conpool.h"
#include "pcheader.h"
#include "initdefs.h"
#include "codegen.h"
#include "memmgr.h"
#include "floatsup.h"


static carve_t carvePOOL_CON;   // - carving info. -- floating point
static POOL_CON *pool_float;    // - floating pool
static POOL_CON *pool_int64;    // - int-64 pool


static POOL_CON* allocPoolCon   // ALLOCATE CONSTANT-POOL ENTRY
    ( POOL_CON** a_hdr )        // - addr[ ring hdr ]
{
    POOL_CON* pool = RingCarveAlloc( carvePOOL_CON, a_hdr );
    pool->free = FALSE;
    pool->i64  = FALSE;
    pool->flt  = FALSE;
    return pool;
}


POOL_CON *ConPoolFloatAdd       // ADD AN ITEM TO THE CONSTANTS POOL
    ( PTREE node )              // - node for floating constant
{
    POOL_CON *pool;
    unsigned len;
    char *stg;
    auto char buff[128];

    len = PTreeGetFPRaw( node, buff, sizeof( buff ) );
    stg = CPermAlloc( len );
    stg = memcpy( stg, buff, len );
    pool = allocPoolCon( &pool_float );
    pool->len = len;
    pool->fp_constant = stg;
    pool->flt  = TRUE;
    return( pool );
}


POOL_CON *ConPoolInt64Add       // ADD AN INT-64 CONSTANT
    ( signed_64 con )           // - new constant
{
    POOL_CON *pool;

    pool = allocPoolCon( &pool_int64 );
    pool->int64_constant = con;
    pool->i64  = TRUE;
    return( pool );
}


static void conPoolsInit(       // INITIALIZE CONSTANT POOLS
    INITFINI* defn )            // - definition
{
    defn = defn;
    pool_float = NULL;
    pool_int64 = NULL;
    carvePOOL_CON = CarveCreate( sizeof( POOL_CON ), 64 );
}


static void conPoolsFini(       // COMPLETE CONSTANT POOLS
    INITFINI* defn )            // - definition
{

    defn = defn;
    CarveDestroy( carvePOOL_CON );
}


INITDEFN( const_pool, conPoolsInit, conPoolsFini )

static void markFreeConstant( void *p )
{
    POOL_CON *b = p;

    b->free = TRUE;
}

static void saveConstant( void *e, carve_walk_base *d )
{
    POOL_CON *fcon = e;
    POOL_CON *save_next;

    if( fcon->free ) {
        return;
    }
    save_next = fcon->next;
    fcon->next = ConstantPoolGetIndex( save_next );
    PCHWriteCVIndex( d->index );
    PCHWrite( fcon, sizeof( *fcon ) );
    if( fcon->flt ) {
        PCHWrite( fcon->fp_constant, fcon->len );
    }
    fcon->next = save_next;
}

pch_status PCHWriteConstantPool( void )
{
    POOL_CON *tmp;
    unsigned terminator = CARVE_NULL_INDEX;
    auto carve_walk_base data;

    tmp = ConstantPoolGetIndex( pool_float );
    PCHWrite( &tmp, sizeof( tmp ) );
    tmp = ConstantPoolGetIndex( pool_int64 );
    PCHWrite( &tmp, sizeof( tmp ) );
    CarveWalkAllFree( carvePOOL_CON, markFreeConstant );
    CarveWalkAll( carvePOOL_CON, saveConstant, &data );
    PCHWriteCVIndex( terminator );
    return( PCHCB_OK );
}

pch_status PCHReadConstantPool( void )
{
    cv_index i;
    unsigned len;
    POOL_CON *c;
    auto cvinit_t data;

    pool_float = ConstantPoolMapIndex( PCHReadPtr() );
    pool_int64 = ConstantPoolMapIndex( PCHReadPtr() );
    CarveInitStart( carvePOOL_CON, &data );
    for(;;) {
        i = PCHReadCVIndex();
        if( i == CARVE_NULL_INDEX ) break;
        c = CarveInitElement( &data, i );
        PCHRead( c, sizeof( *c ) );
        c->next = ConstantPoolMapIndex( c->next );
        if( c->flt ) {
            len = c->len;
            c->fp_constant = CPermAlloc( len );
            PCHRead( c->fp_constant, len );
        }
    }
    return( PCHCB_OK );
}

pch_status PCHInitConstantPool( boolean writing )
{
    cv_index n;

    if( writing ) {
        n = CarveLastValidIndex( carvePOOL_CON );
        PCHWriteCVIndex( n );
    } else {
        carvePOOL_CON = CarveRestart( carvePOOL_CON );
        n = PCHReadCVIndex();
        CarveMapOptimize( carvePOOL_CON, n );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniConstantPool( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carvePOOL_CON );
    }
    return( PCHCB_OK );
}

POOL_CON *ConstantPoolGetIndex( POOL_CON *e )
{
    return( CarveGetIndex( carvePOOL_CON, e ) );
}

POOL_CON *ConstantPoolMapIndex( POOL_CON *e )
{
    return( CarveMapIndex( carvePOOL_CON, e ) );
}
