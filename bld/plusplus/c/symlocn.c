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


#include "plusplus.h"
#include "carve.h"
#include "srcfile.h"
#include "toknlocn.h"
#include "initdefs.h"
#include "pcheader.h"
#include "dbg.h"


#define BLOCK_SYMBOL_LOCN   256
static carve_t carveSYMBOL_LOCN;


static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveSYMBOL_LOCN = CarveCreate( sizeof( SYM_TOKEN_LOCN )
                                  , BLOCK_SYMBOL_LOCN );
}


static void fini(               // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveSYMBOL_LOCN );
}


INITDEFN( symbol_locations, init, fini )


SYM_TOKEN_LOCN* SymbolLocnAlloc(// ALLOCATE A SYM_TOKEN_LOCN, IF REQ'D
    SYM_TOKEN_LOCN** owner )    // - addr[ ptr to location ]
{
    SYM_TOKEN_LOCN* loc;        // - the location

    loc = *owner;
    if( NULL == loc ) {
        loc = CarveAlloc( carveSYMBOL_LOCN );
        *owner = loc;
        loc->u.dwh = 0;
    }
    return loc;
}


void SymbolLocnFree(            // FREE A SYM_TOKEN_LOCN
    SYM_TOKEN_LOCN* loc )       // - the entry
{
    CarveFree( carveSYMBOL_LOCN, loc );
}


void SymbolLocnDefine(          // DEFINE LOCATION SYMBOL
    TOKEN_LOCN *sym_locn,       // - symbol location (NULL for current source file)
    SYMBOL sym )                // - the symbol
{
    SYM_TOKEN_LOCN *locn = NULL;

    if( sym_locn != NULL && sym_locn->src_file != NULL ) {
        locn = SymbolLocnAlloc( &sym->locn );
        locn->tl = *sym_locn;
        sym->flag2 |= SF2_TOKEN_LOCN;
    } else {
        DbgVerify( sym_locn == NULL
                 , "SymbolLocnDefine -- bad location" );
        if( SrcFilesOpen() ) {
            locn = SymbolLocnAlloc( &sym->locn );
            SrcFileGetTokenLocn( &locn->tl );
            sym->flag2 |= SF2_TOKEN_LOCN;
        }
    }
}

//
// Pre-compiled header support
//

SYM_TOKEN_LOCN *SymTokenLocnGetIndex( SYM_TOKEN_LOCN *e )
/*****************************************************/
{
    return( CarveGetIndex( carveSYMBOL_LOCN, e ) );
}

SYM_TOKEN_LOCN *SymTokenLocnMapIndex( SYM_TOKEN_LOCN *e )
/*****************************************************/
{
    return( CarveMapIndex( carveSYMBOL_LOCN, e ) );
}

static void markFreeTokenLocn( void *p )
{
    SYM_TOKEN_LOCN *b = p;

    b->u.dwh = -1;
}

static void saveTokenLocn( void *e, carve_walk_base *d )
{
    SYM_TOKEN_LOCN *b = e;
    SRCFILE save_src_file;

    if( b->u.dwh == -1 ) {
        return;
    }
    DbgAssert( b->u.dwh == 0 );
    save_src_file = b->tl.src_file;
    b->tl.src_file = SrcFileGetIndex( save_src_file );
#ifndef NDEBUG
    if( b->u.dwh != 0 ) {
        CFatal( "token locn contains dwarf info!" );
    }
#endif
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *b );
    b->tl.src_file = save_src_file;
}

pch_status PCHWriteTokenLocns( void )
{
    auto carve_walk_base data;

    CarveWalkAllFree( carveSYMBOL_LOCN, markFreeTokenLocn );
    CarveWalkAll( carveSYMBOL_LOCN, saveTokenLocn, &data );
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

pch_status PCHReadTokenLocns( void )
{
    SYM_TOKEN_LOCN *b;
    auto cvinit_t data;

    // DbgVerify( sizeof( uint_32 ) >= sizeof( void* ), "Bad assumption" );
    CarveInitStart( carveSYMBOL_LOCN, &data );
    for( ; (b = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *b );
        b->tl.src_file = SrcFileMapIndex( b->tl.src_file );
    }
    return( PCHCB_OK );
}

pch_status PCHInitTokenLocns( boolean writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveSYMBOL_LOCN ) );
    } else {
        carveSYMBOL_LOCN = CarveRestart( carveSYMBOL_LOCN );
        CarveMapOptimize( carveSYMBOL_LOCN, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniTokenLocns( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveSYMBOL_LOCN );
    }
    return( PCHCB_OK );
}
